#ifndef HTTP_H
#define HTTP_H

char * handle_http_request(char * request, int request_length,
	                       int * response_length, char * root_dir);

#endif /* HTTP_H */