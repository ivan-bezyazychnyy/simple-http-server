#ifndef COMMON_H
#define COMMON_H

#define DEFAULT_IP "localhost"
#define DEFAULT_PORT 8888
#define DEFAULT_DIR "./"

struct server_parameters {
    char ip[100];
    int port;
    char directory[256];
};

#endif /* COMMON_H */