#include "Simplex.hpp"

using namespace godot;

void Simplex::set_domain_warp_enabled(bool enabled)
{
    if (this->domain_warp_enabled != enabled) {
        this->domain_warp_enabled = enabled;
        _update_preview();
        notify_property_list_changed();
        emit_changed();
    }
}

bool Simplex::get_domain_warp_enabled()
{
    return this->domain_warp_enabled;
}

void Simplex::set_domain_warp_type(DomainWarpType type)
{
    this->domain_warp_type = type;
    _update_preview();
    emit_changed();
}

Simplex::DomainWarpType Simplex::get_domain_warp_type()
{
    return this->domain_warp_type;
}

void Simplex::set_domain_warp_amplitude(float amplitude)
{
    this->noise->mDomainWarpAmplitude = amplitude;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_amplitude()
{
    return this->noise->mDomainWarpAmplitude;
}

void Simplex::set_domain_warp_frequency(float frequency)
{
    float freq = CLAMP(frequency, 0.0f, 1.0f);
    this->noise->mDomainWarpFrequency = freq;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_frequency()
{
    return this->noise->mDomainWarpFrequency;
}

void Simplex::set_domain_warp_fractal_type(DomainWarpFractalType fractal_type)
{
    if (this->domain_warp_fractal_type != fractal_type) {
        this->domain_warp_fractal_type = fractal_type;
        _update_preview();
        notify_property_list_changed();
        emit_changed();
    }
}

Simplex::DomainWarpFractalType Simplex::get_domain_warp_fractal_type()
{
    return this->domain_warp_fractal_type;
}

void Simplex::set_domain_warp_octaves(uint16_t octaves)
{
    this->noise->mDomainWarpFractalOctaves = octaves;
    _update_preview();
    emit_changed();
}

uint16_t Simplex::get_domain_warp_octaves()
{
    return this->noise->mDomainWarpFractalOctaves;
}

void Simplex::set_domain_warp_lacunarity(float lacunarity)
{
    this->noise->mDomainWarpFractalLacunarity = lacunarity;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_lacunarity()
{
    return this->noise->mDomainWarpFractalLacunarity;
}

void Simplex::set_domain_warp_gain(float gain)
{
    this->noise->mDomainWarpFractalGain = gain;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_gain()
{
    return this->noise->mDomainWarpFractalGain;
}

void Simplex::_apply_domain_warp_2d(float &x, float &y) const
{
    switch (domain_warp_fractal_type)
    {
    case DOMAIN_WARP_FRACTAL_INDEPENDENT:
        this->noise->independent_domain_warp_fractal(x, y);
        break;
    case DOMAIN_WARP_FRACTAL_PROGRESSIVE:
        this->noise->progressive_domain_warp_fractal(x, y);
        break;
    default: // DOMAIN_WARP_FRACTAL_NONE
        this->noise->single_domain_warp_gradient(
            this->noise->mDomainWarpAmplitude, x, y, x, y);
        break;
    }
}

void Simplex::_apply_domain_warp_3d(float &x, float &y, float &z) const
{
    switch (domain_warp_fractal_type)
    {
    case DOMAIN_WARP_FRACTAL_INDEPENDENT:
        this->noise->independent_domain_warp_fractal(x, y, z);
        break;
    case DOMAIN_WARP_FRACTAL_PROGRESSIVE:
        this->noise->progressive_domain_warp_fractal(x, y, z);
        break;
    default: // DOMAIN_WARP_FRACTAL_NONE
        this->noise->single_domain_warp_gradient(
            this->noise->mDomainWarpAmplitude, x, y, z, x, y, z);
        break;
    }
}
