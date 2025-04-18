#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isspace()

#include "dispatch.h"

void add_all(int argc, char *argv[])
{
    if (argc == 0)
    {
        printf("nothing to add\n");
        return;
    }

    double sum = 0;

    for (int c = 0; c < argc; c++)
    {
        char *endptr = NULL;
        double v = strtod(argv[c], &endptr);
        if (endptr == argv[c] || *endptr != '\0')
        {
            printf("invalid number: %s\n", argv[c]);
            return;
        }
        else
        {
            sum += v;
        }
    }
    printf("The sum is %f\n", sum);
}

void func2(int argc, char *argv[])
{
    printf("performing Second command\n");
}

void func3(int argc, char *argv[])
{
    printf("performing Third command\n");
}

cmd_t command[] = {
    {.cmd_text = "add_all",
     .cmd_func = add_all},
    {.cmd_text = "cmd2",
     .cmd_func = func2},
    {.cmd_text = "cmd3",
     .cmd_func = func3},
};

const int len_app_cmd = sizeof(command) / sizeof(cmd_t);

int main(int argc, char *argv[])
{
    char *cmd = "add_all 3.14 2 3";
    cmd_dispatch_str(len_app_cmd, command, cmd);
    return 0;
}

int test_tokenized_cmd()
{
    int v = 3;

    const char *fmt = "add_all 1 2 %d";

    int needed = snprintf(NULL, 0, fmt, v);

    char *cmd_raw = malloc(needed + 1);

    if (!cmd_raw)
    {
        printf("error allocating formatter space\n");
        return -2;
    }

    snprintf(cmd_raw, needed + 1, fmt, v);

    char **token_array = NULL;

    int token_count = cmd_tokenize(cmd_raw, &token_array);

    for (int x = 0; x < token_count; x++)
    {
        printf("token %d is %s\n", x, *(token_array + x));
    }

    cmd_dispatch(len_app_cmd, command, token_count, token_array);

    free(cmd_raw);
    free(token_array);
}