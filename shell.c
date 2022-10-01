#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>

#define INPUT 0
#define OUTPUT 1
#define APPEND 2

#define COLOR_DEFAULT "\033[1;0m"

#define COLOR_BOLD_GREEN "\033[1;32m"
#define COLOR_BOLD_RED "\033[1;31m"
#define COLOR_BOLD_BLUE "\033[1;34m"
#define COLOR_BOLD_BLACK "\033[1;30m"

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_BLACK "\033[0;30m"

// initiating shell
void init()
{
    printf("\n\n*************************\n\n" COLOR_BOLD_RED "Welcome to Amin's Shell!\nYou can Use the following commands in my Shell:" COLOR_DEFAULT "\n\n*************************\n\n");
}

int tokenize_input(char **par, char *input, const char *c)
{
    int counter = -1;
    char *token;
    token = strtok(input, c);

    while (token)
    {
        par[++counter] = malloc(sizeof(token) + 1);
        strcpy(par[counter], token);

        if (par[counter][strlen(par[counter]) - 1] == ' ' || par[counter][strlen(par[counter]) - 1] == '\n')
        {
            par[counter][strlen(par[counter]) - 1] = '\0';
        }
        if (par[counter][0] == ' ' || par[counter][0] == '\n')
        {
            memmove(par[counter], par[counter] + 1, strlen(par[counter]));
        }
        token = strtok(NULL, c);
    }
    par[++counter] = NULL;
    return counter;
}

int checkRedirection(char **command, int pc)
{
    int index = 0;
    int fd = 0;
    int count = pc;

    for (size_t i = 0; i < count; i++)
    {
        printf("%s", command[i]);

        if (command[i] == '>')
        {
            if (i < count - 1)
            {
                if ((fd = open(command[i + 1], O_WRONLY | O_CREAT)) < 0)
                {
                    perror("command cannot open file\n");
                    return -1;
                }
                dup2(fd, 1);
                if (index == 0)
                    index = i;
            }
            perror("syntax error near unexpected token `newline'\n");
            return -1;
        }
        else if (command[i] == '>>')
        {
            if (i < count - 1)
            {
                if ((fd = open(command[i + 1], O_WRONLY | O_APPEND | O_CREAT)) < 0)
                {
                    perror("command cannot open file\n");
                    return -1;
                }
                dup2(fd, 1);
                if (index == 0)
                    index = i;
            }
            perror("syntax error near unexpected token `newline'\n");
            return -1;
        }
        else if (command == '<')
        {
            if (i < count - 1)
            {
                if ((fd = open(command[i + 1], O_RDONLY)) < 0)
                {
                    perror("command cannot open file\n");
                    return -1;
                }
                dup2(fd, 0);
                if (index == 0)
                    index = i;
            }
            perror("syntax error near unexpected token `newline'\n");
            return -1;
        }
    }
    return index;
}

void singleExec(char **argv, int pc ,int exec)
{
    int redirection_flag = 0;
    int fd = 0;
    int limiter;
    if ((limiter = checkRedirection(argv, pc)) < 0)
    {
        return;
    } // if there is input output redirection

    if (exec)
    {
        char *new_args[100];
        int count = sizeof(argv);
        if (limiter)
        {
            count = limiter;
        }

        for (int i = 1; i < count; i++)
        {
            new_args[i - 1] = argv[i];
        }
        execvp(new_args[0], new_args);

        perror(COLOR_BOLD_RED "invalid input\n" COLOR_DEFAULT);
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0)
    {
        perror(COLOR_BOLD_RED "process creation error.\n" COLOR_DEFAULT);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child

        if (limiter)
        {
            char **sub_argv = malloc(limiter);
            for (int i = 0; i < limiter; i++)
            {
                sub_argv[i] = argv[i];
            }
            execvp(sub_argv[0], sub_argv);
        }

        execvp(argv[0], argv);

        perror(COLOR_BOLD_RED "invalid input\n");
        exit(EXIT_FAILURE);
    }
    else
    { // parent

        wait(NULL);
    }
}

void pipeExec(char **buf, int command_count)
{
    int fd[command_count + 1][2], pc;
    char *argv[100];

    for (int i = 0; i < command_count; i++)
    {
        pc = tokenize_input(argv, buf[i], " ");
        if (i < command_count - 1)
        {
            if (pipe(fd[i]) < 0)
            {
                printf(COLOR_BOLD_RED "Pipe: Creation failed\n");
                return;
            }
        }
        pid_t pid = fork();
        if (pid == 0)
        { // child
            if (i == command_count - 1)
            {
                dup2(fd[i - 1][0], 0);
                close(fd[i - 1][1]);
                close(fd[i - 1][0]);
            }

            else if (i == 0)
            {
                dup2(fd[i][1], 1);
                close(fd[i][0]);
                close(fd[i][1]);
            }
            else
            {
                dup2(fd[i][1], 1);
                close(fd[i][0]);
                close(fd[i][1]);
                dup2(fd[i - 1][0], 0);
                close(fd[i - 1][1]);
                close(fd[i - 1][0]);
            }

            execvp(argv[0], argv);
            perror("invalid input ");
            exit(1); // in case exec is not successfull, exit
        }
        else if (pid < 0)
        {
            perror(COLOR_BOLD_RED "process creation error.\n" COLOR_DEFAULT);
            exit(EXIT_FAILURE);
        }

        // else
        // { // parent
        if (i != 0)
        {
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
        }
        // }
        wait(NULL);
    }
}

void printInfo()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char *username = getenv("USER");
    char hostname[100];
    gethostname(hostname, 100);
    printf(COLOR_BOLD_GREEN "%s@%s" COLOR_DEFAULT ":" COLOR_BOLD_BLUE "%s" COLOR_DEFAULT "$ ", username, hostname, cwd);
}

void handleInput(char *input)
{

    char *buf[100];
    char *params[100];
    if (strchr(input, '|'))
    {
        int param_count = tokenize_input(buf, input, "|");
        pipeExec(buf, param_count);
    }
    else
    {
        int pc = tokenize_input(params, input, " ");

        if (strstr(params[0], "cd"))
        { // cd builtin command
            chdir(params[1]);
        }
        else if (strstr(params[0], "exec"))
        {
            singleExec(params, pc,1);
        }
        else
        {

            singleExec(params, pc,0);
        }
    }
}

int main()
{
    init();
    while (1)
    {
        printInfo();

        char input[512];

        fgets(input, 512, stdin);
        // printf("%s", input);

        handleInput(input);
    }

    return EXIT_SUCCESS;
}