#ifndef __BASICFILTERS__
#define __BASICFILTERS__

#include "JuceHeader.h"

class BasicFilters
{
public:
	enum Type
	{
		kNone,
		kLowPass,
		kHighPass,
		kLowShelf,
		kHighShelf,
		kBell,
		kBandpass,
		kNotch,

		kNumTypes
	};

	BasicFilters()
		: sampleRate(44100), a1(0), a2(0), b0(1), b1(0), b2(0), enabled(false),
			type(kNone), freq(1000), Q(0.7), gain(0)
	{
	}

	void clear()
	{
		x1 = 0;
		x2 = 0;
		y1 = 0;
		y2 = 0;
	}

	void processBlock(float* data, int numSamples)
	{
		if (! enabled || type == kNone)
			return;

		for (int i=0; i<numSamples; ++i)
		{
			const double x0 = data[i];
			double y0 = x0*b0 + x1*b1 + x2*b2 - a1*y1 - a2*y2;

			if (y0 < 1e-10 && y0 > -1e-10)
				y0 = 0;

			y2 = y1; y1 = y0;
			x2 = x1; x1 = x0;

			data[i] = (float) y0;
		}
	}

	void setSampleRate(double newSampleRate)
	{
		jassert(newSampleRate > 10 && newSampleRate < 200e3);

		sampleRate = newSampleRate;

	}

	void setGain(double newGain)
	{
		setFilter(type, freq, Q, newGain);
	}

	void setQ(double newQ)
	{
		setFilter(type, freq, newQ, gain);
	}

	void setFreq(double newFreq)
	{
		setFilter(type, newFreq, Q, gain);
	}


	void setType(Type newType)
	{
		setFilter(newType, freq, Q, gain);
	}

	double getGain()
	{
		return gain;
	}

	double getQ()
	{
		return Q;
	}

	double getFreq()
	{
		return freq;
	}

	Type getType()
	{
		return type;
	}

	void setFilter(Type filterType, double filterFreq, double filterQ, double filterGain)
	{
		jassert(filterFreq > 0 && filterQ > 0 && filterGain > -50 && filterGain < 50);

		enabled = true;

		type = filterType;
		freq = filterFreq;
		Q = filterQ;
		gain = filterGain;

		const double A = pow(10., gain/40.);
		const double w0 = 2*float_Pi*freq/sampleRate;
		const double alpha = sin(w0)/(2*Q);
		double a0;

		switch (filterType)
		{
		case kLowPass:
      a0 =   1 + alpha;
      b0 =  ((1 - cos(w0))/2)/a0;
      b1 =   (1 - cos(w0))/a0;
      b2 =  ((1 - cos(w0))/2)/a0;
      a1 =  (-2*cos(w0))/a0;
      a2 =   (1 - alpha)/a0;
			break;
		case kHighPass:
      a0 =   1 + alpha;
      b0 =  ((1 + cos(w0))/2) / a0;
      b1 = (-(1 + cos(w0))) / a0;
      b2 =  ((1 + cos(w0))/2) / a0;
      a1 =  (-2*cos(w0)) / a0;
      a2 =   (1 - alpha) / a0;
			break;
		case kLowShelf:
      a0 =        (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha;
      b0 =    A*( (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha ) / a0;
      b1 =  2*A*( (A-1) - (A+1)*cos(w0)                   ) / a0;
      b2 =    A*( (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha ) / a0;
      a1 =   -2*( (A-1) + (A+1)*cos(w0)                   ) / a0;
      a2 =      ( (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha ) / a0;
			break;
		case kHighShelf:
      a0 =       (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha;
      b0 =    A*( (A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha ) / a0;
      b1 = -2*A*( (A-1) + (A+1)*cos(w0)                   ) / a0;
      b2 =    A*( (A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha ) / a0;
      a1 =    2*( (A-1) - (A+1)*cos(w0)                   ) / a0;
      a2 =      ( (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha ) / a0;
			break;
		case kBell:
      a0 =   1 + alpha / A;
      b0 =   (1 + alpha*A) / a0;
      b1 =  (-2*cos(w0)) / a0;
      b2 =   (1 - alpha*A) / a0;
      a1 =  (-2*cos(w0)) / a0;
      a2 =   (1 - alpha/A) / a0;
			break;
		case kBandpass:
      a0 =   1 + alpha;
      b0 =   alpha / a0;
      b1 =   0;
      b2 =  -alpha / a0;
      a1 =  -2*cos(w0) / a0;
      a2 =   (1 - alpha) / a0;
			break;
		case kNotch:
      a0 =   1 + alpha;
      b0 =   1 / a0;
      b1 =  -2*cos(w0) / a0;
      b2 =   1 / a0;
      a1 =  -2*cos(w0) / a0;
      a2 =   (1 - alpha) / a0;
			break;
		default:
				break;
		};

		setAntiResoGain();
		clear();
	}

	String getTypeName()
	{
		switch(type)
		{
			case kLowPass:
				return "Lowpass";
			case kHighPass:
				return "Highpass";
			case kLowShelf:
				return "Lowshelf";
			case kHighShelf:
				return "Highshelf";
			case kBell:
				return "Bell";
			case kBandpass:
				return "Bandpass";
			case kNotch:
				return "Notch";
			default:
				break;
		}
		return "";

	}

	void setEnabled(bool enable)
	{
		if (enable && ! enabled)
			clear();

		enabled = enable;
	}


	void setAntiResoGain()
	{
		double antiResoGain = 1.;

		switch (type)
		{
		case BasicFilters::kBell:
			antiResoGain = Decibels::decibelsToGain((float) jmin(0., -gain));
			break;
		case BasicFilters::kLowPass:
		case BasicFilters::kHighPass:
			antiResoGain = Q > 1 ? 1/Q : 1.;
			break;
		case BasicFilters::kLowShelf:
		case BasicFilters::kHighShelf:
			antiResoGain = Decibels::decibelsToGain((float) jmin(0., -gain)) / jmax(1., Q);
			break;
		default:
			antiResoGain = 1.f;
		}

		b0 *= antiResoGain;
		b1 *= antiResoGain;
		b2 *= antiResoGain;

	}

protected:

	double sampleRate;
	double a1, a2, b0, b1, b2;
	double x1, x2, y1, y2;

	bool enabled;

	Type type;
	double freq;
	double Q;
	double gain;

};




#endif // __BASICFILTERS__