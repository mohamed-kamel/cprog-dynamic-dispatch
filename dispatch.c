#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isspace()

#include "dispatch.h"


int cmd_tokenize(const char *raw_cmd, char ***token_ptr)
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

int cmd_dispatch(int len_app_cmd, const cmd_t app_cmd[], int len_token, char **token)
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

int cmd_dispatch_str(int len_app_cmd, const cmd_t app_cmd[], const char *cmd_str){
    char **token_array = NULL;
    int token_count = cmd_tokenize(cmd_str, &token_array);
    int res = cmd_dispatch(len_app_cmd, app_cmd, token_count, token_array);
    free(token_array);
    return res;
}
