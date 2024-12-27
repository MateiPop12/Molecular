#pragma once

namespace Molecular
{
    class Timestep
    {
    public:
        Timestep(float time = 0.0f)
            : m_Time(time)
        {
        }
        float getSeconds() const { return m_Time; }
        float getMilliseconds() const { return m_Time * 1000.0f; }
    private:
        float m_Time;
    };
}