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
    s_interpolation.setRange(0.0, 1.0);
    s_interpolation.onValueChange = [this] {
        m_interpDisplay.setInterpolation(s_interpolation.getValue());
        m_interpDisplay.repaint();
    };
    
    interpolationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "interpolation", s_interpolation);
    
    addAndMakeVisible(s_interpolation);
    
    //========Other GUI==============//
    addAndMakeVisible(waveBankOne);
    addAndMakeVisible(waveBankTwo);
    
    waveBankOne.addItem("Sine", 1);
    waveBankOne.addItem("Sawtooth", 2);
    
    waveBankTwo.addItem("Sine", 1);
    waveBankTwo.addItem("Sawtooth", 2);
    
    waveBankOne.onChange = [this] {
        int selectedId = waveBankOne.getSelectedId();
        selectNewTable(selectedId);
    };
    
    waveBankTwo.onChange = [this] {
        int selectedId = waveBankTwo.getSelectedId();
        selectNewTable(selectedId);
    };
    
    //========Displays=============//
    
    addAndMakeVisible(m_interpDisplay);
    addAndMakeVisible(m_displayOne);
    addAndMakeVisible(m_displayTwo);
    
    //Set Default Table Display
    m_interpDisplay.setTable(audioProcessor.getTable(0));
    m_interpDisplay.setTableTwo(audioProcessor.getTable(1));
    
    m_displayOne.setTable(audioProcessor.getTable(0));
    m_displayTwo.setTable(audioProcessor.getTable(1));
    
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
    int slider_y = getHeight() - (keyHeight + padding * 2 + sliderHeight);
    
    int combo_width = getWidth() - (display_width + dx + padding * 2);
    int combo_height = 50;
    
    auto mainWindowBounds = getLocalBounds();
    auto keyBedBounds = mainWindowBounds.removeFromBottom(keyHeight);
    
    auto middleBounds = mainWindowBounds.removeFromBottom(sliderHeight + padding * 2);
    
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
    
    
    auto centerPoint = middleBounds.getCentre();
    
    auto sliderBounds = juce::Rectangle<int>(sliderWidth, sliderHeight).withCentre(centerPoint);
    s_interpolation.setBounds(sliderBounds);
    

//    juce::FlexBox mainWindow;
//
//    mainWindow.flexDirection = juce::FlexBox::Direction::row;
//
//
//    juce::FlexBox leftComponent;
//    leftComponent.flexDirection = juce::FlexBox::Direction::column;
//    leftComponent.items.add(juce::FlexItem(m_displayOne).withFlex(1.0));
//    leftComponent.items.add(juce::FlexItem(waveBankOne).withFlex(1.0));
//
//    juce::FlexBox rightComponent;
//    rightComponent.flexDirection = juce::FlexBox::Direction::column;
//    rightComponent.items.add(juce::FlexItem(m_displayTwo).withFlex(1.0));
//    rightComponent.items.add(juce::FlexItem(waveBankTwo).withFlex(1.0));
//
//    mainWindow.items.add(juce::FlexItem(leftComponent).withFlex(1.0));
//    mainWindow.items.add(juce::FlexItem(m_interpDisplay).withFlex(2.0));
//    mainWindow.items.add(juce::FlexItem(rightComponent).withFlex(1.0));
    
//    m_interpDisplay.setBounds(dx, dy, display_width, display_height);
//
//    s_interpolation.setBounds(0 + padding, slider_y, sliderWidth, sliderHeight);
//
//    waveBankOne.setBounds(getWidth() - (combo_width + padding), padding, combo_width, combo_height);
    
//    mainWindow.performLayout(mainWindowBounds);
//    leftComponent.performLayout(mainWindowBounds);
//    rightComponent.performLayout(mainWindowBounds);

    keyboardComponent.setBounds(0 + padding, getHeight() - (keyHeight + padding), getWidth() - 2 * padding, keyHeight);
}

void WaveTablePluginAudioProcessorEditor::selectNewTable(int itemId)
{
    juce::Logger::writeToLog("New Wavetable Selcted! YAY! OMG YES! NICE! YOU SELECTED >>> " + juce::String(itemId) + "!");
    
    //Update Preview Display and Interpoalated Display
    
    auto newTable = audioProcessor.getTable(itemId - 1);
    m_interpDisplay.setTable(newTable);
    m_displayOne.setTable(newTable);
}

//Midi Keyboard Note Callback Functions
void WaveTablePluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}
