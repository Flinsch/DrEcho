/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "MyLogger.h"

//==============================================================================
DrEchoAudioProcessor::DrEchoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    : apvts(*this, nullptr, "PARAMETERS", {
        std::make_unique<juce::AudioParameterInt>(      "dry",      "Dry",      0,      100,    100,    "Dry" ),
        std::make_unique<juce::AudioParameterFloat>(    "delay",    "Delay",    juce::NormalisableRange<float>{ 0.0f, 1.0f, 0.01f },  0.23077f,   "Delay" ),
        std::make_unique<juce::AudioParameterInt>(      "feedback", "Feedback", 0,      100,    0,      "Feedback" ),
        std::make_unique<juce::AudioParameterInt>(      "wet",      "Wet",      0,      100,    50,     "Wet" ),
    })
{
}

DrEchoAudioProcessor::~DrEchoAudioProcessor()
{
}

//==============================================================================
const juce::String DrEchoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DrEchoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DrEchoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DrEchoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DrEchoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DrEchoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DrEchoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DrEchoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DrEchoAudioProcessor::getProgramName (int index)
{
    return {};
}

void DrEchoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DrEchoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    _sample_rate = static_cast<float>( sampleRate );
    _samples_per_block = samplesPerBlock;

    const double MAX_SECONDS = 1.0;

    _buffer_index = 0;
    _buffer_size = static_cast<size_t>( ::ceil( _sample_rate * MAX_SECONDS ) );
    for ( int i = 0; i < 2; ++i )
        _sample_buffers[i].resize( _buffer_size );
}

void DrEchoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DrEchoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DrEchoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    jassert( totalNumInputChannels == totalNumOutputChannels );

    const float dry = apvts.getRawParameterValue("dry")->load() * 0.01f;
    const float delay = apvts.getRawParameterValue("delay")->load();
    const float feedback = apvts.getRawParameterValue("feedback")->load() * 0.01f;
    const float wet = apvts.getRawParameterValue("wet")->load() * 0.01f;

    const size_t num_delayed_samples = static_cast<size_t>( _sample_rate * delay );

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for ( int block_index = 0; block_index < buffer.getNumSamples(); ++block_index )
    {
        for ( int channel = 0; channel < totalNumInputChannels; ++channel )
        {
            const float* in_ptr = buffer.getReadPointer( channel ) + block_index;
            float* out_ptr = buffer.getWritePointer( channel ) + block_index;

            const float dry_sample = *in_ptr;

            const auto current_index = _buffer_index;
            const auto delayed_index = (_buffer_index + _buffer_size - num_delayed_samples) % _buffer_size;

            const float delayed_sample = _sample_buffers[ channel ][ delayed_index ];

            float wet_sample = delayed_sample;

            _sample_buffers[ channel ][ current_index ] = dry_sample + feedback * wet_sample;

            *out_ptr = dry * dry_sample + wet * wet_sample;
        }

        _buffer_index = (_buffer_index + 1) % _buffer_size;
    }
}

//==============================================================================
bool DrEchoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DrEchoAudioProcessor::createEditor()
{
    return new DrEchoAudioProcessorEditor (*this);
}

//==============================================================================
void DrEchoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DrEchoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DrEchoAudioProcessor();
}
