#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_PENDING 5
#define BUFF_SIZE 1024
#define DEFAULT_PORT 5000
#define DEFAULT_FILE_NAME "../cl_data.txt"

struct cmd_option {
    unsigned  int is_local;
    char* port;
    char* addr;
    char* file;
} cmd_opt;

void free_options()
{
    free(cmd_opt.port);
    free(cmd_opt.addr);
    free(cmd_opt.file);
}

int get_cmd_option(int argc, char* const argv[])
{
    int c;
    cmd_opt.is_local = 0;
    cmd_opt.port = NULL;
    cmd_opt.file = NULL;

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
    
        int option_index = 0;
        c = getopt_long (argc, argv, "li:p:f:",
                        long_options, &option_index);

        if (c == -1)
            break;

        switch (c)
        {
        case 0:
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
            cmd_opt.addr = malloc(strlen(optarg));
            strncpy(cmd_opt.addr, optarg, strlen(optarg));
            break;
        case 'p':
            printf ("option -p with value `%s'\n", optarg);
            cmd_opt.port = optarg;
            break;
        case 'f':
            printf ("option -f with value `%s'\n", optarg);
            cmd_opt.file = malloc(strlen(optarg));
            strncpy(cmd_opt.file, optarg, strlen(optarg));
            break;
        case '?':
            break;

        default:
            abort();
        }
    }

    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc){
            printf ("%s ", argv[optind++]);
        }
        
        putchar ('\n');
    }
}

void save_data_to_file(const char* buffer, unsigned int len,
                       const char* file_name)
{
    FILE* fp;
    fp = fopen(file_name, "a");
    
    if (fp == NULL)
        perror("File wasn't open");

    if (fprintf(fp, "%s", buffer) != len)
        perror("Saving failed");
    
    fclose(fp);
}

void signal_handler(int signal, siginfo_t *info, void *ptr)
{
    printf("Server received signal %d\n", signal);
    exit(0);
}

void catch_signal(const int signum)
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = signal_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(signum, &_sigact, NULL);
}

void setup_options(struct sockaddr_in* serv_addr)
{
    if (cmd_opt.port == NULL)
    {
        printf("Port not specified, using defaul port %d\n", DEFAULT_PORT);
        (*serv_addr).sin_port = htons(DEFAULT_PORT);
    }
    else
        (*serv_addr).sin_port = htons(atoi(cmd_opt.port));

    if (cmd_opt.file == NULL)
    {
        printf("File name not specified, using default file name : %s\n",
               DEFAULT_FILE_NAME);
        cmd_opt.file = malloc(strlen(DEFAULT_FILE_NAME));
        strncpy(cmd_opt.file, DEFAULT_FILE_NAME, strlen(DEFAULT_FILE_NAME));
    }

    (*serv_addr).sin_family = AF_INET;
    (*serv_addr).sin_addr.s_addr = htonl(INADDR_ANY);
}

int main(int argc, char* const argv[])
{
    pid_t pid, sid;

    pid = fork();

    if (pid < 0)
        exit(1);

    if (pid > 0)
        exit(0);

    if (setsid() < 0)
        exit(1);

    catch_signal(SIGTERM);
    catch_signal(SIGHUP);
    
    pid = fork();

    if (pid < 0)
        exit(1);

    if (pid > 0)
        exit(0);

    umask(0);

    /* Set new session */
    sid = setsid();
    if (sid < 0)
        exit(1);

    char recvBuff[BUFF_SIZE] = {0};
    int sockfd = 0, connfd = 0, n = 0;
    struct sockaddr_in serv_addr;
    unsigned int addr_len = 0;

    get_cmd_option(argc, argv);
    setup_options(&serv_addr);

    printf("PID is %ld\n", (long)getpid());
    printf("listen %s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror("Error opening socket");

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("Bind failed");

    if (listen(sockfd, MAX_PENDING) < 0)
        perror("Error listening");

    catch_signal(SIGTERM);
    catch_signal(SIGHUP);

    while (1) {
        addr_len = sizeof(serv_addr);
        connfd = accept(sockfd, (struct sockaddr*)&serv_addr, &addr_len);
        if (connfd < 0)
            perror("\nError accept");

        memset(recvBuff, 0, BUFF_SIZE*sizeof(char));
        if ((n = recv(connfd, recvBuff, BUFF_SIZE, 0)) < 0)
            perror("\nRecive failed!");

        save_data_to_file(recvBuff, n, cmd_opt.file);

        close(connfd);
        sleep(1);
    }

    free_options();
    return 0;
}
