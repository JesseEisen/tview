
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

## Install

First you need to install the `ncurses` library.

+ Centos

	yum install ncurses

+ OS X

    brew tap homebrew/dupes

	brew install ncurses

Other System can use the package tool to install this library.

Or install ncurses by hand:

	go to http://ftp.gnu.org/gnu/ncurses/ download the ncurses package    
	$ ./confure 
	$ make
	$ sudo make install
	
Second if you on linux, you can just run `make`.if you on Mac, you can use the
`make -f makefile.macos`. After this, you can use the `make install` 

That's done, Hope you enjoy it~

