#pragma once

#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/gradient.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "Simplex.hpp"

namespace godot {

class SimplexTexture : public Texture2D {
    GDCLASS(SimplexTexture, Texture2D)

private:
    // Core noise
    Ref<Simplex> noise;
    
    // Texture generation parameters
    int width;
    int height;
    bool invert;
    bool in_3d_space;
    bool normalize;
    bool seamless;
    float seamless_blend_skirt;
    Ref<Gradient> color_ramp;
    bool as_normal_map;
    float bump_strength;
    bool generate_mipmaps;
    
    // Cached data
    Ref<ImageTexture> cached_texture;
    bool dirty;
    
    void _update_texture();
    void _on_noise_changed();
    void _on_color_ramp_changed();

protected:
    static void _bind_methods();
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<PropertyInfo> *p_list) const;

public:
    SimplexTexture();
    ~SimplexTexture();

    // Texture2D virtual overrides
    virtual int32_t _get_width() const override;
    virtual int32_t _get_height() const override;
    virtual bool _is_pixel_opaque(int32_t p_x, int32_t p_y) const override;
    virtual bool _has_alpha() const override;
    virtual void _draw(const RID &p_to_canvas_item, const Vector2 &p_pos, const Color &p_modulate, bool p_transpose) const override;
    virtual void _draw_rect(const RID &p_to_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const override;
    virtual void _draw_rect_region(const RID &p_to_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const override;

    // Property accessors
    void set_noise(const Ref<Simplex> &p_noise);
    Ref<Simplex> get_noise() const;
    
    void set_width(int p_width);
    int get_width() const;
    
    void set_height(int p_height);
    int get_height() const;
    
    void set_invert(bool p_invert);
    bool get_invert() const;
    
    void set_in_3d_space(bool p_enabled);
    bool get_in_3d_space() const;
    
    void set_normalize(bool p_normalize);
    bool get_normalize() const;
    
    void set_seamless(bool p_seamless);
    bool get_seamless() const;
    
    void set_seamless_blend_skirt(float p_skirt);
    float get_seamless_blend_skirt() const;
    
    void set_color_ramp(const Ref<Gradient> &p_ramp);
    Ref<Gradient> get_color_ramp() const;
    
    void set_as_normal_map(bool p_enabled);
    bool get_as_normal_map() const;
    
    void set_bump_strength(float p_strength);
    float get_bump_strength() const;
    
    void set_generate_mipmaps(bool p_enabled);
    bool get_generate_mipmaps() const;
    
    // Public utility methods
    void regenerate();
    void mark_dirty();
    
    // Regular method (not virtual override)
    Ref<Image> get_image() const;
};

} // namespace godot