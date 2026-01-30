#pragma once

#include <godot_cpp/classes/resource.hpp>
#include "lib/SimplexNoise.h"
#include <memory>

namespace godot
{
    class Simplex : public Resource {
        GDCLASS(Simplex, Resource)

    private:
        std::unique_ptr<SimplexNoise> noise;
    
    protected:
        static void _bind_methods();

    public:
        float get_noise_1d(float p_x) const;
        float get_noise_2d(float p_x, float p_y) const;
        float get_noise_2dv(const Vector2 &p_v) const;
        float get_noise_3d(float p_x, float p_y, float p_z) const;
        float get_noise_3dv(const Vector3 &p_v) const;
        Simplex();
        ~Simplex();
    };
} // namespace godot
