tview:
	gcc -g src/main.c src/find_render.c src/util.c src/ls_render.c src/nwin.c src/grep_render.c -o bin/tview -I./inc -lncurses

clean:
	rm ./bin/tview
