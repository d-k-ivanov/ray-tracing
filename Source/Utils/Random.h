#pragma once

// Random numbers are SLOW!!!!
// Mersenne Twister (mt19937) is slow like hell. Stick to rand().
// TODO: Investigate usage of:
// https://www.pcg-random.org/
// https://github.com/Reputeless/Xoshiro-cpp
// https://prng.di.unimi.it/
// #include <pcg_random.hpp>
// #include <XoshiroCpp.hpp>

#include <random>

class Random
{
public:
    static void Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    static uint32_t UInt()
    {
        return s_RandomDistribution(s_RandomEngine);
    }

    static uint32_t UInt(const uint32_t min, const uint32_t max)
    {
        return min + (s_RandomDistribution(s_RandomEngine) % (max - min + 1));
    }

    // Returns a random float in [0,1).
    static float Float()
    {
        return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
    }

    static float Float(const float min, const float max)
    {
        return min + (max - min) * Float();
    }

    // Returns a random double in [0,1).
    static double Double()
    {
        return rand() / (RAND_MAX + 1.0);
    }

    static double Double(const double min, const double max)
    {
        return min + (max - min) * Double();
    }

    // Returns a random integer in [min,max].
    static int Int(const int min, const int max)
    {
        return static_cast<int>(Double(min, max + 1));
    }

private:
    static thread_local std::minstd_rand                                             s_RandomEngine;
    static thread_local std::uniform_int_distribution<std::minstd_rand::result_type> s_RandomDistribution;
};
