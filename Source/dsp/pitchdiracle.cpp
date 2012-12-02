#include "pitchdiracle.h"
#include "../DiracLE/Dirac.h"


PitchDiracLE::PitchDiracLE(Quality quality_)
: quality(quality_),
	pitchL(0),
	pitchR(0),
	sampleRate(44100),
	pitch(1.f),
	scale(1.f/32.f)
{
	jassert(quality == kDiracQualityPreview || quality == kDiracQualityGood || quality == kDiracQualityBetter || quality == kDiracQualityBest);
}

PitchDiracLE::~PitchDiracLE()
{
	destroyDirac();
}

void PitchDiracLE::prepareToPlay(double samplerate, int /*blockSize*/)
{
	sampleRate = (float) samplerate;

	jassert(sampleRate == 44100 || sampleRate == 48000);

	if (sampleRate != 44100 && sampleRate != 48000)
		sampleRate = 44100;

	destroyDirac();
	
	pitchL = DiracFxCreate(quality, sampleRate, 1);
	pitchR = DiracFxCreate(quality, sampleRate, 1);

	jassert(pitchL != 0 && pitchR != 0);
}

void PitchDiracLE::processBlock(float* chL, float* chR, int numSamples)
{
	for (int i=0; i<numSamples; ++i)
	{
		chL[i] *= scale;
		chR[i] *= scale;
	}

	DiracFxProcessFloat(1., pitch, &chL, &chL, numSamples, pitchL);
	DiracFxProcessFloat(1., pitch, &chR, &chR, numSamples, pitchR);

	const float invScale = scale > 0.f ? 1.f / scale : 0.f;

	for (int i=0; i<numSamples; ++i)
	{
		chL[i] *= invScale;
		chR[i] *= invScale;
		chL[i] = jlimit(-1.f, 1.f, chL[i]);
		chR[i] = jlimit(-1.f, 1.f, chR[i]);
	}
}

void PitchDiracLE::processBlock(float* ch, int numSamples)
{
	for (int i=0; i<numSamples; ++i)
		ch[i] *= scale;
	
	DiracFxProcessFloat(1., pitch, &ch, &ch, numSamples, pitchL);

	const float invScale = scale > 0.f ? 1.f / scale : 0.f;
	
	for (int i=0; i<numSamples; ++i)
	{
		ch[i] *= invScale;
		ch[i] = jlimit(-1.f, 1.f, ch[i]);
	}
}

void PitchDiracLE::setPitch(float newPitch)
{
	pitch = newPitch;
}

int PitchDiracLE::getLatency()
{
	return DiracFxLatencyFrames( sampleRate);
}

void PitchDiracLE::clear()
{
	DiracFxReset(true, pitchL);
	DiracFxReset(true, pitchR);
}

String PitchDiracLE::getName() 
{ 
	const String qual(quality == kPreview ? "(Preview)" 
		              : quality == kGood    ? "(Good)"
		              : quality == kBetter  ? "(Better)"
		              :                       "(Best)"
									);

	return "Dirac LE " + qual; 
}

void PitchDiracLE::destroyDirac()
{
	if (pitchL != nullptr)
	{
		DiracFxDestroy(pitchL);
		pitchL = 0;
	}


	if (pitchR != 0)
	{
		DiracFxDestroy(pitchR);
		pitchR = 0;
	}
}

