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
                       )
        , synth(m_keystate)
#endif
{
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
    synth.prepareToPlay(samplesPerBlock, sampleRate);
}

void WaveTablePluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    synth.releaseResources();
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
    
    //Wrap AudioBuffer in SourceChannelInfo for SynthAudioSource
    juce::AudioSourceChannelInfo channelData;
    channelData.buffer = &buffer;
    channelData.startSample = 0;
    channelData.numSamples = buffer.getNumSamples();
    
    synth.getNextAudioBlock(channelData);
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

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveTablePluginAudioProcessor();
}
