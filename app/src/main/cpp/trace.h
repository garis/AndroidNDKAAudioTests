//
// Created by riccardo on 21/07/18.
//

#ifndef AUDIOTEST_TRACE_H
#define AUDIOTEST_TRACE_H

class Trace {

public:
    static void beginSection(const char *format, ...);
    static void endSection();
    static void initialize();

private:
    static bool is_tracing_supported_;
};

#endif //AUDIOTEST_TRACE_H
