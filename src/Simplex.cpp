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
    BIND_ENUM_CONSTANT(DOMAIN_WARP_SIMPLEX);
    BIND_ENUM_CONSTANT(DOMAIN_WARP_FRACTAL_NONE);
    BIND_ENUM_CONSTANT(DOMAIN_WARP_FRACTAL_PROGRESSIVE);
    BIND_ENUM_CONSTANT(DOMAIN_WARP_FRACTAL_INDEPENDENT);
}

void Simplex::_get_property_list(List<PropertyInfo> *p_list) const
{
    p_list->push_back(PropertyInfo(Variant::OBJECT, "noise_preview", PROPERTY_HINT_RESOURCE_TYPE, "ImageTexture", PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_READ_ONLY));
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
    if (p_property == StringName("domain_warp_enabled")) {
        r_ret = false;
        return true;
    }
    if (p_property == StringName("domain_warp_type")) {
        r_ret = DOMAIN_WARP_SIMPLEX;
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
        r_ret = DOMAIN_WARP_FRACTAL_PROGRESSIVE;
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
        r_ret = this->noise->mOctaves;
        return true;
    } else if (p_name == StringName("fractal_lacunarity")) {
        r_ret = this->noise->mLacunarity;
        return true;
    } else if (p_name == StringName("fractal_gain")) {
        r_ret = this->noise->mPersistence;
        return true;
    } else if (p_name == StringName("fractal_ping_pong_strength")) {
        r_ret = this->noise->mPingPongStrength;
        return true;
    } else if (p_name == StringName("noise_preview")) {
        if (preview_cache.is_null()) {
            const_cast<Simplex*>(this)->_update_preview();
        }
        r_ret = preview_cache;
        return true;
    }
    else if (p_name == StringName("domain_warp_enabled")) {
        r_ret = domain_warp_enabled;
        return true;
    } else if (p_name == StringName("domain_warp_type")) {
        r_ret = (int)domain_warp_type;
        return true;
    } else if (p_name == StringName("domain_warp_amplitude")) {
        r_ret = this->noise->mDomainWarpAmplitude;
        return true;
    } else if (p_name == StringName("domain_warp_frequency")) {
        r_ret = this->noise->mDomainWarpFrequency;
        return true;
    } else if (p_name == StringName("domain_warp_fractal_type")) {
        r_ret = (int)domain_warp_fractal_type;
        return true;
    } else if (p_name == StringName("domain_warp_octaves")) {
        r_ret = this->noise->mDomainWarpFractalOctaves;
        return true;
    } else if (p_name == StringName("domain_warp_lacunarity")) {
        r_ret = this->noise->mLacunarity;
        return true;
    } else if (p_name == StringName("domain_warp_gain")) {
        r_ret = this->noise->mDomainWarpFractalGain;
        return true;
    }
    return false;
}

float Simplex::get_noise_1d(float p_x) const
{
    return this->noise->fractal(p_x);
}

float Simplex::get_noise_2d(float p_x, float p_y) const
{
    if (domain_warp_enabled)
        _apply_domain_warp_2d(p_x, p_y);

    switch (this->type) {
    case FRACTAL_RIDGED:
        return this->noise->ridged(p_x, p_y);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(p_x, p_y);
    default:
        return this->noise->fractal(p_x, p_y);
    }
}

float Simplex::get_noise_2dv(const Vector2 &p_v) const
{
    real_t x = p_v.x, y = p_v.y;
    if (domain_warp_enabled)
        _apply_domain_warp_2d(x, y);
    

    switch (this->type) {
    case FRACTAL_RIDGED:
        return this->noise->ridged(x, y);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(x, y);
    default:
        return this->noise->fractal(x, y);
    }
    
}

float Simplex::get_noise_3d(float p_x, float p_y, float p_z) const
{
    switch (this->type) {
    case FRACTAL_RIDGED:
        return this->noise->ridged(p_x, p_y, p_z);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(p_x, p_y, p_z);
    default:
        return this->noise->fractal(p_x, p_y, p_z);
    }
}

float Simplex::get_noise_3dv(const Vector3 &p_v) const
{
    real_t x = p_v.x, y = p_v.y, z = p_v.z;

    switch (this->type) {
    case FRACTAL_RIDGED:
        return this->noise->ridged(x, y, z);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(x, y, z);
    default: // None and Fractal
        return this->noise->fractal(x, y, z);
    }
}

void Simplex::set_seed(int32_t seed)
{
    this->noise->mSeed = seed;
    // _update_preview();
    emit_changed();
}

int32_t Simplex::get_seed()
{
    return this->noise->mSeed;
}

void Simplex::set_frequency(float frequency) {
    float freq = CLAMP(frequency, 0.0f, 1.0f);
    this->noise->mFrequency = freq;
    // _update_preview();
    emit_changed();
}

float Simplex::get_frequency() {
    return this->noise->mFrequency;
}

void Simplex::_update_preview()
{
    int size = 128;
    Ref<Image> image = Image::create(size, size, false, Image::FORMAT_L8);
    
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            float n = (get_noise_2d((float)x, (float)y) + 1.0f) * 0.5f;
            image->set_pixel(x, y, Color(n, n, n));
        }
    }
    
    if (preview_cache.is_null()) {
        preview_cache = ImageTexture::create_from_image(image);
    } else {
        preview_cache->update(image); // More efficient than creating a new texture
    }
}
