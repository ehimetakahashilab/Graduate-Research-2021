
with(ImageTools):
QRcodeSize := proc(QRcodeVersion)
 return QRcodeVersion*4+17:
end proc:
#関数：cal_avarage_luminance
#入力１：平均輝度値を取得したいグレースケール画像(正方形)
#出力：入力１の平均輝度値

cal_avarage_luminance := proc(gray_image);
local i,j,L,s,g,avarage_luminance;

L := Height(gray_image);
s := round(L/4);
g := round(L*3/4);

avarage_luminance := 0;
for i from s by 1 to g do
  for j from s by 1 to g do
    avarage_luminance := avarage_luminance + gray_image[j][i];
  end do;
end do;
avarage_luminance := avarage_luminance/((g - s)^2);

return avarage_luminance;
end proc:
#関数：gen_resized_image
#入力１：元画像(正方形)
#入力２：サイズ変更後の画像の縦横の長さ
#出力：サイズを変更した画像

gen_resized_image := proc(image,L);
local resized_L,resized_image;

resized_L := L/Width(image);
resized_image := Scale(image,resized_L); 

return resized_image;
end proc:
#関数：gen_resized_binary_image
#入力：画像のパス
#出力：元画像を21*21に縮小し、グレイスケールにしてから二値化した画像

gen_resized_binary_image := proc(image_path);
local i,j,image,resized_gray_image,avarage_luminance,resized_binary_image;

image := Read(image_path);
resized_gray_image := RGBtoGray(gen_resized_image(image,21));
avarage_luminance := cal_avarage_luminance(resized_gray_image);

resized_binary_image := Create(21,21,1);
for i from 1 by 1 to 21 do
  for j from 1 by 1 to 21 do
    if (resized_gray_image[i,j] >= avarage_luminance) then
      resized_binary_image[i,j] := 1;
    end if;
  end do;
end do;

return resized_binary_image;
end proc:
with(StringTools):
Use_ToByteArray := proc(str):
return ToByteArray(str):
end proc:
with(ListTools):
#関数名：binarize
#入力1：10進数
#出力：入力を2進数にし、8ビット化したベクトル

binarize := proc(dec):
local bin, i;
bin := Reverse(convert(dec,base,2)):
bin := convert(bin,Vector):
for i from 1 by 1 to 8-numelems(bin) do
  bin := <0,bin>;
end do:
return bin:
end proc:
Use_LengthSplit := proc(list,n):
return LengthSplit(list,n):
end proc:
Use_Reverse := proc(list):
return Reverse(list):
end proc:
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



#関数名：get_exp
#入力1：2の8乗のガロア体上の多項式表現
#出力：多項式表現の元となるアルファの冪

get_exp := proc(dec):
local exp, fin, pol,G8,a;
G8 := GF(2, 8, alpha^8+alpha^4+alpha^3+alpha^2+1):
a := G8:-ConvertIn(alpha):
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

#関数名：gen_AestheticQRcode
#入力１：Vec(生成したQRコードが格納されたベクトル)
#入力２：image_path(元画像のパス)
#出力：AestheticQRcodeが格納された二次元リスト

gen_AestheticQRcode := proc(QRcode_vec,image_path);
local i, j,image,resize_height,resize_width,resize_image,AestheticQRcode;
local resize_gray_image,avarage_luminance,epsilon;
  
image := Read(image_path);
resize_height := 21/Height(image);
resize_width := 21/Width(image);
resize_image := Scale(image,resize_height,resize_width); 
resize_gray_image := RGBtoGray(resize_image);

avarage_luminance := 0;
for i from round(21/4) by 1 to round(21*3/4) do
  for j from round(21/4) by 1 to round(21*3/4) do
    avarage_luminance := avarage_luminance + resize_gray_image[i][j];
  end do;
end do;
avarage_luminance := avarage_luminance/((round(21*3/4) - round(21/4))^2);

AestheticQRcode := RGBtoYUV(resize_image);
epsilon := 0.25;

for i from 0 by 1 to 21-1 do
  for j from 1 by 1 to 21 do
    if QRcode_vec[i*21+j] = "Black" then
      if AestheticQRcode[i+1,j,1] >= avarage_luminance - epsilon then
        AestheticQRcode[i+1,j,1] := avarage_luminance - epsilon;
      end if;
    elif QRcode_vec[i*21+j] = "White" then
      if AestheticQRcode[i+1,j,1] <= avarage_luminance + epsilon then
        AestheticQRcode[i+1,j,1] := avarage_luminance + epsilon;
      end if;
    else
      #print("エラー！");
    end if;
  end do;
end do;

AestheticQRcode := YUVtoRGB(AestheticQRcode);
return AestheticQRcode;
end proc:


