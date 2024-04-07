#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include "commands.h"
#include "display.h"

#define NUMCOMMANDS 6

static const char *COMMANDS[NUMCOMMANDS] = {	"exit", "conn", "disc", "nt",
						"send", "recv"};

enum command_code {
	EXIT, CONN, DISC, NEWTAB,
	SEND, RECV
};

int handle_command(struct winfo *wins, struct fds *fds, char *buffer)
{
	int rv;

	rv = parse_commands(buffer);

	if(rv == -1) {
		FDISPLAY("Command not found: %s", buffer);
	}
	else if (rv == EXIT) {
		return -1;
	}

	else if (rv == CONN) {

		char *hostname = strtok(&buffer[strlen("/conn")], " ");
		char *port  = strtok(NULL, " ");

		if (port == NULL || hostname == NULL) {
			DISPLAY("Server not found");
			return 0;
		}

		// TODO implement check for protocol
		int sfd = get_conn(wins, hostname, port);
		if (sfd < 0) {
			DISPLAY("Failed to connect.");
			return 0;
		}

		FD_SET(sfd, &fds->master);
		FD_SET(sfd, &fds->tcp);

		if (sfd > fds->max) {
			fds->max = sfd;
		}

		mktab(wins, hostname);
	}

	else if (rv == NEWTAB) {
		char *tbname = strtok(&buffer[strlen("/nt")], " ");
		mktab(wins, tbname);
	}
	else if (rv == SEND) {
		char *hostname = strtok(&buffer[strlen("/send")], " ");
		char *port = strtok(NULL, " ");
		char *filepath = strtok(NULL, " ");
		send_file(wins, hostname, port, filepath);
	}
	else if (rv == RECV) {
		char *filename = strtok(&buffer[strlen("/recv")], " ");
		recv_file(wins, filename);
	}

	return 0;
}

int parse_commands(char *buffer)
{
	for (int i = 0; i < NUMCOMMANDS; i++) {
		if (!strncmp(&buffer[1], COMMANDS[i], strlen(COMMANDS[i]))) {
			return i;
		}
	}

	return -1;
}

int send_file(struct winfo *wins, char *hostname, char *port, char *filepath)
{
	char buffer[4096];
	int sfd, rv;
	FILE *fd;
	char tmp = 'r';

	sfd = get_conn(wins, hostname, port);
	fd = fopen(filepath, &tmp);

	while ((rv = fread(buffer, 1, sizeof(buffer), fd)) != 0) {
		rv = send(sfd, buffer, rv, 0);
		FDISPLAY("Send %d bytes", rv);
	}

	fclose(fd);
	close(sfd);
	return 0;
}

int recv_file(struct winfo *wins, char *filename)
{
	char buffer[4096];
	int sfd, cfd, rv;
	FILE *fd;
	char tmp = 'w';

	sfd = get_serverfd(wins);
	cfd = accept(sfd, NULL, NULL);
	fd = fopen(filename, &tmp);
	while((rv = recv(cfd, buffer, sizeof(buffer), 0)) > 0) {
		fprintf(fd, "%.*s", rv, buffer);
	}
	if (rv == 0)  {
		DISPLAY("File received.");
	}
	else if (rv == -1) {
		DISPLAY("An error has occurred");
	}

	close(sfd);
	close(cfd);
	return 0;
}
