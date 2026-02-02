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


    // Static Properties
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency", 
        PROPERTY_HINT_RANGE, "0,1,0.0001"), 
        "set_frequency", "get_frequency");
    
    // Enum
    BIND_ENUM_CONSTANT(FRACTAL_NONE);
    BIND_ENUM_CONSTANT(FRACTAL_FBM);
    BIND_ENUM_CONSTANT(FRACTAL_RIDGED);
    BIND_ENUM_CONSTANT(FRACTAL_PING_PONG);
}

void Simplex::_get_property_list(List<PropertyInfo> *p_list) const
{
    // Add a group heading in the inspector
    p_list->push_back(PropertyInfo(Variant::NIL, "Fractal", PROPERTY_HINT_NONE, "fractal_", PROPERTY_USAGE_GROUP));
    // Fractal type enum
    p_list->push_back(PropertyInfo(Variant::INT, "fractal_type", PROPERTY_HINT_ENUM, "FRACTAL_NONE,FRACTAL_FBM,FRACTAL_RIDGED,FRACTAL_PING_PONG"));
    
    if (type != FractalType::FRACTAL_NONE) {
        p_list->push_back(PropertyInfo(Variant::INT, "fractal_octaves", PROPERTY_HINT_RANGE, "1,16,1", PROPERTY_USAGE_DEFAULT));
        p_list->push_back(PropertyInfo(Variant::FLOAT, "fractal_lacunarity", PROPERTY_HINT_RANGE, "1.0,4.0,0.01", PROPERTY_USAGE_DEFAULT));
        p_list->push_back(PropertyInfo(Variant::FLOAT, "fractal_gain", PROPERTY_HINT_RANGE, "0.0,1.0,0.01", PROPERTY_USAGE_DEFAULT));
    }
}

bool Simplex::_property_can_revert(const StringName &p_property) const
{
    if (p_property == StringName("fractal_type")) return true;
    if (p_property == StringName("fractal_octaves")) return true;
    if (p_property == StringName("fractal_lacunarity")) return true;
    if (p_property == StringName("fractal_gain")) return true;
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
    }
    return false;
}

float Simplex::get_noise_1d(float p_x) const
{
    if (this->type == FRACTAL_NONE)
        return this->noise->fractal(1, p_x, seed);
    return this->noise->fractal(this->octaves, p_x, seed);
}

float Simplex::get_noise_2d(float p_x, float p_y) const
{
    if (this->type == FRACTAL_NONE)
        return this->noise->fractal(1, p_x, p_y, seed);
    return this->noise->fractal(this->octaves, p_x, p_y, seed);
}

float Simplex::get_noise_2dv(const Vector2 &p_v) const
{
    if (this->type == FRACTAL_NONE)
        return this->noise->fractal(1, p_v.x, p_v.y, seed);
    return this->noise->fractal(this->octaves, p_v.x, p_v.y, seed);
    
}

float Simplex::get_noise_3d(float p_x, float p_y, float p_z) const
{
    if (this->type == FRACTAL_NONE)
        return this->noise->fractal(1, p_x, p_y, p_z, seed);
    return this->noise->fractal(this->octaves, p_x, p_y, p_z, seed);
}

float Simplex::get_noise_3dv(const Vector3 &p_v) const
{
    if (this->type == FRACTAL_NONE)
        return this->noise->fractal(1, p_v.x, p_v.y, p_v.z, seed);
    return this->noise->fractal(this->octaves, p_v.x, p_v.y, p_v.z, seed);
}

void Simplex::set_seed(int32_t seed)
{
    this->seed = seed;
}

int32_t Simplex::get_seed()
{
    return this->seed;
}

void Simplex::set_frequency(float frequency) {
    float freq = CLAMP(frequency, 0.0f, 1.0f);
    this->noise->mFrequency = freq;
    this->frequency = freq;
}

float Simplex::get_frequency() {
    return this->frequency;
}

void Simplex::set_lacunarity(float lacunarity)
{
    this->noise->mLacunarity = lacunarity;
    this->lacunarity = lacunarity;
}

float Simplex::get_lacunarity()
{
    return this->lacunarity;
}

void Simplex::set_gain(float gain)
{
    this->noise->mPersistence = gain;
    this->gain = gain;
}

float Simplex::get_gain()
{
    return this->gain;
}

void Simplex::set_octaves(uint16_t octaves)
{
    this->octaves = octaves;
}

uint16_t Simplex::get_octaves()
{
    return this->octaves;
}

void Simplex::set_fractal_type(FractalType fractal_type)
{
    if (this->type != fractal_type) {
        this->type = fractal_type;
        notify_property_list_changed();
    }
    
}

Simplex::FractalType Simplex::get_fractal_type()
{
    return this->type;
}

