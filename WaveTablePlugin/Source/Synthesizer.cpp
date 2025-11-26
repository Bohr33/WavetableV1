/*
  ==============================================================================

    Synthesizer.cpp
    Created: 25 Nov 2025 4:54:55pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/


#include "Synthesizer.h"



bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};
void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{};

void SynthVoice::renderNextBlock(juce::AudioBuffer<double> &outputBuffer, int startSample, int numSamples)
{}

//Unused Pure Virtual Functions
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{};
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{};


void SynthAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate){};
void SynthAudioSource::releaseResources(){};
void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &buffertoFill){};

