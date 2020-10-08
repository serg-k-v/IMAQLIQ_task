#include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5000
#define MAX_PENDING 5
#define BUFF_SIZE 1024
#define SAVE_DATA_TO "client_data.txt"

struct cmd_option {
    unsigned  int is_local;
    char* port;
    char* addr;
    char* file;
} cmd_opt;

int get_cmd_option(int argc, char* const argv[])
{
    int c;
    cmd_opt.is_local = 0;

    while (1)
    {
        static struct option long_options[] =
        {
            {"local",  no_argument,       0, 'l'},
            {"addr",   required_argument, 0, 'i'},
            {"port",   required_argument, 0, 'p'},
            {"file",   required_argument, 0, 'f'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "li:p:f:",
                        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf ("option %s", long_options[option_index].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            break;
        case 'l':
            cmd_opt.is_local = 1;
            puts ("option -l\n");
            break;
        case 'i':
            printf ("option -i with value `%s'\n", optarg);
            cmd_opt.addr = malloc(sizeof(optarg)*sizeof(char));
            strncpy(cmd_opt.addr, optarg, sizeof(optarg));
            break;
        case 'p':
            printf ("option -p with value `%s'\n", optarg);
            cmd_opt.port = optarg;
            break;
        case 'f':
            printf ("option -f with value `%s'\n", optarg);
            cmd_opt.file = malloc(sizeof(optarg)*sizeof(char));
            strncpy(cmd_opt.file, optarg, sizeof(optarg));
            break;
        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort();
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc){
            printf ("%s ", argv[optind++]);
        }
        
        putchar ('\n');
    }
}

void save_data_to_file(const char* buffer, unsigned int len, const char* file_name)
{
    FILE* fp;

    fp = fopen(file_name, "a");
    
    if (fp == NULL)
        perror("File wasn't open");

    // printf("len buff: %ld == %d", fprintf(fp, "%s", buffer), len);
    // printf("Buff: %s", buffer);

    if (fprintf(fp, "%s", buffer) != len)
        perror("Saving failed");

    fclose(fp);
}

int main(int argc, char* const argv[])
{
    char recvBuff[BUFF_SIZE] = {0};
    int sockfd = 0, connfd = 0, n = 0;
    struct sockaddr_in serv_addr;
    unsigned int addr_len = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("Error opening socket");

    // memset(&serv_addr, '0', sizeof(serv_addr));
    // memset(sendBuff, '0', sizeof(sendBuff));
    
    get_cmd_option(argc, argv);

    printf("is local : %d, addr : %s, port %s\n", cmd_opt.is_local, cmd_opt.addr, cmd_opt.port);
    printf("file name: %s\n", cmd_opt.file);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(cmd_opt.port));

    printf("listen %s:%d\n", inet_ntoa(serv_addr.sin_addr), atoi(cmd_opt.port));

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("Bind failed");

    if (listen(sockfd, MAX_PENDING) < 0)
        perror("Error listening");

    while (1) {
        addr_len = sizeof(serv_addr);
        connfd = accept(sockfd, (struct sockaddr*)&serv_addr, &addr_len);
        if (connfd < 0)
            perror("Error accept");

        if(fputs(recvBuff, stdout) == EOF)
            printf("\n Error : Fputs error\n");

        if ((n = recv(connfd, recvBuff, BUFF_SIZE, 0)) < 0)
            perror("recv failed!");
        else
            printf("Buff size: %d", n);

        save_data_to_file(recvBuff, n, cmd_opt.file);

        close(connfd);
        sleep(1);
    }

    return 0;
}
