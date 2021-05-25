/*
  ==============================================================================

    DefaultLookAndFeel.h
    Created: 21 May 2021 7:25:27pm
    Author:  Stefan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DefaultLookAndFeel : public juce::LookAndFeel_V4
{

public:
    DefaultLookAndFeel();

public:
    virtual juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;

};
