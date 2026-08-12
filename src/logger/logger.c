#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static FILE *fp = NULL;

void c8_log_init(void)
{
    fp = fopen("c8_log.txt", "w");

    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open log file handle: %s\n", strerror(errno));

        return;
    }

    fprintf(stderr, "Log file handle opened\n");
}

void c8_log(const char *file, const int line, const char *format, ...)
{

    if (fp == NULL)
    {
        return;
    }

    va_list args;
    va_start(args, format);

    fprintf(fp, "[ c8@%s:%d ]: ", file, line);
    vfprintf(fp, format, args);

    va_end(args);
}

void c8_log_close(void)
{
    if (fp == NULL)
    {
        return;
    }

    int err = fclose(fp);
    if (err != 0)
    {
        fprintf(stderr, "Failed to close log file handle: %s\n", strerror(errno));
    }
    fp = NULL;
}