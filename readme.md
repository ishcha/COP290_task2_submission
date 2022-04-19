# IITD Champ: A maze game based in IIT Delhi
Welcome to the documentation for _IITD Champ_. Please find the game installation instructions in this document. The references used for developing this game are also in this document. 

## Installation instructions:
The instructions presented below are tested on an Ubuntu 20.04 machine. I assume that GCC and make are already installed in the machine. 

1. Clone the Github repository of the game. It contains the code for the single and dual-player games in separate folders.
2. Extract the codebase in the desired location.
3. Install SDL2, SDL2-ttf (for TTF fonts), and SDL2-mixer (for audio), by the following instructions:
   1. In a terminal (with any directory as CWD), execute ```sudo su``` to enter the super user mode.
   2. Install SDL2 using the command ```apt-get install libsdl2-dev``` on the terminal.
   3. Install SDL2-ttf using the command ```apt-get install libsdl2-ttf-dev``` on the terminal.
   4. Install SDL2-mixer using the command ```apt-get install libsdl2-mixer-dev``` on the terminal.
4. For the dual-player game, install ENet by the following instructions:
   1. Download the ENet source code from ![here](http://enet.bespin.org/Downloads.html)
   2. Install ENet using the commands ```./configure && make && make install```
5. For the dual player game, set the LD_LIBRARY_PATH environment variable in each terminal used to execute the code. It is set as ```export LD_LIBRARY_PATH=<path of folder containing libenet.so.7>:$LD_LIBRARY_PATH```. 
6. Execute ```make``` in each folder (in the folder of the single player game, and in the server and client folders of the dual-player game) to compile the code. 
7. Run the game by running the executable ```./main``` created in each folder (in the folder of the single player game, and in the server and client folders of the dual-player game). 


## References
1. https://lazyfoo.net/tutorials/SDL/index.php
2. http://enet.bespin.org/index.html
3. https://github.com/schuay/pacman
4. https://github.com/DarkMuffinJoe/Pacman