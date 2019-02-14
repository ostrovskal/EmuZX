
#include "stdafx.h"
#include "zxBlipBuffer.h"

#define PI 3.1415926535897932384626433832795029

#define BLIP_FWD(i) t0 = i0 * delta + buf[fwd + i]; t1 = imp[BLIP_RES * (i + 1)] * delta + buf[fwd + 1 + i]; i0 = imp[BLIP_RES * (i + 2)]; buf[fwd + i] = t0; buf[fwd + 1 + i] = t1;
#define BLIP_REV(r) t0 = i0 * delta + buf[rev - r]; t1 = imp[BLIP_RES * r] * delta + buf[rev + 1 - r]; i0 = imp[BLIP_RES * (r - 1)]; buf[rev - r] = t0; buf[rev + 1 - r] = t1;

static void gen_sinc(float *out, int count, double oversample, double treble, double cutoff) {
	int i;

	if(cutoff > 0.999) cutoff = 0.999;
	if(treble < -300.0) treble = -300.0;
	if(treble > 5.0) treble = 5.0;

	double maxh = 4096.0;
	double rolloff = pow(10.0, 1.0 / (maxh * 20.0) * treble / (1.0 - cutoff));
	double pow_a_n = pow(rolloff, maxh - maxh * cutoff);
	double to_angle = PI / 2 / maxh / oversample;
	for(i = 0; i < count; i++) {

		double angle = ((i - count) * 2 + 1) * to_angle;
		double c = rolloff * cos((maxh - 1.0) * angle) - cos(maxh * angle);
		double cos_nc_angle = cos(maxh * cutoff * angle);
		double cos_nc1_angle = cos((maxh * cutoff - 1.0) * angle);
		double cos_angle = cos(angle);

		c = c * pow_a_n - rolloff * cos_nc1_angle + cos_nc_angle;
		double d = 1.0 + rolloff * (rolloff - cos_angle - cos_angle);
		double b = 2.0 - cos_angle - cos_angle;
		double a = 1.0 - cos_angle - cos_nc_angle + cos_nc1_angle;

		out[i] = (float)((a * d + c * b) / (b * d));
	}
}

static void blip_eq_generate(zxBlipEq* eq, float *out, int count) {
	int i;
	double to_fraction, cutoff;
	double oversample = BLIP_RES * 2.25 / count + 0.85;
	double half_rate = eq->sample_rate * 0.5;

	if(eq->cutoff_freq) oversample = half_rate / eq->cutoff_freq;
	cutoff = eq->rolloff_freq * oversample / half_rate;
	gen_sinc(out, count, BLIP_RES * oversample, eq->treble, cutoff);
	to_fraction = PI / (count - 1);
	for(i = count; i--; ) out[i] *= (float)(0.54 - 0.46 * cos(i * to_fraction));
}

void zxBlipSynth::offsetResampled(unsigned long time, int delta, zxBlipBuffer* blip_buf) {
	int phase, fwd, rev, mid;
	short *imp;
	long *buf, i0, t0, t1;

	delta *= impl.delta_factor;
	phase = (int)(time >> (BLIP_BUFFER_ACCURACY - BLIP_PHASE_BITS) & (BLIP_RES - 1));
	imp = impulses + BLIP_RES - phase;
	buf = blip_buf->buffer_ + (time >> BLIP_BUFFER_ACCURACY);
	i0 = *imp;

	fwd = (BLIP_WIDEST_IMPULSE_ - BLIP_SYNTH_QUALITY) / 2;
	rev = fwd + BLIP_SYNTH_QUALITY - 2;

	BLIP_FWD(0);
	if(BLIP_SYNTH_QUALITY > 8) {
		BLIP_FWD(2);
	}
	if(BLIP_SYNTH_QUALITY > 12) {
		BLIP_FWD(4);
	}

	mid = BLIP_SYNTH_QUALITY / 2 - 1;
	t0 = i0 * delta + buf[fwd + mid - 1];
	t1 = imp[BLIP_RES * mid] * delta + buf[fwd + mid];
	imp = impulses + phase;
	i0 = imp[BLIP_RES * mid];
	buf[fwd + mid - 1] = t0;
	buf[fwd + mid] = t1;

	if(BLIP_SYNTH_QUALITY > 12) {
		BLIP_REV(6);
	}
	if(BLIP_SYNTH_QUALITY > 8) {
		BLIP_REV(4);
	}
	BLIP_REV(2);

	t0 = i0 * delta + buf[rev];
	t1 = *imp * delta + buf[rev + 1];
	buf[rev] = t0;
	buf[rev + 1] = t1;
}

void zxBlipSynth_::trebleEq(zxBlipEq* eq) {
	double total, base_unit, rescale, sum, next, vol;
	int impulses_size, i;
	float fimpulse[BLIP_RES / 2 * (BLIP_WIDEST_IMPULSE_ - 1) + BLIP_RES * 2];
	int half_size = BLIP_RES / 2 * (BLIP_SYNTH_WIDTH - 1);

	blip_eq_generate(eq, &fimpulse[BLIP_RES], half_size);

	for(i = BLIP_RES; i--; ) fimpulse[BLIP_RES + half_size + i] = fimpulse[BLIP_RES + half_size - 1 - i];
	for(i = 0; i < BLIP_RES; i++) fimpulse[i] = 0.0f;
	total = 0.0;
	for(i = 0; i < half_size; i++) total += fimpulse[BLIP_RES + i];
	base_unit = 32768.0; 
	rescale = base_unit / 2 / total;
	kernel_unit = (long)base_unit;
	sum = 0.0;
	next = 0.0;
	impulses_size = impulsesSize();
	for(i = 0; i < impulses_size; i++) {
		impulses[i] = (short)floor((next - sum) * rescale + 0.5);
		sum += fimpulse[i];
		next += fimpulse[i + BLIP_RES];
	}

	adjustImpulse();
	vol = volume_unit_;
	if(vol) {
		volume_unit_ = 0.0;
		volumeUnit(vol);
	}
}

void zxBlipSynth_::volumeUnit(double new_unit) {
	if(new_unit != volume_unit_) {
		double factor;
		if(!kernel_unit) {
			zxBlipEq eq = {-8.0, 0, 44100, 0};
			trebleEq(&eq);
		}
		volume_unit_ = new_unit;
		factor = new_unit * (1L << BLIP_SAMPLE_BITS) / kernel_unit;
		if(factor > 0.0) {
			int shift = 0;
			while(factor < 2.0) {
				shift++;
				factor *= 2.0;
			}
			if(shift) {
				long offset = 0x8000 + (1 << (shift - 1));
				long offset2 = 0x8000 >> shift;
				int i;
				kernel_unit >>= shift;
				for(i = impulsesSize(); i--; ) impulses[i] = (short)(((impulses[i] + offset) >> shift) - offset2);
				adjustImpulse();
			}
		}
		delta_factor = (int)floor(factor + 0.5);
	}
}

long zxBlipBuffer::readSamples(short* out, long max_samples, int stereo) {
	long count = samplesAvail();
	if(count > max_samples) count = max_samples;
	if(count) {
		int sample_shift = BLIP_SAMPLE_BITS - 16;
		int my_bass_shift = bass_shift;
		long accum = reader_accum;
		long *in = buffer_;
		if(!stereo) {
			for(int n = count; n--; ) {
				long s = accum >> sample_shift;
				accum -= accum >> my_bass_shift;
				accum += *in++;
				*out++ = (short)s;
				if((short)s != s) out[-1] = (short)(0x7FFF - (s >> 24));
			}
		} else {
			for(int n = count; n--; ) {
				long s = accum >> sample_shift;
				accum -= accum >> my_bass_shift;
				accum += *in++;
				*out = (short)s;
				out += 2;
				if((short)s != s) out[-2] = (short)(0x7FFF - (s >> 24));
			}
		}
		reader_accum = accum;
		removeSamples(count);
	}
	return count;
}
