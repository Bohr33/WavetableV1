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
WaveTableSound::WaveTableSound()
{};

bool WaveTableSound::appliesToNote(int midiNoteNumber)
{return true;};
bool WaveTableSound::appliesToChannel(int midiChannel)
{return true;};


/*=============================================================================*/
/*----------------------------Synth Voice--------------------------------------*/
/*=============================================================================*/
//SynthVoice::SynthVoice(std::vector<float>& table, std::vector<float>& table2, int tSize) : m_table(table), m_table2(table2), m_tableSize(tSize)
//{
//    //Create default table for pointer
//    auto defaultTable = std::make_shared<TableData>();
//    m_tableOne = defaultTable;
//    m_tableTwo = defaultTable;
//};

SynthVoice::SynthVoice(std::shared_ptr<const TableData> initialTable, int tableSize) : m_tableSize(tableSize), m_tableOne(initialTable), m_tableTwo(initialTable)
{
    //Store default tables, below shouldn't be necessary
    
//    //Create default table for pointer
//    auto defaultTable = std::make_shared<TableData>();
//    m_tableOne = defaultTable;
//    m_tableTwo = defaultTable;
};
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //Zero previous values
    m_angle = 0.0;
    m_angleDelta = 0.0;
    m_level = velocity;
    m_tail = 0.0;
    
    //Set Frequency and Angle
    m_freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    
    float cyclesPerSample = m_freq / getSampleRate();
    
    m_angleDelta = cyclesPerSample * (float) m_tableSize;
    
    
    //Cast sound to new Type, and store wavetable Address
    auto* newSound = dynamic_cast<WaveTableSound*>(sound);

    
    
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

//float SynthVoice::interpNextSamp() noexcept
//{
//    float currentVal;
//
//    jassert(m_table.size() > 2);
//
//
//    int index = (int) m_angle;
//    auto table = m_table.data();
//
//    float val_L = table[index];
//    float val_H = table[index + 1];
//    float frac = m_angle - (float) index;
//
//    currentVal = val_L + (val_H - val_L)*frac;
//    updateAngle();
//
//    return currentVal;
//}

//float SynthVoice::interpNextSamp(std::vector<float>& table) noexcept
//{
//
//    float currentVal;
//    jassert(table.size() > 2);
//    int index = (int) m_angle;
//
//    float val_L = table[index];
//    float val_H = table[index + 1];
//    float frac = m_angle - (float) index;
//
//    currentVal = val_L + (val_H - val_L)*frac;
//    updateAngle();
//
//    return currentVal;
//}


float SynthVoice::interpNextSamp2(std::shared_ptr<const TableData> table) noexcept
{
    
    float currentVal;
    auto data = table->samples;
    
    int index = (int) m_angle;
    
    float val_L = data[index];
    float val_H = data[index + 1];
    float frac = m_angle - (float) index;
    
    currentVal = val_L + (val_H - val_L)*frac;
    updateAngle();

    return currentVal;
}

void SynthVoice::printTable()
{
    for (auto i = 0; i < m_tableSize; i++) {
        auto val = m_tableOne->samples[i];
        juce::Logger::writeToLog("Val = " + juce::String(val));
    }
    juce::Logger::writeToLog("Done");
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
    
    //atomically load tables from shared pointers
    auto tableOne = std::atomic_load(&m_tableOne);
    auto tableTwo = std::atomic_load(&m_tableTwo);
//    auto tableOne = m_tableOne;
//    auto tableTwo = m_tableTwo;
    

    if(m_angleDelta != 0)
    {
        float val = 0.0;
        if(m_tail > 0.0)
        {
            while (--numSamples >= 0) {
                //Processing loop for tail
                
//                val = interpolate(interpVal, tableOne, tableTwo) * m_level * m_tail;
                val = interpNextSamp2(tableTwo) * m_level * m_tail;
                
                for(auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, val);
                    
                startSample++;
                m_tail *= m_tailDec;
                
                if (m_tail <= 0.005) {
                    juce::Logger::writeToLog("Note CLeared");
                    clearCurrentNote();
                    m_angleDelta = 0.0;
                    break;
                    }
                }
        }else
        {
            //Main processing loop when note on
            while (--numSamples >= 0) {
//                val = interpolateValue(interpVal) * m_level;
//                val = interpolate(interpVal, tableOne, tableTwo) * m_level;
                val = interpNextSamp2(tableTwo) * m_level;
//                juce::Logger::writeToLog("Val = " + juce::String(val));
                for(auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                    outputBuffer.addSample(channel, startSample, val);
                
                startSample++;
                }
        }
    }
};

//float SynthVoice::interpolateValue(float interpolation)
//{
//
//    auto interpVal = juce::jlimit(0.0f, 1.0f, interpolation);
//
//    auto val1 = interpNextSamp(m_table);
//    auto val2 = interpNextSamp(m_table2);
//
//    auto diff = val1 - val2;
//
//    float result = val1 - interpVal * diff;
//    return result;
//};

float SynthVoice::interpolate(float interp_val, std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo)
{
    
//    auto val = tableOne->samples[test];
//    test++;
//    test %= m_tableSize;
//    juce::Logger::writeToLog("val = " + juce::String(val));
    
    auto interpVal = juce::jlimit(0.0f, 1.0f, interp_val);
    
    auto val1 = interpNextSamp2(tableOne);
    auto val2 = interpNextSamp2(tableTwo);
    
    auto diff = val1 - val2;
    
    float result = val1 - interpVal * diff;
    return result;
};


void SynthVoice::setParameters(std::atomic<float> *param)
{
    interpParam = param;
    
}

void SynthVoice::setWavetable(int tableID, std::shared_ptr<const TableData> newTable)
{
    
    if (tableID == 1)
        std::atomic_store(&m_tableOne, newTable);
    else
        std::atomic_store(&m_tableTwo, newTable);
}

//Unused Pure Virtual Functions
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{};
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{};

