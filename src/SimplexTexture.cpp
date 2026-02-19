#include "SimplexTexture.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/image.hpp>

namespace godot {

SimplexTexture::SimplexTexture() : 
    width(512),
    height(512),
    invert(false),
    in_3d_space(false),
    normalize(true),
    seamless(false),
    seamless_blend_skirt(0.1f),
    as_normal_map(false),
    bump_strength(8.0f),
    generate_mipmaps(true),
    current_image_size(0, 0),
    current_image_format(Image::FORMAT_MAX),
    dirty(true) {
        UtilityFunctions::print("[SimplexTexture] Constructor");

        if (noise.is_valid()) {
            noise->connect("changed", Callable(this, "_on_noise_changed"));
            UtilityFunctions::print("[SimplexTexture] Noise instantiated and connected");
        }

        // Generate initial texture
        _update_texture();
    }

SimplexTexture::~SimplexTexture() {
    if (noise.is_valid()) {
        noise->disconnect("changed", Callable(this, "_on_noise_changed"));
    }
    if (color_ramp.is_valid()) {
        color_ramp->disconnect("changed", Callable(this, "_on_color_ramp_changed"));
    }
}

void SimplexTexture::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_noise", "noise"), &SimplexTexture::set_noise);
    ClassDB::bind_method(D_METHOD("get_noise"), &SimplexTexture::get_noise);
    
    ClassDB::bind_method(D_METHOD("set_width", "width"), &SimplexTexture::set_width);
    ClassDB::bind_method(D_METHOD("get_width"), &SimplexTexture::get_width);
    
    ClassDB::bind_method(D_METHOD("set_height", "height"), &SimplexTexture::set_height);
    ClassDB::bind_method(D_METHOD("get_height"), &SimplexTexture::get_height);
    
    ClassDB::bind_method(D_METHOD("set_invert", "invert"), &SimplexTexture::set_invert);
    ClassDB::bind_method(D_METHOD("get_invert"), &SimplexTexture::get_invert);
    
    ClassDB::bind_method(D_METHOD("set_in_3d_space", "enabled"), &SimplexTexture::set_in_3d_space);
    ClassDB::bind_method(D_METHOD("get_in_3d_space"), &SimplexTexture::get_in_3d_space);
    
    ClassDB::bind_method(D_METHOD("set_normalize", "normalize"), &SimplexTexture::set_normalize);
    ClassDB::bind_method(D_METHOD("get_normalize"), &SimplexTexture::get_normalize);
    
    ClassDB::bind_method(D_METHOD("set_seamless", "seamless"), &SimplexTexture::set_seamless);
    ClassDB::bind_method(D_METHOD("get_seamless"), &SimplexTexture::get_seamless);
    
    ClassDB::bind_method(D_METHOD("set_seamless_blend_skirt", "skirt"), &SimplexTexture::set_seamless_blend_skirt);
    ClassDB::bind_method(D_METHOD("get_seamless_blend_skirt"), &SimplexTexture::get_seamless_blend_skirt);
    
    ClassDB::bind_method(D_METHOD("set_color_ramp", "ramp"), &SimplexTexture::set_color_ramp);
    ClassDB::bind_method(D_METHOD("get_color_ramp"), &SimplexTexture::get_color_ramp);
    
    ClassDB::bind_method(D_METHOD("set_as_normal_map", "enabled"), &SimplexTexture::set_as_normal_map);
    ClassDB::bind_method(D_METHOD("get_as_normal_map"), &SimplexTexture::get_as_normal_map);
    
    ClassDB::bind_method(D_METHOD("set_bump_strength", "strength"), &SimplexTexture::set_bump_strength);
    ClassDB::bind_method(D_METHOD("get_bump_strength"), &SimplexTexture::get_bump_strength);
    
    ClassDB::bind_method(D_METHOD("set_generate_mipmaps", "enabled"), &SimplexTexture::set_generate_mipmaps);
    ClassDB::bind_method(D_METHOD("get_generate_mipmaps"), &SimplexTexture::get_generate_mipmaps);
    
    ClassDB::bind_method(D_METHOD("regenerate"), &SimplexTexture::regenerate);
    ClassDB::bind_method(D_METHOD("mark_dirty"), &SimplexTexture::mark_dirty);
    
    ClassDB::bind_method(D_METHOD("get_image"), &SimplexTexture::get_image);
    
    ClassDB::bind_method(D_METHOD("_on_noise_changed"), &SimplexTexture::_on_noise_changed);
    ClassDB::bind_method(D_METHOD("_on_color_ramp_changed"), &SimplexTexture::_on_color_ramp_changed);
    ClassDB::bind_method(D_METHOD("_update_texture"), &SimplexTexture::_update_texture);
}

bool SimplexTexture::_set(const StringName &p_name, const Variant &p_value) {
    if (p_name == StringName("width")) {
        set_width(p_value);
        return true;
    } else if (p_name == StringName("height")) {
        set_height(p_value);
        return true;
    } else if (p_name == StringName("invert")) {
        set_invert(p_value);
        return true;
    } else if (p_name == StringName("in_3d_space")) {
        set_in_3d_space(p_value);
        return true;
    } else if (p_name == StringName("normalize")) {
        set_normalize(p_value);
        return true;
    } else if (p_name == StringName("seamless")) {
        set_seamless(p_value);
        return true;
    } else if (p_name == StringName("seamless_blend_skirt")) {
        set_seamless_blend_skirt(p_value);
        return true;
    } else if (p_name == StringName("color_ramp")) {
        set_color_ramp(p_value);
        return true;
    } else if (p_name == StringName("as_normal_map")) {
        set_as_normal_map(p_value);
        return true;
    } else if (p_name == StringName("bump_strength")) {
        set_bump_strength(p_value);
        return true;
    } else if (p_name == StringName("generate_mipmaps")) {
        set_generate_mipmaps(p_value);
        return true;
    } else if (p_name == StringName("noise")) {
        set_noise(p_value);
        return true;
    }
    return false;
}

bool SimplexTexture::_get(const StringName &p_name, Variant &r_ret) const {
    if (p_name == StringName("width")) {
        r_ret = width;
        return true;
    } else if (p_name == StringName("height")) {
        r_ret = height;
        return true;
    } else if (p_name == StringName("invert")) {
        r_ret = invert;
        return true;
    } else if (p_name == StringName("in_3d_space")) {
        r_ret = in_3d_space;
        return true;
    } else if (p_name == StringName("normalize")) {
        r_ret = normalize;
        return true;
    } else if (p_name == StringName("seamless")) {
        r_ret = seamless;
        return true;
    } else if (p_name == StringName("seamless_blend_skirt")) {
        r_ret = seamless_blend_skirt;
        return true;
    } else if (p_name == StringName("color_ramp")) {
        r_ret = color_ramp;
        return true;
    } else if (p_name == StringName("as_normal_map")) {
        r_ret = as_normal_map;
        return true;
    } else if (p_name == StringName("bump_strength")) {
        r_ret = bump_strength;
        return true;
    } else if (p_name == StringName("generate_mipmaps")) {
        r_ret = generate_mipmaps;
        return true;
    } else if (p_name == StringName("noise")) {
        r_ret = noise;
        return true;
    }
    return false;
}

void SimplexTexture::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "Simplex"));
    
    p_list->push_back(PropertyInfo(Variant::NIL, "Size", PROPERTY_HINT_NONE, "size_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,4096,1,or_greater"));
    p_list->push_back(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,4096,1,or_greater"));
    
    p_list->push_back(PropertyInfo(Variant::NIL, "Generation", PROPERTY_HINT_NONE, "generation_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::BOOL, "invert"));
    p_list->push_back(PropertyInfo(Variant::BOOL, "in_3d_space"));
    p_list->push_back(PropertyInfo(Variant::BOOL, "normalize"));
    p_list->push_back(PropertyInfo(Variant::BOOL, "generate_mipmaps"));
    
    p_list->push_back(PropertyInfo(Variant::NIL, "Seamless", PROPERTY_HINT_NONE, "seamless_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::BOOL, "seamless"));
    p_list->push_back(PropertyInfo(Variant::FLOAT, "seamless_blend_skirt", PROPERTY_HINT_RANGE, "0.0,0.5,0.01"));
    
    p_list->push_back(PropertyInfo(Variant::NIL, "Color", PROPERTY_HINT_NONE, "color_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "color_ramp", PROPERTY_HINT_RESOURCE_TYPE, "Gradient"));
    
    p_list->push_back(PropertyInfo(Variant::NIL, "Normal Map", PROPERTY_HINT_NONE, "normal_map_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::BOOL, "as_normal_map"));
    p_list->push_back(PropertyInfo(Variant::FLOAT, "bump_strength", PROPERTY_HINT_RANGE, "0.1,32.0,0.1"));
}

void SimplexTexture::_update_texture() {
    UtilityFunctions::print("[SimplexTexture] _update_texture() called, dirty=", dirty);
    if (!dirty) {
        return;
    }

    if (noise.is_null()) {
        UtilityFunctions::print("[SimplexTexture] _update_texture: noise is NULL!");
        return;
    }
    
    Ref<Image> image;
    
    if (seamless) {
        image = noise->get_seamless_image(width, height, invert, in_3d_space, seamless_blend_skirt, normalize);
    } else {
        image = noise->get_image(width, height, invert, in_3d_space, normalize);
    }

    if (image.is_null()) {
        UtilityFunctions::print("[SimplexTexture] _update_texture: image is NULL!");
        return;
    }
    UtilityFunctions::print("[SimplexTexture] _update_texture: image obtained, format=", image->get_format());
    
    // Apply color ramp if provided
    if (color_ramp.is_valid() && color_ramp->get_point_count() > 0) {
        image->convert(Image::FORMAT_RGBA8);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                Color pixel = image->get_pixel(x, y);
                float gray = pixel.r;
                gray = CLAMP(gray, 0.0f, 1.0f);
                Color mapped = color_ramp->sample(gray);
                image->set_pixel(x, y, mapped);
            }
        }
    }
    
    // Convert to normal map if requested
    if (as_normal_map) {
        Ref<Image> normal_map = Image::create(width, height, false, Image::FORMAT_RGBA8);
        Ref<Image> height_map = image;
        
        for (int y = 1; y < height - 1; y++) {
            for (int x = 1; x < width - 1; x++) {
                float h_center = height_map->get_pixel(x, y).r;
                float h_right = height_map->get_pixel(x + 1, y).r;
                float h_left = height_map->get_pixel(x - 1, y).r;
                float h_down = height_map->get_pixel(x, y + 1).r;
                float h_up = height_map->get_pixel(x, y - 1).r;
                
                float dx = (h_right - h_left) * bump_strength;
                float dy = (h_down - h_up) * bump_strength;
                
                Vector3 normal = Vector3(-dx, -dy, 1.0f);
                float length = normal.length();
                if (length > 0.0f) {
                    normal /= length;
                }
                
                Color normal_color(
                    (normal.x * 0.5f) + 0.5f,
                    (normal.y * 0.5f) + 0.5f,
                    (normal.z * 0.5f) + 0.5f,
                    1.0f
                );
                
                normal_map->set_pixel(x, y, normal_color);
            }
        }
        
        // Handle edges
        for (int x = 0; x < width; x++) {
            if (x > 0 && x < width - 1) {
                normal_map->set_pixel(x, 0, normal_map->get_pixel(x, 1));
                normal_map->set_pixel(x, height - 1, normal_map->get_pixel(x, height - 2));
            }
        }
        for (int y = 0; y < height; y++) {
            if (y > 0 && y < height - 1) {
                normal_map->set_pixel(0, y, normal_map->get_pixel(1, y));
                normal_map->set_pixel(width - 1, y, normal_map->get_pixel(width - 2, y));
            }
        }
        
        if (width > 1 && height > 1) {
            normal_map->set_pixel(0, 0, normal_map->get_pixel(1, 1));
            normal_map->set_pixel(width - 1, 0, normal_map->get_pixel(width - 2, 1));
            normal_map->set_pixel(0, height - 1, normal_map->get_pixel(1, height - 2));
            normal_map->set_pixel(width - 1, height - 1, normal_map->get_pixel(width - 2, height - 2));
        }
        
        image = normal_map;
    }

    if (generate_mipmaps) {
        image->generate_mipmaps();
    }
    

    Vector2i new_size = image->get_size();
    Image::Format new_format = image->get_format();

    if (new_size == current_image_size && new_format == current_image_format) {
        // Compatible → use update()
        printf("Updated\n");
        update(image);
    } else {
        printf("Initialized\n");
        // Size or format changed → must use set_image()
        set_image(image);
        // Update stored properties
        current_image_size = new_size;
        current_image_format = new_format;
    }
    
    dirty = false;
    emit_changed();
}

void SimplexTexture::_on_noise_changed() {
    dirty = true;
    emit_changed();
}

void SimplexTexture::_on_color_ramp_changed() {
    dirty = true;
    emit_changed();
}

Ref<Image> SimplexTexture::get_image() const {
    const_cast<SimplexTexture*>(this)->_update_texture();
    return ImageTexture::get_image();
}

// Property accessors (unchanged)
void SimplexTexture::set_noise(const Ref<Simplex> &p_noise) {
    if (noise != p_noise) {
        if (noise.is_valid()) {
            noise->disconnect("changed", Callable(this, "_on_noise_changed"));
        }
        noise = p_noise;
        if (noise.is_valid()) {
            noise->connect("changed", Callable(this, "_on_noise_changed"));
        }
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

Ref<Simplex> SimplexTexture::get_noise() const {
    return noise;
}

void SimplexTexture::set_width(int p_width) {
    p_width = MAX(1, p_width);
    if (width != p_width) {
        width = p_width;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

int SimplexTexture::get_width() const {
    return width;
}

void SimplexTexture::set_height(int p_height) {
    p_height = MAX(1, p_height);
    if (height != p_height) {
        height = p_height;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

int SimplexTexture::get_height() const {
    return height;
}

void SimplexTexture::set_invert(bool p_invert) {
    if (invert != p_invert) {
        invert = p_invert;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

bool SimplexTexture::get_invert() const {
    return invert;
}

void SimplexTexture::set_in_3d_space(bool p_enabled) {
    if (in_3d_space != p_enabled) {
        in_3d_space = p_enabled;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

bool SimplexTexture::get_in_3d_space() const {
    return in_3d_space;
}

void SimplexTexture::set_normalize(bool p_normalize) {
    if (normalize != p_normalize) {
        normalize = p_normalize;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

bool SimplexTexture::get_normalize() const {
    return normalize;
}

void SimplexTexture::set_seamless(bool p_seamless) {
    if (seamless != p_seamless) {
        seamless = p_seamless;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

bool SimplexTexture::get_seamless() const {
    return seamless;
}

void SimplexTexture::set_seamless_blend_skirt(float p_skirt) {
    p_skirt = CLAMP(p_skirt, 0.0f, 0.5f);
    if (seamless_blend_skirt != p_skirt) {
        seamless_blend_skirt = p_skirt;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

float SimplexTexture::get_seamless_blend_skirt() const {
    return seamless_blend_skirt;
}

void SimplexTexture::set_color_ramp(const Ref<Gradient> &p_ramp) {
    if (color_ramp != p_ramp) {
        if (color_ramp.is_valid()) {
            color_ramp->disconnect("changed", Callable(this, "_on_color_ramp_changed"));
        }
        color_ramp = p_ramp;
        if (color_ramp.is_valid()) {
            color_ramp->connect("changed", Callable(this, "_on_color_ramp_changed"));
        }
        dirty = true;
        emit_changed();
        call_deferred("_update_texture");
    }
}

Ref<Gradient> SimplexTexture::get_color_ramp() const {
    return color_ramp;
}

void SimplexTexture::set_as_normal_map(bool p_enabled) {
    if (as_normal_map != p_enabled) {
        as_normal_map = p_enabled;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

bool SimplexTexture::get_as_normal_map() const {
    return as_normal_map;
}

void SimplexTexture::set_bump_strength(float p_strength) {
    if (bump_strength != p_strength) {
        bump_strength = p_strength;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

float SimplexTexture::get_bump_strength() const {
    return bump_strength;
}

void SimplexTexture::set_generate_mipmaps(bool p_enabled) {
    if (generate_mipmaps != p_enabled) {
        generate_mipmaps = p_enabled;
        dirty = true;
        emit_changed();
        _update_texture();
    }
}

bool SimplexTexture::get_generate_mipmaps() const {
    return generate_mipmaps;
}

void SimplexTexture::regenerate() {
    dirty = true;
    _update_texture();
}

void SimplexTexture::mark_dirty() {
    dirty = true;
}

} // namespace godot