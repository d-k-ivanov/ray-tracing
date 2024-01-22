#pragma once

#include <Math/Vector3.h>

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
        return s_RandomDistributionInt(s_RandomEngine);
    }

    static uint32_t UInt(const uint32_t min, const uint32_t max)
    {
        return min + (s_RandomDistributionInt(s_RandomEngine) % (max - min + 1));
    }

    static float Float()
    {
        return static_cast<float>(s_RandomDistributionInt(s_RandomEngine)) / static_cast<float>(std::numeric_limits<uint32_t>::max());
    }

    // Returns a random double in [0,1).
    static double Double()
    {
        std::random_device randDevice;
        std::mt19937 generator(randDevice());
        std::uniform_real_distribution<> distribution(0.0, 1.0);
        return distribution(generator);
    }

    static double Double(const double min, const double max)
    {
        return min + (max - min) * Double();
    }

    static Vector3 Vector3()
    {
        return {Double(), Double(), Double()};
    }

    // Returns a random integer in [min,max].
    static int Int(const int min, const int max)
    {
        return static_cast<int>(Double(min, max + 1));
    }

private:
    static std::mt19937                                             s_RandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_RandomDistributionInt;
};
