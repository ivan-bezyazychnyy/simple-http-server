#ifndef CMDARGS_H
#define CMDARGS_H

int parse_arguments(int argc, char **argv,
                    struct server_parameters * server_parameters);

void print_server_parameters(struct server_parameters * server_parameters);

#endif /* CMDARGS_H */