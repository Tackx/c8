#ifdef _WIN32
#include <errno.h>
#include <fcntl.h>
#include <io.h>

#else
#include <unistd.h>
#define _isatty isatty
#define _fileno fileno
#endif

#include <stdio.h>
#include <string.h>

#include "c8.h"
#include "logger/logger.h"

// TODO: More error handling (see `load_data_input`)
int load_data_path(C8 *c8, const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Could not read file with path %s. %s\n", path, strerror(errno));

        return 1;
    }

    size_t bytes_read = fread(&c8->ram[C8_PROGRAM_START_LOCATION], sizeof(unsigned char), C8_RAM_SIZE - C8_PROGRAM_START_LOCATION, file);
    fclose(file);

    if (bytes_read > 0)
    {
        // https://stackoverflow.com/questions/8060170/printing-hexadecimal-characters-in-c
        C8_LOG("Read: %02hhX\n", c8->ram[C8_PROGRAM_START_LOCATION]);
    }

    return 0;
}

int load_data_input(C8 *c8)
{
    int readTotal = 0;

    if (_isatty(_fileno(stdin)))
    {
        fprintf(stderr, "No input file provided\n");

        return 1;
    };

    int result = _setmode(_fileno(stdin), _O_BINARY);
    if (result == -1)
    {
        perror("Cannot set stdin to binary mode");

        return 1;
    }

    while (readTotal < C8_RAM_SIZE - C8_PROGRAM_START_LOCATION)
    {

        int read = fread(&c8->ram[C8_PROGRAM_START_LOCATION] + readTotal, 1, C8_RAM_SIZE - C8_PROGRAM_START_LOCATION - readTotal, stdin);

        C8_LOG("Read chars: %d\n", read);

        readTotal += read;

        if (read == 0)
        {
            break;
        }
    }

    if (readTotal == 0)
    {
        fprintf(stderr, "No input file provided\n");

        return 1;
    }

    unsigned char read_check;
    size_t remains = fread(&read_check, 1, 1, stdin);

    C8_LOG("Remains = %zu\n", remains);

    if (remains > 0)
    {
        // Error out, ROM too big
        fprintf(stderr, "Input file too big\n");

        return 1;
    }

    C8_LOG("Total read chars: %d\n", readTotal);

    return 0;
}