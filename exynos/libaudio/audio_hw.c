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

#define LOG_TAG "audio_hw_m86"
//#define LOG_NDEBUG 0

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

struct stream_out;
struct stream_in;

// size : 0x140 -> 320
struct audio_device {
    struct audio_hw_device device;
    pthread_mutex_t lock;// *+ 41 //  adev + 164 /* see note below on mutex acquisition order */
    audio_mode_t amode; // *+ 42 // adev + 168
    struct stream_out *out_device; // out_device; // *+ 43 // adev + 172

    struct stream_in *in_device; // *+ 44 // adev + 176
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

static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    return 44100;
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    ALOGV("out_set_sample_rate: %d", 0);
    return -ENOSYS;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    ALOGV("out_get_buffer_size: %d", 4096);
    return 4096;
}

static audio_channel_mask_t out_get_channels(const struct audio_stream *stream)
{
    ALOGV("out_get_channels");
    return AUDIO_CHANNEL_OUT_STEREO;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    ALOGV("out_get_format");
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
    ALOGV("out_set_format: %d",format);
    return -ENOSYS;
}

static int out_standby(struct audio_stream *stream)
{
    ALOGV("out_standby");
    return 0;
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    ALOGV("out_dump");
    return 0;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    ALOGV("out_set_parameters");
    return 0;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    ALOGV("out_get_parameters");
    return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    ALOGV("out_get_latency");
    return 0;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    ALOGV("out_set_volume: Left:%f Right:%f", left, right);
    return 0;
}

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    ALOGV("out_write: bytes: %d", bytes);

    struct stream_out *out = (struct stream_out *)stream;
    
    return bytes;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    *dsp_frames = 0;
    ALOGV("out_get_render_position: dsp_frames: %p", dsp_frames);
    return -EINVAL;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOGV("out_add_audio_effect: %p", effect);
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    ALOGV("out_remove_audio_effect: %p", effect);
    return 0;
}

static int out_get_next_write_timestamp(const struct audio_stream_out *stream,
                                        int64_t *timestamp)
{
    *timestamp = 0;
    ALOGV("out_get_next_write_timestamp: %ld", (long int)(*timestamp));
    return -EINVAL;
}

/** audio_stream_in implementation **/
static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    ALOGV("in_get_sample_rate");
    return 8000;
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    ALOGV("in_set_sample_rate: %d", rate);
    return -ENOSYS;
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    ALOGV("in_get_buffer_size: %d", 320);
    return 320;
}

static audio_channel_mask_t in_get_channels(const struct audio_stream *stream)
{
    ALOGV("in_get_channels: %d", AUDIO_CHANNEL_IN_MONO);
    return AUDIO_CHANNEL_IN_MONO;
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int in_set_format(struct audio_stream *stream, audio_format_t format)
{
    return -ENOSYS;
}

static int in_standby(struct audio_stream *stream)
{
    struct stream_in *in = (struct stream_in *)stream;
    return 0;
}

static int in_dump(const struct audio_stream *stream, int fd)
{
    return 0;
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    return 0;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    return strdup("");
}

static int in_set_gain(struct audio_stream_in *stream, float gain)
{
    return 0;
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
                       size_t bytes)
{
    ALOGV("in_read: bytes %d", bytes);

    struct stream_in *in = (struct stream_in *)stream;
    return bytes;
}

static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
    return 0;
}

static int in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    return 0;
}

static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out,
                                   const char *address __unused)
{
    ALOGV("adev_open_output_stream...");

    struct audio_device *ladev = (struct audio_device *)dev;
    struct stream_out *out;
    int ret;

    out = (struct stream_out *)calloc(1, sizeof(struct stream_out));
    if (!out)
        return -ENOMEM;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;
    out->stream.get_next_write_timestamp = out_get_next_write_timestamp;

    *stream_out = &out->stream;
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    ALOGV("adev_close_output_stream...");
    free(stream);
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    ALOGV("adev_set_parameters");
    return -ENOSYS;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    ALOGV("adev_get_parameters");
    return strdup("");
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    ALOGV("adev_init_check");
    return 0;
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    ALOGV("adev_set_voice_volume: %f", volume);
    return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    ALOGV("adev_set_master_volume: %f", volume);
    return -ENOSYS;
}

static int adev_get_master_volume(struct audio_hw_device *dev, float *volume)
{
    ALOGV("adev_get_master_volume: %f", *volume);
    return -ENOSYS;
}

static int adev_set_master_mute(struct audio_hw_device *dev, bool muted)
{
    ALOGV("adev_set_master_mute: %d", muted);
    return -ENOSYS;
}

static int adev_get_master_mute(struct audio_hw_device *dev, bool *muted)
{
    ALOGV("adev_get_master_mute: %d", *muted);
    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, audio_mode_t mode)
{
    ALOGV("adev_set_mode: %d", mode);
    return 0;
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    ALOGV("adev_set_mic_mute: %d",state);
    return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    ALOGV("adev_get_mic_mute");
    return -ENOSYS;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                         const struct audio_config *config)
{
    ALOGV("adev_get_input_buffer_size: %d", 320);
    return 320;
}

static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in,
                                  audio_input_flags_t flags __unused,
                                  const char *address __unused,
                                  audio_source_t source __unused)
{
    ALOGV("adev_open_input_stream...");

    struct audio_device *ladev = (struct audio_device *)dev;
    struct stream_in *in;
    int ret;

    in = (struct stream_in *)calloc(1, sizeof(struct stream_in));
    if (!in)
        return -ENOMEM;

    in->stream.common.get_sample_rate = in_get_sample_rate;
    in->stream.common.set_sample_rate = in_set_sample_rate;
    in->stream.common.get_buffer_size = in_get_buffer_size;
    in->stream.common.get_channels = in_get_channels;
    in->stream.common.get_format = in_get_format;
    in->stream.common.set_format = in_set_format;
    in->stream.common.standby = in_standby;
    in->stream.common.dump = in_dump;
    in->stream.common.set_parameters = in_set_parameters;
    in->stream.common.get_parameters = in_get_parameters;
    in->stream.common.add_audio_effect = in_add_audio_effect;
    in->stream.common.remove_audio_effect = in_remove_audio_effect;
    in->stream.set_gain = in_set_gain;
    in->stream.read = in_read;
    in->stream.get_input_frames_lost = in_get_input_frames_lost;

    *stream_in = &in->stream;
    return 0;

err_open:
    free(in);
    *stream_in = NULL;
    return ret;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *in)
{
    ALOGV("adev_close_input_stream...");
    return;
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    ALOGV("adev_dump");
    return 0;
}

static int adev_close(hw_device_t *device)
{
    ALOGV("adev_close");
    struct audio_device *adev = (struct audio_device *)device;
    NxpTfa98xx_Stop();
    mixer_close(adev->mixer);
    mixer_close(adev->hifi_mixer);
    audio_route_free(adev->audio_route);
    free(device);
    return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    ALOGV("adev_open: %s", name);

    struct audio_device *adev;
    int ret;

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
        return -EINVAL;

    adev = calloc(1, sizeof(struct audio_device));
    if (!adev)
        return -ENOMEM;

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->device.common.module = (struct hw_module_t *) module;
    adev->device.common.close = adev_close;

    adev->device.init_check = adev_init_check;
    adev->device.set_voice_volume = adev_set_voice_volume;
    adev->device.set_master_volume = adev_set_master_volume;
    adev->device.get_master_volume = adev_get_master_volume;
    adev->device.set_master_mute = adev_set_master_mute;
    adev->device.get_master_mute = adev_get_master_mute;
    adev->device.set_mode = adev_set_mode;
    adev->device.set_mic_mute = adev_set_mic_mute;
    adev->device.get_mic_mute = adev_get_mic_mute;
    adev->device.set_parameters = adev_set_parameters;
    adev->device.get_parameters = adev_get_parameters;
    adev->device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->device.open_output_stream = adev_open_output_stream;
    adev->device.close_output_stream = adev_close_output_stream;
    adev->device.open_input_stream = adev_open_input_stream;
    adev->device.close_input_stream = adev_close_input_stream;
    adev->device.dump = adev_dump;

    *device = &adev->device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = AUDIO_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "m86 audio HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_module_methods,
    },
};
