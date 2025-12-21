/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveTablePluginAudioProcessor::WaveTablePluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    
//    //add sound to synth
//    synth.addSound(new WaveTableSound(m_table));
//
//    for(auto i = 0; i < maxVoices; ++i)
//    {
//        //add voice to synth
//        synth.addVoice(new SynthVoice(m_table, m_table2, defaultTableSize));
//    }
}

WaveTablePluginAudioProcessor::~WaveTablePluginAudioProcessor()
{
}

//==============================================================================
const juce::String WaveTablePluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaveTablePluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WaveTablePluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WaveTablePluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WaveTablePluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaveTablePluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WaveTablePluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaveTablePluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WaveTablePluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void WaveTablePluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WaveTablePluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    generateWavetable(m_table, defaultTableSize);
    generateSawtooth(m_table2, defaultTableSize);
    
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    synth.clearSounds();
    synth.clearVoices();
    
    //add sound to synth
    synth.addSound(new WaveTableSound(m_table));

    auto* interpolateParam = apvts.getRawParameterValue("interpolation");
    
    for(auto i = 0; i < maxVoices; ++i)
    {
        //add voice to synth
        auto* voice = new SynthVoice(m_table, m_table2, defaultTableSize);
        voice->setParameters(interpolateParam);
        synth.addVoice(voice);
    }
    
    midiCollector.reset(sampleRate);
    
}

void WaveTablePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
//    synth.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveTablePluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void WaveTablePluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    
    auto numSamples = buffer.getNumSamples();
    
    
    juce::MidiBuffer screenKeyBuffer;
    m_keystate.processNextMidiBuffer(screenKeyBuffer, 0, numSamples, true);
    
    //Combine both buffers by making a new buffer and add both events
    juce::MidiBuffer    combinedMidi;
    combinedMidi.addEvents(midiMessages, 0, numSamples, 0);
    combinedMidi.addEvents(screenKeyBuffer, 0, numSamples, 0);
    
    //Don't forget to clear the buffer!
    buffer.clear();
    
    synth.renderNextBlock(buffer, combinedMidi, 0, numSamples);
    
    midiCollector.removeNextBlockOfMessages(screenKeyBuffer, numSamples);
}

//==============================================================================
bool WaveTablePluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WaveTablePluginAudioProcessor::createEditor()
{
    return new WaveTablePluginAudioProcessorEditor (*this);
}

//==============================================================================
void WaveTablePluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WaveTablePluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


//Added Functions
juce::MidiKeyboardState& WaveTablePluginAudioProcessor::getMidiKeyboardState()
{
    return m_keystate;
}

void WaveTablePluginAudioProcessor::setTable(int tableID)
{
    
    
}

std::vector<double>* WaveTablePluginAudioProcessor::getTable(int tableID)
{
    //Logic to select table from ID
    
    std::vector<double>* currentTable;
    
    switch (tableID) {
        case 0:
            currentTable = &m_table;
            break;
        case 1:
            currentTable = &m_table2;
            break;
        default:
            currentTable = &m_table;
            break;
    }
    
    return currentTable;
}

void WaveTablePluginAudioProcessor::generateWavetable(std::vector<double>& bufferToFill, unsigned int size)
{
    //Resize Vector as Size + Guard Point
    bufferToFill.resize(size + 1);
    
    //Prepare Sine Variables for Loop
    double incr = 1.0 / (double)size;
    double angle = 0.0;

    //Loop and add value to vector
    for(auto i = 0; i < size; ++i)
    {
        auto val = std::sin(angle * juce::MathConstants<double>::twoPi);
        bufferToFill[i] = val;
        angle += incr;
    }
    //Guard Point
    bufferToFill[size] = bufferToFill[0];
};

//Sawtooth Generation Function
void WaveTablePluginAudioProcessor::generateSawtooth(std::vector<double>& bufferToFill, unsigned int size)
{
    //Resize Vector as Size + Guard Point
    bufferToFill.resize(size + 1);
    
    //Arbitrary Value of Harmonics
    int numHarmonics = 10;
    
    //Prepare Sine Variables for Loop
    double incr = 0.0;
    double angle = 0.0;

    //Loop and add value to vector
    for (auto harmonic = 1; harmonic <= numHarmonics; harmonic++) {
        
        incr = (double)harmonic / (double)size;
        angle = 0.0;
        
        for(auto i = 0; i < size; ++i)
        {
            auto val = std::sin(angle * juce::MathConstants<double>::twoPi) / (double)harmonic;
            bufferToFill[i] += val;
            angle += incr;
        }
        
    }
    
    
    //Normalize Wave
    auto peak = 0.0;
    for (auto j = 0; j < size; j++)
        peak = std::max(peak, std::abs(bufferToFill[j]));
    
    if(peak > 0.0)
        for (auto k = 0; k < size; k++)
            bufferToFill[k] /= peak;

    //Guard Point
    bufferToFill[size] = bufferToFill[0];
    
    
}

//Helper function to create parameter layout for AudioValueTreeState
juce::AudioProcessorValueTreeState::ParameterLayout WaveTablePluginAudioProcessor::createParameterLayout()
{
    int versionHint = 1;
    
    using namespace juce;
    
    return
    {
        std::make_unique<AudioParameterFloat>(ParameterID {"interpolation", versionHint}, "Interpolation", 0.0f, 1.0f, 0.5f)
    };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveTablePluginAudioProcessor();
}
