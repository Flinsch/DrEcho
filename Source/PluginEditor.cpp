/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "MyLogger.h"

//==============================================================================
DrEchoAudioProcessorEditor::DrEchoAudioProcessorEditor (DrEchoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    const int W = 560;
    const int H = 300;

    const int sw = 70;
    const int sh = 120;

    _add_slider( "dry",         { 0 * (W/4) + (W/4-sw)/2, (H-sh)/2, sw, sh }, " %", true );
    _add_slider( "delay",       { 1 * (W/4) + (W/4-sw)/2, (H-sh)/2, sw, sh }, " secs" );
    _add_slider( "feedback",    { 2 * (W/4) + (W/4-sw)/2, (H-sh)/2, sw, sh }, " %" );
    _add_slider( "wet",         { 3 * (W/4) + (W/4-sw)/2, (H-sh)/2, sw, sh }, " %", true );

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize( W, H );

    constrainer.setSizeLimits( W, H, W, H );
    this->setConstrainer( &constrainer );
}

DrEchoAudioProcessorEditor::~DrEchoAudioProcessorEditor()
{
    // The attachments have to be deleted before their respective components.
    // So let's do this explicitly and let the destructor do the rest.
    for ( auto& p : _component_infos )
    {
        ComponentInfo& ci = p.second;
        ci.component_attachment_wrapper_ptr.reset();
    }
}

//==============================================================================
void DrEchoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    /*g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);*/

    g.setFillType( juce::FillType( juce::Colour( 0xff0b2430 ) ) );
    g.fillRect( 0 * getWidth() / 4, 0, 1 * getWidth() / 4, getHeight() );
    g.setFillType( juce::FillType( juce::Colour( 0xffefefef ) ) );
    g.fillRect( 1 * getWidth() / 4, 0, 2 * getWidth() / 4, getHeight() );
    g.setFillType( juce::FillType( juce::Colour( 0xff0f2f3f ) ) );
    g.fillRect( 3 * getWidth() / 4, 0, 3 * getWidth() / 4, getHeight() );

    g.setFont( juce::Font( "Arial", 32.0f, juce::Font::bold ) );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawSingleLineText( "Dr", 5, 30 );
    g.setColour( juce::Colour( 0x3fffffff ) );
    g.drawSingleLineText( ".", 33, 30 );
    g.setColour( juce::Colour( 0xefffffff ) );
    g.drawSingleLineText( "Echo", 40, 30 );

    g.setFont( juce::Font( "Arial", 14.0f, juce::Font::italic ) );
    g.setColour( juce::Colour( 0x3fffffff ) );
    g.drawSingleLineText( "A plug-in by", 4, getHeight() - 20 );
    g.setFont( juce::Font( "Arial", 14.0f, juce::Font::bold ) );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawSingleLineText( "Stefan Fleischer", 4, getHeight() - 6 );

    g.setFont( juce::Font( "Arial", 14.0f, juce::Font::italic ) );
    g.setColour( juce::Colour( 0x3fffffff ) );
    g.drawSingleLineText( "Version", getWidth() - 4, 14, juce::Justification::right );
    g.setFont( juce::Font( "Arial", 14.0f, juce::Font::bold ) );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawSingleLineText( "1.0.0", getWidth() - 4, 28, juce::Justification::right );
}

void DrEchoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    for ( auto& p : _component_infos )
    {
        ComponentInfo& ci = p.second;
        juce::Component& component = *ci.component_ptr;
        component.setBounds( ci.bounds );
    }
}

juce::Label& DrEchoAudioProcessorEditor::_add_label(const juce::String& text, const juce::Rectangle<int>& bounds, bool meta)
{
    ComponentInfo& ci = _component_infos.emplace( text + " (" + juce::String(::rand()) + ")", ComponentInfo{
        std::make_unique<juce::Label>(),
        bounds,
        nullptr,
    } ).first->second;

    juce::Label& label = *dynamic_cast<juce::Label*>( ci.component_ptr.get() );

    label.setText( text, juce::NotificationType::dontSendNotification );
    label.setJustificationType( juce::Justification::centred );

    label.setColour( juce::Label::backgroundColourId, juce::Colours::transparentBlack );
    label.setColour( juce::Label::textColourId, juce::Colour( meta ? 0xff878787 : 0xff888888 ) );

    addAndMakeVisible( label );

    return label;
}

juce::Slider& DrEchoAudioProcessorEditor::_add_slider(const juce::String& parameterID, const juce::Rectangle<int>& bounds, const juce::String& textValueSuffix, bool meta)
{
    ComponentInfo& ci = _component_infos.emplace( parameterID, ComponentInfo{
        std::make_unique<juce::Slider>(),
        bounds,
        nullptr,
    } ).first->second;

    juce::Slider& slider = *dynamic_cast<juce::Slider*>( ci.component_ptr.get() );
    ci.component_attachment_wrapper_ptr.reset( _create_component_attachment_wrapper<juce::AudioProcessorValueTreeState::SliderAttachment>( parameterID, slider ) );

    ci.bounds.setHeight( bounds.getHeight() - slider.getTextBoxHeight() );

    slider.setSliderStyle( juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag );

    slider.setTextBoxStyle( juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, bounds.getWidth(), slider.getTextBoxHeight() );
    slider.setTextValueSuffix( textValueSuffix );

    //slider.setColour( juce::Slider::backgroundColourId, juce::Colours::red );
    slider.setColour( juce::Slider::thumbColourId, meta ? juce::Colours::white : juce::Colours::black );
    //slider.setColour( juce::Slider::trackColourId, juce::Colours::red );
    slider.setColour( juce::Slider::rotarySliderFillColourId, juce::Colour( meta ? 0xffd7d7d7 : 0xff383838 ) );
    slider.setColour( juce::Slider::rotarySliderOutlineColourId, juce::Colour( meta ? 0xff383838 : 0xffd7d7d7 ) );
    slider.setColour( juce::Slider::textBoxTextColourId, juce::Colour( meta ? 0xffd7d7d7 : 0xff383838 ) );
    slider.setColour( juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack );
    //slider.setColour( juce::Slider::textBoxHighlightColourId, juce::Colours::red );
    slider.setColour( juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack );

    addAndMakeVisible( slider );

    _add_label( audioProcessor.apvts.getParameter( parameterID )->getLabel(), { bounds.getX(), bounds.getBottom() - slider.getTextBoxHeight(), bounds.getWidth(), slider.getTextBoxHeight() }, meta );

    return slider;
}
