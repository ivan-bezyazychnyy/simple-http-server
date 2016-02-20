#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <pthread.h>

typedef struct logger_t {
    FILE *log_file;
    pthread_mutex_t mutex;

} logger_t;


int logger_init(logger_t *logger, char *log_file);

int logger_close(logger_t *logger);

void to_log(logger_t *logger, const char * format, ... );

#endif /* LOGGER_H */

