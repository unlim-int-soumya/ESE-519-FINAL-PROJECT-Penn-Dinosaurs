/*
 * By Satyajeet Das
 * Dino Game for PICO RP2040
 */
/*
*Level 1: SPEED = 4 & VARY = 0.00005
*Level 2: SPEED = 5 & VARY = 0.0001
*Level 3: SPEED = 6 & VARY = 0.0003
level 1 -> PIN 6
level 2 -> PIN 4
level 1 -> PIN 5
JUMP PIN -> PIN 2
PWM audio -> PIN 3
*/

#include <stdio.h> //The standard C library
#include <stdlib.h> //The standard C library
#include "hardware/pio.h" //The hardware PIO library
#include "hardware/dma.h" //The hardware DMA library
#include <math.h> //The standard math library
#include "pico/stdlib.h" //Standard library for Pico
#include "pico/time.h" //The pico time library
#include "pico/types.h" //The pico time library
#include <time.h>// Time library
#include "hardware/gpio.h" //The hardware GPIO library
#include "Bitmaps.h" //The bitmaps library
#include "vga_graphics.h" //The graphics library
#include "pwmsnd.h"//pwm audio
#include "jump.h"//jump sound
#include "gameover.h"// gameover sound

#define JUMP_PIN 2 //Pin attached to the Push Button
#define SCORE_X_OFFSET1 5 //x offset for text
#define SCORE_X_OFFSET2 150 //x offset for variables
#define SCORE_Y_OFFSET1 5 //y offset for score
#define SCORE_Y_OFFSET2 30 //y offset for high score
#define PWMSND_GPIO	1 // pin to audio

#define OBS_W_0 10 //Width of obstacle type 0
#define OBS_H_0 20 //Height of obstacle type 0
#define OBS_W_1 15 //Width of obstacle type 1
#define OBS_H_1 30 //Height of obstacle type 1
#define OBS_W_2 30 //Width of obstacle type 2
#define OBS_H_2 20 //Height of obstacle type 2
#define GROUND_SIZE 20 //Number of elements in the ground array
#define GROUND_HEIGHT 60 //Height of the ground above the base
#define HEIGHT 480 //Height of the VGA screen
#define WIDTH 640 //Width of the VGA screen
#define GRAVITY 1.2 //The gravity value
#define JUM_VEL 15 //Velocity with which the dino jumps
#define RUNNER_FRAMES 10 //Used for alternating between 2 dino running bitmaps
#define JUMP_PIN 2 //The pin to attach the pushbutton to

float SPEED; // decration of variable
float VARY; // decration of variable
int level; // decration of variable

unsigned char exTime, obsType; //The extra time to maintain 30fps and the obstacle type
unsigned int score = 0, highScore = 0; //Score and high score
unsigned int score1 = 0, highScore1 = 0; //Score and high score
unsigned int score2 = 0, highScore2 = 0; //Score and high score
unsigned int score3 = 0, highScore3 = 0; //Score and high score
volatile char resetGame = 1; //Flag to indicate game reset
char runner = 0; //The variable to store which how many frames have elapsed since the running bitmap was changed

char scoreBuffer[40], highScoreBuffer[40]; //The buffer to display score and high score as strings
char scoreBuffer1[40], highScoreBuffer1[40]; //The buffer to display score and high score as strings
char scoreBuffer2[40], highScoreBuffer2[40]; //The buffer to display score and high score as strings
char scoreBuffer3[40], highScoreBuffer3[40]; //The buffer to display score and high score as strings


inline int randomRange(int min, int max){ //Function to generate a random number between min and max
    return (rand() % (max - min)) + min;
}
struct Ground{ //Ground structure to store each small ground element with x-coordinate, y-coordinate and width
    int x, y, w;
};

struct Obstacle{ //Obstacle structure to store the x-coordinate, y-coordinate and width of the obstacle
    int x, w, h;
};
struct Player{ //Player structure to store the x-coordinate, y-coordinate, width, height, velocity, jumped and alive status of the dino
    int x, y, w, h, alive;
    volatile int vy, jumped;
};


struct Ground ground[GROUND_SIZE]; //Ground array
struct Obstacle obstacle; //The obstacle object
struct Player myPlayer; //The player object
#define _width 640 // width of screen
#define _height 480 // height of screen


void DinoJump(uint gpio, uint32_t events) { //The dino jump interrupt handler
    if(!myPlayer.y & myPlayer.alive){ //If the dino is alive and at ground level (y is the y-coordinateand gate)
        myPlayer.jumped = 1; //Change the jumped status to 1
        myPlayer.vy = JUM_VEL; //Change the velocity
        PlaySound(jumpsnd, sizeof(jumpsnd)); // play jump sound     
    }
    else if(!myPlayer.alive){ //If the dino is dead
        resetGame = 1; //Set the reset flag
    }
}

int main(){ //The program running on core 0
    stdio_init_all(); //Initialize all of the present standard stdio types that are linked into the binary
    initVGA(); //Initialize the VGA screen and functions
    PWMSndInit(); // initialize PWM
    srand(1); //Random seed
    int level; // declaration of level

    // level selector switch declaration
    gpio_set_dir(6,false);
    gpio_pull_down(6);
    gpio_set_dir(4,false);
    gpio_pull_down(4);
    gpio_set_dir(5,false);
    gpio_pull_down(5);
    
    int i; //Counter variable
    myPlayer.x = 30; //Set the dino x-coordinate to be 30
    myPlayer.y = 0; //Set the dino y-coordinate to be 0
    myPlayer.w = 22; //Set the dino width to be 22
    myPlayer.h = 25; //Set the dino height to be 25
    myPlayer.vy = 0; //Set the dino velocity to be 0
    myPlayer.alive = 1; //Set the dino alive flag to be true
    
    // obstacle type random generator
    obsType = randomRange(0, 3); //Get a random obstacle type
    obstacle.x = WIDTH; //Set its x-coordinate to the right most edge
    switch(obsType){ //Switch based on the obstacle type
        //Obstacle type 0
        case 0: obstacle.w = OBS_W_0;
                obstacle.h = OBS_H_0;
                break;
        //Obstacle type 1
        case 1: obstacle.w = OBS_W_1;
                obstacle.h = OBS_H_1;
                break;
        //Obstacle type 2
        case 2: obstacle.w = OBS_W_2;
                obstacle.h = OBS_H_2;
                break;
    }

    for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
        ground[i].x = randomRange(0, WIDTH); //Assign them a random x-coordinate
        ground[i].y = randomRange(HEIGHT - GROUND_HEIGHT + 10, HEIGHT); //Assign them a random y-coordinate
        ground[i].w = randomRange(3, 10); //Assign them a random width
    }

//Convenience function which performs multiple GPIO IRQ related initializations.
//This method is a slightly eclectic mix of initialization, that:
//Updates whether the specified events for the specified GPIO causes an interrupt on the calling core based on the enable flag.
//Sets the callback handler for the calling core to callback (or clears the handler if the callback is NULL).
//Enables GPIO IRQs on the current core if enabled is true.
//This method is commonly used to perform a one time setup, and following that any additional IRQs/events are enabled via gpio_set_irq_enabled. 
//All GPIOs/events added in this way on the same core share the same callback; for multiple independent handlers for different GPIOs you should use gpio_add_raw_irq_handler and related functions.
    
    gpio_set_irq_enabled_with_callback(JUMP_PIN, GPIO_IRQ_EDGE_FALL, true, &DinoJump); //Attach the callback to the specified GPIO


    sleep_ms(15000); //Sleep for 15 seconds for letting people get ready

    setTextColor(BLUE); //Set text colour to white
    setTextSize(2); //Set text size to 2
    setCursor(SCORE_X_OFFSET1, SCORE_Y_OFFSET1); //Set the cursor
    writeString("Score: "); //Print the text
    setTextColor(BLUE); //Set text colour to white
    setTextSize(2); //Set text size to 2
    setCursor(SCORE_X_OFFSET1, SCORE_Y_OFFSET2); //Set the cursor
    writeString("High Score: "); //Print the text


    setTextColor(MAGENTA); //Set text colour to white
    setTextSize(2); //Set text size to 2
    setCursor(300, 15); //Set the cursor
    writeString("Welcome to DINO Game!"); //Print the text
    setTextColor(MAGENTA); //Set text colour to white
    setTextSize(2); //Set text size to 2
    setCursor(225, 40); //Set the cursor
    writeString("Greetings! Team - Penn Dinosaurs!"); // displsay team name

    if (gpio_get(6) == 0) // if level is 1
    {
        level = 1;
        printf("level 1"); 
        setTextColor(MAGENTA); //Set text colour to white
        setTextSize(2); //Set text size to 2
        setCursor(5, 55); //Set the cursor
        writeString("LEVEL 1");// display level
        SPEED = 4; // speed
        VARY = 0.00005; //vary speed 
        setTextColor(BLUE); //Set text colour to white
        setTextSize(2); //Set text size to 2
        sprintf(scoreBuffer1, "%u", score1); //Send the score into the buffer array
        sprintf(highScoreBuffer1, "%u", highScore1); //Send the highscore into the buffer array
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
        writeString(scoreBuffer1); //Print the score
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
        writeString(highScoreBuffer1); //Print the highscore
        
    }
    if (gpio_get(4) == 0) // if level is 2
    {
        level = 2;
        printf("level 2");
        setTextColor(MAGENTA); //Set text colour to white
        setTextSize(2); //Set text size to 2
        setCursor(5, 55); //Set the cursor
        writeString("LEVEL 2"); // write string
        SPEED = 5; // speed
        VARY = 0.0001;// vary speed 
        setTextColor(BLUE); //Set text colour to white
        setTextSize(2); //Set text size to 2
        sprintf(scoreBuffer2, "%u", score2); //Send the score into the buffer array
        sprintf(highScoreBuffer2, "%u", highScore2); //Send the highscore into the buffer array
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
        writeString(scoreBuffer2); //Print the score
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
        writeString(highScoreBuffer2); //Print the highscore
    }
    if (gpio_get(5) == 0) // if level is 3
    {
        level = 3;
        printf("level 3");
        setTextColor(MAGENTA); //Set text colour to white
        setTextSize(2); //Set text size to 2
        setCursor(5, 55); //Set the cursor
        writeString("LEVEL 3");
        SPEED = 6; // speed
        VARY = 0.0003; // vary speed
        setTextColor(BLUE); //Set text colour to white
        setTextSize(2); //Set text size to 2
        sprintf(scoreBuffer3, "%u", score3); //Send the score into the buffer array
        sprintf(highScoreBuffer3, "%u", highScore3); //Send the highscore into the buffer array
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
        writeString(scoreBuffer3); //Print the score
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
        writeString(highScoreBuffer3); //Print the highscore
    }
    else{ // if no level is pressed the default level is 1
        level = 1;
        printf("level 1 default");
        setTextColor(MAGENTA); //Set text colour to white
        setTextSize(2); //Set text size to 2
        setCursor(5, 55); //Set the cursor
        writeString("LEVEL 1"); // display level in vga
        SPEED = 4; // speed
        VARY = 0.00005; // vary speed
        setTextColor(BLUE); //Set text colour to white
        setTextSize(2); //Set text size to 2
        sprintf(scoreBuffer1, "%u", score1); //Send the score into the buffer array
        sprintf(highScoreBuffer1, "%u", highScore1); //Send the highscore into the buffer array
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
        writeString(scoreBuffer1); //Print the score
        setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
        writeString(highScoreBuffer1); //Print the highscore
    }

    //start a infinite loop
    while(1){ 

        unsigned long begin_time = (unsigned long)(get_absolute_time()._private_us_since_boot / 1000); //start time
        if(resetGame){ //If the reset flag is set
            fillRect(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obstacle.w, obstacle.h, BLACK); //Clear the obstacle
            fillRect(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), myPlayer.w, myPlayer.h, BLACK); //Clear the player
            SPEED = 5;
            setTextColor(BLACK); //Set the text colour to black
            setTextSize(2); //Set text size to 4
            setCursor(30, 200); //Set the cursor
            writeString("Better Luck Next Time !"); //Clear the text

            setTextColor(BLACK); //Set text colour to white
            setTextSize(2); //Set text size to 2
            setCursor(5, 55); //Set the cursor
            writeString("LEVEL 1"); // display level

            setTextColor(BLACK); //Set text colour to white
            setTextSize(2); //Set text size to 2
            setCursor(5, 55);
            writeString("LEVEL 2");// display level

            setTextColor(BLACK); //Set text colour to white
            setTextSize(2); //Set text size to 2
            setCursor(5, 55);
            writeString("LEVEL 3");// display level
            // fillRect(5,55,100, 15, BLACK);


            setTextSize(2); //Set the cursor
            setTextColor(BLACK); //Set text colour to 0
            setCursor(100, 200); //Set the cursor
            writeString("Well Played! Better Luck Next Time !"); // display in VGA
            
            setTextSize(2); //Set text size to 2
            sprintf(scoreBuffer, "%u", score); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer); //Clear the score

            setTextSize(2); //Set text size to 2
            sprintf(scoreBuffer1, "%u", score1); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer1); //Clear the score
            setTextSize(2); //Set text size to 2
            sprintf(scoreBuffer2, "%u", score2); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer2); //Clear the score
            setTextSize(2); //Set text size to 2
            sprintf(scoreBuffer3, "%u", score3); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer3); //Clear the score
            
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
            writeString(highScoreBuffer1); //Clear the score
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
            writeString(highScoreBuffer2); //Clear the score
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
            writeString(highScoreBuffer3); //Clear the score

            myPlayer.y = 0; //Reset the player height
            myPlayer.vy = 0; //Reset the player velocity
            myPlayer.alive = 1; //Resurrect the player
            score1 = 0; //Reset the player score
            score2 = 0; //Reset the player score
            score3 = 0; //Reset the player score

            setTextColor(WHITE); //Set the text colour to white
            sprintf(scoreBuffer1, "%u", score1); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer1); //Print the score

            setTextColor(WHITE); //Set the text colour to white
            sprintf(scoreBuffer2, "%u", score2); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer2); //Print the score

            // setTextColor(WHITE); //Set the text colour to white
            // sprintf(scoreBuffer, "%u", score); //Send the score into the buffer array
            // setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            // writeString(scoreBuffer); //Print the score

            setTextColor(WHITE); //Set the text colour to white
            sprintf(scoreBuffer3, "%u", score3); //Send the score into the buffer array
            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
            writeString(scoreBuffer3); //Print the score


            obsType = randomRange(0, 3); //Get a random obstacle type
            obstacle.x = WIDTH; //Set its x-coordinate to the right most edge
            switch(obsType){ //Switch based on the obstacle type
                //Obstacle type 0
                case 0: obstacle.w = OBS_W_0;
                        obstacle.h = OBS_H_0;
                        break;
                //Obstacle type 1
                case 1: obstacle.w = OBS_W_1;
                        obstacle.h = OBS_H_1;
                        break;
                //Obstacle type 2
                case 2: obstacle.w = OBS_W_2;
                        obstacle.h = OBS_H_2;
                        break;
            }
            // DEBUG LOOP
            int gp3 = gpio_get(6); 
            int gp4 = gpio_get(4); 
            int gp5 = gpio_get(5);
            // PRINT VALUE TO DEBUG 
            printf("GP3 value %d \n", gp3 );
            printf("GP4 value %d \n", gp4 );
            printf("GP5 value %d \n", gp5 );

            if (gpio_get(6) == 0){
                level = 1;
                printf("level 1");
                setTextColor(MAGENTA); //Set text colour to white
                setTextSize(2); //Set text size to 2
                setCursor(5, 55); //Set the cursor
                writeString("LEVEL 1");// DISPLAY IN VGA
                SPEED = 4; // SPEED
                VARY = 0.00005; // VARY SPEED
                
            }
            if (gpio_get(4) == 0)
            {
                level = 2;
                printf("level 2");
                setTextColor(MAGENTA); //Set text colour to white
                setTextSize(2); //Set text size to 2
                setCursor(5, 55); //Set the cursor
                writeString("LEVEL 2");// DISPLAY IN VGA
                SPEED = 5; // SPEED
                VARY = 0.0001; // VARY SPEED
                
            }
            if (gpio_get(5) == 0)
            {
                level = 3;
                printf("level 3");
                setTextColor(MAGENTA); //Set text colour to white
                setTextSize(2); //Set text size to 2
                setCursor(5, 55); //Set the cursor
                writeString("LEVEL 3");// DISPLAY IN VGA
                SPEED = 6; // SPEED
                VARY = 0.0003; // VARY SPEED
            }
            if(gpio_get(6) == 1 && gpio_get(4) == 1 && gpio_get(5) == 1){
                level = 1;
                printf("level 1 default");
                setTextColor(MAGENTA); //Set text colour to white
                setTextSize(2); //Set text size to 2
                setCursor(5, 55); //Set the cursor
                writeString("LEVEL 1"); // DISPLAY IN VGA
                SPEED = 4; // SPEED
                VARY = 0.00005; // VARY SPEED
            }
            resetGame = 0; //Clear the reset flag
        }

        
        if(myPlayer.alive){ //If the player is alive
            // LEVEL 1 LOOP
            if (level == 1){
                setTextColor(WHITE); //Set text colour to white
                setTextSize(2); //Set text size to 2
                sprintf(scoreBuffer1, "%u", score1); //Send the score into the buffer array
                sprintf(highScoreBuffer1, "%u", highScore1); //Send the highscore into the buffer array
                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                writeString(scoreBuffer1); //Print the score
                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                writeString(highScoreBuffer1); //Print the highscore
                if(myPlayer.alive){ //If the player is alive
                        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
                            drawHLine(ground[i].x, ground[i].y, ground[i].w, BLACK); //Clear the older line
                            SPEED  = SPEED + VARY;
                            //printf("speed %f\n", SPEED );
                            ground[i].x -= SPEED; //Update the ground
                            if(ground[i].x + ground[i].w <= 0){ //If the ground element moves out of the frame
                                ground[i].x = WIDTH; //Reset the ground element's x-coordinate
                                ground[i].y = randomRange(HEIGHT - GROUND_HEIGHT + 10, HEIGHT); //Get a new y-coordinate for the ground element
                                ground[i].w = randomRange(3, 10); //Get a new width for the ground element
                            }
                        }
                        fillRect(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), myPlayer.w, myPlayer.h, BLACK); //Clear the player's older position
                        fillRect(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obstacle.w, obstacle.h, BLACK); //Clear the obstacle's older position
                        if(myPlayer.jumped){ //If the player has jumped
                            myPlayer.y += 1; //Change its y position
                            myPlayer.jumped = 0; //Reset the jump flag
                        }
                        if(myPlayer.y > 0){ //If the y position is greater than 0
                            myPlayer.y += myPlayer.vy; //Change the y position based on the velocity
                            myPlayer.vy -= GRAVITY; //Change the velocity based on the gravity
                        }
                        else if(myPlayer.y < 0){ //If the y position is less than 0
                            myPlayer.y = 0; //Reset y position
                            myPlayer.vy = 0; //Reset the velocity
                        }

                        obstacle.x -= SPEED; //Change the obstacle x position based on speed of the frame

                        if(obstacle.x + obstacle.w < 0){ //If the obstacle goes out of the screen
                            obsType = randomRange(0, 3); //Get new obstacle type
                            obstacle.x = WIDTH + randomRange(0, 50); //Generate the new x-coordinate for the obstacle
                            switch(obsType){ //Switch based on the obstacle type
                                //Obstacle type 0
                                case 0: obstacle.w = OBS_W_0;
                                        obstacle.h = OBS_H_0;
                                        break;
                                //Obstacle type 1
                                case 1: obstacle.w = OBS_W_1;
                                        obstacle.h = OBS_H_1;
                                        break;
                                //Obstacle type 2
                                case 2: obstacle.w = OBS_W_2;
                                        obstacle.h = OBS_H_2;
                                        break;
                            }

                            setTextColor(BLACK); //Set the text colour to black
                            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                            writeString(scoreBuffer1); //Clear the score

                            score1++; //Increment the score

                            sprintf(scoreBuffer1, "%u", score1); //Send the score into the buffer array
                            setTextColor(WHITE); //Set the text colour to white
                            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                            writeString(scoreBuffer1); //Write the new score

                            if(score1 > highScore1){
                                setTextColor(BLACK); //Set the text colour to black
                                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                                writeString(highScoreBuffer1); //Clear the highscore

                                highScore1++;

                                sprintf(highScoreBuffer1, "%u", highScore1); //Send the highscore into the buffer array
                                setTextColor(WHITE); //Set the text colour to white
                                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                                writeString(highScoreBuffer1); //Write the new highscore
                            }
                        }

                        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
                            drawHLine(ground[i].x, ground[i].y, ground[i].w, WHITE); //Draw the ground lines
                        }
                        drawHLine(0, HEIGHT - GROUND_HEIGHT, WIDTH, WHITE); //Draw the ground line
                        switch(obsType){ //Switch based on the obstacle type, draw the corrosponding bitmap
                            case 0: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeZer, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                            case 1: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeOne, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                            case 2: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeTwo, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                        }
                        if(myPlayer.y > 0){ //If the player is jumping
                            drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoJumpUp, myPlayer.w, myPlayer.h, WHITE, BLACK); //Draw the jump bitmap
                        }
                        else{ //If the player is not jumping, alternate bitmaps depending on the frames
                            if(runner > RUNNER_FRAMES / 2){
                                drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoRunOne, myPlayer.w, myPlayer.h, WHITE, BLACK);
                            }
                            else{
                                drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoRunTwo, myPlayer.w, myPlayer.h, WHITE, BLACK);
                            }
                        }
                        runner = (runner + 1) % RUNNER_FRAMES; //Increment the runner and modulo based on the upper limit of the frames
                        

                        if((myPlayer.x + myPlayer.w > obstacle.x) && (myPlayer.x < obstacle.x + obstacle.w) && (myPlayer.y < obstacle.h)){ //If the player has hit the obstacle
                            myPlayer.alive = 0; //Kill the player
                            //PlaySound(GameOverSnd, sizeof(GameOverSnd));
                            setTextColor(RED); //Set text colour to white
                            setTextSize(2); //Set the text size as 4
                            setCursor(100, 200); //Set the cursor
                            writeString("Well Played! Better Luck Next Time !"); // display in VGA
                            PlaySound(gameover, sizeof(gameover));  
                             //Print the message
                            sleep_ms(100);
                        }
                    }
            }
            // LEVEL 2 LOOP
            if (level == 2){
                setTextColor(WHITE); //Set text colour to white
                setTextSize(2); //Set text size to 2
                sprintf(scoreBuffer2, "%u", score2); //Send the score into the buffer array
                sprintf(highScoreBuffer2, "%u", highScore2); //Send the highscore into the buffer array
                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                writeString(scoreBuffer2); //Print the score
                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                writeString(highScoreBuffer2); //Print the highscore
                if(myPlayer.alive){ //If the player is alive
                        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
                            drawHLine(ground[i].x, ground[i].y, ground[i].w, BLACK); //Clear the older line
                            SPEED  = SPEED + VARY;
                            //printf("speed %f\n", SPEED );
                            ground[i].x -= SPEED; //Update the ground
                            if(ground[i].x + ground[i].w <= 0){ //If the ground element moves out of the frame
                                ground[i].x = WIDTH; //Reset the ground element's x-coordinate
                                ground[i].y = randomRange(HEIGHT - GROUND_HEIGHT + 10, HEIGHT); //Get a new y-coordinate for the ground element
                                ground[i].w = randomRange(3, 10); //Get a new width for the ground element
                            }
                        }
                        fillRect(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), myPlayer.w, myPlayer.h, BLACK); //Clear the player's older position
                        fillRect(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obstacle.w, obstacle.h, BLACK); //Clear the obstacle's older position
                        if(myPlayer.jumped){ //If the player has jumped
                            myPlayer.y += 1; //Change its y position
                            myPlayer.jumped = 0; //Reset the jump flag
                        }
                        if(myPlayer.y > 0){ //If the y position is greater than 0
                            myPlayer.y += myPlayer.vy; //Change the y position based on the velocity
                            myPlayer.vy -= GRAVITY; //Change the velocity based on the gravity
                        }
                        else if(myPlayer.y < 0){ //If the y position is less than 0
                            myPlayer.y = 0; //Reset y position
                            myPlayer.vy = 0; //Reset the velocity
                        }

                        obstacle.x -= SPEED; //Change the obstacle x position based on speed of the frame

                        if(obstacle.x + obstacle.w < 0){ //If the obstacle goes out of the screen
                            obsType = randomRange(0, 3); //Get new obstacle type
                            obstacle.x = WIDTH + randomRange(0, 50); //Generate the new x-coordinate for the obstacle
                            switch(obsType){ //Switch based on the obstacle type
                                //Obstacle type 0
                                case 0: obstacle.w = OBS_W_0;
                                        obstacle.h = OBS_H_0;
                                        break;
                                //Obstacle type 1
                                case 1: obstacle.w = OBS_W_1;
                                        obstacle.h = OBS_H_1;
                                        break;
                                //Obstacle type 2
                                case 2: obstacle.w = OBS_W_2;
                                        obstacle.h = OBS_H_2;
                                        break;
                            }

                            setTextColor(BLACK); //Set the text colour to black
                            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                            writeString(scoreBuffer2); //Clear the score

                            score2++; //Increment the score

                            sprintf(scoreBuffer2, "%u", score2); //Send the score into the buffer array
                            setTextColor(WHITE); //Set the text colour to white
                            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                            writeString(scoreBuffer2); //Write the new score

                            if(score2 > highScore2){
                                setTextColor(BLACK); //Set the text colour to black
                                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                                writeString(highScoreBuffer2); //Clear the highscore

                                highScore2++;

                                sprintf(highScoreBuffer2, "%u", highScore2); //Send the highscore into the buffer array
                                setTextColor(WHITE); //Set the text colour to white
                                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                                writeString(highScoreBuffer2); //Write the new highscore
                            }
                        }

                        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
                            drawHLine(ground[i].x, ground[i].y, ground[i].w, WHITE); //Draw the ground lines
                        }
                        drawHLine(0, HEIGHT - GROUND_HEIGHT, WIDTH, WHITE); //Draw the ground line
                        switch(obsType){ //Switch based on the obstacle type, draw the corrosponding bitmap
                            case 0: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeZer, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                            case 1: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeOne, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                            case 2: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeTwo, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                        }
                        if(myPlayer.y > 0){ //If the player is jumping
                            drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoJumpUp, myPlayer.w, myPlayer.h, WHITE, BLACK); //Draw the jump bitmap
                        }
                        else{ //If the player is not jumping, alternate bitmaps depending on the frames
                            if(runner > RUNNER_FRAMES / 2){
                                drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoRunOne, myPlayer.w, myPlayer.h, WHITE, BLACK);
                            }
                            else{
                                drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoRunTwo, myPlayer.w, myPlayer.h, WHITE, BLACK);
                            }
                        }
                        runner = (runner + 1) % RUNNER_FRAMES; //Increment the runner and modulo based on the upper limit of the frames

                        if((myPlayer.x + myPlayer.w > obstacle.x) && (myPlayer.x < obstacle.x + obstacle.w) && (myPlayer.y < obstacle.h)){ //If the player has hit the obstacle
                            myPlayer.alive = 0; //Kill the player
                            setTextColor(RED); //Set text colour to white
                            setTextSize(2); //Set the text size as 4
                            setCursor(100, 200); //Set the cursor
                            writeString("Well Played! Better Luck Next Time !"); // display in vga
                            PlaySound(gameover, sizeof(gameover));  //Print the message
                            sleep_ms(100); // sleep
                        }
                    }
                
            }
            // IF LEVEL 3
            if (level == 3){
                setTextColor(WHITE); //Set text colour to white
                setTextSize(2); //Set text size to 2
                sprintf(scoreBuffer3, "%u", score3); //Send the score into the buffer array
                sprintf(highScoreBuffer3, "%u", highScore3); //Send the highscore into the buffer array
                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                writeString(scoreBuffer3); //Print the score
                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                writeString(highScoreBuffer3); //Print the highscore
                if(myPlayer.alive){ //If the player is alive
                        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
                            drawHLine(ground[i].x, ground[i].y, ground[i].w, BLACK); //Clear the older line
                            SPEED  = SPEED + VARY;
                            //printf("speed %f\n", SPEED );
                            ground[i].x -= SPEED; //Update the ground
                            if(ground[i].x + ground[i].w <= 0){ //If the ground element moves out of the frame
                                ground[i].x = WIDTH; //Reset the ground element's x-coordinate
                                ground[i].y = randomRange(HEIGHT - GROUND_HEIGHT + 10, HEIGHT); //Get a new y-coordinate for the ground element
                                ground[i].w = randomRange(3, 10); //Get a new width for the ground element
                            }
                        }
                        fillRect(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), myPlayer.w, myPlayer.h, BLACK); //Clear the player's older position
                        fillRect(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obstacle.w, obstacle.h, BLACK); //Clear the obstacle's older position
                        if(myPlayer.jumped){ //If the player has jumped
                            myPlayer.y += 1; //Change its y position
                            myPlayer.jumped = 0; //Reset the jump flag
                        }
                        if(myPlayer.y > 0){ //If the y position is greater than 0
                            myPlayer.y += myPlayer.vy; //Change the y position based on the velocity
                            myPlayer.vy -= GRAVITY; //Change the velocity based on the gravity
                        }
                        else if(myPlayer.y < 0){ //If the y position is less than 0
                            myPlayer.y = 0; //Reset y position
                            myPlayer.vy = 0; //Reset the velocity
                        }

                        obstacle.x -= SPEED; //Change the obstacle x position based on speed of the frame

                        if(obstacle.x + obstacle.w < 0){ //If the obstacle goes out of the screen
                            obsType = randomRange(0, 3); //Get new obstacle type
                            obstacle.x = WIDTH + randomRange(0, 50); //Generate the new x-coordinate for the obstacle
                            switch(obsType){ //Switch based on the obstacle type
                                //Obstacle type 0
                                case 0: obstacle.w = OBS_W_0;
                                        obstacle.h = OBS_H_0;
                                        break;
                                //Obstacle type 1
                                case 1: obstacle.w = OBS_W_1;
                                        obstacle.h = OBS_H_1;
                                        break;
                                //Obstacle type 2
                                case 2: obstacle.w = OBS_W_2;
                                        obstacle.h = OBS_H_2;
                                        break;
                            }

                            setTextColor(BLACK); //Set the text colour to black
                            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                            writeString(scoreBuffer3); //Clear the score
                            score3++; //Increment the score
                            sprintf(scoreBuffer3, "%u", score3); //Send the score into the buffer array
                            setTextColor(WHITE); //Set the text colour to white
                            setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET1); //Set the cursor
                            writeString(scoreBuffer3); //Write the new score

                            if(score3 > highScore3){
                                setTextColor(BLACK); //Set the text colour to black
                                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                                writeString(highScoreBuffer3); //Clear the highscore

                                highScore3++;

                                sprintf(highScoreBuffer3, "%u", highScore3); //Send the highscore into the buffer array
                                setTextColor(WHITE); //Set the text colour to white
                                setCursor(SCORE_X_OFFSET2, SCORE_Y_OFFSET2); //Set the cursor
                                writeString(highScoreBuffer3); //Write the new highscore
                            }
                        }
                        for(i = 0; i < GROUND_SIZE; i++){ //For all elements ranging in the ground array
                            drawHLine(ground[i].x, ground[i].y, ground[i].w, WHITE); //Draw the ground lines
                        }
                        drawHLine(0, HEIGHT - GROUND_HEIGHT, WIDTH, WHITE); //Draw the ground line

                        switch(obsType){ //Switch based on the obstacle type, draw the corrosponding bitmap
                            case 0: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeZer, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                            case 1: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeOne, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                            case 2: drawBitmap(obstacle.x, (HEIGHT - GROUND_HEIGHT - ((obstacle.h / 2))), obsTypeTwo, obstacle.w, obstacle.h, GREEN, BLACK);
                                    break;
                        }
                        if(myPlayer.y > 0){ //If the player is jumping
                            drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoJumpUp, myPlayer.w, myPlayer.h, WHITE, BLACK); //Draw the jump bitmap
                        }
                        else{ //If the player is not jumping, alternate bitmaps depending on the frames
                            if(runner > RUNNER_FRAMES / 2){
                                drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoRunOne, myPlayer.w, myPlayer.h, WHITE, BLACK);
                            }
                            else{
                                drawBitmap(myPlayer.x, (HEIGHT - GROUND_HEIGHT - (myPlayer.y + (myPlayer.h / 2))), dinoRunTwo, myPlayer.w, myPlayer.h, WHITE, BLACK);
                            }
                        }
                        runner = (runner + 1) % RUNNER_FRAMES; //Increment the runner and modulo based on the upper limit of the frames
                        // drawHLine(0, HEIGHT - GROUND_HEIGHT, WIDTH, WHITE); //Draw the ground line

                        if((myPlayer.x + myPlayer.w > obstacle.x) && (myPlayer.x < obstacle.x + obstacle.w) && (myPlayer.y < obstacle.h)){ //If the player has hit the obstacle
                            myPlayer.alive = 0; //Kill the player
                            setTextColor(RED); //Set text colour to white
                            setTextSize(2); //Set the text size as 4
                            setCursor(100, 200); //Set the cursor
                            writeString("Well Played! Better Luck Next Time !"); // display in vga
                            PlaySound(gameover, sizeof(gameover));  //Print the message
                            sleep_ms(100);// sleep
                        }
                    }
                
            }      
        }
        exTime = 33 - ((unsigned long)(get_absolute_time()._private_us_since_boot / 1000) - begin_time); // Determine how long you should sleep for 30 frames per second.
        sleep_ms(exTime); //Sleep for that time
    }
}