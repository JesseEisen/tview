tview:
	gcc -g src/main.c src/util.c src/ls_render.c -o bin/tview -I./inc

clean:
	rm ./bin/tview
