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
    const int H = 360;

    const int sw = 80;
    const int sh = 100;

    const int cw = W / 4;
    const int ch = H / 3;

    const int ox = (cw - sw) / 2;
    const int oy = (ch - sh) / 2 + ch / 2;

    _add_slider( "level",       { ox + cw * 0,      oy + ch * 0,      sw, sh }, " %", true );
    _add_slider( "bank",        { ox + cw * 0,      oy + ch * 1,      sw, sh }, " °", true );

    _add_slider( "delay",       { ox + cw * 3/2-20, oy + ch * 0-20,   sw+40, sh+40 }, " ms" );

    _add_slider( "pingpong",    { ox + cw * 1,      oy + ch * 1,      sw, sh }, " %" );
    _add_slider( "feedback",    { ox + cw * 2,      oy + ch * 1,      sw, sh }, " %" );

    _add_slider( "dry",         { ox + cw * 3,      oy + ch * 0,      sw, sh }, " %", true );
    _add_slider( "wet",         { ox + cw * 3,      oy + ch * 1,      sw, sh }, " %", true );

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize( W, H );

    constrainer.setSizeLimits( W, H, W, H );
    this->setConstrainer( &constrainer );
}

DrEchoAudioProcessorEditor::~DrEchoAudioProcessorEditor()
{
    // The attachments have to be deleted before their respective components.
    // And the components have to be deleted or at least detached from the
    // look-and-feel object before deleting it. So let's do this explicitly.
    for ( auto& p : _component_infos )
    {
        ComponentInfo& ci = p.second;
        ci.component_attachment_wrapper_ptr.reset();
        ci.component_ptr.reset();
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

    const int W = getWidth();
    const int H = getHeight();

    const int cw = W / 4;

    g.setFillType( juce::FillType( juce::Colour( 0xff0b2430 ) ) );
    g.fillRect( 0 * cw, 0, 1 * cw, H );
    g.setFillType( juce::FillType( juce::Colour( 0xffefefef ) ) );
    g.fillRect( 1 * cw, 0, 2 * cw, H );
    g.setFillType( juce::FillType( juce::Colour( 0xff0f2f3f ) ) );
    g.fillRect( 3 * cw, 0, 3 * cw, H );

    g.setFont( juce::Font( "Arial", 24.0f, juce::Font::bold ) );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawFittedText( "INPUT MIX", 4, 4, cw - 8, 28, juce::Justification::centredTop, 1 );
    g.setColour( juce::Colour( 0x7f000000 ) );
    g.drawFittedText( "DELAY / ECHO", cw + 4, 4, cw * 2 - 8, 28, juce::Justification::centredTop, 1 );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawFittedText( "OUTPUT MIX", cw * 3 + 4, 4, cw - 8, 28, juce::Justification::centredTop, 1 );

    {
        juce::Font logo_font = juce::Font( "Arial", 32.0f, juce::Font::bold );
        static int Dr = logo_font.getStringWidth( "Dr" );
        static int _ = logo_font.getStringWidth( "." );
        static int Echo = logo_font.getStringWidth( "Echo" );
        static int Dr_Echo = Dr + _ / 2 + Echo;

        g.setFont( logo_font );
        g.setColour( juce::Colour( 0x7fffffff ) );
        g.drawSingleLineText( "Dr", 4, H - 6 );
        g.setColour( juce::Colour( 0x3fffffff ) );
        g.drawSingleLineText( ".", 4 + Dr - _ / 2, H - 6 );
        g.setColour( juce::Colour( 0xefffffff ) );
        g.drawSingleLineText( "Echo", 4 + Dr + _ / 2, H - 6 );
    }

    {
        juce::Font italic_font = juce::Font( "Arial", 14.0f, juce::Font::italic );
        juce::Font bold_font = juce::Font( "Arial", 14.0f, juce::Font::bold );
        static int Stefan_Fleischer = bold_font.getStringWidth( "Stefan Fleischer" );

        g.setFont( italic_font );
        g.setColour( juce::Colour( 0x3fffffff ) );
        g.drawFittedText( "A plug-in by", W - Stefan_Fleischer - 4, H - 12 - 12 - 4, Stefan_Fleischer, 12, juce::Justification::bottomLeft, 1 );
        g.setFont( bold_font );
        g.setColour( juce::Colour( 0x7fffffff ) );
        g.drawFittedText( "Stefan Fleischer", W - Stefan_Fleischer - 4, H - 12 - 2, Stefan_Fleischer, 12, juce::Justification::bottomRight, 1 );

        g.setFont( italic_font );
        g.setColour( juce::Colour( 0x3f000000 ) );
        g.drawFittedText( "Version", cw, H - 10 - 10 - 4, cw * 2, 10, juce::Justification::centredBottom, 1 );
        g.setFont( bold_font );
        g.setColour( juce::Colour( 0x7f000000 ) );
        g.drawFittedText( "1.0.0", cw, H - 10 - 2, cw * 2, 10, juce::Justification::centredBottom, 1 );
    }

    /*for ( auto& p : _component_infos )
    {
        ComponentInfo& ci = p.second;
        juce::Component& component = *ci.component_ptr;
        g.setColour( juce::Colours::red );
        g.drawRect( ci.bounds, 2 );
        for ( auto& p : component.getChildren() )
        {
            g.setColour( juce::Colours::pink );
            g.drawRect( p->getBounds().translated( component.getX(), component.getY() ), 1 );
        }
    }*/
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

    label.setLookAndFeel( &_look_and_feel );

    label.setText( text, juce::NotificationType::dontSendNotification );
    label.setJustificationType( juce::Justification::centred );
    
    juce::Font font = label.getFont();
    font.setBold( true );
    label.setFont( font );

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

    slider.setLookAndFeel( &_look_and_feel );

    int text_box_height = slider.getTextBoxHeight() * 2 / 3;

    ci.bounds.setHeight( bounds.getHeight() - text_box_height );

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

    _add_label( audioProcessor.apvts.getParameter( parameterID )->getLabel(), { bounds.getX(), bounds.getBottom() - text_box_height, bounds.getWidth(), text_box_height }, meta );

    return slider;
}
