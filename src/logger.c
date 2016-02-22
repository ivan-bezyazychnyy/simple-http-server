#ifndef LOGGER_C
#define LOGGER_C

#define NO_LOG

#include "logger.h"

#include <stdarg.h>

int logger_init(logger_t *logger, char *log_file) {
#ifdef NO_LOG
    return 0;
#else
    logger->log_file = fopen(log_file, "w+");
    if (logger->log_file == NULL) {
        return -1;
    }

    if (pthread_mutex_init(&logger->mutex, NULL) != 0) {
        fclose(logger->log_file);
        return -1;
    }

    return 0;
#endif
}

int logger_close(logger_t *logger) {
#ifdef NO_LOG
    return 0;
#else
    fclose(logger->log_file);
    pthread_mutex_destroy(&logger->mutex);
#endif
}

void to_log(logger_t *logger, const char * format, ... ) {
#ifdef NO_LOG
    return;
#else

    pthread_mutex_lock (&logger->mutex);

    va_list args;
    va_start(args, format);
    vfprintf(logger->log_file, format, args);
    va_end(args);

    fflush(logger->log_file);

    pthread_mutex_unlock (&logger->mutex);
#endif
}

#endif /* LOGGER_C */