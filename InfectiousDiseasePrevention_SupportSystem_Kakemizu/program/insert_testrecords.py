# -*- coding: utf-8 -*-
import dbFunction
import datetime
import random
import time

def main():
    time.sleep(5.0)
    former1 = 500
    former2 = 500   
    f1 = open('/home/team-emb/gra_thesis/testrecords1.txt','r') #シミュレーション用
    f2 = open('/home/team-emb/gra_thesis/testrecords2.txt','r') #シミュレーション用 
    min = 30
    now = datetime.datetime.now() 
    for i in range(20):
        #diff_1 = random.randint(-30,30)
        #diff_2 = random.randint(-30,30)
        r1 = f1.readline()
        r2 = f2.readline()
        nowtime = datetime.datetime(now.year,now.month,now.day,(now + datetime.timedelta(minutes = min)).hour,(now + datetime.timedelta(minutes = min)).minute,now.second) 
        dbFunction.add_record(nowtime,12345,random.randint(10,50),random.randint(5,10),random.randint(30,35),random.randint(1015,1016),random.randint(1700,1800),r1)
        dbFunction.add_record(nowtime,67890,random.randint(10,50),random.randint(5,10),random.randint(30,35),random.randint(1015,1016),random.randint(1700,1800),r2)
        #former1 = former1 + diff_1
        #former2 = former2 + diff_2 
        min += 3
        i += 1
        time.sleep(15.0)
