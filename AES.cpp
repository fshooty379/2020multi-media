
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define Nr128 10
#define Nr192 12
#define Nr256 14
#define Nk 4

typedef unsigned char byte;

byte state[4][4] = { 0x00 };      //state数组
byte w[44][4] = { 0x00 };		   //扩展密钥数组
byte S_box[16][16] = { //S-盒16进制数据
	/*        0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
	/*0*/  {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
	/*1*/  {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
	/*2*/  {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
	/*3*/  {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
	/*4*/  {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
	/*5*/  {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
	/*6*/  {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
	/*7*/  {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
	/*8*/  {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
	/*9*/  {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
	/*a*/  {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
	/*b*/  {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
	/*c*/  {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
	/*d*/  {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
	/*e*/  {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
	/*f*/  {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
};
byte InvS_box[16][16] = {  //逆S-盒16进制数据
	/*        0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
	/*0*/  {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
	/*1*/  {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
	/*2*/  {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
	/*3*/  {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
	/*4*/  {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
	/*5*/  {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
	/*6*/  {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
	/*7*/  {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
	/*8*/  {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
	/*9*/  {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
	/*a*/  {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
	/*b*/  {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
	/*c*/  {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
	/*d*/  {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
	/*e*/  {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
	/*f*/  {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}
};

void AddRoundKey(int round)  //按行异或 byte w[][4], 
{
	int i, j;
	for (i = 0; i < 4; i++, round++)
		for (j = 0; j < 4; j++)
			state[i][j] ^= w[round][j];
}

void Initstate(byte *text) //初始化—填充State  *text --*Plain
{
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = text[i + 4 * j];   //以列为顺序进行初始化
}

void StateToCipher(byte *Cipher) //将state转化为密文
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Cipher[4 * i + j] = state[i][j];  //以行为顺序输出
}

void RotWord(byte *b)  //对输入的四个字节进行循环左移
{
	byte temp;
	temp = b[0];
	b[0] = b[1];
	b[1] = b[2];
	b[2] = b[3];
	b[3] = temp;
}

void SubBytes()   //字节替换
{
	int i, j, x, y; //x表示S_box的列，y表示S_box的行
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			x = state[i][j] >> 4;
			y = state[i][j] & 0x0f;
			state[i][j] = S_box[x][y];
		}
	}
}

void ShiftRows()  //行移位变换
{
	byte temp1, temp2, temp3; //temp1左移一位时临时变量，
							  //temp1、temp2左移两位时的临时变量
							  //temp1、temp2、temp3左移三位时的临时变量
	int i;
	for (i = 1; i < 4; i++) {
		switch (i) {
		case 1:   //左移一位
			temp1 = state[i][0];
			state[i][0] = state[i][1];
			state[i][1] = state[i][2];
			state[i][2] = state[i][3];
			state[i][3] = temp1;
			break;
		case 2:  //左移两位
			temp1 = state[i][0];
			temp2 = state[i][1];
			state[i][0] = state[i][2];
			state[i][1] = state[i][3];
			state[i][2] = temp1;
			state[i][3] = temp2;
			break;
		case 3:  //左移三位
			temp1 = state[i][0];
			temp2 = state[i][1];
			temp3 = state[i][2];
			state[i][0] = state[i][3];
			state[i][1] = temp1;
			state[i][2] = temp2;
			state[i][3] = temp3;
			break;
		default:
			break;
		}
	}
}

byte GFMulti(byte a, byte b)   //有限域乘法
{
	byte Sign = 0x01;         //Sign分解被乘数的变量
	byte Constant = 0x1B;     //当最高位为1时要比特异或constant
	byte mul = 0x00;          //乘积
	byte temp;                //临时变量
	int i, andd, top, j;       //and值为b&Sign的结果, top为最高位的值
	if (b&Sign)                //当最低位为1时不需要移位操作
		mul = a;
	for (i = 1; i < 8; i++) {
		Sign = Sign << 1;
		andd = b & Sign;
		if (andd != 0) {
			temp = a;
			for (j = 1; j <= i; j++) {         //当and不为0时需要一共需要左移i位
				top = temp >> 7;
				temp = temp << 1;             //左移一位 
				if (top)
					temp = temp ^ Constant;   //最高位为1异或{1B}即Constant
			}
			mul ^= temp;                //计算结果
		}
	}
	return mul;
}

void MixColumns()  //列混合变换
{
	byte MixMatrix[4][4] = { {0x02, 0x03, 0x01, 0x01},  //列混合矩阵
							{0x01, 0x02, 0x03, 0x01},
							{0x01, 0x01, 0x02, 0x03},
							{0x03, 0x01, 0x01, 0x02} };
	int i, j;                                          //i表示行，j表示列
	//把state的值赋给tempstate进行列混合运算
	byte tempstate[4][4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			tempstate[i][j] = state[i][j];
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++) {
			state[i][j] = GFMulti(MixMatrix[i][0], tempstate[0][j]) ^ GFMulti(MixMatrix[i][1], tempstate[1][j]) ^
				GFMulti(MixMatrix[i][2], tempstate[2][j]) ^ GFMulti(MixMatrix[i][3], tempstate[3][j]);
		}
}

void SubWord(byte *B)  //密钥扩展中的字替换
{
	int i, x, y; //x表示S_box的列，y表示S_box的行
	for (i = 0; i < 4; i++) {
		x = B[i] >> 4;
		y = B[i] & 0x0f;
		B[i] = S_box[x][y];
	}
}

byte *Rcon(int i)  //Rcon函数
{
	//Rc[]
	byte Rc[12] = { 0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36, 0x6C };
	byte* temp = (byte*)malloc(sizeof(byte) * 4);
	temp[0] = Rc[i]; temp[1] = 0x00; temp[2] = 0x00; temp[3] = 0x00;
	return temp;
}

void KeyExpasion(byte *key) //密钥扩展
{
	byte *temp = (byte*)malloc(sizeof(byte) * 4);//temp[4]; 
	byte* rcon = (byte*)malloc(sizeof(byte) * 4);//rcon[4]为Rcon()之后的返回值
	int t, i;

	//扩展密钥的前Nk个字的初始密钥即前256bit
	for (i = 0; i < Nk; i++) {    //Nk为密钥长度以字为单位
		t = 4 * i;
		w[i][0] = key[t];
		w[i][1] = key[t + 1];
		w[i][2] = key[t + 2];
		w[i][3] = key[t + 3];
	}

	for (i = Nk; i < 4 * (Nr128 + 1); i++) {
		temp[0] = w[i - 1][0]; temp[1] = w[i - 1][1];    //temp = word[i-1] 
		temp[2] = w[i - 1][2]; temp[3] = w[i - 1][3];
		if (i % Nk == 0) {
			RotWord(temp);          //循环左移
			SubWord(temp);          //字替换
			rcon = Rcon(i / Nk);   //Rcon[i]替换
			//异或
			temp[0] ^= rcon[0]; temp[1] ^= rcon[1];
			temp[2] ^= rcon[2]; temp[3] ^= rcon[3];
		}
		else if (Nk == 8 && (i % Nk == 4))
			SubWord(temp);          //字替换
		//异或
		w[i][0] = w[i - Nk][0] ^ temp[0]; w[i][1] = w[i - Nk][1] ^ temp[1];
		w[i][2] = w[i - Nk][2] ^ temp[2]; w[i][3] = w[i - Nk][3] ^ temp[3];
	}
}

void AESEncrypt(char *PlainPath, char *CipherPath)
{
	byte  plainbuf[16], cipherbuf[16];
	char temp;
	FILE *fplain;
	FILE *fcipher;
	long f_len;
	int l, k;

	fplain = fopen(PlainPath, "rb");    //以二进制方式读取文件
	fcipher = fopen(CipherPath, "wb");  //以二进制方式想文件中写入数据
	fseek(fplain, 0L, SEEK_END);		//将文件当作数组处理
	f_len = ftell(fplain);              //返回文件的当前位置
	rewind(fplain);                     //指向文件头
	//printf("flen = %d\n", f_len);
	for (l = 0; l < f_len / 16; l++) {
		for (k = 0; k < 16; k++) {
			temp = fgetc(fplain);
			plainbuf[k] = temp;
		}
		Initstate(plainbuf);       //初始化state
		int round = 0;
		AddRoundKey(round);

		//前Nr-1轮加密
		for (round = 1; round < Nr128; round++) {
			SubBytes();
			ShiftRows();
			MixColumns();
			AddRoundKey(4 * round);   //修改round为4*round
		}

		//最后一轮
		SubBytes();
		ShiftRows();
		AddRoundKey(4 * round);

		//将state转化为Cipher
		StateToCipher(cipherbuf);
		for (k = 0; k < 16; k++)
			fputc(cipherbuf[k], fcipher);
	}
	k = 0;
	while ((temp = fgetc(fplain)) != EOF)
		plainbuf[k++] = temp;
	while (k < 16) {
		plainbuf[k] = 0xff;
		k++;
	}
	Initstate(plainbuf);       //初始化state
	int round = 0;
	AddRoundKey(round);

	//前Nr-1轮加密
	for (round = 1; round < Nr128; round++) {
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(4 * round);   //修改round为4*round
	}

	//最后一轮
	SubBytes();
	ShiftRows();
	AddRoundKey(4 * round);

	//将state转化为Cipher
	StateToCipher(cipherbuf);
	for (k = 0; k < 16; k++)
		fputc(cipherbuf[k], fcipher);
	fclose(fcipher);
	fclose(fplain);
}

void InvShiftRows()   //逆行变换
{
	byte temp1, temp2, temp3; //temp1右移一位时临时变量，
							  //temp1、temp2右移两位时的临时变量
							  //temp1、temp2、temp3右移三位时的临时变量
	int i;
	for (i = 1; i < 4; i++) {
		switch (i) {
		case 1:   //右移一位
			temp1 = state[i][3];
			state[i][3] = state[i][2];
			state[i][2] = state[i][1];
			state[i][1] = state[i][0];
			state[i][0] = temp1;
			break;
		case 2:  //右移两位
			temp1 = state[i][0];
			temp2 = state[i][1];
			state[i][0] = state[i][2];
			state[i][1] = state[i][3];
			state[i][2] = temp1;
			state[i][3] = temp2;
			break;
		case 3:  //右移三位
			temp1 = state[i][3];
			temp2 = state[i][2];
			temp3 = state[i][1];
			state[i][3] = state[i][0];
			state[i][2] = temp1;
			state[i][1] = temp2;
			state[i][0] = temp3;
			break;
		default:
			break;
		}
	}
}

void InvSubBytes()
{
	int i, j, x, y; //x表示InvS_box的列，y表示InvS_box的行
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			x = state[i][j] >> 4;
			y = state[i][j] & 0x0f;
			state[i][j] = InvS_box[x][y];
		}
	}
}

void InvMixColumns()
{
	byte InvMixMatrix[4][4] = { {0x0e, 0x0b, 0x0d, 0x09},  //逆列混合矩阵
								{0x09, 0x0e, 0x0b, 0x0d},
								{0x0d, 0x09, 0x0e, 0x0b},
								{0x0b, 0x0d, 0x09, 0x0e} };
	int i, j;                                          //i表示行，j表示列
	//把state的值赋给tempstate
	byte tempstate[4][4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			tempstate[i][j] = state[i][j];
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++) {
			state[i][j] = GFMulti(InvMixMatrix[i][0], tempstate[0][j]) ^ GFMulti(InvMixMatrix[i][1], tempstate[1][j]) ^
				GFMulti(InvMixMatrix[i][2], tempstate[2][j]) ^ GFMulti(InvMixMatrix[i][3], tempstate[3][j]);
		}
}

void StateToPlain(byte *Plain)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			Plain[i + 4 * j] = state[i][j];  //以列为顺序输出
}

void InvInitstate(byte *text)
{
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = text[4 * i + j];   //以行为顺序进行初始化
}

void AESDecrypt(char *CipherPath, char *DecryptPath)  //AES解密
{
	byte  decryptbuf[16], cipherbuf[16];
	char temp;
	FILE *fcipher;
	FILE *fdecrypt;
	long f_len;
	int l, k;
	fcipher = fopen(CipherPath, "rb");    //以二进制方式读取文件
	fdecrypt = fopen(DecryptPath, "wb");  //以二进制方式想文件中写入数据
	fseek(fcipher, 0L, SEEK_END);		//将文件当作数组处理
	f_len = ftell(fcipher);              //返回文件的当前位置
	rewind(fcipher);                     //指向文件头
	//printf("flen = %d\n", f_len);
	for (l = 0; l < f_len / 16; l++) {
		for (k = 0; k < 16; k++) {
			temp = fgetc(fcipher);
			cipherbuf[k] = temp;
		}
		InvInitstate(cipherbuf);  //初始化state
		AddRoundKey(4 * Nr128);
		//前Nr-1轮解密
		for (int round = Nr128 - 1; round > 0; round--) {
			InvShiftRows();
			InvSubBytes();
			AddRoundKey(4 * round);
			InvMixColumns();
		}

		//最后一轮解密
		int round = 0;
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(round);

		//将state转化为Plain
		StateToPlain(decryptbuf);
		for (k = 0; k < 16; k++)
			fputc(decryptbuf[k], fdecrypt);
	}
	fclose(fcipher);
	fclose(fdecrypt);
}

int main()
{
	byte key[16] = { 0xab, 0xcd, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78, 0x90, 0x12, 0x34, 0x56, 0x78 };
	
	char src_file[] = "D:\\1012\\demo.rar";
	char *PlainPath = src_file;
	char en_file[] = "D:\\1012\\en_demo.rar";
	char *CipherPath = en_file;
	char de_file[] = "D:\\1012\\de_demo.rar";
	char *DecryptPath = de_file;
	int begin_time, end_time; 
	int i;
	/*---------------AES加密----------------*/
	printf("AES加密:\n");
	printf("密钥：");
	for (i = 0; i < 16; i++)
		printf("%02x", key[i]);
	printf("\n");
	KeyExpasion(key);    //密钥扩展
	
	begin_time = clock();
	AESEncrypt(PlainPath, CipherPath);
	end_time = clock();
	printf("加密时间=%dms\n", end_time - begin_time);
	printf("文件大小=12238KB\n");
	printf("加密速度=%.2fKB/s\n", (12238.0*1000/(end_time - begin_time)));
	
	/*---------------AES解密----------------*/
	printf("AES解密:\n");
	printf("密钥：");
	for (i = 0; i < 16; i++)
		printf("%02x", key[i]);
	printf("\n");
	
	begin_time = clock();
	AESDecrypt(CipherPath, DecryptPath);
	end_time = clock();
	printf("解密时间=%dms\n", end_time - begin_time);
	printf("文件大小=12238KB\n");
	printf("解密速度=%.2fKB/s\n", (12238.0*1000/(end_time - begin_time)));
	return 0;
}
