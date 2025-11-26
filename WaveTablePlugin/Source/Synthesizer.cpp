/*
  ==============================================================================

    Synthesizer.cpp
    Created: 25 Nov 2025 4:54:55pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/


#include "Synthesizer.h"


/*=============================================================================*/
/*----------------------------Synth Sound--------------------------------------*/
/*=============================================================================*/
WaveTableSound::WaveTableSound(juce::AudioBuffer<double>& table) : m_table(table)
{};

bool WaveTableSound::appliesToNote(int midiNoteNumber)
{return true;};
bool WaveTableSound::appliesToChannel(int midiChannel)
{return true;};


/*=============================================================================*/
/*----------------------------Synth Voice--------------------------------------*/
/*=============================================================================*/
SynthVoice::SynthVoice(){};
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};
void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{};

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
    
};

//Unused Pure Virtual Functions
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{};
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{};


/*=============================================================================*/
/*--------------------------------Audio Source---------------------------------*/
/*=============================================================================*/
SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState) : m_keyState(keyState){
    juce::Logger::outputDebugString("Hello!");
    
    
    generateWavetable(m_waveTable, defaultTableSize);
    
    //add sound to synth
    synth.addSound(new WaveTableSound(m_waveTable));
    
    for(auto i = 0; i < maxVoices; ++i)
    {
        //add voice to synth
        synth.addVoice(new SynthVoice);

    }
};

SynthAudioSource::~SynthAudioSource()
{
    
};

void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
};

void SynthAudioSource::releaseResources(){};

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &buffertoFill)
{
    buffertoFill.clearActiveBufferRegion();
    juce::MidiBuffer incomingMidi;
    m_keyState.processNextMidiBuffer(incomingMidi, buffertoFill.startSample, buffertoFill.numSamples, true);
    synth.renderNextBlock(*buffertoFill.buffer, incomingMidi, buffertoFill.startSample, buffertoFill.numSamples);
  
};

void SynthAudioSource::generateWavetable(juce::AudioBuffer<double> bufferToFill, unsigned int size)
{
    bufferToFill.setSize(1, size + 1);
    double period = 1 / size;
    double frequency = 1 / period;
    double angle = 0.0;
    
    for(auto i = 0; i <= size; ++i)
    {
        auto val = std::sin(angle * juce::MathConstants<double>::twoPi);
        bufferToFill.addSample(0, i, val);
        angle += frequency;
    }
    
};

