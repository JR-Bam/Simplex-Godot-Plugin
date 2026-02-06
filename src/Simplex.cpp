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
    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, p_v.x, p_v.y, seed);
    case FRACTAL_RIDGED:
        return this->noise->ridged(this->octaves, p_v.x, p_v.y, seed);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(this->octaves, p_v.x, p_v.y, seed);
    default:
        return this->noise->fractal(this->octaves, p_v.x, p_v.y, seed);
    }
    
}

float Simplex::get_noise_3d(float p_x, float p_y, float p_z) const
{
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
    switch (this->type) {
    case FRACTAL_NONE:
        return this->noise->fractal(1, p_v.x, p_v.y, p_v.z, seed);
    case FRACTAL_RIDGED:
        return this->noise->ridged(this->octaves, p_v.x, p_v.y, p_v.z, seed);
    case FRACTAL_PING_PONG:
        return this->noise->pingpong(this->octaves, p_v.x, p_v.y, p_v.z, seed);
    default:
        return this->noise->fractal(this->octaves, p_v.x, p_v.y, p_v.z, seed);
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
    }
    
}

Simplex::FractalType Simplex::get_fractal_type()
{
    return this->type;
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
