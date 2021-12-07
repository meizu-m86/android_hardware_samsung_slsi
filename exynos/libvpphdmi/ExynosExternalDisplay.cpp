//#define LOG_NDEBUG 0
#include "ExynosHWC.h"
#include "ExynosHWCUtils.h"
#include "ExynosMPPModule.h"
#include "ExynosExternalDisplay.h"
#include "ExynosSecondaryDisplayModule.h"
//#include "decon_tv.h"
#include <errno.h>

extern struct v4l2_dv_timings dv_timings[];
bool is_same_dv_timings(const struct v4l2_dv_timings *t1,
        const struct v4l2_dv_timings *t2)
{
    if (t1->type == t2->type &&
            t1->bt.width == t2->bt.width &&
            t1->bt.height == t2->bt.height &&
            t1->bt.interlaced == t2->bt.interlaced &&
            t1->bt.polarities == t2->bt.polarities &&
            t1->bt.pixelclock == t2->bt.pixelclock &&
            t1->bt.hfrontporch == t2->bt.hfrontporch &&
            t1->bt.vfrontporch == t2->bt.vfrontporch &&
            t1->bt.vsync == t2->bt.vsync &&
            t1->bt.vbackporch == t2->bt.vbackporch &&
            (!t1->bt.interlaced ||
             (t1->bt.il_vfrontporch == t2->bt.il_vfrontporch &&
              t1->bt.il_vsync == t2->bt.il_vsync &&
              t1->bt.il_vbackporch == t2->bt.il_vbackporch)))
        return true;
    return false;
}

int ExynosExternalDisplay::getDVTimingsIndex(int preset)
{
    for (int i = 0; i < SUPPORTED_DV_TIMINGS_NUM; i++) {
        if (preset == preset_index_mappings[i].preset)
            return preset_index_mappings[i].dv_timings_index;
    }
    return -1;
}

ExynosExternalDisplay::ExynosExternalDisplay(struct exynos5_hwc_composer_device_1_t *pdev)
    :   ExynosDisplay(EXYNOS_EXTERNAL_DISPLAY, pdev),
        mEnabled(false),
        mBlanked(false),
        mUseSubtitles(false),
        mReserveMemFd(-1),
        mDRMTempBuffer(NULL),
        mFlagIONBufferAllocated(false)
{
    mXres = 0;
    mYres = 0;
    mXdpi = 0;
    mYdpi = 0;
    mVsyncPeriod = 0;
    
}

ExynosExternalDisplay::~ExynosExternalDisplay()
{
    if (mDRMTempBuffer != NULL) {
        mAllocDevice->free(mAllocDevice, mDRMTempBuffer);
        mDRMTempBuffer = NULL;
    }
    if (mReserveMemFd > 0)
        close(mReserveMemFd);
}

int ExynosExternalDisplay::prepare(hwc_display_contents_1_t* contents)
{
    ExynosDisplay::prepare(contents);
    return 0;
}

int ExynosExternalDisplay::set(hwc_display_contents_1_t* contents)
{
    int err = 0;
    bool drm_skipped = false;

    if (!mEnabled || mBlanked) {
        for (size_t i = 0; i < contents->numHwLayers; i++) {
            hwc_layer_1_t &layer = contents->hwLayers[i];
            if (layer.acquireFenceFd >= 0) {
                close(layer.acquireFenceFd);
                layer.acquireFenceFd = -1;
            }
        }
        return 0;
    }

    if ((mHasDrmSurface == true) && (mForceOverlayLayerIndex != -1) &&
        (mLayerInfos[mForceOverlayLayerIndex]->mExternalMPP != NULL)) {
        hwc_layer_1_t &layer = contents->hwLayers[mForceOverlayLayerIndex];
        if (mFlagIONBufferAllocated == false) {
            layer.flags |= HWC_SKIP_RENDERING;
            drm_skipped = true;
        } else {
            layer.flags &= ~(HWC_SKIP_RENDERING);
        }
    }
    err = ExynosDisplay::set(contents);

    /* HDMI was disabled to change S3D mode */
    if (mEnabled == false)
        return 0;

    /* Restore flags */
    if (drm_skipped) {
        if ((mHasDrmSurface == true) && (mForceOverlayLayerIndex != -1) &&
                (mLayerInfos[mForceOverlayLayerIndex]->mExternalMPP != NULL)) {
            hwc_layer_1_t &layer = contents->hwLayers[mForceOverlayLayerIndex];
            layer.flags &= ~(HWC_SKIP_RENDERING);
        }
    }

    if (this->mYuvLayers == 0 && !mHwc->local_external_display_pause) {
        if (mHwc->mS3DMode == S3D_MODE_RUNNING && contents->numHwLayers > 1) {
            int preset = convert3DTo2D(mHwc->mHdmiCurrentPreset);
            if (isPresetSupported(preset)) {
                ALOGI("S3D video is removed, Set Resolution(%d)", preset);
                setPreset(preset);
                mHwc->mS3DMode = S3D_MODE_STOPPING;
                mHwc->mHdmiPreset = preset;
                if (mHwc->procs)
                    mHwc->procs->invalidate(mHwc->procs);
            } else {
                ALOGI("S3D video is removed, Resolution(%d) is not supported. mHdmiCurrentPreset(%d)", preset, mHwc->mHdmiCurrentPreset);
                mHwc->mS3DMode = S3D_MODE_DISABLED;
                mHwc->mHdmiPreset = mHwc->mHdmiCurrentPreset;
            }
        }
    }

    return err;
}

int ExynosExternalDisplay::openHdmi()
{
    int ret = 0;
    int sw_fd;

    if (mHwc->externalDisplay->mDisplayFd > 0)
        ret = mHwc->externalDisplay->mDisplayFd;
    else {
        mHwc->externalDisplay->mDisplayFd = open("/dev/graphics/fb1", O_RDWR);
        if (mHwc->externalDisplay->mDisplayFd < 0) {
            ALOGE("failed to open framebuffer for externalDisplay");
        }
        ret = mHwc->externalDisplay->mDisplayFd;
    }

    ALOGD("open fd for HDMI(%d)", ret);

    return ret;
}

void ExynosExternalDisplay::setHdmiStatus(bool status)
{
    if (status) {
#if defined(USES_VIRTUAL_DISPLAY)
        char value[PROPERTY_VALUE_MAX];
        property_get("wlan.wfd.status", value, "disconnected");
        bool bWFDDisconnected = !strcmp(value, "disconnected");

        if (bWFDDisconnected) {
#endif
        if (mEnabled == false && mHwc->mS3DMode != S3D_MODE_DISABLED)
            mHwc->mHdmiResolutionChanged = true;

        if (mEnabled == false)
            requestIONMemory();
        enable();
#if defined(USES_VIRTUAL_DISPLAY)
        }
#endif
    } else {
        disable();

        if (mDRMTempBuffer != NULL) {
            mAllocDevice->free(mAllocDevice, mDRMTempBuffer);
            mDRMTempBuffer = NULL;
        }
    }
}

bool ExynosExternalDisplay::isPresetSupported(unsigned int preset)
{
    bool found = false;
    int index = 0;
    int ret = 0;
    exynos_hdmi_data hdmi_data;
    int dv_timings_index = getDVTimingsIndex(preset);

    if (dv_timings_index < 0) {
        ALOGE("%s: unsupported preset, %d", __func__, preset);
        return -1;
    }

    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_ENUM_PRESET;
    while (true) {
        hdmi_data.etimings->index = index++;
        ret = ioctl(this->mDisplayFd, EXYNOS_GET_HDMI_CONFIG, &hdmi_data);

        if (ret < 0) {
            if (errno == EINVAL)
                break;
            ALOGE("%s: enum_dv_timings error, %d", __func__, errno);
            return -1;
        }

        ALOGV("%s: %d width=%d height=%d",
                __func__, hdmi_data.etimings->index,
                hdmi_data.etimings->timings.bt.width, hdmi_data.etimings->timings.bt.height);

        if (is_same_dv_timings(&hdmi_data.etimings->timings, &dv_timings[dv_timings_index])) {
            mXres  = hdmi_data.etimings->timings.bt.width;
            mYres  = hdmi_data.etimings->timings.bt.height;
            found = true;
            mHwc->mHdmiCurrentPreset = preset;
            break;
        }
    }
    return found;
}

int ExynosExternalDisplay::getConfig()
{
    if (!mHwc->hdmi_hpd)
        return -1;

    exynos_hdmi_data hdmi_data;
    int dv_timings_index = 0;

    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_PRESET;
    if (ioctl(this->mDisplayFd, EXYNOS_GET_HDMI_CONFIG, &hdmi_data) < 0) {
        ALOGE("%s: g_dv_timings error, %d", __func__, errno);
        return -1;
    }

        mActiveConfigIndex = 0;

    for (int i = 0; i < SUPPORTED_DV_TIMINGS_NUM; i++) {
        dv_timings_index = preset_index_mappings[i].dv_timings_index;
        if (is_same_dv_timings(&hdmi_data.etimings->timings, &dv_timings[dv_timings_index])) {
            float refreshRate = (float)((float)hdmi_data.timings->bt.pixelclock /
                    ((hdmi_data.timings->bt.width + hdmi_data.timings->bt.hfrontporch + hdmi_data.timings->bt.hsync + hdmi_data.timings->bt.hbackporch) *
                     (hdmi_data.timings->bt.height + hdmi_data.timings->bt.vfrontporch + hdmi_data.timings->bt.vsync + hdmi_data.timings->bt.vbackporch)));
            mXres = hdmi_data.timings->bt.width;
            mYres = hdmi_data.timings->bt.height;
            mVsyncPeriod = 1000000000 / refreshRate;
            mHwc->mHdmiCurrentPreset = preset_index_mappings[i].preset;
            break;
        }
    }
    ALOGD("HDMI resolution is (%d x %d)", mXres, mYres);

    return 0;
}

void ExynosExternalDisplay::setHdmiResolution(int resolution, int s3dMode)
{
    if (resolution == 0)
        resolution = mHwc->mHdmiCurrentPreset;
    if (s3dMode == S3D_NONE) {
        if (mHwc->mHdmiCurrentPreset == resolution)
            return;
        mHwc->mHdmiPreset = resolution;
        mHwc->mHdmiResolutionChanged = true;
        mHwc->procs->invalidate(mHwc->procs);
        return;
    }

    switch (resolution) {
    case HDMI_720P_60:
        resolution = S3D_720P_60_BASE + s3dMode;
        break;
    case HDMI_720P_59_94:
        resolution = S3D_720P_59_94_BASE + s3dMode;
        break;
    case HDMI_720P_50:
        resolution = S3D_720P_50_BASE + s3dMode;
        break;
    case HDMI_1080P_24:
        resolution = S3D_1080P_24_BASE + s3dMode;
        break;
    case HDMI_1080P_23_98:
        resolution = S3D_1080P_23_98_BASE + s3dMode;
        break;
    case HDMI_1080P_30:
        resolution = S3D_1080P_30_BASE + s3dMode;
        break;
    case HDMI_1080I_60:
        if (s3dMode != S3D_SBS)
            return;
        resolution = V4L2_DV_1080I60_SB_HALF;
        break;
    case HDMI_1080I_59_94:
        if (s3dMode != S3D_SBS)
            return;
        resolution = V4L2_DV_1080I59_94_SB_HALF;
        break;
    case HDMI_1080P_60:
        if (s3dMode != S3D_SBS && s3dMode != S3D_TB)
            return;
        resolution = S3D_1080P_60_BASE + s3dMode;
        break;
    default:
        return;
    }
    mHwc->mHdmiPreset = resolution;
    mHwc->mHdmiResolutionChanged = true;
    mHwc->mS3DMode = S3D_MODE_READY;
    mHwc->procs->invalidate(mHwc->procs);
}

void ExynosExternalDisplay::cleanConfigurations()
{
    mConfigurations.clear();
}

void ExynosExternalDisplay::dumpConfigurations()
{
    ALOGI("External display configurations:: total(%d), active configuration(%d)",
            mConfigurations.size(), mActiveConfigIndex);
    for (size_t i = 0; i <  mConfigurations.size(); i++ ) {
        unsigned int dv_timings_index = preset_index_mappings[mConfigurations[i]].dv_timings_index;
        v4l2_dv_timings configuration = dv_timings[dv_timings_index];
        float refresh_rate = (float)((float)configuration.bt.pixelclock /
                ((configuration.bt.width + configuration.bt.hfrontporch + configuration.bt.hsync + configuration.bt.hbackporch) *
                 (configuration.bt.height + configuration.bt.vfrontporch + configuration.bt.vsync + configuration.bt.vbackporch)));
        uint32_t vsyncPeriod = 1000000000 / refresh_rate;
        ALOGI("%d : type(%d), %d x %d, fps(%f), vsyncPeriod(%d)", i, configuration.type, configuration.bt.width,
                configuration.bt.height,
                refresh_rate, vsyncPeriod);
    }
}

int ExynosExternalDisplay::enable()
{
    if (mEnabled)
        return 0;

    if (mBlanked)
        return 0;

    char value[PROPERTY_VALUE_MAX];
    property_get("persist.hdmi.hdcp_enabled", value, "1");
    int hdcp_enabled = atoi(value);
    ALOGD("%s:: hdcp_enabled (%d)", __func__, hdcp_enabled);

    exynos_hdmi_data hdmi_data;
    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_HDCP;
    hdmi_data.hdcp = hdcp_enabled;

    if ((mDisplayFd < 0) && (openHdmi() < 0))
        return -1;

    if (ioctl(this->mDisplayFd, EXYNOS_SET_HDMI_CONFIG, &hdmi_data) < 0) {
        ALOGE("%s: failed to set HDCP status %d", __func__, errno);
    }

    /* "2" is RGB601_16_235 */
    property_get("persist.hdmi.color_range", value, "2");
    int color_range = atoi(value);

#if 0 // This should be changed
    if (exynos_v4l2_s_ctrl(mMixerLayers[mUiIndex].fd, V4L2_CID_TV_SET_COLOR_RANGE,
                           color_range) < 0)
        ALOGE("%s: s_ctrl(CID_TV_COLOR_RANGE) failed %d", __func__, errno);
#endif

    int err = ioctl(mDisplayFd, FBIOBLANK, FB_BLANK_UNBLANK);
    if (err < 0) {
        if (errno == EBUSY)
            ALOGI("unblank ioctl failed (display already unblanked)");
        else
            ALOGE("unblank ioctl failed: %s", strerror(errno));
        return -errno;
    }

    mEnabled = true;
    return 0;
}

void ExynosExternalDisplay::disable()
{
    if (!mEnabled)
        return;

    blank();

    mEnabled = false;
    checkIONBufferPrepared();
}

void ExynosExternalDisplay::setPreset(int preset)
{
    mHwc->mHdmiResolutionChanged = false;
    mHwc->mHdmiResolutionHandled = false;
    mHwc->hdmi_hpd = false;
    int dv_timings_index = getDVTimingsIndex(preset);
    if (dv_timings_index < 0) {
        ALOGE("invalid preset(%d)", preset);
        return;
    }

    disable();

    exynos_hdmi_data hdmi_data;
    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_PRESET;
    hdmi_data.timings = &dv_timings[dv_timings_index];
    if (ioctl(this->mDisplayFd, EXYNOS_SET_HDMI_CONFIG, &hdmi_data) != -1) {
        if (mHwc->procs)
            mHwc->procs->hotplug(mHwc->procs, HWC_DISPLAY_EXTERNAL, false);
    }
}

int ExynosExternalDisplay::convert3DTo2D(int preset)
{
    switch (preset) {
    case V4L2_DV_720P60_FP:
    case V4L2_DV_720P60_SB_HALF:
    case V4L2_DV_720P60_TB:
        return V4L2_DV_720P60;
    case V4L2_DV_720P50_FP:
    case V4L2_DV_720P50_SB_HALF:
    case V4L2_DV_720P50_TB:
        return V4L2_DV_720P50;
    case V4L2_DV_1080P60_SB_HALF:
    case V4L2_DV_1080P60_TB:
        return V4L2_DV_1080P60;
    case V4L2_DV_1080P30_FP:
    case V4L2_DV_1080P30_SB_HALF:
    case V4L2_DV_1080P30_TB:
        return V4L2_DV_1080P30;
    default:
        return HDMI_PRESET_ERROR;
    }
}

void ExynosExternalDisplay::setHdcpStatus(int status)
{
    exynos_hdmi_data hdmi_data;
    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_HDCP;
    hdmi_data.hdcp = !!status;
    if (ioctl(this->mDisplayFd, EXYNOS_SET_HDMI_CONFIG, &hdmi_data) < 0) {
        ALOGE("%s: failed to set HDCP status %d", __func__, errno);
    }
}

void ExynosExternalDisplay::setAudioChannel(uint32_t channels)
{
    exynos_hdmi_data hdmi_data;
    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_AUDIO;
    hdmi_data.audio_info = channels;
    if (ioctl(this->mDisplayFd, EXYNOS_SET_HDMI_CONFIG, &hdmi_data) < 0) {
        ALOGE("%s: failed to set audio channels %d", __func__, errno);
    }
}

uint32_t ExynosExternalDisplay::getAudioChannel()
{
    int channels = 0;

    exynos_hdmi_data hdmi_data;
    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_AUDIO;
    if (ioctl(this->mDisplayFd, EXYNOS_GET_HDMI_CONFIG, &hdmi_data) < 0) {
        ALOGE("%s: failed to get audio channels %d", __func__, errno);
    }
    channels = hdmi_data.audio_info;

    return channels;
}

int ExynosExternalDisplay::getCecPaddr()
{
    if (!mHwc->hdmi_hpd)
        return -1;

    exynos_hdmi_data hdmi_data;

    hdmi_data.state = hdmi_data.EXYNOS_HDMI_STATE_CEC_ADDR;
    if (ioctl(this->mDisplayFd, EXYNOS_GET_HDMI_CONFIG, &hdmi_data) < 0) {
        ALOGE("%s: g_dv_timings error, %d", __func__, errno);
        return -1;
    }

    return (int)hdmi_data.cec_addr;
}

int ExynosExternalDisplay::blank()
{
    int fence = clearDisplay();
    if (fence >= 0)
        close(fence);
    int err = ioctl(mDisplayFd, FBIOBLANK, FB_BLANK_POWERDOWN);
    if (err < 0) {
        if (errno == EBUSY)
            ALOGI("blank ioctl failed (display already blanked)");
        else
            ALOGE("blank ioctl failed: %s", strerror(errno));
        return -errno;
    }

    return 0;
}

int ExynosExternalDisplay::clearDisplay()
{
    if (!mEnabled)
        return 0;
    return ExynosDisplay::clearDisplay();
}

void ExynosExternalDisplay::requestIONMemory()
{
    if (mReserveMemFd > 0) {
        unsigned int value;
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        int err = lseek(mReserveMemFd, 0, SEEK_SET);
        err = read(mReserveMemFd, buffer, sizeof(buffer));
        value = atoi(buffer);

        if ((err > 0) && (value == 0)) {
            memset(buffer, 0, sizeof(buffer));
            buffer[0] = '2';
            if (write(mReserveMemFd, buffer, sizeof(buffer)) < 0)
                ALOGE("fail to request isolation of memmory for HDMI");
            else
                ALOGV("isolation of memmory for HDMI was requested");
        } else {
            if (err < 0)
                ALOGE("fail to read hdmi_reserve_mem_fd");
            else
                ALOGE("ion memmory for HDMI is isolated already");
        }
    }
}
void ExynosExternalDisplay::freeIONMemory()
{
    if ((mHwc->hdmi_hpd == false) && (mReserveMemFd > 0)) {
        unsigned int value;
        char buffer[4096];
        int ret = 0;
        memset(buffer, 0, sizeof(buffer));
        int err = lseek(mReserveMemFd, 0, SEEK_SET);
        err = read(mReserveMemFd, buffer, sizeof(buffer));
        value = atoi(buffer);
        if ((err > 0) && (value == 1)) {
            memset(buffer, 0, sizeof(buffer));
            buffer[0] = '0';
            if (write(mReserveMemFd, buffer, sizeof(buffer)) < 0)
                ALOGE("fail to request isolation of memmory for HDMI");
            else
                ALOGV("deisolation of memmory for HDMI was requested");
        } else {
            if (err < 0)
                ALOGE("fail to read hdmi_reserve_mem_fd");
            else
                ALOGE("ion memmory for HDMI is deisolated already");
        }
        mFlagIONBufferAllocated = false;
    }
}
bool ExynosExternalDisplay::checkIONBufferPrepared()
{
    if (mFlagIONBufferAllocated)
        return true;

    if ((mReserveMemFd > 0)) {
        unsigned int value;
        char buffer[4096];
        int ret = 0;
        memset(buffer, 0, sizeof(buffer));
        int err = lseek(mReserveMemFd, 0, SEEK_SET);
        err = read(mReserveMemFd, buffer, sizeof(buffer));
        value = atoi(buffer);

        if ((err > 0) && (value == 1)) {
            mFlagIONBufferAllocated = true;
            return true;
        } else {
            mFlagIONBufferAllocated = false;
            return false;
        }
        return false;
    } else {
        /* isolation of video_ext is not used */
        mFlagIONBufferAllocated = true;
        return true;
    }
}

int ExynosExternalDisplay::configureDRMSkipHandle(decon_win_config &cfg)
{
    int err = 0;
    private_handle_t *dst_handle = NULL;

    if (mDRMTempBuffer == NULL) {
        int dst_stride;
        int usage = GRALLOC_USAGE_SW_READ_NEVER |
            GRALLOC_USAGE_SW_WRITE_NEVER |
            GRALLOC_USAGE_HW_COMPOSER;

        err = mAllocDevice->alloc(mAllocDevice, 32, 32, HAL_PIXEL_FORMAT_BGRA_8888,
                usage, &mDRMTempBuffer, &dst_stride);
        if (err < 0) {
            ALOGE("failed to allocate destination buffer(%dx%d): %s", 32, 32,
                    strerror(-err));
            return err;
        } else {
            ALOGV("temBuffer for DRM video was allocated");
        }
    }

    dst_handle = private_handle_t::dynamicCast(mDRMTempBuffer);
    cfg.state = cfg.DECON_WIN_STATE_BUFFER;
    cfg.fd_idma[0] = dst_handle->fd;
    cfg.fd_idma[1] = dst_handle->fd1;
    cfg.fd_idma[2] = dst_handle->fd2;
    cfg.dst.f_w = dst_handle->stride;
    cfg.dst.f_h = dst_handle->vstride;
    cfg.dst.x = 0;
    cfg.dst.y = 0;
    cfg.dst.w = cfg.dst.f_w;
    cfg.dst.h = cfg.dst.f_h;
    cfg.format = halFormatToS3CFormat(HAL_PIXEL_FORMAT_RGBX_8888);

    cfg.src.f_w = dst_handle->stride;
    cfg.src.f_h = dst_handle->vstride;
    cfg.src.x = 0;
    cfg.src.y = 0;
    cfg.src.w = cfg.dst.f_w;
    cfg.src.h = cfg.dst.f_h;
    cfg.blending = DECON_BLENDING_NONE;
    cfg.fence_fd = -1;
    cfg.plane_alpha = 255;

    return 0;
}
