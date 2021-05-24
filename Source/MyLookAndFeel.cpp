/*
  ==============================================================================

    MyLookAndFeel.cpp
    Created: 21 May 2021 7:25:27pm
    Author:  Stefan

  ==============================================================================
*/

#include "MyLookAndFeel.h"

juce::Slider::SliderLayout MyLookAndFeel::getSliderLayout(juce::Slider& slider)
{
    juce::Slider::SliderLayout layout = juce::LookAndFeel_V4::getSliderLayout( slider );
    layout.sliderBounds.setHeight( layout.sliderBounds.getHeight() + slider.getTextBoxHeight() * 2 / 3 );
    return layout;
}
