# Customer-experience enhancement system!
### By - Team Penn Dinosaurs ☘️

## University of Pennsylvania, ESE 5190: Intro to Embedded Systems - Final Project
    SATYAJEET DAS
        LinkedIn: https://www.linkedin.com/in/satyajeet-das/
    Soumya Dash
        LinkedIn: https://www.linkedin.com/in/srd00/
    Qiao Xu
        LinkedIn: https://www.linkedin.com/in/qiao-xu-b520a3249/

    Tested on:DELL Inspiron 16 Plus (16-inch, 2022), Windows 11 Home 21H2
    
    
    
# Introduction:<br />
The inconvenience of taking care of the children has bothered the customers who go for service or business for a long time, and few ways are efficient enough to help the customer take care of the children. One possible option is to hire people for children-care services, which is expensive and may not be feasible for most general service institutions since they may not have much extra budget. Inspired by the idea of the chrome dino game, a design of the customer-experience enhancement system is proposed to provide a feasible and reliable way to take care of the children of customers when they come for service to improve customer experience further. The system combines the dino game, face tracking pan-tilt camera, and the LCD & Speaker arrangement for customer interaction for playing music.<br />

In the paper, we will introduce the project's background(problem statement) first, then discuss the source of inspiration. Next, we will illustrate the system architecture and objects of this system. After giving this basic information about the project, we will show the technical details, such as the component lists, the connection diagram, the troubleshooting process, and the result.<br />


# Problem statement:<br />
It is common to see many customers carrying their children in the customer waiting room. And this is very inconvenient for them since they need to focus on the service or the business they need to do, making it hard to distribute their attention to the naughty children. However, usually, they could not find a safe and reliable place to leave their children. <br />
Admittedly, some options are employed to help with this problem. However, none of them could be feasible and effective for society. One possible option is to let the service provider, such as the bank and supermarkets, hire some people to take care of the children, which could bring too much investment. So this would be unaffordable for many small or medium institutions with insufficient extra money. Another option would be leaving the phone to the children, which is quite common nowadays since phones are almost necessary for everyone in this high-tech era. However, this still could not ensure 100% safety and is not feasible for anyone. This is because some places have a poor internet connection, which could lead to a failure in contacting the children. And children will get bored and try to go somewhere else for fun when there is no internet or poor internet, which will still bring the risk of losing the children. Also, some people are not wealthy enough to get a phone for each of their children, and giving their phone to children is also inconvenient since the customer may need to use the phone for the service.
Hence, a generally feasible solution to help customers take care of their children safely is in need. <br />

# Inspiration of the project:<br />

The idea of the chrome dino game inspires the concept of this customer-experience enhancement system. “ It is a play on going back to the “prehistoric age” when you had no Wi-Fi,” says the Chrome UX engineer Edward Jung. The chrome dino game was designed for website users to have fun when getting kicked offline, and it was deliberately kept simple and not boring so that everyone could get a good handle on it quickly and enjoy it. As a result, the chrome dino game was popular in countries with spotty data connections. It was so popular that some enterprise administrators had to disable it for the school kids and employees. <br />
Inspired by its huge success and the problem behind it, we think this game could be perfectly embedded in the customer-experience enhancement system we design. Firstly, it could provide children with a way to have fun offline, reducing the risk of losing an internet connection. Also, games are always an excellent attraction to children, and this game is simple enough that anyone can play it without spending the time to learn the skills.<br />
To better reduce the safety concern and provide a reliable for the customer to take care of their children, we also add the Pan-tilt camera and the LCD & Speaker. The pan-tilt camera is used for person detection so that it can track the children who are playing the game. If the camera loses sight of the children for a certain time, a warning message will be sent to the customer so that the customer can avoid dispersing their attention to worry about their children all the time. Children are also fascinated by these moving face tracking systems too, which not only serve the primary purpose but also act as a fun game for children and keep them busy. So a customer could peacefully do their purposeful work without worrying about their children. The combined use of LCD and speaker creates a human interaction since the children are also curious about these things and love greetings. And they could also play music based on their preference.<br />
    
    
    

# Project Architecture: <br />
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/triangle.png)<br />


# Circuit Diagram of the elements of our Project:<br />
### Dino Game
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/diagram/Circuit%20Diagram/Dino_Game_Ckt.png)<br />
### Pan - Tilt Person Decting & Following Camera
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/diagram/Circuit%20Diagram/Pan_Tilt_Camera_Ckt.png)<br />
### Customer Interaction - LCD & Speaker for Playing
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/diagram/Circuit%20Diagram/Human_Interaction_System(LCD%2BSpeaker)_Ckt.png)<br />

# Operations/Functionalities:<br />
## 1. Dino Game:<br />
Our aim in this game was to develop Google's Dino game using the Raspberry Pi Pico board, which has an RP2040 microcontroller.<br />

The following features are included in this game:<br />
1. It has three levels (level1 - easy, Level 2 - Medium, Level 3 - Hard). The difficulty at each level would rise in proportion to the increasing score.<br />
2. The game has two sounds, one for the humping dino and the other when the “Game is Over” (Dinosaur crashes with the cactus and dies),  signifying that the user input of button press supplied by the Game player was successfully executed. Moreover, make the game more interactive. A speaker was employed for this reason.<br />
3. It employs a VGA display to display all visuals on the monitor. We leveraged the PIO functionality of RP2040 to achieve this feat of running the VGA display using the tiny and cheap Pico. We used bitmaps to draw the shape of a dinosaur and different types of cacti.<br />
4. For user interaction, we used a “glowing” big push button, for an interactive user experience.<br />
5. We also created different high-score buffers for different levels. The aim was to keep track of the highest score achieved at each level. Moreover, to create a competition during the demo day to beat the highest score. This makes the game more user interactive and fun.<br />
## 2. Person Detection:<br />
This element of our project was utilized to recognize faces and shift the pan tilt to that direction dependent on the movement of the individual. This enables face tracking feature.<br />
This work includes the following features:<br />
1. It houses the Raspberry Pi Camera Module V2 on the Arducam Pan Tilt arrangement. This coordinated movement of servos and cameras enables the functionality of face tracking.<br />
2. It identifies humans by using the openCV library and Machine Learning model “haarcascade_frontalface_default.xml” to detect faces and put a frame around them.
3. Its pan tilt camera would move up/down and left/right based on the person's movement measuring the relative movement of the frame and trying to reduce the positional error by moving the servos and hence the camera. <br />

## 3. Human Interaction Interface:<br />
This portion of our project was leveraged to construct a user interactive interface like "Google Assistant".<br />

This work includes the following features:<br />
1. It asks the person's name.<br />
2. It greets the user by name, asks if they want to listen to music, and suggests some songs.<br />
3. It plays the music based on various interest filters.<br />

# Please find the detailed code and explaination here: https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/tree/main/code<br />

# Photos of our final model:<br />
### Dino Game
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Photos%20of%20Final%20Models/Dino%20Game.JPG)<br />
### Pan - Tilt Person Decting & Following Camera
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Photos%20of%20Final%20Models/Pan%20Tilt.jpg)<br />
### Customer Interaction - LCD & Speaker for Playing
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Photos%20of%20Final%20Models/Speaker%20and%20LCD.JPG)<br />

# Gifs from our projects:<br />
### Dino - Game: Level - 1<br />
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/GIF/dino_level1%2000_00_00-00_00_30.gif)<br />
### Dino - Game: Level - 2<br />
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/GIF/dino_level2%2000_00_00-00_00_30.gif)<br />
### Dino - Game: Level - 3<br />
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/GIF/Dino_Level3%2000_00_00-00_00_30.gif)<br />
### Pan - Tilt Person Decting & Following Camera<br /><br />
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/GIF/Face%20detection%2000_00_00-00_00_30.gif)<br />
#### Customer Interaction - LCD & Speaker for Playing Songs<br />
![alt text](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/GIF/LCD_Speaker%2000_00_00-00_00_30.gif)<br />

# Videos from our projects:<br />
### Dino - Game: Level - 1<br />
[![Watch the video](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/images.jpg)](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Videos/dino_level1.mp4)
### Dino - Game: Level - 2<br />
[![Watch the video](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/images.jpg)](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Videos/dino_level2.mp4)
### Dino - Game: Level - 3<br />
[![Watch the video](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/images.jpg)](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Videos/Dino_Level3.mp4)
### Pan - Tilt Person Decting & Following Camera<br /><br />
[![Watch the video](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/images.jpg)](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Videos/Face%20detection.mp4)
#### Customer Interaction - LCD & Speaker for Playing Songs<br />
[![Watch the video](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/images.jpg)](https://github.com/satyajeetburla/ESE-519-FINAL-PROJECT/blob/main/media/Videos/LCD_Speaker.mp4)

# Thank You !!!
