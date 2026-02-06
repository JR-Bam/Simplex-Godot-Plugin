#pragma once

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/global_constants.hpp>
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

        float get_noise_1d(float p_x) const;
        float get_noise_2d(float p_x, float p_y) const;
        float get_noise_2dv(const Vector2 &p_v) const;
        float get_noise_3d(float p_x, float p_y, float p_z) const;
        float get_noise_3dv(const Vector3 &p_v) const;
        Simplex() : seed{0}, octaves{5}, frequency{.5f}, lacunarity{2.f}, gain{.5f}, pingPongStrength{2.f},
            type{FRACTAL_NONE}, noise{std::make_unique<SimplexNoise>()} {}
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
    private:
        std::unique_ptr<SimplexNoise> noise;
        int32_t seed;
        uint16_t octaves;
        float frequency;
        float lacunarity;
        float gain;
        float pingPongStrength;
        FractalType type;

        Ref<ImageTexture> preview_cache; 
        void _update_preview(); // Helper to refresh the cache
    };
} // namespace godot

VARIANT_ENUM_CAST(Simplex::FractalType);
