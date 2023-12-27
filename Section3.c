#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXLINE 80

// Global string to store the most recent command
char history[MAXLINE];
char lastCommand[MAXLINE];

//Save the most recent command
void SaveLastCommand(const char *command) {
    strcpy(lastCommand, command);
}

//Update History
void UpdateHistory(const char *command) {
    strcpy(history, command);
}

int main() {
    char args[MAXLINE / 2 + 1];
    int should_run = 1;

    while (should_run) {
        // Display the shell prompt
        printf("21521832>");
        fflush(stdout);

        // Read the user input
        fgets(args, sizeof(args), stdin);
        args[strcspn(args, "\n")] = 0;  // Remove the newline character from the input

        // Check if the user wants to exit
        if (strcmp(args, "exit") == 0) {
            should_run = 0;
        } else if (strcmp(args, "!!") == 0) {
            // Execute the most recent command
            if (history[0] == '\0') {
                printf("No commands in history.\n");
            } else {
                printf("Executing: %s\n", history);

                // Fork a new process to execute the command
                pid_t pid = fork();

                if (pid == 0) {
                    // Child process
                    char *token;
                    char *tokens[MAXLINE / 2 + 1];
                    int i = 0;

                    // Tokenize the command
                    token = strtok(history, " ");
                    while (token != NULL) {
                        tokens[i++] = token;
                        token = strtok(NULL, " ");
                    }
                    tokens[i] = NULL;

                    // Execute the command
                    execvp(tokens[0], tokens);
                    // If execvp fails, print an error message
                    fprintf(stderr, "Exec failed\n");

                    return 1;
                }

                // Wait for the child process to finish
                if (pid > 0) {
                    wait(NULL);
                }
            }
        } else {
            // Save the entered command as the most recent
            SaveLastCommand(args);

            pid_t pid = fork();

            if (pid == 0) {
                char *token;
                char *tokens[MAXLINE / 2 + 1];
                int i = 0;

                token = strtok(args, " ");
                while (token != NULL) {
                    tokens[i++] = token;
                    token = strtok(NULL, " ");
                }
                tokens[i] = NULL;
                execvp(tokens[0], tokens);
                fprintf(stderr, "Exec failed\n");
                return 1;
            }
            if (pid > 0) {
                int status;
                wait(&status);

                // Check if the child process exited normally
                if (WIFEXITED(status))
                {
                    int exit_status = WEXITSTATUS(status);
                    if (exit_status == 1)
                    {
                        // Child process encountered an error, we clear lastCommand
                        SaveLastCommand("");
                    } else
                    {
                        // otherwise, copy last command to history
                        UpdateHistory(lastCommand);
                    }
                }
            }
        }
    }
    return 0;
}
