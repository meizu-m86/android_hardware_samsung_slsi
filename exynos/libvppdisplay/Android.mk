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
LOCAL_SHARED_LIBRARIES := liblog libion libutils libcutils libexynosutils \
	libexynosv4l2 libsync libhwcutils libhwcutilsmodule

LOCAL_SHARED_LIBRARIES += libexynosgscaler

LOCAL_CFLAGS += -DUSES_VPP -DUSES_VIRTUAL_DISPLAY 

ifeq ($(TARGET_ARCH), arm64)
	LOCAL_CFLAGS += -DUSES_ARCH_ARM64
endif

LOCAL_C_INCLUDES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libhwc \
	$(LOCAL_PATH)/../libhwcutils \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule

LOCAL_ADDITIONAL_DEPENDENCIES += \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvpphdmi



LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppvirtualdisplay


LOCAL_SRC_FILES := \
	ExynosDisplay.cpp \
	ExynosOverlayDisplay.cpp \
	ExynosDisplayResourceManager.cpp



LOCAL_MODULE := libdisplay

include $(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule/Android.mk

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblog libutils libcutils libhwcutils libexynosutils libdisplay libhwcutilsmodule

LOCAL_CFLAGS += -DUSES_VPP -DUSES_VIRTUAL_DISPLAY 

ifeq ($(TARGET_ARCH), arm64)
	LOCAL_CFLAGS += -DUSES_ARCH_ARM64
endif

LOCAL_C_INCLUDES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libhwc \
	$(LOCAL_PATH)/../libhwcutils \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule

LOCAL_ADDITIONAL_DEPENDENCIES += \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvpphdmi

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppvirtualdisplay

LOCAL_MODULE := libdisplaymodule

include $(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule/Android.mk

include $(BUILD_SHARED_LIBRARY)

