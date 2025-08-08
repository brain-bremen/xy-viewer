#pragma once
#include <JuceHeader.h>
#include <mutex>
#include <vector>

namespace XYViewerPlugin
{

class CircularXYBuffer : public juce::AudioBuffer<float>
{
public:
    CircularXYBuffer (int capacity)
        : juce::AudioBuffer<float> (2, capacity), m_capacity (capacity)
    {
        clear();
    }

    // Pushes numSamples from x and y arrays into the buffer
    void push (const float* x, const float* y, int numSamples)
    {
        std::lock_guard<std::mutex> lock (m_mutex);
        for (int i = 0; i < numSamples; ++i)
        {
            setSample (0, m_writePos, x[i]);
            setSample (1, m_writePos, y[i]);
            m_writePos = (m_writePos + 1) % m_capacity;
            if (m_size < m_capacity)
                ++m_size;
        }
    }

    // Copies the latest numSamples into xOut and yOut (oldest to newest)
    void getLatest (std::vector<float>& xOut, std::vector<float>& yOut, int numSamples)
    {
        std::lock_guard<std::mutex> lock (m_mutex);
        int n = std::min (numSamples, m_size);
        xOut.resize (n);
        yOut.resize (n);
        int start = (m_writePos + m_capacity - n) % m_capacity;
        for (int i = 0; i < n; ++i)
        {
            int idx = (start + i) % m_capacity;
            xOut[i] = getSample (0, idx);
            yOut[i] = getSample (1, idx);
        }
    }

    int getCapacity() const { return m_capacity; }
    int getSize() const { return m_size; }

private:
    int m_capacity;
    int m_writePos = 0;
    int m_size = 0;
    std::mutex m_mutex;
};

} // namespace XYViewerPlugin
