#include "whereami.c"

#include <stdio.h>

#define JULIE_IMPL
#include "julie.h"

#include <limits.h>
#include <stdlib.h>
#include <libgen.h>

Julie_Interp *interp;

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


    interp = julie_init_interp();
    julie_set_error_callback(interp, on_julie_error);
    julie_set_argv(interp, argc - 1, argv + 1);
    julie_set_cur_file(interp, julie_get_string_id(interp, argv[1]));

    exe_path_length = wai_getExecutablePath(NULL, 0, NULL);
    if (exe_path_length >= 0) {
        exe_path        = malloc(exe_path_length + 1 + strlen("/packages"));
        wai_getExecutablePath(exe_path, exe_path_length, NULL);
        exe_path[exe_path_length] = 0;

        exe_dir = dirname(exe_path);
/*         strcat(exe_dir, "/packages"); */

        julie_add_package_directory(interp, exe_dir);
        free(exe_path);
    }

    julie_parse(interp, code, strlen(code));
    julie_interp(interp);
    julie_free(interp);

    return 0;
}


static void on_julie_error(Julie_Error_Info *info) {
    Julie_Status           status;
    const char            *blue;
    const char            *red;
    const char            *reset;
    char                  *s;
    unsigned               i;
    Julie_Backtrace_Entry *it;
    Julie_Value           *key;
    Julie_Value           *bt;
    Julie_Value           *frame;
    Julie_Value           *message;

    status = info->status;

    if (isatty(2)) {
        blue  = "\033[34m";
        red   = "\033[31m";
        reset = "\033[0m";
    } else {
        blue = red = reset = "";
    }

    fprintf(stderr, "%s%s:%llu:%llu:%s %serror: %s",
            blue,
            info->file_id == NULL ? "<?>" : julie_get_cstring(info->file_id),
            info->line,
            info->col,
            reset,
            red,
            julie_error_string(status));

    switch (status) {
        case JULIE_ERR_LOOKUP:
            if (info->lookup.sym != NULL) {
                fprintf(stderr, " (%s)", info->lookup.sym);
            }
            break;
        case JULIE_ERR_RELEASE_WHILE_BORROWED:
            if (info->release_while_borrowed.sym != NULL) {
                fprintf(stderr, " (%s)", info->release_while_borrowed.sym);
            }
            break;
        case JULIE_ERR_REF_OF_TRANSIENT:
            if (info->ref_of_transient.sym != NULL) {
                fprintf(stderr, " (%s)", info->ref_of_transient.sym);
            }
            break;
        case JULIE_ERR_REF_OF_OBJECT_KEY:
            if (info->ref_of_object_key.sym != NULL) {
                fprintf(stderr, " (%s)", info->ref_of_object_key.sym);
            }
            break;
        case JULIE_ERR_NOT_LVAL:
            if (info->not_lval.sym != NULL) {
                fprintf(stderr, " (%s)", info->not_lval.sym);
            }
            break;
        case JULIE_ERR_MODIFY_WHILE_ITER:
            if (info->modify_while_iter.sym != NULL) {
                fprintf(stderr, " (%s)", info->modify_while_iter.sym);
            }
            break;
        case JULIE_ERR_ARITY:
            fprintf(stderr, " (wanted %s%llu, got %llu)",
                    info->arity.at_least ? "at least " : "",
                    info->arity.wanted_arity,
                    info->arity.got_arity);
            break;
        case JULIE_ERR_TYPE:
            fprintf(stderr, " (wanted %s, got %s)",
                    julie_type_string(info->type.wanted_type),
                    julie_type_string(info->type.got_type));
            break;
        case JULIE_ERR_BAD_APPLY:
            fprintf(stderr, " (got %s)", julie_type_string(info->bad_application.got_type));
            break;
        case JULIE_ERR_BAD_INDEX:
            s = julie_to_string(info->interp, info->bad_index.bad_index, 0);
            fprintf(stderr, " (index: %s)", s);
            free(s);
            break;
        case JULIE_ERR_FILE_NOT_FOUND:
        case JULIE_ERR_FILE_IS_DIR:
        case JULIE_ERR_MMAP_FAILED:
            fprintf(stderr, " (%s)", info->file.path);
            break;
        case JULIE_ERR_LOAD_PACKAGE_FAILURE:
            fprintf(stderr, " (%s) %s", info->load_package_failure.path, info->load_package_failure.package_error_message);
            break;
        case JULIE_ERR_REGEX:
            fprintf(stderr, " %s", info->regex.regex_error_message);
            break;
        default:
            break;
    }

    fprintf(stderr, "%s\n", reset);

    i = 0;
    while ((it = julie_bt_entry(info->interp, i)) != NULL) {
        s = julie_to_string(info->interp, it->fn, 0);
        fprintf(stderr, "    %s%s:%llu:%llu %s%s\n",
                blue,
                it->file_id == NULL ? "<?>" : julie_get_cstring(it->file_id),
                it->line,
                it->col,
                s,
                reset);
        free(s);

        i += 1;
    }

    if (info->status == JULIE_ERR_ERROR_VALUE) {
        key = julie_symbol_value(info->interp, julie_get_string_id(info->interp, "'__message__"));
        message = julie_object_get_field(info->thrown.error_value, key);
        julie_free_value(info->interp, key);

        if (message != NULL) {
            s = julie_to_string(info->interp, message, JULIE_NO_QUOTE);
            fprintf(stderr, "%serror thrown: %s%s%s\n", blue, red, s, reset);
            free(s);
        } else {
            fprintf(stderr, "%serror thrown:%s\n", blue, reset);
        }

        key = julie_symbol_value(info->interp, julie_get_string_id(info->interp, "'__backtrace__"));
        bt = julie_object_get_field(info->thrown.error_value, key);
        julie_free_value(info->interp, key);

        if (bt != NULL && bt->type == JULIE_LIST) {
            ARRAY_FOR_EACH(bt->list, frame) {
                s = julie_to_string(info->interp, frame, JULIE_NO_QUOTE);
                fprintf(stderr, "    %s%s%s\n", blue, s, reset);
                free(s);
            }
        }
    }

    julie_free_error_info(info);

    exit(status);
}
