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
    setSize (900, 500);
//    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);
    
    juce::LookAndFeel::setDefaultLookAndFeel(&laf);
    
    //============Sliders==========//
    s_interpolation.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_interpolation.setRange(0.0f, 1.0f);
    s_interpolation.onValueChange = [this] {
        m_interpDisplay.setInterpolation(s_interpolation.getValue());
        m_interpDisplay.repaint();
        
        m_interpWaveDisplay.setInterpolation(s_interpolation.getValue());
        m_interpWaveDisplay.repaint();

    };
    
    interpolationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "interpolation", s_interpolation);
    
    addAndMakeVisible(s_interpolation);
    
    
    //ADSR
    s_envAttack.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_envAttack.setRange(0.0f, 1.0f);
    envAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_attack", s_envAttack);
    addAndMakeVisible(s_envAttack);
    s_envAttack.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_envDecay.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_envDecay.setRange(0.0f, 1.0f);
    envDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_decay", s_envDecay);
    addAndMakeVisible(s_envDecay);
    s_envDecay.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_envSustain.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_envSustain.setRange(0.0f, 1.0f);
    envSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_sustain", s_envSustain);
    addAndMakeVisible(s_envSustain);
    s_envSustain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_envRelease.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s_envRelease.setRange(0.0f, 1.0f);
    envReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_release", s_envRelease);
    addAndMakeVisible(s_envRelease);
    s_envRelease.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    //ADSR Slope Controls
    s_attCurve.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    attCurveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_att_curve", s_attCurve);
    addAndMakeVisible(s_attCurve);
    s_attCurve.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_decCurve.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    decCurveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_dec_curve", s_decCurve);
    addAndMakeVisible(s_decCurve);
    s_decCurve.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    s_relCurve.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    relCurveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "env_rel_curve", s_relCurve);
    addAndMakeVisible(s_relCurve);
    s_relCurve.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    
    //Filter Sliders
    s_cutoff.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "filter_cutoff", s_cutoff);
    addAndMakeVisible(s_cutoff);
    s_cutoff.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    
    s_resonance.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    resonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "filter_resonance", s_resonance);
    addAndMakeVisible(s_resonance);
    s_resonance.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    
    //ADSR GUI Slider Calls
    
    s_envAttack.onValueChange = [this](){
        updateADSRDisplay();
    };
    
    s_envDecay.onValueChange = [this](){
        updateADSRDisplay();
    };
    
    s_envSustain.onValueChange = [this](){
        updateADSRDisplay();
    };
    
    s_envRelease.onValueChange = [this](){
        updateADSRDisplay();
    };
    
    s_attCurve.onValueChange = [this](){
        updateADSRDisplay();
    };
    
    s_decCurve.onValueChange = [this](){
        updateADSRDisplay();
    };
    
    s_relCurve.onValueChange = [this](){
        updateADSRDisplay();
    };

    
    //========Other GUI==============//
    addAndMakeVisible(waveBankOne);
    addAndMakeVisible(waveBankTwo);
    addAndMakeVisible(waveTableSelect);
    
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
        juce::Logger::writeToLog("Change on wave frame one");
    };
    
    waveBankTwo.onChange = [this] {
        int selectedId = waveBankTwo.getSelectedId();
        selectNewWaveformTableTwo(selectedId-1);
        juce::Logger::writeToLog("Change on wave frame two");
    };
    
    
    
    //New wave bank gui, not yet implemented?
    waveTableSelect.addItem("Basic 1", 1);
    waveTableSelect.addItem("Rename 2", 2);
    waveTableSelect.addItem("Pontific 3", 3);
    
    waveTableSelect.onChange = [this] {
        int selectedId = waveTableSelect.getSelectedId() - 1;
        selectNewWavetable(selectedId);
        juce::Logger::writeToLog("Selected new wave bank");
    };
    
    //ends here
    
    
    
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
    
//    addAndMakeVisible(m_interpDisplay);
    addAndMakeVisible(m_interpWaveDisplay);
    addAndMakeVisible(m_displayOne);
    addAndMakeVisible(m_displayTwo);
    addAndMakeVisible(m_envelopeDisplay);
    
    //Set Default Table Display for Left and Right Tables
    auto defaultTableOne = audioProcessor.getFrameForDisplay(0, 0);
    auto defaultTableTwo = audioProcessor.getFrameForDisplay(1, 0);
    
    //Set wavetable for Main interpolating display
    auto defaultWavetableOne = audioProcessor.getWavetableForDisplay(0);
    
    for(int i = 0; i < 50; i++)
    {
        juce::Logger::writeToLog("Val = " + juce::String(defaultWavetableOne[0][i]));
    }

//    m_interpDisplay.setTable(defaultTableOne);
//    m_interpDisplay.setTableTwo(defaultTableTwo);
    
    m_interpWaveDisplay.setWavetable(defaultWavetableOne);

    
    m_displayOne.setTable(defaultTableOne);
    m_displayTwo.setTable(defaultTableTwo);
    
//    m_interpDisplay.setColours(juce::Colours::rebeccapurple);
    m_interpWaveDisplay.setColours(juce::Colours::black);
    m_displayOne.setColours(juce::Colours::gold);
    m_displayTwo.setColours(juce::Colours::peru);
}

WaveTablePluginAudioProcessorEditor::~WaveTablePluginAudioProcessorEditor()
{
    keyboardState.removeListener(this);
    //Must Clean up look and feel after using custom
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void WaveTablePluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
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
    
    
    // Begin Bounds Definitions
    auto mainWindowBounds = getLocalBounds();
    
    auto bottomThirdBounds = mainWindowBounds.removeFromBottom(keyHeight);

    auto middleBounds = mainWindowBounds.removeFromBottom(sliderHeight + padding * 2);
        
    auto quarterWidth = mainWindowBounds.getWidth()/4.0;
    
    auto leftQuarterBounds = mainWindowBounds.removeFromLeft(quarterWidth);
    auto rightQuarterBounds = mainWindowBounds.removeFromRight(quarterWidth);
    
    auto leftMiddle = middleBounds.removeFromLeft(quarterWidth);
    auto rightMiddle = middleBounds.removeFromRight(quarterWidth);
    
    auto bottomLeftQuarter = bottomThirdBounds.removeFromLeft(quarterWidth);
    auto bottomRightQuarter = bottomThirdBounds.removeFromRight(quarterWidth);
    
    //----------Set Display & Combo Box Bounds------------///
//    m_interpDisplay.setBounds(mainWindowBounds);
    m_interpWaveDisplay.setBounds(mainWindowBounds);
    
    auto quarterHalf = leftQuarterBounds.getHeight()/2;
    auto leftTopBounds = leftQuarterBounds.removeFromTop(quarterHalf);
    m_displayOne.setBounds(leftTopBounds);
    waveBankOne.setBounds(leftQuarterBounds);
    
    auto rightTopBounds = rightQuarterBounds.removeFromTop(quarterHalf);
    m_displayTwo.setBounds(rightTopBounds);
    waveBankTwo.setBounds(rightQuarterBounds);
    

    auto miscComboBounds = bottomRightQuarter;
    waveTableSelect.setBounds(miscComboBounds);
    
    
    //--- ADSR Display ----------
    
    //ADSR Sliders
    juce::FlexBox fb_adsrSliders;
    
    fb_adsrSliders.flexDirection = juce::FlexBox::Direction::row;
    
    fb_adsrSliders.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    fb_adsrSliders.items.add(juce::FlexItem(s_envAttack).withFlex(1));
    fb_adsrSliders.items.add(juce::FlexItem(s_envDecay).withFlex(1));
    fb_adsrSliders.items.add(juce::FlexItem(s_envSustain).withFlex(1));
    fb_adsrSliders.items.add(juce::FlexItem(s_envRelease).withFlex(1));
    
    
    auto middleSlidersBounds = middleBounds.removeFromBottom(middleBounds.getHeight()/3.0f);
    
    fb_adsrSliders.performLayout (middleSlidersBounds);
    m_envelopeDisplay.setBounds(middleBounds);
        
    s_attCurve.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_decCurve.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    s_relCurve.setBounds(rightMiddle.removeFromLeft(verticalSliderWidth));
    
    
    //Filter Sliders
    int filterSliderHeight = 60;
    
    juce::FlexBox fb_sliders;
    fb_sliders.flexDirection = juce::FlexBox::Direction::row;
    fb_sliders.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    fb_sliders.alignItems = juce::FlexBox::AlignItems::flexStart;
    
    fb_sliders.items.add(juce::FlexItem(s_cutoff).withFlex(1).withHeight(filterSliderHeight));
    fb_sliders.items.add(juce::FlexItem(s_resonance).withFlex(1).withHeight(filterSliderHeight));
    fb_sliders.performLayout(bottomLeftQuarter);
    
    
    
    
    
    
    
    //Left Middle
    juce::FlexBox fb;
    fb.flexDirection = juce::FlexBox::Direction::column;
    fb.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    fb.alignItems = juce::FlexBox::AlignItems::flexStart;
    
    fb.items.add(juce::FlexItem(load_table_btn).withHeight(btn_height).withWidth(btn_width));
    fb.items.add(juce::FlexItem(s_interpolation).withHeight(sliderHeight).withWidth(sliderWidth));
    
    fb.performLayout(leftMiddle);
}



void WaveTablePluginAudioProcessorEditor::selectNewWaveformTableOne(int waveformID)
{
//    audioProcessor.setWaveform(0, waveformID);
    
    juce::Logger::writeToLog("Setting New Waveform for Table One");
    
    
    //The getMipMap function was re-done to return the bank from the wavebank manager
    std::vector<float> displayTable = audioProcessor.getFrameForDisplay(0, waveformID);
    
    m_displayOne.setTable(displayTable);
    m_interpDisplay.setTable(displayTable);
    
    m_displayOne.repaint();
    m_interpDisplay.repaint();
}


void WaveTablePluginAudioProcessorEditor::selectNewWaveformTableTwo(int waveformID)
{
    std::vector<float> displayTable = audioProcessor.getFrameForDisplay(1, waveformID);
    
    juce::Logger::writeToLog("Setting New Waveform for Table Two");
    
//    audioProcessor.setWaveform(1, waveformID);
    
    m_displayTwo.setTable(displayTable);
    m_interpDisplay.setTableTwo(displayTable);
    
    m_displayTwo.repaint();
    m_interpDisplay.repaint();
}


void WaveTablePluginAudioProcessorEditor::selectNewWavetable(int wavetableID)
{
    auto display = audioProcessor.getWavetableForDisplay(wavetableID);
    
    juce::Logger::writeToLog("Setting New Wavetable");
    
    audioProcessor.setWavetable(wavetableID);
    m_interpWaveDisplay.setWavetable(display);
    m_interpWaveDisplay.repaint();
}


void WaveTablePluginAudioProcessorEditor::updateADSRDisplay()
{
    m_envelopeDisplay.setParameters({
        (float)s_envAttack.getValue(),
        (float)s_envDecay.getValue(),
        (float)s_envSustain.getValue(),
        (float)s_envRelease.getValue(),
        (float)s_attCurve.getValue(),
        (float)s_decCurve.getValue(),
        (float)s_relCurve.getValue()
    }
                                    );
}




//Midi Keyboard Note Callback Functions
void WaveTablePluginAudioProcessorEditor::handleNoteOn(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

void WaveTablePluginAudioProcessorEditor::handleNoteOff(juce::MidiKeyboardState* state, int midiChannel, int midiNoteNumber, float velocity)
{
    
}

