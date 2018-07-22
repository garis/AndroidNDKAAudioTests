#include <dlfcn.h>
#include "logging_macros.h"
#include <cstdio>
#include "trace.h"

static const int TRACE_MAX_SECTION_NAME_LENGTH = 100;

// Tracing functions
static void *(*ATrace_beginSection)(const char *sectionName);

static void *(*ATrace_endSection)(void);

typedef void *(*fp_ATrace_beginSection)(const char *sectionName);

typedef void *(*fp_ATrace_endSection)(void);

bool Trace::is_tracing_supported_ = false;

void Trace::beginSection(const char *fmt, ...){

    static char buff[TRACE_MAX_SECTION_NAME_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    va_end(args);

    if (is_tracing_supported_) {
        ATrace_beginSection(buff);
    } else {
        LOGE("Tracing is either not initialized (call Trace::initialize()) "
                     "or not supported on this device");
    }
}

void Trace::endSection() {

    if (is_tracing_supported_) {
        ATrace_endSection();
    }
}

void Trace::initialize() {

    // Using dlsym allows us to use tracing on API 21+ without needing android/trace.h which wasn't
    // published until API 23
    void *lib = dlopen("libandroid.so", RTLD_NOW | RTLD_LOCAL);
    if (lib == nullptr) {
        LOGE("Could not open libandroid.so to dynamically load tracing symbols");
    } else {
        ATrace_beginSection =
                reinterpret_cast<fp_ATrace_beginSection >(
                        dlsym(lib, "ATrace_beginSection"));
        ATrace_endSection =
                reinterpret_cast<fp_ATrace_endSection >(
                        dlsym(lib, "ATrace_endSection"));

        if (ATrace_beginSection != nullptr && ATrace_endSection != nullptr){
            is_tracing_supported_ = true;
        }
    }
}