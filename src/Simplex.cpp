#include "Simplex.hpp"
#include <godot_cpp/core/class_db.hpp>
using namespace godot;

void Simplex::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_noise_1d", "x"), &Simplex::get_noise_1d);
    ClassDB::bind_method(D_METHOD("get_noise_2d", "x", "y"), &Simplex::get_noise_2d);
    ClassDB::bind_method(D_METHOD("get_noise_3d", "x", "y", "z"), &Simplex::get_noise_3d);
    ClassDB::bind_method(D_METHOD("get_noise_2dv", "v"), &Simplex::get_noise_2dv);
    ClassDB::bind_method(D_METHOD("get_noise_3dv", "v"), &Simplex::get_noise_3dv);

    // Bind setter and getter
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &Simplex::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &Simplex::get_seed);
    ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &Simplex::set_frequency);
    ClassDB::bind_method(D_METHOD("get_frequency"), &Simplex::get_frequency);
    ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &Simplex::set_octaves);
    ClassDB::bind_method(D_METHOD("get_octaves"), &Simplex::get_octaves);
    ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &Simplex::set_lacunarity);
    ClassDB::bind_method(D_METHOD("get_lacunarity"), &Simplex::get_lacunarity);
    ClassDB::bind_method(D_METHOD("set_gain", "gain"), &Simplex::set_gain);
    ClassDB::bind_method(D_METHOD("get_gain"), &Simplex::get_gain);
    ClassDB::bind_method(D_METHOD("set_fractal_type", "fractal_type"), &Simplex::set_fractal_type);
    ClassDB::bind_method(D_METHOD("get_fractal_type"), &Simplex::get_fractal_type);
    ClassDB::bind_method(D_METHOD("set_ping_pong_strength", "ping_pong_strength"), &Simplex::set_ping_pong_strength);
    ClassDB::bind_method(D_METHOD("get_ping_pong_strength"), &Simplex::get_ping_pong_strength);

    // Domain Warp binding
    ClassDB::bind_method(D_METHOD("set_domain_warp_enabled", "enabled"), &Simplex::set_domain_warp_enabled);
    ClassDB::bind_method(D_METHOD("get_domain_warp_enabled"), &Simplex::get_domain_warp_enabled);
    ClassDB::bind_method(D_METHOD("set_domain_warp_type", "type"), &Simplex::set_domain_warp_type);
    ClassDB::bind_method(D_METHOD("get_domain_warp_type"), &Simplex::get_domain_warp_type);
    ClassDB::bind_method(D_METHOD("set_domain_warp_amplitude", "amplitude"), &Simplex::set_domain_warp_amplitude);
    ClassDB::bind_method(D_METHOD("get_domain_warp_amplitude"), &Simplex::get_domain_warp_amplitude);
    ClassDB::bind_method(D_METHOD("set_domain_warp_frequency", "frequency"), &Simplex::set_domain_warp_frequency);
    ClassDB::bind_method(D_METHOD("get_domain_warp_frequency"), &Simplex::get_domain_warp_frequency);
    ClassDB::bind_method(D_METHOD("set_domain_warp_fractal_type", "fractal_type"), &Simplex::set_domain_warp_fractal_type);
    ClassDB::bind_method(D_METHOD("get_domain_warp_fractal_type"), &Simplex::get_domain_warp_fractal_type);
    ClassDB::bind_method(D_METHOD("set_domain_warp_octaves", "octaves"), &Simplex::set_domain_warp_octaves);
    ClassDB::bind_method(D_METHOD("get_domain_warp_octaves"), &Simplex::get_domain_warp_octaves);
    ClassDB::bind_method(D_METHOD("set_domain_warp_lacunarity", "lacunarity"), &Simplex::set_domain_warp_lacunarity);
    ClassDB::bind_method(D_METHOD("get_domain_warp_lacunarity"), &Simplex::get_domain_warp_lacunarity);
    ClassDB::bind_method(D_METHOD("set_domain_warp_gain", "gain"), &Simplex::set_domain_warp_gain);
    ClassDB::bind_method(D_METHOD("get_domain_warp_gain"), &Simplex::get_domain_warp_gain);

    // Static Properties
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency", 
        PROPERTY_HINT_RANGE, "0.0001,1,0.0001,exp"), 
        "set_frequency", "get_frequency");
    
    // Enum
    BIND_ENUM_CONSTANT(FRACTAL_NONE);
    BIND_ENUM_CONSTANT(FRACTAL_FBM);
    BIND_ENUM_CONSTANT(FRACTAL_RIDGED);
    BIND_ENUM_CONSTANT(FRACTAL_PING_PONG);

    // Domain Warp Enums
    BIND_ENUM_CONSTANT(DOMAIN_WARP_SIMPLEX);
    BIND_ENUM_CONSTANT(DOMAIN_WARP_FRACTAL_NONE);
    BIND_ENUM_CONSTANT(DOMAIN_WARP_FRACTAL_PROGRESSIVE);
    BIND_ENUM_CONSTANT(DOMAIN_WARP_FRACTAL_INDEPENDENT);
}

void Simplex::_get_property_list(List<PropertyInfo> *p_list) const
{
    p_list->push_back(PropertyInfo(Variant::OBJECT, "noise_preview", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY));
    // Add a group heading in the inspector
    p_list->push_back(PropertyInfo(Variant::NIL, "Fractal", PROPERTY_HINT_NONE, "fractal_", PROPERTY_USAGE_GROUP));
    // Fractal type enum
    p_list->push_back(PropertyInfo(Variant::INT, "fractal_type", PROPERTY_HINT_ENUM, "None,FBM,Ridged,Ping-Pong"));
    
    if (type != FractalType::FRACTAL_NONE) {
        p_list->push_back(PropertyInfo(Variant::INT, "fractal_octaves"));
        p_list->push_back(PropertyInfo(Variant::FLOAT, "fractal_lacunarity"));
        p_list->push_back(PropertyInfo(Variant::FLOAT, "fractal_gain"));
        if (type == FractalType::FRACTAL_PING_PONG)
            p_list->push_back(PropertyInfo(Variant::FLOAT, "fractal_ping_pong_strength"));
    }

    // Domain Warp group
    p_list->push_back(PropertyInfo(Variant::NIL, "Domain Warp", PROPERTY_HINT_NONE, "domain_warp_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::BOOL, "domain_warp_enabled"));

    if (domain_warp_enabled) {
        p_list->push_back(PropertyInfo(Variant::INT, "domain_warp_type", PROPERTY_HINT_ENUM, "Simplex"));
        p_list->push_back(PropertyInfo(Variant::FLOAT, "domain_warp_amplitude"));
        p_list->push_back(PropertyInfo(Variant::FLOAT, "domain_warp_frequency", 
            PROPERTY_HINT_RANGE, "0.0001,1,0.0001,exp"));
        p_list->push_back(PropertyInfo(Variant::INT, "domain_warp_fractal_type", 
            PROPERTY_HINT_ENUM, "None,Progressive,Independent"));

        if (domain_warp_fractal_type != DomainWarpFractalType::DOMAIN_WARP_FRACTAL_NONE) {
            p_list->push_back(PropertyInfo(Variant::INT, "domain_warp_octaves"));
            p_list->push_back(PropertyInfo(Variant::FLOAT, "domain_warp_lacunarity"));
            p_list->push_back(PropertyInfo(Variant::FLOAT, "domain_warp_gain"));
        }
    }
}

bool Simplex::_property_can_revert(const StringName &p_property) const
{
    if (p_property == StringName("fractal_type")) return true;
    if (p_property == StringName("fractal_octaves")) return true;
    if (p_property == StringName("fractal_lacunarity")) return true;
    if (p_property == StringName("fractal_gain")) return true;
    if (p_property == StringName("fractal_ping_pong_strength")) return true;
    if (p_property == StringName("frequency")) return true;
    if (p_property == StringName("seed")) return true;

    // Domain Warp properties
    if (p_property == StringName("domain_warp_enabled")) return true;
    if (p_property == StringName("domain_warp_type")) return true;
    if (p_property == StringName("domain_warp_amplitude")) return true;
    if (p_property == StringName("domain_warp_frequency")) return true;
    if (p_property == StringName("domain_warp_fractal_type")) return true;
    if (p_property == StringName("domain_warp_octaves")) return true;
    if (p_property == StringName("domain_warp_lacunarity")) return true;
    if (p_property == StringName("domain_warp_gain")) return true;

    return false;
}

bool Simplex::_property_get_revert(const StringName &p_property, Variant &r_ret) const
{
    if (p_property == StringName("fractal_type")) {
        r_ret = (int)FRACTAL_NONE;  // Use the enum constant directly
        return true;
    }
    if (p_property == StringName("fractal_octaves")) {
        r_ret = 8;
        return true;
    }
    if (p_property == StringName("fractal_lacunarity")) {
        r_ret = 2.0f;  // Default value for lacunarity
        return true;
    }
    if (p_property == StringName("fractal_gain")) {
        r_ret = 0.5f;  // Default value for gain
        return true;
    }
    if (p_property == StringName("fractal_ping_pong_strength")) {
        r_ret = 2.0f;  
        return true;
    }
    if (p_property == StringName("frequency")) {
        r_ret = 0.5f;
        return true;
    }
    if (p_property == StringName("seed")) {
        r_ret = 0;
        return true;
    }

    // Domain Warp defaults
    if (p_property == StringName("domain_warp_enabled")) {
        r_ret = false;
        return true;
    }
    if (p_property == StringName("domain_warp_type")) {
        r_ret = (int)DOMAIN_WARP_SIMPLEX;
        return true;
    }
    if (p_property == StringName("domain_warp_amplitude")) {
        r_ret = 30.0f;
        return true;
    }
    if (p_property == StringName("domain_warp_frequency")) {
        r_ret = 0.05f;
        return true;
    }
    if (p_property == StringName("domain_warp_fractal_type")) {
        r_ret = (int)DOMAIN_WARP_FRACTAL_PROGRESSIVE;
        return true;
    }
    if (p_property == StringName("domain_warp_octaves")) {
        r_ret = 5;
        return true;
    }
    if (p_property == StringName("domain_warp_lacunarity")) {
        r_ret = 6.0f;
        return true;
    }
    if (p_property == StringName("domain_warp_gain")) {
        r_ret = 0.5f;
        return true;
    }

    return false;
}

bool Simplex::_set(const StringName &p_name, const Variant &p_value) {
    if (p_name == StringName("fractal_type")) {
        set_fractal_type((FractalType)p_value.operator int64_t());
        return true;
    } else if (p_name == StringName("fractal_octaves")) {
        set_octaves(p_value);
        return true;
    } else if (p_name == StringName("fractal_lacunarity")) {
        set_lacunarity(p_value);
        return true;
    } else if (p_name == StringName("fractal_gain")) {
        set_gain(p_value);
        return true;
    } else if (p_name == StringName("fractal_ping_pong_strength")) {
        set_ping_pong_strength(p_value);
        return true;
    }

    // Domain Warp setters
    else if (p_name == StringName("domain_warp_enabled")) {
        set_domain_warp_enabled(p_value);
        return true;
    } else if (p_name == StringName("domain_warp_type")) {
        set_domain_warp_type((DomainWarpType)p_value.operator int64_t());
        return true;
    } else if (p_name == StringName("domain_warp_amplitude")) {
        set_domain_warp_amplitude(p_value);
        return true;
    } else if (p_name == StringName("domain_warp_frequency")) {
        set_domain_warp_frequency(p_value);
        return true;
    } else if (p_name == StringName("domain_warp_fractal_type")) {
        set_domain_warp_fractal_type((DomainWarpFractalType)p_value.operator int64_t());
        return true;
    } else if (p_name == StringName("domain_warp_octaves")) {
        set_domain_warp_octaves(p_value);
        return true;
    } else if (p_name == StringName("domain_warp_lacunarity")) {
        set_domain_warp_lacunarity(p_value);
        return true;
    } else if (p_name == StringName("domain_warp_gain")) {
        set_domain_warp_gain(p_value);
        return true;
    }
    return false;
}

bool Simplex::_get(const StringName &p_name, Variant &r_ret) const {
    if (p_name == StringName("fractal_type")) {
        r_ret = (int)type;
        return true;
    } else if (p_name == StringName("fractal_octaves")) {
        r_ret = octaves;
        return true;
    } else if (p_name == StringName("fractal_lacunarity")) {
        r_ret = lacunarity;
        return true;
    } else if (p_name == StringName("fractal_gain")) {
        r_ret = gain;
        return true;
    } else if (p_name == StringName("fractal_ping_pong_strength")) {
        r_ret = pingPongStrength;
        return true;
    } else if (p_name == StringName("noise_preview")) {
        if (preview_cache.is_null()) {
            const_cast<Simplex*>(this)->_update_preview();
        }
        r_ret = preview_cache;
        return true;
    }

    // Domain Warp getters
    else if (p_name == StringName("domain_warp_enabled")) {
        r_ret = domain_warp_enabled;
        return true;
    } else if (p_name == StringName("domain_warp_type")) {
        r_ret = (int)domain_warp_type;
        return true;
    } else if (p_name == StringName("domain_warp_amplitude")) {
        r_ret = domain_warp_amplitude;
        return true;
    } else if (p_name == StringName("domain_warp_frequency")) {
        r_ret = domain_warp_frequency;
        return true;
    } else if (p_name == StringName("domain_warp_fractal_type")) {
        r_ret = (int)domain_warp_fractal_type;
        return true;
    } else if (p_name == StringName("domain_warp_octaves")) {
        r_ret = domain_warp_octaves;
        return true;
    } else if (p_name == StringName("domain_warp_lacunarity")) {
        r_ret = domain_warp_lacunarity;
        return true;
    } else if (p_name == StringName("domain_warp_gain")) {
        r_ret = domain_warp_gain;
        return true;
    }
    return false;
}

float Simplex::get_noise_1d(float p_x) const
{
    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, p_x, seed);
    default:
        return this->noise->fractal(this->octaves, p_x, seed);
    }
}

float Simplex::get_noise_2d(float p_x, float p_y) const
{
    // Apply domain warp if enabled
    if (domain_warp_enabled) {
        _apply_domain_warp_2d(p_x, p_y);
    }

    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, p_x, p_y, seed);
    case FRACTAL_RIDGED:
        return this->noise->ridged(this->octaves, p_x, p_y, seed);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(this->octaves, p_x, p_y, seed);
    default:
        return this->noise->fractal(this->octaves, p_x, p_y, seed);
    }
}

float Simplex::get_noise_2dv(const Vector2 &p_v) const
{
    float x = p_v.x;
    float y = p_v.y;

    // Apply domain warp if enabled
    if (domain_warp_enabled) {
        _apply_domain_warp_2d(x, y);
    }

    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, x, y, seed);
    case FRACTAL_RIDGED:
        return this->noise->ridged(this->octaves, x, y, seed);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(this->octaves, x, y, seed);
    default:
        return this->noise->fractal(this->octaves, x, y, seed);
    }
    
}

float Simplex::get_noise_3d(float p_x, float p_y, float p_z) const
{
    // Apply domain warp if enabled
    if (domain_warp_enabled) {
        _apply_domain_warp_3d(p_x, p_y, p_z);
    }

    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, p_x, p_y, p_z, seed);
    case FRACTAL_RIDGED:
        return this->noise->ridged(this->octaves, p_x, p_y, p_z, seed);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(this->octaves, p_x, p_y, p_z, seed);
    default:
        return this->noise->fractal(this->octaves, p_x, p_y, p_z, seed);
    }
}

float Simplex::get_noise_3dv(const Vector3 &p_v) const
{
    float x = p_v.x;
    float y = p_v.y;
    float z = p_v.z;

    // Apply domain warp if enabled
    if (domain_warp_enabled) {
        _apply_domain_warp_3d(x, y, z);
    }

    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, x, y, z, seed);
    case FRACTAL_RIDGED:
        return this->noise->ridged(this->octaves, x, y, z, seed);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(this->octaves, x, y, z, seed);
    default:
        return this->noise->fractal(this->octaves, x, y, z, seed);
    }
}

void Simplex::set_seed(int32_t seed)
{
    this->seed = seed;
    _update_preview();
    emit_changed();
}

int32_t Simplex::get_seed()
{
    return this->seed;
}

void Simplex::set_frequency(float frequency) {
    float freq = CLAMP(frequency, 0.0f, 1.0f);
    this->noise->mFrequency = freq;
    this->frequency = freq;
    _update_preview();
    emit_changed();
}

float Simplex::get_frequency() {
    return this->frequency;
}

void Simplex::set_lacunarity(float lacunarity)
{
    this->noise->mLacunarity = lacunarity;
    this->lacunarity = lacunarity;
    _update_preview();
    emit_changed();
}

float Simplex::get_lacunarity()
{
    return this->lacunarity;
}

void Simplex::set_gain(float gain)
{
    this->noise->mPersistence = gain;
    this->gain = gain;
    _update_preview();
    emit_changed();
}

float Simplex::get_gain()
{
    return this->gain;
}

void Simplex::set_ping_pong_strength(float ping_pong_strength)
{
    this->noise->mPingPongStrength = ping_pong_strength;
    this->pingPongStrength = ping_pong_strength;
    _update_preview();
    emit_changed();
}

float Simplex::get_ping_pong_strength()
{
    return this->pingPongStrength;
}

void Simplex::set_octaves(uint16_t octaves)
{
    this->octaves = octaves;
    _update_preview();
    emit_changed();
}

uint16_t Simplex::get_octaves()
{
    return this->octaves;
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

// ============================================================================
// DOMAIN WARP PROPERTY IMPLEMENTATIONS
// ============================================================================

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
    this->domain_warp_amplitude = amplitude;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_amplitude()
{
    return this->domain_warp_amplitude;
}

void Simplex::set_domain_warp_frequency(float frequency)
{
    float freq = CLAMP(frequency, 0.0f, 1.0f);
    this->domain_warp_frequency = freq;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_frequency()
{
    return this->domain_warp_frequency;
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
    this->domain_warp_octaves = octaves;
    _update_preview();
    emit_changed();
}

uint16_t Simplex::get_domain_warp_octaves()
{
    return this->domain_warp_octaves;
}

void Simplex::set_domain_warp_lacunarity(float lacunarity)
{
    this->domain_warp_lacunarity = lacunarity;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_lacunarity()
{
    return this->domain_warp_lacunarity;
}

void Simplex::set_domain_warp_gain(float gain)
{
    this->domain_warp_gain = gain;
    _update_preview();
    emit_changed();
}

float Simplex::get_domain_warp_gain()
{
    return this->domain_warp_gain;
}

// ============================================================================
// DOMAIN WARP APPLICATION
// ============================================================================

/// THREE FRACTAL TYPES:
/// 1. NONE: Single-pass warp - fast, simple distortion
/// 2. PROGRESSIVE: Multi-pass warp - applies warping multiple times sequentially, each pass adds more detail. Coordinates accumulate distortion.
/// 3. INDEPENDENT: Each octave warps independently, then blended together. Creates smoother, more natural-looking warping.

void Simplex::_apply_domain_warp_2d(float& x, float& y) const
{
    if (domain_warp_fractal_type == DomainWarpFractalType::DOMAIN_WARP_FRACTAL_NONE) {
        // Single-pass warp
        SimplexNoise(domain_warp_frequency, domain_warp_amplitude, domain_warp_lacunarity, domain_warp_gain)
            .domainWarp2D(x, y, seed, 1);
        return;
    }

    float frequency = domain_warp_frequency;
    float amplitude = domain_warp_amplitude;

    if (domain_warp_fractal_type == DomainWarpFractalType::DOMAIN_WARP_FRACTAL_PROGRESSIVE) {
        // Progressive: accumulate warps per octave
        for (uint16_t i = 0; i < domain_warp_octaves; i++) {
            SimplexNoise(frequency, amplitude, domain_warp_lacunarity, domain_warp_gain)
                .domainWarp2D(x, y, seed + i, 1); // single-pass per octave
            frequency *= domain_warp_lacunarity;
            amplitude *= domain_warp_gain;
        }
    } 
    else if (domain_warp_fractal_type == DomainWarpFractalType::DOMAIN_WARP_FRACTAL_INDEPENDENT) {
        // Independent: warp each octave independently and blend
        float final_x = 0.f;
        float final_y = 0.f;

        for (uint16_t i = 0; i < domain_warp_octaves; i++) {
            float warp_x = x;
            float warp_y = y;

            SimplexNoise(frequency, amplitude, domain_warp_lacunarity, domain_warp_gain)
                .domainWarp2D(warp_x, warp_y, seed + i, 1);

            final_x += (warp_x - x) * amplitude;
            final_y += (warp_y - y) * amplitude;

            frequency *= domain_warp_lacunarity;
            amplitude *= domain_warp_gain;
        }

        x += final_x;
        y += final_y;
    }
}

// Apply 3D domain warping
// Same logic as 2D but operates on three axes (x, y, z)
void Simplex::_apply_domain_warp_3d(float& x, float& y, float& z) const
{
    if (domain_warp_fractal_type == DomainWarpFractalType::DOMAIN_WARP_FRACTAL_NONE) {
        SimplexNoise(domain_warp_frequency, domain_warp_amplitude, domain_warp_lacunarity, domain_warp_gain)
            .domainWarp3D(x, y, z, seed, 1);
        return;
    }

    float frequency = domain_warp_frequency;
    float amplitude = domain_warp_amplitude;

    if (domain_warp_fractal_type == DomainWarpFractalType::DOMAIN_WARP_FRACTAL_PROGRESSIVE) {
        // Progressive: accumulate warps per octave
        for (uint16_t i = 0; i < domain_warp_octaves; i++) {
            SimplexNoise(frequency, amplitude, domain_warp_lacunarity, domain_warp_gain)
                .domainWarp3D(x, y, z, seed + i, 1); // single-pass per octave
            frequency *= domain_warp_lacunarity;
            amplitude *= domain_warp_gain;
        }
    } 
    else if (domain_warp_fractal_type == DomainWarpFractalType::DOMAIN_WARP_FRACTAL_INDEPENDENT) {
        // Independent: warp each octave independently and blend
        float final_x = 0.f;
        float final_y = 0.f;
        float final_z = 0.f;

        for (uint16_t i = 0; i < domain_warp_octaves; i++) {
            float warp_x = x;
            float warp_y = y;
            float warp_z = z;

            SimplexNoise(frequency, amplitude, domain_warp_lacunarity, domain_warp_gain)
                .domainWarp3D(warp_x, warp_y, warp_z, seed + i, 1);

            final_x += (warp_x - x) * amplitude;
            final_y += (warp_y - y) * amplitude;
            final_z += (warp_z - z) * amplitude;

            frequency *= domain_warp_lacunarity;
            amplitude *= domain_warp_gain;
        }

        x += final_x;
        y += final_y;
        z += final_z;
    }
}

// ============================================================================
// IMAGE PREVIEW GENERATION
// ============================================================================

/// Generate 128x128 preview texture
/// SHOULD update in real-time as settings change, but does not reflect it yet (i think)

void Simplex::_update_preview()
{
    const int size = 128;  // Preview resolution (128x128 pixels)
    
    // Create image for the preview texture
    Ref<Image> image = Image::create(size, size, false, Image::FORMAT_L8);
    
    // Sample noise at each pixel and convert to grayscale
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Get noise value at this position, normalized to [0, 1]
            float n = (get_noise_2d((float)x, (float)y) + 1.0f) * 0.5f;
            // Set pixel to grayscale color
            image->set_pixel(x, y, Color(n, n, n));
        }
    }
    
    // Cache the texture for display in inspector
    if (preview_cache.is_null()) {
        // First time: create new texture
        preview_cache = ImageTexture::create_from_image(image);
    } else {
        // Subsequent times: update existing texture (more efficient)
        preview_cache->update(image);
    }
}