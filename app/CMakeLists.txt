cmake_minimum_required(VERSION 3.4.1)

find_library(log-lib log)
find_library(android-lib android)

add_library(native-lib
        SHARED
        src/main/cpp/native-lib.cpp
        src/main/cpp/egl/egl_core.cpp
        src/main/cpp/egl/base_surface.cpp
        src/main/cpp/egl/window_surface.cpp
        src/main/cpp/egl/offscreen_surface.cpp
        src/main/cpp/common/gl_utils.cpp

        src/main/cpp/render/looper.cpp
        src/main/cpp/render/gl_looper.cpp
        src/main/cpp/render/gl_renderer.cpp

        src/main/cpp/filter/base_filter.cpp
        src/main/cpp/filter/bg_filter.cpp

        src/main/cpp/decoder/video_decoder.cpp
        src/main/cpp/decoder/audio_decoder.cpp
        src/main/cpp/decoder/circle_av_frame_queue.cpp

        src/main/cpp/audio/audio_looper.cpp
        src/main/cpp/audio/opensles_player.cpp
        src/main/cpp/audio/audio_player.cpp

        src/main/cpp/synchronize/video_audiio_synchronizer.cpp

        src/main/cpp/offscreen/off_screen_thread.cpp
        src/main/cpp/offscreen/on_screen_thread.cpp
        src/main/cpp/encoder/video_encoder.cpp

        src/main/cpp/dexmuxer/av_demuxer.cpp
        src/main/cpp/dexmuxer/circle_av_packet_queue.cpp
        )

set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswresample.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswscale.so)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavcodec.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavfilter.so)

add_library(avdevice
        SHARED
        IMPORTED)
set_target_properties(avdevice
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavdevice.so)

add_library(x264
        SHARED
        IMPORTED)
set_target_properties(x264
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libx264.a)

include_directories(${JNI_LIBS_DIR}/includes)

target_link_libraries(native-lib
        avutil swresample swscale avcodec avformat avfilter avdevice x264
        ${log-lib} ${android-lib}
        android EGL GLESv3 jnigraphics OpenSLES
        oboe
        )

# Build the Oboe library
set(OBOE_DIR ../../../ndk/oboe)
add_subdirectory(${OBOE_DIR} ./oboe)

# Make the Oboe public headers available to our app
include_directories(${OBOE_DIR}/include)