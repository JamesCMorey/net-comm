#pragma once

#include <ncurses.h>

#define FDISPLAY(x, y) \
mvwprintw(wins->display, ++wins->dy, 1, x, y); \
wrefresh(wins->display);

#define DISPLAY(x) \
mvwprintw(wins->display, ++wins->dy, 1, x); \
wrefresh(wins->display);

#define CONN_NUM 1024

struct winfo {
	WINDOW *nav;
	WINDOW *display;
	WINDOW *input;
	WINDOW *tbwins[CONN_NUM];
	int dy, dx;
	int ny, nx;
	int iy, ix;
	int max_ny, max_nx;
	int rows, cols;
	char *tabs[CONN_NUM];
};

struct fds;

int mktab(struct winfo *wins, char *tbname);
int handle_input(struct winfo *wins, struct fds *fds, char *buffer);
struct winfo init_display();
int stop_display(struct winfo wins);
