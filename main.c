#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void (*cmd_func_t)(int argc, char *argv[]); // define a function signature

typedef struct
{
    char *cmd_text;
    cmd_func_t cmd_func;
} cmd_t;

int cmd_dispatch(int len_app_cmd, cmd_t app_cmd[], int len_token, char *token[])
{
    int cmd_input = -1;
    for (int idx_com = 0; idx_com < len_app_cmd; idx_com++)
    {
        int res = strcmp(token[0], app_cmd[idx_com].cmd_text);
        if (res == 0)
        {
            cmd_input = idx_com;
            break;
        }
    }

    if (cmd_input < 0)
    {
        printf("unknown command: %s\n", token[0]);
        return -1;
    }
    else
    {
        len_token--;

        char **argv;

        if (len_token == 0)
        {
            argv = NULL;
        }
        else
        {
            argv = token + 1;
        }

        app_cmd[cmd_input].cmd_func(len_token, argv);
    }
    return 0;
}

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

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        printf("no commands provided\n");
        return 1;
    }

    argc--;
    char **args = argv + 1;

    int len_cmd = sizeof(command) / sizeof(cmd_t);

    return cmd_dispatch(len_cmd, command, argc, args);
}