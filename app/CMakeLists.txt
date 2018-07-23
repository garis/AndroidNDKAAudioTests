cmake_minimum_required(VERSION 3.4.1)
#-----------------------------------------------------------------------------#
add_library( audio_engine-lib

             SHARED

             src/main/cpp/audio_common.cpp
             src/main/cpp/audio_engine.cpp
             src/main/cpp/jni_bridge.cpp
             src/main/cpp/trace.cpp
             src/main/cpp/sound_gen.cpp
             src/main/cpp/MP3Decoder.cpp
             src/main/cpp/audio_file.cpp)

add_library( lame_mp3-lib

             SHARED

             src/main/cpp/lame-3.100_libmp3lame/VbrTag.c
	         src/main/cpp/lame-3.100_libmp3lame/bitstream.c
	         src/main/cpp/lame-3.100_libmp3lame/encoder.c
	         src/main/cpp/lame-3.100_libmp3lame/fft.c
	         src/main/cpp/lame-3.100_libmp3lame/gain_analysis.c
             src/main/cpp/lame-3.100_libmp3lame/id3tag.c
             src/main/cpp/lame-3.100_libmp3lame/lame.c
             src/main/cpp/lame-3.100_libmp3lame/newmdct.c
	         src/main/cpp/lame-3.100_libmp3lame/presets.c
	         src/main/cpp/lame-3.100_libmp3lame/psymodel.c
	         src/main/cpp/lame-3.100_libmp3lame/quantize.c
	         src/main/cpp/lame-3.100_libmp3lame/quantize_pvt.c
	         src/main/cpp/lame-3.100_libmp3lame/reservoir.c
	         src/main/cpp/lame-3.100_libmp3lame/set_get.c
	         src/main/cpp/lame-3.100_libmp3lame/tables.c
	         src/main/cpp/lame-3.100_libmp3lame/takehiro.c
	         src/main/cpp/lame-3.100_libmp3lame/util.c
	         src/main/cpp/lame-3.100_libmp3lame/vbrquantize.c
	         src/main/cpp/lame-3.100_libmp3lame/version.c
	         src/main/cpp/lame-3.100_libmp3lame/mpglib_interface.c
	         src/main/cpp/lame-3.100_libmp3lame/interface.c
	         src/main/cpp/lame-3.100_libmp3lame/common.c
	         src/main/cpp/lame-3.100_libmp3lame/tabinit.c
	         src/main/cpp/lame-3.100_libmp3lame/layer1.c
	         src/main/cpp/lame-3.100_libmp3lame/layer2.c
	         src/main/cpp/lame-3.100_libmp3lame/layer3.c
	         src/main/cpp/lame-3.100_libmp3lame/decode_i386.c
	         src/main/cpp/lame-3.100_libmp3lame/dct64_i386.c)

#-----------------------------------------------------------------------------#
find_library( log-lib

              log )
#-----------------------------------------------------------------------------#
target_link_libraries( audio_engine-lib lame_mp3-lib android atomic log aaudio

                       ${log-lib} )
