#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

int getIndex(char *data)
{
    int index = 0;
    for (int i = 0; data[i] != '\0'; i++)
    {
        index++;
    }
    return index;
}

main()
{
    int fd;
    char *myfifo = "/tmp/myfifo";

    mkfifo(myfifo, 0666);

    while (1)
    {
        char data[128];
        char response[128];
        fd = open(myfifo, O_WRONLY);
        fgets(data, 128, stdin);
        if (data[getIndex(data) - 1] == '\n')
        {
            data[getIndex(data) - 1] = '\0';
        }
        write(fd, data, sizeof(data));

        fd = open(myfifo, O_RDONLY);
        read(fd, response, 128);

        printf("%s\n", response);

        close(fd);
    }
}
