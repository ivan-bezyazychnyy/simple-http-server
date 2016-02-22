#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "http.h"
#include "logger.h"

#define LOG_FILE "/home/ivan/simple_http_server.log"

char * dir = "/home/ivan/study/simple-http-server";

logger_t logger;

int main(int argc, char ** arhv) {
	if (logger_init(&logger, LOG_FILE) < 0) {
        exit(-2);
    }

	printf("Enter http request:\n");

	char request[1024];
	fgets(request, 1024, stdin);
	printf("Got request:\n%s\n", request);

	int response_length = 0;
	char * response = handle_http_request(request, strlen(request),
		&response_length, dir);
	printf("Response:\n%s\n", response);
	free(response);

	logger_close(&logger);

	return 0;
}