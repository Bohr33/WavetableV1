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
WaveTableSound::WaveTableSound(std::vector<double>& table) : m_table(table)
{};

bool WaveTableSound::appliesToNote(int midiNoteNumber)
{return true;};
bool WaveTableSound::appliesToChannel(int midiChannel)
{return true;};

std::vector<double>& WaveTableSound::getTable()
{
    return m_table;
}


/*=============================================================================*/
/*----------------------------Synth Voice--------------------------------------*/
/*=============================================================================*/
SynthVoice::SynthVoice(std::vector<double>& table, int tSize) : m_table(table), m_tableSize(tSize){};
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};
void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //Zero previous values
    m_angle = 0.0;
    m_angleDelta = 0.0;
    m_level = velocity * 0.20;
    
    //Set Frequewncy and Angle
    m_freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    auto cyclesPerSample = m_freq / getSampleRate();
    
    m_angleDelta = cyclesPerSample * (double) m_tableSize;
    
    
    //Cast sound to new Type, and store wavetable Address
    if(auto* newSound = dynamic_cast<WaveTableSound*> (sound))
    {
        m_table = newSound->getTable();
    }
    
    
    juce::Logger::outputDebugString("MIDI Note = " + juce::String(midiNoteNumber));
    juce::Logger::outputDebugString("angle_delta = " + juce::String(m_angleDelta));
    
};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    m_level = 0.0;
    clearCurrentNote();
};

double SynthVoice::interpNextSamp() noexcept
{
    double currentVal;
    
    jassert(m_table.size() > 2);

    
    int index = (unsigned int)m_angle;
    auto table = m_table.data();
    
    double val_L = table[index];
    double val_H = table[index + 1];
    double frac = m_angle - (float) index;
    
    currentVal = val_L + (val_H - val_L)*frac;
    updateAngle();

    return currentVal;
}

void SynthVoice::updateAngle()
{
    m_angle += m_angleDelta;
    if(m_angle >= m_tableSize)
        m_angle -= m_tableSize;
};


//We never have to explicitely call this function, it is automatically called for every voice and summed in
//the synthesizerSource class
void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    if(m_angleDelta != 0)
    {
        int channel = 0;
        auto buffer = outputBuffer.getWritePointer(channel);
        double val = 0.0;
        for(auto i = 0; i < numSamples; ++i)
        {
            val = interpNextSamp() * m_level;
            outputBuffer.addSample(channel, i, val);
            outputBuffer.addSample(1, i, val);
        }
    }

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
    
    jassert(m_table.size() > 2);
    
    juce::Logger::outputDebugString("Table Size = " + juce::String(m_table.size()));
    auto val = m_table[100];
    juce::Logger::outputDebugString("val at 50 = " + juce::String(m_table[50]));


    //add sound to synth
    synth.addSound(new WaveTableSound(m_table));

    for(auto i = 0; i < maxVoices; ++i)
    {
        //add voice to synth
        synth.addVoice(new SynthVoice(m_table, defaultTableSize));
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
//    buffertoFill.clearActiveBufferRegion();
    juce::MidiBuffer incomingMidi;
    m_keyState.processNextMidiBuffer(incomingMidi, buffertoFill.startSample, buffertoFill.numSamples, true);
    synth.renderNextBlock(*buffertoFill.buffer, incomingMidi, buffertoFill.startSample, buffertoFill.numSamples);
  
};



void SynthAudioSource::generateWavetable(std::vector<double>& bufferToFill, unsigned int size)
{
    //Resize Vector as Size + Guard Point
    bufferToFill.resize(size + 1);
    
    //Prepare Sine Variables for Loop
    double delta = 1.0 / (double)size;
    double angle = 0.0;

    //Loop and add value to vector
    for(auto i = 0; i < size; ++i)
    {
        auto val = std::sin(angle * juce::MathConstants<double>::twoPi);
        bufferToFill[i] = val;
        angle += delta;
        juce::Logger::outputDebugString("val = " + juce::String(val));
    }
    //Guard Point
    bufferToFill[size] = bufferToFill[0];
    
};

