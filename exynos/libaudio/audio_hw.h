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
#include <audio_utils/resampler.h>

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

    audio_source_t in_source;// * + 50 // adev + 200
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

    struct audio_stream_in **stream_in;      // * + 63 // adev + 252
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
    bool hifi_pcm_opened; //  adev + 289
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

    void *adev_pcm_out_buffer; // * + 79 // adev + 316
};

// size : 0xD8 -> 216
struct stream_out {
    struct audio_stream_out stream;
    pthread_mutex_t lock; // *out + 25  // out + 100

    struct pcm *pcm; // *out + 26 // out + 104
    struct pcm *hifi_pcm; // *out + 27 // out + 108
    struct pcm *spdif_pcm; // *out + 28 // out + 112
    struct pcm_config pcmconfig; // *out + 29 // out + 116  // size: 40

    audio_devices_t devices; // *out + 39 // out + 156

    bool *out_b_160; // *out + 40 // out + 160
    bool *out_b_161;
    bool *out_b_162;
    bool *out_b_163;

    void *out_v_164; // *out + 41 // out + 164

    int channel_mask; // *out + 42 // out + 168
    void *out_v_172; // *out + 43 // out + 172
    void *out_v_176; // *out + 44 // out + 176
    void *out_v_180; // *out + 45 // out + 180

    int in_sample_rate; // *out + 46 // out + 184
    audio_format_t in_format; // *out + 47 // out + 188
    void *out_v_192; // *out + 48 // out + 192
    struct resampler_itfe **resampler; // *out + 49 // out + 196
    void *out_resampler_pcm_out_buffer; // *out + 50 // out + 200

    struct audio_device *adev; // *out + 51 // out + 204
    void *out_v_208; // *out + 52 // out + 208
    void *out_v_212; // *out + 53 // out + 212
};

// size : 0x4E264 -> 1161144
struct CVQStream {
    unsigned int *L_0;
    void *cvq_v_4;// + 4
    pthread_t *read_on_thread;// + 8
    void *cvq_v_12;// + 12
    void *cvq_v_16;// + 16

    void *cvq_v_320040;// + 320040
    FILE *uart_char_dev;// + 320056

    void *cvq_v_320068;// + 320068
    void *cvq_v_320072;// + 320072

    int (*cvq_open)();// + 320076
    int (*cvq_close)();// + 320080
    int (*cvq_start)();// + 320084
    int (*readdirect)();// + 320088
    int (*cvq_read)();// + 320092
    int (*pcm_read_uart_char_dev)();// + 320096

};

// size : 0xB0 -> 176
struct stream_in {
    struct audio_stream_in stream;

    struct CVQStream * cvqStream;// *in + 17  //  in + 68

    int is_cvq;/// *in + 18  //  in + 72
    int in_int_76;/// *in + 19  //  in + 76
    pthread_t *pcm_read_thread;/// *in + 20  //  in + 80
    int in_int_84;/// *in + 21  //  in + 84

    int in_int_88;/// *in + 22  //  in + 88
    void *in_v_92;/// *in + 23  //  in + 92
    int max_bytes;/// *in + 24  //  in + 96
    int read_offset;/// *in + 25  //  in + 100

    int write_offset;/// *in + 26  //  in + 104
    int write_size;/// *in + 27  //  in + 108
    void *in_v_112;/// *in + 28  //  in + 112

    pthread_mutex_t lock;// *in + 29  //  in + 116

    struct pcm *pcm; // *in + 30  //  in + 120

    bool in_b_124; // *in + 31 // in + 124
    bool in_b_125;             // in + 125
    bool in_b_126;             // in + 126
    bool in_b_127;             // in + 127

    struct audio_config *config; // *in + 32 // in + 128

    int (*in_fun_132)();/// *in + 33  //  in + 132

    int (*get_next_buffer)(struct stream_in *stream_in);/// *in + 34  //  in + 136
    int (*in_fun_140)();/// *in + 35  //  in + 140

    void *in_v_144;/// *in + 36  //  in + 144

    void *in_v_148;/// *in + 37  //  in + 148
    int pcm_read_result;/// *in + 38  //  in + 152
    void *in_v_156;/// *in + 39  //  in + 156
    audio_devices_t devices; // *in + 40 // in + 160

    void *in_v_164;/// *in + 41  //  in + 164
    void *in_v_168;/// *in + 42  //  in + 168

    struct audio_device *adev; // *in + 43 // in + 172
};

// Tfa98xx fun
extern int NxpTfa98xx_Stop();


#endif  // __EXYNOS_AUDIOHAL_H__
