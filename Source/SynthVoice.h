/*
  ==============================================================================

	SynthVoice.h
	Created: 1 Dec 2022 11:05:25am
	Author:  Sonderbo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "WavetableOscillator.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
	bool canPlaySound(juce::SynthesiserSound* sound) override;
	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void updateAdsr(const float attack, const float decay, const float sustain, const float release);
	void updateAmplitude(const float A);
private:

	float A;

	juce::ADSR adsr;
	juce::ADSR::Parameters adsrParams;
	const unsigned int tableSize = 1 << 7;
	juce::AudioBuffer<float> triangleWavetable{ 1, tableSize };
	WavetableOscillator osc{triangleWavetable, WavetableOscillator::TRIANGLE};
	juce::dsp::Gain<float> gain;
	bool isPrepared = false;

	float sampleRate;
};