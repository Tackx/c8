#pragma once

#ifdef C8_DEBUG
#define C8_LOG(format, ...) fprintf(stderr, format __VA_OPT__(, ) __VA_ARGS__)
#else
#define C8_LOG(format, ...)                                                                                                                                    \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
    } while (0)
#endif
