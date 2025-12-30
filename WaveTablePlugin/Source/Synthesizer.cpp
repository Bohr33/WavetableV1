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
SynthVoice::SynthVoice(std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo, int tableSize) : m_tableSize(tableSize), m_tableOne(tableOne), m_tableTwo(tableTwo)
{};
bool SynthVoice::canPlaySound(juce::SynthesiserSound*)
{return true;};

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, int currentPitchWheelPosition)
{
    //Zero previous values
    m_angle = 0.0;
    m_angleDelta = 0.0;
    m_level = velocity;
    
    //Set Frequency and Angle
    m_freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    float cyclesPerSample = m_freq / getSampleRate();
    m_angleDelta = cyclesPerSample * (float) m_tableSize;
    
    envelope.noteOn();
    
    
    //Cast sound to new Type, and store wavetable Address
    auto* newSound = dynamic_cast<WaveTableSound*>(sound);

    juce::Logger::outputDebugString("MIDI Note = " + juce::String(midiNoteNumber));
    
};
void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    envelope.noteOff();
};

void SynthVoice::prepare(double sampleRate)
{
    envelope.setSampleRate(sampleRate);
}


void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
{
    
    auto interpVal = interpParam->load();
    
    ADSR::Parameters params =
    {
        attackParam->load(),
        decayParam->load(),
        sustainParam->load(),
        releaseParam->load(),
        0.0,
        0.0,
        0.0
    };
    
    envelope.setParameters(params);
    
    //atomically load tables from shared pointers
    auto tableOne = std::atomic_load(&m_tableOne);
    auto tableTwo = std::atomic_load(&m_tableTwo);

    if(m_angleDelta != 0.0)
    {
        float val1 = 0.0;
        float val2 = 0.0;
        float output = 0.0;
        float envVal;
        
        //Main processing loop when note on
        while (--numSamples >= 0) {
            envVal = envelope.getNextSample();
            val1 = interpNextSamp(tableOne);
            val2 = interpNextSamp(tableTwo);
            output = interpolate(interpVal, val1,  val2) * m_level * envVal;
            
            for(auto channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
                outputBuffer.addSample(channel, startSample, output);
            
            updateAngle();
            startSample++;
            
            if(!envelope.isActive())
            {
                clearCurrentNote();
                m_angleDelta = 0.0;
                break;
            }
        }
    }
        
   
};








//Main table reading function, performs basic linearly interpolation expacting a guard point
float SynthVoice::interpNextSamp(std::shared_ptr<const TableData> table) noexcept
{
    jassert(table->samples.size() == 2049);
    
    float currentVal;
    auto data = table->samples.data();
    
    int index = (int) m_angle;
    
    float val_L = data[index];
    float val_H = data[index + 1];
    float frac = m_angle - (float) index;
    
    currentVal = val_L + (val_H - val_L)*frac;

    return currentVal;
}

void SynthVoice::updateAngle()
{
    m_angle += m_angleDelta;
    if(m_angle >= m_tableSize)
        m_angle -= m_tableSize;
};

//Returns interpolated value based on interpolation value and tables given
//This version isn't used, instead the below version is used rendering loop
float SynthVoice::interpolate(float interp_val, std::shared_ptr<const TableData> tableOne, std::shared_ptr<const TableData> tableTwo)
{
    auto interpVal = juce::jlimit(0.0f, 1.0f, interp_val);

    auto val1 = interpNextSamp(tableOne);
    auto val2 = interpNextSamp(tableTwo);
    
    auto diff = val1 - val2;
    
    float result = val1 - interpVal * diff;
    return result;
};


float SynthVoice::interpolate(float interp_val, float val1, float val2)
{
    auto interpVal = juce::jlimit(0.0f, 1.0f, interp_val);
    
    auto diff = val1 - val2;
    
    float result = val1 - interpVal * diff;
    return result;
    
}

//Setter Function for Audio Processor Value True State
void SynthVoice::setAPVTS(juce::AudioProcessorValueTreeState* apvts)
{
    apvtsRef = apvts;
    
    interpParam = apvts->getRawParameterValue("interpolation");
    
    attackParam = apvts->getRawParameterValue("env_attack");
    decayParam = apvts->getRawParameterValue("env_decay");
    sustainParam = apvts->getRawParameterValue("env_sustain");
    releaseParam = apvts->getRawParameterValue("env_release");
}

void SynthVoice::setWavetable(int tableID, std::shared_ptr<const TableData> newTable)
{
    //index from One
    if (tableID == 1)
        std::atomic_store(&m_tableTwo, newTable);
    else
        std::atomic_store(&m_tableOne, newTable);
}



void SynthVoice::printTable()
{
    for (auto i = 0; i < m_tableSize; i++) {
        auto val = m_tableOne->samples[i];
        juce::Logger::writeToLog("Val = " + juce::String(val));
    }
    juce::Logger::writeToLog("Done");
}

void SynthVoice::reportTables()
{
    
    auto tableSize1 = m_tableOne->samples.size();
    auto tableSize2 = m_tableTwo->samples.size();
    
    
    juce::Logger::writeToLog("Table 1; Size = " + juce::String(tableSize1));
    
    
    juce::Logger::writeToLog("Values 100 - 150: " + juce::String(tableSize1));
    
    for (auto i = 0; i < 50; i++) {
        auto val = m_tableOne->samples[i + 100];
        juce::Logger::writeToLog(juce::String(i + 100) + " = " + juce::String(val));
    }
    
    juce::Logger::writeToLog("Table 2; Size = " + juce::String(tableSize2));
    
    for (auto i = 0; i < 50; i++) {
        auto val = m_tableTwo->samples[i + 100];
        juce::Logger::writeToLog(juce::String(i + 100) + " = " + juce::String(val));
    }

}

//Unused Pure Virtual Functions
void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{};
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{};

