
#ifndef __EXYNOS_AUDIOHAL_DEF_H__
#define __EXYNOS_AUDIOHAL_DEF_H__

#include <system/audio.h>

struct pcm_config pcmconfig = {
    .channels = DEFAULT_OUTPUT_CHANNELS,
    .rate = DEFAULT_OUTPUT_SAMPLING_RATE,
    .period_size = PRIMARY_OUTPUT_PERIOD_SIZE,
    .period_count = PRIMARY_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = PRIMARY_OUTPUT_PERIOD_SIZE,
    .stop_threshold = PRIMARY_OUTPUT_STOP_THREASHOLD,
//    .silence_threshold = 0,
//    .avail_min = PRIMARY_OUTPUT_PERIOD_SIZE,
};

struct pcm_config pcm_config_amplifier = {
    .channels = DEFAULT_OUTPUT_CHANNELS,
    .rate = DEFAULT_OUTPUT_SAMPLING_RATE,
    .period_size = PRIMARY_OUTPUT_PERIOD_SIZE,
    .period_count = PRIMARY_OUTPUT_PERIOD_COUNT,
    .format = PCM_FORMAT_S16_LE,
    .start_threshold = PRIMARY_OUTPUT_PERIOD_SIZE,
    .stop_threshold = PRIMARY_OUTPUT_STOP_THREASHOLD,
//    .silence_threshold = 0,
//    .avail_min = PRIMARY_OUTPUT_PERIOD_SIZE,
};

#endif  // __EXYNOS_AUDIOHAL_DEF_H__