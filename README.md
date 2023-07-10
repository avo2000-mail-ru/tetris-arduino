# tetris-arduino
Tetris game for Arduino

To start the game you need to:

1. Purchase an Arduino Uno or Nano. I used a $3 Chinese Nano clone with an ATMEGA328P chip, but ATMEGA168 will do as well.

2. Install a VT52 or ANSI terminal emulator on your computer, for example VT220. A good option is Tera Term.

3. Copy the files to the libraries/Tetris directory of your installed Arduino IDE program.

4. Open the sketch in the Arduino IDE program, located in the file examples/Tetris/Tetris.into. Compile and upload the sketch to your board.

5. Launch the terminal emulator, configure it on the Serial port of your board (in my case it is COM5), communication parameters 9600,8,N,1.

6. Press the Reset button of your board if the reset was not performed automatically when connecting the port in the terminal emulator.

7. Enjoy.
