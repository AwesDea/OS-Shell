#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>

#define COLOR_DEFAULT "\033[1;0m"

#define COLOR_BOLD_GREEN "\033[1;32m"
#define COLOR_BOLD_RED "\033[1;31m"
#define COLOR_BOLD_BLUE "\033[1;34m"
#define COLOR_BOLD_BLACK "\033[1;30m"

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED "\033[0;31m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_BLACK "\033[0;30m"

int fd_in = -1;
int fd_out = -1;
// initiating shell
void init()
{
    printf("\n\n*************************\n\n" COLOR_BOLD_RED "Welcome to Amin's Shell!\nSay Her Name - #MahsaAmini" COLOR_DEFAULT "\n\n*************************\n\n");
}

int max(int a, int b)
{
    if (a < b)
        return b;

    return a;
}

int tokenize_input(char **par, char *input, const char *c, int pipe_flag)
{
    int counter = -1;
    char *token;
    token = strtok(input, c);

    while (token)
    {
        if (pipe_flag && (strcmp(token, "exec") == 0))
        {
            token = strtok(NULL, c);
            continue;
        }

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

char *checkRedirection(char *command)
{
    int i = 0;
    int count = strlen(command);
    int input = 0;
    int output = 0;
    int output_index = 0;
    char c;
    int index = 1;
    fd_in = -1;
    fd_out = -1;

    for (size_t i = 0; i < count; i++)
    {
        c = command[i];

        // if (c == '\0')
        //     break;
        if (c == '>')
        {
            if (output_index == 0)
            {
                output = index;
                index++;
            }
            output_index++;
        }
        if (c == '<')
        {
            input = index;
            index++;
        }
    }
    char *buf[100];

    int pc = tokenize_input(buf, command, "><>>", 0);
    int file_mod = O_WRONLY | O_CREAT;
    if (max(input, output) > (pc - 1))
    {
        perror("syntax error near unexpected token `newline'\n");
        return "error";
    }
    if (output_index > 0)
    {
        if (output_index == 2)
        {
            file_mod = file_mod | O_APPEND;
        } else{
            file_mod = file_mod | O_TRUNC;
        }
        if ((fd_out = open(buf[output], file_mod, 0666)) < 0)
        {
            perror("command cannot open file\n");
            return "error";
        }

        dup2(fd_out, 1);
        close(fd_out);
    }

    if (input > 0)
    {
        file_mod = O_RDONLY;

        if ((fd_in = open(buf[input], file_mod)) < 0)
        {
            perror("command cannot open file\n");
            return "error";
        }
        dup2(fd_in, 0);
        close(fd_in);
    }

    return buf[0];
}

void singleExec(char *input, int exec)
{
    fd_in = -1;
    fd_out = -1;
    int fd = 0;
    pid_t pid = fork();
    if (pid < 0)
    {
        perror(COLOR_BOLD_RED "process creation error.\n" COLOR_DEFAULT);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child
        if (exec)
            exit(1);

        char *command = checkRedirection(input);
        if (strcmp(command, "error") == 0)
        {
            return;
        }
        char *buf[100];
        int pc = tokenize_input(buf, command, " ", 0);

        if (strcmp(buf[0], "head") == 0)
        {
            execv("./head.o", buf);
        }
        if (strcmp(buf[0], "uniq") == 0)
        {
            execv("./uniq.o", buf);
        }
        if (strcmp(buf[0], "sort") == 0)
        {
            execv("./sort.o", buf);
        }

        execvp(buf[0], buf);

        perror(COLOR_BOLD_RED "invalid input\n" COLOR_DEFAULT);
        return;
    }
    else
    { // parent
        if (exec)
        {
            char *command = checkRedirection(input);
            if (strcmp(command, "error") == 0)
            {
                return;
            }
            char *buf[100];
            int pc = tokenize_input(buf, command, " ", 0);
            char *new_args[100];
            memset(new_args, 0, sizeof(new_args));
            int count = pc;

            for (int i = 1; i < count; i++)
            {
                new_args[i - 1] = buf[i];
            }
            new_args[pc] = NULL;

            if (strcmp(new_args[0], "head") == 0)
            {
                execv("./head.o", new_args);
            }
            if (strcmp(new_args[0], "uniq") == 0)
            {
                execv("./uniq.o", new_args);
            }
            if (strcmp(new_args[0], "sort") == 0)
            {
                execv("./sort.o", new_args);
            }

            execvp(new_args[0], new_args);

            perror(COLOR_BOLD_RED "invalid input\n" COLOR_DEFAULT);
            return;
        }
        wait(NULL);
    }
}

void pipeExec(char *input)
{
    char *buf[100];
    int command_count = tokenize_input(buf, input, "|", 0);
    int fd[command_count + 1][2], pc;
    char *argv[100];

    for (int i = 0; i < command_count; i++)
    {
        fd_in = -1;
        fd_out = -1;

        if (i < command_count - 1)
        {
            if (pipe(fd[i]) < 0)
            {
                printf(COLOR_BOLD_RED "Pipe: Creation failed\n" COLOR_DEFAULT);
                return;
            }
        }

        pid_t pid = fork();
        if (pid == 0)
        { // child

            char *command = checkRedirection(buf[i]);
            if (strcmp(command, "error") == 0)
            {
                return;
            }

            pc = tokenize_input(argv, command, " ", 1);

            if (i == command_count - 1)
            {
                if (fd_in < 0)
                {
                    dup2(fd[i - 1][0], 0);
                }

                close(fd[i - 1][1]);
                close(fd[i - 1][0]);
            }

            else if (i == 0)
            {
                if (fd_out < 0)
                {
                    dup2(fd[i][1], 1);
                }
                close(fd[i][0]);
                close(fd[i][1]);
            }
            else
            {
                if (fd_out < 0)
                {
                    dup2(fd[i][1], 1);
                }
                if (fd_in < 0)
                {
                    dup2(fd[i - 1][0], 0);
                }
                close(fd[i][0]);
                close(fd[i][1]);
                close(fd[i - 1][1]);
                close(fd[i - 1][0]);
            }

            if (strcmp(argv[0], "head") == 0)
            {
                execv("./head.o", argv);
            }
            if (strcmp(argv[0], "uniq") == 0)
            {
                execv("./uniq.o", argv);
            }
            if (strcmp(argv[0], "sort") == 0)
            {
                execv("./sort.o", argv);
            }

            execvp(argv[0], argv);
            perror(COLOR_BOLD_RED "invalid input " COLOR_DEFAULT);
            return;
        }
        else if (pid < 0)
        {
            perror(COLOR_BOLD_RED "process creation error.\n" COLOR_DEFAULT);
            return;
        }

        if (i != 0)
        {
            close(fd[i - 1][0]);
            close(fd[i - 1][1]);
        }
    }
    while (wait(NULL) > 0)
        ;
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
    char input_copy[512];
    strcpy(input_copy, input);

    char *buf[100];

    if (strchr(input, '|'))
    {
        pipeExec(input);
    }
    else
    {
        tokenize_input(buf, input, " ", 0);

        if (strstr(buf[0], "cd"))
        {
            chdir(buf[1]);
        }
        else if (strstr(buf[0], "exit"))
        {
            exit(EXIT_SUCCESS);
        }
        else if (strstr(buf[0], "exec"))
        {
            singleExec(input_copy, 1);
        }
        else
        {
            singleExec(input_copy, 0);
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
        handleInput(input);
    }

    return EXIT_SUCCESS;
}