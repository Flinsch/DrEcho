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
    setColour( juce::Slider::thumbColourId, juce::Colours::black );
    //setColour( juce::Slider::trackColourId, juce::Colours::red );
    setColour( juce::Slider::rotarySliderFillColourId, juce::Colour( 0xff383838 ) );
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
