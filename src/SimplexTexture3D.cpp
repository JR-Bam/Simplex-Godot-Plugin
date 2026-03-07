#include "SimplexTexture3D.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/image.hpp>

namespace godot {

SimplexTexture3D::SimplexTexture3D() : 
    width(64),
    height(64),
    depth(64),
    invert(false),
    normalize(true),
    seamless(false),
    seamless_blend_skirt(0.1f),
    current_width(0),
    current_height(0),
    current_depth(0),
    current_format(Image::FORMAT_MAX),
    current_mipmaps(false),
    dirty(true) {
        
        if (noise.is_valid()) {
            noise->connect("changed", Callable(this, "_on_noise_changed"));
        }
    }

SimplexTexture3D::~SimplexTexture3D() {
    if (noise.is_valid()) noise->disconnect("changed", Callable(this, "_on_noise_changed"));
    if (color_ramp.is_valid()) color_ramp->disconnect("changed", Callable(this, "_on_color_ramp_changed"));
}

void SimplexTexture3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_noise", "noise"), &SimplexTexture3D::set_noise);
    ClassDB::bind_method(D_METHOD("get_noise"), &SimplexTexture3D::get_noise);
    ClassDB::bind_method(D_METHOD("set_width", "width"), &SimplexTexture3D::set_width);
    ClassDB::bind_method(D_METHOD("get_width"), &SimplexTexture3D::get_width);
    ClassDB::bind_method(D_METHOD("set_height", "height"), &SimplexTexture3D::set_height);
    ClassDB::bind_method(D_METHOD("get_height"), &SimplexTexture3D::get_height);
    ClassDB::bind_method(D_METHOD("set_depth", "depth"), &SimplexTexture3D::set_depth);
    ClassDB::bind_method(D_METHOD("get_depth"), &SimplexTexture3D::get_depth);
    ClassDB::bind_method(D_METHOD("set_invert", "invert"), &SimplexTexture3D::set_invert);
    ClassDB::bind_method(D_METHOD("get_invert"), &SimplexTexture3D::get_invert);
    ClassDB::bind_method(D_METHOD("set_normalize", "normalize"), &SimplexTexture3D::set_normalize);
    ClassDB::bind_method(D_METHOD("get_normalize"), &SimplexTexture3D::get_normalize);
    ClassDB::bind_method(D_METHOD("set_seamless", "seamless"), &SimplexTexture3D::set_seamless);
    ClassDB::bind_method(D_METHOD("get_seamless"), &SimplexTexture3D::get_seamless);
    ClassDB::bind_method(D_METHOD("set_seamless_blend_skirt", "skirt"), &SimplexTexture3D::set_seamless_blend_skirt);
    ClassDB::bind_method(D_METHOD("get_seamless_blend_skirt"), &SimplexTexture3D::get_seamless_blend_skirt);
    ClassDB::bind_method(D_METHOD("set_color_ramp", "ramp"), &SimplexTexture3D::set_color_ramp);
    ClassDB::bind_method(D_METHOD("get_color_ramp"), &SimplexTexture3D::get_color_ramp);
    
    ClassDB::bind_method(D_METHOD("_on_noise_changed"), &SimplexTexture3D::_on_noise_changed);
    ClassDB::bind_method(D_METHOD("_on_color_ramp_changed"), &SimplexTexture3D::_on_color_ramp_changed);
}

bool SimplexTexture3D::_set(const StringName &p_name, const Variant &p_value) {
    if (p_name == StringName("width")) { set_width(p_value); return true; }
    else if (p_name == StringName("height")) { set_height(p_value); return true; }
    else if (p_name == StringName("depth")) { set_depth(p_value); return true; }
    else if (p_name == StringName("invert")) { set_invert(p_value); return true; }
    else if (p_name == StringName("normalize")) { set_normalize(p_value); return true; }
    else if (p_name == StringName("seamless")) { set_seamless(p_value); return true; }
    else if (p_name == StringName("seamless_blend_skirt")) { set_seamless_blend_skirt(p_value); return true; }
    else if (p_name == StringName("color_ramp")) { set_color_ramp(p_value); return true; }
    else if (p_name == StringName("noise")) { set_noise(p_value); return true; }
    return false;
}

bool SimplexTexture3D::_get(const StringName &p_name, Variant &r_ret) const {
    if (p_name == StringName("width")) { r_ret = width; return true; }
    else if (p_name == StringName("height")) { r_ret = height; return true; }
    else if (p_name == StringName("depth")) { r_ret = depth; return true; }
    else if (p_name == StringName("invert")) { r_ret = invert; return true; }
    else if (p_name == StringName("normalize")) { r_ret = normalize; return true; }
    else if (p_name == StringName("seamless")) { r_ret = seamless; return true; }
    else if (p_name == StringName("seamless_blend_skirt")) { r_ret = seamless_blend_skirt; return true; }
    else if (p_name == StringName("color_ramp")) { r_ret = color_ramp; return true; }
    else if (p_name == StringName("noise")) { r_ret = noise; return true; }
    return false;
}

void SimplexTexture3D::_get_property_list(List<PropertyInfo> *p_list) const {
    p_list->push_back(PropertyInfo(Variant::OBJECT, "noise", PROPERTY_HINT_RESOURCE_TYPE, "Simplex"));
    p_list->push_back(PropertyInfo(Variant::NIL, "Dimensions", PROPERTY_HINT_NONE, "dim_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,4096,1,or_greater"));
    p_list->push_back(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,4096,1,or_greater"));
    p_list->push_back(PropertyInfo(Variant::INT, "depth", PROPERTY_HINT_RANGE, "1,4096,1,or_greater"));
    p_list->push_back(PropertyInfo(Variant::NIL, "Generation", PROPERTY_HINT_NONE, "generation_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::BOOL, "invert"));
    p_list->push_back(PropertyInfo(Variant::BOOL, "normalize"));
    p_list->push_back(PropertyInfo(Variant::NIL, "Seamless", PROPERTY_HINT_NONE, "seamless_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::BOOL, "seamless"));
    p_list->push_back(PropertyInfo(Variant::FLOAT, "seamless_blend_skirt", PROPERTY_HINT_RANGE, "0.0,0.5,0.01"));
    p_list->push_back(PropertyInfo(Variant::NIL, "Color", PROPERTY_HINT_NONE, "color_", PROPERTY_USAGE_GROUP));
    p_list->push_back(PropertyInfo(Variant::OBJECT, "color_ramp", PROPERTY_HINT_RESOURCE_TYPE, "Gradient"));
}

void SimplexTexture3D::_update_texture() {
    if (!dirty) return;
    dirty = false;

    if (noise.is_null()) return;

    // 1. Get the raw slices from your Simplex class
    TypedArray<Image> data_cache;
    if (seamless) {
        data_cache = noise->get_seamless_image_3d(width, height, depth, invert, seamless_blend_skirt, normalize);
    } else {
        data_cache = noise->get_image_3d(width, height, depth, invert, normalize);
    }

    if (data_cache.is_empty()) return;

    // 2. Apply color ramp (mimicking your 2D logic)
    if (color_ramp.is_valid() && color_ramp->get_point_count() > 0) {
        for (int z = 0; z < data_cache.size(); z++) {
            Ref<Image> slice = data_cache[z];
            if (slice.is_valid()) {
                slice->convert(Image::FORMAT_RGBA8); // Force format change
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        Color pixel = slice->get_pixel(x, y);
                        float gray = CLAMP(pixel.r, 0.0f, 1.0f);
                        slice->set_pixel(x, y, color_ramp->sample(gray));
                    }
                }
            }
        }
    }

    // 3. Extract the EXACT properties from the generated images to prevent mismatches
    Ref<Image> first_slice = data_cache[0];
    if (first_slice.is_null()) return;

    int actual_width = first_slice->get_width();
    int actual_height = first_slice->get_height();
    int actual_depth = data_cache.size();
    Image::Format actual_format = first_slice->get_format();
    bool actual_mipmaps = first_slice->has_mipmaps();

    // 4. Mirroring your 2D update/create logic safely
    if (actual_width == current_width && 
        actual_height == current_height && 
        actual_depth == current_depth && 
        actual_format == current_format &&
        actual_mipmaps == current_mipmaps) {
        
        // Compatible -> Safe to update GPU buffer
        
        update(data_cache);
        
    } else {
        
        // Size, format, or depth changed -> Must completely recreate
        
        
        Error err = create(actual_format, actual_width, actual_height, actual_depth, actual_mipmaps, data_cache);
        
        if (err == OK) {
            // Update stored properties so subsequent tweaks (like color) use update()
            current_width = actual_width;
            current_height = actual_height;
            current_depth = actual_depth;
            current_format = actual_format;
            current_mipmaps = actual_mipmaps;
        } else {
            UtilityFunctions::printerr("Failed to create 3D texture. Godot Error: ", err);
        }
    }
}

// -------------------------------------------------------------------------
// Events and Accessors
// -------------------------------------------------------------------------
void SimplexTexture3D::_on_noise_changed() { dirty = true; _update_texture(); emit_changed(); }
void SimplexTexture3D::_on_color_ramp_changed() { dirty = true; _update_texture(); emit_changed(); }

void SimplexTexture3D::set_noise(const Ref<Simplex> &p_noise) {
    if (noise != p_noise) {
        if (noise.is_valid()) noise->disconnect("changed", Callable(this, "_on_noise_changed"));
        noise = p_noise;
        if (noise.is_valid()) noise->connect("changed", Callable(this, "_on_noise_changed"));
        dirty = true;
        _update_texture();
        emit_changed();
    }
}
Ref<Simplex> SimplexTexture3D::get_noise() const { return noise; }

void SimplexTexture3D::set_width(int p_width) {
    p_width = MAX(1, p_width);
    if (width != p_width) { width = p_width; dirty = true; _update_texture(); emit_changed(); }
}
int SimplexTexture3D::get_width() const { return width; }

void SimplexTexture3D::set_height(int p_height) {
    p_height = MAX(1, p_height);
    if (height != p_height) { height = p_height; dirty = true; _update_texture(); emit_changed(); }
}
int SimplexTexture3D::get_height() const { return height; }

void SimplexTexture3D::set_depth(int p_depth) {
    p_depth = MAX(1, p_depth);
    if (depth != p_depth) { depth = p_depth; dirty = true; _update_texture(); emit_changed(); }
}
int SimplexTexture3D::get_depth() const { return depth; }

void SimplexTexture3D::set_invert(bool p_invert) {
    if (invert != p_invert) { invert = p_invert; dirty = true; _update_texture(); emit_changed(); }
}
bool SimplexTexture3D::get_invert() const { return invert; }

void SimplexTexture3D::set_normalize(bool p_normalize) {
    if (normalize != p_normalize) { normalize = p_normalize; dirty = true; _update_texture(); emit_changed(); }
}
bool SimplexTexture3D::get_normalize() const { return normalize; }

void SimplexTexture3D::set_seamless(bool p_seamless) {
    if (seamless != p_seamless) { seamless = p_seamless; dirty = true; _update_texture(); emit_changed(); }
}
bool SimplexTexture3D::get_seamless() const { return seamless; }

void SimplexTexture3D::set_seamless_blend_skirt(float p_skirt) {
    p_skirt = CLAMP(p_skirt, 0.0f, 0.5f);
    if (seamless_blend_skirt != p_skirt) { seamless_blend_skirt = p_skirt; dirty = true; _update_texture(); emit_changed(); }
}
float SimplexTexture3D::get_seamless_blend_skirt() const { return seamless_blend_skirt; }

void SimplexTexture3D::set_color_ramp(const Ref<Gradient> &p_ramp) {
    if (color_ramp != p_ramp) {
        if (color_ramp.is_valid()) color_ramp->disconnect("changed", Callable(this, "_on_color_ramp_changed"));
        color_ramp = p_ramp;
        if (color_ramp.is_valid()) color_ramp->connect("changed", Callable(this, "_on_color_ramp_changed"));
        dirty = true;
        _update_texture();
        emit_changed();
    }
}
Ref<Gradient> SimplexTexture3D::get_color_ramp() const { return color_ramp; }

} // namespace godot