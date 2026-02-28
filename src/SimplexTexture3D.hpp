#pragma once

#include <godot_cpp/classes/image_texture3d.hpp>
#include <godot_cpp/classes/gradient.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "Simplex.hpp"

namespace godot {

class SimplexTexture3D : public ImageTexture3D {
    GDCLASS(SimplexTexture3D, ImageTexture3D)

private:
    Ref<Simplex> noise;
    
    int width;
    int height;
    int depth;
    bool invert;
    bool normalize;
    bool seamless;
    float seamless_blend_skirt;
    Ref<Gradient> color_ramp;
    
    bool dirty;

    int current_width;
    int current_height;
    int current_depth;
    Image::Format current_format;
    bool current_mipmaps;
    
    void _on_noise_changed();
    void _on_color_ramp_changed();
    void _update_texture();

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    SimplexTexture3D();
    ~SimplexTexture3D();

    void set_noise(const Ref<Simplex> &p_noise);
    Ref<Simplex> get_noise() const;
    
    void set_width(int p_width);
    int get_width() const;
    
    void set_height(int p_height);
    int get_height() const;

    void set_depth(int p_depth);
    int get_depth() const;
    
    void set_invert(bool p_invert);
    bool get_invert() const;
    
    void set_normalize(bool p_normalize);
    bool get_normalize() const;
    
    void set_seamless(bool p_seamless);
    bool get_seamless() const;
    
    void set_seamless_blend_skirt(float p_skirt);
    float get_seamless_blend_skirt() const;
    
    void set_color_ramp(const Ref<Gradient> &p_ramp);
    Ref<Gradient> get_color_ramp() const;
};

} // namespace godot