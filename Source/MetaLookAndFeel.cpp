/*
  ==============================================================================

    MetaLookAndFeel.cpp
    Created: 25 May 2021 10:57:50am
    Author:  sflei_01

  ==============================================================================
*/

#include "MetaLookAndFeel.h"

MetaLookAndFeel::MetaLookAndFeel()
    : DefaultLookAndFeel()
{
    setColour( juce::Label::backgroundColourId, juce::Colours::transparentBlack );
    setColour( juce::Label::textColourId, juce::Colour( 0xff878787 ) );

    //setColour( juce::Slider::backgroundColourId, juce::Colours::red );
    setColour( juce::Slider::thumbColourId, juce::Colours::white );
    //setColour( juce::Slider::trackColourId, juce::Colours::red );
    setColour( juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd7d7d7 ) );
    setColour( juce::Slider::rotarySliderOutlineColourId, juce::Colour( 0xff383838 ) );
    setColour( juce::Slider::textBoxTextColourId, juce::Colour( 0xffd7d7d7 ) );
    setColour( juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack );
    //setColour( juce::Slider::textBoxHighlightColourId, juce::Colours::red );
    setColour( juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack );
}
