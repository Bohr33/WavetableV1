/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Graphics.h"
#include "ImportWavetable.h"

//==============================================================================
/**
*/
class WaveTablePluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
        juce::AudioProcessorValueTreeState::Listener,
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
    
    //APVTS Listener/Callback Overide
    void parameterChanged(const juce::String& paramID, float newValue) override;
    
    
//    void selectNewWaveform(int tableId, int waveformId);
    
    void selectNewWaveformTableOne(int waveformID);
    void selectNewWaveformTableTwo(int waveformID);
    
    void selectNewWavetable(int wavetableID);
    
    void updateADSRDisplay();
    
//    void selectNewWaveBank(int wavebankID);
    
    void importWavetable(const juce::File& file);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    WaveTablePluginAudioProcessor& audioProcessor;
    juce::MidiKeyboardComponent    keyboardComponent;
    juce::MidiKeyboardState        keyboardState;
    
    MyLookAndFeel laf;
    
    juce::Slider    s_interpolation;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> interpolationAttachment;
    

    InterpolatedDisplay m_interpDisplay;
    
    InterpolatedWavetableDisplay m_interpWaveDisplay;
    
    WavetableFrameDisplay m_displayOne;
    juce::ComboBox  waveBankOne;

    WavetableFrameDisplay m_displayTwo;
    juce::ComboBox  waveBankTwo;
    
    juce::ComboBox  waveTableSelect;
    
    
    ADSRDisplay m_envelopeDisplay;
    
    
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
    
    
    //Filter Elements
    
    juce::Slider s_cutoff;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    
    juce::Slider s_resonance;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    
    juce::TextButton lowPassBtn;
    juce::TextButton bandPassBtn;
    juce::TextButton highPassBtn;
    
//    juce::DrawableButton lowPassBtn;
//    juce::DrawableButton bandPassBtn;
//    juce::DrawableButton highPassBtn;
    
    //Wavetable Import Button
    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton load_table_btn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTablePluginAudioProcessorEditor)
};
