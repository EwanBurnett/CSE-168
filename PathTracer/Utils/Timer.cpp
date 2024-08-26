#include "Timer.h"

EDX::Timer::Timer() {
    m_bIsRunning = false;
    Reset();
}

void EDX::Timer::Tick()
{
    if (m_bIsRunning) {
        m_tPrevious = m_tCurrent;
        m_tCurrent = std::chrono::high_resolution_clock::now();

        m_Duration += (m_tCurrent - m_tPrevious);
    }
}

void EDX::Timer::Start()
{
    m_bIsRunning = true;
}

void EDX::Timer::Stop()
{
    m_bIsRunning = false;
}

void EDX::Timer::Reset()
{
    m_tPrevious = std::chrono::high_resolution_clock::now();
    m_tCurrent = std::chrono::high_resolution_clock::now();

    m_Duration = {};
}