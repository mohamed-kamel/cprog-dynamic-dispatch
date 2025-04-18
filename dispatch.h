#pragma once

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
int cmd_tokenize(const char *raw_cmd, char ***token_ptr);

int cmd_dispatch(int len_app_cmd, const cmd_t app_cmd[], int len_token, char **token);

int cmd_dispatch_str(int len_app_cmd, const cmd_t app_cmd[], const char *cmd_str);
