#pragma once

#include <chrono>
#include <iostream>
#include <string>

class Timer
{
public:
    Timer()
    {
        Reset();
    }

    void Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    double Elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001 * 0.001 * 0.001;
    }

    double ElapsedMillis() const
    {
        return Elapsed() * 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

class ScopedTimer
{
public:
    explicit ScopedTimer(std::string name)
        : m_Name(std::move(name))
    {
    }
    ~ScopedTimer()
    {
        const double time = m_Timer.ElapsedMillis();
        std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
    }

private:
    std::string m_Name;
    Timer m_Timer;
};
