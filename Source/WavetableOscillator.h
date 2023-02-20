/*
  ==============================================================================

    WavetableOscillator.h
    Created: 16 Feb 2023 11:23:52am
    Author:  Sonderbo

  ==============================================================================
*/

#include <JuceHeader.h>

class WavetableOscillator
{
public:
    enum Wavetype
    {
        TRIANGLE,
        SINE
    };

    WavetableOscillator(juce::AudioSampleBuffer& wavetableToUse, Wavetype type);

    

    void setFrequency(float frequency, float sampleRate);
    float getNextSample();
    static void createWavetable(Wavetype wavetype, juce::AudioSampleBuffer& wavetableToUse);

    

private:
    const juce::AudioSampleBuffer& wavetable;
    const int tableSize;
    const int tableSize_m1;
    float currentIndex = 0.0f;
    float tableDelta = 0.0f;

    static void buildSineWavetable(int tableSize, float* samples);

    static void buildTriangleWavetable(int tableSize, float* samples);
};