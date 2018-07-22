cmake_minimum_required(VERSION 3.4.1)
#-----------------------------------------------------------------------------#
add_library( audioengine-lib

             SHARED

             src/main/cpp/audio_common.cpp
             src/main/cpp/audioengine.cpp
             src/main/cpp/jni_bridge.cpp
             src/main/cpp/trace.cpp
             src/main/cpp/sound_gen.cpp)
#-----------------------------------------------------------------------------#
find_library( log-lib

              log )
#-----------------------------------------------------------------------------#
target_link_libraries( audioengine-lib android atomic log aaudio

                       ${log-lib} )
