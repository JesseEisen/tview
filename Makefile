tview:
	gcc -g src/main.c src/util.c src/ls_render.c src/nwin.c -o bin/tview -I./inc -lncurses

clean:
	rm ./bin/tview
