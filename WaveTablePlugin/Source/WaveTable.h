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
    
    enum class AssetID
    {
        Test_1,
        Test_2,
        Test_3,
        Count
    };
    
    WaveBankManager()
    {
        formatManager.registerBasicFormats();
    }
    
    //Put this one in the prepare to play function
    void updateSampleRate(double newSampleRate);
    
    //New initial loading class, all other operations should stem from here at some point
    void loadWavetablesFromBinary();
    
    Wavetable parseBinaryData(juce::AudioBuffer<float> binaryData);
    
    void reportTableData(int tableNum = 0, int frameNum = 0);
    void printTables(int tableNumber = 0);
    
    void generateMipmaps();
    
    std::shared_ptr<const MipMap> formatMipMapForSynth(int bankID, int mapID);
    
    std::shared_ptr<const Wavetable> getWavetable(int index);
    
    bool verifySampleRate();
    
    //Mainly creates mipmaps
    void prepareToPlay();
    
    static const char* getAssetData(AssetID id, size_t& size);
    
    
private:
    juce::String sourcePath;
    juce::AudioFormatManager formatManager;
    
    //Continue by giving the waveBankManager class a proper way to report the number of banks and tables
    //it has to the outside functions
    
    double sampleRate;
    
    MipMapGenerator mipmapGenerator;
    
    int numWavetables;
    
    //Each outer Vector will hold the entire file data for each wavtable (contiguous wavetables
    std::vector<std::shared_ptr<Wavetable>> waveTables;
    
    //Not sure if we need this rawTables array, maybe just process into waveTables on load
    std::vector<std::vector<float>> rawTables;
    
};

// A general interpolating wavetable loading function, creates a mipMap and stores it based on wavetable file provided 
bool loadWavetable(juce::File wavetableFile, std::vector<std::vector<std::shared_ptr<const MipMap>>> waveBank, double sampleRate);


