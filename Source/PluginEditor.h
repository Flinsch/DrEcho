/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "ComponentAttachmentWrapper.h"
#include "MyLookAndFeel.h"

//==============================================================================
/**
*/
class DrEchoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DrEchoAudioProcessorEditor (DrEchoAudioProcessor&);
    ~DrEchoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DrEchoAudioProcessor& audioProcessor;

private:
    struct ComponentInfo
    {
        std::unique_ptr<juce::Component> component_ptr;
        juce::Rectangle<int> bounds;
        std::unique_ptr<AbstractComponentAttachmentWrapper> component_attachment_wrapper_ptr;
    };

    std::map<juce::String, ComponentInfo> _component_infos;

    juce::ComponentBoundsConstrainer constrainer;

    MyLookAndFeel _look_and_feel;

private:
    juce::Label& _add_label(const juce::String& text, const juce::Rectangle<int>& bounds, bool meta = false);
    juce::Slider& _add_slider(const juce::String& parameterID, const juce::Rectangle<int>& bounds, const juce::String& textValueSuffix, bool meta = false);

    template <class TComponentAttachment, class TComponent>
    AbstractComponentAttachmentWrapper* _create_component_attachment_wrapper(const juce::String& parameterID, TComponent& component)
    {
        return new ComponentAttachmentWrapper<TComponentAttachment>( std::make_unique<TComponentAttachment>( audioProcessor.apvts, parameterID, component ) );
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrEchoAudioProcessorEditor)
};
