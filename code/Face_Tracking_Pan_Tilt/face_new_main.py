
# import the necessary libraries
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

import time
import adafruit_servokit
import os

# angle to set at start
angle1 = 20 
angle2 = 90

# Define Class Servokit to control the servos
class ServoKit(object):
    default_angle1 = 45
    default_angle2 = 90
   
    def __init__(self, num_ports):
        print("Initializing the servo...")
        self.kit = adafruit_servokit.ServoKit(channels=16)
        self.num_ports = num_ports
        self.resetAll()
        print("Initializing complete.")

    def setAngle(self, port, angle):
        if angle < 0:
            self.kit.servo[port].angle = 0
        elif angle > 180:
            self.kit.servo[port].angle = 180
        else:
            
            self.kit.servo[port].angle = angle
   
    def getAngle(self, port):
        return self.kit.servo[port].angle

    def reset(self, port):
        self.kit.servo[port].angle = self.default_angle

    def resetAll(self):
        
        self.kit.servo[0].angle = self.default_angle1
        self.kit.servo[1].angle = self.default_angle2

# initialise srvos           
servoKit = ServoKit(4)
servoKit.resetAll()
           
# Set up the Cascade Classifier for face tracking. This is using the Haar Cascade face recognition method with LBP = Local Binary Patterns.
# Seen below is commented out the slower method to get face tracking done using only the HAAR method.
# cascPath = 'haarcascade_frontalface_default.xml' # sys.argv[1]
face_cascade= cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

# Define the dimentions of camera frames
width,height=320,240
# initialise pi camera
camera = PiCamera()
camera.resolution = (width,height)
# define framerate
camera.framerate = 30
rawCapture = PiRGBArray(camera, size=(width,height))
time.sleep(1)

# run a iterative for loop to activte servos when a person is detected
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    image = frame.array
    frame=cv2.flip(image,1)
    gray=cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)    
    # set the servos to initialise to  angle1 = 20 angle2 = 90 - you guys can change the angle so that servos get initialize and camera can see the face.
    servoKit.setAngle(0, angle1)
    servoKit.setAngle(1, angle2)
   
    #detect face coordinates x,y,w,h
    faces=face_cascade.detectMultiScale(gray,1.3,5)
    c=0
    # servos control loop
    for(x,y,w,h) in faces:
        c+=1
        if(c>1):
            break
        #centre of face
        face_centre_x=x+w/2
        face_centre_y=y+h/2
        #pixels to move
        x = x + (w/2)
        y = y + (h/2)

        # Correct relative to centre of image
        turn_x  = float(x - (width/2))
        turn_y  = float(y - (height/2))

        # Convert to percentage offset
        turn_x  /= float(width/2)
        turn_y  /= float(height/2)


        # Scale offset to degrees 
        turn_x   *= -20.5 # VFOV
        turn_y   *= -20.5 # HFOV
        # create the new angles for the servos so that it moves as the face moves
        angle2  += turn_x
        angle1 += turn_y
        # activate the servos to keep track of the face
        servoKit.setAngle(0, int(angle1))
        servoKit.setAngle(1, int(angle2))
        # make the new values of x,y,w,h as integers. so that it does not crete error when sent to "frame=cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),6)"
        x = int(x)
        y = int(y)
        w = int(w)
        h = int(h)
        if(c==1):
            newa = (255,0,0)
            newb = 6
            frame=cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),6)
    # display frame/image in screen
    cv2.imshow('frame',frame) 
   
    key = cv2.waitKey(1) & 0xFF
    rawCapture.truncate(0)
    if key == ord("q"):
        break
# destroy all windows
cv2.destroyAllWindows()

