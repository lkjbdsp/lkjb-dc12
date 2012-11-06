#include "delaytabdsp.h"



DelayTabDsp::DelayTabDsp(const String& id)
: Parameters(id),
	volume(0),
	enabled(false),
	mode(kMono),
	sync(0),
	dataSize(0)
{
	addParameter(kPitch, "Pitch", -12, 12, 0, 0);
	addParameter(kSync, "Sync", 0, 7, 0, 0);
	addParameter(kPitchType, "PitchType", 0, (int) delay.getNumPitches() + 1, 0, 0);
	addParameter(kDelay, "Delay", 0, MAXDELAYSECONDS, 0, MAXDELAYSECONDS/2);
	addParameter(kPrePitch, "PrePitch", 0, 1, 0, 0);
	addParameter(kFeedback, "Feedback", 0, 100, 0, 0);

	addParameter(kFilterType, "EQ-Type", 0, 7, 0, 0);
	addParameter(kFilterFreq, "EQ-Freq", 20, 20e3, 1000, 1000);
	addParameter(kFilterQ, "EQ-Q", 0.3, 10, 3, 0.707);
	addParameter(kFilterGain, "EQ-Gain", -FILTERGAIN, FILTERGAIN, 0, 0);

	addParameter(kMode, "Mode", 0, (double) kNumModes - 1, 0, 0);

	addParameter(kVolume, "Volume", -60, 0, 0, 0);

	addParameter(kEnabled, "Enabled", 0, 1, 0, 0);

}

void DelayTabDsp::setParam(int index, double val)
{
	jassert(getParamRange(index).clipValue(val) == val);

	switch(index)
	{
	case kPitch:
		delay.setPitchSemitones(val);
		break;
	case kSync:
		sync = val;
		break;
	case kDelay:
		delay.setDelay(val, delay.isPrePitch());
		break;
	case kPitchType:
		delay.setCurrentPitch(int(val - 1));
		break;
	case kPrePitch:
		delay.setDelay(delay.getDelay(), val > 0.5);
		break;
	case kFeedback:
		delay.setFeedback((float) val/100);
		break;

	case kFilterType:
		delay.setType((BasicFilters::Type) (int) (val+0.5));
		break;
	case kFilterFreq:
		delay.setFreq(val);
		break;
	case kFilterQ: 
		delay.setQ(val);
		break;
	case kFilterGain:
		delay.setGain(val);
		break;

	case kMode:
		mode = (Mode) jlimit(0, kNumModes - 1, int(val +0.5));
		delay.setPingPong(mode == kPingpong);
		break;

	case kVolume:
		volume = val;
		volumeLin = pow(10.f, (float) volume/20.f);
		break;

	case kEnabled:
		enabled = val > 0.5;

		if (! enabled && dataSize > 0)
			clearData();
		break;
	default:
		jassertfalse;
	}


}

double DelayTabDsp::getParam(int index)
{
	double tmp = 0;

	switch(index)
	{
	case kPitch:
		tmp = delay.getPitchSemitones();
		break;
	case kSync:
		tmp = sync;
		break;
	case kDelay:
		tmp = delay.getDelay();
		break;
	case kPitchType:
		tmp = delay.getCurrentPitch() + 1;
		break;
	case kPrePitch:
		tmp = delay.isPrePitch() ? 1 : 0;
		break;
	case kFeedback:
		tmp = 100 * delay.getFeedback();
		break;

	case kFilterType:
		tmp = (double) (int) delay.getType();
		break;
	case kFilterFreq:
		tmp = delay.getFreq();
		break;
	case kFilterQ: 
		tmp = delay.getQ();
		break;
	case kFilterGain:
		tmp = delay.getGain();
		break;

	case kMode:
		return (double) (int) mode;
		break;

	case kVolume:
		tmp = volume;
		break;
	case kEnabled:
		tmp = enabled ? 1 : 0;
		break;
	default:
		jassertfalse;
	}

	return tmp;

}


void DelayTabDsp::prepareToPlay(double sampleRate, int numSamples)
{
	delay.prepareToPlay(sampleRate, numSamples);
	checkDataSize(numSamples);

}

void DelayTabDsp::checkDataSize(int numSamples)
{
	if (dataSize < numSamples)
	{
		dataL.realloc(numSamples);
		dataR.realloc(numSamples);
		dataSize = numSamples;
		clearData();
	}
}

void DelayTabDsp::clearData()
{
	jassert(dataSize > 0);

	for (int i=0; i<dataSize; ++i)
	{
		dataL[i] = 0;
		dataR[i] = 0;
	}
}

void DelayTabDsp::processBlock(const float* inL, const float* inR, int numSamples)
{

	checkDataSize(numSamples);

	if (! enabled)
		return;

	if (mode != kMono)
		processStereo(inL, inR, numSamples);
	else
		processMono(inL, inR, numSamples);
}

void DelayTabDsp::processMono(const float* inL, const float* inR, int numSamples)
{
	for (int i=0; i<numSamples; ++i)
		dataL[i] = 0.5f * (inL[i] + inR[i]);

	delay.processBlock(dataL, numSamples);

	for (int i=0; i<numSamples; ++i)
	{
		dataL[i] *= volumeLin;
		dataR[i] = dataL[i];
	}
}

void DelayTabDsp::processStereo(const float* inL, const float* inR, int numSamples)
{
	for (int i=0; i<numSamples; ++i)
	{
		dataL[i] = inL[i];
		dataR[i] = inR[i];
	}

	delay.processBlock(dataL, dataR, numSamples);

	for (int i=0; i<numSamples; ++i)
	{
		dataL[i] *= volumeLin;
		dataR[i] *= volumeLin;
	}

}

