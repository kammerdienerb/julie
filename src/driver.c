#include "whereami.c"

#include <stdio.h>

#define JULIE_IMPL
#include "julie.h"

#include <limits.h>
#include <stdlib.h>
#include <libgen.h>

Julie_Interp interp;

static void on_julie_error(Julie_Error_Info *info);

int main(int argc, char **argv) {
    Julie_Status        status;
    const char         *code;
    unsigned long long  code_size;
    int                 exe_path_length;
    char               *exe_path;
    char               *exe_dir;

    if (argc < 2) {
        fprintf(stderr, "expected at least one argument: a julie file path\n");
        return 1;
    }

    if ((status = julie_map_file_into_readonly_memory(argv[1], &code, &code_size))) {
        fprintf(stderr, "error opening '%s': %s\n", argv[1], julie_error_string(status));
        return 1;
    }


    julie_init_interp(&interp);
    julie_set_error_callback(&interp, on_julie_error);
    julie_set_argv(&interp, argc - 1, argv + 1);
/*     interp.cur_file = julie_get_string_id(&interp, argv[1]); */


    exe_path_length = wai_getExecutablePath(NULL, 0, NULL);
    if (exe_path_length >= 0) {
        exe_path        = malloc(exe_path_length + 1 + strlen("/packages"));
        wai_getExecutablePath(exe_path, exe_path_length, NULL);
        exe_path[exe_path_length] = 0;

        exe_dir = dirname(exe_path);
        strcat(exe_dir, "/packages");

/*         julie_add_package_directory(&interp, exe_dir); */
        free(exe_path);
    }

    julie_parse(&interp, code, strlen(code));
    julie_interp(&interp);
    julie_free(&interp);

    return 0;
}


static void on_julie_error(Julie_Error_Info *info) {
    Julie_Status           status;

    status = info->status;

    printf("%s\n", julie_error_string(status));
/*     julie_free_error_info(info); */

    exit(status);
}
