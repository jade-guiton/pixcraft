# PixCraft
Yet Another Minecraft clone written in C++. Mostly for learning and practice purposes.

Basic controls:
- WASD for movement (or equivalent keys on other layouts)
- Space to jump / fly up
- Left Shift to fly down
- Escape to pause / unpause
- Enter to enter a command:
	- Escape to cancel the command
	  (no editing capabilities for now, so just retry if you make a mistake)
	- Enter to execute the command

Available commands:
- clear: clear command history
- fly: start flying
- fall: stop flying
- noclip: start flying and disable collisions
- save: save the world to data/world.bin (experimental)
- load: loads the world from that same location
- debug: enables/disables the debug printout
- antialias: enables/disables antialiasing (initially disabled, not very visible)
- further: increases render distance
- closer: decreases render distance

![Screenshot](https://i.imgur.com/qYKhC8V.png)
