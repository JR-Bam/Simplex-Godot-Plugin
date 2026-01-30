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
}

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

Simplex::Simplex()
{
    this->noise = std::unique_ptr<SimplexNoise>();
}

Simplex::~Simplex()
{
}
