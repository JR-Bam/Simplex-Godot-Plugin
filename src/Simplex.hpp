#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <type_traits>

#include "lib/SimplexNoise.h"
#include <memory>

namespace godot
{
    class Simplex : public Resource {
        GDCLASS(Simplex, Resource)

    protected:
        static void _bind_methods();
        void _get_property_list(List<PropertyInfo> *p_list) const;
        bool _property_can_revert(const StringName &p_property) const;
        bool _property_get_revert(const StringName &p_property, Variant &r_ret) const;
        bool _set(const StringName &p_name, const Variant &p_value);
        bool _get(const StringName &p_name, Variant &r_ret) const;

    public:
        enum FractalType {
            FRACTAL_NONE = 0,
            FRACTAL_FBM = 1,
            FRACTAL_RIDGED = 2,
            FRACTAL_PING_PONG = 3,
        };

        enum DomainWarpType {
            DOMAIN_WARP_SIMPLEX = 0,
        };

        enum DomainWarpFractalType {
            DOMAIN_WARP_FRACTAL_NONE = 0,
            DOMAIN_WARP_FRACTAL_PROGRESSIVE = 1,
            DOMAIN_WARP_FRACTAL_INDEPENDENT = 2,
        };

        float get_noise_1d(float p_x) const;
        float get_noise_2d(float p_x, float p_y) const;
        float get_noise_2dv(const Vector2 &p_v) const;
        float get_noise_3d(float p_x, float p_y, float p_z) const;
        float get_noise_3dv(const Vector3 &p_v) const;
        Ref<Image> get_image(int32_t p_width, int32_t p_height, bool p_invert = false, bool p_in_3d_space = false, bool p_normalize = true) const;
        Ref<Image> get_seamless_image(int32_t p_width, int32_t p_height, bool p_invert = false, bool p_in_3d_space = false, float p_skirt = 0.1, bool p_normalize = true) const;
        TypedArray<Image> get_image_3d(int32_t p_width, int32_t p_height, int32_t p_depth, bool p_invert = false, bool p_normalize = true) const;
        TypedArray<Image> get_seamless_image_3d(int32_t p_width, int32_t p_height, int32_t p_depth, bool p_invert = false, float p_skirt = 0.1, bool p_normalize = true) const;
        Simplex() : domain_warp_enabled(false), domain_warp_type(DOMAIN_WARP_SIMPLEX),
            domain_warp_fractal_type(DOMAIN_WARP_FRACTAL_PROGRESSIVE),
            type(FRACTAL_NONE), noise(std::make_unique<SimplexNoise>()) {}
        ~Simplex() {};

        // Property getters setters
        void set_seed(int32_t seed);
        int32_t get_seed();
        void set_frequency(float frequency);
        float get_frequency();
        void set_lacunarity(float lacunarity);
        float get_lacunarity();
        void set_gain(float gain);
        float get_gain();
        void set_ping_pong_strength(float ping_pong_strength);
        float get_ping_pong_strength();
        void set_octaves(uint16_t octaves);
        uint16_t get_octaves();
        void set_fractal_type(FractalType fractal_type);
        FractalType get_fractal_type();

        // Domain Warp properties
        void set_domain_warp_enabled(bool enabled);
        bool get_domain_warp_enabled();
        void set_domain_warp_type(DomainWarpType type);
        DomainWarpType get_domain_warp_type();
        void set_domain_warp_amplitude(float amplitude);
        float get_domain_warp_amplitude();
        void set_domain_warp_frequency(float frequency);
        float get_domain_warp_frequency();
        void set_domain_warp_fractal_type(DomainWarpFractalType fractal_type);
        DomainWarpFractalType get_domain_warp_fractal_type();
        void set_domain_warp_octaves(uint16_t octaves);
        uint16_t get_domain_warp_octaves();
        void set_domain_warp_lacunarity(float lacunarity);
        float get_domain_warp_lacunarity();
        void set_domain_warp_gain(float gain);
        float get_domain_warp_gain();
    private:
        std::unique_ptr<SimplexNoise> noise;
        FractalType type;

        // Domain Warp properties
        bool domain_warp_enabled;
        DomainWarpType domain_warp_type;
        DomainWarpFractalType domain_warp_fractal_type;

        // Helper methods
        void _apply_domain_warp_2d(float& x, float& y) const;
        void _apply_domain_warp_3d(float& x, float& y, float& z) const;

        Ref<ImageTexture> preview_cache; 
        void _update_preview(); // Helper to refresh the cache
    };
} // namespace godot

VARIANT_ENUM_CAST(Simplex::FractalType);
VARIANT_ENUM_CAST(Simplex::DomainWarpType);
VARIANT_ENUM_CAST(Simplex::DomainWarpFractalType);
