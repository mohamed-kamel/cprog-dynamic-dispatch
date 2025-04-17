#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isspace()

typedef void (*cmd_func_t)(int argc, char *argv[]); // define a function signature

typedef struct
{
    char *cmd_text;
    cmd_func_t cmd_func;
} cmd_t;

/**
 * @brief Tokenizes a string by whitespace (spaces and tabs) into an array of strings.
 *
 * Allocates a single contiguous block of memory to hold both the array of pointers (argv)
 * and the null-terminated token strings. The input string is copied into this block
 * and modified in place (whitespace replaced with null terminators).
 *
 * @param raw_cmd The input string to tokenize. This string is NOT modified.
 * @param token_ptr A pointer to a char**, which will be set to the base address
 * of the allocated block containing the token pointers and data.
 * The caller is responsible for freeing this pointer using free()
 * when done. On allocation failure or if no tokens are found,
 * *argv_ptr is set to NULL.
 *
 * @return The number of tokens found (token_count), or -1 on memory allocation error.
 * Returns 0 if the input string is NULL, empty, or contains only whitespace.
 */
int tokenize_cmd(const char *raw_cmd, char ***token_ptr)
{
    // --- Argument Validation ---
    if (!token_ptr)
    {
        return -1;
    }
    *token_ptr = NULL;

    if (!raw_cmd)
    {
        return 0;
    }

    int token_count = 0;
    const char *count_p = raw_cmd; // Use a specific pointer for counting pass
    int in_token = 0;

    // --- Pass 1: Count Tokens ---
    while (*count_p != '\0')
    {
        if (!isspace((unsigned char)*count_p))
        {
            if (!in_token)
            {
                token_count++;
                in_token = 1;
            }
        }
        else
        {
            in_token = 0;
        }
        count_p++;
    }

    if (token_count == 0)
    {
        return 0;
    }

    // --- Memory Allocation ---
    size_t string_len = strlen(raw_cmd);
    size_t pointers_size = token_count * sizeof(char *);
    size_t string_data_size = string_len + 1;
    size_t total_size = pointers_size + string_data_size;

    // Allocate the single block
    void *block = malloc(total_size); // Using malloc directly is fine
    if (!block)
    {
        // perror("Failed to allocate memory for tokenization"); // Optional: perror writes to stderr
        return -1; // Indicate allocation failure
    }

    // --- Pass 2: Populate Block ---
    *token_ptr = (char **)block;
    char *string_copy = (char *)block + pointers_size;

    // Copy the original string into our allocated space
    memcpy(string_copy, raw_cmd, string_data_size); // includes null terminator

    // *** Use a NEW pointer (char *) for iterating the mutable copy ***
    char *iter_p = string_copy;
    char *current_token_start = NULL;
    int current_token_index = 0;

    while (current_token_index < token_count)
    { // Loop until all expected tokens are found
        // Skip leading whitespace within the copy
        while (*iter_p != '\0' && isspace((unsigned char)*iter_p))
        {
            iter_p++;
        }

        // Check for end of string after skipping whitespace
        if (*iter_p == '\0')
        {
            // Should not happen if argc > 0 and counting logic was correct,
            // but handle defensively.
            break;
        }

        // Found the start of a token
        current_token_start = iter_p;
        (*token_ptr)[current_token_index] = current_token_start;
        current_token_index++;

        // Move to the end of the current token
        while (*iter_p != '\0' && !isspace((unsigned char)*iter_p))
        {
            iter_p++;
        }

        // If not end of string, terminate the token by replacing whitespace
        if (*iter_p != '\0')
        {
            *iter_p = '\0'; // Terminate the token - OK now because iter_p is char*
            iter_p++;       // Move past the null terminator for the next iteration
        }
        // If it was the end of the string, the token is already null-terminated
        // from the memcpy, and the outer loop condition will handle termination.
    }

    // Defensive check: Ensure we found the expected number of tokens
    if (current_token_index != token_count)
    {
        fprintf(stderr, "Tokenization logic error: final count mismatch (%d != %d)\n", current_token_index, token_count);
        free(block); // Clean up allocated block on error
        *token_ptr = NULL;
        return -1; // Internal error
    }

    return token_count;
}

int cmd_dispatch(int len_app_cmd, cmd_t app_cmd[], int len_token, char **token)
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
    int len_cmd = sizeof(command) / sizeof(cmd_t);

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

    int token_count = tokenize_cmd(cmd_raw, &token_array);

    for(int x = 0; x < token_count; x++){
        printf("token %d is %s\n", x, *(token_array + x));
    }

    cmd_dispatch(len_cmd, command, token_count, token_array);

    free(cmd_raw);
    free(token_array);
}