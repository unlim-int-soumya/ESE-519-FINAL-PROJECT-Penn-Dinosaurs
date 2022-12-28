# ESE-519-Final-Project
Team Dino !

Where are we?

Stage 1: Run the example code of VGA 
Problem 1: No VGA signal was detected on the computer screen
Debugging+Solution: 
Use the oscilloscope to test signal→ signal is transmitted properly.
Change the DVI to VGA cable to the normal VGA cable.
Result: VGA signal detected
Problem 2:VGA signal detected, nothing displayed
Debugging+solution: 
Used 330 k ohm instead of 330 ohm.
Solution: Replace all the resistors to 330 ohm( 2*150 ohm,3*10 ohm, connected in series)
	Result: The example code is working
VGA connection diagram:



Stage 2: Make the dino game code working
	Expected result: After the pushbotton is pressed, the dino game will begin. Then, use the pushbotton to control the dino jump in front of the barrier. Different words will be displayed based on the performance of the player.
	Problem 1: The pushbotton is not working
 	Debugging and Solution: Wrong connecting way of pushbotton—correct the connection.
	Problem 2: There is some display issue
	Debugging and Solution: Solved using the vga_graphics.c functions.
	Problem 3: Adding Sound Effects.
	Debugging and Solution: -Working on the problem. Currently we are able to generate any mp4 audio to speaker PWM output. But that is not working in dino game. So currently working on it.

Stage 3: Discuss with the Professor about the idea change
Problem 1:
In this step, we first considered using the Arducam on the Pico4ML board to detect humans. However, this board's storage capacity was only 2MB. And after we installed Circuitpython, this memory space was reduced to 0.9MB. Then we decided to implement this part of the project using the Raspberry Pi 4 board in order to get some functionality working before the demo day.  

Stage 4: Run the ST7735 LCD example code, using C. May consider using PIO too.
	Problem 1: There is blacklight in the LCD, which means it is functioning well, but no images displayed as expected.
	Debugging and Solution: The ST7735 used in the example has a different Pin number, which means the jump wire connection will be different.
Find the pin function distribution of the ST7735 used in the video and the Pin value defined in the code to determine the wire connection.

Find the Pin distribution of the ST7735 we are going to use, connect it based on the wire connection.


Stage 5: custom the LCD display to achieve user interaction. (in process)
