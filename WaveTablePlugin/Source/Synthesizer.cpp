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
SynthSound::SynthSound(){};
bool SynthSound::appliesToNote(int midiNoteNumber)
{return true;};
bool SynthSound::appliesToChannel(int midiChannel)
{return true;};


/*=============================================================================*/
/*----------------------------Synth Voice--------------------------------------*/
/*=============================================================================*/
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};
void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{};

void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{};

//Unused Pure Virtual Functions
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{};
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{};


/*=============================================================================*/
/*--------------------------------Audio Source---------------------------------*/
/*=============================================================================*/
SynthAudioSource::SynthAudioSource(juce::MidiKeyboardState& keyState) : keyboardState(keyState){
    juce::Logger::outputDebugString("Hello!");
    
    for(auto i = 0; i < maxVoices; ++i)
    {
        //add voice to synth
        synth.addVoice(new SynthVoice);
        //add sound to synth
        synth.addSound(new SynthSound);
    }
};

SynthAudioSource::~SynthAudioSource(){};

void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate){};
void SynthAudioSource::releaseResources(){};
void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &buffertoFill){};

