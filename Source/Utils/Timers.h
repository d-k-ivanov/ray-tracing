#pragma once

#include <chrono>
#include <string>

class Timer
{
public:
    Timer();

    void   Reset();
    double ElapsedSeconds() const;
    double ElapsedMilliseconds() const;

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

    ~ScopedTimer();

private:
    std::string m_Name;
    Timer       m_Timer;
};
