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

    // Bind image generation methods
    ClassDB::bind_method(D_METHOD("get_image", "width", "height", "invert", "in_3d_space", "normalize"), 
        &Simplex::get_image, DEFVAL(false), DEFVAL(false), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("get_seamless_image", "width", "height", "invert", "in_3d_space", "skirt", "normalize"), 
        &Simplex::get_seamless_image, DEFVAL(false), DEFVAL(false), DEFVAL(0.1), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("get_image_3d", "width", "height", "depth", "invert", "normalize"), 
        &Simplex::get_image_3d, DEFVAL(false), DEFVAL(true));
    ClassDB::bind_method(D_METHOD("get_seamless_image_3d", "width", "height", "depth", "invert", "skirt", "normalize"), 
        &Simplex::get_seamless_image_3d, DEFVAL(false), DEFVAL(0.1), DEFVAL(true));

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
        r_ret = 0.01f;
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
        r_ret = this->noise->mDomainWarpFractalLacunarity;
        return true;
    } else if (p_name == StringName("domain_warp_gain")) {
        r_ret = this->noise->mDomainWarpFractalGain;
        return true;
    }
    return false;
}

float Simplex::get_noise_1d(float p_x) const
{
    return this->noise->fractal(p_x, this->type == FRACTAL_NONE);
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
        return this->noise->fractal(p_x, p_y, this->type == FRACTAL_NONE);
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
        return this->noise->fractal(x, y, this->type == FRACTAL_NONE);
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
        return this->noise->fractal(p_x, p_y, p_z, this->type == FRACTAL_NONE);
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

Ref<Image> Simplex::get_image(int32_t p_width, int32_t p_height, bool p_invert, bool p_in_3d_space, bool p_normalize) const
{
    Ref<Image> image = Image::create(p_width, p_height, false, Image::FORMAT_L8);
    
    for (int y = 0; y < p_height; y++) {
        for (int x = 0; x < p_width; x++) {
            float n;
            if (p_in_3d_space) {
                // Use x,z plane with y=0
                n = get_noise_3d((float)x, 0.0f, (float)y);
            } else {
                n = get_noise_2d((float)x, (float)y);
            }
            
            if (p_normalize) {
                n = (n + 1.0f) * 0.5f;
            }
            
            if (p_invert) {
                n = 1.0f - n;
            }
            
            n = CLAMP(n, 0.0f, 1.0f);
            uint8_t value = static_cast<uint8_t>(n * 255.0f);
            image->set_pixel(x, y, Color(value / 255.0f, value / 255.0f, value / 255.0f));
        }
    }
    
    return image;
}

Ref<Image> Simplex::get_seamless_image(int p_width, int p_height, bool p_invert, bool p_in_3d_space, float p_skirt, bool p_normalize) const {
    Ref<Image> image = Image::create(p_width, p_height, false, Image::FORMAT_L8);
    
    if (p_in_3d_space) {
        // Map 2D texture coordinates to two intersecting cylinders in 3D space.
        // This emulates a 4D torus to avoid the "pinch" distortion of a 3D torus.
        for (int y = 0; y < p_height; y++) {
            for (int x = 0; x < p_width; x++) {
                float nx = (float)x / p_width;
                float ny = (float)y / p_height;
                
                float angle_x = nx * Math_TAU;
                float angle_y = ny * Math_TAU;
                
                // Radii scaled so that the circumference matches pixel dimensions
                float scale_x = p_width / Math_TAU;
                float scale_y = p_height / Math_TAU;
                
                float px = Math::cos(angle_x) * scale_x;
                float pz = Math::sin(angle_x) * scale_x;
                float py = Math::cos(angle_y) * scale_y;
                float pw = Math::sin(angle_y) * scale_y;
                
                // Average two 3D samples to simulate 4D seamlessness
                float n = get_noise_3d(px, py, pz) * 0.5f +
                          get_noise_3d(py, pz, pw) * 0.5f;

                if (p_normalize) n = (n + 1.0f) * 0.5f;
                if (p_invert) n = 1.0f - n;

                n = CLAMP(n, 0.0f, 1.0f);
                image->set_pixel(x, y, Color(n, n, n));
            }
        }
    } else {
        float skirt_w = p_width * p_skirt;
        float skirt_h = p_height * p_skirt;

        for (int y = 0; y < p_height; y++) {
            for (int x = 0; x < p_width; x++) {
                // Use raw pixel coordinates to maintain noise scale
                float n_center = get_noise_2d((float)x, (float)y);

                // Determine offset direction based on which edge we are closer to
                float off_x = (x < p_width / 2) ? (float)p_width : -(float)p_width;
                float off_y = (y < p_height / 2) ? (float)p_height : -(float)p_height;

                // Sample wrapped neighbors
                float n_x = get_noise_2d(x + off_x, (float)y);
                float n_y = get_noise_2d((float)x, y + off_y);
                float n_xy = get_noise_2d(x + off_x, y + off_y);

                // Calculate weights (0.0 at edge, 1.0 at center)
                float weight_x = (x < skirt_w) ? (x / skirt_w) : ((p_width - 1 - x) < skirt_w ? (p_width - 1 - x) / skirt_w : 1.0f);
                float weight_y = (y < skirt_h) ? (y / skirt_h) : ((p_height - 1 - y) < skirt_h ? (p_height - 1 - y) / skirt_h : 1.0f);

                // Smooth interpolation alpha
                float ax = 0.5f + 0.5f * weight_x;
                float ay = 0.5f + 0.5f * weight_y;

                // Bilinear blend of the 4 samples
                float n = Math::lerp(
                    Math::lerp(n_xy, n_y, ax),
                    Math::lerp(n_x, n_center, ax),
                    ay
                );

                if (p_normalize) n = (n + 1.0f) * 0.5f;
                if (p_invert) n = 1.0f - n;

                n = CLAMP(n, 0.0f, 1.0f);
                image->set_pixel(x, y, Color(n, n, n));
            }
        }
    }
    return image;
}
TypedArray<Image> Simplex::get_image_3d(int32_t p_width, int32_t p_height, int32_t p_depth, bool p_invert, bool p_normalize) const
{
    TypedArray<Image> images;
    images.resize(p_depth);
    
    for (int z = 0; z < p_depth; z++) {
        Ref<Image> slice = Image::create(p_width, p_height, false, Image::FORMAT_L8);
        
        for (int y = 0; y < p_height; y++) {
            for (int x = 0; x < p_width; x++) {
                float n = get_noise_3d((float)x, (float)y, (float)z);
                
                if (p_normalize) {
                    n = (n + 1.0f) * 0.5f;
                }
                
                if (p_invert) {
                    n = 1.0f - n;
                }
                
                n = CLAMP(n, 0.0f, 1.0f);
                uint8_t value = static_cast<uint8_t>(n * 255.0f);
                slice->set_pixel(x, y, Color(value / 255.0f, value / 255.0f, value / 255.0f));
            }
        }
        
        images[z] = slice;
    }
    
    return images;
}

TypedArray<Image> Simplex::get_seamless_image_3d(int p_width, int p_height, int p_depth, bool p_invert, float p_skirt, bool p_normalize) const {
    TypedArray<Image> images;
    images.resize(p_depth);

    float skirt_w = p_width * p_skirt;
    float skirt_h = p_height * p_skirt;
    float skirt_d = p_depth * p_skirt;

    for (int z = 0; z < p_depth; z++) {
        Ref<Image> slice = Image::create(p_width, p_height, false, Image::FORMAT_L8);
        
        for (int y = 0; y < p_height; y++) {
            for (int x = 0; x < p_width; x++) {
                // Use raw pixel coordinates (x, y, z)
                float n_center = get_noise_3d((float)x, (float)y, (float)z);

                // Offset directions
                float ox = (x < p_width / 2) ? (float)p_width : -(float)p_width;
                float oy = (y < p_height / 2) ? (float)p_height : -(float)p_height;
                float oz = (z < p_depth / 2) ? (float)p_depth : -(float)p_depth;

                // Sample wrapped neighbors (7 additional samples for full 3D seamlessness)
                float n_x = get_noise_3d(x + ox, (float)y, (float)z);
                float n_y = get_noise_3d((float)x, y + oy, (float)z);
                float n_z = get_noise_3d((float)x, (float)y, z + oz);

                // Blending weights
                float wx = (x < skirt_w) ? (x / skirt_w) : ((p_width - 1 - x) < skirt_w ? (p_width - 1 - x) / skirt_w : 1.0f);
                float wy = (y < skirt_h) ? (y / skirt_h) : ((p_height - 1 - y) < skirt_h ? (p_height - 1 - y) / skirt_h : 1.0f);
                float wz = (z < skirt_d) ? (z / skirt_d) : ((p_depth - 1 - z) < skirt_d ? (p_depth - 1 - z) / skirt_d : 1.0f);

                float ax = 0.5f + 0.5f * wx;
                float ay = 0.5f + 0.5f * wy;
                float az = 0.5f + 0.5f * wz;

                // Triple lerp for 3D blending
                float nx = Math::lerp(n_x, n_center, ax);
                float ny = Math::lerp(n_y, n_center, ay);
                float nz = Math::lerp(n_z, n_center, az);
                float n = (nx + ny + nz) / 3.0f;

                if (p_normalize) n = (n + 1.0f) * 0.5f;
                if (p_invert) n = 1.0f - n;

                n = CLAMP(n, 0.0f, 1.0f);
                slice->set_pixel(x, y, Color(n, n, n));
            }
        }
        images[z] = slice;
    }
    return images;
}

void Simplex::set_seed(int32_t seed)
{
    this->noise->mSeed = seed;
    _update_preview();
    emit_changed();
}

int32_t Simplex::get_seed()
{
    return this->noise->mSeed;
}

void Simplex::set_frequency(float frequency) {
    float freq = CLAMP(frequency, 0.0f, 1.0f);
    this->noise->mFrequency = freq;
    _update_preview();
    emit_changed();
}

float Simplex::get_frequency() {
    return this->noise->mFrequency;
}

void Simplex::_update_preview()
{
    int size = 128;
    Ref<Image> image = get_image(size, size, false, false, true);
    
    if (preview_cache.is_null()) {
        preview_cache = ImageTexture::create_from_image(image);
    } else {
        preview_cache->update(image);
    }
}