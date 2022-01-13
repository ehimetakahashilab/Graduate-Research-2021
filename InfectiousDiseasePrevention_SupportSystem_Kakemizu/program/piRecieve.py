import serial
import traceback
import time
import decode

class UART:

    def __init__(self):

        # シリアル通信設定　
        # ボーレートはラズパイのデフォルト値:115200に設定
        try:
            self.uartport = serial.Serial(
                port="/dev/ttyS0",
                baudrate=115200,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=None,
            )
        except serial.SerialException:
            print(traceback.format_exc())

        # 受信バッファ、送信バッファクリア
        self.uartport.reset_input_buffer()
        self.uartport.reset_output_buffer()
        time.sleep(1)

    # データ送信関数
    def send_serial(self, cmd): 

        print("send data : {0}".format(cmd))
        try:
            # 改行コードを必ずつけるために、1回削除して、送信時に再度付与する
            cmd = cmd.rstrip()
             # 改行コードを付与　文字列をバイナリに変換して送信
            self.uartport.write((cmd + "\n").encode("utf-8"))
        except serial.SerialException:
            print(traceback.format_exc())

    # データ受信関数
    def receive_serial(self):

        try:
            rcvdata = self.uartport.readline()
        except serial.SerialException:
            print(traceback.format_exc())
    
        # 受信したバイナリデータを文字列に変換　改行コードを削除
        return rcvdata.decode("utf-8").rstrip() 

if __name__ == '__main__':
    uart = UART()
    while True:
        # ターミナルから入力された文字を取得
        #input_data = input("input data:")
        # ターミナルから入力された文字をWindowsに送信　
        #uart.send_serial(input_data)
        # データ受信 
        data = uart.receive_serial() 
        # 受信データを表示
        print("recv data : {0}".format(data))
        data1 = decode.sencer_data(data)
        data1.print_all()