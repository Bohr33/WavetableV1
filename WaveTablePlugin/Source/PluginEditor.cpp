/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveTablePluginAudioProcessorEditor::WaveTablePluginAudioProcessorEditor (WaveTablePluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(p.getMidiKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 500);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    
    //============Sliders==========//
    s_interpolation.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_interpolation.setRange(0.0f, 1.0f);
    s_interpolation.onValueChange = [this] {
        m_interpDisplay.setInterpolation(s_interpolation.getValue());
        m_interpDisplay.repaint();
    };
    
    interpolationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "interpolation", s_interpolation);
    
    addAndMakeVisible(s_interpolation);
    
    
    //ADSR
    s_envAttack.setSliderStyle(juce::Slider::LinearVertical);
    s_envAttack.setRange(0.0f, 1.0f);
    envAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_attack", s_envAttack);
    addAndMakeVisible(s_envAttack);
    s_envAttack.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_envDecay.setSliderStyle(juce::Slider::LinearVertical);
    s_envDecay.setRange(0.0f, 1.0f);
    envDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_decay", s_envDecay);
    addAndMakeVisible(s_envDecay);
    s_envDecay.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_envSustain.setSliderStyle(juce::Slider::LinearVertical);
    s_envSustain.setRange(0.0f, 1.0f);
    envSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_sustain", s_envSustain);
    addAndMakeVisible(s_envSustain);
    s_envSustain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_envRelease.setSliderStyle(juce::Slider::LinearVertical);
    s_envRelease.setRange(0.0f, 1.0f);
    envReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_release", s_envRelease);
    addAndMakeVisible(s_envRelease);
    s_envRelease.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    //ADSR Slope Controls
    s_attCurve.setSliderStyle(juce::Slider::LinearVertical);
    attCurveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_att_curve", s_attCurve);
    addAndMakeVisible(s_attCurve);
    s_attCurve.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_decCurve.setSliderStyle(juce::Slider::LinearVertical);
    decCurveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_dec_curve", s_decCurve);
    addAndMakeVisible(s_decCurve);
    s_decCurve.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_relCurve.setSliderStyle(juce::Slider::LinearVertical);
    relCurveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_rel_curve", s_relCurve);
    addAndMakeVisible(s_relCurve);
    s_relCurve.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    
    //========Other GUI==============//
    addAndMakeVisible(waveBankOne);
    addAndMakeVisible(waveBankTwo);
    
    waveBankOne.addItem("Sine", 1);
    waveBankOne.addItem("Triangle", 2);
    waveBankOne.addItem("Sawtooth", 3);
    waveBankOne.addItem("Square", 4);
    
    waveBankTwo.addItem("Sine", 1);
    waveBankTwo.addItem("Triangle", 2);
    waveBankTwo.addItem("Sawtooth", 3);
    waveBankTwo.addItem("Square", 4);
    
    waveBankOne.onChange = [this] {
        int selectedId = waveBankOne.getSelectedId();
        selectNewWaveformTableOne(selectedId-1);
        juce::Logger::writeToLog("Change on wave bank one");
    };
    
    waveBankTwo.onChange = [this] {
        int selectedId = waveBankTwo.getSelectedId();
        selectNewWaveformTableTwo(selectedId-1);
        juce::Logger::writeToLog("Change on wave bank two");
    };
    
    
    
    addAndMakeVisible(load_table_btn);
    
    load_table_btn.onClick = [this] {
        chooser = std::make_unique<juce::FileChooser>(
            "Select a wavetable...",
            juce::File{},
            "*.wav"
        );

    chooser->launchAsync(juce::FileBrowserComponent::canSelectFiles, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                //Get total number of files added, and add to display
                int numNewFiles = audioProcessor.loadWavetableFile(file);
                
                for(int i = 1; i <= numNewFiles; i++)
                {
                    waveBankOne.addItem("Import " + juce::String(i), 4 + i);
                    waveBankTwo.addItem("Import " + juce::String(i), 4 + i);
                
                }
                
            }
        });
        
    };
    
    
    //========Displays=============//
    
    addAndMakeVisible(m_interpDisplay);
    addAndMakeVisible(m_displayOne);
    addAndMakeVisible(m_displayTwo);
    
    //Set Default Table Display
    
    
    m_interpDisplay.setTable(audioProcessor.getMipMap(0)->getStage(0));
    m_interpDisplay.setTableTwo(audioProcessor.getMipMap(1)->getStage(1));
    
    m_displayOne.setTable(audioProcessor.getMipMap(0)->getStage(0));
    m_displayTwo.setTable(audioProcessor.getMipMap(1)->getStage(1));
    
    m_interpDisplay.setColours(juce::Colours::rebeccapurple);
    m_displayOne.setColours(juce::Colours::gold);
    m_displayTwo.setColours(juce::Colours::peru);
}

WaveTablePluginAudioProcessorEditor::~WaveTablePluginAudioProcessorEditor()
{
    keyboardState.removeListener(this);
}

//==============================================================================
void WaveTablePluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
//    g.drawFittedText ("The Current Of Now", getLocalBounds(), juce::Justification::centred, 1);
}

void WaveTablePluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    int padding = 20;
    
    int display_width = getWidth()/2;
    int display_height = getHeight()/3;
    
    int dx = display_width - display_width/2;
    int dy = display_height/3;
    
    int keyHeight = getHeight()/4.0;
    
    
    int sliderHeight = 150;
    int sliderWidth = 150;
    int slider_y = getHeight() - (keyHeight + padding);
    
    auto btn_height = 20;
    auto btn_width = 50;
    auto btn_y = slider_y + btn_height + padding;
    
    
    
    int verticalSliderHeight = 150;
    int verticalSliderWidth = 50;
    int vertSliderTextHeight = 20;
    
    int combo_width = getWidth() - (display_width + dx + padding * 2);
    int combo_height = 50;
    
    auto mainWindowBounds = getLocalBounds();
    auto keyBedBounds = mainWindowBounds.removeFromBottom(keyHeight);
    
    auto middleBounds = mainWindowBounds.removeFromBottom(sliderHeight + padding * 2);
    
    
    auto leftMiddle = middleBounds.removeFromLeft(middleBounds.getWidth()/4.0f);
    auto rightMiddle = middleBounds;
    
    
    auto quarterWidth = mainWindowBounds.getWidth()/4.0;
    

    
    
    
    auto leftQuarterBounds = mainWindowBounds.removeFromLeft(quarterWidth);
    auto rightQuarterBounds = mainWindowBounds.removeFromRight(quarterWidth);
    
    m_interpDisplay.setBounds(mainWindowBounds);
    
    auto quarterHalf = leftQuarterBounds.getHeight()/2;
    auto leftTopBounds = leftQuarterBounds.removeFromTop(quarterHalf);
    m_displayOne.setBounds(leftTopBounds);
    waveBankOne.setBounds(leftQuarterBounds);
    
    auto rightTopBounds = rightQuarterBounds.removeFromTop(quarterHalf);
    m_displayTwo.setBounds(rightTopBounds);
    waveBankTwo.setBounds(rightQuarterBounds);
    
    
    //ADSR Sliders
    rightMiddle.removeFromBottom(vertSliderTextHeight);
    
    s_envAttack.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_envDecay.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_envSustain.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_envRelease.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    
    s_attCurve.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_decCurve.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_relCurve.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    fb.alignItems = juce::FlexBox::AlignItems::flexStart;
    
    fb.items.add(juce::FlexItem(load_table_btn).withHeight(btn_height).withWidth(btn_width));
    fb.items.add(juce::FlexItem(s_interpolation).withHeight(sliderHeight).withWidth(sliderWidth));
    
    fb.performLayout(leftMiddle);
    
    

    keyboardComponent.setBounds(0 + padding, getHeight() - (keyHeight + padding), getWidth() - 2 * padding, keyHeight);
}



void WaveTablePluginAudioProcessorEditor::selectNewWaveformTableOne(int waveformID)
{
    auto mipmap = audioProcessor.getMipMap(waveformID);
    
    if(!mipmap)
        return;
    
    auto firstStage = mipmap->getStage(0);
    
    
    
    for(int i = 0; i < 2048; i++)
    {
        juce::Logger::writeToLog("Val at " + juce::String(i) + " = " + juce::String(firstStage[i]));
    }
    
    audioProcessor.setWaveform(0, waveformID);
    
    
    m_displayOne.setTable(firstStage);
    m_interpDisplay.setTable(firstStage);
    
    m_displayOne.repaint();
    m_interpDisplay.repaint();
    
}


void WaveTablePluginAudioProcessorEditor::selectNewWaveformTableTwo(int waveformID)
{
    auto mipmap = audioProcessor.getMipMap(waveformID);
    
    if(!mipmap)
        return;
    
    auto firstStage = mipmap->getStage(0);
    
    
    audioProcessor.setWaveform(1, waveformID);
    
    m_displayTwo.setTable(firstStage);
    m_interpDisplay.setTableTwo(firstStage);
    
    m_displayTwo.repaint();
    m_interpDisplay.repaint();

}

//Midi Keyboard Note Callback Functions
void WaveTablePluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

