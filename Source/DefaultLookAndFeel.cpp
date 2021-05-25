/*
  ==============================================================================

    DefaultLookAndFeel.cpp
    Created: 21 May 2021 7:25:27pm
    Author:  Stefan

  ==============================================================================
*/

#include "DefaultLookAndFeel.h"

DefaultLookAndFeel::DefaultLookAndFeel()
    : juce::LookAndFeel_V4()
{
    setColour( juce::Label::backgroundColourId, juce::Colours::transparentBlack );
    setColour( juce::Label::textColourId, juce::Colour( 0xff888888 ) );

    //setColour( juce::Slider::backgroundColourId, juce::Colours::red );
    setColour( juce::Slider::thumbColourId, juce::Colour( 0xff383838 ) );
    setColour( juce::Slider::trackColourId, juce::Colours::white );
    setColour( juce::Slider::rotarySliderFillColourId, juce::Colours::black );
    setColour( juce::Slider::rotarySliderOutlineColourId, juce::Colour( 0xffd7d7d7 ) );
    setColour( juce::Slider::textBoxTextColourId, juce::Colour( 0xff383838 ) );
    setColour( juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack );
    //setColour( juce::Slider::textBoxHighlightColourId, juce::Colours::red );
    setColour( juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack );
}

juce::Slider::SliderLayout DefaultLookAndFeel::getSliderLayout(juce::Slider& slider)
{
    juce::Slider::SliderLayout layout = juce::LookAndFeel_V4::getSliderLayout( slider );
    layout.sliderBounds.setHeight( layout.sliderBounds.getHeight() + slider.getTextBoxHeight() * 2 / 3 );
    return layout;
}

void DefaultLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto outline = slider.findColour( juce::Slider::rotarySliderOutlineColourId );
    auto fill    = slider.findColour( juce::Slider::rotarySliderFillColourId );
    auto thumb   = slider.findColour( juce::Slider::thumbColourId );
    auto track   = slider.findColour( juce::Slider::trackColourId );

    auto rotaryTotalAngleDiff = rotaryEndAngle - rotaryStartAngle;
    auto toAngle = rotaryStartAngle + sliderPos * rotaryTotalAngleDiff;

    auto bounds = juce::Rectangle<int>( x, y, width, height ).toFloat().reduced( 5 );

    auto radius = juce::jmin( bounds.getWidth(), bounds.getHeight() ) * 0.5f;
    auto ledWidth = juce::jlimit( 4.0f, 8.0f, radius * 0.25f );
    auto arcRadius = radius - ledWidth * 0.5f;
    auto knobRadius = arcRadius - ledWidth * 1.5f;
    auto knobWidth = knobRadius * 2.0f;

    juce::Path knob;
    knob.addTriangle( 0.0f, -knobRadius - ledWidth * 0.5f, +knobRadius, 0.0f, -knobRadius, 0.0f );
    knob.applyTransform( juce::AffineTransform::rotation( toAngle - (rotaryStartAngle + rotaryEndAngle) * 0.5f ) );
    knob.applyTransform( juce::AffineTransform::translation( bounds.getCentre() ) );
    knob.addEllipse( juce::Rectangle<float>( knobWidth, knobWidth ).withCentre( bounds.getCentre() ) );

    g.setColour( thumb );
    g.fillPath( knob );

    //bool twoSided = std::signbit( slider.getMinimum() ) != std::signbit( slider.getMaximum() );
    bool twoSided = -slider.getMinimum() == +slider.getMaximum();

    int totalCount;
    if ( twoSided )
        totalCount = static_cast<int>( ::fabsf( rotaryTotalAngleDiff ) * arcRadius / (ledWidth * 1.2f) ) / 2 * 2; // Force an even number.
    else
        totalCount = (static_cast<int>( ::fabsf( rotaryTotalAngleDiff ) * arcRadius / (ledWidth * 1.2f) ) - 1) / 2 * 2 + 1; // Force an odd number.
    float K = static_cast<float>( totalCount );
    float _1_K = 1.0f / K;

    for ( int i = 0; i < totalCount; ++i )
    {
        float f = static_cast<float>( i );
        float minPos = f * _1_K;
        float maxPos = minPos + _1_K;
        float pos = (minPos + maxPos) * 0.5f;
        float angle = rotaryStartAngle + pos * rotaryTotalAngleDiff;
        
        float alpha = 0.0f;
        if ( slider.isEnabled() )
        {
            float mn = minPos;
            float mx = maxPos;
            jassert( !twoSided || totalCount % 2 == 0 );
            if ( twoSided && i < totalCount / 2 )
                std::swap( mn, mx );
            alpha = juce::jmap( sliderPos, mn, mx, 0.0f, 1.0f );
        }

        juce::Point<float> point( bounds.getCentreX() + arcRadius * std::cos( angle - juce::MathConstants<float>::halfPi ), bounds.getCentreY() + arcRadius * std::sin( angle - juce::MathConstants<float>::halfPi ) );

        g.setColour( slider.isEnabled() ? outline.interpolatedWith( fill, juce::jlimit( 0.0f, 1.0f, alpha ) ) : outline );
        g.fillEllipse( juce::Rectangle<float>( ledWidth, ledWidth ).withCentre( point ) );
    }

    juce::Point<float> thumbPoint( bounds.getCentreX() + (knobRadius - ledWidth) * std::cos( toAngle - juce::MathConstants<float>::halfPi ), bounds.getCentreY() + (knobRadius - ledWidth) * std::sin( toAngle - juce::MathConstants<float>::halfPi ) );

    g.setColour( track );
    g.fillEllipse( juce::Rectangle<float>( ledWidth, ledWidth ).withCentre( thumbPoint ) );
}
