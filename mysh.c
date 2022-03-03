#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <string.h>
//#include <sys/wait.h>

#include "parse.h"
#include "utils.h"

// TODO: `mysh> exe >> output.txt` is invalid!!!!!!!

const char* fns[] = { "alias", "unalias", "exit", NULL };

int main(int argc, char* argv[])
{
    // Invalid number of arguments
    if (argc > 2)
    {
        error("Usage: mysh [batch-file]\n");
        return 1;
    }

    // Batch mode
    if (argc == 2)
    {
        FILE* f = fopen(argv[1], "r");
        if (f == NULL)
        {
            error("Error: Cannot open file %s.\n", argv[1]);
            return 1;
        }

        fclose(f);
        return 0;
    }

    t_arg* arg = NULL;
    char line[kBufferSize] = { 0 };

    do
    {
        if (arg != NULL) arg_free(arg);
        arg = arg_init();

        info("mysh> ");
        if (fgets(line, kBufferSize, stdin) == NULL)
        {
            info("\n");
            break;
        }

        line[strlen(line) - 1] = 0;
        parse(line, arg);

//      for (unsigned int i = 0; i < arg->argc; i++)
//          info("argv[%d]: \"%s\"; ", i, arg->argv[i]);
//      if (arg->redir_file != NULL)
//          info("redir_file: \"%s\";", arg->redir_file);
    } while (!arg->argc || strncmp("exit", arg->argv[0], 5) != 0);

    arg_free(arg);
    return 0;
}
