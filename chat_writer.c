
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    // Create the named pipe (FIFO)
    // with read/write permissions
    mkfifo("/tmp/my_fifo", 0666);

    // Open the named pipe for writing
    int pipe_fd = open("/tmp/my_fifo", O_WRONLY);

    // Loop until the user enters the string "exit"
    while (1)
    {
        // Read a message from standard input (terminal)
        char message[100];
        printf("Enter message to send (enter 'exit' to quit): ");
        gets(message);

        // Check if the message is "exit"
        if (strcmp(message, "exit") == 0)
        {
            break;
        }

        // Tokenize the message using strtok()
        char *token = strtok(message, " ");

        // Write the tokens to the named pipe
        while (token != NULL)
        {
            write(pipe_fd, token, strlen(token));
            token = strtok(NULL, " ");
        }
    }

    // Close the named pipe
    close(pipe_fd);

    return 0;
}