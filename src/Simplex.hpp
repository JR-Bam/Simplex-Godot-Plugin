#pragma once

#include <godot_cpp/classes/noise.hpp>
#include "lib/SimplexNoise.h"
#include <memory>

namespace godot
{
    class Simplex : public Noise {
        GDCLASS(Simplex, Noise)

    private:
        std::unique_ptr<SimplexNoise> noise;
    
    public:
        // float get_noise_1d(float p_x) const override;
        // float get_noise_2d(float p_x, float p_y) const;
        // float get_noise_2dv(const Vector2 &p_v) const;
        // float get_noise_3d(float p_x, float p_y, float p_z) const;
        // float get_noise_3dv(const Vector3 &p_v) const;
        Simplex();
        ~Simplex();
    };
} // namespace godot
