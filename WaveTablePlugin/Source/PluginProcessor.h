/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Synthesizer.h"
#include "WavetableGenerator.h"
#include "WaveTable.h"
#include <span>

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
    std::shared_ptr<const TableData> getTable(int tableID);
    void generateWavetableBank();
    
    //==============================================================================
    //Value Tree State
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    

private:
    juce::MidiKeyboardState m_keystate;
    juce::MidiMessageCollector midiCollector;
    
    unsigned int bufferSize = 128;
    
    //Synth Parameters
    const int maxVoices = 8;
    const int defaultTableSize = 1 << 11;
    const int numTables = 4;
    
    juce::Synthesiser synth;
    
    const int defaultNumHarmonics = 16;
    WavetableGenerator tableGenerator;
    std::vector<float> m_table;
    std::vector<float> m_table2;
    
    //Wavetable Bank
    std::vector<std::shared_ptr<const TableData>> wavetableBank;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTablePluginAudioProcessor)
};
