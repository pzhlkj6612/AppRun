/**************************************************************************
 *
 * Copyright (c) 2020 Alexis Lopez Zubieta <contact@azubieta.net>
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime_environmet.h"
#include "../common/file_utils.h"
#include "../common/shell_utils.h"
#include "../hooks/environment.h"

char* get_env_file_path(const char* appdir) {
    char* path = calloc(strlen(appdir) + strlen("/.env") + 1, sizeof(char));
    strcat(path, appdir);
    strcat(path, "/.env");

    return path;
}

void apprun_setenv_prefixed(const char* prefix, const char* name, const char* value) {
    unsigned prefixed_name_size = strlen(prefix) + strlen(name) + 1;
    char* prefixed_name = calloc(prefixed_name_size, sizeof(char));
    strcat(prefixed_name, prefix);
    strcat(prefixed_name, name);

    if (value)
        setenv(prefixed_name, value, 1);

    free(prefixed_name);
}

void setup_runtime_environment(char* appdir) {
    char* env_file_path = get_env_file_path(appdir);
    char** env = apprun_file_read_lines(env_file_path);

    for (char** itr = env; *itr != NULL; itr++) {
        char* name = apprun_env_str_entry_extract_name(*itr);
        char* value = apprun_env_str_entry_extract_value(*itr);
        char* expanded_value = apprun_shell_expand_variables(value);

        char* original_value = getenv(name);
        setenv(name, expanded_value, 1);
        apprun_setenv_prefixed(APPRUN_ENV_ORIG_PREFIX, name, original_value);
        apprun_setenv_prefixed(APPRUN_ENV_STARTUP_PREFIX, name, expanded_value);

        free(expanded_value);
        free(value);
        free(name);
    }
}