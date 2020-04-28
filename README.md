# PixCraft
Yet Another Minecraft clone written in C++. Mostly for learning and practice purposes.

Basic controls:
- F11 to enter/exit fullscreen mode
- WASD for movement (or equivalent keys on other layouts)
- Space to jump / fly up
- Left Shift to fly down
- Escape to pause / unpause
- Enter to enter a command:
	- Escape to cancel
	- Left/Right arrows, Backspace/Delete to edit
	- Enter to execute

Available commands:
- clear: clear command history
- fly: start flying
- fall: stop flying
- noclip: start flying and disable collisions
- save: save the world to data/world.bin (experimental)
- load: load the world from that same location
- debug: enables/disables the debug printout
- antialias: enables/disables antialiasing (initially disabled, not very visible)
- further: increases render distance
- closer: decreases render distance

![Screenshot](https://i.imgur.com/qYKhC8V.png)
