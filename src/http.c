#ifndef HTTP_C
#define HTTP_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "logger.h"

extern logger_t logger;

char * TEMPLATE_200 =
	"HTTP/1.0 200 OK\r\n"
//	"Content-length: %d\r\n"
	"Connection: close\r\n"
	"Content-Type: text/html\r\n\r\n";
char * TEMPLATE_404 = "HTTP/1.0 404 Not Found\r\n"
	"Content-Type: text/html\r\n\r\n";

char * create_404_response() {
	char * response = (char *) malloc(strlen(TEMPLATE_404) + 1);
	strcpy(response, TEMPLATE_404);
	return response;
}

char * handle_http_request(char * request, int request_length,
	                       int * response_length, char * root_dir) {
	

	char cmd[100];
	char url[200];
	char full_path[1024];

	for (int i = 0; i < request_length; ++i) {
		if (request[i] == '?') {
			request[i] = '\0';
		}
	}

	sscanf(request, "%s %s", &cmd, &url);
	strcpy(full_path, root_dir);
	strcat(full_path, url);

	int file_size = 0;
	struct stat st; 
    if (stat(full_path, &st) < 0) {
    	to_log(&logger, "Failed to get file size:%s\n", strerror(errno));
    	char * response = create_404_response();
    	*response_length = strlen(response) + 1;
    	return response;
    }
    file_size = st.st_size;

    FILE * file = fopen(full_path, "r");
    if (file == NULL) {
    	to_log(&logger, "Failed to open file:%s\n", strerror(errno));
    	char * response = create_404_response();
    	*response_length = strlen(response) + 1;
    	return response;
    }

	char * header = (char *) malloc(strlen(TEMPLATE_200) + 100);
	sprintf(header, TEMPLATE_200);
    char * response = (char *) malloc(file_size + strlen(header) + 100);
    strcpy(response, header);
    free(header);
    int pos = strlen(response);
    int c;
    while ((c = fgetc(file)) != EOF) {
    	response[pos] = c;
    	pos++;
    }
    response[pos] = '\0';

    *response_length = strlen(response);
    return response;
}

#endif /* HTTP_C */ 