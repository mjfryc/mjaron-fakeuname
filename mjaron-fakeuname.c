// #define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#ifndef mjaron_fakeuname_H
#include "mjaron-fakeuname.h"
#endif

mjaron_fakeuname_struct mjaron_fakeuname_preset_i686 =
{
    "Linux",
    "mjaron_fakeuname",
    "2.6.32-754.28.1.el6.i686",
    "#SMP Wed Mar 11 19:03:51 UTC 2020",
    "i686",
    "unknown"
};

#define MAX_PARAM_LENGTH 1024
#define MAX_VALUE_LENGTH 1024

void mjaron_fakeuname_trim_whitespace(char *str) {
    // Trim leading whitespace
    int frontCount = 0;
    for (char* it = str; isspace((unsigned char)*it) != 0; ++it)
    {
        ++frontCount;
    }

    // Trim end trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
    {
        end--;
    }

    // Null-terminate the trimmed string
    *(end + 1) = '\0';

    const int len = strlen(str);
    for (int i = 0; i < len - frontCount + 1; ++i)
    {
        const char moved = str[i + frontCount];
        str[i] = moved;
        if (moved == '\0')
        {
            break;
        }
    }
}

int mjaron_fakeuname_read_property_line(const char *line, char *param, char *value) {
    // Initialize the output variables
    param[0] = '\0';
    value[0] = '\0';

    // Find the position of the colon
    const char *colon_pos = strchr(line, ':');
    if (colon_pos == NULL)
    {
        fprintf(stderr, "Invalid format: no colon found\n");
        return EXIT_FAILURE;
    }

    // Calculate the length of the parameter
    const size_t param_length = colon_pos - line;

    // Check for buffer overflow
    if (param_length >= MAX_PARAM_LENGTH)
    {
        fprintf(stderr, "Parameter too long\n");
        return EXIT_FAILURE;
    }

    // Copy the parameter into the provided buffer and trim whitespace
    strncpy(param, line, param_length);
    param[param_length] = '\0'; // Null-terminate the parameter string
    mjaron_fakeuname_trim_whitespace(param); // Trim leading and trailing whitespace from param

    // Copy the value (skip the colon and any leading whitespace)
    const char *value_start = colon_pos + 1;
    mjaron_fakeuname_trim_whitespace((char *)value_start); // Trim leading whitespace from value_start

    // Copy the value into the provided buffer
    strcpy(value, value_start); // Copy the value
    mjaron_fakeuname_trim_whitespace(value); // Trim trailing whitespace from value
    return EXIT_SUCCESS;
}

int mjaron_fakeuname_read(const char* const conf, struct utsname *un)
{
    if (un == NULL)
    {
        return EXIT_FAILURE;
    }

    char * line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    FILE * fp = fopen(conf, "r");
    if (fp == NULL)
    {
        return EXIT_FAILURE;
    }

    while ((read = getline(&line, &len, fp)) != -1)
    {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
        if (read == 0 || len == 0)
        {
            continue;
        }
        int isEmpty = 1;
        for (size_t i = 0; i < read; ++i)
        {
            const char iLine = line[i];
            if (iLine == ' ')
            {
                continue;
            }
            if (iLine == '#' || iLine == '\n' || iLine == '\r')
            {
                isEmpty = 1;
                break;
            }

            isEmpty = 0;
            break;
        }

        if (isEmpty)
        {
            continue;
        }
        char param[MAX_PARAM_LENGTH] = { '\0' };
        char value[MAX_PARAM_LENGTH] = { '\0' };
        if (mjaron_fakeuname_read_property_line(line, param, value) != EXIT_SUCCESS)
        {
            fprintf(stderr, "Failed to read line:[%s]\n", line);
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
            fprintf(stderr, "Failed to read line, unsupported parameter: [%s]\n", line);
            return EXIT_FAILURE;
        }
        // fprintf(stderr, "Line processed: [%s]\n", line);
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
extern int uname(struct utsname *un)
{
    const int ret = syscall(SYS_uname, un);
    if (ret != 0)
    {
        return ret;
    }

    // Loading preset.
    const char* preset = getenv("MJARON_FAKEUNAME_PRESET");
    if (preset && strcmp(preset, "i686") == 0)
    {
        mjaron_fakeuname_set_struct(un, mjaron_fakeuname_preset_i686);
    }

    // Trying to load the settings.
    const char* conf = getenv("MJARON_FAKEUNAME_CONF");
    if (conf == NULL)
    {
        conf = "/etc/mjaron_fakeuname.conf";
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
