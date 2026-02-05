/**
 * @file    SimplexNoise.h
 * @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
 *
 * Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <cstddef>  // size_t
#include <cstdint>   // int32_t

/**
 * @brief A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
 */
class SimplexNoise {
public:
    // 1D Perlin simplex noise
    static float noise(float x, int32_t seed);
    // 2D Perlin simplex noise
    static float noise(float x, float y, int32_t seed);
    // 3D Perlin simplex noise
    static float noise(float x, float y, float z, int32_t seed);

    static float Lerp(float a, float b, float t) { return a + t * (b - a); }
    static float FastAbs(float f) { return f < 0 ? -f : f; }
    template <typename FNfloat>
    static int FastFloor(FNfloat f) { return f >= 0 ? (int)f : (int)f - 1; }
    static float PingPong(float t) 
    {
        t -= (int)(t * 0.5f) * 2;
        return t < 1 ? t : 2 - t;
    }

    // Fractal/Fractional Brownian Motion (fBm) noise summation
    float fractal(size_t octaves, float x, int32_t seed) const;
    float fractal(size_t octaves, float x, float y, int32_t seed) const;
    float fractal(size_t octaves, float x, float y, float z, int32_t seed) const;

    // Ridged Noise
    float ridged(size_t octaves, float x, float y, int32_t seed) const;
    float ridged(size_t octaves, float x, float y, float z, int32_t seed) const;

    // Ping-Pong Noise
    float pingpong(size_t octaves, float x, float y, int32_t seed) const;
    float pingpong(size_t octaves, float x, float y, float z, int32_t seed) const;


    /**
     * Constructor of to initialize a fractal noise summation
     *
     * @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
     * @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
     * @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
     * @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
     */
    explicit SimplexNoise(float frequency = 1.0f,
                          float amplitude = 1.0f,
                          float lacunarity = 2.0f,
                          float persistence = 0.5f,
                          float pingpongStrength = 2.0f) :
        mFrequency(frequency),
        mAmplitude(amplitude),
        mLacunarity(lacunarity),
        mPersistence(persistence),
        mPingPongStrength(pingpongStrength) {
    }

    // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
    float mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
    float mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
    float mPingPongStrength;

private:
    float calcFractalBounding(size_t octaves) const;
};
