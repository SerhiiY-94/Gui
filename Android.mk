LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libui
LOCAL_CFLAGS += $(GLOBAL_CFLAGS)
LOCAL_C_INCLUDES += $(GLOBAL_C_INCLUDES)
LOCAL_SRC_FILES :=  _ui.cpp

include $(BUILD_STATIC_LIBRARY)
