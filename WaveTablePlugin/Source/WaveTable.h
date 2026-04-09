/*
  ==============================================================================

    WaveTable.h
    Created: 26 Dec 2025 9:39:19pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

#include "MipMapGenerator.h"



//------Wavetable Structure------//
//- Will hold the raw frame data and -//
//- basic info for each loaded file -//
struct Wavetable
{
    juce::String name;
    int frameSize;
    int frameCount;

    std::vector<std::vector<float>> frames;
    std::vector<MipMap> mipmaps;
};

//----WaveBank structure will store all the mipmaps for each wavetable section for each wavebank---//
struct WaveBank
{
    std::vector<MipMap> wavetables;
    
    int numTables;
    
    const MipMap& getWavetable(int index) const
    {
        return wavetables[index];
    }
    
    size_t size() const {
        return wavetables.size();
    }
    
};



//Class for maintaining wavetables and their mipmaps
class WaveBankManager{
public:
    
    WaveBankManager() = default;
    
//    void loadDefaultTables(const juce::File& rescourceFolder);
    
    //Put this one in the prepare to play function
    void updateSampleRate(double newSampleRate);
    
//    Wavetable loadWaveTableFile(juce::File file);
    
    
    
    //New initial loading class, all other operations should stem from here at some point
    void loadWavetableFromBinary();
    
    void parseBinaryData(juce::AudioBuffer<float> binaryData);
    
    
    void reportTableData(int tableNum = 0, int frameNum = 0);
    void printTables(int tableNumber = 0);
    
    void generateMipmaps();
    
    bool verifySampleRate();
    
    //Mainly creates mipmaps
    void prepareToPlay();
    
    
private:
    juce::String sourcePath;
    
    double sampleRate;
    
    MipMapGenerator mipmapGenerator;
    
    //Each outer Vector will hold the entire file data for each wavtable (contiguous wavetables
    std::vector<Wavetable> waveTables;
    
    //Not sure if we need this rawTables array, maybe just process into waveTables on load
    std::vector<std::vector<float>> rawTables;
    
};

// A general interpolating wavetable loading function, creates a mipMap and stores it based on wavetable file provided 
bool loadWavetable(juce::File wavetableFile, std::vector<std::vector<std::shared_ptr<const MipMap>>> waveBank, double sampleRate);


