/*
  ==============================================================================

	SynthVoice.cpp
	Created: 1 Dec 2022 11:05:25am
	Author:  Sonderbo

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
	//Return true if sound is valid
	return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
	adsr.noteOn();
	
	double freq = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

	osc.setFrequency(freq, sampleRate);
	delay = (size_t)(sampleRate/freq);
	inputSamples = (int)delay;

	f1_p = -1 * (1 / (freq * log(abs(p * cos(juce::MathConstants<float>::pi * freq * Ts)))));
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
	adsr.noteOff();
	
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
	
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
	juce::dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.sampleRate = sampleRate;
	spec.numChannels = numChannels;

	gain.prepare(spec);
	gain.setGainLinear(1.0f);
	adsr.setSampleRate(sampleRate);

	//osc.setFrequency(100, sampleRate);

	this->sampleRate = sampleRate;
	delayLine.resize(3952); // Max B7

	auto filterCoefs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, float(1e3));
	lowpass.prepare(spec);
	lowpass.coefficients = filterCoefs;

	adsrParams.attack = 0.00001f;
	adsrParams.decay = 0.3f;
	adsrParams.sustain = 1.0f;
	adsrParams.release = 0.2f;

	adsr.setParameters(adsrParams);

	Ts = 1.0f / sampleRate;

	isPrepared = true;
}

void SynthVoice::updateAdsr(const float attack, const float decay, const float sustain, const float release)
{
	adsrParams.attack = attack;
	adsrParams.decay = decay;
	adsrParams.sustain = sustain;
	adsrParams.release = release;

	adsr.setParameters(adsrParams);
}

void SynthVoice::updateAmplitude(const float A)
{
	this->A = A;
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
	if (!isVoiceActive()) return;

	while (--numSamples >= 0)
	{
		float sample = 0.0f;
		if (inputSamples > 0)
		{
			//sample = osc.getNextSample();
			sample = (random.nextFloat() * 2 - 1)* osc.getNextSample() ;
			inputSamples--;
		}
		float S1 = delayLine.get(delay) * (1.0f - S);
		float S2 = delayLine.get(delay + 1) * S;
		sample += (S1 + S2);

		//sample = lowpass.processSample(sample);

		delayLine.push(sample);

		sample *= adsr.getNextSample();

		sample = gain.processSample(sample);

		for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
		{
			outputBuffer.addSample(channel, startSample, sample);
		}
		startSample++;
		
	}

	if (!adsr.isActive())
	{
		clearCurrentNote();
		delayLine.clear();
	}
		
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{

}