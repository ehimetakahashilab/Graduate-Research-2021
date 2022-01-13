# -*- coding: utf-8 -*-
import Jetson.GPIO as GPIO
import time
import threading
import sys
import random
import datetime
import dbFunction
import jetsonReceive
import capture
import countPersons
import darknet
import insert_testrecords #発表デモ用

buz_pin = 33 #buzzer
gled_pin = 19 #green led
yled_pin = 21 #yellow led
rled_pin = 23 #red led
hled_pin = 31 #humidity led
slm_mode = False #Standard Level Monitering Flag
stb_mode = False #Stand By Mode Flag

GPIO.setmode(GPIO.BOARD)  # BOARD pin-numbering scheme
GPIO.setup(buz_pin, GPIO.OUT) # BUZZER pin set as output
GPIO.setup(gled_pin, GPIO.OUT) #GREEN LED pin set as output 
GPIO.setup(yled_pin, GPIO.OUT) #YELLOW LED pin set as output 
GPIO.setup(rled_pin, GPIO.OUT) #RED LED pin set as output  
GPIO.setup(hled_pin, GPIO.OUT) #Humidity LED pin set as output 

class Room():
	def __init__(self, roomArea, densityRule):
		self.roomArea = roomArea #部屋の広さ(平方メートル)
		self.densityRule = densityRule #滞在可能人数規則(何平方メートルに1人滞在可能化の部屋ルール)
		self.standardDR = int(roomArea/densityRule) #部屋に滞在できる最大人数

class Condition():
    def __init__(self, numofPersons, co2Concentration, temperature, humidity):
        self.numofPersons = numofPersons
        self.co2Concentration = co2Concentration
        self.temparature = temperature
        self.humidity = humidity

    def checkCondition(self, numofPersons, co2Concentration, temperature, humidity):
        self.numofPersons = numofPersons
        self.co2Concentration = co2Concentration
        self.temparature = temperature
        self.humidity = humidity

class WarningLevel():
    proportion = 0
    lv1Co2UpperLim = 700 #警戒レベル1でのco2上限値(co2濃度規定)
    lv2Co2UpperLim = 800 #警戒レベル2でのco2上限値
    lv3Co2UpperLim = 900 #警戒レベル3でのco2上限値
    lv4Co2UpperLim = 1000 #警戒レベル4でのco2上限値、警戒レベル5では上限値なし
    humidityLowerLim = 30 #湿度下限値(警戒レベルによらず一定)
    formerlv = 0

    def __init__(self): #標準警戒レベルとして設定
        self.level = 0
        #self.co2LowerLim = 0

    def standardMonitering(self, co2Concentration):
        if co2Concentration >= WarningLevel.lv1Co2UpperLim:
            print("***標準警戒レベル　CO2アラート***")
            self.level = 0
            GPIO.output(rled_pin,GPIO.HIGH)
            return 1     
                    
    def is_levelChanged(self, level):
        if WarningLevel.formerlv < self.level and WarningLevel.formerlv != 0:
            print("***警戒レベルUPアラート***")
            return 1

    def humidityCheck(self, humidity):
        if humidity <= WarningLevel.humidityLowerLim:
            return 1

    def levelConfig(self, level, co2Concentration): #co2濃度に基づいた警戒レベル設定
        WarningLevel.formerlv = level
        if co2Concentration < WarningLevel.lv1Co2UpperLim:
            self.level = 1
        elif WarningLevel.lv1Co2UpperLim <= co2Concentration and co2Concentration < WarningLevel.lv2Co2UpperLim:
            self.level = 2
        elif WarningLevel.lv2Co2UpperLim <= co2Concentration and co2Concentration < WarningLevel.lv3Co2UpperLim:
            self.level = 3
        elif WarningLevel.lv3Co2UpperLim <= co2Concentration and co2Concentration < WarningLevel.lv4Co2UpperLim:
            self.level = 4
        elif WarningLevel.lv4Co2UpperLim <= co2Concentration:
            self.level = 5

    def regulationConfig(self, level ,standardDR): #警戒レベルに基づいた滞在人数基準設定
        if level == 1:
            WarningLevel.proportion = 0.95 #警戒レベル1:規定人数の100~95%が人数基準
        elif level == 2:
            WarningLevel.proportion = 0.90 #警戒レベル1:規定人数の95~90%が人数基準
        elif level == 3:
            WarningLevel.proportion = 0.85 #警戒レベル1:規定人数の90~85%が人数基準
        elif level == 4:
            WarningLevel.proportion = 0.80 #警戒レベル1:規定人数の85~80%が人数基準
        elif level == 5:
            WarningLevel.proportion = 0.75 #警戒レベル1:規定人数の80~75%が人数基準
        
        self.personsUpperLim = int(standardDR * (WarningLevel.proportion + 0.05)) #上限人数
        self.personsLowerLim = int(standardDR * WarningLevel.proportion) #下限人数

class Risk(): #滞在人数基準と滞在人数に基づいたリスク分析
    riskLevel = "" #危険度をgreen、yellow、redで表現
    global risk_configured
    #def __init__(self):

    def riskConfig(self, numofPersons, personsLowerLim, personsUpperLim):
        if numofPersons < personsLowerLim: #下限人数未満ならgreen
            Risk.riskLevel = 'green'
            dbFunction.regi_enter_risk('green')
            GPIO.output(gled_pin,GPIO.HIGH)
            GPIO.output(yled_pin,GPIO.LOW)
            GPIO.output(rled_pin,GPIO.LOW)
        elif personsLowerLim <= numofPersons and numofPersons < personsUpperLim: #下限人数以上 上限人数未満ならyellow
            Risk.riskLevel = 'yellow'
            dbFunction.regi_enter_risk('yellow')
            GPIO.output(gled_pin,GPIO.LOW)
            GPIO.output(yled_pin,GPIO.HIGH)
            GPIO.output(rled_pin,GPIO.LOW)
        elif personsUpperLim <= numofPersons: #上限人数以上ならred
            Risk.riskLevel = 'red'
            dbFunction.regi_enter_risk('red')
            GPIO.output(gled_pin,GPIO.LOW)
            GPIO.output(yled_pin,GPIO.LOW)
            GPIO.output(rled_pin,GPIO.HIGH)

def modeSwitch(numofPersons, standardDR): #規定人数の75%以下の時は、標準警戒レベルでの監視モードに
    global slm_mode
    if numofPersons >= standardDR * 0.75:
        slm_mode = False
        return 0
    else: 
        slm_mode = False
        slm_mode = True
        GPIO.output(gled_pin,GPIO.LOW)
        GPIO.output(yled_pin,GPIO.LOW)
        GPIO.output(rled_pin,GPIO.LOW)               
        return 1

'''
def temparatureAverage():
'''   
def humidityCheck(devRist,devNum):
    data = []
    sum = 0
    nowTime = datetime.datetime.now()
    timeRange = datetime.datetime(nowTime.year,nowTime.month,nowTime.day,(nowTime - datetime.timedelta(minutes = 3)).hour,(nowTime - datetime.timedelta(minutes = 3)).minute) #データベースからの取得を、3分前以降のものに限定する
    for i in range (devNum):
        try:
            data = dbFunction.get_records(devRist[i],timeRange)
            sum += data[0][4]
        except IndexError: #取得したデータ数が5に満たない場合
            GPIO.output(hled_pin,GPIO.LOW)
    if sum / devNum < 30:
        humAlert()
    print(sum/devNum)

def checkRecentCo2(devRist,devNum): #直近15分の間のデータ分析
    data = []
    min = []
    nowTime = datetime.datetime.now()
    timeRange = datetime.datetime(nowTime.year,nowTime.month,nowTime.day,(nowTime - datetime.timedelta(minutes = 21)).hour,(nowTime - datetime.timedelta(minutes = 21)).minute) #データベースからの取得を、21分前以降のものに限定する
    if devNum == 0:
        return -1
    for i in range (devNum):
        data = dbFunction.get_records(devRist[i],timeRange) #接続されたデバイスの直近のtimeRamge(現在時刻の21分前)以降のデータのみを取得
        #print(devRist[i])
        print(data)
        min.append(2000)
        for j in range (5): #取得したデータのうち分析対象は直近5回分のデータのみ
            try:
                print(data[j][7])
                if min[i] > data[j][7]: #二酸化炭素データ
                    min[i]= data[j][7]
            except IndexError: #取得したデータ数が5に満たない場合
                return -1

    max = min[0]
    for i in range (devNum): #各デバイスの5回分のデータの最小値を求める
        if min[i] > max:
            max = min [i]
    
    return max #各デバイスの5回分のデータの最小値のうち最大の値を代表値として返す

def lvAlert(): #レベル上昇時アラート 
    for i in range (2):
        GPIO.output(buz_pin,GPIO.HIGH)
        time.sleep(0.5)
        GPIO.output(buz_pin,GPIO.LOW)
        time.sleep(0.2)
        GPIO.output(buz_pin,GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(buz_pin,GPIO.LOW)
        time.sleep(0.2)
        GPIO.output(buz_pin,GPIO.HIGH)
        time.sleep(0.5)
        GPIO.output(buz_pin,GPIO.LOW)
        time.sleep(0.5)

def co2Alert(): #標準警戒レベルモニタリング時のco2アラート
    for i in range (3):
        GPIO.output(buz_pin,GPIO.HIGH)
        time.sleep(0.7)
        GPIO.output(buz_pin,GPIO.LOW)
        time.sleep(0.3)

def humAlert(): #標準警戒レベルモニタリング時のco2アラート
    for i in range (2):
        GPIO.output(buz_pin,GPIO.HIGH)
        GPIO.output(yled_pin,GPIO.HIGH)
        time.sleep(0.4)
        GPIO.output(buz_pin,GPIO.LOW)
        time.sleep(0.4)
        GPIO.output(buz_pin,GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(buz_pin,GPIO.LOW)
        time.sleep(0.2)
        GPIO.output(buz_pin,GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(buz_pin,GPIO.LOW)
        GPIO.output(yled_pin,GPIO.LOW)
        time.sleep(0.6)
    time.sleep(1.0)

def startSign(): #システム起動時サイン
    print('startsign start')
    GPIO.output(buz_pin,GPIO.HIGH)
    for i in range (3):
        GPIO.output(gled_pin,GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(gled_pin,GPIO.LOW)
        GPIO.output(yled_pin,GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(yled_pin,GPIO.LOW)
        GPIO.output(rled_pin,GPIO.HIGH)
        time.sleep(0.2)
        GPIO.output(rled_pin,GPIO.LOW)
    GPIO.output(buz_pin,GPIO.LOW)    
    print('startsign end')
def ledClear(): #LED消灯/スリープ時サイン
    print('sleepsign start')
    GPIO.output(gled_pin,GPIO.LOW)
    GPIO.output(yled_pin,GPIO.LOW)
    GPIO.output(rled_pin,GPIO.LOW)
    GPIO.output(hled_pin,GPIO.LOW)
    print('sleepsign end')
def standbySign(): #データ収集待ちサイン(1日のはじめデータ数が少ないとき)
    global stb_mode
    #stb_mode = True
    print('standbysign start')
    while stb_mode == True:
        GPIO.output(gled_pin,GPIO.HIGH)
        time.sleep(0.4)
        GPIO.output(gled_pin,GPIO.LOW)
        if stb_mode != True:
            break
        GPIO.output(yled_pin,GPIO.HIGH)
        time.sleep(0.4)
        GPIO.output(yled_pin,GPIO.LOW)
        if stb_mode != True:
            break
        GPIO.output(rled_pin,GPIO.HIGH)
        time.sleep(0.4)
        GPIO.output(rled_pin,GPIO.LOW)
        if stb_mode != True:
            break
        time.sleep(0.8)
    GPIO.output(gled_pin,GPIO.LOW)
    GPIO.output(yled_pin,GPIO.LOW)
    GPIO.output(rled_pin,GPIO.LOW)
    print('standbysign end')
def standardSign(): #標準警戒レベルモニタリング時サイン
    global slm_mode
    #slm_mode = True
    print('standardsign start')
    while slm_mode == True:
        GPIO.output(gled_pin,GPIO.HIGH)
        time.sleep(0.5)
        if slm_mode != True:
            break
        GPIO.output(gled_pin,GPIO.LOW)
        time.sleep(0.5)
    GPIO.output(gled_pin,GPIO.LOW)
    print('standardsign end')
def main(): 
    #darknet.exit()
    #部屋情報の登録
    print('部屋情報を取得します')
    f = open('/home/team-emb/gra_thesis/roominfo.txt','r') #テキストから部屋情報取得
    roomArea = f.readline()
    densityRule = f.readline()
    f.close()
    room = Room(int(roomArea),int(densityRule))
    print('この部屋の規定では、' + str(room.standardDR) + '人までが滞在可能です')
    print('------------------------------\n')
    
    #condition1 = Condition(0,0,0,0)
    warningLevel = WarningLevel()
    risk = Risk()
  
    global stb_mode
    global slm_mode
    global risk_configured
    standby_necessity = True
    f1 = open('test.txt', 'w')
    f2 = open('exp.txt', 'w')
    f3 = open('/home/team-emb/gra_thesis/persons.txt','r') #シミュレーション用
    t3 = threading.Thread(target=standbySign)
    t4 = threading.Thread(target=standardSign)
    t5 = threading.Thread(target=co2Alert)
    t6 = threading.Thread(target=lvAlert)
    
    #環境モニタリング
    try:
        while True:        
            now = datetime.datetime.now()
            today = datetime.date.today()
            if datetime.time(8,00) > datetime.time(now.hour,now.minute): #8時以前の場合のスリープ
                sleeptime = datetime.datetime(now.year,now.month,now.day,8,00) - datetime.datetime.now()
                ledClear()
                f1.write('sleep = '+str(sleeptime)+'\n')
                time.sleep(sleeptime.seconds)               
            elif datetime.time(20,00) < datetime.time(now.hour,now.minute): #20時以降の場合のスリープ
                sleeptime = datetime.datetime((datetime.datetime.now() + datetime.timedelta(days = 1)).year,(datetime.datetime.now() + datetime.timedelta(days = 1)).month,
                (datetime.datetime.now() + datetime.timedelta(days = 1)).day,8,00) - datetime.datetime.now()
                ledClear()
                f1.write('sleep = '+str(sleeptime)+'\n')
                time.sleep(sleeptime.seconds)
            starttime = datetime.datetime.now()
            f1.write('starttime = '+str(starttime)+'\n')

            formerDataNum = len(dbFunction.get_all_record(today))
            dataNum = formerDataNum   
          
            t1 = threading.Thread(target=startSign)
            t1.start()
            t2 = threading.Thread(target=jetsonReceive.main) #受信開始
            t2.setDaemon(True)
            t2.start()
            dbFunction.regi_enter_risk('NONE')
            t1.join()
            roopcnt = 0

            while t2.isAlive() == True: #受信機能稼働時のみ実行
                dataNum = formerDataNum #前回取得時データ数をデータ数とする
                print(dataNum)
                cnt = 0
                while True:
                    if t2.isAlive() != True:
                        break
                    nowTime = datetime.datetime.now()
                    timeRange = datetime.datetime(now.year,now.month,now.day,(nowTime - datetime.timedelta(minutes = 5)).hour,(nowTime - datetime.timedelta(minutes = 5)).minute) #データベースからの取得を、5分前以降のものに限定する
                    devRist = dbFunction.get_recent_dev(timeRange) #直近5分間にデータを更新したデバイスリスト
                    devNum = len(devRist) #接続デバイス数
                    print(devRist)
                    if devNum != 0: #デバイス数が0台でなくなればモニタリング再開
                        stb_mode = False
                        break
                    if t3.isAlive() == False and standby_necessity == True: #5分間更新が確認されてない場合、情報の信憑性が損なわれるためスタンバイサインのみを出す
                        stb_mode = True
                        t3 = threading.Thread(target=standbySign)
                        t3.start()
                    time.sleep(5.0)
                    cnt += 1
                    if cnt == 60:
                        standby_necessity = True
                        slm_mode = False
                        if t4.isAlive() == True: #標準警戒レベル監視モードサイン終了を待つ
                            t4.join()
                        if t3.isAlive() == True: #データ収集待ちサイン終了を待つ
                            t3.join()  
                        ledClear()
                print(devRist)
                print("デバイス台数:" + str(devNum))
                if t3.isAlive() == True: #接続デバイス確認待ちサイン終了を待つ
                    t3.join()  
                cnt = 0
                while dataNum != formerDataNum + devNum: #データ数が、前回取得時からデバイス台数分増えているか
                    if t2.isAlive() != True:
                        break
                    if t3.isAlive() == False and standby_necessity == True: #5分間更新が確認されてない場合、情報の信憑性が損なわれるためスタンバイサインのみを出す
                        stb_mode = True
                        t3 = threading.Thread(target=standbySign)
                        t3.start()
                    dataNum = len(dbFunction.get_all_record(today)) #今日取得されたデータ数
                    time.sleep(5.0)
                    cnt += 1
                    if cnt == 60:
                        standby_necessity = True
                        slm_mode = False
                        if t4.isAlive() == True: #標準警戒レベル監視モードサイン終了を待つ
                            t4.join()
                        if t3.isAlive() == True: #データ収集待ちサイン終了を待つ
                            t3.join() 
                        ledClear() 
                if t2.isAlive() != True:
                        break
                #capture.capImage() #カメラ画像取得      
                #numofPersons = countPersons.count() #人数推定
                print("input:")
                #numofPersons = random.randint(12,22)
                numofPersons = f3.readline()
                print(numofPersons)
                
                formerDataNum = dataNum #上記で取得したデータ数
                stb_mode = False
                if t3.isAlive() == True: #データ収集待ちサイン終了を待つ
                    t3.join()  

                roopcnt += 1                
                co2Concentration = checkRecentCo2(devRist,devNum) #直近5回分のデータ分析
                print('二酸化炭素濃度は:' + str(co2Concentration))
                if co2Concentration == -1: #データ数が5に満たないとき
                    dbFunction.regi_enter_risk('NONE')
                    standby_necessity = True
                    if t3.isAlive() == False and standby_necessity == True:
                        stb_mode = True
                        t3 = threading.Thread(target=standbySign)
                        t3.start()
                else:
                    standby_necessity = False
                    stb_mode = False
                    if t3.isAlive() == True: #データ収集待ちサイン終了を待つ
                        t3.join()                   
                    humidityCheck(devRist,devNum)
                    if modeSwitch(int(numofPersons),room.standardDR) == 1: #標準警戒レベルモニタリングモード
                        print('標準警戒レベルでの監視モードに入ります')
                        dbFunction.regi_enter_risk('GREEN')
                        if t4.isAlive() == False:
                            t4 = threading.Thread(target=standardSign)
                            t4.start()
                        if warningLevel.standardMonitering(co2Concentration) == 1: #標準警戒レベルでのco2上限をオーバーしているとき
                            t5 = threading.Thread(target=co2Alert)
                            t5.start()              
                    else: #環境評価モニタリングモード
                        if t4.isAlive() == True and slm_mode == False: #標準警戒レベルモニタリング時サイン終了を待つ
                            t4.join()
                        warningLevel.levelConfig(int(warningLevel.level),co2Concentration) #警戒レベル判定
                        print('警戒レベルは' + str(warningLevel.level))
                        if warningLevel.is_levelChanged(int(warningLevel.level)) == 1: #レベル上昇判定
                            t6 = threading.Thread(target=lvAlert)
                            t6.start()
                        warningLevel.regulationConfig(warningLevel.level,room.standardDR)
                        print(str(warningLevel.personsLowerLim) + '人から' +str(warningLevel.personsUpperLim) + '人までに制限されています')                                   
                        risk.riskConfig(int(numofPersons),warningLevel.personsLowerLim,warningLevel.personsUpperLim) #リスク判定
                        print('現在のリスクレベルは' + risk.riskLevel + 'です')                   
                    print('------------------------------\n')
                f1.write(str(roopcnt)+'回目:'+str(datetime.datetime.now())+'\n') 
                dbFunction.add_results(datetime.datetime.now(),co2Concentration,warningLevel.level,numofPersons,risk.riskLevel)
                f2.write(str(datetime.datetime.now())+'   '+ str(warningLevel.level)+'   '+ str(numofPersons) +'   '+ risk.riskLevel +'   '+str(co2Concentration)+'\n')                     
            slm_mode = False
            stb_mode = False
            standby_necessity = True
            if t3.isAlive() == True:
                t3.join()
            if t4.isAlive() == True:
                t4.join()
            dbFunction.regi_enter_risk('NONE')
            sleepintime = datetime.datetime.now()
            f1.write('sleepintime = '+str(sleepintime))

    
    except KeyboardInterrupt:
        pass
    finally:
        dbFunction.regi_enter_risk('NONE')
        slm_mode = False
        stb_mode = False
        if t3.isAlive() == True:
            t3.join()
        if t4.isAlive() == True:
            t4.join()
        if t5.isAlive() == True:
            t5.join()
        if t6.isAlive() == True:
            t6.join()
        GPIO.output(gled_pin,0)
        GPIO.output(yled_pin,0)
        GPIO.output(rled_pin,0)
        GPIO.output(hled_pin,0)
        GPIO.cleanup()  # cleanup all GPIO   
        f.close() 
        f2.close() 
        jetsonReceive.clear_led()
        print('cleanup')

if __name__ == '__main__':
    main()
