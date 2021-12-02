# Copyright (C) 2012 The Android Open Source Project
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
# HAL module implemenation, not prelinked and stored in
# hw/<COPYPIX_HARDWARE_MODULE_ID>.<ro.product.board>.so

ifneq ($(BOARD_DISABLE_HWC_DEBUG),true)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := ExynosHWCDebug.c
LOCAL_MODULE := hwcdebug
include $(BUILD_EXECUTABLE)
endif

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SHARED_LIBRARIES := liblog libion libcutils libEGL libGLESv1_CM libhardware \
	libhardware_legacy libutils libsync \
	libexynosv4l2 libexynosutils libhwcutils libdisplay libhdmi \
	libmpp libhwcutilsmodule libdisplaymodule libhdmimodule libvirtualdisplaymodule

LOCAL_SHARED_LIBRARIES += libexynosgscaler

LOCAL_SHARED_LIBRARIES += libExynosHWCService
LOCAL_C_INCLUDES += $(TOP)/hardware/samsung_slsi/exynos/libhwcService





LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppvirtualdisplay

LOCAL_SHARED_LIBRARIES += libvirtualdisplay
LOCAL_C_INCLUDES += $(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libvirtualdisplaymodule



LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvpphdmi


LOCAL_CFLAGS += -DLOG_TAG=\"hwcomposer\"
LOCAL_CFLAGS += -DHLOG_CODE=0  -DUSES_VPP -DUSES_VIRTUAL_DISPLAY -DUSES_HWC_SERVICES -DHWC_SERVICES

LOCAL_C_INCLUDES += \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libhwcutils \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/exynos/libcec \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhdmimodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp \
	$(TOP)/system/core/libsync/include

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay


LOCAL_ADDITIONAL_DEPENDENCIES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_SRC_FILES := ExynosHWC.cpp

LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

