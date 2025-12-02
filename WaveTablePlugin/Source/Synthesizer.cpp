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
WaveTableSound::WaveTableSound(std::vector<double>* table) : m_table(table)
{};

bool WaveTableSound::appliesToNote(int midiNoteNumber)
{return true;};
bool WaveTableSound::appliesToChannel(int midiChannel)
{return true;};

std::vector<double>* WaveTableSound::getTable()
{
    return m_table;
}


/*=============================================================================*/
/*----------------------------Synth Voice--------------------------------------*/
/*=============================================================================*/
SynthVoice::SynthVoice(){};
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};
void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //Zero previous values
    m_angle = 0.0;
    m_level = velocity * 0.20;
    
    //Set Frequewncy and Angle
    m_freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    
    //Cast sound to new Type, and store wavetable Address
    if(auto* newSound = dynamic_cast<WaveTableSound*> (sound))
    {
        m_table = newSound->getTable();
    }
    
    
    
};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{};

double SynthVoice::interpNextSamp() noexcept
{
    double currentVal;
    int index = (unsigned int)currentIndex;
    auto& table = *m_table;
    
    double val_L = table[index];
    double val_H = table[index + 1];
    double frac = currentIndex - (float) index;
    
    currentVal = val_L + (val_H - val_L)*frac;
    updateAngle();
    return currentVal;
}

void SynthVoice::updateAngle()
{
};

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
    
    
    generateWavetable(m_table, defaultTableSize);
    
    //add sound to synth
    synth.addSound(new WaveTableSound(m_table));
    
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



void SynthAudioSource::generateWavetable(std::vector<double>* bufferToFill, unsigned int size)
{
    //Resize Vector as Size + Guard Point
    bufferToFill->reserve(size + 1);
    
    //Prepare Sine Variables for Loop
    double period = 1 / size;
    double frequency = 1 / period;
    double angle = 0.0;

    //Loop and add value to vector
    for(auto i = 0; i < size; ++i)
    {
        bufferToFill->push_back(std::sin(angle * juce::MathConstants<double>::twoPi));
        angle += frequency;
    }
    //Guard Point
    bufferToFill->push_back(bufferToFill->at(0));
    
};

