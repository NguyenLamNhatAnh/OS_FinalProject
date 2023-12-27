#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#define MAXLINE 80

int main()
{
    char args[MAXLINE / 2 + 1];
    int should_run = 1;

    while (should_run)
    {
        // Display the shell prompt
        printf("21521832>");
        fflush(stdout);

        // Read the user input
        fgets(args, sizeof(args), stdin);
        args[strcspn(args, "\n")] = 0; // Remove the newline character from the input

        // Check if the user wants to exit
        if (strcmp(args, "exit") == 0)
        {
            should_run = 0;`
        }
        else
        {
            pid_t pid = fork();

            if (pid == 0)
            {
                char *token;
                char *tokens[MAXLINE / 2 + 1];
                int i = 0;

                token = strtok(args, " ");
                while (token != NULL)
                {
                    tokens[i++] = token;
                    token = strtok(NULL, " ");
                }
                tokens[i] = NULL;

                // Redirection logic for the history command
                int redirect_input = 0;
                int redirect_output = 0;
                for (int j = 0; tokens[j] != NULL; ++j)
                {
                    if (strcmp(tokens[j], ">") == 0)
                    {
                        // Output redirection
                        redirect_output = 1;
                        tokens[j] = NULL; // Remove '>' from the command
                        break;
                    }
                    else if (strcmp(tokens[j], "<") == 0)
                    {
                        // Input redirection
                        redirect_input = 1;
                        tokens[j] = NULL; // Remove '<' from the command
                        break;
                    }
                }
                if (redirect_input)
                {
                    // Input redirection
                    int fd = open(tokens[i - 1], O_RDONLY);
                    if (fd == -1)
                    {
                        perror("open");
                        return 1;
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (redirect_output)
                {
                    // Output redirection
                    int fd = open(tokens[i - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd == -1)
                    {
                        perror("open");
                        return 1;
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }

                execvp(tokens[0], tokens);
                fprintf(stderr, "Exec failed\n");
                return 1;
            }
            if (pid > 0)
            {
                wait(NULL);
            }
        }
    }
    return 0;
}
