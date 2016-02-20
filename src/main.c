#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "cmdargs.h"
#include "logger.h"

#define LOG_FILE "/home/ivan/simple_http_server.log"

logger_t logger;

int daemonize() {
    pid_t pid = fork();
    if (pid < 0) {
        printf("Failed to fork: %d\n", strerror(errno));
        return -1;
    } else if (pid > 0) {
        printf("Daemon process with pid %d created.\n", pid);
        exit(0);
    } else {
        /* child (daemon) process is here */

        /* Change the file mode mask */
        umask(0);         

        /* Create a new SID for the child process */
        pid_t sid = setsid();
        if (sid < 0) {
            printf("Failed to set the session id: %s\n", strerror(errno));
            return -1;
        }
            
        /* Change the current working directory */
        if ((chdir("/")) < 0) {
            printf("Failed to change the current working directory: %s\n",
                strerror(errno));
            return -1;
        }
            
        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
}

int main(int argc, char **argv) {
	printf("Starting Simple Http Server...\n");

    struct server_parameters server_parameters;
    if (parse_arguments(argc, argv, &server_parameters) < 0) {
        printf("Failed to parse cmd arguments\n");
        goto exit;
    }
    printf("Server parameters: ");
    print_server_parameters(&server_parameters);
    printf("\n");

    if (daemonize() < 0) {
        printf("Failed to become daemon process.\n");
        goto exit;
    }

    if (logger_init(&logger, LOG_FILE) < 0) {
        exit(-2);
    }

    to_log(&logger, "Simple http server with pid %d started logging.", getpid());


    sleep(10);

exit_with_logging:
    logger_close(&logger);


exit:
	return 0;
}
