from cvzone.SerialModule import SerialObject
from cvzone.HandTrackingModule import HandDetector
import cv2
import mediapipe as mp
import time
import numpy as np
import array as arr
import serial
import collections



pTime = 0
cTime = 0
period = 1000
cap = cv2.VideoCapture(0)
wCam, hCam = 1024, 768
lClose = [1, 0, 0, 0, 0]
lOpen = [0, 1, 1, 1, 1]
cap.set(3, wCam)
cap.set(4, hCam)
detector = HandDetector(maxHands=2, detectionCon=0.7)


while True:
    success, img = cap.read()
    hands, img = detector.findHands(img, flipType=False)

    # hands = detector.findHands(img, draw=False) #no draw

    if hands:
        hand1 = hands[0]
        lmList1 = hand1["lmList"]  # list of 21 landmarks points
        bbox1 = hand1["bbox"]
        centerPoint1 = hand1["center"]  # center of the hand cx cy
        handType = hand1["type"]



        fingers1 = detector.fingersUp(hand1)
        cx, cy = hand1['center']
        if fingers1== [0, 1, 1, 1, 1]:
            if cx < 200:
                print("yes")
            else:
                print("No")


        print(cx, cy)




        #print(lmList1)
        # print(bbox1)
        #print(centerPoint1)
        # print(handType)

        fingers = detector.fingersUp(hand1)
        print(fingers)

        if len(hands)==2:
            hand2 = hands[1]
            lmList2 = hand2["lmList"]  # list of 21 landmarks points
            bbox2 = hand2["bbox"]
            centerPoint2 = hand2["center"]  # center of the hand cx cy
            handType2 = hand2["type"]

            fingers2 = detector.fingersUp(hand2)

            length, info, img = detector.findDistance(centerPoint1, centerPoint2, img)
            #print(fingers1, fingers2)
            #lOpen.sort()
            #fingers2.sort()
            #if lOpen == fingers2:
                #print("yes")
            #else:
                #print("no")

        #print(fingers1)



       # mySerial.sendData(fingers)
    cTime = time.time()
    fps = 1 / (cTime - pTime)
    pTime = cTime

    cv2.rectangle(img, (200, 100), (450, 350), (0, 255, 0), 1)
    cv2.putText(img, str(int(fps)), (10, 70), cv2.FONT_HERSHEY_PLAIN, 3, (255, 0, 0), 3)
    cv2.imshow("image", img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break


