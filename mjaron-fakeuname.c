// #define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <unistd.h>

#ifndef mjaron_fakeuname_H
#include "mjaron-fakeuname.h"
#endif

const mjaron_fakeuname_struct mjaron_fakeuname_preset_i686 = {
    "Linux",
    "mjaron-fakeuname",
    "2.6.32-754.28.1.el6.i686",
    "#SMP Wed Mar 11 19:03:51 UTC 2020",
    "i686",
    "unknown"
};

const mjaron_fakeuname_struct mjaron_fakeuname_preset_x86_64 = {
    "Linux",
    "mjaron-fakeuname",
    "6.8.0-48-generic",
    "#48-Ubuntu SMP PREEMPT_DYNAMIC Fri Sep 27 14:04:52 UTC 2024",
    "x86_64",
    "unknown"
};

#define MJARON_FAKEUNAME_LOG_MAX 1024

MJARON_FAKEUNAME_INLINE void mjaron_fakeuname_loge(const char* line)
{
    fprintf(stderr, "%s\n", line);
}

#define MJARON_FAKEUNAME_LOGE(_line) mjaron_fakeuname_loge(_line)

#define MAX_PARAM_LENGTH 1024
#define MAX_VALUE_LENGTH 1024

void mjaron_fakeuname_trim_whitespace(char* str)
{
    // Trim leading whitespace
    int frontCount = 0;
    {
        char* it = str;
        while (isspace((unsigned char)*it) != 0)
        {
            ++frontCount;
            ++it;
        }
    }

    // Trim end trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';

    const int len = strlen(str);

    {
        int i = 0;
        while (i < len - frontCount + 1)
        {
            const char moved = str[i + frontCount];
            str[i] = moved;
            if (moved == '\0')
            {
                break;
            }
            ++i;
        }
    }
}

int mjaron_fakeuname_read_property_line(const char* line, char* param, char* value)
{
    // Initialize the output variables
    param[0] = '\0';
    value[0] = '\0';

    // Find the position of the colon
    const char* colon_pos = strchr(line, ':');
    if (colon_pos == NULL)
    {
        MJARON_FAKEUNAME_LOGE("Invalid format: no colon found.");
        return EXIT_FAILURE;
    }

    // Calculate the length of the parameter
    const size_t param_length = colon_pos - line;

    // Check for buffer overflow
    if (param_length >= MAX_PARAM_LENGTH)
    {
        MJARON_FAKEUNAME_LOGE("Parameter too long.");
        return EXIT_FAILURE;
    }

    // Copy the parameter into the provided buffer and trim whitespace
    strncpy(param, line, param_length);
    param[param_length] = '\0'; // Null-terminate the parameter string
    mjaron_fakeuname_trim_whitespace(param); // Trim leading and trailing whitespace from param

    // Copy the value (skip the colon and any leading whitespace)
    const char* value_start = colon_pos + 1;
    mjaron_fakeuname_trim_whitespace((char*)value_start); // Trim leading whitespace from value_start

    // Copy the value into the provided buffer
    strncpy(value, value_start, MAX_VALUE_LENGTH); // Copy the value
    mjaron_fakeuname_trim_whitespace(value); // Trim trailing whitespace from value
    return EXIT_SUCCESS;
}

int mjaron_fakeuname_config_should_ignore_line(const char* line, const size_t len)
{
    size_t i = 0;
    while (i < len)
    {
        const char iLine = line[i];
        if (iLine == ' ')
        {
            ++i;
            continue;
        }
        if (iLine == '#' || iLine == '\n' || iLine == '\r')
        {
            return 1; // Line is empty (ignored.
        }

        return 0; // Line not empty.
    }
    return 1;
}

int mjaron_fakeuname_read(const char* const conf, struct utsname* un)
{
    if (un == NULL)
    {
        return EXIT_FAILURE;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    FILE* fp = fopen(conf, "r");
    if (fp == NULL)
    {
        return EXIT_FAILURE;
    }

    while ((read = getline(&line, &len, fp)) != -1)
    {
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s", line);
        if (read == 0 || len == 0)
        {
            continue;
        }

        const int isEmpty = mjaron_fakeuname_config_should_ignore_line(line, read);
        if (isEmpty)
        {
            continue;
        }

        char param[MAX_PARAM_LENGTH] = { '\0' };
        char value[MAX_PARAM_LENGTH] = { '\0' };
        if (mjaron_fakeuname_read_property_line(line, param, value) != EXIT_SUCCESS)
        {
            MJARON_FAKEUNAME_LOGE("Failed to read line:");
            MJARON_FAKEUNAME_LOGE(line);
            return EXIT_FAILURE;
        }

        if (strcmp(param, "sysname") == 0)
        {
            mjaron_fakeuname_set_sysname(un, value);
        }
        else if (strcmp(param, "nodename") == 0)
        {
            mjaron_fakeuname_set_nodename(un, value);
        }
        else if (strcmp(param, "release") == 0)
        {
            mjaron_fakeuname_set_release(un, value);
        }
        else if (strcmp(param, "version") == 0)
        {
            mjaron_fakeuname_set_version(un, value);
        }
        else if (strcmp(param, "machine") == 0)
        {
            mjaron_fakeuname_set_machine(un, value);
        }
        else if (strcmp(param, "domainname") == 0)
        {
            mjaron_fakeuname_set_domainname(un, value);
        }
        else
        {
            MJARON_FAKEUNAME_LOGE("Failed to read line, unsupported parameter:");
            MJARON_FAKEUNAME_LOGE(line);
            return EXIT_FAILURE;
        }
        // MJARON_FAKEUNAME_LOGE("Line processed:");
        // MJARON_FAKEUNAME_LOGE(line);
    }

    fclose(fp);
    if (line)
    {
        free(line);
    }
    return EXIT_SUCCESS;
}

/**
 * @brief   Used to shadow original uname system function.
 *          See: https://linux.die.net/man/2/uname
 * @param   un The initialized `utsname` structure which should be filled by this function.
 * @return  `0` on success, other number on failure.
 */
extern int uname(struct utsname* un)
{
    const int ret = syscall(SYS_uname, un);
    if (ret != 0)
    {
        return ret;
    }

    // Loading preset.
    const char* preset = getenv("MJARON_FAKEUNAME_PRESET");

    if (preset)
    {
        if (preset && strcmp(preset, "i686") == 0)
        {
            mjaron_fakeuname_set_struct(un, mjaron_fakeuname_preset_i686);
        }
        if (strcmp(preset, "x86_64") == 0 || strcmp(preset, "x64") == 0)
        {
            mjaron_fakeuname_set_struct(un, mjaron_fakeuname_preset_x86_64);
        }
    }

    // Trying to load the settings.
    const char* conf = getenv("MJARON_FAKEUNAME_CONF");
    if (conf == NULL)
    {
        conf = MJARON_FAKEUNAME_CONF_DEFAULT;
    }
    mjaron_fakeuname_read(conf, un);

    // Loading particular variables from environment variables.
    const char* machine = getenv("MJARON_FAKEUNAME_MACHINE");
    if (machine)
    {
        mjaron_fakeuname_set_machine(un, machine);
    }

    return 0;
}
