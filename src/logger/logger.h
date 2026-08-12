#pragma once

#ifdef C8_DEBUG

void c8_log_init(void);
void c8_log(const char *file, const int line, const char *format, ...);
void c8_log_close(void);

#define C8_LOG(format, ...)                                                                                                                                    \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
                                                                                                                                                               \
        c8_log(__FILE__, __LINE__, format __VA_OPT__(, ) __VA_ARGS__);                                                                                         \
    } while (0)

#else
#define C8_LOG(format, ...)                                                                                                                                    \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
    } while (0)
#endif
