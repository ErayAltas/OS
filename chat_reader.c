#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

// Function prototypes
void *function1(void *arg);
void *function2(void *arg);
void *function3(void *arg);
void *function4(void *arg);
void *function5(void *arg);

pthread_mutex_t mutex; // Mutex lock

int main()
{
    // Initialize the mutex lock
    pthread_mutex_init(&mutex, NULL);

    // Open the named pipe for reading
    int pipe_fd = open("/tmp/my_fifo", O_RDONLY);

    // Create a thread pool with five threads
    pthread_t threads[5];

    // Read from the named pipe
    char buffer[100];
    int bytes_read = read(pipe_fd, buffer, 100);

    // Tokenize the message using strtok()
    char *token = strtok(buffer, " ");

    // Call different functions based on the tokens
    while (token != NULL)
    {
        // Acquire the mutex lock
        pthread_mutex_lock(&mutex);

        if (strcmp(token, "function1") == 0)
        {
            pthread_create(&threads[0], NULL, function1, NULL);
        }
        else if (strcmp(token, "function2") == 0)
        {
            pthread_create(&threads[1], NULL, function2, NULL);
        }
        else if (strcmp(token, "function3") == 0)
        {
            pthread_create(&threads[2], NULL, function3, NULL);
        }
        else if (strcmp(token, "function4") == 0)
        {
            pthread_create(&threads[3], NULL, function4, NULL);
        }
        else if (strcmp(token, "function5") == 0)
        {
            pthread_create(&threads[4], NULL, function5, NULL);
        }
        else
        {
            printf("Invalid token: %s\n", token);
        }

        // Release the mutex lock
        pthread_mutex_unlock(&mutex);

        token = strtok(NULL, " ");
    }

    // Wait for all threads to complete
    for (int i = 0; i < 5; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Close the named pipe
    close(pipe_fd);

    // Destroy the mutex lock
    pthread_mutex_destroy(&mutex);

    return 0;
}

void *function1(void *arg)
{
    // Get the file descriptor of the named pipe
    int *pipe_fd = (int)arg;

    // Do some work here
    int result = 1 + 2;

    // Write the result to the named pipe
    char response[100];
    sprintf(response, "Result of function1: %d", result);
    write(pipe_fd, response, strlen(response));

    pthread_exit(NULL);
}