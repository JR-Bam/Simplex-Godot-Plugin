#include "Simplex.hpp"

using namespace godot;

Simplex::Simplex()
{
    this->noise = std::unique_ptr<SimplexNoise>();
}

Simplex::~Simplex()
{
}
