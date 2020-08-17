# PixCraft
Yet Another Minecraft clone written in C++. Mostly for learning and practice purposes.

To compile this, you will need to install the Flatbuffers compiler and development files, as well as the the libraries GLFW3, GLM, Freetype. On Linux, this can usually be done with the following command:
`sudo apt install libglfw3 libglfw3-dev libglm-dev libfreetype6 libfreetype6-dev`

You will probably also need to change the options at the top of the Makefile to match your current OS and installation.

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
- load: loads the world from that same location
- debug: enables/disables the debug printout
- antialias: enables/disables antialiasing (initially disabled, not very visible)
- further: increases render distance
- closer: decreases render distance

![Screenshot](https://i.imgur.com/qYKhC8V.png)
