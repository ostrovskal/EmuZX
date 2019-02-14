#pragma once

#define BLIP_MED_QUALITY				8
#define BLIP_GOOD_QUALITY				12
#define BLIP_HIGH_QUALITY				16

#define BLIP_BUFFER_DEF_MSEC_LENGTH		( 1000 / 4 )
#define BLIP_BUFFER_DEF_STEREO			0
#define BLIP_BUFFER_DEF_ENTIRE_BUFF		1

#define BLIP_EQ_DEF_CUTOFF				0
#define BLIP_SAMPLE_BITS				30
#define BLIP_UNSCALED					65535
#define BLIP_MAX_LENGTH					0

#define BLIP_SYNTH_QUALITY				BLIP_GOOD_QUALITY
#define BLIP_SYNTH_RANGE				BLIP_UNSCALED
#define BLIP_SYNTH_WIDTH				BLIP_SYNTH_QUALITY

#define BLIP_BUFFER_ACCURACY			16
#define BLIP_PHASE_BITS					6
#define BLIP_WIDEST_IMPULSE_			16
#define BLIP_RES						(1 << BLIP_PHASE_BITS)

#define BUFFER_EXTRA					( BLIP_WIDEST_IMPULSE_ + 2 )

class zxBlipEq {
public:
	double treble;
	long rolloff_freq;
	long sample_rate;
	long cutoff_freq;
};

class zxBlipBuffer {
	friend class zxBlipSynth;
public:
	zxBlipBuffer() : factor_(LONG_MAX), offset_(0), buffer_(nullptr), buffer_size_(0), sample_rate_(0), reader_accum(0), 
					 bass_shift(0), clock_rate_(0), bass_freq_(16), length_(0) {
	}
	
	virtual ~zxBlipBuffer() {
		SAFE_DELETE(buffer_);
	}
	
	const char* setSampleRate(long new_rate, int msec) {
		long new_size = (ULONG_MAX >> BLIP_BUFFER_ACCURACY) - BUFFER_EXTRA - 64;
		if(msec != BLIP_MAX_LENGTH) {
			long s = (new_rate * (msec + 1) + 999) / 1000;
			if(s < new_size) new_size = s;
		}
		if(buffer_size_ != new_size) {
			new_size += BUFFER_EXTRA;
			void* pp = new long[new_size];
			memcpy(pp, buffer_, (new_size > buffer_size_ ? buffer_size_ : new_size));
			buffer_ = (long*)pp;
		}
		buffer_size_ = new_size;
		sample_rate_ = new_rate;
		length_ = new_size * 1000 / new_rate - 1;
		if(clock_rate_) setClockRate(clock_rate_);
		setBassFreq(bass_freq_);
		clear(BLIP_BUFFER_DEF_ENTIRE_BUFF);
		return 0;
	}
	
	void setClockRate(long rate) {
		factor_ = clockRateFactor(clock_rate_ = rate);
	}
	
	void endFrame(long t) {
		offset_ += t * factor_;
	}

	long readSamples(short* dest, long max_samples, int stereo);
	
	void setBassFreq(int frequency) {
		int shift = 31;
		long f;
		bass_freq_ = frequency;
		if(frequency > 0) {
			shift = 13;
			f = (frequency << 16) / sample_rate_;
			while((f >>= 1) && --shift) {};
		}
		bass_shift = shift;
	}
	
	void clear(int entire_buffer) {
		offset_ = 0;
		reader_accum = 0;
		if(buffer_) {
			long count = (entire_buffer ? buffer_size_ : samplesAvail());
			memset(buffer_, 0, (count + BUFFER_EXTRA) * sizeof(long));
		}
	}
	
	long samplesAvail() {
		return (long)(offset_ >> BLIP_BUFFER_ACCURACY);
	}
	
	void removeSamples(long count) {
		long remain;
		if(count) {
			removeSilence(count);
			remain = samplesAvail() + BUFFER_EXTRA;
			memmove(buffer_, buffer_ + count, remain * sizeof(long));
			memset(buffer_ + remain, 0, count * sizeof(long));
		}
	}
	
	void removeSilence(long count) {
		offset_ -= (unsigned long)count << BLIP_BUFFER_ACCURACY;
	}
	
	unsigned long clockRateFactor(long clock_rate) {
		double ratio = (double)sample_rate_ / clock_rate;
		long factor = (long)floor(ratio * (1L << BLIP_BUFFER_ACCURACY) + 0.5);
		return (unsigned long)factor;
	}
protected:
	unsigned long factor_;
	unsigned long offset_;
	long* buffer_;
	long buffer_size_;
	long reader_accum;
	int bass_shift;
	long sample_rate_;
	long clock_rate_;
	int bass_freq_;
	int length_;
};

class zxBlipSynth_ {
	friend class zxBlipSynth;
public:
	void init(short* p) {
		impulses = p;
		volume_unit_ = 0.0;
		kernel_unit = 0;
		buf = nullptr;
		last_amp = 0;
		delta_factor = 0;
	}

	int impulsesSize() {
		return BLIP_RES / 2 * BLIP_SYNTH_WIDTH + 1;
	}
	void adjustImpulse() {
		int size = impulsesSize();
		int i, p, p2, error;
		for(p = BLIP_RES; p-- >= BLIP_RES / 2; ) {
			error = kernel_unit;
			p2 = BLIP_RES - 2 - p;
			for(i = 1; i < size; i += BLIP_RES) {
				error -= impulses[i + p];
				error -= impulses[i + p2];
			}
			if(p == p2) error /= 2;
			impulses[size - BLIP_RES + p] += error;
		}
	}
	void trebleEq(zxBlipEq* eq);
	void volumeUnit(double v);
protected:
	double volume_unit_;
	short* impulses;
	long kernel_unit;
	zxBlipBuffer* buf;
	int last_amp;
	int delta_factor;
};

class zxBlipSynth {
public:
	zxBlipSynth() {
		impulses = new short[(BLIP_RES * (BLIP_SYNTH_QUALITY / 2) + 1) * sizeof(short) * 4];
		impl.init((short*)impulses);
	}

	virtual ~zxBlipSynth() {
		SAFE_DELETE(impulses);
	}
	
	void setVolume(double v) {
		impl.volumeUnit(v * (1.0 / (BLIP_SYNTH_RANGE < 0 ? -(BLIP_SYNTH_RANGE) : BLIP_SYNTH_RANGE)));
	}
	
	void setTrebleEq(double treble) {
		zxBlipEq eq = {0.0, 0, 44100, 0};
		eq.treble = treble;
		impl.trebleEq(&eq);
	}
	
	void setOutput(zxBlipBuffer* b) {
		impl.buf = b;
		impl.last_amp = 0;
	}
	
	void update(long t, int amplitude) {
		int delta = amplitude - impl.last_amp;
		impl.last_amp = amplitude;
		offsetResampled(t * impl.buf->factor_ + impl.buf->offset_, delta, impl.buf);
	}
	
	void offsetResampled(unsigned long, int delta, zxBlipBuffer* buff);
protected:
	short* impulses;
	zxBlipSynth_ impl;
};
