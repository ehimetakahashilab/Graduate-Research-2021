# -*-coding: utf-8 -*-

import ctypes
import socket
import mysql.connector as mysql
from datetime import datetime

import connection
import db_reset

sasl2_lib = ctypes.cdll.LoadLibrary('./sasl2.so')
sasl2_lib.An1.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))
sasl2_lib.alpha.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))
sasl2_lib.judge.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))
sasl2_lib.Mn1.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))
sasl2_lib.judge.restype = ctypes.c_int

crypto_lib = ctypes.cdll.LoadLibrary('./crypto.so')
crypto_lib.sd_date.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_int))
crypto_lib.An2.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))
crypto_lib.alpha.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))
crypto_lib.Mn2.argtypes = (ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte))


### 認証データなどの初期値 ###
# client1 #
_password_1 = [183, 22, 131, 83, 98, 149, 100, 231, 28, 242, 145, 26, 21, 129, 20, 253]
_An_1 = [178, 245, 112, 211, 180, 246, 130, 31, 124, 110, 240, 62, 180, 60, 207, 37]
_An1_1 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_An2_1 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_Mn_1 = [162, 26, 250, 146, 155, 39, 187, 14, 46, 181, 163, 232, 188, 207, 146, 118]
_Mn1_1 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_Mn2_1 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
password_1 = (ctypes.c_ubyte*len(_password_1))(*_password_1)
An_1 = (ctypes.c_ubyte*len(_An_1))(*_An_1)
An1_1 = (ctypes.c_ubyte*len(_An1_1))(*_An1_1)
An2_1 = (ctypes.c_ubyte*len(_An2_1))(*_An2_1)
Mn_1 = (ctypes.c_ubyte*len(_Mn_1))(*_Mn_1)
Mn1_1 = (ctypes.c_ubyte*len(_Mn1_1))(*_Mn1_1)
Mn2_1 = (ctypes.c_ubyte*len(_Mn2_1))(*_Mn2_1)
cnt_1 = 1
# client2 #
_password_2 = [33, 33, 29, 44, 215, 98, 175, 192, 226, 120, 227, 33, 184, 80, 201, 110]
_An_2 = [192, 32, 187, 98, 41, 116, 108, 17, 50, 136, 76, 143, 79, 59, 60, 47]
_An1_2 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_An2_2 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_Mn_2 = [18, 255, 254, 102, 154, 146, 47, 183, 214, 94, 14, 237, 19, 100, 168, 251]
_Mn1_2 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_Mn2_2 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
password_2 = (ctypes.c_ubyte*len(_password_2))(*_password_2)
An_2 = (ctypes.c_ubyte*len(_An_2))(*_An_2)
An1_2 = (ctypes.c_ubyte*len(_An1_2))(*_An1_2)
An2_2 = (ctypes.c_ubyte*len(_An2_2))(*_An2_2)
Mn_2 = (ctypes.c_ubyte*len(_Mn_2))(*_Mn_2)
Mn1_2 = (ctypes.c_ubyte*len(_Mn1_2))(*_Mn1_2)
Mn2_2 = (ctypes.c_ubyte*len(_Mn2_2))(*_Mn2_2)
cnt_2 = 1
# client3 #
_password_3 = [201, 140, 96, 184, 12, 154, 6, 166, 234, 126, 65, 230, 28, 54, 106, 26]
_An_3 = [250, 57, 154, 7, 109, 193, 128, 199, 238, 219, 69, 144, 230, 94, 210, 38]
_An1_3 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_An2_3 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_Mn_3 = [166, 171, 52, 18, 117, 207, 24, 65, 15, 89, 207, 94, 234, 138, 28, 48]
_Mn1_3 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
_Mn2_3 = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
password_3 = (ctypes.c_ubyte*len(_password_3))(*_password_3)
An_3 = (ctypes.c_ubyte*len(_An_3))(*_An_3)
An1_3 = (ctypes.c_ubyte*len(_An1_3))(*_An1_3)
An2_3 = (ctypes.c_ubyte*len(_An2_3))(*_An2_3)
Mn_3 = (ctypes.c_ubyte*len(_Mn_3))(*_Mn_3)
Mn1_3 = (ctypes.c_ubyte*len(_Mn1_3))(*_Mn1_3)
Mn2_3 = (ctypes.c_ubyte*len(_Mn2_3))(*_Mn2_3)
cnt_3 = 1

### データベースを初期化 ###
db_reset.reset_table_1()
db_reset.reset_table_2()
db_reset.reset_table_3()


### ソケット通信 ###
IPADDR = "192.168.2.110"
PORT = 49152
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((IPADDR, PORT))
sock.listen(3)


while True:
    while True:
        ### client --- (request) ---> server ###
        print("////////////  client --- (request) ---> server  ////////////")
        conn = connection.connect(sock)
        data = connection.receive_reqest(conn)
        print("request: ", end = " ")
        for num in range(len(data)):
            print(data[num], end = " ")
        print("")
        if data[0] == 1:
            print("client1: Start Connect.")
            print("")
            password = password_1
            An = An_1
            An1 = An1_1
            An2 = An2_1
            Mn = Mn_1
            Mn1 = Mn1_1
            Mn2 = Mn2_1
            table = 'table_1'
            cnt = cnt_1
        elif data[0] == 2:
            print("client2: Start Connect.")
            print("")
            password = password_2
            An = An_2
            An1 = An1_2
            An2 = An2_2
            Mn = Mn_2
            Mn1 = Mn1_2
            Mn2 = Mn2_2
            table = 'table_2'
            cnt = cnt_2
        elif data[0] == 3:
            print("client3: Start Connect.")
            print("")
            password = password_3
            An = An_3
            An1 = An1_3
            An2 = An2_3
            Mn = Mn_3
            Mn1 = Mn1_3
            Mn2 = Mn2_3
            table = 'table_3'
            cnt = cnt_3
        else:
            print("Error: Unregisterd Client.")
            print("")

    
        ### An+1 ← H(S XOR N+1) ###
        print("////////////  An+1 ← H(S XOR N+1)  ////////////")
        sasl2_lib.An1(ctypes.cast(ctypes.pointer(password),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)))
        print("An+1: ", end = " ")
        for num in range(len(An1)):
            print(An1[num], end = " ")
        print("")
        print("")


        ### α ← An+1 XOR An XOR Mn ###
        print("////////////  α ← An+1 XOR An XOR Mn  ////////////")
        _alpha = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        alpha = (ctypes.c_ubyte*len(_alpha))(*_alpha)
        sasl2_lib.alpha(ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(Mn),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(alpha),ctypes.POINTER(ctypes.c_ubyte)))
        print("alpha: ", end = " ")
        for num in range(len(alpha)):
            print(alpha[num], end = " ")
        print("")
        print("")


        ### server --- (alpha) ---> client ###
        print("////////////  server --- (alpha) ---> client  ////////////")
        connection.send(conn, alpha)
        print("send alpha.")
        print("")


        ### client --- (beta) ---> server ###
        print("////////////  client --- (beta) ---> server  ////////////")
        _beta = connection.receive(conn)
        if _beta == 0:
            print("RECEIVE ERROR: client", data[0])
            connection.close(sock, conn)
            print("close connection.")
            print("")
            break
        beta = (ctypes.c_ubyte*len(_beta))(*_beta)
        print("beta: ", end = " ")
        for num in range(len(beta)):
            print(beta[num], end = " ")
        print("")
        print("")


        ### An+1 + An = β ? ###
        print("////////////  An+1 + An = β ?  ////////////")
        result = sasl2_lib.judge(ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(beta),ctypes.POINTER(ctypes.c_ubyte)))
        print("")


        ### server --- (result) ---> client ###
        print("////////////  server --- (result) ---> client  ////////////")
        if result == 1:
            print("Success Authentication.")
            _flag = [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            flag = (ctypes.c_ubyte*len(_flag))(*_flag)
            connection.send(conn, flag)
            print("send result.")
            sasl2_lib.Mn1(ctypes.cast(ctypes.pointer(An),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(Mn),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(Mn1),ctypes.POINTER(ctypes.c_ubyte)))
            print("Mn+1: ", end = " ")
            for num in range(len(Mn1)):
                print(Mn1[num], end = " ")
            print("")
            print("")
            if data[0] == 1:
                An1_1 = An1
                Mn1_1 = Mn1
            elif data[0] == 2:
                An1_2 = An1
                Mn1_2 = Mn1
            elif data[0] == 3:
                An1_3 = An1
                Mn1_3 = Mn1
        elif result == 0:
            print("Authentication failed.")
            _flag = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            flag = (ctypes.c_ubyte*len(_flag))(*_flag)
            connection.send(conn, flag)
            print("send result.")
            connection.close(sock, conn)
            print("close connection.")
            print("")
            break


        ### client --- (gamma) ---> server ###
        feedback_cnt = 0
        for num in range(10):
            if num == 0:
                if data[0] == 1:
                    An = An_1
                    An1 = An1_1
                    An2 = An2_1
                    Mn = Mn_1
                    Mn1 = Mn1_1
                    Mn2 = Mn2_1
                elif data[0] == 2:
                    An = An_2
                    An1 = An1_2
                    An2 = An2_2
                    Mn = Mn_2
                    Mn1 = Mn1_2
                    Mn2 = Mn2_2
                elif data[0] == 3:
                    An = An_3
                    An1 = An1_3
                    An2 = An2_3
                    Mn = Mn_3
                    Mn1 = Mn1_3
                    Mn2 = Mn2_3

            print("////////////  client --- (gamma) ---> server  ////////////")
            _gamma = connection.receive(conn)
            if _gamma == 0:
                print("RECEIVE ERROR: client", data[0])
                connection.close(sock, conn)
                print("")
                break
            gamma = (ctypes.c_ubyte*len(_gamma))(*_gamma)
            print("gamma:", end = " ")
            for num in range(len(gamma)):
                print(gamma[num], end = " ")
            print("")
            print("")


            ### SD, date ← γ XOR An+1 XOR An ###
            print("////////////  SD, date ← γ XOR An+1 XOR An  ////////////")
            _SD = [0]
            SD = (ctypes.c_float*len(_SD))(*_SD)
            _DATE = [0, 0, 0, 0, 0, 0]
            DATE = (ctypes.c_int*len(_DATE))(*_DATE)
            crypto_lib.sd_date(ctypes.cast(ctypes.pointer(gamma),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(SD),ctypes.POINTER(ctypes.c_float)), ctypes.cast(ctypes.pointer(DATE),ctypes.POINTER(ctypes.c_int)))
            print("SD: ", SD[0])
            print("DATE:", end = " ")
            for num in range(len(DATE)):
                print(DATE[num], end = " ")
            print("")
            print("")


            ### server --- (SD) ---> database ###
            myconn = mysql.connect(
                host = 'localhost',
                user = 'root',
                password = 'password',
                db = 'sd_db')
            myconn.ping(reconnect=True)

            cursor = myconn.cursor()

            comm_1 = "INSERT INTO "
            comm_2 = " (No, year, month, day, hour, min, sec, sd) VALUES (%s, %s, %s, %s, %s, %s, %s, %s)"
            comm = comm_1 + table + comm_2
            cursor.execute(comm, (cnt, DATE[0], DATE[1], DATE[2], DATE[3], DATE[4], DATE[5], SD[0]))
            cnt = cnt + 1

            myconn.commit()
    
            cursor.close()
            myconn.close()

            if data[0] == 1:
                cnt_1 = cnt
            elif data[0] == 2:
                cnt_2 = cnt
            elif data[0] == 3:
                cnt_3 = cnt



            ### An+2 ← H(S XOR N+2) ###
            print("////////////  An+2 ← H(S XOR N+2)  ////////////")
            crypto_lib.An2(ctypes.cast(ctypes.pointer(password),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An2),ctypes.POINTER(ctypes.c_ubyte)))
            print("An+2: ")
            for num in range(len(An2)):
                print(An2[num], end = " ")
            print("")
            print("")


            ### α ← An+2 XOR An+1 XOR Mn+1 ###
            print("////////////  α ← An+2 XOR An+1 XOR Mn+1  ////////////")
            _alpha = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            alpha = (ctypes.c_ubyte*len(_alpha))(*_alpha)
            crypto_lib.alpha(ctypes.cast(ctypes.pointer(An2),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(Mn1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(alpha),ctypes.POINTER(ctypes.c_ubyte)))
            print("alpha: ", end = " ")
            for num in range(len(alpha)):
                print(alpha[num], end = " ")
            print("")
            print("")


            ### Mn+2 ← An+1 + Mn+1 ###
            crypto_lib.Mn2(ctypes.cast(ctypes.pointer(An1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(Mn1),ctypes.POINTER(ctypes.c_ubyte)), ctypes.cast(ctypes.pointer(Mn2),ctypes.POINTER(ctypes.c_ubyte)))
            for num in range(len(Mn2)):
                print(Mn2[num], end = " ")
            print("")
            print("")


            ### server --- (alpha) ---> client ###
            print("////////////  server --- (alpha) ---> client  ////////////")
            connection.send(conn, alpha)
            print("send alpha.")
            print("")


            ### An ← An+1, An+1 ← An+2, Mn ← Mn+1, Mn+1 ← Mn+2 ###
            An = An1
            An1 = An2
            An2 = (ctypes.c_ubyte*len(_An2_1))(*_An2_1)
            Mn = Mn1
            Mn1 = Mn2
            Mn2 = (ctypes.c_ubyte*len(_Mn2_1))(*_Mn2_1)
            feedback_cnt = feedback_cnt + 1
            print("feedback_cnt :",feedback_cnt)
        

        if feedback_cnt == 10:
            ### feedback ###
            print("////////////  server --- (feedback) ---> client  ////////////")
            _fb = [1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            fb = (ctypes.c_ubyte*len(_fb))(*_fb)
            connection.send(conn, fb)
            print("send feedback.")
            print("")
            print("")
            

            ### An ← An+1, Mn ← Mn+1 ###
            if data[0] == 1:
                An_1 = An1
                Mn_1 = Mn1
                print("An: ")
                for num in range(len(An_1)):
                    print(An_1[num], end = " ")
                print("")
                print("Mn: ")
                for num in range(len(Mn_1)):
                    print(Mn_1[num], end = " ")
                print("")
                print("")
            elif data[0] == 2:
                An_2 = An1
                Mn_2 = Mn1
                print("An: ")
                for num in range(len(An_2)):
                    print(An_2[num], end = " ")
                print("")
                print("Mn: ")
                for num in range(len(Mn_2)):
                    print(Mn_2[num], end = " ")
                print("")
                print("")
            elif data[0] == 3:
                An_3 = An1
                Mn_3 = Mn1
                print("An: ")
                for num in range(len(An_3)):
                    print(An_3[num], end = " ")
                print("")
                print("Mn: ")
                for num in range(len(Mn_3)):
                    print(Mn_3[num], end = " ")
                print("")
                print("")

            ### close ###
            connection.close(sock, conn)
            if data[0] == 1:
                print("client", data[0], " End Connect.")
            elif data[0] == 2:
                print("client", data[0], " End Connect.")
            elif data[0] == 3:
                print("client", data[0], " End Connect.")

                
        break
