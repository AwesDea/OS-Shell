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

// void executeBasic(char **argv)
// {
//     if (fork() > 0)
//     {
//         // parent
//         wait(NULL);
//     }
//     else
//     {
//         // child
//         execvp(argv[0], argv);
//         // in case exec is not successfull, exit
//         perror(ANSI_COLOR_RED "invalid input" ANSI_COLOR_RESET "\n");
//         exit(1);
//     }
// }

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
            if (i < command_count - 1)
            {
                dup2(fd[i][1], 1);
                close(fd[i][0]);
                close(fd[i][1]);
            }

            if (i != 0)
            {
                dup2(fd[i - 1][0], 0);
                close(fd[i - 1][1]);
                close(fd[i - 1][0]);
            }
            execvp(argv[0], argv);
            perror("invalid input ");
            exit(1); // in case exec is not successfull, exit
        }
        else
        { // parent
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
        }
        wait(NULL);
    }
}

// void executeRedirect(char **buf, int nr, int mode)
// {
//     int pc, fd;
//     char *argv[100];
//     removeWhiteSpace(buf[1]);
//     tokenize_input(argv, &pc, buf[0], " ");
//     if (fork() == 0)
//     {

//         switch (mode)
//         {
//         case INPUT:
//             fd = open(buf[1], O_RDONLY);
//             break;
//         case OUTPUT:
//             fd = open(buf[1], O_WRONLY);
//             break;
//         case APPEND:
//             fd = open(buf[1], O_WRONLY | O_APPEND);
//             break;
//         default:
//             return;
//         }

//         if (fd < 0)
//         {
//             perror("cannot open file\n");
//             return;
//         }

//         switch (mode)
//         {
//         case INPUT:
//             dup2(fd, 0);
//             break;
//         case OUTPUT:
//             dup2(fd, 1);
//             break;
//         case APPEND:
//             dup2(fd, 1);
//             break;
//         default:
//             return;
//         }
//         execvp(argv[0], argv);
//         perror("invalid input ");
//         exit(1); // in case exec is not successfull, exit
//     }
//     wait(NULL);
// }

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
    if (strchr(input, '|'))
    {
        int param_count = tokenize_input(buf, input, "|");
        pipeExec(buf, param_count);
    }
}

int main()
{
    init();
    printInfo();
    char input[512];
    // scanf("%s", input);
    fgets(input, 512, stdin);
    // printf("%s", input);
    handleInput(input);

    return EXIT_SUCCESS;
}