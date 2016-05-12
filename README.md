
## Usage for tview

	tview command [option] [pattern] [file|dir]

tview moves the command's output into ncurses interface
It just a TUI tool,you can use some keys to operate it.

## Option:
	--help	   show help
	--list	   list the current configed command
	--ignore   ignore some dirs or files

## Keymap:
	j	move down
	k	move up
	e	edit with vim
	r	reload command's output
	q	quit

## Example:
	tview --list
	tview ls
	tview grep hello


