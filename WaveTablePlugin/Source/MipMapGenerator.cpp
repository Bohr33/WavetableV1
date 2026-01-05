/*
  ==============================================================================

    MipMapGenerator.cpp
    Created: 3 Jan 2026 7:08:41pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "MipMapGenerator.h"


MipMapGenerator::MipMapGenerator() : fft_object(fft_order)
{
    //resize buffers
    fft_data.resize(m_tableSize * 2.0);
    mipmap_buffer.resize(m_tableSize * 2.0);
};
MipMapGenerator::~MipMapGenerator() = default;

std::vector<std::vector<float>> MipMapGenerator::generateMipMaps(const std::vector<float>& wavetable, double sampleRate)
{
    
    //Create mipmap object to store mipmaps and eventually return to caller
    std::vector<std::vector<float>> mipmaps;
    
    //Check wavetable is valid size
    if(!validateTable(wavetable))
    {
        juce::Logger::writeToLog("Error Generating MipMap: Table Invalid");
        return {};
    }
    
    //Copy wavetable to fft operation buffer
    juce::FloatVectorOperations::copy(fft_data.data(), wavetable.data(), m_tableSize);
    //Perform Forward Transform on 2*tablesize Sized buffer
    fft_object.performRealOnlyForwardTransform(fft_data.data());
    
    
    //Note: B-1, the highest frequency of the lowest octave
    float lowestMaxFreq = 15.534;
    int num_mipmaps = calculate_num_mipmaps(sampleRate);
    
    mipmaps.reserve(num_mipmaps);
    
    for (auto octave = 0; octave < num_mipmaps; ++octave) {
        //copy data to new buffer
        juce::FloatVectorOperations::clear(mipmap_buffer.data(), mipmap_buffer.size());
        juce::FloatVectorOperations::copy(mipmap_buffer.data(), fft_data.data(), fft_data.size());
        //calculate highest frequencies, and zero bins
        auto tablePointer = mipmap_buffer.data();
        auto highestOctaveFreq = lowestMaxFreq * std::pow(2.0f, octave);
        int cutoffBin = (sampleRate/2.0f)/highestOctaveFreq;
        for (auto bin = cutoffBin; bin < m_tableSize/2; ++bin) {
            tablePointer[bin * 2] = 0.0f;
            tablePointer[bin * 2 + 1]= 0.0f;
        }
        
        //perform Inverse Transform
        fft_object.performRealOnlyInverseTransform(mipmap_buffer.data());
        
        
        std::vector<float> processedTable(m_tableSize + 1);
        auto mipmap = mipmap_buffer.data();
        
        for (auto i = 0; i < m_tableSize; ++i) {
            //Copy only First half of buffer
            processedTable[i] = mipmap[i];
        }
        
        //Guard Point
        processedTable[m_tableSize] = processedTable[0];
        
        //store table
        mipmaps.push_back(std::move(processedTable));
        juce::Logger::writeToLog("MipMap Added!");
        
    }
    
    return mipmaps;
};


//Takes a source table, a cutoff frequency, and sample rate, and
//Filters the table via FFT and IFT
void MipMapGenerator::filterTable(const std::vector<float>& source, float cutoffFreq, float sampleRate)
{
    
}


//Function calculates the number of mipmaps need based on sample rate and max MIDI note
int MipMapGenerator::calculate_num_mipmaps(float sample_rate, int max_midi_note)
{
    float max_freq = 440.0f * std::pow(2.0f, (max_midi_note - 69) / 12.0f);
    float nyquist = sample_rate  / 2.0f;
    int max_harmonics = nyquist / max_freq;
    
    int num_mipmaps = std::ceil(std::log2((m_tableSize/2.0f)/max_harmonics)) + 1;
    
    return num_mipmaps;
}


//Ensure table is proper size (2048/2049) and flag guard point
bool MipMapGenerator::validateTable(const std::vector<float>& wavetable)
{
    auto size = wavetable.size();
    
    juce::Logger::writeToLog("TableSize = " + juce::String(size));
    
    return (size == 2048 || size == 2049);
}
