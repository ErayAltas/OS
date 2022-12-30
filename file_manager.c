#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

#define MAX_BUF 128

char file_list[10][100];
char response[128];
char c;
int file_length = sizeof(file_list) / sizeof(file_list[0]);
int count = 0;
FILE *fl;
int id = 0;
pthread_t threads[4];

struct params
{
    char *arg1;
    char *arg2;
};

/*
char *listenPort = "/tmp/file_manager";
char *port1 = "/tmp/named_pipe_1";
char *port2 = "/tmp/named_pipe_2";
char *port3 = "/tmp/named_pipe_3";
char *port4 = "/tmp/named_pipe_4";
char *port5 = "/tmp/named_pipe_5"; */

void *create(char *args);
void *delete(char *args);
void *myRead(char *args);
void *writeFile(char *args);
char **matrixGenerate(int row, int column);
char **arraySplit(char *array);

int main()

{

    char **myArray;
    void *status;
    int fd;
    char *myfifo = "/tmp/myfifo";
    char buf[MAX_BUF];
    memset(file_list, '\0', sizeof(file_list));

    while (1)
    {
        fd = open(myfifo, O_RDONLY);
        read(fd, buf, MAX_BUF);

        myArray = arraySplit(buf);

        struct params params;

        params.arg1 = myArray[1];
        params.arg2 = myArray[2];

        printf("arg1 %s\n", params.arg1);
        printf("arg2 %s\n", params.arg2);

        if (strcmp(myArray[0], "create") == 0)
        {

            //  create(myArray[1]);

            pthread_create(&threads[0], NULL, create, &params);
        }
        else if (strcmp(myArray[0], "delete") == 0)
        {

            // delete (myArray[1]);
            pthread_create(&threads[1], NULL, delete, &params);
        }
        else if (strcmp(myArray[0], "write") == 0)
        {

            // writeFile(myArray[1], myArray[2]);
            pthread_create(&threads[2], NULL, writeFile, &params);
        }
        else if (strcmp(myArray[0], "read") == 0)
        {
            // myRead(myArray[1]);
            pthread_create(&threads[3], NULL, myRead, &params);
        }
        else if (strcmp(myArray[0], "exit") == 0)
        {
            exit(0);
        }
        for (int i = 0; i < 4; i++)
        {
            pthread_join(threads[i], &status);
        }

        fd = open(myfifo, O_WRONLY);
        write(fd, response, sizeof(response));
        close(fd);
    }

    exit(0);
}

void *create(char *args)
{
    struct params *params = args;
    char *file_name = params->arg1;
    printf("filename : %s", file_name);

    int idx = -1;
    for (int i = 0; i < file_length; i++)
    {
        if (file_list[i] != NULL)
        {
            if (strcmp(file_list[i], file_name) == 0)
            {
                idx = i;
            }
        }
    }
    if (idx == -1)
    {
        for (int i = 0; i < 10; i++)
        {
            if (file_list[i][0] == '\0')
            {
                strcpy(file_list[i], file_name);
                FILE *file = fopen(file_name, "w");
                fclose(file);
                strcpy(response, "File Created");

                break;
            }
        }
    }
    else
    {
        strcpy(response, "File zaten var.");
    }
}

void *delete(char *args)
{

    struct params *params = args;
    char *file_name = params->arg1;
    printf("filename : %s", file_name);

    int idx = -1;

    for (int i = 0; i < file_length; i++)
    {
        if (file_list[i] != NULL)
        {
            if (strcmp(file_list[i], file_name) == 0)
            {
                idx = i;

                break;
            }
        }
    }

    if (idx != -1)
    {

        file_list[idx][0] = '\0';
        remove(file_name);
        strcpy(response, "File Deleted!");
    }
    else
    {
        strcpy(response, "Silinecek dosya bulunmadı.");
    }
}

void *myRead(char *args)
{

    struct params *params = args;
    char *file_name = params->arg1;
    printf("filename : %s", file_name);
    int idx = -1;
    for (int i = 0; i < file_length; i++)
    {
        if (file_list[i] != NULL)
        {
            if (strcmp(file_list[i], file_name) == 0)
            {
                idx = i;
                break;
            }
        }
    }

    if (idx != -1)
    {
        FILE *fptr = fopen(file_name, "r");
        while ((c = fgetc(fptr)) != EOF)
        {
            printf("%c", c);
        }
        fclose(fptr);

        strcpy(response, "File Readed!");
    }
    else
    {
        strcpy(response, "Okunacak Dosya Bulunamadı");
    }
}

void *writeFile(char *args)
{

    struct params *params = args;
    char *file_name = params->arg1;
    char *data = params->arg2;
    printf("filename : %s", file_name);
    printf("data : %s", data);

    int idx = -1;

    for (int i = 0; i < 10; i++)
    {

        if (file_list[i] != NULL)
        {
            if (strcmp(file_list[i], file_name) == 0)
            {
                idx = i;
                break;
            }
        }
    }
    if (idx != -1)
    {

        FILE *file = fopen(file_name, "a+");
        if (file == NULL)
        {
            perror("fopen failed");
            // return;
        }

        fprintf(file, "%s\n", data);

        if (fclose(file) == EOF)
        {
            perror("fclose failed");
            // return;
        }
        strcpy(response, "File Writed!");
    }
    else
    {
        strcpy(response, "Yazılacak Dosya Bulunamadı");
    }
}

char **matrixGenerate(int row, int column)
{
    int i;
    char **matrix = malloc(row * sizeof(int *));
    for (i = 0; i < row; i++)
    {
        matrix[i] = malloc(column * sizeof(int));
    }
    return matrix;
}

char **arraySplit(char *array)
{

    int i = 0;
    char *p = strtok(array, " ");
    char **arr;
    arr = matrixGenerate(10, 10);
    while (p != NULL)
    {
        *(arr + i) = p;
        i++;
        p = strtok(NULL, " ");
    }

    return arr;
}