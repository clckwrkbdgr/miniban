Miniban
=======

Miniban - a clone of Sokoban game pixel graphics and with minimalistic interface.

Installation
------------

Requirements:

* SDL2 library.
* [libchton](https://github.com/umi0451/libchthon)

Simply run `make` and put created `miniban` file to wherever you want.

THE GAME
========

The goal of the game is to place all items onto special placeholders.
Items can be pushed, but not pulled.
Item can be pushed only until hits a wall or another box, so two boxes in a row cannot be moved.
Hero can move only in four basic directions (diagonal movement available only if there are normal non-diagonal passage).
Boxes cannot be pushed diagonally.

USAGE
=====

	miniban [levelset]

`levelset` - name of file with Sokoban level collection, usually with .slc extension.
They're available online, for example [here](https://sokoban-game.com/packs) is good collection.

At start miniban will load previous game (last played level). If levelset is specified and it differs from levelset that saved, specified one is loaded instead.

CONTROLS
========

'q' or Ctrl-Q - quit.
Space - skip intelevel message.
Left or 'h' - move left.
Down or 'j' - move down.
Up or 'k' - move up.
Right or 'l' - move right.
'y', 'u', 'b', 'n' - diagonal movement (doesn't move boxes).
Shift-direction - run until wall or box are found.
X - start target mode (control cursor with usual movement keys, then press 'period' to go there).
Ctrl-Z or Backspace - undo last action.
Ctrl-R or Home - revert to the starting position.
