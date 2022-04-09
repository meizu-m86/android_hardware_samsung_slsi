/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __EXYNOS_AUDIOHAL_H__
#define __EXYNOS_AUDIOHAL_H__

#include <errno.h>
#include <malloc.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>

#include <cutils/log.h>

#include <hardware/hardware.h>
#include <system/audio.h>
#include <hardware/audio.h>

#include <audio_route/audio_route.h>

/* PCM Interface of ALSA & Compress Offload Interface */
#include <tinyalsa/asoundlib.h>
#include <tinycompress/tinycompress.h>
#include <compress_params.h>


enum {
    OUT_DEVICE_SPEAKER,
    OUT_DEVICE_HEADSET,
    OUT_DEVICE_HEADPHONES,
    OUT_DEVICE_BT_SCO,
    OUT_DEVICE_SPEAKER_AND_HEADSET,
    OUT_DEVICE_OFF,
    OUT_DEVICE_TAB_SIZE,           /* number of rows in route_configs[][] */
    OUT_DEVICE_NONE,
    OUT_DEVICE_CNT
};

enum {
    IN_SOURCE_MIC,
    IN_SOURCE_CAMCORDER,
    IN_SOURCE_VOICE_RECOGNITION,
    IN_SOURCE_VOICE_COMMUNICATION,
    IN_SOURCE_OFF,
    IN_SOURCE_TAB_SIZE,            /* number of lines in route_configs[][] */
    IN_SOURCE_NONE,
    IN_SOURCE_CNT
};


struct stream_out;
struct stream_in;

// size : 0x140 -> 320
struct audio_device {
    struct audio_hw_device device;
    pthread_mutex_t lock;// *+ 41 //  adev + 164 /* see note below on mutex acquisition order */
    audio_mode_t amode; // *+ 42 // adev + 168
    audio_devices_t out_device; // out_device; // *+ 43 // adev + 172
    // audio_devices_t :/system/media/audio/include/system/audio.h  844: typedef uint32_t audio_devices_t;
    audio_devices_t in_device; // *+ 44 // adev + 176
    bool mic_mute_state; // adev + 180
    bool screen_state; // adev + 181
    bool bt_headset_nrec; // adev + 182
    bool voice_call_wb; // adev + 183

    bool mic_nr_off; // adev + 184
    bool b_185; // adev + 185
    bool b_186; // adev + 186
    bool b_187; // adev + 187

    struct audio_route* audio_route;// *+ 47 // adev + 188
    struct mixer *mixer;// * +48 // adev + 192
    struct mixer *hifi_mixer;// * + 49 // adev + 196

    void *v_50;// * + 50 // adev + 200
    void *v_51;// * + 51 // adev + 204
    void *v_52;// * + 52 // adev + 208
    void *v_53;// * + 53 // adev + 212
    void *v_54;// * + 54 // adev + 216

    struct pcm *pcm_voice_out;// * + 55 // adev + 220 // VOICE_OUT
    struct pcm *pcm_voice_in;// * + 56 // adev + 224 // VOICE_IN

    struct pcm *pcm_be_out;// * + 57 // adev + 228 // VOICE_BE_OUT
    struct pcm *pcm_be_in;// * + 58 // adev + 232 // VOICE_BE_IN

    /* BT-SCO */
    struct pcm *pcm_btsco_out;// * + 59 // adev + 236      // SCO_OUT
    struct pcm *pcm_btsco_in;// * + 60 // adev + 240     // SCO_IN

    struct pcm *pcm_pa_out;// * + 61 // adev + 244      // PA_OUT
    struct pcm *pcm_pa_in;// * + 62 // adev + 248     // PA_IN

    void *v_63;      // * + 63 // adev + 252
    struct audio_stream_out **stream_out;      // * + 64 // adev + 256

    bool tfa_power_state; // adev + 260
    bool mute_state; // adev + 261
    bool b_262; // adev + 262
    bool b_263; // adev + 263

    int sample_rate; // * + 66 //  adev + 264

    bool b_268; // * + 67 // adev + 268
    bool output_stream_state; // adev + 269
    bool b_270;  // adev + 270
    bool b_271;  // adev + 271

    int headset_volume;      // * + 68 // adev + 272
    int headphone_volume;    // * + 69 // adev + 276
    void *v_70;      // * + 70 // adev + 280
    void *v_71;      // * + 71 // adev + 284

    bool hifi_state; // * + 72 // adev + 288
    bool b_289; //  adev + 289
    bool b_290; // adev + 290
    bool b_291; // adev + 291

    int hifi_gain_state; // * + 73 // adev + 292
    int hifi_gain; // * + 74 // adev + 296
    int hifi_volume; // * + 75 // adev + 300

    bool is_oversea; // * + 76 // adev + 304
    bool b_305; // adev + 305
    bool b_306; // adev + 306
    bool b_307; // adev + 307

    int hifi_impedance; // * + 77 // adev + 308
    bool messagerecord_state; // * + 78 // adev + 312
    bool asr_ready; // adev + 313
    bool b_314; // adev + 314
    bool b_315; // adev + 315

    void *v_79; // * + 79 // adev + 316
};

// size : 0xD8 -> 216
struct stream_out {
    struct audio_stream_out stream;
    pthread_mutex_t lock; // *out + 25  // out + 100

    void *v_104; // *out + 26 // out + 104
    void *v_108; // *out + 27 // out + 108
    void *v_112; // *out + 28 // out + 112
    void *v_116; // *out + 29 // out + 116

    void *v_120; // *out + 30 // out + 120
    void *v_124; // *out + 31 // out + 124
    void *v_128; // *out + 32 // out + 128
    void *v_132; // *out + 33 // out + 132

    void *v_136; // *out + 34 // out + 136
    void *v_140; // *out + 35 // out + 140
    void *v_144; // *out + 36 // out + 144
    void *v_148; // *out + 37 // out + 148

    void *v_152; // *out + 38 // out + 152
    void *v_156; // *out + 39 // out + 156
    void *v_160; // *out + 40 // out + 160
    void *v_164; // *out + 41 // out + 164

    int channel_mask; // *out + 42 // out + 168
    void *v_172; // *out + 43 // out + 172
    void *v_176; // *out + 44 // out + 176
    void *v_180; // *out + 45 // out + 180

    int sample_rate; // *out + 46 // out + 184
    audio_format_t format; // *out + 47 // out + 188
    void *v_192; // *out + 48 // out + 192
    void *v_196; // *out + 49 // out + 196
    void *v_200; // *out + 50 // out + 200

    struct audio_device *adev; // *out + 51 // out + 204
    void *v_208; // *out + 52 // out + 208
    void *v_212; // *out + 53 // out + 212
};

// size : 0xB0 -> 176
struct stream_in {
    struct audio_stream_in stream;
    pthread_mutex_t lock;// *in + 29  //  in + 116

    bool b_124; // *in + 31 // in + 124
    bool b_125;             // in + 125
    bool b_126;             // in + 126
    bool b_127;             // in + 127

    struct audio_config *config; // *in + 32 // in + 128

    audio_devices_t devices; // *in + 40 // in + 160

    struct audio_device *adev; // *in + 43 // in + 172
};

// Tfa98xx fun
extern int NxpTfa98xx_Stop();


#endif  // __EXYNOS_AUDIOHAL_H__
