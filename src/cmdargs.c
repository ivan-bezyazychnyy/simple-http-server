#ifndef CMDARGS_CPP
#define CMDARGS_CPP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  
#include <unistd.h>
#include <getopt.h>

#include <common.h>

int parse_arguments(int argc, char **argv,
                    server_parameters * server_parameters) {

    /* Assign default values fitst */
    strcpy(server_parameters->ip, DEFAULT_IP);
    server_parameters->port = DEFAULT_PORT;
    strcpy(server_parameters->directory, DEFAULT_DIR);

    int option = 0;
    while ((option = getopt(argc, argv,"h:p:d:")) != -1) {
        switch (option) {
            case 'h':
                if (strlen(optarg) >= 100) {
                    printf("Host name (ip address) is too long. "\
                        "(max length is 99 characters");
                    return -1;
                }
                strcpy(server_parameters->ip, optarg);
                break;
            case 'p':
                server_parameters->port = atoi(optarg);
                if (server_parameters->port == 0) {
                    printf("Failed to parse port argument.");
                    return -1;
                }
                break;
            case 'd':
                if (strlen(optarg) >= 100) {
                    printf("Directory path is too long. "\
                        "(max length is 255 characters");
                    return -1;
                }     
                strcpy(server_parameters->directory, optarg);               
                break;
        }
    }

    return 0;
}

void print_server_parameters(server_parameters * server_parameters) {
    printf("ip=%s, port=%d, dir=%s",
        server_parameters->ip, server_parameters->port,
        server_parameters->directory);
}

#endif /* CMDARGS_CPP */