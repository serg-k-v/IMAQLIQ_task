#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


#define PORT 5000
#define MAX_PENDING 5
#define BUFF_SIZE 1024

// const char* get_data_for_send() {
//     FILE* fp;
//     char c;
//     int cur_size = 0;
//     int size = 255;
//     int new_size = size;
//     char *buffer = malloc(size*sizeof(char));

//     fp = fopen("data_for_sending.txt", "r");
    
//     if (fp == NULL)
//         perror("File wasn't open");

//     while (fscanf(fp, "%c", &c) != EOF){
//         if (cur_size == size){
//             new_size +=size;
//             buffer = (char*)realloc(buffer, new_size*sizeof(char));
//             if(buffer == NULL){
//                 free(buffer);
//                 perror("Error (re)allocating memory");
//             }
//         }
//         buffer[cur_size++] = c;
//     }
//     fclose(fp);
//     return buffer;
// }

void save_data_to_file(const char* buffer, unsigned int len)
{
    FILE* fp;

    fp = fopen("data_for_sending.txt", "a");
    
    if (fp == NULL)
        perror("File wasn't open");

    if (fprintf(fp, "%s", buffer) != len)
        perror("Saving failed");

    fclose(fp);
}

int main(int argc, char const *argv[])
{
    char* data = get_data_for_send();
    char recvBuff[BUFF_SIZE];
    int sockfd = 0, connfd = 0, n = 0;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("Error opening socket");

    // memset(&serv_addr, '0', sizeof(serv_addr));
    // memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port   = htons(PORT);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("Bind failed");

    if (listen(sockfd, MAX_PENDING) < 0)
        perror("Error listening");

    while (1) {
        
        // clntLen = sizeof(echoClntAddr);
        // accept(servSock,(struct sockaddr *)&echoClntAddr,&clntLen))

        if (connfd = accept(sockfd, (struct sockaddr*)NULL, NULL) < 0)
            perror("Error accept");

        while ( (n = recv(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
        {
            recvBuff[n] = 0;
            save_data_to_file(recvBuff, n);
            // if(fputs(recvBuff, stdout) == EOF)
            // {
            //     printf("\n Error : Fputs error\n");
            // }
        }
        
        close(connfd);
        sleep(1);
    }


    printf("%s", data);
    return 0;
}
