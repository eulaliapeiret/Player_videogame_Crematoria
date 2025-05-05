This project includes the development of a strategic game player named AILaLali, 
capable of navigating a game board, avoiding enemies, and maximizing occupied territory. 

1. Running the first game
Here, we will explain how to run the game under Linux, but it should work under Windows, Mac, FreeBSD, OpenSolaris, …
You only need a recent g++ version, make installed in your system, plus a modern browser like Firefox or Chrome.

1.1. Open a console and cd to the directory where you extracted the source code.

1.2. If, for example, you are using Linux, run:

cp AIDummy.o.Linux AIDummy.o

cp Board.o.Linux Board.o

1.3. Run

make all

to build the game and all the players. Note that Makefile identifies as a player any file matching AI\*.cc.

1.4. This creates an executable file called Game. This executable allows you to run a game using a command like:

./Game Demo Demo Demo Demo -s 30 -i default.cnf -o default.res

This starts a match, with random seed 30, of four instances of the player Demo, in the board defined in default.cnf. The output of this match is redirected to default.res.

1.5. To watch a game, open the viewer file viewer.html with your browser and load the file default.res.

Use

./Game --help

to see the list of parameters that you can use. Particularly useful is

./Game --list

to show all the recognized player names.
