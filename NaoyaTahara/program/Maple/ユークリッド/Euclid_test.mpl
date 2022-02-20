
#Mapleの操作説明
#文字を打ち込みたいときは先頭に#
#改行はShift + Enter
#行の削除はCtrl + Delete
#行の挿入はCtrl + K
#Mathの横のタブで文字のスタイルを変更できる(これはText)

;
restart;
with(StringTools):
str := "tahara";
L := Length(str):
#A_QRコードを生成するもととなる画像のパスを""内に入力
;
image_path := "./pic/Lenna.jpg";
#ガロア体GF(2^8)の定義
#今回は2の8乗なのでガロア体上の原子多項式G8をこのように定義する
#GFに関しては　https://www.maplesoft.com/support/helpjp/maple/view.aspx?path=GF　を参照のこと
;
G8 := GF(2, 8, alpha^8+alpha^4+alpha^3+alpha^2+1);
#2の8乗のガロア体からMapleの積和(Maple sum of product)
#原始元a を定義している
;
a := G8:-ConvertIn(alpha);
# `こんなこともできる`;
hoge := G8:-random(alpha);
# QRコード、バージョン1、誤り訂正レベルMの定義（https://sites.google.com/a/osshc.co.cc/web/studies/it/qr）
# 以下、岡崎高校の資料を資料１と呼ぶ
# 埋め草シンボルの数 khat の定義
# n:総コード語数
# k:訂正レベルMの時必要なデータは128ビットで、バイトに直すと16バイトである。1バイトの情報群を今回は1コード語という。
# k:データコード語数、情報語、符号化前の元のデータの語数といえる
# khat:埋め草シンボルといい、符号化する情報数によって変わる値である。
# khatはまた後ほど定義する
# n,k を定義
#ここからは入力した文字列からkhatを求める作業
#資料１を参考にしている
#埋め草コード数khatの値を求めていく
;
#８ビットバイトモードなので一文字あたり1コード語(1バイト)
#先頭にモード指定子0.5コード語、文字数指定子1コード語を追加する
#データ部の終わりに終端符号を0.5コード語分追加するので、モード指定子を合わせて1とする
#これらの情報から埋め草コード数を求める
;
#以下は8ビットバイトモードのkhat
;
khat := 16 - (1 + 1 + L);
n:=26: k:=16:
#符号を生成するための生成多項式GPを定義する
#QRコードのバージョン１誤り訂正レベルMの誤り訂正コード語数(検査符号数？)は10なので
#次数10の生成多項式を用いるとうまくいくらしい(検査符号分横にずらすアレ)
#今回は生成多項式の a^t の t のみを連ねたベクトルで表現する
#内容は　https://sites.google.com/a/osshc.co.cc/web/studies/it/qr　の表6を参照のこと
#最初が 0 なのは a^0 = 1 を表している
# n-k+1とは 総コード数 - データコード数 + 1(調整) であり、これが生成多項式の次数となっている
#Vectorとはベクトルデータ構造を表している
#Vectorに関しては　https://www.maplesoft.com/support/helpJP/Maple/view.aspx?path=Vector　を参照のこと
;
GP := Vector(n-k+1,[0,251,67,46,61,118,70,64,94,32,45]);
#生成行列# Gの作成（「QRコードの符号化・復号アルゴリズム解説」佐藤創（専修大学）をネットで検索して読んで下さい）
# #上記の資料を以下、資料２と呼ぶ
;
G := Matrix(k,n);
#すべてのGの要素に対して、ガロア体を適用
## G8:-input(0) を G8:-random(alpha) とかにするとランダムに要素が決まる
;
for i from 1 to k do
  for j from 1 to n do
    G[i, j] := G8:-input(0);
  end do;
end do;
G
;
#資料２の(4)を参照
;
for i from 1 to k do
  for j from 1 to 11 do
    G[i,(i-1)+j] := G8:-`^`(a,GP[j]);
  end do;
end do;
#上プログラムの3行目について
#ガロア体上のアルファGP[x]乗を行列の要素に格納している
#例
b := G8:-`^`(a,GP[2]);
#資料１の表４を参照するとよく確認できる
;
G8 :-`^`(a,2);
G
;
#delta#  : 符号に現れる、情報シンボル(データ部)の位置(1~(k-khat))と埋め草シンボルの位置(k-khat~nのどこかのkhat個)を 定義
# 
# #本来、生成行列Gは情報部(データ部+埋め草シンボル)が基底ベクトルで1列目から情報部の長さ分、単位ベクトルのように並んでいる
# #そして残りの検査部は何らかの値になっている。Gは縦の長さが情報部の長さ、横の長さが情報部+検査部である
# #しかしランダム法において、情報部のうちデータ部の位置は変えられないが、埋め草シンボルや検査部の位置は自由に選択できることになっている
# 
# #deltaは何列目が基底ベクトルになっているのかを表したものである
# #deltaにおいて、1からしばらく数字が順序良く並んでいるのは、その部分がデータ部であるからである
# #まず、deltaを情報部の長さ分とり、初期化する
;
delta := Vector(k);
#ランダム法において、埋め草シンボルの位置はデータ部から行列の最後までの範囲なら自由に選択できるので、
#rand関数を用いて埋め草シンボルの位置をランダムに決めている
#myrand:自由に選択できる範囲からランダムに値を返してくれる関数、埋め草シンボルの位置を決めている
#myset:ランダムな値を格納していく配列
#<> : ノットイコール
#nops:行列の要素数   https://www.maplesoft.com/support/help/Maple/view.aspx?path=op
#mylistがrestartすると一緒になる
;
for i from 1 to k-khat do
  delta[i] := i;
end do:
randomize():
myrand := rand(k-khat+1..n):
myset := {}:
while (nops(myset)<>khat) do
  myset := `union`(myset, {myrand()});
end do:
mylist := convert(myset, list);

#mylist:=[13,14,15,16];#####Debug

for i from 1 to khat do
  delta[i+k-khat] := mylist[i];
end do:
delta;
with(ImageTools):
#関数：get_resize_binary_image
#入力：画像のパス
#出力：元画像を21*21に縮小し、グレイスケールにしてからマスクをかけ二値化した画像

get_resize_binary_image := proc(image_path);
local i,j,image,resize_height,resize_width,resize_image,resize_gray_image,avarage_luminance,resize_binary_image;

image := Read(image_path);
resize_height := 21/Height(image);
resize_width := 21/Width(image);
resize_image := Scale(image,resize_height,resize_width); Preview(resize_image);
resize_gray_image := RGBtoGray(resize_image);
avarage_luminance := 0;
for i from round(21/4) by 1 to round(21*3/4) do
  for j from round(21/4) by 1 to round(21*3/4) do
    avarage_luminance := avarage_luminance + resize_gray_image[i][j];
  end do;
end do;
avarage_luminance := avarage_luminance/((round(21*3/4) - round(21/4))^2);
resize_binary_image := Create(21,21,1);
for i from 1 by 1 to 21 do
  for j from 1 by 1 to 21 do
    if (resize_gray_image[i,j] >= avarage_luminance) then
      resize_binary_image[i,j] := 1;
    end if;
  end do;
end do;

return resize_binary_image;
end proc:
#Preview(Scale(Read(image_path),63/225,63/225));
#生成行列Gを変形させランダム法が適用できる形にしている
;
for i from 1 to k do
  inv := G8:-inverse(G[i,delta[i]]):
  for j from 1 to n do
    G[i,j] := G8:-`*`(inv, G[i,j]);
  end do;
  for l from 1 to i-1 do
    tmp := G[l,delta[i]];
    for j from 1 to n do
      G[l,j] := G8:-`-`(G[l,j], G8:-`*`(tmp, G[i,j]));
    end do;
  end do;
  for l from i+1 to k do
    tmp := G[l,delta[i]];
    for j from 1 to n do
      G[l,j] := G8:-`-`(G[l,j], G8:-`*`(tmp, G[i,j]));
    end do;
  end do;
end do:
G
;
#情報を2進数化したものを8ビット毎に区切り(区切ったひとかたまりを１シンボルと呼ぶ)、
#１シンボルをアルファの何とか乗で表す。
#毎シンボルごとのアルファの指数をリストにしたものをFPとし、
#その元となる2進数化した情報をFP_binとする
#今回は扱いやすいように8ビットバイトモードに変換する
;
#まずモード指定子を格納する
#8ビットバイトモードのモード指定子は0100
FP_bin := [0,1,0,0];
#Reverseなどを使用するため導入
with(ListTools):
#関数名：binarize
#入力1：10進数
#出力：入力を2進数にし、8ビット化したリスト

binarize := proc(dec):
local bin, i;
bin := Reverse(convert(dec,base,2)):
for i from 1 by 1 to 8-nops(bin) do
  bin := [0,op(bin)];
end do:
end proc:
#次に文字数指定子を追加する
FP_bin := [op(FP_bin),op(binarize(L))];
#各文字を8ビットに変換したものをFP_binに追加
Str_bin := ToByteArray(str);
for i from 1 by 1 to L do
  tmp := op(binarize(Str_bin[i]));
  FP_bin := [op(FP_bin),tmp];
end do:

FP_bin;
#終端パターンを追加(データビット数を超えることは無い)
FP_bin := [op(FP_bin),op([0,0,0,0])];
#埋め草コードの追加(廃止)
#j := 16 - nops(FP_bin)/8;
#for i from 1 by 1 to j do
#  if (i mod 2) <> 0 then
#    FP_bin := [op(FP_bin),op([1,1,1,0,1,1,0,0])];
#  else
#    FP_bin := [op(FP_bin),op([0,0,0,1,0,0,0,1])];
#  end if;
#end do:
FP_bin;
nops(FP_bin);

rewrited_binary_image := get_resize_binary_image(image_path);
rewrited_binary_image[1,1];
#関数名：get_padding_and_rewrite_binary_image
#入力１：画像から取得した埋め草コードを挿入したいリスト
#入力２：埋め草コードを取得する21*21に縮小された画像の行列
#入力３：埋め草コードの位置を示した行列
#入力４：総データコード数n
#入力５：データ部のコード数k
#入力６：埋め草コード数khat
#出力：埋め草コードを挿入したリスト

get_padding_and_rewrite_binary_image := proc(list,binary_image,delta,n,k,khat);
local i,j,p,cnt,padding_location,code_num;
local padding_list := list;
#注意：以下のグローバル変数を使っています
global rewrited_binary_image;

#code_num：何コード目かを示す
#deltaのうち埋め草コード部のみをpadding_locationに格納
padding_location := [];
for p from k-khat+1 by 1 to n do
  if member(p,delta) then
    padding_location := [op(padding_location),p];
  end if;
end do;

i:=21; j:=21; cnt:=1;
code_num := floor((cnt-1)/8) + 1;
while code_num <> n+1 do

#print(cnt);
#print("i=",i);
#print("j=",j);

  if code_num < k-khat+1 then
    rewrited_binary_image[i,j] := ((padding_list[cnt] + 1) mod 2);
  elif member(code_num, padding_location) then
    padding_list := [op(padding_list),floor(binary_image[i,j])];
  end if;

  #次のi,jを指定
  #↓強制左移動
  if ((i=1) and (j mod 4 = 0)) or ((i=21) and (j mod 4 = 2)) then
    j--;
  else
    if j = 1 then
      j++;
    else
      if (i+j) mod 2 = 1 then
        if j mod 4 < 2 then
          #上へ
          i--;
        else
          #下へ
          i++;
        end if;
      else
        if i mod 2 = 1 then
          #左へ
          j--;
        else
          #右へ
          j++;
        end if;
      end if;
    end if;
  end if;
cnt++;
code_num := floor((cnt-1)/8) + 1;
end do;

return padding_list;
end proc:
FP_bin := get_padding_and_rewrite_binary_image(FP_bin,get_resize_binary_image(image_path),delta,n,k,khat);
#8ビットに分割(8ビットバイトモードなのでちょうどに分割できるはず)
FP_bin := LengthSplit(FP_bin,8);
Preview(get_resize_binary_image(image_path)):
Preview(rewrited_binary_image):
#関数名：get_exp
#入力1：2の8乗のガロア体上の多項式表現
#出力：多項式表現の元となるアルファの冪

get_exp := proc(dec):
local exp, fin, pol;
exp := 0;
pol := G8:-input(dec);
fin := G8:-`^`(a,0);
if dec = 0 then
  exp := -1;
else
  while pol<>fin do
    pol := G8:-`/`(pol,a);
    exp := exp+1;
  end do:
end if;
return exp;
end proc:
get_exp(0);
FP_bin;
#FP_listを作成する
FP_list := []:
for i from 1 by 1 to 16 do
  #print("  i=    ",i);
  For_FP_list := Reverse(FP_bin[i]);
  #print(FP_bin[i]);
  #print(Rev,For_FP_list);
  dec := 0:
  for j from 1 by 1 to 8 do
    dec := dec + For_FP_list[j] * (2 ^ (j-1));
    #print("j=",j);
    #print("dec=",dec);
  end do:
  FP_list := [op(FP_list),get_exp(dec)];
  #print("For_FP_list : ",For_FP_list);
  #print("FP_list : ",FP_list);
end do:
FP_list;
FP := Vector(k, FP_list);
F := Vector(k);
#FはFP(アルファの指数のみを格納してある)をアルファのべき乗の形にしている
for i from 1 to k do
  if (FP[i]>=0) then
    F[i] := G8:-`^`(a, FP[i]);
  else
    F[i] := G8:-input(0);
  end if;
end do;
F;
# 
# 符号の計算 C=F*G
# 
C := Vector(n);
for i from 1 to n do
  C[i] := G8:-input(0);
  for j from 1 to k do
    C[i] := G8:-`+`(C[i], G8:-`*`(F[j], G[j, i]));
  end do;
end do:
# 
# 符号 C を表示してみる（ホームページの結果と比較して同じものが得られている、https://sites.google.com/a/osshc.co.cc/web/studies/it/qr）
# 
t := 5;
F[1];
F;
mylist;
Euclid_list := []:
j:=0:
for i from 1 by 1 to n do
  if i<=(k-khat) then
    Euclid_list := [op(Euclid_list),F[i]];
    j:=i;
  elif member(i,mylist) then
    j++;
    Euclid_list := [op(Euclid_list),F[j]];
  else
    Euclid_list := [op(Euclid_list),X];
  end if;
end do;
Euclid_list;
#erasure_location：消失誤り位置

erasure_location := []:
for i from 1 by 1 to n do
  if Euclid_list[i]=X then
    erasure_location := [op(erasure_location),i-1];
  end if;
end do;
erasure_location;
#関数名：change_0_to_zero
#入力１：多項式リスト
#出力：入力した多項式リスト内の0をG8上のzeroに変換
change_0_to_zero := proc(x);
local i,X;
X := x;

for i from 1 by 1 to nops(x) do
  if x[i]=0 then
    X[i] := G8:-zero;
  end if;
end do;

return X;
end proc:
#関数名：change_zero_to_0
#入力１：多項式リスト
#出力：入力した多項式リスト内のG8上のzeroを0に変換
change_zero_to_0 := proc(x);
local i,X;
X := x;

for i from 1 by 1 to nops(x) do
  if x[i] = G8:-zero then
    X[i] := 0;
  end if;
end do;

return X;
end proc:
#関数名：plus_on_G8
#入力１,２：多項式リスト
#出力：入力した多項式リスト同士の和の多項式リスト

plus_on_G8 := proc(x,y);
local i,j,long_vec,short_vec,length;

if nops(x) <= nops(y) then
  length := nops(x);
  long_vec := y;
  short_vec := x;
else
  length := nops(y);
  long_vec := x;
  short_vec := y;
end if;

long_vec := change_0_to_zero(long_vec);
short_vec := change_0_to_zero(short_vec);

for i from 1 by 1 to length do
  long_vec[i] := G8:-`+`(long_vec[i],short_vec[i]);
end do;

long_vec := change_zero_to_0(long_vec);

return long_vec;
end proc:
#関数名：multi_on_G8
#入力１,２：多項式リスト
#出力：入力した多項式リスト同士の積の多項式リスト

multi_on_G8 := proc(X,Y);
local i,j,result,x,y;

x := X;
y := Y;

for i from 1 by 1 to nops(x) do
  if x[i]=0 then
    x[i] := G8:-zero;
  end if;
end do;

for i from 1 by 1 to nops(y) do
  if y[i]=0 then
    y[i] := G8:-zero;
  end if;
end do;

result := [];
for i from 1 by 1 to nops(x)+nops(y)-1 do
  result := [op(result),G8:-zero];
end do;

for i from 1 by 1 to nops(x) do
  for j from 1 by 1 to nops(y) do
    result[i+j-1] := G8:-`+`(result[i+j-1], G8:-`*`(x[i],y[j]));
  end do;
end do;

for i from 1 by 1 to nops(result) do
  if result[i]=0 then
    result[i] := G8:-zero;
  end if;
end do;

return result;
end proc:
#関数名：division_on_G8
#入力１：割られる多項式リスト
#入力２：割る多項式リスト
#出力：入力した多項式リストの割り算の結果の商([1])と余り([2])を表す二重多項式リスト

division_on_G8 := proc(x,y);
local i,j,q,quotient,Top_divisor,divisor,dividend,flag;
local result:=[[],[]];

dividend := x;
divisor := y;
q:=0;

#割るリスト割られるリストの余分な0を上から除く
i := nops(dividend);
while dividend[nops(dividend)]=0 do
  dividend := subsop(i=NULL,dividend);
  i--;
end do;

i := nops(divisor);
while divisor[nops(divisor)]=0 do
  divisor := subsop(i=NULL,divisor);
  i--;
end do;

dividend := change_0_to_zero(dividend);
divisor := change_0_to_zero(divisor);

while nops(dividend)>=nops(divisor) and nops(dividend)<>0 do

  q:=0;
  Top_divisor := divisor[nops(divisor)];
  #割る数の最上位の値が割られる最上位の値と同じになるまで繰り返す
  while Top_divisor<>dividend[nops(dividend)] do
    Top_divisor := G8:-`*`(Top_divisor,a);
    q++;
  end do;

  quotient := G8:-`^`(a,q);
  result[1] := [quotient,op(result[1])];

  #割られるリストの最上位を消去
  dividend := subsop(nops(dividend)=NULL,dividend);

  #次に割られるリスト(余り)を生成
  j:=0;
  for i from 1 by 1 to nops(dividend) do
    j++;
    if i > (nops(dividend)+1)-nops(divisor) then
      dividend[i] := G8:-`+`(dividend[i],G8:-`*`(divisor[j],quotient));
    else
      j--;
    end if;
  end do;
  i := nops(dividend);
  flag := 0;
  while dividend[nops(dividend)]=G8:-zero do
    dividend := subsop(i=NULL,dividend);
    i--;
    #割られる数の最上位がzeroの時、商に一つだけzeroを加えるための操作
    flag++;
    if flag=1 then
      result[1] := [G8:-zero,op(result[1])];

    elif nops(dividend)=0 then
      break;
    end if;
  end do;
end do;

if nops(dividend)=0 then
  result[2] := [G8:-zero];
else
  result[2] := dividend;
end if;

result[1] := change_zero_to_0(result[1]);
result[2] := change_zero_to_0(result[2]);

return result;
end proc:
#関数名：diff_on_G8
#入力１：多項式リスト
#出力：入力した多項式リストの微分の多項式リスト

diff_on_G8 := proc(X);
local i,j,result,x,length;

x := X;

for i from 1 by 1 to nops(x) do
  if x[i]=0 then
    x[i] := G8:-zero;
  end if;
end do;

if (nops(x) mod 2) = 1 then
  length := nops(x)-2;
else
  length := nops(x)-1;
end if;

result := [];
for i from 1 by 1 to length do
  result := [op(result),G8:-zero];
end do;

for i from 1 by 2 to length do
  result[i] := x[i+1]; 
end do;

for i from 1 by 1 to nops(result) do
  if result[i]=0 then
    result[i] := G8:-zero;
  end if;
end do;

return result;
end proc:
#関数名：substitute
#入力１：リスト多項式
#入力２：多項式に代入したいガロア体上の値
#出力：リスト多項式に値を代入した際に得られる値
substitute := proc(Pol,val);
local i,result,index,pol;

pol := change_0_to_zero(Pol);
result := G8:-zero;
for i from 1 by 1 to nops(pol) do
  result := G8:-`+`(result,G8:-`*`(pol[i],(G8:-`^`(val,(i-1)))));
end do;
return result;
end proc:
#betaの多項式リストの生成
use G8 in
beta := [G8:-one];
for i from 1 by 1 to nops(erasure_location) do
  beta := multi_on_G8(beta,[G8:-one,a^erasure_location[i]]);
end do;
end use;
for i from 1 by 1 to nops(erasure_location) do
P := -erasure_location[i];
  use G8 in
    substitute(beta,a^P);
  end use;
end do;
#元の受信多項式リストの消失誤りを0にした修正後の受信多項式を生成
modified_r := Euclid_list:
for i from 1 by 1 to nops(erasure_location) do
  modified_r[erasure_location[i]+1] := 0:
end do:
modified_r;
use G8 in
substitute(modified_r,a^10);
end use;
#シンドローム多項式Sの生成
S:=[]:
for i from 1 by 1 to 2*t do
 S := [op(S),substitute(modified_r,(G8:-`^`(a,i)))];
end do;

use G8 in
substitute(modified_r,a^3);
end use;
#Tの生成
T:=[]:
T := multi_on_G8(beta,S);

#2tより多い部分を省く
extra := nops(T)-2*t;
if extra>0 then
  for i from nops(T) by -1 to nops(T)-extra+1 do
    T := subsop(i=NULL,T);
  end do;
end if;
T;

erasure_location;
for i from 1 by 1 to n do
P := -i;
use G8 in
  substitute(T,a^P);
end use;
end do:
#ユークリッド互除法を行う
Z:=[]:
Z1:=[]:
#Z1の生成
for i from 0 by 1 to 2*t do
  if i<>2*t then
    Z1:=[op(Z1),0];
  else
    Z1:=[op(Z1),G8:-one];
  end if;
end do:
Z:=[op(Z),Z1]:
Z:=[op(Z),T];
sigma:=[]:
sigma:=[op(sigma),[0]]:
sigma:=[op(sigma),[G8:-one]];
q:=[];
#終了条件conの定義
if (nops(erasure_location) mod 2)=0 then
  con := t + nops(erasure_location)/2;
else
  con := t + (nops(erasure_location)-1)/2;
end if;
division_on_G8(Z[1],Z[2])[2];
i:=1:
print(nops(Z[nops(Z)])-1);
print(con);
while nops(Z[nops(Z)])-1>=con do
  Div_result := division_on_G8(Z[i],Z[i+1]);
  print("Div_result",Div_result);
  q := [op(q),Div_result[1]];
  next_sigma := plus_on_G8(multi_on_G8(q[i],sigma[i+1]),sigma[i]);
  sigma := [op(sigma),next_sigma];
  Z := [op(Z),Div_result[2]];
  i++;
end do;
Z := Z[nops(Z)];
if q = [] then
  q := [];
else
  q := q[nops(q)];
end if;
sigma := sigma[nops(sigma)];

beta;
Ga := multi_on_G8(beta,sigma);
diff_Ga := diff_on_G8(Ga);
#復号を行う
v := Euclid_list;
for i from 1 by 1 to nops(erasure_location) do
  EL := erasure_location[i];
  dividend := substitute(Z,G8:-`^`(a,-EL));
  divisor := substitute(diff_Ga,G8:-`^`(a,-EL));
  G8:-`/`(dividend,divisor);
  v[op(erasure_location[i]+1)] := G8:-`/`(dividend,divisor);
end do;
change_zero_to_0(v);
C := v;
for i from 1 to n do
  C[i];
end do;
# 
# 検査行列 H の定義
# 
H := Matrix(n-k, n);
for i from 1 to n-k do
  for j from 1 to n do
    H[i,j] := G8:-`^`(a, (i-1)*(n-j));
  end do;
end do;
# 
# Check=C*(H^T)を計算してCheckの各要素が0になるかどうか確認する。
# 
Check := Vector[n-k];
for i from 1 to n-k do
  Check[i] := G8:-input(0);
  for j from 1 to n do
    Check[i] := G8:-`+`(Check[i], G8:-`*`(C[j], H[i,j]));
  end do;
  print(Check[i]);
end do:
with(ListTools);
#関数名 := proc(引数)
;
mybin := proc(a)
  local i, t, r;
  t := a;
  r := [-1, -1, -1, -1, -1, -1, -1, -1];
  for i from 1 to 8 do
    r[i] := t mod 2;
    t := iquo(t, 2);
  end do;
  r := Reverse(r):
  return r;
end proc:
with(ColorTools):

M := 21:
QRcodeVersion := 1;
maskPattern := "001";
Vec := Vector(M*M):

setVec := proc(x, y, exp) global Vec, M; if evalb(exp) then Vec[M*y + x + 1] := "Black"; else Vec[M*y + x + 1] := "White"; end if; end proc;
`fillは`*`x座標y座標中心に半径分だけ白黒`(`右上とかにある四角`);
fill := proc(centerX, centerY, halfWidth, exp) local x, y, size; global QRcodeVersion; size := QRcodeSize(QRcodeVersion); for y from max(centerY - halfWidth, 0) to min(centerY + halfWidth, size - 1) do for x from max(centerX - halfWidth, 0) to min(centerX + halfWidth, size - 1) do setVec(x, y, exp); end do; end do; end proc;
`上で計算した情報をもとに実際に四角を作る`;
PlaceFinderPattern := proc () 
  local size; 
  global QRcodeVersion; 
  size := QRcodeSize(QRcodeVersion); 
 
  #左上
  fill(3, 3, 4, false):
  fill(3, 3, 3, true):
  fill(3, 3, 2, false):
  fill(3, 3, 1, true):

  #右上
  fill(size - 1 - 3, 3, 4, false):
  fill(size - 1 - 3, 3, 3, true):
  fill(size - 1 - 3, 3, 2, false):
  fill(size - 1 - 3, 3, 1, true):

  #左下
  fill(3, size - 1 - 3, 4, false):
  fill(3, size - 1 - 3, 3, true):
  fill(3, size - 1 - 3, 2, false):
  fill(3, size - 1 - 3, 1, true):

end proc:
`タイミングパターンを作成`;
PlaceTimingPattern := proc() local i, size; global Vec, QRcodeVersion; size := QRcodeSize(QRcodeVersion); for i from 8 to size - 9 do setVec(i, 6, i mod 2 = 0); end do; for i from 8 to size - 9 do setVec(6, i, i mod 2 = 0); end do; end proc;
`いつも黒い箇所を決めている`;
PlaceAlwaysBlack := proc() local size; global Vec, QRcodeVersion; size := QRcodeSize(QRcodeVersion); Vec[size*(size - 8) + 9] := "Black"; end proc;
`上の情報らを機能パターンと呼ぶ`;
PlaceFunctionPattern := proc() PlaceTimingPattern(); PlaceFinderPattern(); PlaceAlwaysBlack(); end proc;
NULL;
QRcodeSize := proc(QRcodeVersion) return 4*QRcodeVersion + 17; end proc;
`形式情報の計算を行う`;
PlaceFormatInfo := proc() local Gxx, formatInfox, errCorCodex, result, maskPattern, deg, pos_x, pos_y, size; global Vec, M; Gxx := x^10 + x^8 + x^5 + x^4 + x^2 + x + 1; formatInfox := 1; errCorCodex := formatInfox*x^10; errCorCodex := rem(errCorCodex, Gxx, x) mod 2; result := formatInfox*x^10 + errCorCodex; maskPattern := x^14 + x^12 + x^10 + x^4 + x; result := (result + maskPattern) mod 2; deg := 14; for pos_x from 0 to 5 do setVec(pos_x, 8, coeff(result, x, deg) = 1); deg := deg - 1; end do; setVec(7, 8, coeff(result, x, deg) = 1); deg := deg - 1; setVec(8, 8, coeff(result, x, deg) = 1); deg := deg - 1; setVec(8, 7, coeff(result, x, deg) = 1); deg := deg - 1; for pos_y from 5 by -1 to 0 do setVec(8, pos_y, coeff(result, x, deg) = 1); deg := deg - 1; end do; deg := 14; size := M; for pos_y from size - 1 by -1 to size - 7 do setVec(8, pos_y, coeff(result, x, deg) = 1); deg := deg - 1; end do; for pos_x from size - 8 to size - 1 do setVec(pos_x, 8, coeff(result, x, deg) = 1); deg := deg - 1; end do; print(result); end proc;
isFinderPattern := proc(version, x, y) local size; size := QRcodeSize(version); return x <= 7 and y <= 7 or x <= 7 and size - 8 <= y or size - 8 <= x and y <= 7; end proc;
isTimingPattern := proc(version, x, y) return not isFinderPattern(version, x, y) and (x = 6 or y = 6); end proc;
isFunctionPattern := proc(version, x, y) return isFinderPattern(version, x, y) or isTimingPattern(version, x, y); end proc;

dataWritable := proc(version, x, y) local size; size := QRcodeSize(version); if isFunctionPattern(version, x, y) then return false; end if; if x <= 8 and y <= 8 then return false; end if; if x = 8 and size - 8 <= y then return false; end if; if y = 8 and size - 8 <= x then return false; end if; if 7 <= version then if x < 6 and size - 11 <= y and y < size - 8 then return false; end if; if y < 6 and size - 11 <= x and x < size - 8 then return false; end if; end if; if x = 8 and y = 4*version + 9 then return false; end if; return true; end proc;
PosProceed := proc(version, xref::uneval, yref::uneval) local size, x, y, relx, rely; x := eval(xref); y := eval(yref); size := QRcodeSize(version); ASSERT(x <> 6); relx := x; rely := y; if 6 < x then relx := relx - 1; end if; if relx mod 2 = 0 then if iquo(relx, 2) mod 2 = 0 then if rely = size - 1 then if relx = 0 then x := size - 1; else x := x - 1; end if; else x := x + 1; y := y + 1; end if; else if rely = 0 then if x = 7 then x := x - 2; else x := x - 1; end if; else x := x + 1; y := y - 1; end if; end if; else x := x - 1; end if; xref := x; yref := y; end proc;
PosNext := proc(version, x_::uneval, y_::uneval) local size, x, y; x := eval(x_); y := eval(y_); size := QRcodeSize(version); do break; if x = 0 and y = size - 1; PosProceed(version, x, y); break; if dataWritable(version, x, y); end do; x_ := x; y_ := y; end proc;
flip := proc(x, y) local size; global Vec, QRcodeVersion; size := QRcodeSize(QRcodeVersion); if Vec[x + 1 + size*y] = "White" then Vec[x + 1 + size*y] := "Black"; elif Vec[x + 1 + size*y] = "Black" then Vec[x + 1 + size*y] := "White"; end if; end proc;

myMask := proc() local condition, size, x, y; global QRcodeVersion, maskPattern; size := QRcodeSize(QRcodeVersion); for y from 0 to size - 1 do for x from 0 to size - 1 do condition := false; if maskPattern = "000" then condition := evalb((x + y) mod 2 = 0); elif maskPattern = "001" then condition := evalb(y mod 2 = 0); elif maskPattern = "010" then condition := evalb(x mod 3 = 0); elif maskPattern = "011" then condition := evalb((x + y) mod 3 = 0); elif maskPattern = "100" then condition := evalb((iquo(x, 3) + iquo(y, 2)) mod 2 = 0); elif maskPattern = "101" then condition := evalb(((x*y mod 3) + x*y) mod 2 = 0); elif maskPattern = "110" then condition := evalb((((x*y mod 3) + x*y) mod 2) mod 2 = 0); elif maskPattern = "111" then condition := evalb((((x*y mod 3) + x + y) mod 2) mod 2 = 0); else print("マスクパターン値が異常です"); condition := false; end if; if condition then flip(x, y); end if; end do; end do; end proc;
SetModule := proc(x, y, isBlack) global Vec; if isBlack then Vec[x + M*y] := "Black"; else Vec[x + M*y] := "White"; end if; end proc;
`RS符号の配置`;
PlaceCode := proc(codePairs) local x, y, index, i, size; global QRcodeVersion; size := QRcodeSize(QRcodeVersion); x := size - 1; y := size - 1; for index to numelems(codePairs) do for i to 8 do setVec(x, y, codePairs[index][i] = 1); PosNext(QRcodeVersion, x, y); end do; end do; end proc;
c := [];
for i to n do
    c := [op(c), mybin(G8:-output(C[i]))];
end do;
TestC := [64, 100, 101, 84, 181, 84, 68, 16, 236, 17, 236, 17, 236, 17, 236, 17, 33, 96, 131, 82, 135, 238, 115, 111, 156, 217];
for i to n do
    ;
end do;
for i to M*M do
    Vec[i] := "Pink";
end do;
PlaceCode(c);
myMask():
PlaceFormatInfo():
PlaceFunctionPattern():
Swatches(
  convert(Vec, list),
  gap=0,
  borders=["White", "White"]
);


















