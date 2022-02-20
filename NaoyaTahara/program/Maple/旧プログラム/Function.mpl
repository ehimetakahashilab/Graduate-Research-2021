
#�֐��Fget_resize_binary_image
#���́F�摜�̃p�X
#�o�́F���摜��21*21�ɏk�����A�O���C�X�P�[���ɂ��Ă���}�X�N��������l�������摜

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




#�֐����Fbinarize
#����1�F10�i��
#�o�́F���͂�2�i���ɂ��A8�r�b�g���������X�g

binarize := proc(dec):
local bin, i;
bin := Reverse(convert(dec,base,2)):
for i from 1 by 1 to 8-nops(bin) do
  bin := [0,op(bin)];
end do:
end proc:
#�֐����Fget_exp
#����1�F2��8��̃K���A�̏�̑������\��
#�o�́F�������\���̌��ƂȂ�A���t�@�̙p

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
#�֐����Fchange_0_to_zero
#���͂P�F���������X�g
#�o�́F���͂������������X�g����0��G4���zero�ɕϊ�
change_0_to_zero := proc(x);
local i,X;
X := x;

for i from 1 by 1 to nops(x) do
  if x[i]=0 then
    X[i] := G4:-zero;
  end if;
end do;

return X;
end proc:
#�֐����Fchange_zero_to_0
#���͂P�F���������X�g
#�o�́F���͂������������X�g����G4���zero��0�ɕϊ�
change_zero_to_0 := proc(x);
local i,X;
X := x;

for i from 1 by 1 to nops(x) do
  if x[i] = G4:-zero then
    X[i] := 0;
  end if;
end do;

return X;
end proc:
#�֐����Fplus_on_G4
#���͂P,�Q�F���������X�g
#�o�́F���͂������������X�g���m�̘a�̑��������X�g

plus_on_G4 := proc(x,y);
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
  long_vec[i] := G4:-`+`(long_vec[i],short_vec[i]);
end do;

long_vec := change_zero_to_0(long_vec);

return long_vec;
end proc:
#�֐����Fmulti_on_G4
#���͂P,�Q�F���������X�g
#�o�́F���͂������������X�g���m�̐ς̑��������X�g

multi_on_G4 := proc(X,Y);
local i,j,result,x,y;

x := X;
y := Y;

for i from 1 by 1 to nops(x) do
  if x[i]=0 then
    x[i] := G4:-zero;
  end if;
end do;

for i from 1 by 1 to nops(y) do
  if y[i]=0 then
    y[i] := G4:-zero;
  end if;
end do;

result := [];
for i from 1 by 1 to nops(x)+nops(y)-1 do
  result := [op(result),G4:-zero];
end do;

for i from 1 by 1 to nops(x) do
  for j from 1 by 1 to nops(y) do
    result[i+j-1] := G4:-`+`(result[i+j-1], G4:-`*`(x[i],y[j]));
  end do;
end do;

for i from 1 by 1 to nops(result) do
  if result[i]=0 then
    result[i] := G4:-zero;
  end if;
end do;

return result;
end proc:
#�֐����Fdivision_on_G4
#���͂P�F�����鑽�������X�g
#���͂Q�F���鑽�������X�g
#�o�́F���͂������������X�g�̊���Z�̌��ʂ̏�([1])�Ɨ]��([2])��\����d���������X�g

division_on_G4 := proc(x,y);
local i,j,q,quotient,Top_divisor,divisor,dividend,flag;
local result:=[[],[]];

dividend := x;
divisor := y;
q:=0;

#���郊�X�g�����郊�X�g�̗]����0���ォ�珜��
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
#print("dividend",dividend);
#print("divisor",divisor);

  q:=0;
  Top_divisor := divisor[nops(divisor)];
  #print("Top_divisor",divisor[nops(divisor)]);
  #print("Top_dividend",dividend[nops(dividend)]);
  #���鐔�̍ŏ�ʂ̒l��������ŏ�ʂ̒l�Ɠ����ɂȂ�܂ŌJ��Ԃ�
  while Top_divisor<>dividend[nops(dividend)] do
    Top_divisor := G4:-`*`(Top_divisor,a);
    q++;
  end do;

  quotient := G4:-`^`(a,q);
  result[1] := [quotient,op(result[1])];

  #print("befor",dividend);
  #�����郊�X�g�̍ŏ�ʂ�����
  dividend := subsop(nops(dividend)=NULL,dividend);
  #print("after",dividend);

  #���Ɋ����郊�X�g(�]��)�𐶐�
  j:=0;
  for i from 1 by 1 to nops(dividend) do
    j++;
    if i > (nops(dividend)+1)-nops(divisor) then
      dividend[i] := G4:-`+`(dividend[i],G4:-`*`(divisor[j],quotient));
    else
      j--;
    end if;
  end do;
  i := nops(dividend);
  flag := 0;
  while dividend[nops(dividend)]=G4:-zero do
    dividend := subsop(i=NULL,dividend);
    i--;
    #�����鐔�̍ŏ�ʂ�zero�̎��A���Ɉ����zero�������邽�߂̑���
    flag++;
    if flag=1 then
      result[1] := [G4:-zero,op(result[1])];

    elif nops(dividend)=0 then
      break;
    end if;
  end do;
end do;

if nops(dividend)=0 then
  result[2] := [G4:-zero];
else
  result[2] := dividend;
end if;

result[1] := change_zero_to_0(result[1]);
result[2] := change_zero_to_0(result[2]);

return result;
end proc:
#�֐����Fdiff_on_G4
#���͂P�F���������X�g
#�o�́F���͂������������X�g�̔����̑��������X�g

diff_on_G4 := proc(X);
local i,j,result,x,length;

x := X;

for i from 1 by 1 to nops(x) do
  if x[i]=0 then
    x[i] := G4:-zero;
  end if;
end do;

if (nops(x) mod 2) = 1 then
  length := nops(x)-2;
else
  length := nops(x)-1;
end if;

result := [];
for i from 1 by 1 to length do
  result := [op(result),G4:-zero];
end do;

for i from 1 by 2 to length do
  result[i] := x[i+1]; 
end do;

for i from 1 by 1 to nops(result) do
  if result[i]=0 then
    result[i] := G4:-zero;
  end if;
end do;

return result;
end proc:
#�֐����Fsubstitute
#���͂P�F���X�g������
#���͂Q�F�������ɑ���������K���A�̏�̒l
#�o�́F���X�g�������ɒl���������ۂɓ�����l
substitute := proc(Pol,val);
local i,result,index,pol;

pol := change_0_to_zero(Pol);
result := G4:-zero;
for i from 1 by 1 to nops(pol) do
  result := G4:-`+`(result,G4:-`*`(pol[i],(G4:-`^`(val,(i-1)))));
end do;
return result;
end proc:















