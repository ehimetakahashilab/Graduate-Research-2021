# -*- coding: utf-8 -*-
import cv2
import os
import datetime
#from PIL import Image
#import numpy as np
import time
#import countPersons

dir_path = '/home/team-emb/gra_thesis/cap_img'
basename = 'cam_img'
ext = 'jpg'
def capImage():
    # VideoCapture オブジェクトを取得します
    capture = cv2.VideoCapture(0, cv2.CAP_V4L)
    #capture.set(cv2.CAP_PROP_FRAME_WIDTH,640)
    #capture.set(cv2.CAP_PROP_FRAME_HIGHT,480)
    #capture = cv2.VideoCapture(0) 
    #os.makedirs(dir_path, exist_ok=True)
    base_path = os.path.join(dir_path, basename)

    ret, frame = capture.read()
    #frame = cv2.cvtColor(frame)
    # resize the window
    windowsize = (640, 480)
    frame = cv2.resize(frame, windowsize)
        #cv2.imshow('title',frame)
        #cv2.imwrite('{}_{}.{}'.format(base_path, datetime.datetime.now().strftime('%Y%m%d%H%M'), ext),frame)
    cv2.imwrite('{}.{}'.format(base_path, ext),frame)

    capture.release()
    cv2.destroyAllWindows()

'''
    while True:
        ti=time.time()
        ref,frame = capture.read()
        frame = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        frame = Image.fromarray(np.uint8(frame))

        num = np.array(countPersons.count(frame))
	frame = cv2.cvtColor(frame,cv2.COLOR_RGB2BGR)
        frame = cv2.putText(frame,'num=()',(0,40),cv2.FONT_HERSHEY_SIMPLEX,1,(0,255,0),2)
        cv2.imshow('video',frame)
'''
   
#capImage()
