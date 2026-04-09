/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "Synthesizer.h"
#include "WavetableGenerator.h"
#include "WaveTable.h"
#include <span>
#include "MipMapGenerator.h"
#include "ImportWavetable.h"
#include <BinaryData.h>

//==============================================================================
/**
*/
class WaveTablePluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    WaveTablePluginAudioProcessor();
    ~WaveTablePluginAudioProcessor() override;
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    //Added Functions
    juce::MidiKeyboardState& getMidiKeyboardState();
    void setWaveform(int tableID, int waveformID);
    const std::vector<float> getTable(int tableID);
    std::shared_ptr<const MipMap> getMipMap(int mapID);
    
    void loadWavetableRescources();
    void generateWavetableBank();
    
    int loadWavetableFile(const juce::File& file);
    
    void loadBinaryData();
    
    void parseBinaryWavetableData();
    

    
    //==============================================================================
    //Value Tree State
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    

private:
    juce::MidiKeyboardState m_keystate;
    juce::MidiMessageCollector midiCollector;
    
    unsigned int bufferSize = 128;
    const int defaultNumHarmonics = 100;
    
    
    // Get plugin path for rescource folder
    juce::File exeFile = juce::File::getSpecialLocation(juce::File::currentExecutableFile);
    juce::File baseFolder = exeFile.getParentDirectory();
    
    // wavetable folder
    juce::File wavetableFolder = baseFolder.getChildFile("assets/Test");
    
    
    //Synth Parameters
    const int maxVoices = 16;
    const int defaultTableSize = 1 << 11;
    const int numTables = 4;
    
    juce::Synthesiser synth;
    
    std::vector<float> m_table;
    std::vector<float> m_table2;
    
    WavetableGenerator tableGenerator;
    MipMapGenerator m_mipmapGenerator;

    //Wavetable Bank
    std::vector<const std::vector<float>> wavetableBank;
    
    //MipMap Bank
    std::vector<std::shared_ptr<const MipMap>> mipmapBank;
    
    //Has constructor load, and prepare to play functions
    WaveBankManager m_waveManager;
    
    
    //Wavebank Bank [Wavetable Index][Frame Index(waveform)][mipmap]
    std::vector<std::vector<std::shared_ptr<const MipMap>>> wavebankBank;
    

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTablePluginAudioProcessor)
};
