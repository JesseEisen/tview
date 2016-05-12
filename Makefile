all:
	gcc -g src/main.c src/find_render.c src/util.c src/ls_render.c src/nwin.c src/grep_render.c -o bin/tview -I./inc -lncurses

install:
	cp ./bin/tview /usr/local/bin/

clean:
	rm -rf bin/tview



