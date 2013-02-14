#ifndef __PITCHDIRACLE__
#define __PITCHDIRACLE__

#include "JuceHeader.h"
#include "PitchBase.h"
#include "basicfilters.h"

#if ! _WIN64


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
	float scale;

	JUCE_DECLARE_NON_COPYABLE(PitchDiracLE);

};
#endif
#endif // __PITCHDIRACLE__ 

