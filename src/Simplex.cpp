#include "Simplex.hpp"
#include <godot_cpp/core/class_db.hpp>
using namespace godot;

float Simplex::get_noise_1d(float p_x) const
{
    return this->noise->noise(p_x);
}

float Simplex::get_noise_2d(float p_x, float p_y) const
{
    return this->noise->noise(p_x, p_y);
}

float Simplex::get_noise_2dv(const Vector2 &p_v) const
{
    return this->noise->noise(p_v.x, p_v.y);
}

float Simplex::get_noise_3d(float p_x, float p_y, float p_z) const
{
    return this->noise->noise(p_x, p_y, p_z);
}

float Simplex::get_noise_3dv(const Vector3 &p_v) const
{
    return this->noise->noise(p_v.x, p_v.y, p_v.z);
}

Simplex::~Simplex()
{
}
