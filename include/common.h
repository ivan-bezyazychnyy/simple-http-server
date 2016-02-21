#ifndef COMMON_H
#define COMMON_H

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 8888
#define DEFAULT_DIR "./"

typedef struct server_parameters {
    char ip[100];
    int port;
    char directory[256];
} server_parameters;

#endif /* COMMON_H */