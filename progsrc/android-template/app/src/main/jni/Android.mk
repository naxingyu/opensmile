
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := opensmile
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libopensmile.a
#LOCAL_EXPORT_C_INCLUDES := 
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

############ you might need to change this if you move the
############ android studio project somewhere else, i.e.
############ outside of the opensmile root folder
OPENSMILE_ROOT := ../../../../../../
##########################################################

OPENSMILE_INCLUDES_DIR := $(OPENSMILE_ROOT)/src/include
OPENSMILE_LIBS_DIR := $(OPENSMILE_ROOT)/inst/android

LOCAL_MODULE	:= smile_jni
LOCAL_SRC_FILES := smile_jni.cpp 
LOCAL_SRC_FILES += smilextract.cpp

LOCAL_CPP_FEATURES := exceptions

LOCAL_C_INCLUDES := $(OPENSMILE_INCLUDES_DIR)

LOCAL_STATIC_LIBRARIES := opensmile
LOCAL_LDLIBS := -ggdb -lstdc++ -lm -ldl -llog -D__STDC_CONSTANT_MACROS -lc
LOCAL_LDLIBS += -lOpenSLES

include $(BUILD_SHARED_LIBRARY)

