#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void splitCommand(char *cmd, char **argv)
{
    int i = 0;
    argv[i] = strtok(cmd, " ");
    while (argv[i] != NULL)
    {
        argv[++i] = strtok(NULL, " ");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return 1;
    }

    char *inp = argv[1];
    char *cmd = argv[2];
    char *out = argv[3];
    char *cmd_args[10];

    splitCommand(cmd, cmd_args);

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return 1;
    }
    else if (pid == 0)
    {
        // Redirect input if inp is not "-"
        if (strcmp(inp, "-") != 0)
        {
            int fd_in = open(inp, O_RDONLY);
            if (fd_in == -1)
            {
                perror("open input");
                return 1;
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // Redirect output if out is not "-"
        if (strcmp(out, "-") != 0)
        {
            int fd_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1)
            {
                perror("open output");
                return 1;
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (execvp(cmd_args[0], cmd_args) == -1)
        {
            perror("execvp");
            return 1;
        }
    }
    else
    {
        wait(NULL);
    }

    return 0;
}