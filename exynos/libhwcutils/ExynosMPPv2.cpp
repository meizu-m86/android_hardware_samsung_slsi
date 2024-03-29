#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#include <utils/Trace.h>
#include "ExynosMPPv2.h"
#include "ExynosHWCUtils.h"
#ifdef USES_VIRTUAL_DISPLAY
#include "ExynosVirtualDisplay.h"
#endif
#include "ExynosHWCDebug.h"

int ExynosMPP::mainDisplayWidth = 0;
size_t visibleWidth(ExynosMPP *processor, hwc_layer_1_t &layer, int format,
        int xres)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    int bpp;
    if (processor->isProcessingRequired(layer, format) && format != HAL_PIXEL_FORMAT_RGB_565)
        bpp = 32;
    else
        bpp = formatToBpp(format);
    int left = max(layer.displayFrame.left, 0);
    int right = min(layer.displayFrame.right, xres);

    return (right - left) * bpp / 8;
}

void ExynosMPP::initMPP()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mMPPHandle = NULL;
    memset(&mSrcConfig, 0, sizeof(mSrcConfig));
    memset(&mMidConfig, 0, sizeof(mMidConfig));
    memset(&mDstConfig, 0, sizeof(mDstConfig));
    for (uint32_t i = 0; i < NUM_MPP_DST_BUFS; i++) {
        mDstBuffers[i] = NULL;
        mMidBuffers[i] = NULL;
        mDstBufFence[i] = -1;
        mMidBufFence[i] = -1;
    }
    mCurrentBuf = 0;
    mLastMPPLayerHandle= 0;
    mS3DMode = 0;
    mppFact = NULL;
    libmpp = NULL;
    mDoubleOperation = false;
    mCanRotate = (mType != MPP_VG);
    mCanBlend = (mType != MPP_VG);

    mAllocDevice = NULL;
    // mBufferFreeThread 初始化
    mBufferFreeThread = new BufferFreeThread(this);
    mBufferFreeThread->mRunning = true;
    mBufferFreeThread->run("MPPThread");

    mNumAvailableDstBuffers = NUM_MPP_DST_BUFS;
    mBufferType = MPP_BUFFER_NORMAL;
    mSMemFd = -1;
    if (mType == MPP_MSC) {
        mSMemFd = open(SMEM_PATH, O_RDWR);
        if (mSMemFd < 0)
            ALOGE("Fail to open smem_fd %s, error(%d)", SMEM_PATH, mSMemFd);
        else
            ALOGI("Open %s", SMEM_PATH);
    }

    mCanBeUsed = true;

    mAllocatedBufferNum = 0;
    mAllocatedMidBufferNum = 0;
    mMidRealloc = false;
    mDstRealloc = false;
}

ExynosMPP::ExynosMPP()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mDisplay = NULL;
    mType = 0;
    mIndex = 0;
    initMPP();
}

ExynosMPP::ExynosMPP(ExynosDisplay *display, int gscIndex)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mDisplay = display;
    mType = AVAILABLE_GSC_UNITS[gscIndex];
    mIndex = gscIndex;
    initMPP();
}

ExynosMPP::ExynosMPP(ExynosDisplay *display, unsigned int mppType, unsigned int mppIndex)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mDisplay = display;
    mType = mppType;
    mIndex = mppIndex;
    initMPP();
}

ExynosMPP::~ExynosMPP()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (mBufferFreeThread != NULL) {
        mBufferFreeThread->mRunning = false;
        mBufferFreeThread->requestExitAndWait();
        delete mBufferFreeThread;
    }
    if (mSMemFd >= 0)
        close(mSMemFd);
    mSMemFd = -1;
}
/** 多余的
const android::String8& ExynosMPP::getName() const
{
    return mName;
}
*/
bool ExynosMPP::isSrcConfigChanged(exynos_mpp_img &c1, exynos_mpp_img &c2)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return isDstConfigChanged(c1, c2) ||
            c1.fw != c2.fw ||
            c1.fh != c2.fh;
}

bool ExynosMPP::isFormatSupportedByMPP(int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (mType == MPP_VPP_G) {
        if (!isFormatRgb(format))
            return false;
    }

    switch (format) {
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M:
    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M:
    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV:
    case HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL:
    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED:
        return true;
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_EXYNOS_YV12_M:
    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P_M:
        if (mType == MPP_MSC)
            return true;
        else
            return false;

    default:
        return false;
    }
}

bool ExynosMPP::formatRequiresMPP(int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return isFormatSupportedByMPP(format) && !isFormatRgb(format);
}

int ExynosMPP::isProcessingSupported(hwc_layer_1_t &layer, int dst_format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);

    if ((mType == MPP_VG) || (mType == MPP_VGR))
        dst_format = INTERNAL_MPP_DST_FORMAT;
    int maxWidth = getMaxWidth(layer);
    int maxHeight = getMaxHeight(layer);
    int minWidth = getMinWidth(layer);
    int minHeight = getMinHeight(layer);
    int srcWidthAlign = getSrcWidthAlign(layer);
    int srcHeightAlign = getSrcHeightAlign(layer);
    int maxCropWidth = getMaxCropWidth(layer);
    int maxCropHeight = getMaxCropHeight(layer);
    int minCropWidth = getMinCropWidth(layer);
    int minCropHeight = getMinCropHeight(layer);
    int cropWidthAlign = getCropWidthAlign(layer);
    int cropHeightAlign = getCropHeightAlign(layer);
    int srcXOffsetAlign = getSrcXOffsetAlign(layer);
    int srcYOffsetAlign = getSrcYOffsetAlign(layer);
    int maxDstWidth = getMaxDstWidth(dst_format);
    int maxDstHeight = getMaxDstHeight(dst_format);
    int minDstWidth = getMinDstWidth(dst_format);
    int minDstHeight = getMinDstHeight(dst_format);
    int dstWidthAlign = getDstWidthAlign(dst_format);
    int dstHeightAlign = getDstHeightAlign(dst_format);
    ALOGE("[%s: %d]", __func__, __LINE__);
    int maxDownscale = getMaxDownscale(layer);
    if (((mType == MPP_VG) || (mType == MPP_VGR)) &&
        isRotated(layer) && isFormatRgb(handle->format))
        maxDownscale = 1;
    ALOGE("[%s: %d]", __func__, __LINE__);
    int maxUpscale = getMaxUpscale();
    ALOGE("[%s: %d]", __func__, __LINE__);
    bool isPerpendicular = !!(layer.transform & HAL_TRANSFORM_ROT_90);
    ALOGE("[%s: %d]", __func__, __LINE__);
    int srcW = WIDTH(layer.sourceCropf), srcH = HEIGHT(layer.sourceCropf);
    if (isFloat(layer.sourceCropf.right - layer.sourceCropf.left))
        srcW = ceilf(layer.sourceCropf.right - layer.sourceCropf.left);
    if (isFloat(layer.sourceCropf.bottom - layer.sourceCropf.top))
        srcH = ceilf(layer.sourceCropf.bottom - layer.sourceCropf.top);
    ALOGE("[%s: %d]", __func__, __LINE__);
    int xOffset = layer.sourceCropf.left;
    int yOffset = layer.sourceCropf.top;
    int dstW, dstH;
    if (isPerpendicular) {
        dstW = HEIGHT(layer.displayFrame);
        dstH = WIDTH(layer.displayFrame);
    } else {
        dstW = WIDTH(layer.displayFrame);
        dstH = HEIGHT(layer.displayFrame);
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (isFormatYUV420(handle->format)) {
        if ((mType == MPP_VG) || (mType == MPP_VGR)) {
            if (xOffset % srcXOffsetAlign != 0) {
                xOffset = ALIGN(xOffset, srcXOffsetAlign);
                srcW = (int)layer.sourceCropf.right - xOffset;
            }
            if (yOffset % srcYOffsetAlign != 0) {
                yOffset = ALIGN(yOffset, srcYOffsetAlign);
                srcH = (int)layer.sourceCropf.bottom - yOffset;
            }
        }
        if (srcW % cropWidthAlign != 0)
            srcW = ALIGN_DOWN(srcW, cropWidthAlign);
        if (srcH % cropHeightAlign != 0)
            srcH = ALIGN_DOWN(srcH, cropHeightAlign);
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (mType == MPP_MSC) {
        bool needDoubleOperation = false;
        if (getDrmMode(handle->flags) != NO_DRM) {
            needDoubleOperation = (dstW > srcW * maxUpscale) || (dstH > srcH * maxUpscale);
            if (needDoubleOperation)
                maxUpscale = maxUpscale * maxUpscale;

            needDoubleOperation = (srcW > dstW * maxDownscale) || (srcH > dstH * maxDownscale);
            if (needDoubleOperation)
                maxDownscale = maxDownscale * maxDownscale;
        }
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (!isFormatSupportedByMPP(handle->format))
        return -eMPPUnsupportedFormat;
    else if (!isFormatSupportedByMPP(dst_format))
        return -eMPPUnsupportedFormat;
    else if (!mCanBlend &&
              (handle->format == HAL_PIXEL_FORMAT_RGBA_8888 || handle->format == HAL_PIXEL_FORMAT_BGRA_8888) &&
              layer.blending != HWC_BLENDING_NONE)
        return -eMPPUnsupportedBlending;
    else if (!mCanRotate && layer.transform)
        return -eMPPUnsupportedRotation;
    else if (srcW < minWidth)
        return -eMPPExeedMinSrcWidth;
    else if (srcH < minHeight)
        return -eMPPExeedMinSrcHeight;
    else if (srcW < minCropWidth)
        return -eMPPExeedSrcWCropMin;
    else if (srcH < minCropHeight)
        return -eMPPExeedSrcHCropMin;
    else if (dstW > maxDstWidth)
        return -eMPPExeedMaxDstWidth;
    else if (dstW > maxDstHeight)
        return -eMPPExeedMaxDstHeight;
    else if (dstW < minDstWidth)
        return -eMPPExeedMinDstWidth;
    else if (dstW < minDstHeight)
        return -eMPPExeedMinDstWidth;
    else if ((dstW % dstWidthAlign != 0) || (dstH % dstHeightAlign != 0))
        return -eMPPNotAlignedDstSize;
    else if (srcW > dstW * maxDownscale)
        return -eMPPExeedMaxDownScale;
    else if (dstW > srcW * maxUpscale)
        return -eMPPExeedMaxUpScale;
    else if (srcH > dstH * maxDownscale)
        return -eMPPExeedMaxDownScale;
    else if (dstH > srcH * maxUpscale)
        return -eMPPExeedMaxUpScale;
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (getDrmMode(handle->flags) == NO_DRM) {
        if (handle->stride > maxWidth)
            return -eMPPExceedHStrideMaximum;
        else if (handle->vstride > maxHeight)
            return -eMPPExceedVStrideMaximum;
        else if (handle->stride % srcWidthAlign != 0)
            return -eMPPNotAlignedHStride;
        else if (handle->vstride % srcHeightAlign != 0)
            return -eMPPNotAlignedVStride;
        else if (srcW > maxCropWidth)
            return -eMPPExeedSrcWCropMax;
        else if (srcH > maxCropHeight)
            return -eMPPExeedSrcHCropMax;
        else if ((srcW % cropWidthAlign != 0) || (srcH % cropHeightAlign != 0))
            return -eMPPNotAlignedCrop;
        else if ((xOffset % srcXOffsetAlign != 0) || (yOffset % srcYOffsetAlign != 0))
            return -eMPPNotAlignedOffset;
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    return 1;
}

bool ExynosMPP::isProcessingRequired(hwc_layer_1_t &layer, int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return formatRequiresMPP(format) || isScaled(layer) || isTransformed(layer);
}

void ExynosMPP::adjustSourceImage(hwc_layer_1_t &layer, exynos_mpp_img &srcImg)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *srcHandle = private_handle_t::dynamicCast(layer.handle);
    hwc_frect_t tmpFCrop = layer.sourceCropf;
    int cropWidthAlign = getCropWidthAlign(layer);
    int cropHeightAlign = getCropHeightAlign(layer);
    int srcXOffsetAlign = getSrcXOffsetAlign(layer);
    int srcYOffsetAlign = getSrcYOffsetAlign(layer);

    float srcW = layer.sourceCropf.right - layer.sourceCropf.left;
    float srcH = layer.sourceCropf.bottom - layer.sourceCropf.top;

    srcImg.fw = srcHandle->stride;
    srcImg.fh = srcHandle->vstride;
    if (srcImg.fw > (unsigned int)getMaxWidth(layer))
        srcImg.fw = (unsigned int)getMaxWidth(layer);
    if (srcImg.fh > (unsigned int)getMaxHeight(layer))
        srcImg.fh = (unsigned int)getMaxHeight(layer);

    srcImg.fw = ALIGN((unsigned int)srcImg.fw, getSrcWidthAlign(layer));
    srcImg.fh = ALIGN((unsigned int)srcImg.fh, getSrcHeightAlign(layer));

    if (isFloat(tmpFCrop.left) &&
        (((int)tmpFCrop.left % srcXOffsetAlign) == 0)) {
        srcImg.x = setFloatValue(tmpFCrop.left);
    } else {
        if (ALIGN_DOWN((unsigned int)tmpFCrop.left, srcXOffsetAlign) > 0)
            tmpFCrop.left = ALIGN_DOWN((unsigned int)tmpFCrop.left, srcXOffsetAlign);
        else
            tmpFCrop.left = 0;
        srcImg.x = tmpFCrop.left;
    }

    if (isFloat(tmpFCrop.top) &&
        (((int)tmpFCrop.top % srcYOffsetAlign) == 0)) {
        srcImg.y = setFloatValue(tmpFCrop.top);
    } else {
        if (ALIGN_DOWN((unsigned int)tmpFCrop.top, srcYOffsetAlign) > 0)
            tmpFCrop.top = ALIGN_DOWN((unsigned int)tmpFCrop.top, srcYOffsetAlign);
        else
            tmpFCrop.top = 0;
        srcImg.y = tmpFCrop.top;
    }

    srcW = tmpFCrop.right - tmpFCrop.left;
    srcH = tmpFCrop.bottom - tmpFCrop.top;

    if (tmpFCrop.left + srcW > srcImg.fw)
        srcW = srcImg.fw - tmpFCrop.left;
    if (tmpFCrop.top + srcH > srcImg.fh)
        srcH = srcImg.fh - tmpFCrop.top;

    if (isFloat(srcW) && ((int)(ceilf(srcW)) % cropWidthAlign == 0)) {
        srcImg.w = setFloatValue(srcW);
    } else {
        if (ALIGN_DOWN((unsigned int)srcW, cropWidthAlign) > 0)
            srcImg.w = ALIGN_DOWN((unsigned int)srcW, cropWidthAlign);
        else
            srcImg.w = 0;
    }

    if (isFloat(srcH) && ((int)(ceilf(srcH)) % cropHeightAlign == 0)) {
        srcImg.h = setFloatValue(srcH);
    } else {
        if (ALIGN_DOWN((unsigned int)srcH, cropHeightAlign) > 0)
            srcImg.h = ALIGN_DOWN((unsigned int)srcH, cropHeightAlign);
        else
            srcImg.h = 0;
    }
}

void ExynosMPP::setupSrc(exynos_mpp_img &srcImg, hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *srcHandle = private_handle_t::dynamicCast(layer.handle);
    ALOGE("[%s: %d]", __func__, __LINE__);
    srcImg.fw = srcHandle->stride;
    srcImg.fh = srcHandle->vstride;
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (srcHandle->format == HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV) {
    ALOGE("[%s: %d]", __func__, __LINE__);
        if (srcHandle->fd2 >= 0) {
            void *metaData = NULL;
            int interlacedType = -1;
    ALOGE("[%s: %d]", __func__, __LINE__);
            metaData = mmap(0, 64, PROT_READ|PROT_WRITE, MAP_SHARED, srcHandle->fd2, 0);
            if (metaData)
                interlacedType = *(int *)metaData;
            else
                interlacedType = -1;

            if (interlacedType == V4L2_FIELD_INTERLACED_TB ||
                interlacedType == V4L2_FIELD_INTERLACED_BT) {
                srcImg.fw = srcHandle->stride * 2;
                srcImg.fh = srcHandle->vstride / 2;
    ALOGE("[%s: %d]", __func__, __LINE__);
            }
            if (metaData)
                munmap(metaData, 64);
        }
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (srcImg.fw > (unsigned int)getMaxWidth(layer))
        srcImg.fw = (unsigned int)getMaxWidth(layer);
    if (srcImg.fh > (unsigned int)getMaxHeight(layer))
        srcImg.fh = (unsigned int)getMaxHeight(layer);
    srcImg.fw = ALIGN((unsigned int)srcImg.fw, getSrcWidthAlign(layer));
    srcImg.fh = ALIGN((unsigned int)srcImg.fh, getSrcHeightAlign(layer));
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (isSrcCropFloat(layer.sourceCropf))
    {
        adjustSourceImage(layer, srcImg);
    } else {
        srcImg.x = ALIGN((unsigned int)layer.sourceCropf.left, getSrcXOffsetAlign(layer));
        srcImg.y = ALIGN((unsigned int)layer.sourceCropf.top, getSrcYOffsetAlign(layer));
        srcImg.w = WIDTH(layer.sourceCropf) - (srcImg.x - (uint32_t)layer.sourceCropf.left);
        srcImg.h = HEIGHT(layer.sourceCropf) - (srcImg.y - (uint32_t)layer.sourceCropf.top);
        if (srcImg.x + srcImg.w > srcImg.fw)
            srcImg.w = srcImg.fw - srcImg.x;
        if (srcImg.y + srcImg.h > srcImg.fh)
            srcImg.h = srcImg.fh - srcImg.y;
        srcImg.w = ALIGN_DOWN(srcImg.w, getCropWidthAlign(layer));
        srcImg.h = ALIGN_DOWN(srcImg.h, getCropHeightAlign(layer));
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    srcImg.yaddr = srcHandle->fd;
    if (mS3DMode == S3D_SBS)
        srcImg.w /= 2;
    if (mS3DMode == S3D_TB)
        srcImg.h /= 2;
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (isFormatYCrCb(srcHandle->format)) {
        srcImg.uaddr = srcHandle->fd2;
        srcImg.vaddr = srcHandle->fd1;
    } else {
        srcImg.uaddr = srcHandle->fd1;
        srcImg.vaddr = srcHandle->fd2;
    }
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (srcHandle->format != HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL)
        srcImg.format = srcHandle->format;
    else
        srcImg.format = HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M;

    ALOGE("[%s: %d]", __func__, __LINE__);
    if (layer.blending == HWC_BLENDING_COVERAGE)
        srcImg.pre_multi = false;
    else
        srcImg.pre_multi = true;
    srcImg.drmMode = !!(getDrmMode(srcHandle->flags) == SECURE_DRM);
    srcImg.acquireFenceFd = layer.acquireFenceFd;
    srcImg.mem_type = MPP_MEM_DMABUF;
}

bool ExynosMPP::setupDoubleOperation(exynos_mpp_img &srcImg, exynos_mpp_img &midImg, hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    bool needDoubleOperation = false;
    bool needUnscaledCSC = false;
    private_handle_t *srcHandle = private_handle_t::dynamicCast(layer.handle);
    const int maxUpscale = getMaxUpscale();
    const int maxDownscale = getMaxDownscale(layer);
    bool isPerpendicular = !!(layer.transform & HAL_TRANSFORM_ROT_90);
    int srcW = WIDTH(layer.sourceCropf), srcH = HEIGHT(layer.sourceCropf);
    int dstW, dstH;
    if (isPerpendicular) {
        dstW = HEIGHT(layer.displayFrame);
        dstH = WIDTH(layer.displayFrame);
    } else {
        dstW = WIDTH(layer.displayFrame);
        dstH = HEIGHT(layer.displayFrame);
    }
    if ((mType == MPP_MSC) && (getDrmMode(srcHandle->flags) != NO_DRM))
        needDoubleOperation = (dstW > srcW * maxUpscale) || (dstH > srcH * maxUpscale)
            || (srcW > dstW * maxDownscale) || (srcH > dstH * maxDownscale);

    if ((mType == MPP_MSC) && (srcHandle->format == HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED)) {
        needDoubleOperation = true;
        needUnscaledCSC = true;
    }

    if (needDoubleOperation) {
        midImg.x = 0;
        midImg.y = 0;

        int midW = 0, midH = 0;
        // Use YUV420 to save bandwidth
        // Format is either YUV420 or RGB, no conversion needed if already YUV420
        // Will RGB layers ever need double scaling? If so, is the saved BW worth the extra CSC?
        int midFormat = isFormatRgb(srcHandle->format) ? HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M : srcHandle->format;

        if (needUnscaledCSC) {
            midImg.w = srcW;
            midImg.h = srcH;
        } else {
            if (isPerpendicular) {
                midW = HEIGHT(layer.displayFrame);
                midH = WIDTH(layer.displayFrame);
            } else {
                midW = WIDTH(layer.displayFrame);
                midH = HEIGHT(layer.displayFrame);
            }

            if (WIDTH(layer.sourceCropf) * maxUpscale  < midW)
                midImg.w = ALIGN(midW / maxUpscale, getDstWidthAlign(midFormat));
            else if (WIDTH(layer.sourceCropf) > midW * maxDownscale)
                midImg.w = ALIGN((int)ceilf((double)WIDTH(layer.sourceCropf) / (double)maxDownscale), getDstWidthAlign(midFormat));
            else
                midImg.w = midW;

            if (HEIGHT(layer.sourceCropf) * maxUpscale < midH)
                midImg.h = ALIGN(midH / maxUpscale, getDstHeightAlign(midFormat));
            else if (HEIGHT(layer.sourceCropf) > midH * maxDownscale)
                midImg.h = ALIGN((int)ceilf((double)HEIGHT(layer.sourceCropf) / (double)maxDownscale), getDstHeightAlign(midFormat));
            else
                midImg.h = midH;
        }
        if (layer.blending == HWC_BLENDING_COVERAGE)
            midImg.pre_multi = false;
        else
            midImg.pre_multi = true;
        midImg.drmMode = srcImg.drmMode;
        midImg.format = midFormat;
        midImg.mem_type = MPP_MEM_DMABUF;
        midImg.narrowRgb = midFormat != HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL;
    }

    return needDoubleOperation;
}

void ExynosMPP::setupMid(exynos_mpp_img &midImg)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    buffer_handle_t &midBuf = mMidBuffers[mCurrentBuf];
    private_handle_t *midHandle = private_handle_t::dynamicCast(midBuf);

    midImg.acquireFenceFd = mMidBufFence[mCurrentBuf];
    mMidBufFence[mCurrentBuf] = -1;
    midImg.fw = midHandle->stride;
    midImg.fh = midHandle->vstride;
    midImg.yaddr = midHandle->fd;
    if (isFormatYCrCb(midHandle->format)) {
        midImg.uaddr = midHandle->fd2;
        midImg.vaddr = midHandle->fd1;
    } else {
        midImg.uaddr = midHandle->fd1;
        midImg.vaddr = midHandle->fd2;
    }
}

void ExynosMPP::setupDst(exynos_mpp_img &srcImg, exynos_mpp_img &dstImg,
        int dstFormat, hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *srcHandle = private_handle_t::dynamicCast(layer.handle);
    dstImg.x = 0;
    dstImg.y = 0;
    dstImg.w = WIDTH(layer.displayFrame);
    dstImg.h = HEIGHT(layer.displayFrame);
    dstImg.rot = layer.transform;
    if (layer.blending == HWC_BLENDING_COVERAGE)
        dstImg.pre_multi = false;
    else
        dstImg.pre_multi = true;
    dstImg.drmMode = srcImg.drmMode;
    dstImg.format = dstFormat;
    if (dstFormat == HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV)
        dstImg.format = HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M;
    dstImg.mem_type = MPP_MEM_DMABUF;
    if (srcHandle->format == HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL)
        dstImg.narrowRgb = 0;
    else
        dstImg.narrowRgb = !isFormatRgb(srcHandle->format);
}

size_t ExynosMPP::getBufferType(uint32_t usage)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    if ((getDrmMode(usage) == SECURE_DRM) && (usage & GRALLOC_USAGE_VIDEO_EXT))
        return MPP_BUFFER_VIDEO_EXT;
    else if (getDrmMode(usage) == SECURE_DRM)
        return MPP_BUFFER_SECURE_DRM;
    else if (getDrmMode(usage) == NORMAL_DRM)
        return MPP_BUFFER_NORMAL_DRM;
    else
        return MPP_BUFFER_NORMAL;
}

int ExynosMPP::getBufferUsage(private_handle_t *srcHandle)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    int usage = GRALLOC_USAGE_SW_READ_NEVER |
            GRALLOC_USAGE_SW_WRITE_NEVER |
            GRALLOC_USAGE_NOZEROED |
            GRALLOC_USAGE_HW_COMPOSER;

    if (getDrmMode(srcHandle->flags) == SECURE_DRM) {
        usage |= GRALLOC_USAGE_PROTECTED;
        usage &= ~GRALLOC_USAGE_PRIVATE_NONSECURE;
    } else if (getDrmMode(srcHandle->flags) == NORMAL_DRM) {
        usage |= GRALLOC_USAGE_PROTECTED;
        usage |= GRALLOC_USAGE_PRIVATE_NONSECURE;
    }
    if (srcHandle->flags & GRALLOC_USAGE_VIDEO_EXT)
        usage |= GRALLOC_USAGE_VIDEO_EXT;

    /* HACK: for distinguishing FIMD_VIDEO_region */
    usage |= (GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_HW_RENDER);

    return usage;
}

int ExynosMPP::reallocateBuffers(private_handle_t *srcHandle, exynos_mpp_img &dstImg, exynos_mpp_img &midImg, bool needDoubleOperation, uint32_t index)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    alloc_device_t* allocDevice = mAllocDevice;
    int ret = 0;
    int dstStride;
    if (mDisplay == NULL) {
        ALOGE("%s is called without assigning to the display", __func__);
        return -1;
    }

    int usage = getBufferUsage(srcHandle);
    size_t bufferType = getBufferType(usage);

    int w, h;
    {
        int dstWidthAlign = getDstWidthAlign(dstImg.format);
        int dstHeightAlign = getDstHeightAlign(dstImg.format);
        w = ALIGN(mDisplay->mXres, dstWidthAlign);
        h = ALIGN(mDisplay->mYres, dstHeightAlign);
    }

    if ((mBufferType == MPP_BUFFER_VIDEO_EXT) &&
        (bufferType != MPP_BUFFER_VIDEO_EXT) && (mSMemFd > 0)) {
        int ret = 0;
        int protection = 0;
        ret = ioctl(mSMemFd, SECMEM_IOC_SET_VIDEO_EXT_PROC, &protection);
        if (ret < 0) {
            ALOGE("%s:: Unprotection failed, ret(%d)", __func__, ret);
            return false;
        }
        ALOGI("%s:: VIDEO_EXT is unprotected", __func__);
    }

    if (mAllocatedBufferNum < mNumAvailableDstBuffers) {
        if (mDstBuffers[index]) {
            android::Mutex::Autolock lock(mMutex);
            if (mBufferType != MPP_BUFFER_NORMAL) {
                allocDevice->free(allocDevice, mDstBuffers[index]);
            } else {
                deleteBufferInfo buffInfo;
                buffInfo.buffer = mDstBuffers[index];
                buffInfo.bufFence = mDstBufFence[index];
                mFreedBuffers.push_back(buffInfo);
                mDstBufFence[index] = -1;
            }
            mDstBuffers[index] = NULL;
        }
        if (mDstBufFence[index] >= 0)
            close(mDstBufFence[index]);
        mDstBufFence[index] = -1;
    }

    if (needDoubleOperation && (mAllocatedMidBufferNum < mNumAvailableDstBuffers)) {
        if (mMidBuffers[index] != NULL) {
            android::Mutex::Autolock lock(mMutex);
            if (mBufferType != MPP_BUFFER_NORMAL) {
                allocDevice->free(allocDevice, mMidBuffers[index]);
            } else {
                deleteBufferInfo buffInfo;
                buffInfo.buffer = mMidBuffers[index];
                buffInfo.bufFence = mMidBufFence[index];
                mFreedBuffers.push_back(buffInfo);
                mMidBufFence[index] = -1;
            }
            mMidBuffers[index] = NULL;
        }
        if (mMidBufFence[index] >= 0)
            close(mMidBufFence[index]);
        mMidBufFence[index] = -1;
    }

    int format = dstImg.format;
    if (mAllocatedBufferNum < mNumAvailableDstBuffers) {
        ret = allocDevice->alloc(allocDevice, w, h,
                format, usage, &mDstBuffers[index],
                &dstStride);
        if (ret < 0) {
            ALOGE("failed to allocate destination buffer(%dx%d): %s", w, h,
                    strerror(-ret));
            return ret;
        }
        HDEBUGLOGD(eDebugMPP, "%d dst buffer is allocated, mAllocatedBufferNum(%d)", mCurrentBuf, mAllocatedBufferNum);
        mAllocatedBufferNum++;
    }

    if (needDoubleOperation && (mAllocatedMidBufferNum < mNumAvailableDstBuffers)) {
        ret = allocDevice->alloc(allocDevice, midImg.w, midImg.h, midImg.format,
                usage, &mMidBuffers[index], &dstStride);
        if (ret < 0) {
            ALOGE("failed to allocate intermediate buffer(%dx%d): %s",
                    midImg.w, midImg.h, strerror(-ret));
            return ret;
        }
        HDEBUGLOGD(eDebugMPP, "%d mid buffer is allocated, mAllocatedBufferNum(%d)", mCurrentBuf, mAllocatedMidBufferNum);
        mAllocatedMidBufferNum++;
    }

    if ((mBufferType != MPP_BUFFER_VIDEO_EXT) &&
        (bufferType == MPP_BUFFER_VIDEO_EXT) && (mSMemFd > 0)) {
        int ret = 0;
        int protection = 1;
        ret = ioctl(mSMemFd, SECMEM_IOC_SET_VIDEO_EXT_PROC, &protection);
        if (ret < 0) {
            ALOGE("Protection failed, ret(%d)", ret);
            return false;
        }
        ALOGI("%s:: VIDEO_EXT is protected", __func__);
    }


    {
        android::Mutex::Autolock lock(mMutex);
        mBufferFreeThread->mCondition.signal();
    }

    return ret;
}

void ExynosMPP::reusePreviousFrame(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    HDEBUGLOGD(eDebugMPP, "[USE] MPP_SKIP_DUPLICATE_FRAME_PROCESSING\n");
    if (layer.acquireFenceFd >= 0)
        close(layer.acquireFenceFd);

    layer.releaseFenceFd = -1;
    layer.acquireFenceFd = -1;
    mDstConfig.releaseFenceFd = -1;

    mCurrentBuf = (mCurrentBuf + mNumAvailableDstBuffers - 1) % mNumAvailableDstBuffers;
}

void ExynosMPP::freeBuffersCloseFences()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    alloc_device_t* allocDevice = mAllocDevice;
    if ((mBufferType == MPP_BUFFER_VIDEO_EXT) && (mSMemFd > 0)) {
        int ret = 0;
        int protection = 0;
        ret = ioctl(mSMemFd, SECMEM_IOC_SET_VIDEO_EXT_PROC, &protection);
        if (ret < 0) {
            ALOGE("%s:: Deprotection failed, ret(%d)", __func__, ret);
        }
        ALOGI("%s:: VIDEO_EXT is unprotected", __func__);
    }

    for (size_t i = 0; i < NUM_MPP_DST_BUFS; i++) {
        if (mDstBuffers[i]) {
            android::Mutex::Autolock lock(mMutex);
            if (mBufferType != MPP_BUFFER_NORMAL)
                allocDevice->free(allocDevice, mDstBuffers[i]);
            else {
                deleteBufferInfo buffInfo;
                buffInfo.buffer = mDstBuffers[i];
                buffInfo.bufFence = mDstBufFence[i];
                mFreedBuffers.push_back(buffInfo);
                mDstBufFence[i] = -1;
            }
            mDstBuffers[i] = NULL;
        }
        if (mDstBufFence[i] >= 0)
            close(mDstBufFence[i]);
        mDstBufFence[i] = -1;

        if (mMidBuffers[i]) {
            android::Mutex::Autolock lock(mMutex);
            if (mBufferType != MPP_BUFFER_NORMAL)
                allocDevice->free(allocDevice, mMidBuffers[i]);
            else {
                deleteBufferInfo buffInfo;
                buffInfo.buffer = mMidBuffers[i];
                buffInfo.bufFence = mMidBufFence[i];
                mFreedBuffers.push_back(buffInfo);
                mMidBufFence[i] = -1;
            }
            mMidBuffers[i] = NULL;
        }
        if (mMidBufFence[i] >= 0)
            close(mMidBufFence[i]);
        mMidBufFence[i] = -1;
    }

    {
        android::Mutex::Autolock lock(mMutex);
        mBufferFreeThread->mCondition.signal();
    }
}

int ExynosMPP::processM2M(hwc_layer_1_t &layer, int dstFormat, hwc_frect_t *sourceCrop, bool __unused needBufferAlloc)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    HDEBUGLOGD(eDebugMPP, "configuring mType(%u) mIndex(%u) for memory-to-memory", mType, mIndex);

#ifdef USES_VIRTUAL_DISPLAY
    alloc_device_t* allocDevice = mAllocDevice;
#endif

    private_handle_t *srcHandle = private_handle_t::dynamicCast(layer.handle);
    buffer_handle_t dstBuf;
    private_handle_t *dstHandle;
    int ret = 0;

    exynos_mpp_img srcImg, dstImg;
    memset(&srcImg, 0, sizeof(srcImg));
    memset(&dstImg, 0, sizeof(dstImg));
    exynos_mpp_img midImg;
    memset(&midImg, 0, sizeof(midImg));

    ALOGE("[%s: %d]", __func__, __LINE__);
    setupSrc(srcImg, layer);
    ALOGE("[%s: %d]", __func__, __LINE__);

    HDEBUGLOGD(eDebugMPP, "source configuration:");
    dumpMPPImage(eDebugMPP, srcImg);

    setupDst(srcImg, dstImg, dstFormat, layer);
    HDEBUGLOGD(eDebugMPP, "destination configuration:");
    dumpMPPImage(eDebugMPP, dstImg);

    /* mMPPHandle should be created again with proper bufferType */
    if ((mMPPHandle != NULL) && (mBufferType != getBufferType(srcHandle->flags))) {
        stopMPP(mMPPHandle);
        destroyMPP(mMPPHandle);
        mMPPHandle = NULL;
    }

    bool needDoubleOperation = setupDoubleOperation(srcImg, midImg, layer);

    bool reconfigure = isSrcConfigChanged(srcImg, mSrcConfig) ||
            isDstConfigChanged(dstImg, mDstConfig);

    mDstRealloc = mDstConfig.fw <= 0 ||
        mDstConfig.format != (uint32_t)dstFormat ||
        dstImg.drmMode != mDstConfig.drmMode ||
        mBufferType != getBufferType(srcHandle->flags);

    mMidRealloc = needDoubleOperation &&
        (needDoubleOperation != mDoubleOperation ||
         dstImg.drmMode != mDstConfig.drmMode ||
         mBufferType != getBufferType(srcHandle->flags) ||
         ((dstImg.w != mDstConfig.w) || (dstImg.h != mDstConfig.h)));

#ifdef USES_VIRTUAL_DISPLAY
    if (!needBufferAlloc) {
        dstImg.x = mDisplay->mHwc->mVirtualDisplayRect.left;
        dstImg.y = mDisplay->mHwc->mVirtualDisplayRect.top;
        dstImg.w = mDisplay->mHwc->mVirtualDisplayRect.width;
        dstImg.h = mDisplay->mHwc->mVirtualDisplayRect.height;
        /* WFD can use MSC for blending or scaling. recofigure is required on WFD */
        reconfigure = true;
    }
#endif

#ifdef USES_VIRTUAL_DISPLAY
    if (needBufferAlloc) {
#endif
        if (reconfigure && (mMidRealloc || mDstRealloc)) {
            if (getDrmMode(srcHandle->flags) != NO_DRM &&
                mDisplay != NULL && mDisplay->mType == EXYNOS_EXTERNAL_DISPLAY)
                mNumAvailableDstBuffers = NUM_DRM_MPP_DST_BUFS;
            else
                mNumAvailableDstBuffers = NUM_MPP_DST_BUFS;
            mBufferType = getBufferType(srcHandle->flags);

            if (mDstRealloc) {
                mCurrentBuf = 0;
                mLastMPPLayerHandle = 0;
                mAllocatedBufferNum = 0;
            }
            if (mMidRealloc)
                mAllocatedMidBufferNum = 0;
        }

        if (!reconfigure && (mLastMPPLayerHandle == (intptr_t)layer.handle)) {
            reusePreviousFrame(layer);
            return 0;
        } else {
            mLastMPPLayerHandle = (intptr_t)layer.handle;
        }

        if ((ret = reallocateBuffers(srcHandle, dstImg, midImg, needDoubleOperation, mCurrentBuf)) < 0)
            goto err_alloc;

#ifdef USES_VIRTUAL_DISPLAY
    } else {
        if (reconfigure && needDoubleOperation) {
            int dstStride;
            if (mMidBuffers[0] != NULL) {
                android::Mutex::Autolock lock(mMutex);
                deleteBufferInfo buffInfo;
                buffInfo.buffer = mMidBuffers[0];
                buffInfo.bufFence = mMidBufFence[0];
                mFreedBuffers.push_back(buffInfo);
                mBufferFreeThread->mCondition.signal();
                mMidBuffers[0] = NULL;
                mMidBufFence[0] = -1;
            }
            if (mMidBufFence[0] >= 0) {
                close(mMidBufFence[0]);
            }
            mMidBufFence[0] = -1;

            ret = allocDevice->alloc(allocDevice, midImg.w, midImg.h,
                    HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M, ((ExynosVirtualDisplay*)mDisplay)->mSinkUsage, &mMidBuffers[0],
                    &dstStride);
            if (ret < 0) {
                ALOGE("failed to allocate intermediate buffer(%dx%d): %s", midImg.w, midImg.h,
                        strerror(-ret));
                goto err_alloc;
            }
        }
    }
#endif

    layer.acquireFenceFd = -1;
    if (needDoubleOperation) {
        setupMid(midImg);

        HDEBUGLOGD(eDebugMPP, "mid configuration:");
        dumpMPPImage(eDebugMPP, midImg);
    }

    dstBuf = mDstBuffers[mCurrentBuf];
    dstHandle = private_handle_t::dynamicCast(dstBuf);

    dstImg.fw = dstHandle->stride;
    dstImg.fh = dstHandle->vstride;
    dstImg.yaddr = dstHandle->fd;
    dstImg.uaddr = dstHandle->fd1;
    dstImg.vaddr = dstHandle->fd2;
    dstImg.acquireFenceFd = mDstBufFence[mCurrentBuf];
    dstImg.drmMode = !!(getDrmMode(dstHandle->flags) == SECURE_DRM);
    mDstBufFence[mCurrentBuf] = -1;

    if ((int)dstImg.w != WIDTH(layer.displayFrame))
        HDEBUGLOGD(eDebugMPP, "padding %u x %u output to %u x %u and cropping to {%7.1f,%7.1f,%7.1f,%7.1f}",
                WIDTH(layer.displayFrame), HEIGHT(layer.displayFrame),
                dstImg.w, dstImg.h, sourceCrop->left, sourceCrop->top,
                sourceCrop->right, sourceCrop->bottom);

    if (mMPPHandle) {
        HDEBUGLOGD(eDebugMPP, "reusing open mType(%u) mIndex(%u)", mType, mIndex);
    } else {
        HDEBUGLOGD(eDebugMPP, "opening mType(%u) mIndex(%u), drm(%d)", mType, mIndex, (getDrmMode(srcHandle->flags) != NO_DRM));
        mMPPHandle = createMPP(
                mType, GSC_M2M_MODE, GSC_DUMMY, (getDrmMode(srcHandle->flags) != NO_DRM) || (getDrmMode(dstHandle->flags) != NO_DRM));
        if (!mMPPHandle) {
            ALOGE("failed to create gscaler handle");
            ret = -1;
            goto err_alloc;
        }
    }

    if (!needDoubleOperation)
        memcpy(&midImg, &dstImg, sizeof(exynos_mpp_img));

    /* src -> mid or src->dest */
    if (reconfigure || needDoubleOperation) {
        ret = stopMPP(mMPPHandle);
        if (ret < 0) {
            ALOGE("failed to stop mType(%u) mIndex(%u)", mType, mIndex);
            goto err_gsc_config;
        }
        ret = setCSCProperty(mMPPHandle, 0, !midImg.narrowRgb, 1);
        ret = configMPP(mMPPHandle, &srcImg, &midImg);
        if (ret < 0) {
            ALOGE("failed to configure mType(%u) mIndex(%u)", mType, mIndex);
            goto err_gsc_config;
        }
    }

    ret = runMPP(mMPPHandle, &srcImg, &midImg);
    if (ret < 0) {
        ALOGE("failed to run mType(%u) mIndex(%d)", mType, mIndex);
        goto err_gsc_config;
    }

    /* mid -> dst */
    if (needDoubleOperation) {
        ret = stopMPP(mMPPHandle);
        if (ret < 0) {
            ALOGE("failed to stop mType(%d) mIndex(%u)", mType, mIndex);
            goto err_gsc_config;
        }

        midImg.acquireFenceFd = midImg.releaseFenceFd;
        ret = setCSCProperty(mMPPHandle, 0, !dstImg.narrowRgb, 1);
        ret = configMPP(mMPPHandle, &midImg, &dstImg);
        if (ret < 0) {
            ALOGE("failed to configure mType(%u) mIndex(%u)", mType, mIndex);
            goto err_gsc_config;
        }

        ret = runMPP(mMPPHandle, &midImg, &dstImg);
        if (ret < 0) {
            ALOGE("failed to run mType(%u) mIndex(%u)", mType, mIndex);
            goto err_gsc_config;
        }
        mMidBufFence[mCurrentBuf] = midImg.releaseFenceFd;
    }

    mSrcConfig = srcImg;
    mMidConfig = midImg;

    if (needDoubleOperation) {
        mDstConfig = dstImg;
    } else {
        mDstConfig = midImg;
    }

    mDoubleOperation = needDoubleOperation;

    layer.releaseFenceFd = srcImg.releaseFenceFd;

    return 0;

err_gsc_config:
    destroyMPP(mMPPHandle);
    mMPPHandle = NULL;
err_alloc:
    if (srcImg.acquireFenceFd >= 0)
        close(srcImg.acquireFenceFd);
#ifdef USES_VIRTUAL_DISPLAY
    if (needBufferAlloc) {
#endif
        freeBuffersCloseFences();
#ifdef USES_VIRTUAL_DISPLAY
    } else {
        if (mMidBuffers[0]) {
            android::Mutex::Autolock lock(mMutex);
            deleteBufferInfo buffInfo;
            buffInfo.buffer = mMidBuffers[0];
            buffInfo.bufFence = mMidBufFence[0];
            mFreedBuffers.push_back(buffInfo);
            mMidBuffers[0] = NULL;
            mMidBufFence[0] = -1;
        }
        if (mMidBufFence[0] >= 0) {
            close(mMidBufFence[0]);
        }
        mMidBufFence[0] = -1;
    }
#endif

    {
        android::Mutex::Autolock lock(mMutex);
        mBufferFreeThread->mCondition.signal();
    }

    memset(&mSrcConfig, 0, sizeof(mSrcConfig));
    memset(&mDstConfig, 0, sizeof(mDstConfig));
    memset(&mMidConfig, 0, sizeof(mMidConfig));
    mBufferType = MPP_BUFFER_NORMAL;
    mAllocatedBufferNum = 0;
    mAllocatedMidBufferNum = 0;
    return ret;
}

int ExynosMPP::setupInternalMPP()
{
    char s[32];
    ALOGE("[%s: %d]", __func__, __LINE__);
    if(mType != MPP_VGR){
        return -1;
    }
    if(mSubdevFd == NULL){
        snprintf(s, 32, "/dev/v4l-subdev%d", mType + mIndex);
        mSubdevFd = exynos_subdev_open(s, 2);
    }
    return 0;
}

void ExynosMPP::cleanupM2M()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    cleanupM2M(false);
}

void ExynosMPP::cleanupM2M(bool noFenceWait)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    if (!mMPPHandle)
        return;

    if (noFenceWait) {
        for (size_t i = 0; i < NUM_MPP_DST_BUFS; i++) {
            if (mDstBufFence[i] >= 0) {
                close(mDstBufFence[i]);
                mDstBufFence[i] = -1;
            }
            if (mMidBufFence[i] >= 0) {
                close(mMidBufFence[i]);
                mMidBufFence[i] = -1;
            }
        }
    }

    stopMPP(mMPPHandle);
    destroyMPP(mMPPHandle);
    freeBuffersCloseFences();

    mMPPHandle = NULL;
    memset(&mSrcConfig, 0, sizeof(mSrcConfig));
    memset(&mMidConfig, 0, sizeof(mMidConfig));
    memset(&mDstConfig, 0, sizeof(mDstConfig));
    memset(mDstBuffers, 0, sizeof(mDstBuffers));
    memset(mMidBuffers, 0, sizeof(mMidBuffers));
    mCurrentBuf = 0;
    mLastMPPLayerHandle = 0;

    for (size_t i = 0; i < NUM_MPP_DST_BUFS; i++) {
        mDstBufFence[i] = -1;
        mMidBufFence[i] = -1;
    }
    mState = MPP_STATE_FREE;
    mDisplay = NULL;
    mBufferType = MPP_BUFFER_NORMAL;
    mAllocatedBufferNum = 0;
    mAllocatedMidBufferNum = 0;
    mMidRealloc = 0;
    mDstRealloc = 0;
}

void ExynosMPP::cleanupInternalMPP()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    if (mType != MPP_VG && mType != MPP_VGR && mType != MPP_VPP_G)
        return;

    mDisplay = NULL;
    startTransition(mDisplay);
}

bool ExynosMPP::isDstConfigChanged(exynos_mpp_img &c1, exynos_mpp_img &c2)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return c1.x != c2.x ||
            c1.y != c2.y ||
            c1.w != c2.w ||
            c1.h != c2.h ||
            c1.format != c2.format ||
            c1.rot != c2.rot ||
            c1.narrowRgb != c2.narrowRgb ||
            c1.cacheable != c2.cacheable ||
            c1.pre_multi != c2.pre_multi ||
            c1.drmMode != c2.drmMode;
}

int ExynosMPP::getMaxWidth(hwc_layer_1_t __unused &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return 8190;
    case MPP_MSC:
    default:
        return 8192;
    }
}

int ExynosMPP::getMaxHeight(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return isRotated(layer) ? 8190 : 4096;
    case MPP_MSC:
    default:
        return 8192;
    }
}

int ExynosMPP::getMinWidth(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format)) {
            if (formatToBpp(handle->format))
                return 32;
            else
                return isRotated(layer) ? 32 : 64;
        } else if (isFormatYUV420(handle->format)) {
            return 64;
        } else {
            return 64;
        }
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getMinHeight(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format))
            return 16;
        else if (isFormatYUV420(handle->format))
            return 32;
        else return 32;
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getSrcWidthAlign(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format)) {
            if (formatToBpp(handle->format) == 32)
                return 1;
            else
                return isRotated(layer) ? 2 : 1;
        } else if (isFormatYUV420(handle->format)) {
            return isRotated(layer) ? 4 : 2;
        } else {
            return 4;
        }
    case MPP_MSC:
    default:
        if (isFormatRgb(handle->format))
            return 1;
        else if (isFormatYUV420(handle->format))
            return 2;
        else
            return 2;
    }
}

int ExynosMPP::getSrcHeightAlign(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format)) {
            if (formatToBpp(handle->format) == 32)
                return 1;
            else
                return isRotated(layer) ? 2 : 1;
        } else if (isFormatYUV420(handle->format)) {
            return isRotated(layer) ? 4 : 2;
        } else {
            return 4;
        }
    case MPP_MSC:
    default:
        if (isFormatRgb(handle->format))
            return 1;
        else if (isFormatYUV420(handle->format))
            return 2;
        else
            return 2;
    }
}

int ExynosMPP::getMaxCropWidth(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if ((layer.transform == HAL_TRANSFORM_ROT_180) || (layer.transform == HAL_TRANSFORM_FLIP_H) ||
            (layer.transform == HAL_TRANSFORM_FLIP_V))
            return 2048;
        return isRotated(layer) ? 2560 : 4096;
    case MPP_MSC:
    default:
        return 8192;
    }
}

int ExynosMPP::getMaxCropHeight(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return isRotated(layer) ? 2048 : 4096;
    case MPP_MSC:
    default:
        return 8192;
    }
}

int ExynosMPP::getMinCropWidth(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format))
            return isRotated(layer) ? 16 : 32;
        else if (isFormatYUV420(handle->format))
            return isRotated(layer) ? 32 : 64;
        else
            return 64;
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getMinCropHeight(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format))
            return isRotated(layer) ? 32 : 16;
        else if (isFormatYUV420(handle->format))
            return isRotated(layer) ? 64 : 32;
        else
            return 64;
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getCropWidthAlign(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
    case MPP_MSC:
    default:
        if (isFormatRgb(handle->format))
            return 1;
        else if (isFormatYUV420(handle->format))
            return 2;
        else
            return 2;
    }
}

int ExynosMPP::getCropHeightAlign(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
    case MPP_MSC:
    default:
        if (isFormatRgb(handle->format))
            return 1;
        else if (isFormatYUV420(handle->format))
            return 2;
        else
            return 2;
    }
}

int ExynosMPP::getSrcXOffsetAlign(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format))
            return isRotated(layer) ? 2 : 1;
        else if (isFormatYUV420(handle->format))
            return isRotated(layer) ? 4 : 2;
        else
            return 4;
    case MPP_MSC:
    default:
        if (isFormatRgb(handle->format))
            return 1;
        else if (isFormatYUV420(handle->format))
            return 1;
        else
            return 2;
    }
}

int ExynosMPP::getSrcYOffsetAlign(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(handle->format))
            return isRotated(layer) ? 2 : 1;
        else if (isFormatYUV420(handle->format))
            return isRotated(layer) ? 4 : 2;
        else
            return 4;
    case MPP_MSC:
    default:
        if (isFormatRgb(handle->format))
            return 1;
        else if (isFormatYUV420(handle->format))
            return 1;
        else
            return 2;
    }
}

int ExynosMPP::getMaxDstWidth(int __unused format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return 4096;
    case MPP_MSC:
    default:
        return 8192;
    }
}

int ExynosMPP::getMaxDstHeight(int __unused format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return 4096;
    case MPP_MSC:
    default:
        return 8192;
    }
}

int ExynosMPP::getMinDstWidth(int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(format))
            return 16;
        else if (isFormatYUV420(format))
            return 32;
        else
            return 32;
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getMinDstHeight(int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if (isFormatRgb(format))
            return 8;
        else if (isFormatYUV420(format))
            return 16;
        else
            return 16;
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getDstWidthAlign(int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return 1;
    case MPP_MSC:
    default:
        if (isFormatRgb(format))
            return 1;
        else if (isFormatYUV420(format))
            return 2;
        else
            return 2;
    }
}

int ExynosMPP::getDstHeightAlign(int format)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return 1;
    case MPP_MSC:
    default:
        if (isFormatRgb(format))
            return 1;
        else if (isFormatYUV420(format))
            return 2;
        else
            return 2;
    }
}

int ExynosMPP::getMaxDownscale()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        return 2;
    case MPP_MSC:
    default:
        return 16;
    }
}

int ExynosMPP::getMaxDownscale(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    bool isPerpendicular = !!(layer.transform & HAL_TRANSFORM_ROT_90);
    float scaleRatio_H = (layer.sourceCropf.right - layer.sourceCropf.left)/WIDTH(layer.displayFrame);
    float scaleRatio_V = (layer.sourceCropf.bottom - layer.sourceCropf.top)/HEIGHT(layer.displayFrame);
    float dstW = (float)WIDTH(layer.displayFrame);
    float displayW = (float)ExynosMPP::mainDisplayWidth;
    if (isPerpendicular)
        dstW = (float)HEIGHT(layer.displayFrame);

    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
        if ((float)VPP_CLOCK < (VPP_VCLK * VPP_MIC_FACTOR * scaleRatio_H * scaleRatio_V)/2 * (dstW/displayW))
            return 1;
        return 2;
    case MPP_MSC:
        if (handle && (getDrmMode(handle->flags) != NO_DRM))
            return 4;
        else
            return 16;
    default:
        return 16;
    }
}

int ExynosMPP::getMaxUpscale()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    switch (mType) {
    case MPP_VG:
    case MPP_VGR:
    case MPP_MSC:
    default:
        return 8;
    }
}

bool ExynosMPP::inUse()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return mMPPHandle != NULL;
}

void ExynosMPP::setDisplay(ExynosDisplay *display)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mDisplay = display;
}
/** 多余的
void ExynosMPP::preAssignDisplay(ExynosDisplay *display)
{
    mPreAssignedDisplay = display;
}

bool ExynosMPP::isAssignable(ExynosDisplay *display)
{
    return (mPreAssignedDisplay == display || mPreAssignedDisplay == NULL);
}
*/
void ExynosMPP::setAllocDevice(alloc_device_t* allocDevice)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mAllocDevice = allocDevice;
}

bool ExynosMPP::wasUsedByDisplay(ExynosDisplay *display)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return mState == MPP_STATE_FREE && mDisplay == display;
}

void ExynosMPP::startTransition(ExynosDisplay *display)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    mState = MPP_STATE_TRANSITION;
    mDisplay = display;
    {
        android::Mutex::Autolock lock(mMutex);
        mBufferFreeThread->mCondition.signal();
    }
}

bool ExynosMPP::isOTF()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return false;
}

void ExynosMPP::cleanupOTF()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
}

bool ExynosMPP::checkNoExtVideoBuffer()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    android::Mutex::Autolock lock(mMutex);
    if (mBufferType != MPP_BUFFER_VIDEO_EXT)
        return true;
    else
        return false;
}

void ExynosMPP::freeBuffers()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    alloc_device_t* alloc_device = mAllocDevice;
    android::List<deleteBufferInfo >::iterator it;
    android::List<deleteBufferInfo >::iterator end;
    {
        android::Mutex::Autolock lock(mMutex);
        it = mFreedBuffers.begin();
        end = mFreedBuffers.end();
    }
    while (it != end) {
        deleteBufferInfo bufferInfo = (deleteBufferInfo)(*it);
        if (bufferInfo.bufFence >= 0) {
            if (sync_wait(bufferInfo.bufFence, 1000) < 0)
                ALOGE("sync_wait error");
            close(bufferInfo.bufFence);
        }
        alloc_device->free(alloc_device, bufferInfo.buffer);
        {
            android::Mutex::Autolock lock(mMutex);
            it = mFreedBuffers.erase(it);
        }
    }
}
        
bool BufferFreeThread::threadLoop()
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    while(mRunning) {
        {
            android::Mutex::Autolock lock(mExynosMPP->mMutex);
            while(mExynosMPP->mFreedBuffers.size() == 0 &&
                  mExynosMPP->mState != MPP_STATE_TRANSITION) {
                    mCondition.wait(mExynosMPP->mMutex);
            }
        }
        if (mExynosMPP->mState == MPP_STATE_TRANSITION) {
            if ((mExynosMPP->mDstBufFence[0] >= 0) && sync_wait(mExynosMPP->mDstBufFence[0], 1000) < 0)
                ALOGE("fence wait error");
            mExynosMPP->mState = MPP_STATE_FREE;
        }
        if (mExynosMPP->mFreedBuffers.size() != 0) {
            mExynosMPP->freeBuffers();
        }
    }
    return true;
}
        
void *ExynosMPP::createMPP(int id, int mode, int outputMode, int drm)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    mppFact = new MppFactory();
    libmpp = mppFact->CreateMpp(id, mode, outputMode, drm);

    return reinterpret_cast<void *>(libmpp);
}

int ExynosMPP::configMPP(void *handle, exynos_mpp_img *src, exynos_mpp_img *dst)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    return libmpp->ConfigMpp(handle, src, dst);
}

int ExynosMPP::runMPP(void *handle, exynos_mpp_img *src, exynos_mpp_img *dst)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    return libmpp->RunMpp(handle, src, dst);
}

int ExynosMPP::stopMPP(void *handle)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    return libmpp->StopMpp(handle);
}

void ExynosMPP::destroyMPP(void *handle)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    libmpp->DestroyMpp(handle);
    delete(mppFact);
}

int ExynosMPP::setCSCProperty(void *handle, unsigned int eqAuto, unsigned int fullRange, unsigned int colorspace)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    return libmpp->SetCSCProperty(handle, eqAuto, fullRange, colorspace);
}

int ExynosMPP::freeMPP(void *handle)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    ATRACE_CALL();
    return libmpp->FreeMpp(handle);
}

bool ExynosMPP::bufferChanged(hwc_layer_1_t &layer)
{
    ALOGE("[%s: %d]", __func__, __LINE__);
    private_handle_t *handle = private_handle_t::dynamicCast(layer.handle);
    return mSrcConfig.fw != (uint32_t)handle->stride ||
        mSrcConfig.fh != (uint32_t)handle->vstride ||
        mSrcConfig.format != (uint32_t)handle->format ||
        mDstConfig.rot != (uint32_t)layer.transform;
}
