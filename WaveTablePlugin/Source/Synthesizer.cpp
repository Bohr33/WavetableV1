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
SynthVoice::SynthVoice(std::vector<double>& table, std::vector<double>& table2, int tSize) : m_table(table), m_table2(table2), m_tableSize(tSize){};
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};
void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //Zero previous values
    m_angle = 0.0;
    m_angleDelta = 0.0;
    m_level = velocity * 0.20;
    m_tail = 0.0;
    
    //Set Frequency and Angle
    m_freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    double cyclesPerSample = m_freq / getSampleRate();
    
    m_angleDelta = cyclesPerSample * (double) m_tableSize;
    
    
    //Cast sound to new Type, and store wavetable Address
    if(auto* newSound = dynamic_cast<WaveTableSound*> (sound))
    {
        m_table = newSound->getTable();
    }
    
    
    juce::Logger::outputDebugString("MIDI Note = " + juce::String(midiNoteNumber));
    juce::Logger::outputDebugString("Freq = " + juce::String(m_freq));
    juce::Logger::outputDebugString("angle_delta = " + juce::String(m_angleDelta));
    juce::Logger::outputDebugString("TableSize = " + juce::String(m_tableSize));
    
};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    if(allowTailOff)
    {
        if(m_tail == 0.0)
            m_tail = 1.0;
    }
    else
    {
        clearCurrentNote();
        m_angle = 0.0;
    }
};

double SynthVoice::interpNextSamp() noexcept
{
    double currentVal;
    
    jassert(m_table.size() > 2);

    
    int index = (int) m_angle;
    auto table = m_table.data();
    
    double val_L = table[index];
    double val_H = table[index + 1];
    double frac = m_angle - (float) index;
    
    currentVal = val_L + (val_H - val_L)*frac;
    updateAngle();

    return currentVal;
}

double SynthVoice::interpNextSamp(std::vector<double>& table) noexcept
{
    
    double currentVal;
    jassert(table.size() > 2);
    int index = (int) m_angle;
    
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
    
    auto interpVal = interpParam->load();
    
    if(m_angleDelta != 0)
    {
        double val = 0.0;
        if(m_tail > 0.0)
        {
            while (--numSamples >= 0) {
                val = interpolateValue(interpVal) * m_level * m_tail;
                
                for(auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, val);
                    
                startSample++;
                m_tail *= m_tailDec;
                
                if (m_tail <= 0.005) {
                    clearCurrentNote();
                    m_angleDelta = 0.0;
                    break;
                    }
                }
        }else
        {
            while (--numSamples >= 0) {
                val = interpolateValue(interpVal) * m_level;
                for(auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, val);
                
                startSample++;
                }
        }
    }
};

double SynthVoice::interpolateValue(float interpolation)
{
    
    auto interpVal = juce::jlimit(0.0f, 1.0f, interpolation);
    
    auto val1 = interpNextSamp(m_table);
    auto val2 = interpNextSamp(m_table2);
    
    auto diff = val1 - val2;
    
    double result = val1 - interpVal * diff;
    return result;
};


void SynthVoice::setParameters(std::atomic<float> *param)
{
    interpParam = param;
    
}

//Unused Pure Virtual Functions
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{};
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{};

