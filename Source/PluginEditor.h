/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ScopeComponent.h"
#include "AudioBufferQueue.h"

//==============================================================================
/**
*/
class StringSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    StringSynthAudioProcessorEditor (StringSynthAudioProcessor&);
    ~StringSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StringSynthAudioProcessor& audioProcessor;

    ScopeComponent oscilloscope;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringSynthAudioProcessorEditor)
};
