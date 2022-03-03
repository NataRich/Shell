#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "alias.h"
#include "parse.h"
#include "utils.h"

const char* fns[] = { "alias", "unalias", "exit", NULL };

void execute(const t_arg* arg, t_amap* map);

int main(int argc, char* argv[])
{
    // Invalid number of arguments
    if (argc > 2)
    {
        error("Usage: mysh [batch-file]\n");
        return 1;
    }

    FILE* fptr = stdin;
    if (argc == 2)
    {
        fptr = fopen(argv[1], "r");
        if (fptr == NULL)
        {
            error("Error: Cannot open file %s.\n", argv[1]);
            return 1;
        }
    }

    t_arg* arg = NULL;
    t_amap* map = amap_init();
    char line[kBufferSize] = { 0 };
    do
    {
        if (arg != NULL) arg_free(arg);
        arg = arg_init();

        if (fptr == stdin) info("mysh> ");
        if (fgets(line, kBufferSize, fptr) == NULL) break;
        if (fptr != stdin) info("%s", line);

        line[strlen(line) - 1] = 0;
        parse(line, arg);
        if (arg->argc == 0) continue;
        if (strncmp("exit", arg->argv[0], 5) == 0) break;

        execute(arg, map);
    } while (1);

    arg_free(arg);
    amap_free(map);
    if (fptr != stdin)
        fclose(fptr);
    return 0;
}

void execute(const t_arg* arg, t_amap* map)
{
    char* exe = arg->argv[0];
    unsigned int argc = arg->argc;

    if (strncmp("alias", exe, 6) == 0)
    {
        if (argc == 1)
            amap_print_all(map);
        else if(argc == 2)
            amap_print(map, arg->argv[1]);
        else
            amap_add(map, arg->argv[1], &arg->argv[2], argc - 2);
    }
    else if (strncmp("unalias", exe, 8) == 0)
    {
        if (argc != 2)
            error("unalias: Incorrect number of arguments.\n");
        else
            amap_remove(map, arg->argv[1]);
    }
    else
    {
        int pid = fork();
        if (pid < 0)
            error("Error: Failed to fork.\n");
        else if (pid > 0)
            waitpid(pid, NULL, 0);
        else
        {
            if (arg->redir_file != NULL)
            {
                FILE* fp = fopen(arg->redir_file, "w");
                if (fp == NULL)
                {
                    error("Cannot write to file %s.\n", arg->redir_file);
                    _exit(0);
                }
                dup2(fileno(fp), STDOUT_FILENO);
            }

            t_arg* alias_arg = amap_find_arg(map, exe);
            if (alias_arg == NULL)
            {
                execv(exe, arg->argv);
                error("%s: Command not found.\n", exe);
                _exit(0);
            }
            else
            {
                t_arg* comb = arg_cat(alias_arg, arg);
                execv(comb->argv[0], comb->argv);
                error("%s: Command not found.\n", comb->argv[0]);
                arg_free(comb);
                _exit(0);
            }
        }
    }
}
