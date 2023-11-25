/*
 * sample_filter.h
 *
 *  Created on: Nov 5, 2023
 *      Author: x2800m
 */

#ifndef SAMPLEFILTER_H_
#define SAMPLEFILTER_H_

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 10000 Hz

* 0 Hz - 200 Hz
  gain = 1
  desired ripple = 1 dB
  actual ripple = 0.6673565605535 dB

* 500 Hz - 5000 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -41.03641191751322 dB

*/

#define SAMPLEFILTER_TAP_NUM 55

typedef struct {
  float history[SAMPLEFILTER_TAP_NUM];
  unsigned int last_index;
} SampleFilter;

void SampleFilter_init(SampleFilter* f);
void SampleFilter_put(SampleFilter* f, float input);
float SampleFilter_get(SampleFilter* f);

#endif /* SAMPLE_FILTER_H_ */
