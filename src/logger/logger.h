#pragma once

#ifdef C8_DEBUG
#include <stdio.h>

// TODO: Handle NO_COLOR

#define C8_LOG(format, ...)                                                                                                                                    \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        fprintf(stderr, "\e[0;34m[ c8@%s:%d ]:\e[0m ", __FILE__, __LINE__);                                                                                    \
        fprintf(stderr, format __VA_OPT__(, ) __VA_ARGS__);                                                                                                    \
    } while (0)

#else
#define C8_LOG(format, ...)                                                                                                                                    \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
    } while (0)
#endif
