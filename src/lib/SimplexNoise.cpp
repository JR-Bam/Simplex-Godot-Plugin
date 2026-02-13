/**
 * @file    SimplexNoise.cpp
 * @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
 *
 * Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * This C++ implementation is based on the speed-improved Java version 2012-03-09
 * by Stefan Gustavson (original Java source code in the public domain).
 * http://webstaff.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java:
 * - Based on example code by Stefan Gustavson (stegu@itn.liu.se).
 * - Optimisations by Peter Eastman (peastman@drizzle.stanford.edu).
 * - Better rank ordering method by Stefan Gustavson in 2012.
 *
 * This implementation is "Simplex Noise" as presented by
 * Ken Perlin at a relatively obscure and not often cited course
 * session "Real-Time Shading" at Siggraph 2001 (before real
 * time shading actually took on), under the title "hardware noise".
 * The 3D function is numerically equivalent to his Java reference
 * code available in the PDF course notes, although I re-implemented
 * it from scratch to get more readable code. The 1D, 2D and 4D cases
 * were implemented from scratch by me from Ken Perlin's text.
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */

#include "SimplexNoise.h"

#include <cstdint>  // int32_t/uint8_t
#include <cmath>

/**
 * Computes the largest integer value not greater than the float one
 *
 * This method is faster than using (int32_t)std::floor(fp).
 *
 * I measured it to be approximately twice as fast:
 *  float:  ~18.4ns instead of ~39.6ns on an AMD APU),
 *  double: ~20.6ns instead of ~36.6ns on an AMD APU),
 * Reference: http://www.codeproject.com/Tips/700780/Fast-floor-ceiling-functions
 *
 * @param[in] fp    float input value
 *
 * @return largest integer value not greater than fp
 */
static inline int32_t fastfloor(float fp) {
    int32_t i = static_cast<int32_t>(fp);
    return (fp < i) ? (i - 1) : (i);
}

/**
 * Permutation table. This is just a random jumble of all numbers 0-255.
 *
 * This produce a repeatable pattern of 256, but Ken Perlin stated
 * that it is not a problem for graphic texture as the noise features disappear
 * at a distance far enough to be able to see a repeatable pattern of 256.
 *
 * This needs to be exactly the same for all instances on all platforms,
 * so it's easiest to just keep it as static explicit data.
 * This also removes the need for any initialisation of this class.
 *
 * Note that making this an uint32_t[] instead of a uint8_t[] might make the
 * code run faster on platforms with a high penalty for unaligned single
 * byte addressing. Intel x86 is generally single-byte-friendly, but
 * some other CPUs are faster with 4-aligned reads.
 * However, a char[] is smaller, which avoids cache trashing, and that
 * is probably the most important aspect on most architectures.
 * This array is accessed a *lot* by the noise functions.
 * A vector-valued noise over 3D accesses it 96 times, and a
 * float-valued 4D noise 64 times. We want this to fit in the cache!
 */
static const uint8_t perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

/**
 * Helper function to hash an integer using the above permutation table
 *
 *  This inline function costs around 1ns, and is called N+1 times for a noise of N dimension.
 *
 *  Using a real hash function would be better to improve the "repeatability of 256" of the above permutation table,
 * but fast integer Hash functions uses more time and have bad random properties.
 *
 * @param[in] i    Integer value to hash
 * @param[in] seed Seed value for hash variation
 *
 * @return 8-bits hashed value
 */
static inline uint8_t hash(int32_t i, int32_t seed) {
    uint32_t h = static_cast<uint32_t>(i) ^ static_cast<uint32_t>(seed);
    h = h * 747796405u + 2891336453u;  // PCG constants
    h ^= h >> 16;
    h ^= h >> 8;  // Better distribution for perm table
    return perm[h & 0xFF];
}

/* NOTE Gradient table to test if lookup-table are more efficient than calculs
static const float gradients1D[16] = {
        -8.f, -7.f, -6.f, -5.f, -4.f, -3.f, -2.f, -1.f,
         1.f,  2.f,  3.f,  4.f,  5.f,  6.f,  7.f,  8.f
};
*/

static inline void grad_for_domain_warp(int seed, int hashval, float x, float y, float& wx, float& wy) {
    // Get gradient vector from hash (like in the regular grad() function)
    const int32_t h = hashval & 0x3F;
    const float u = h < 4 ? x : y;
    const float v = h < 4 ? y : x;
    float gx = ((h & 1) ? -u : u);
    float gy = ((h & 2) ? -2.0f * v : 2.0f * v);
    
    // The warp vector IS the gradient, not a random rotated version
    wx = gx;
    wy = gy;
}

static inline void grad_for_domain_warp(int seed, int hashval, float x, float y, float z, float& wx, float& wy, float& wz) {
    int h = hashval & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    
    wx = ((h & 1) ? -u : u);
    wy = ((h & 2) ? -v : v);
    wz = 0.0f;
    
    // Handle cases where we need Z component
    if (h >= 8) {
        if (h == 8 || h == 10 || h == 12 || h == 14) {
            wz = ((h & 1) ? -x : x);
        } else {
            wz = ((h & 2) ? -y : y);
        }
    }
}

/**
 * Helper function to compute gradients-dot-residual vectors (1D)
 *
 * @note that these generate gradients of more than unit length. To make
 * a close match with the value range of classic Perlin noise, the final
 * noise values need to be rescaled to fit nicely within [-1,1].
 * (The simplex noise functions as such also have different scaling.)
 * Note also that these noise functions are the most practical and useful
 * signed version of Perlin noise.
 *
 * @param[in] hash  hash value
 * @param[in] x     distance to the corner
 *
 * @return gradient value
 */
static float grad(int32_t hash, float x) {
    const int32_t h = hash & 0x0F;  // Convert low 4 bits of hash code
    float grad = 1.0f + (h & 7);    // Gradient value 1.0, 2.0, ..., 8.0
    if ((h & 8) != 0) grad = -grad; // Set a random sign for the gradient
//  float grad = gradients1D[h];    // NOTE : Test of Gradient look-up table instead of the above
    return (grad * x);              // Multiply the gradient with the distance
}

/**
 * Helper functions to compute gradients-dot-residual vectors (2D)
 *
 * @param[in] hash  hash value
 * @param[in] x     x coord of the distance to the corner
 * @param[in] y     y coord of the distance to the corner
 *
 * @return gradient value
 */
static float grad(int32_t hash, float x, float y) {
    const int32_t h = hash & 0x3F;  // Convert low 3 bits of hash code
    const float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    const float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
}

/**
 * Helper functions to compute gradients-dot-residual vectors (3D)
 *
 * @param[in] hash  hash value
 * @param[in] x     x coord of the distance to the corner
 * @param[in] y     y coord of the distance to the corner
 * @param[in] z     z coord of the distance to the corner
 *
 * @return gradient value
 */
static float grad(int32_t hash, float x, float y, float z) {
    int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
    float u = h < 8 ? x : y; // gradient directions, and compute dot product.
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

/**
 * 1D Perlin simplex noise
 *
 *  Takes around 74ns on an AMD APU.
 *
 * @param[in] x    float coordinate
 * @param[in] seed Seed value for noise variation (enables reproducible different noise patterns)
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float SimplexNoise::noise(float x, int32_t seed) {
    float n0, n1;   // Noise contributions from the two "corners"

    // No need to skew the input space in 1D

    // Corners coordinates (nearest integer values):
    int32_t i0 = fastfloor(x);
    int32_t i1 = i0 + 1;
    // Distances to corners (between 0 and 1):
    float x0 = x - i0;
    float x1 = x0 - 1.0f;

    // Calculate the contribution from the first corner
    float t0 = 1.0f - x0*x0;
//  if(t0 < 0.0f) t0 = 0.0f; // not possible
    t0 *= t0;
    n0 = t0 * t0 * grad(hash(i0, seed), x0);

    // Calculate the contribution from the second corner
    float t1 = 1.0f - x1*x1;
//  if(t1 < 0.0f) t1 = 0.0f; // not possible
    t1 *= t1;
    n1 = t1 * t1 * grad(hash(i1, seed), x1);

    // The maximum value of this noise is 8*(3/4)^4 = 2.53125
    // A factor of 0.395 scales to fit exactly within [-1,1]
    return 0.395f * (n0 + n1);
}

/**
 * 2D Perlin simplex noise
 *
 *  Takes around 150ns on an AMD APU.
 *
 * @param[in] x    float coordinate
 * @param[in] y    float coordinate
 * @param[in] seed Seed value for noise variation (enables reproducible different noise patterns)
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float SimplexNoise::noise(float x, float y, int32_t seed) {
    float n0, n1, n2;   // Noise contributions from the three corners

    // Skewing/Unskewing factors for 2D
    static const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
    static const float G2 = 0.211324865f;  // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

    // Skew the input space to determine which simplex cell we're in
    const float s = (x + y) * F2;  // Hairy factor for 2D
    const float xs = x + s;
    const float ys = y + s;
    const int32_t i = fastfloor(xs);
    const int32_t j = fastfloor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0;  // The x,y distances from the cell origin
    const float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    } else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = hash(i + hash(j, seed), seed);
    const int gi1 = hash(i + i1 + hash(j + j1, seed), seed);
    const int gi2 = hash(i + 1 + hash(j + 1, seed), seed);

    // Calculate the contribution from the first corner
    float t0 = 0.5f - x0*x0 - y0*y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1*x1 - y1*y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2*x2 - y2*y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}


/**
 * 3D Perlin simplex noise
 *
 * @param[in] x    float coordinate
 * @param[in] y    float coordinate
 * @param[in] z    float coordinate
 * @param[in] seed Seed value for noise variation (enables reproducible different noise patterns)
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float SimplexNoise::noise(float x, float y, float z, int32_t seed) {
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skewing/Unskewing factors for 3D
    static const float F3 = 1.0f / 3.0f;
    static const float G3 = 1.0f / 6.0f;

    // Skew the input space to determine which simplex cell we're in
    float s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
    int i = fastfloor(x + s);
    int j = fastfloor(y + s);
    int k = fastfloor(z + s);
    float t = (i + j + k) * G3;
    float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0; // The x,y,z distances from the cell origin
    float y0 = y - Y0;
    float z0 = z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
        } else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
        } else {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
        }
    } else { // x0<y0
        if (y0 < z0) {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
        } else if (x0 < z0) {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
        } else {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = hash(i + hash(j + hash(k, seed), seed), seed);
    int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1, seed), seed), seed);
    int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2, seed), seed), seed);
    int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1, seed), seed), seed);

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
    if (t0 < 0) {
        n0 = 0.0;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0, z0);
    }
    float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
    if (t1 < 0) {
        n1 = 0.0;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1, z1);
    }
    float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
    if (t2 < 0) {
        n2 = 0.0;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2, z2);
    }
    float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
    if (t3 < 0) {
        n3 = 0.0;
    } else {
        t3 *= t3;
        n3 = t3 * t3 * grad(gi3, x3, y3, z3);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to stay just inside [-1,1]
    return 32.0f*(n0 + n1 + n2 + n3);
}

// void SimplexNoise::transform_domain_warp_coordinate(float &x, float &y)
// {
//     static const float F2 = 0.366025403f;  // F2 = (sqrt(3) - 1) / 2
//     float t = (x + y) * F2;
//     x += t;
//     y += t;
// }

// void SimplexNoise::transform_domain_warp_coordinate(float &x, float &y, float &z)
// {
// }

/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] x       float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float SimplexNoise::fractal(float x, bool single) const {
    float output    = 0.f;
    float denom     = 0.f;
    float frequency = mFrequency;
    float amplitude = mAmplitude;
    int octaves = (single)? 1: mOctaves;
    
    for (size_t i = 0; i < octaves; i++) {
        output += (amplitude * noise(x * frequency, mSeed));
        denom += amplitude;

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return (output / denom);
}

/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 2D Perlin Simplex noise
 *
 * @param[in] octaves number of fraction of noise to sum
 * @param[in] x       x float coordinate
 * @param[in] y       y float coordinate
 * @param[in] seed    Seed value for noise variation (enables reproducible different noise patterns)
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float SimplexNoise::fractal(float x, float y, bool single) const {
    float output = 0.f;
    float denom  = 0.f;
    float frequency = mFrequency;
    float amplitude = mAmplitude;
    int octaves = (single)? 1: mOctaves;
    
    for (size_t i = 0; i < octaves; i++) {
        output += (amplitude * noise(x * frequency, y * frequency, mSeed));
        denom += amplitude;

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return (output / denom);
}

/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 3D Perlin Simplex noise
 *
 * @param[in] x       x float coordinate
 * @param[in] y       y float coordinate
 * @param[in] z       z float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float SimplexNoise::fractal(float x, float y, float z, bool single) const {
    float output = 0.f;
    float denom  = 0.f;
    float frequency = mFrequency;
    float amplitude = mAmplitude;
    int octaves = (single)? 1: mOctaves;

    for (size_t i = 0; i < octaves; i++) {
        output += (amplitude * noise(x * frequency, y * frequency, z * frequency, mSeed));
        denom += amplitude;

        frequency *= mLacunarity;
        amplitude *= mPersistence;
    }

    return (output / denom);
}

float SimplexNoise::ridged(float x, float y) const
{
    float sum = 0;
    float amp = calcFractalBounding();

    for (int i = 0; i < mOctaves; i++)
    {
        float noise = FastAbs(SimplexNoise::noise(x * mFrequency, y * mFrequency, mSeed));
        sum += (noise * -2 + 1) * amp;
        amp *= Lerp(1.0f, 1 - noise, 0.f);

        x *= mLacunarity;
        y *= mLacunarity;
        amp *= mPersistence;
    }

    return sum;
}

float SimplexNoise::ridged(float x, float y, float z) const
{
    float sum = 0;
    float amp = calcFractalBounding();

    for (int i = 0; i < mOctaves; i++)
    {
        float noise = FastAbs(SimplexNoise::noise(x * mFrequency, y * mFrequency, z * mFrequency, mSeed));
        sum += (noise * -2 + 1) * amp;
        amp *= Lerp(1.0f, 1 - noise, 0.f);

        x *= mLacunarity;
        y *= mLacunarity;
        z *= mLacunarity;
        amp *= mPersistence;
    }

    return sum;
}

float SimplexNoise::pingpong(float x, float y) const
{
    float sum = 0;
    float amp = calcFractalBounding();

    for (int i = 0; i < mOctaves; i++)
    {
        float noise = PingPong((SimplexNoise::noise(x * mFrequency, y * mFrequency, mSeed) + 1) * mPingPongStrength);
        sum += (noise - 0.5f) * 2 * amp;
        amp *= Lerp(1.0f, noise, 0.f);

        x *= mLacunarity;
        y *= mLacunarity;
        amp *= mPersistence;
    }

    return sum;
}

float SimplexNoise::pingpong(float x, float y, float z) const
{
    float sum = 0;
    float amp = calcFractalBounding();

    for (int i = 0; i < mOctaves; i++)
    {
        float noise = PingPong((SimplexNoise::noise(x * mFrequency, y * mFrequency, z * mFrequency, mSeed) + 1) * mPingPongStrength);
        sum += (noise - 0.5f) * 2 * amp;
        amp *= Lerp(1.0f, noise, 0.f);

        x *= mLacunarity;
        y *= mLacunarity;
        z *= mLacunarity;
        amp *= mPersistence;
    }

    return sum;
}

void SimplexNoise::single_domain_warp_gradient(float warpAmp, float x, float y, float &xr, float &yr) const
{
    warpAmp *= calcFractalBounding() * 38.283687591552734375f;
    x *= mDomainWarpFrequency;
    y *= mDomainWarpFrequency;

    static const float F2 = 0.366025403f;
    float s = (x + y) * F2;
    float xs = x + s;
    float ys = y + s;

    static const float G2 = 0.211324865f;

    const int32_t i = fastfloor(xs);
    const int32_t j = fastfloor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = static_cast<float>(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = x - X0;  // The x,y distances from the cell origin
    const float y0 = y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int32_t i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    } else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = hash(i + hash(j, mSeed), mSeed);
    const int gi1 = hash(i + i1 + hash(j + j1, mSeed), mSeed);
    const int gi2 = hash(i + 1 + hash(j + 1, mSeed), mSeed);

    float vx = 0.0f, vy = 0.0f;  // Accumulated warp vector
    
    // Process each simplex corner
    // Corner 0
    float t0 = 0.5f - x0*x0 - y0*y0;
    if (t0 > 0.0f) {
        t0 *= t0;
        float influence = t0 * t0;  // t0⁴ (smooth falloff)
        
        // Get warp vector for this corner
        float wx0, wy0;
        grad_for_domain_warp(mSeed, gi0, x0, y0, wx0, wy0);
        
        vx += influence * wx0;
        vy += influence * wy0;
    }
    
    // Corner 1
    float t1 = 0.5f - x1*x1 - y1*y1;
    if (t1 > 0.0f) {
        t1 *= t1;
        float influence = t1 * t1;
        
        float wx1, wy1;
        grad_for_domain_warp(mSeed, gi1, x1, y1, wx1, wy1);
        
        vx += influence * wx1;
        vy += influence * wy1;
    }
    
    // Corner 2
    float t2 = 0.5f - x2*x2 - y2*y2;
    if (t2 > 0.0f) {
        t2 *= t2;
        float influence = t2 * t2;
        
        float wx2, wy2;
        grad_for_domain_warp(mSeed, gi2, x2, y2, wx2, wy2);
        
        vx += influence * wx2;
        vy += influence * wy2;
    }
    
    // Apply the accumulated warp
    xr += vx * warpAmp;
    yr += vy * warpAmp;
}

void SimplexNoise::single_domain_warp_gradient(float warpAmp, float x, float y, float z, float &xr, float &yr, float &zr) const
{
    warpAmp *= calcFractalBounding() * 38.283687591552734375f;
    x *= mDomainWarpFrequency;
    y *= mDomainWarpFrequency;
    z *= mDomainWarpFrequency;

    static const float F3 = 1.0f / 3.0f;
    static const float G3 = 1.0f / 6.0f;

    float s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
    int i = fastfloor(x + s);
    int j = fastfloor(y + s);
    int k = fastfloor(z + s);
    float t = (i + j + k) * G3;
    float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = x - X0; // The x,y,z distances from the cell origin
    float y0 = y - Y0;
    float z0 = z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
        } else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
        } else {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
        }
    } else { // x0<y0
        if (y0 < z0) {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
        } else if (x0 < z0) {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
        } else {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = hash(i + hash(j + hash(k, mSeed), mSeed), mSeed);
    int gi1 = hash(i + i1 + hash(j + j1 + hash(k + k1, mSeed), mSeed), mSeed);
    int gi2 = hash(i + i2 + hash(j + j2 + hash(k + k2, mSeed), mSeed), mSeed);
    int gi3 = hash(i + 1 + hash(j + 1 + hash(k + 1, mSeed), mSeed), mSeed);
    
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    // Corner 0
    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
    if (t0 > 0.0f) {
        t0 *= t0;
        float influence = t0 * t0;
        float wx0, wy0, wz0;
        grad_for_domain_warp(mSeed, gi0, x0, y0, z0, wx0, wy0, wz0);
        vx += influence * wx0;
        vy += influence * wy0;
        vz += influence * wz0;
    }
    
    // Corner 1
    float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
    if (t1 > 0.0f) {
        t1 *= t1;
        float influence = t1 * t1;
        float wx1, wy1, wz1;
        grad_for_domain_warp(mSeed, gi1, x1, y1, z1, wx1, wy1, wz1);
        vx += influence * wx1;
        vy += influence * wy1;
        vz += influence * wz1;
    }
    
    // Corner 2
    float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
    if (t2 > 0.0f) {
        t2 *= t2;
        float influence = t2 * t2;
        float wx2, wy2, wz2;
        grad_for_domain_warp(mSeed, gi2, x2, y2, z2, wx2, wy2, wz2);
        vx += influence * wx2;
        vy += influence * wy2;
        vz += influence * wz2;
    }
    
    // Corner 3
    float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
    if (t3 > 0.0f) {
        t3 *= t3;
        float influence = t3 * t3;
        float wx3, wy3, wz3;
        grad_for_domain_warp(mSeed, gi3, x3, y3, z3, wx3, wy3, wz3);
        vx += influence * wx3;
        vy += influence * wy3;
        vz += influence * wz3;
    }
    
    // Apply the accumulated warp
    xr += vx * warpAmp;
    yr += vy * warpAmp;
    zr += vz * warpAmp;
}

void SimplexNoise::progressive_domain_warp_fractal(float &x, float &y) const
{
    float current_x = x;
    float current_y = y;
    float amp = mDomainWarpAmplitude;
    float freq = mDomainWarpFrequency;
    
    for (size_t i = 0; i < mDomainWarpFractalOctaves; i++) {
        // Use the CURRENT position for warping (this is "progressive")
        single_domain_warp_gradient(amp, current_x * freq, current_y * freq, x, y);
        
        // Update position for next octave using the warped coordinates
        current_x = x;
        current_y = y;
        
        amp *= mDomainWarpFractalGain;
        freq *= mDomainWarpFractalLacunarity;
    }
}

void SimplexNoise::independent_domain_warp_fractal(float &x, float &y) const
{
    float amp = mDomainWarpAmplitude;
    float freq = mDomainWarpFrequency;
    float dx = 0, dy = 0;  // Accumulate warp independently
    
    for (size_t i = 0; i < mDomainWarpFractalOctaves; i++) {
        // Each octave warps the ORIGINAL coordinates independently
        single_domain_warp_gradient(amp, x * freq, y * freq, dx, dy);
        
        amp *= mDomainWarpFractalGain;
        freq *= mDomainWarpFractalLacunarity;
    }
    
    x += dx;
    y += dy;
}

void SimplexNoise::progressive_domain_warp_fractal(float &x, float &y, float &z) const
{
    float current_x = x;
    float current_y = y;
    float current_z = z;
    float amp = mDomainWarpAmplitude;
    float freq = mDomainWarpFrequency;
    
    for (size_t i = 0; i < mDomainWarpFractalOctaves; i++) {
        single_domain_warp_gradient(amp, current_x * freq, current_y * freq, current_z * freq, x, y, z);
        
        current_x = x;
        current_y = y;
        current_z = z;
        
        amp *= mDomainWarpFractalGain;
        freq *= mDomainWarpFractalLacunarity;
    }
}

void SimplexNoise::independent_domain_warp_fractal(float &x, float &y, float &z) const
{
    float amp = mDomainWarpAmplitude;
    float freq = mDomainWarpFrequency;
    float dx = 0, dy = 0, dz = 0;
    
    for (size_t i = 0; i < mDomainWarpFractalOctaves; i++) {
        single_domain_warp_gradient(amp, x * freq, y * freq, z * freq, dx, dy, dz);
        
        amp *= mDomainWarpFractalGain;
        freq *= mDomainWarpFractalLacunarity;
    }
    
    x += dx;
    y += dy;
    z += dz;
}

float SimplexNoise::calcFractalBounding() const
{
    float gain = FastAbs(mPersistence);
    float amp = gain;
    float ampFractal = 1.0f;
    for (int i = 1; i < mOctaves; i++)
    {
        ampFractal += amp;
        amp *= gain;
    }
    return 1 / ampFractal;
}

/**
 * Domain Warp 2D using fractal Simplex noise
 * Matches FastNoiseLite's approach with multiple octaves and axis decorrelation
 *
 * @param[in,out] x    x coordinate to be warped
 * @param[in,out] y    y coordinate to be warped
 * @param[in] seed     Seed value for noise variation
 * @param[in] octaves  Number of octaves to use in the warp (default = 3)
 */
void SimplexNoise::domainWarp2D(float& x, float& y, int32_t seed, size_t octaves) const
{
    float freq = mFrequency;
    float amp  = mAmplitude;

    float origX = x;
    float origY = y;

    for (size_t i = 0; i < octaves; i++) {
        // Sample noise for x and y axes separately with offsets for decorrelation
        float nx = noise(origX * freq, origY * freq, seed + int(i * 100)) * amp;
        float ny = noise((origX + 31.0f) * freq, (origY + 47.0f) * freq, seed + int(i * 100)) * amp;

        x += nx;
        y += ny;

        freq *= mLacunarity;
        amp  *= mPersistence;
    }
}

/**
 * Domain Warp 3D using fractal Simplex noise
 * Matches FastNoiseLite's approach with multiple octaves and axis decorrelation
 *
 * @param[in,out] x    x coordinate to be warped
 * @param[in,out] y    y coordinate to be warped
 * @param[in,out] z    z coordinate to be warped
 * @param[in] seed     Seed value for noise variation
 * @param[in] octaves  Number of octaves to use in the warp (default = 3)
 */
void SimplexNoise::domainWarp3D(float& x, float& y, float& z, int32_t seed, size_t octaves) const
{
    float freq = mFrequency;
    float amp  = mAmplitude;

    float origX = x;
    float origY = y;
    float origZ = z;

    for (size_t i = 0; i < octaves; i++) {
        // Sample noise for x, y, z axes separately with offsets for decorrelation
        float nx = noise(origX * freq, origY * freq, origZ * freq, seed + int(i * 100)) * amp;
        float ny = noise((origX + 31.0f) * freq, (origY + 47.0f) * freq, (origZ + 59.0f) * freq, seed + int(i * 100)) * amp;
        float nz = noise((origX + 71.0f) * freq, (origY + 97.0f) * freq, (origZ + 101.0f) * freq, seed + int(i * 100)) * amp;

        x += nx;
        y += ny;
        z += nz;

        freq *= mLacunarity;
        amp  *= mPersistence;
    }
}
