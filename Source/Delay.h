/*
  ==============================================================================

    Delay.h
    Created: 20 Feb 2023 10:38:06am
    Author:  Sonderbo

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "DelayLine.h"

template <typename Type, size_t maxNumChannels = 2>
class Delay
{
public:
    //==============================================================================
    Delay()
    {
        setMaxDelayTime(2.0f);
        setDelayTime(0, 0.7f);
        setDelayTime(1, 0.5f);
        setWetLevel(0.8f);
        setFeedback(0.5f);
    }

    //==============================================================================
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        jassert(spec.numChannels <= maxNumChannels);
        sampleRate = (Type)spec.sampleRate;
        updateDelayLineSize();
        updateDelayTime();

        filterCoefs = juce::dsp::IIR::Coefficients<Type>::makeFirstOrderLowPass (sampleRate, Type (1e3));
        //filterCoefs = juce::dsp::IIR::Coefficients<Type>::makeFirstOrderHighPass(sampleRate, Type(1e3));

        for (auto& f : filters)
        {
            f.prepare(spec);
            f.coefficients = filterCoefs;
        }
    }

    //==============================================================================
    void reset() noexcept
    {
        for (auto& f : filters)
            f.reset();      // [5]

        for (auto& dline : delayLines)
            dline.clear();  // [6]
    }

    //==============================================================================
    size_t getNumChannels() const noexcept
    {
        return delayLines.size();
    }

    //==============================================================================
    void setMaxDelayTime(Type newValue)
    {
        jassert(newValue > Type(0));
        maxDelayTime = newValue;
        updateDelayLineSize(); // [1]
    }

    //==============================================================================
    void setFeedback(Type newValue) noexcept
    {
        jassert(newValue >= Type(0) && newValue <= Type(1));
        feedback = newValue;
    }

    //==============================================================================
    void setWetLevel(Type newValue) noexcept
    {
        jassert(newValue >= Type(0) && newValue <= Type(1));
        wetLevel = newValue;
    }

    //==============================================================================
    void setDelayTime(size_t channel, Type newValue)
    {
        if (channel >= getNumChannels())
        {
            jassertfalse;
            return;
        }

        jassert(newValue >= Type(0));
        delayTimes[channel] = newValue;

        updateDelayTime();  // [3]
    }

    //==============================================================================
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        auto numSamples = outputBlock.getNumSamples();
        auto numChannels = outputBlock.getNumChannels();

        jassert(inputBlock.getNumSamples() == numSamples);
        jassert(inputBlock.getNumChannels() == numChannels);

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* input = inputBlock.getChannelPointer(ch);
            auto* output = outputBlock.getChannelPointer(ch);
            auto& dline = delayLines[ch];
            auto delayTime = delayTimesSample[ch];
            auto& filter = filters[ch];

            for (size_t i = 0; i < numSamples; ++i)
            {
                //auto delayedSample = dline.get (delayTime);
                auto delayedSample = filter.processSample(dline.get(delayTime));
                auto inputSample = input[i];
                auto dlineInputSample = std::tanh(inputSample + feedback * delayedSample);
                dline.push(dlineInputSample);
                auto outputSample = inputSample + wetLevel * delayedSample;
                output[i] = outputSample;
            }
        }
    }

private:
    //==============================================================================
    std::array<DelayLine<Type>, maxNumChannels> delayLines;
    std::array<size_t, maxNumChannels> delayTimesSample;
    std::array<Type, maxNumChannels> delayTimes;
    Type feedback{ Type(0) };
    Type wetLevel{ Type(0) };

    std::array<juce::dsp::IIR::Filter<Type>, maxNumChannels> filters;
    typename juce::dsp::IIR::Coefficients<Type>::Ptr filterCoefs;

    Type sampleRate{ Type(44.1e3) };
    Type maxDelayTime{ Type(2) };

    //==============================================================================
    void updateDelayLineSize()
    {
        auto delayLineSizeSamples = (size_t)std::ceil(maxDelayTime * sampleRate);

        for (auto& dline : delayLines)
            dline.resize(delayLineSizeSamples);    // [2]
    }

    //==============================================================================
    void updateDelayTime() noexcept
    {
        for (size_t ch = 0; ch < maxNumChannels; ++ch)
            delayTimesSample[ch] = (size_t)juce::roundToInt(delayTimes[ch] * sampleRate);
    }
};