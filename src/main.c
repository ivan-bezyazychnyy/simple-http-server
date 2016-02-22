#define _BSD_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "common.h"
#include "cmdargs.h"
#include "logger.h"
#include "http.h"

#define LOG_FILE "/home/box/simple_http_server.log"

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

void client_handler(int client_socket, server_parameters * parameters) {
    to_log(&logger, "Client handler started: Worker %d\n", getpid());

    char buffer[1024];
    int read  = recv(client_socket, buffer, 1024, MSG_NOSIGNAL);
    while (1) {
        if (read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // should retry
            continue;
        } else if (read <= 0) {
            to_log(&logger, "Worker %d: client discontected\n", getpid());
            break;
        } else {
            to_log(&logger, "Worker %d: received request: %s",
                getpid(), buffer);
            int response_length = 0;
            char * response = handle_http_request(buffer, read, &response_length,
                parameters->directory);
            to_log(&logger, "Worker %d: sending response: %s",
                getpid(), response);
            send(client_socket, response, response_length, MSG_NOSIGNAL);
            free(response);
            close(client_socket);
            break;
        }
    }
    exit(0);
}

void run_client_handler(int client_socket, server_parameters * parameters) {
    pid_t pid = fork();
    if (pid < 0) {
        to_log(&logger, "Failed to fork client handler process:\n",
            strerror(errno));
    } else if (pid > 0) {
        // this is a server process, nothing to do
        return;
    } else {
        // this is a process to handle client
        client_handler(client_socket, parameters);
    }
}

void accept_clients_loop(server_parameters * parameters) {
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket < 0) {
        to_log(&logger, "Failed to create master socket: %s\n", strerror(errno));
        return;
    } else {
        to_log(&logger, "Master socket created.\n");
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(parameters->port);
    if (inet_aton(parameters->ip, &sa.sin_addr) == 0) {
        to_log(&logger, "Failed to convert address.\n");
        goto close_master_socket;
    }
    if (bind(master_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        to_log(&logger, "Failed to bind: %s\n", strerror(errno));
        goto close_master_socket;
    } else {
        to_log(&logger, "Binded adderss\n");
    }

    //set_nonblock(master_socket);

    if (listen(master_socket, SOMAXCONN) < 0) {
        to_log(&logger, "Failed to listen: %s\n", strerror(errno));
        goto close_master_socket;
    } else {
        to_log(&logger, "Started listening master socket for clients\n");
    }

    while(1) {
        // accept connections and handle in separate processes
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int client_socket = accept(master_socket, (struct sockaddr*) &client_addr, &client_addr_len);
        if (client_socket > 0) {
            to_log(&logger, "Accepted a new connection\n");
            run_client_handler(client_socket, parameters);
            close(client_socket);
        } else {
            if (errno == EAGAIN) {
                continue;
            } else {
                to_log(&logger, "Failed to accept client connection: %s\n",
                    strerror(errno));
                break;
            }
        }
    }
    

close_master_socket:
    if (close(master_socket) != 0) {
        to_log(&logger, "Failed to close master socket: %s\n", strerror(errno));
        return;
    } else {
        to_log(&logger, "Closed master socket\n");
    }
}

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
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

    to_log(&logger, "Simple http server with pid %d started logging.\n", getpid());

    /* lets remove dead processes */
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        to_log(&logger, "Failed to set up SIGCHLD handler: %d\n",
            strerror(errno));
        goto exit_with_logging;
    }

    accept_clients_loop(&server_parameters);

    sleep(3);

    to_log(&logger, "Simple http server is stopping.\n");

exit_with_logging:
    logger_close(&logger);


exit:
	return 0;
}
