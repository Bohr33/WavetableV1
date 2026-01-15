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
SynthVoice::SynthVoice(std::shared_ptr<const MipMap> mipmapA, std::shared_ptr<const MipMap> mipmapB, int tableSize) : m_tableSize(tableSize), m_mipmapA(mipmapA), m_mipmapB(mipmapB)
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
    auto freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    m_baseFreq = freq;
    setFrequency(freq);
    
    //Set stage so processor can retrieve right table
    m_mapStage = midiNoteNumber / 12;

    
    m_pitchBend = currentPitchWheelPosition;
    
    envelope.noteOn();
    
    //Cast sound to new Type, and store wavetable Address
    auto* newSound = dynamic_cast<WaveTableSound*>(sound);

    juce::Logger::outputDebugString("MIDI Note = " + juce::String(midiNoteNumber));
    juce::Logger::outputDebugString("Map Stage = " + juce::String(m_mapStage));
    juce::Logger::outputDebugString("Current Wheel Position = " + juce::String(currentPitchWheelPosition));
    
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
        attCurveParam->load(),
        decCurveParam->load(),
        relCurveParam->load()
    };
    
    envelope.setParameters(params);
    
    //atomically load tables from shared pointers
    auto mapA = std::atomic_load(&m_mipmapA);
    auto mapB = std::atomic_load(&m_mipmapB);
    
    if(!mapA || !mapB)
        return;
    
    auto tableA = mapA->getStage(m_mapStage);
    auto tableB = mapB->getStage(m_mapStage);

    if(m_angleDelta != 0.0)
    {
        float val1 = 0.0;
        float val2 = 0.0;
        float output = 0.0;
        float envVal;
        

        
        //Main processing loop when note on
        while (--numSamples >= 0) {
            
            float bentFreq = calculateBendFreq();
            setFrequency(bentFreq);
            
            envVal = envelope.getNextCurveSample();
            val1 = interpNextSamp(tableA);
            val2 = interpNextSamp(tableB);
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
float SynthVoice::interpNextSamp(std::vector<float>& table)
{
    jassert(table.size() == 2049 || table.size() == 2048);
    
    float currentVal;
    auto data = table.data();
    
    int index = (int) m_angle;
    
    float val_L = data[index];
    float val_H = data[index + 1];
    float frac = m_angle - (float) index;
    
    currentVal = val_L + (val_H - val_L)*frac;

    return currentVal;
}




void SynthVoice::setFrequency(float frequency)
{
    m_currentFreq = frequency;
    float cyclesPerSample = frequency / getSampleRate();
    m_angleDelta = cyclesPerSample * (float) m_tableSize;
}

void SynthVoice::updateAngle()
{
    m_angle += m_angleDelta;
    if(m_angle >= m_tableSize)
        m_angle -= m_tableSize;
};

//Returns interpolated value based on interpolation value and tables given
//This version isn't used, instead the below version is used rendering loop


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
    
    attCurveParam = apvts->getRawParameterValue("env_att_curve");
    decCurveParam = apvts->getRawParameterValue("env_dec_curve");
    relCurveParam = apvts->getRawParameterValue("env_rel_curve");
}

void SynthVoice::setMipMap(int displayID, std::shared_ptr<const MipMap> newMipMap)
{
    //DisplayID !!!! ID Index starts at 0!
    if (displayID == 1)
        std::atomic_store(&m_mipmapB, newMipMap);
    else
        std::atomic_store(&m_mipmapA, newMipMap);
}


void SynthVoice::reportMipMaps()
{
    auto tableSize1 = m_mipmapA->size();
    
    int i = 0;
    for (auto stages : m_mipmapA->getMipMap()) {
        juce::Logger::writeToLog("Table " + juce::String(i) + " Size = " + juce::String(stages.size()));
        i++;
    }
    
    juce::Logger::writeToLog("Total Stages = " + juce::String(i));
    
    juce::Logger::writeToLog("Values 100 - 150: " + juce::String(tableSize1));
    
    for (auto i = 0; i < 50; i++) {
        auto val = "Implement this";
        juce::Logger::writeToLog(juce::String(i + 100) + " = " + juce::String(val));
    }

};



float SynthVoice::calculateBendFreq()
{
    //Pitch wheel values 0 - 8192 - 16384
    
    float bendSemitones = m_pitchBendRange * ((m_pitchBend - 8192.0f) / 8192.0f);
    float pitchMultiplier = std::pow(2.0f, bendSemitones /12.0f);
    
    return m_baseFreq * pitchMultiplier;
    
}



//Unused Pure Virtual Functions
void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    juce::Logger::writeToLog("Mod Wheel Value: " + juce::String(newControllerValue));
};

