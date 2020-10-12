#include <sys/socket.h>
// #include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 5006

const char* get_data_for_send(const char* file_name)
{
    FILE* fp;
    char c;
    int cur_size = 0;
    int size = 255;
    int new_size = size;
    char *buffer = malloc(size*sizeof(char));

    fp = fopen(file_name, "r");
    
    if (fp == NULL){
        perror("File wasn't open");
        return NULL;
    }

    while (fscanf(fp, "%c", &c) != EOF)
    {
        if (cur_size == size)
        {
            new_size +=size;
            buffer = (char*)realloc(buffer, new_size*sizeof(char));
            if (buffer == NULL)
            {
                free(buffer);
                perror("Error (re)allocating memory");
            }
        }
        buffer[cur_size++] = c;
    }

    fclose(fp);
    return buffer;
}

int main(int argc, char *argv[]) 
{
    int sockfd = 0, n = 0;
    struct sockaddr_in serv_addr;
    char* file_n = NULL;

    if (argc != 4 ) 
    {
        printf("\n Usage: %s <ip of server> %s <number port> %s <file name>  \n", 
               argv[1], argv[2], argv[3]);
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        perror("inet_pton error occured ");
        return 1;
    }

    printf("listen %s:%d\n", inet_ntoa(serv_addr.sin_addr), atoi(argv[2]));

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       perror("Connection failed ");
       return 1;
    }

    const char* buff = get_data_for_send(argv[3]);
    if (send(sockfd, buff, strlen(buff), 0) != strlen(buff))
        perror("Sent a different number of bytes than expected ");
    else
        printf("Data was sent\n");

    return 0;
}
