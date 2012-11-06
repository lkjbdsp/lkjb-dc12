#ifndef __PITCHDIRACLE__
#define __PITCHDIRACLE__

#include "JuceHeader.h"
#include "PitchBase.h"

//class DiracWrapper
//{
//public:
//	DiracWrapper()
//		: dirac(0)
//	{
//	}
//
//	~DiracWrapper()
//	{
//		if (dirac != nullptr)
//		{
//			DiracFxDestroy(pitchL);
//			dirac = nullptr;
//		}
//	}
//
//	void init(float sampleRate, int quality)
//	{
//		if (sampleRate != 44100 || sampleRate != 48000)
//		{
//			jassertfalse;
//			sampleRate = 44100;
//		}
//
//		dirac = DiracFxCreate(quality, sampleRate, 1);
//		jassert(dirac != nullptr);
//	}
//
//	void processBlock(float* data, int numSamples, float pitch)
//	{
//		if (dirac != 0)
//			DiracFxProcessFloat(1., pitch, &chL, &chL, numSamples, dirac);
//	}
//
//private:
//
//	void* dirac;
//
//};
//

class PitchDiracLE : public PitchBase
{
public:
	enum Quality
	{
		kPreview = 300,	
		kGood,
		kBetter,
		kBest,

		kNumQualities
	};

	PitchDiracLE(Quality quality_ = kBest);
	~PitchDiracLE();

	void prepareToPlay(double sampleRate, int blockSize);
	void processBlock(float* chL, float* chR, int numSamples);
	void processBlock(float* ch, int numSamples);
	void setPitch(float newPitch);

	int getLatency();
	void clear();
	String getName();

private:

	void destroyDirac();

	const Quality quality;

	void* pitchL;
	void* pitchR;

	float sampleRate;
	float pitch;

	JUCE_DECLARE_NON_COPYABLE(PitchDiracLE);

};

#endif // __PITCHDIRACLE__ 

