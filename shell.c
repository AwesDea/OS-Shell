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

void singleExec(char **argv, int exec)
{
    if (exec)
    {
        char *new_args[100];
        for (int i = 1; i < sizeof(argv); i++)
        {
            new_args[i - 1] = argv[i];
        }
        printf("heylooow %s", new_args[0]);
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
        printf(COLOR_BOLD_GREEN "child is here\n" COLOR_DEFAULT);

        execvp(argv[0], argv);

        perror(COLOR_BOLD_RED "invalid input\n" COLOR_DEFAULT);
        exit(EXIT_FAILURE);
    }
    else
    { // parent

        printf("parents is gonna wait\n");
        wait(NULL);
        printf("parents is done with waiting\n");
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
        // pipeExec(buf, param_count);
    }
    else
    {
        tokenize_input(params, input, " ");

        if (strstr(params[0], "cd"))
        { // cd builtin command
            chdir(params[1]);
        }
        else if (strstr(params[0], "exec"))
        {
            singleExec(params, 1);
        }

        else
        {

            singleExec(params, 0);
        }
    }
}

int main()
{
    init();
    printInfo();
    char input[512];
    // scanf("%s", input);
    fgets(input, 512, stdin);
    printf("%s", input);

    handleInput(input);

    return EXIT_SUCCESS;
}