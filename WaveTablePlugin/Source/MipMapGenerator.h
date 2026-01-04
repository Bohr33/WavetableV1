/*
  ==============================================================================

    MipMapGenerator.h
    Created: 3 Jan 2026 7:08:41pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <juce_dsp/juce_dsp.h>


class MipMapGenerator
{
public:
    MipMapGenerator();
    ~MipMapGenerator();
    
    std::vector<std::vector<float>>& generateMipMaps(const std::vector<float>& wavetable, double sampleRate);

    int calculate_num_mipmaps(float sample_rate, int max_midi_note = 127);
    
    void filterTable(const std::vector<float>& source, float cutoffFreq, float sampleRate);
    
    
    
private:
    
    bool validateTable(const std::vector<float>& wavetable);
    static constexpr int m_tableSize = 2048;
    
    static constexpr auto fft_order = 11; //2^11 = 2048
    static constexpr auto fftSize = 1 << fft_order;
    
    juce::dsp::FFT fft_object;
    std::vector<float> fft_data;
    std::vector<float> mipmap_buffer;
    
    bool guardPoint;
};
