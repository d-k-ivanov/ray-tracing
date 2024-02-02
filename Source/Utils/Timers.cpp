#include "Timers.h"

#include <iostream>

Timer::Timer()
{
    Reset();
}

void Timer::Reset()
{
    m_Start = std::chrono::high_resolution_clock::now();
}

double Timer::ElapsedSeconds() const
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001 * 0.001 * 0.001;
}

double Timer::ElapsedMilliseconds() const
{
    return ElapsedSeconds() * 1000.0;
}

ScopedTimer::~ScopedTimer()
{
    const double time = m_Timer.ElapsedMilliseconds();
    std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
}
