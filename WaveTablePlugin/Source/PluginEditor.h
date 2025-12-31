/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"

//==============================================================================
/**
*/
class WaveTablePluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
private juce::MidiKeyboardState::Listener
{
public:
    WaveTablePluginAudioProcessorEditor (WaveTablePluginAudioProcessor&);
    ~WaveTablePluginAudioProcessorEditor() override;
    
    //MidiKeyboard Listener Virtual Callbacks
    void handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)override;
    void handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void selectNewWaveform(int tableId, int waveformId);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WaveTablePluginAudioProcessor& audioProcessor;
    juce::MidiKeyboardComponent    keyboardComponent;
    juce::MidiKeyboardState        keyboardState;
    
    juce::Slider    s_interpolation;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> interpolationAttachment;
    

    InterpolatedDisplay m_interpDisplay;
    
    WavetableDisplay m_displayOne;
    juce::ComboBox  waveBankOne;

    WavetableDisplay m_displayTwo;
    juce::ComboBox  waveBankTwo;
    
    
    
    //ADSR Components
    juce::Slider    s_envAttack;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envAttackAttachment;
    
    juce::Slider    s_envDecay;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envDecayAttachment;
    
    juce::Slider    s_envSustain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envSustainAttachment;
    
    juce::Slider    s_envRelease;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> envReleaseAttachment;
    
    juce::Slider    s_attCurve;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attCurveAttachment;
    
    juce::Slider    s_decCurve;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decCurveAttachment;
    
    juce::Slider    s_relCurve;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relCurveAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTablePluginAudioProcessorEditor)
};
