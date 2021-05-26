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

    _add_slider( "gain",        { ox + cw * 0,      oy + ch * 0,      sw, sh }, " dB", true );
    _add_slider( "bank",        { ox + cw * 0,      oy + ch * 1,      sw, sh }, L" °", true );

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
    g.setFillType( juce::FillType( juce::Colour( 0xff0b2430 ) ) );
    g.fillRect( 3 * cw, 0, 3 * cw, H );

    g.setFont( juce::Font( "Arial", 24.0f, juce::Font::bold ) );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawFittedText( "INPUT MIX", 4, 4, cw - 8, 28, juce::Justification::centredTop, 1 );
    g.setColour( juce::Colour( 0x7f000000 ) );
    g.drawFittedText( "DELAY / ECHO", cw + 4, 4, cw * 2 - 8, 28, juce::Justification::centredTop, 1 );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawFittedText( "OUTPUT MIX", cw * 3 + 4, 4, cw - 8, 28, juce::Justification::centredTop, 1 );

    const juce::Font logo_font = juce::Font( "Arial", 32.0f, juce::Font::bold );
    const juce::Font italic_font = juce::Font( "Arial", 14.0f, juce::Font::italic );
    const juce::Font bold_font = juce::Font( "Arial", 14.0f, juce::Font::bold );

    const juce::StringRef logo_Dr( "Dr" );
    const juce::StringRef logo_dot( "." );
    const juce::StringRef logo_Echo( "Echo" );
    static const int w_Dr = logo_font.getStringWidth( logo_Dr );
    static const int w_dot = logo_font.getStringWidth( logo_dot );
    static const int w_Echo = logo_font.getStringWidth( logo_Echo );
    static const int w_Dr_Echo = w_Dr + w_dot / 2 + w_Echo;

    g.setFont( logo_font );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawSingleLineText( logo_Dr, 4, H - 6 );
    g.setColour( juce::Colour( 0x3fffffff ) );
    g.drawSingleLineText( logo_dot, 4 + w_Dr - w_dot / 2, H - 6 );
    g.setColour( juce::Colour( 0xefffffff ) );
    g.drawSingleLineText( logo_Echo, 4 + w_Dr + w_dot / 2, H - 6 );

    const juce::StringRef author_name( JucePlugin_Manufacturer );
    const juce::StringRef version( JucePlugin_VersionString );
    static const int w_author_name = bold_font.getStringWidth( author_name );

    g.setFont( italic_font );
    g.setColour( juce::Colour( 0x3fffffff ) );
    g.drawFittedText( "A plug-in by", W - w_author_name - 4, H - 12 - 12 - 4, w_author_name, 12, juce::Justification::bottomLeft, 1 );
    g.setFont( bold_font );
    g.setColour( juce::Colour( 0x7fffffff ) );
    g.drawFittedText( author_name, W - w_author_name - 4, H - 12 - 2, w_author_name, 12, juce::Justification::bottomRight, 1 );

    g.setFont( italic_font );
    g.setColour( juce::Colour( 0x3f000000 ) );
    g.drawFittedText( "Version", cw, H - 10 - 10 - 4, cw * 2, 10, juce::Justification::centredBottom, 1 );
    g.setFont( bold_font );
    g.setColour( juce::Colour( 0x7f000000 ) );
    g.drawFittedText( version, cw, H - 10 - 2, cw * 2, 10, juce::Justification::centredBottom, 1 );

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

    label.setLookAndFeel( meta ? &_meta_look_and_feel : &_default_look_and_feel );

    label.setText( text, juce::NotificationType::dontSendNotification );
    label.setJustificationType( juce::Justification::centred );
    
    juce::Font font = label.getFont();
    font.setBold( true );
    label.setFont( font );

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

    slider.setLookAndFeel( meta ? &_meta_look_and_feel : &_default_look_and_feel );

    int text_box_height = slider.getTextBoxHeight() * 2 / 3;

    ci.bounds.setHeight( bounds.getHeight() - text_box_height );

    slider.setSliderStyle( juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag );

    slider.setTextBoxStyle( juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, bounds.getWidth(), slider.getTextBoxHeight() );
    slider.setTextValueSuffix( textValueSuffix );

    //slider.setPopupMenuEnabled( true );

    addAndMakeVisible( slider );

    _add_label( audioProcessor.apvts.getParameter( parameterID )->getLabel(), { bounds.getX(), bounds.getBottom() - text_box_height, bounds.getWidth(), text_box_height }, meta );

    return slider;
}
