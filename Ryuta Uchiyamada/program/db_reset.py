import mysql.connector as mysql
from datetime import datetime

def reset_table_1():
    table = 'table_1'
    
    # コネクションの作成
    connection = mysql.connect(
        host = 'localhost',
        user = 'root',
        password = 'password',
        db = 'sd_db')
    
    # テーブル初期化
    cursor = connection.cursor()
    cursor.execute("DROP TABLE IF EXISTS %s;" % table)
    
    # テーブル作成
    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS %s(
        No INT(100) NOT NULL,
        year INT(1) NOT NULL,
        month INT(1) NOT NULL,
        day INT(1) NOT NULL,
        hour INT(1) NOT NULL,
        min INT(1) NOT NULL,
        sec INT(1) NOT NULL,
        sd FLOAT(1) NOT NULL,
        PRIMARY KEY (No))
        """ % table)

    # データを表示
    cursor.execute("SELECT * FROM %s ORDER BY No ASC" % table)
    rows = cursor.fetchall()
    for row in rows:
        print(row)

    # コミット
    connection.commit()

    # カーソルとコネクションを閉じる
    cursor.close()
    connection.close()


def reset_table_2():
    table = 'table_2'
    
    # コネクションの作成
    connection = mysql.connect(
        host = 'localhost',
        user = 'root',
        password = 'password',
        db = 'sd_db')
    
    # テーブル初期化
    cursor = connection.cursor()
    cursor.execute("DROP TABLE IF EXISTS %s;" % table)
    
    # テーブル作成
    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS %s(
        No INT(100) NOT NULL,
        year INT(1) NOT NULL,
        month INT(1) NOT NULL,
        day INT(1) NOT NULL,
        hour INT(1) NOT NULL,
        min INT(1) NOT NULL,
        sec INT(1) NOT NULL,
        sd FLOAT(1) NOT NULL,
        PRIMARY KEY (No))
        """ % table)

    # データを表示
    cursor.execute("SELECT * FROM %s ORDER BY No ASC" % table)
    rows = cursor.fetchall()
    for row in rows:
        print(row)

    # コミット
    connection.commit()

    # カーソルとコネクションを閉じる
    cursor.close()
    connection.close()


def reset_table_3():
    table = 'table_3'
    
    # コネクションの作成
    connection = mysql.connect(
        host = 'localhost',
        user = 'root',
        password = 'password',
        db = 'sd_db')
    
    # テーブル初期化
    cursor = connection.cursor()
    cursor.execute("DROP TABLE IF EXISTS %s;" % table)
    
    # テーブル作成
    cursor.execute(
        """
        CREATE TABLE IF NOT EXISTS %s(
        No INT(100) NOT NULL,
        year INT(1) NOT NULL,
        month INT(1) NOT NULL,
        day INT(1) NOT NULL,
        hour INT(1) NOT NULL,
        min INT(1) NOT NULL,
        sec INT(1) NOT NULL,
        sd FLOAT(1) NOT NULL,
        PRIMARY KEY (No))
        """ % table)

    # データを表示
    cursor.execute("SELECT * FROM %s ORDER BY No ASC" % table)
    rows = cursor.fetchall()
    for row in rows:
        print(row)

    # コミット
    connection.commit()

    # カーソルとコネクションを閉じる
    cursor.close()
    connection.close()
