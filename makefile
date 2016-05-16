TVIE_SRCS = \
	   ./src/find_render.c \
	   ./src/grep_render.c \
	   ./src/ls_render.c \
	   ./src/nwin.c \
	   ./src/util.c \

MAIN_SRC = \
		./src/main.c \
		

SRC = \
	 $(TVIE_SRCS) \
	 $(MAIN_SRC) \

LINK_C_DIRS = \
		-I./inc \

CC = gcc 

LCFLAGS += -g -Wall -O1
LCFLAGS += -Wuninitialized

LINKFLAG += -lncurses

tview:
	$(CC) $(LCFLAGS) $(SRC) -o bin/tview $(LINK_C_DIRS) $(LINKFLAG)

clean:
	rm -rf bin/tview
	if [ -z core* ];then
		rm -rf core*
	fi

install:
	cp ./bin/tview /usr/local/bin/view
	cp r_view /usr/local/bin/tview





