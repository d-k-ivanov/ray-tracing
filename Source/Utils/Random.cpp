// ReSharper disable CppClangTidyCertMsc51Cpp
#include "Random.h"

thread_local std::minstd_rand                                             Random::s_RandomEngine;
thread_local std::uniform_int_distribution<std::minstd_rand::result_type> Random::s_RandomDistribution;
