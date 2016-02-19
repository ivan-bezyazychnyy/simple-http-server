#include <stdio.h>

#include "common.h"
#include "cmdargs.h"

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

    

exit:
	return 0;
}
