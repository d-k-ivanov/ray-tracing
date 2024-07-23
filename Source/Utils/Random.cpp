// ReSharper disable CppClangTidyCertMsc51Cpp
#include "Random.h"

namespace Utils
{

void Random::Init()
{
    s_RandomEngine.seed(std::random_device()());
}

// Returns a random integer in [min,max].
int Random::Int(const int min, const int max)
{
    return static_cast<int>(Double(min, max + 1));
}

uint32_t Random::UInt()
{
    return s_RandomDistribution(s_RandomEngine);
}

uint32_t Random::UInt(const uint32_t min, const uint32_t max)
{
    return min + (s_RandomDistribution(s_RandomEngine) % (max - min + 1));
}

// Returns a random float in [0,1).
float Random::Float()
{
    return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
}

float Random::Float(const float min, const float max)
{
    return min + (max - min) * Float();
}

// Returns a random double in [0,1).
double Random::Double()
{
    return rand() / (RAND_MAX + 1.0);
}

double Random::Double(const double min, const double max)
{
    return min + (max - min) * Double();
}

thread_local std::minstd_rand                                             Random::s_RandomEngine;
thread_local std::uniform_int_distribution<std::minstd_rand::result_type> Random::s_RandomDistribution;

}    // namespace Utils
