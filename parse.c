#include "parse.h"

unsigned int len_fns()
{
    unsigned int count = 0, index = 0;
    while (fns[index++] != NULL && ++count) {}
    return count;
}

int is_redirected(const char* command)
{
    unsigned int len_cmd = strlen(command) + 1;
    for (unsigned int i = 0; i < len_cmd; i++)
        if (command[i] == '>')
            return 0;
    return -1;
}

void parse_normal(const char* command, t_arg* arg)
{
    char* dup_cmd = strdup(command);
    char* record = dup_cmd;
    char* token;
    while ((token = strtok_r(dup_cmd, " ", &dup_cmd)))
        arg_add(arg, token);
    free(record);
}

void parse_redirection(const char* command, t_arg* arg)
{
    char* dup_cmd = strdup(command);
    char* record = dup_cmd;
    char* exe_cmd = strtok_r(dup_cmd, ">", &dup_cmd);
    char* redir = strtok_r(dup_cmd, ">", &dup_cmd);
    char* redir_file = strtok_r(redir, " ", &redir);
    char* token;
    int n_invalid = 0;

    while ((token = strtok_r(dup_cmd, ">", &dup_cmd)) && ++n_invalid) {}
    while ((token = strtok_r(redir, " ", &redir)) && ++n_invalid) {}

    if (exe_cmd == NULL || redir_file == NULL || n_invalid != 0)
        error("Redirection misformatted.\n");
    else
    {
        parse_normal(exe_cmd, arg);
        arg_add_redir_file(arg, redir_file);
    }
    free(record);
}

void parse(const char* command, t_arg* arg)
{
    char* dup_cmd = strdup(command);
    char* record = dup_cmd;
    char* exe = strtok_r(dup_cmd, " ", &dup_cmd);
    if (exe == NULL)
    {
        free(record);
        return;
    }
    free(record);

    if (is_redirected(command) == 0)
        parse_redirection(command, arg);
    else
        parse_normal(command, arg);
}
