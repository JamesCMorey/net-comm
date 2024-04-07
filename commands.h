#pragma once

#include "net.h"

int recv_file(struct winfo *wins, char *filename);
int send_file(struct winfo *wins, char *hostname, char *port, char *filepath);
int handle_command(struct winfo *wins, struct fds *fds, char *buffer);
int parse_commands(char *buffer);
