# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
#LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libexynosutils \
#libexynosv4l2 libsync libion_exynos libmpp
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libexynosutils \
			  libexynosv4l2 libsync libion libmpp

ifeq ($(BOARD_DISABLE_HWC_DEBUG), true)
	LOCAL_CFLAGS += -DDISABLE_HWC_DEBUG
endif


LOCAL_SHARED_LIBRARIES += libexynosgscaler



LOCAL_CFLAGS += -DLOG_TAG=\"hwcutils\"
LOCAL_CFLAGS += -DHLOG_CODE=4 -DUSES_VPP
LOCAL_C_INCLUDES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libhwc \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp \
	$(TOP)/system/core/libsync/include


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay


LOCAL_ADDITIONAL_DEPENDENCIES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvpphdmi


LOCAL_SRC_FILES += \
	ExynosHWCUtils.cpp


LOCAL_SRC_FILES += \
	ExynosMPPv2.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule



LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../libvppvirtualdisplay

LOCAL_MODULE := libhwcutils

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libhwcutils

ifeq ($(BOARD_DISABLE_HWC_DEBUG), true)
	LOCAL_CFLAGS += -DDISABLE_HWC_DEBUG
endif

LOCAL_CFLAGS += -DLOG_TAG=\"libhwcutilsmodule\"
LOCAL_CFLAGS += -DHLOG_CODE=4 -DUSES_VPP
LOCAL_C_INCLUDES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libhwc \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp \
	$(TOP)/system/core/libsync/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay
LOCAL_ADDITIONAL_DEPENDENCIES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvpphdmi
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../libvppvirtualdisplay

include $(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule/Android.mk

LOCAL_MODULE := libhwcutilsmodule

include $(BUILD_SHARED_LIBRARY)

