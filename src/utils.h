#ifndef __UTILS__
#define __UTILS__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "log.h"

static inline char * util_LoadText(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f != NULL)
    {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        char *data = malloc((fsize + 1) * sizeof(char));
        if (data != NULL)
        {
            size_t bytesRead = fread(data, 1, fsize, f);
            if (ferror(f))
            {
                log_error("Failed reading file %s", path);
                fclose(f);
                return NULL;
            }

            log_info("File read %i bytes", (int)bytesRead);

            fclose(f);
            data[fsize] = 0;
            return data;
        }
        else
        {
            log_error("Failed to allocate memory", "");
            return NULL;
        }
    }
    else
    {
        log_error("No file found at %s", path);
        return NULL;
    }
}

static inline unsigned char * util_LoadData(const char *path, int *bytesRead)
{
    unsigned char *data = NULL;
    FILE *file = fopen(path, "rb");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);

        data = malloc(fsize * sizeof(unsigned char));
        if (data != NULL)
        {
            *bytesRead = (unsigned int)fread(data, sizeof(unsigned char), fsize, file);
            if (*bytesRead != fsize)
            {
                log_error("File (%s) partially loaded", path);
            }
        }

        fclose(file);
    }
    else
    {
        log_error("Failed to open file (%s)", path);
    }

    return data;
}

#endif