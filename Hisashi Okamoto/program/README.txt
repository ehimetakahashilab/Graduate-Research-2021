使用ツール：Quartus Prime 18.1 Lite Edition
使用FPGA:MAX10(FPGA開発ボード:DE10-Lite)
.v:verilog HDLのソースコード
_tb:シミュレーション用のテストベンチ
.c:c言語のソースコード
コンパイルするにはTopModuleにあるモジュールが必要(なければそのまま実行可能)
adr_reg:アドレスレジスタ
ALU:ステートマシンと演算器の役割をもつモジュール
data_in32:データ受信モジュール
data_out32:データ送信モジュール
ram:メモリ
  メモリに関してはライブラリにあるものを使用したため、ram.vとram_bb.vをダウンロードしても動作しないかもしれないので、その場合はQuartus Primeのライブラリから選び生成する。
SAS-L2_client:SAS認証回路
SAS-L2_server:サーバ側の処理
  SASL2.qsys:NIOSⅡのロジック回路
  SASL2.sopcinfo:サーバ側の処理を行うハードウェアの情報
