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

namespace Utils
{

class Random
{
public:
    static void Init();

    // Returns a random integer in [min,max].
    static int Int(int min, int max);

    static uint32_t UInt();
    static uint32_t UInt(uint32_t min, uint32_t max);

    // Returns a random float in [0,1).
    static float Float();
    static float Float(float min, float max);

    // Returns a random double in [0,1).
    static double Double();
    static double Double(double min, double max);

private:
    static thread_local std::minstd_rand                                             s_RandomEngine;
    static thread_local std::uniform_int_distribution<std::minstd_rand::result_type> s_RandomDistribution;
};

}    // namespace Utils
