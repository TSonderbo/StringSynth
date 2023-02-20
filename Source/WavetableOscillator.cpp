/*
  ==============================================================================

	WavetableOscillator.cpp
	Created: 16 Feb 2023 11:23:52am
	Author:  Sonderbo

  ==============================================================================
*/

#include "WavetableOscillator.h"

WavetableOscillator::WavetableOscillator(juce::AudioSampleBuffer& wavetableToUse, Wavetype type)
	: wavetable(wavetableToUse),
	tableSize(wavetable.getNumSamples() - 1),
	tableSize_m1(tableSize - 1)
{
	jassert(wavetableToUse.getNumChannels() == 1);
	createWavetable(type, wavetableToUse);
}


void WavetableOscillator::setFrequency(float frequency, float sampleRate)
{
	auto tableSizeOverSampleRate = (float)tableSize / sampleRate;
	tableDelta = frequency * tableSizeOverSampleRate;
}

float WavetableOscillator::getNextSample()
{
	unsigned int index0 = (unsigned int)currentIndex;
	unsigned int index1 = index0 == (tableSize_m1) ? (unsigned int)0 : index0 + 1;

	float frac = currentIndex - (float)index0;

	auto* table = wavetable.getReadPointer(0);
	float value0 = table[index0];
	float value1 = table[index1];

	float currentSample = value0 + frac * (value1 - value0);

	if ((currentIndex += tableDelta) > (float)tableSize)
		currentIndex -= (float)tableSize;

	return currentSample;
}

void WavetableOscillator::createWavetable(Wavetype wavetype, juce::AudioSampleBuffer& wavetableToUse)
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
		float sample = 0.0f;

		if (phase < juce::MathConstants<float>::pi)
			sample = -1 + (2 * 1 / juce::MathConstants<float>::pi) * phase;
		else
			sample = 3 - (2 * 1 / juce::MathConstants<float>::pi) * phase;

		phase = phase + ((2 * juce::MathConstants<float>::pi) / tableSize);
		samples[i] = (float)sample;
	}

	samples[tableSize] = samples[0];
}
