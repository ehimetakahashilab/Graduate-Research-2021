# -*-coding: utf-8 -*-
import socket
import time


def connect(sock):
    conn, addr = sock.accept()
    print("connect:", conn)
    return conn


def receive_reqest(conn):
    data = conn.recv(1024)
    print("recieve data")
    #print("data -> ", data)
    return data


def receive(conn):
    conn.settimeout(5.0)
    try:
        data = conn.recv(1024)
        print("recieve data")
    except socket.timeout:
        data = 0
    #print("data -> ", data)
    return data


def send(conn, data):
    conn.send(data)
    print("send data")


def close(sock, conn):
    conn.close()
    print("close connection.")
