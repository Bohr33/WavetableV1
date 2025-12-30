/*
  ==============================================================================

    ADSR.cpp
    Created: 29 Dec 2025 7:09:57pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "ADSR.h"


ADSR::ADSR()
{
    Parameters defaultParams;
    setParameters(defaultParams);
};

ADSR::~ADSR() = default;


bool ADSR::isActive()
{
    return m_currentStage != Stage::Idle;
}

void ADSR::noteOn()
{
    //Initialize parameters and Update Stage
    stageProgress = 0.0f;
    stageSamples = m_attack * m_sampleRate;
    m_currentStage = Stage::Attack;
    
    juce::Logger::writeToLog("Stage Samples = " + juce::String(stageSamples));
    
}

void ADSR::noteOff()
{
    stageProgress = 0.0f;
    stageSamples = m_release * m_sampleRate;
    m_currentStage = Stage::Release;
};

float ADSR::getNextSample()
{

    switch (m_currentStage) {
        case Stage::Idle:
            return 0.0f;
            
        case Stage::Attack:
            stageProgress += (1.0f / stageSamples);
            m_currentLevel = stageProgress;
            if (stageProgress >= 1.0f)
            {
                m_currentStage = Stage::Decay;
                stageProgress = 0.0f;
                stageSamples = m_decay * m_sampleRate;
                juce::Logger::writeToLog("Done With Attack");
            }
            break;
        case Stage::Decay:
            stageProgress += 1.0f / stageSamples;
            m_currentLevel = 1.0 - ((1.0 - m_sustain) * stageProgress);
            if(stageProgress >= 1.0f)
            {
                m_currentStage = Stage::Sustain;
                m_currentLevel = m_sustain;
                juce::Logger::writeToLog("Done With Decay");
            }
            break;
        case Stage::Sustain:
            m_currentLevel = m_sustain;
            break;
        case Stage::Release:
            stageProgress += 1.0 / stageSamples;
            m_currentLevel = m_sustain * (1.0f - stageProgress);
            
            if(stageProgress >= 1.0f)
            {
                m_currentStage = Stage::Idle;
                m_currentLevel = 0.0f;
                juce::Logger::writeToLog("Done With Release");
            }
            break;
        
    }
    
    return m_currentLevel;
};


void ADSR::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
}


void ADSR::setParameters(Parameters params)
{
    m_attack = params.attackTime;
    m_decay = params.decayTime;
    m_sustain = params.sustainLevel;
    m_release = params.releaseTime;
    
    m_attackCurve = params.attackCurve;
    m_decayCurve = params.decayCurve;
    m_releaseCurve = params.releaseCurve;
    
}

void ADSR::setParameter(Param param, float value)
{
    auto val = juce::jlimit(0.0f, 1.0f, value);
    switch (param) {
        case Param::AttackTime:
            m_attack = val;
            break;
        case Param::DecayTime:
            m_decay = val;
            break;
        case Param::SustainLevel:
            m_sustain = val;
            break;
        case Param::ReleaseTime:
            m_release = val;
            break;
        case Param::AttackCurve:
            m_attackCurve = val;
            break;
        case Param::DecayCurve:
            m_decayCurve = val;
            break;
        case Param::ReleaseCurve:
            m_releaseCurve = val;
            break;
        default:
            break;
    }
  
    
    
};


