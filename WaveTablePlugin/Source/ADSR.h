/*
  ==============================================================================

    ADSR.h
    Created: 29 Dec 2025 7:09:57pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>




class ADSR
{
public:
    enum class Param {
        AttackTime, DecayTime, SustainLevel, ReleaseTime,
        AttackCurve, DecayCurve, ReleaseCurve
    };
    
    struct Parameters {
        float attackTime = 0.01f;
        float decayTime = 0.1f;
        float sustainLevel = 0.7f;
        float releaseTime = 0.5f;
        float attackCurve = 1.0f;
        float decayCurve = 1.0f;
        float releaseCurve = 1.0f;
    };
    
    
    ADSR();
    ~ADSR();
    
    void noteOn();
    void noteOff();
    
    bool isActive();
    
    float getNextSample();
    
    void setSampleRate(double sampleRate);
    void setParameters(Parameters params);
    void setParameter(Param param, float value);
    
    
private:
  
    enum class Stage{Idle, Attack, Decay, Sustain, Release };
    
    //Private Member Variables
    Stage m_currentStage = Stage::Idle;
    float m_currentLevel = 0.0f;
    double m_sampleRate;
    
    //Stage Variables
    float stageProgress = 0.0f;
    float stageSamples = 0.0f;
    
    //Parameters
    float m_attack, m_decay, m_sustain, m_release;
    float m_attackCurve, m_decayCurve, m_releaseCurve;
    
};
