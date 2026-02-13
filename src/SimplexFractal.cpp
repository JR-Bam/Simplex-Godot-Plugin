#include "Simplex.hpp"

using namespace godot;

void Simplex::set_lacunarity(float lacunarity)
{
    this->noise->mLacunarity = lacunarity;
    _update_preview();
    emit_changed();
}

float Simplex::get_lacunarity()
{
    return this->noise->mLacunarity;
}

void Simplex::set_gain(float gain)
{
    this->noise->mPersistence = gain;
    _update_preview();
    emit_changed();
}

float Simplex::get_gain()
{
    return this->noise->mPersistence;
}

void Simplex::set_ping_pong_strength(float ping_pong_strength)
{
    this->noise->mPingPongStrength = ping_pong_strength;
    _update_preview();
    emit_changed();
}

float Simplex::get_ping_pong_strength()
{
    return this->noise->mPingPongStrength;
}

void Simplex::set_octaves(uint16_t octaves)
{
    this->noise->mOctaves = octaves;
    _update_preview();
    emit_changed();
}

uint16_t Simplex::get_octaves()
{
    return this->noise->mOctaves;
}

void Simplex::set_fractal_type(FractalType fractal_type)
{
    if (this->type != fractal_type) {
        this->type = fractal_type;
        _update_preview();
        notify_property_list_changed();
        emit_changed();
    }
    
}

Simplex::FractalType Simplex::get_fractal_type()
{
    return this->type;
}