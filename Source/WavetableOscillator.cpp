/*
  ==============================================================================

	WavetableOscillator.cpp
	Created: 16 Feb 2023 11:23:52am
	Author:  Sonderbo

  ==============================================================================
*/

#include "WavetableOscillator.h"

WavetableOscillator::WavetableOscillator(const juce::AudioSampleBuffer& wavetableToUse)
	: wavetable(wavetableToUse),
	tableSize(wavetable.getNumSamples() - 1),
	tableSize_m1(tableSize - 1)
{
	jassert(wavetable.getNumChannels() == 1);
}


void WavetableOscillator::setFrequency(float frequency, float sampleRate)
{
	auto tableSizeOverSampleRate = (float)tableSize / sampleRate;
	tableDelta = frequency * tableSizeOverSampleRate;
}

float WavetableOscillator::getNextSample()
{
	auto index0 = (unsigned int)currentIndex;
	auto index1 = index0 == (tableSize_m1) ? (unsigned int)0 : index0 + 1;

	auto frac = currentIndex - (float)index0;

	auto* table = wavetable.getReadPointer(0);
	auto value0 = table[index0];
	auto value1 = table[index1];

	auto currentSample = value0 + frac * (value1 - value0);

	if ((currentIndex += tableDelta) > (float)tableSize)
		currentIndex -= (float)tableSize;

	return currentSample;
}

void WavetableOscillator::createWavetable(Wavetype wavetype, juce::AudioSampleBuffer wavetableToUse)
{
	auto* samples = wavetableToUse.getWritePointer(0);
	auto tableSize = wavetableToUse.getNumSamples();

	switch (wavetype)
	{
	case WavetableOscillator::TRIANGLE:
	{
		buildTriangleWavetable(tableSize, samples);
		break;
	}
	case WavetableOscillator::SINE:
	{
		buildSineWavetable(tableSize, samples);
		break;
	}
	default:
		break;
	}
}

void WavetableOscillator::buildSineWavetable(int tableSize, float* samples)
{
	auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
	auto currentAngle = 0.0;

	for (unsigned int i = 0; i < tableSize; ++i)
	{
		auto sample = std::sin(currentAngle);
		samples[i] = (float)sample;
		currentAngle += angleDelta;
	}

	samples[tableSize] = samples[0];
}

void WavetableOscillator::buildTriangleWavetable(int tableSize, float* samples)
{
	float phase = 0.0f;
	for (unsigned int i = 0; i < tableSize; ++i)
	{
		auto sample = 0;

		if (phase < juce::MathConstants<float>::pi)
			sample = -1 + (2 * 1 / juce::MathConstants<float>::pi) * phase;
		else
			sample = 3 - (2 * 1 / juce::MathConstants<float>::pi) * phase;

		phase = phase + ((2 * juce::MathConstants<float>::pi) / tableSize);
		samples[i] = (float)sample;

		if (phase > juce::MathConstants<float>::twoPi)
		{
			DBG("Triangle Wavetable Loop iterated for too long");
		}
	}

	samples[tableSize] = samples[0];
}
