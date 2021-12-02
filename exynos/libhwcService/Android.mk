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

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblog libcutils libhardware_legacy libutils libbinder \
			  libexynosv4l2 libhdmi libhwcutils libsync

LOCAL_CFLAGS += -DLOG_TAG=\"HWCService\"
LOCAL_CFLAGS += -DHLOG_CODE=4 -DUSES_VPP -DUSES_VIRTUAL_DISPLAY

LOCAL_C_INCLUDES := \
	$(TOP)/hardware/samsung_slsi/$(TARGET_BOARD_PLATFORM)/include \
	$(TOP)/hardware/samsung_slsi/exynos/include \
	$(TOP)/hardware/samsung_slsi/exynos/libhwcutils \
	$(TOP)/hardware/samsung_slsi/exynos/libexynosutils \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcmodule \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule


LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvppdisplay \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libdisplaymodule




LOCAL_C_INCLUDES += $(LOCAL_PATH)/../libvpphdmi




LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/exynos/libhwc




LOCAL_C_INCLUDES += $(TOP)/hardware/samsung_slsi/exynos/libvppvirtualdisplay


LOCAL_C_INCLUDES += $(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libvirtualdisplaymodule

LOCAL_SHARED_LIBRARIES += libvirtualdisplay

LOCAL_C_INCLUDES += \
	$(TOP)/hardware/samsung_slsi/$(TARGET_SOC)/libhwcutilsmodule \
	$(TOP)/hardware/samsung_slsi/exynos/libmpp



LOCAL_SRC_FILES := ExynosHWCService.cpp IExynosHWC.cpp

LOCAL_MODULE := libExynosHWCService
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)


