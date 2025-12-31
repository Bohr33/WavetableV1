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
    generateWavetableBank();
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
    
    //initialize members for the actual synth, including the sound and all voices
    synth.setCurrentPlaybackSampleRate(sampleRate);
    
    synth.clearSounds();
    synth.clearVoices();
    
    
    //add sound to synth
    synth.addSound(new WaveTableSound());
    
    auto defaultTableOne = wavetableBank[0];
    auto defaultTableTwo = wavetableBank[1];
    
    for(auto i = 0; i < maxVoices; ++i)
    {
        //add voice to synth; also provides default Table and Table Size to Synth Voice
        auto* voice = new SynthVoice(defaultTableOne, defaultTableTwo, defaultTableSize);
//        voice->setParameters(interpolateParam);
        voice->setAPVTS(&apvts);
        voice->prepare(sampleRate);
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
    
    //Don't forget to clear the buffer!
    buffer.clear();
    
    
    juce::MidiBuffer screenKeyBuffer;
    m_keystate.processNextMidiBuffer(screenKeyBuffer, 0, numSamples, true);
    
    //Combine both buffers by making a new buffer and add both events
    juce::MidiBuffer    combinedMidi;
    
//    for(const auto metadata : midiMessages){
//        auto message = metadata.getMessage();
//        juce::Logger::writeToLog("MIDI: " + message.getDescription());
//    }
    
    
    combinedMidi.addEvents(midiMessages, 0, numSamples, 0);
    combinedMidi.addEvents(screenKeyBuffer, 0, numSamples, 0);
    

    
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


//Updates the pointers to new tables within Synth Voice
void WaveTablePluginAudioProcessor::setWaveform(int tableID, int waveformID)
{
    juce::Logger::writeToLog("Setting new table in voices; ID = " + juce::String(waveformID));
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->setWavetable(tableID, wavetableBank[waveformID]);
        }
            
    }
}

//Returns a table from the wavetable bank
std::shared_ptr<const TableData> WaveTablePluginAudioProcessor::getTable(int tableID)
{
    //Logic to select table from ID
    juce::Logger::writeToLog("Gettting new Table!");
    
    if(tableID > 0 && tableID < 4)
        return wavetableBank[tableID];
    else
        return wavetableBank[0];
    
}

//Is called on Construction, generates the basic wavetable shapes
void WaveTablePluginAudioProcessor::generateWavetableBank()
{
    using GenFunc = void (WavetableGenerator::*)(std::span<float>, int);
    
    static const std::vector<GenFunc> generatorFuncs = {
        &WavetableGenerator::genSine,
        &WavetableGenerator::genTri,
        &WavetableGenerator::genSaw,
        &WavetableGenerator::genSquare
    };
    
    for (auto func : generatorFuncs) {
        auto table = std::make_shared<TableData>();
        table->samples.resize(defaultTableSize + 1);
        (tableGenerator.*func)(table->samples, defaultNumHarmonics);
        wavetableBank.push_back(table);
    }
}

//Helper function to create parameter layout for AudioValueTreeState
juce::AudioProcessorValueTreeState::ParameterLayout WaveTablePluginAudioProcessor::createParameterLayout()
{
    //Here is where the ranges and ID's are defined for each GUI parameter. Here the skew values for sliders
    //can be altered by using a juce::NormalisableRange
    
    int versionHint = 1;
    
    using namespace juce;
    
    float skew = std::log(0.5) / std::log((1.0 - 0.1) / (10.0 - 0.1));
    
    return
    {
        std::make_unique<AudioParameterFloat>(ParameterID {"interpolation", versionHint}, "Interpolation", 0.0f, 1.0f, 0.5f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_attack", versionHint}, "Envelope Attack", 0.0f, 1.0f, 0.01f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_decay", versionHint}, "Envelope Decay", 0.0f, 1.0f, 0.1f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_sustain", versionHint}, "Envelope Sustain", 0.0f, 1.0f, 0.7f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_release", versionHint}, "Envelope Release", 0.0f, 1.0f, 0.7f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_att_curve", versionHint}, "Attack Slope", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, skew), 1.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_dec_curve", versionHint}, "Decay Slope", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, skew), 1.0f),
        std::make_unique<AudioParameterFloat>(ParameterID {"env_rel_curve", versionHint}, "Release Slope", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, skew), 1.0f)
    };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveTablePluginAudioProcessor();
}
