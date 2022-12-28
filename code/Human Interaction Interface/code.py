
# Write your code here :-)
# Write your code here :-)
# Raspberry Pi Pico

#Importing all the required libraries
import board,busio
from time import sleep
from adafruit_st7735r import ST7735R        #Also imported library for st7735 LCD
import displayio
import terminalio
from adafruit_display_text import label

import board
import audiomp3         #library for audio
import audiopwmio       #library for PWM of audio


#Declaring the raspberry pi Pico pin names for interfacing with ST7735R display
mosi_pin = board.GP11
clk_pin = board.GP10
reset_pin = board.GP17
cs_pin = board.GP18
dc_pin = board.GP16

displayio.release_displays()        #Discharges any currently active displays

#configuration of clock and MOSI pins for SPI
spi = busio.SPI(clock=clk_pin, MOSI=mosi_pin)

display_bus = displayio.FourWire(spi, command=dc_pin, chip_select=cs_pin, reset=reset_pin)

#defining width and height of ST7735R display 
display = ST7735R(display_bus, width=128, height=160, bgr = True)

#Handle a cluster of sprites and groups and their relationships.
splash = displayio.Group()

#used for displaying purpose
display.show(splash)

# Making a bitmap 128 pixels wide by 160 pixels high, with 1 potential values for each pixel.
color_bitmap = displayio.Bitmap(128, 160, 1)

#making a palette of one colors
color_palette = displayio.Palette(1)

#defining the palette color value i.e Bright Green
color_palette[0] = 0x00FF00  # Bright Green

#Making a TileGrid object with 2D pixels
bg_sprite = displayio.TileGrid(color_bitmap, pixel_shader=color_palette, x=0, y=0)

#appending the bg_sprite object to the splash
splash.append(bg_sprite)

# Draw a smaller inner rectangle
inner_bitmap = displayio.Bitmap(118, 150, 1)    
inner_palette = displayio.Palette(1)
inner_palette[0] = 0x000000  # Black
inner_sprite = displayio.TileGrid(inner_bitmap, pixel_shader=inner_palette, x=5, y=5)
splash.append(inner_sprite)

# Drawing the Initial label "Hello,there! What's your name?. Type it in keyboard..."
text_group = displayio.Group(scale=1, x=11, y=24)   ##Handle a cluster of sprites and groups and their relationships.
text = "Hello,there! \nWhat's your \nname?. Type it in \nkeyboard..."
text_area = label.Label(terminalio.FONT, text=text, color=0xFFFFFF) 
text_group.append(text_area)  # Subgroup for text scaling
splash.append(text_group)

#Takes the input from the user
text=input()

#If any input given, It 
if(text):
    splash.pop()    #clear the content on the screen written before
    text_group = displayio.Group(scale=1, x=11, y=24)
    text_area= label.Label(terminalio.FONT, text= text, color=0xFFFFFF)
    text_group.append(text_area)  # Subgroup for text scaling
    splash.append(text_group)
else:
    #Showing input not detected in message if no input given
    splash.pop()    
    text_group = displayio.Group(scale=1, x=11, y=24)
    text="Input not detected"
    text_area= label.Label(terminalio.FONT, text=text, color=0xFFFFFF)
    text_group.append(text_area)  # Subgroup for text scaling
    splash.append(text_group)

#clearing the screen once the last task done
#The folloing part was used to greet the user with its name and asking their choice whether 
# they plan to listen to the music or not. 
splash.pop()
text_group = displayio.Group(scale=1, x=11, y=24)
text = "Hello, " + text + "!"+"\nDo you want to play the \n music. Type it in \nkeyboard[y/n]..."
text_area = label.Label(terminalio.FONT, text=text, color=0xFFFFFF)
text_group.append(text_area)  # Subgroup for text scaling
splash.append(text_group)

#Again receiving input from the user
text=input()

#If the user answers yes, it displays music options with three options from which the user can choose.
if(text=="y"):
    #Defining the Raspberry Pi Pico pin GP0 for audio output
    audio = audiopwmio.PWMAudioOut(board.GP0)
    splash.pop()
    text_group = displayio.Group(scale=1, x=11, y=24)
    text = "Which song would you like to play" + "!"+"\nWe have three choices.\n(1)Under Your\nInfluence\n(2)Shape of You\n(3)Blinding Lights\n Type it in \nkeyboard[1/2/3]..."
    text_area = label.Label(terminalio.FONT, text=text, color=0xFFFFFF)
    text_group.append(text_area)  # Subgroup for text scaling
    splash.append(text_group)

    #It again takes input from the user
    ainp = input()
    #Creating a list of mp3 files, for each file name as a string that contains the.mp3 extension.
    #if user selects 1 the it would play "song_chris.mp3"
    if ainp == "1":
        mp3files=["song_chris.mp3"]
    #if user selects 2 the it would play "shape.mp3"
    if ainp == "2":
        mp3files=["shape.mp3"]
    #if user selects 3 the it would play "blinding_night.mp3"
    if ainp == "3":
        mp3files=["blinding_night.mp3"]
    #mp3files = ["slow.mp3", "happy.mp3"]


    #The following statement was used to open the music file in the read binary mode
    mp3 = open(mp3files[0], "rb")

    #Using the mp3 decoder, decoding the mp3 file is done
    #Creating the decoder object by opening the first MP3 file in the list.
    decoder = audiomp3.MP3Decoder(mp3)

    for filename in mp3files:
        decoder.file = open(filename, "rb")
        audio.play(decoder)
        splash.pop()

        #The following code was used to display "Playing the music..." message
        text_group = displayio.Group(scale=1, x=11, y=24)
        text = "Playing the music..."
        text_area = label.Label(terminalio.FONT, text=text, color=0xFFFFFF)
        text_group.append(text_area)  # Subgroup for text scaling
        splash.append(text_group)

        #print("Playing:", filename)
        # Pass or do nothing while the music is playing.
        while audio.playing:
            pass

    #While playing the music it gives "Playing the music..." message to the user
    splash.pop()
    text_group = displayio.Group(scale=1, x=11, y=24)
    text = "Playing the music..."
    text_area = label.Label(terminalio.FONT, text=text, color=0xFFFFFF)
    text_group.append(text_area)  # Subgroup for text scaling
    splash.append(text_group)

else:
    #If any issues come while playing the music it would display error message showing "Error in playing music"
    splash.pop()
    text_group = displayio.Group(scale=1, x=11, y=24)
    text = "Error in playing music"
    text_area = label.Label(terminalio.FONT, text=text, color=0xFFFFFF)
    text_group.append(text_area)  # Subgroup for text scaling
    splash.append(text_group)

while (1):
    pass

