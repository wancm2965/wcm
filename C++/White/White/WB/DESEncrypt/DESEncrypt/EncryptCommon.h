#pragma once
#include <Windows.h>
#define KEYLENGTH 8

#define ASC2KEY 1
#define HEXKEY 2

typedef enum _SYSTEMTIMECompareEnum
{
	_TIME_BIG = 0,
	_TIME_EQUAL,
	_TIME_LESS,
}SYSTEMTIMECompareEnum;
//比较BeginTime和EndTime两个日期时间，如果EndTime比BeginTime后则返回值为_TIME_BIG，同一天为_TIME_EQUAL，
//如果EndTime比BeginTime前则返回值为_TIME_LESS
SYSTEMTIMECompareEnum CompareBeginEndTime(SYSTEMTIME BeginTime,SYSTEMTIME EndTime)
{
	if (EndTime.wYear > BeginTime.wYear)//EndTime年份大于BeginTime年份
	{
		return _TIME_BIG;
	} 
	else if(EndTime.wYear == BeginTime.wYear)//EndTime年份等于BeginTime年份
	{
		if (EndTime.wMonth > BeginTime.wMonth)//EndTime月份大于BeginTime月份
		{
			return _TIME_BIG;
		} 
		else if(EndTime.wMonth = BeginTime.wMonth)//EndTime月份等于BeginTime月份
		{
			if (EndTime.wDay > BeginTime.wDay)
			{
				return _TIME_BIG;
			} 
			else if(EndTime.wDay == BeginTime.wDay)
			{
				return _TIME_EQUAL;
			}
			else
			{
				return _TIME_LESS;
			}
		}
		else//EndTime月份小于BeginTime月份
		{
			return _TIME_LESS;
		}
	}
	else//EndTime年份小于于BeginTime年份
	{
		return _TIME_LESS;
	}

}

char* checkValidKey(CString key,int flag,int keyLength,int* pValidFlag)
{
	int i,j;
	char* data;
	char* finalData=new char[65];
	int weakKeyFlag=1;

	char* dataOri = (char*)(LPCTSTR)key;
	data=new char[keyLength*2];
	for (i=0;i<keyLength*2;++i)
	{
		data[i]=dataOri[i];
	}

	//检查密钥长度等是否合理
	if (flag==ASC2KEY)
	{
		keyLength*=2;

		for (i=0;i<keyLength;++i)
		{
			if (data[i]==0)
			{
				for (j=i+1;j<keyLength;++j)
				{
					data[j-1]=data[j];
				}
				keyLength--;
			}
		}

		if (keyLength>KEYLENGTH)
		{
			*pValidFlag=-1;
			delete data;
			delete finalData;
			return NULL;
		}
		if (keyLength<KEYLENGTH)
		{
			*pValidFlag=-2;
			delete data;
			delete finalData;
			return NULL;
		}

		data[KEYLENGTH]=0;
	}
	else
	{
		for (i=0;i<keyLength;++i)
		{
			if (!((key[i]>='0'&&key[i]<='9')||(key[i]>='a'&&key[i]<='f')||(key[i]>='A'&&key[i]<='F')))
			{
				*pValidFlag=0;
				delete data;
				delete finalData;
				return NULL;
			}
		}

		keyLength*=2;

		for (i=0;i<keyLength;++i)
		{
			if (!((data[i]>='0'&&data[i]<='9')||(data[i]>='a'&&data[i]<='f')||(data[i]>='A'&&data[i]<='F')))
			{
				for (j=i+1;j<keyLength;++j)
				{
					data[j-1]=data[j];
				}
				keyLength--;
			}
		}

		if (keyLength>(KEYLENGTH*2))
		{
			*pValidFlag=-1;
			delete data;
			delete finalData;
			return NULL;
		}
		if (keyLength<(KEYLENGTH*2))
		{
			*pValidFlag=-2;
			delete data;
			delete finalData;
			return NULL;
		}

		data[KEYLENGTH*2]=0;
	}

	//将密钥变为二进制字符串
	if (flag==ASC2KEY)
	{
		for (i=0;i<KEYLENGTH;++i)
		{
			for (j=0;j<KEYLENGTH;++j)
			{
				finalData[i*8+(KEYLENGTH-1-j)]=(((data[i]&(1<<j)))>>j)+'0';
			}
		}
	}
	else
	{
		for (i=0;i<KEYLENGTH*2;++i)
		{
			char temp=data[i];

			if (temp>='A'&&temp<='F')
			{
				temp=temp-'A'+10;
			}
			else if (temp>='a'&&temp<='f')
			{
				temp=temp-'a'+10;
			}
			else
			{
				temp-='0';
			}

			for (j=0;j<KEYLENGTH/2;++j)
			{
				finalData[i*4+(KEYLENGTH/2-1-j)]=(((temp&(1<<j)))>>j)+'0';
			}
		}
	}
	finalData[64]=0;

	//检查是否为弱密钥
	for (i=0;i<31;++i)
	{
		if (finalData[i]!=finalData[i+1])
		{
			weakKeyFlag=0;
			break;
		}
	}
	for (i=32;i<63;++i)
	{
		if (finalData[i]!=finalData[i+1])
		{
			weakKeyFlag=0;
			break;
		}
	}

	if (weakKeyFlag)
	{
		*pValidFlag=-3;
		delete data;
		delete finalData;
		return NULL;
	}

	*pValidFlag=1;
	delete data;
	return finalData;
}
//检查明文的合法性
int checkValidPlainText(CString plaintext)
{
	if (plaintext.GetLength()==0)
	{
		return 0;
	}

	return 1;
}
void genAllSubKey(char* oriKey,char subEncodeKey[16][48],char subDecodeKey[16][48])
{
	char Ci[28],Di[28];
	int leftShiftTimes[16]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1,};
	int PC_1Pass[56]=
	{
		57,49,41,33,25,17,9,
		1,58,50,42,34,26,18,
		10,2,59,51,43,35,27,
		19,11,3,60,52,44,36,
		63,55,47,39,31,23,15,
		7,62,54,46,38,30,22,
		14,6,61,53,45,37,29,
		21,13,5,28,20,12,4,
	};
	int PC_2Pass[48]=
	{
		14,17,11,24,1,5,
		3,28,15,6,21,10,
		23,19,12,4,26,8,
		16,7,27,20,13,2,
		41,52,31,37,47,55,
		30,40,51,45,33,48,
		44,49,39,56,34,53,
		46,42,50,36,29,32,
	};
	int i,j;

	//得到C0与D0
	for (i=0;i<28;++i)
	{
		Ci[i]=oriKey[PC_1Pass[i]-1];
	}
	for (i=28;i<56;++i)
	{
		Di[i-28]=oriKey[PC_1Pass[i]-1];
	}

	//生成子密钥
	for (i=0;i<16;++i)
	{
		if (leftShiftTimes[i]==1)
		{
			char tempChar1=Ci[0];
			char tempChar2=Di[0];

			for (j=0;j<27;++j)
			{
				Ci[j]=Ci[j+1];
				Di[j]=Di[j+1];
			}

			Ci[27]=tempChar1;
			Di[27]=tempChar2;
		}
		else
		{
			char tempChar1=Ci[0];
			char tempChar2=Ci[1];
			char tempChar3=Di[0];
			char tempChar4=Di[1];

			for (j=0;j<26;++j)
			{
				Ci[j]=Ci[j+2];
				Di[j]=Di[j+2];
			}

			Ci[26]=tempChar1;
			Ci[27]=tempChar2;
			Di[26]=tempChar3;
			Di[27]=tempChar4;
		}

		char tempString[56];

		for (j=0;j<28;++j)
		{
			tempString[j]=Ci[j];
		}
		for (j=28;j<56;++j)
		{
			tempString[j]=Di[j-28];
		}

		for (j=0;j<48;++j)
		{
			subEncodeKey[i][j]=tempString[PC_2Pass[j]-1];
			subDecodeKey[15-i][j]=tempString[PC_2Pass[j]-1];
		}
	}
}

char* CStringToBit_64Grouped(CString oriData,int* pLength)
{
	int oriPlaintextLengthInBytes=oriData.GetLength()*2;
	int newPlaintextLengthInBytes;
	int i,j;
	char* oriPlaintextInChar = (char*)(LPCTSTR)oriData;

	//将明文的Unicode编码变为char型
	if (oriPlaintextLengthInBytes%8==0)
	{
		newPlaintextLengthInBytes=oriPlaintextLengthInBytes;
	}
	else
	{
		newPlaintextLengthInBytes=(oriPlaintextLengthInBytes/8+1)*8;
	}


	char* newPlaintextInChar = new char[newPlaintextLengthInBytes];
	memset(newPlaintextInChar,0,newPlaintextLengthInBytes);

	for (i=0;i<oriPlaintextLengthInBytes;++i)
	{
		newPlaintextInChar[i]=oriPlaintextInChar[i];
	}

	//将明文的char型变为bit
	char* newPlaintextInBit=new char[newPlaintextLengthInBytes*8];
	int newPlaintextLengthInBit = newPlaintextLengthInBytes*8;
	for (i=0;i<newPlaintextLengthInBytes;++i)
	{
		for (j=0;j<8;++j)
		{
			newPlaintextInBit[i*8+(8-1-j)] = (((newPlaintextInChar[i]&(1<<j)))>>j)+'0';
		}
	}

	*pLength = newPlaintextLengthInBit;
	delete newPlaintextInChar;
	return newPlaintextInBit;
}

char* BinToHex_4Grouped(char* inputBinData,int lengthInBin)
{
	char* result=new char[lengthInBin/4+1];
	int i,tempInt;

	for (i=0;i<lengthInBin/4;++i)
	{
		tempInt=(inputBinData[i*4]-'0')*8+(inputBinData[i*4+1]-'0')*4+(inputBinData[i*4+2]-'0')*2+(inputBinData[i*4+3]-'0');

		if (tempInt>=0&&tempInt<=9)
		{
			result[i]=tempInt+'0';
		}
		else
		{
			result[i]=tempInt-10+'A';
		}
	}

	result[lengthInBin/4]=0;

	return result;
}

void IPTransform(char* bitGroup)
{
	int IPMatrix[64]=
	{
		58,50,42,34,26,18,10,2,
		60,52,44,36,28,20,12,4,
		62,54,46,38,30,22,14,6,
		64,56,48,40,32,24,16,8,
		57,49,41,33,25,17,9,1,
		59,51,43,35,27,19,11,3,
		61,53,45,37,29,21,13,5,
		63,55,47,39,31,23,15,7,
	};

	char tempString[64];

	for (int i=0;i<64;++i)
	{
		tempString[i]=bitGroup[i];
	}

	for (int i=0;i<64;++i)
	{
		bitGroup[i]=tempString[IPMatrix[i]-1];
	}
}
void EExtension(char* Ri,char* result)
{
	int EExtensionMatrix[48]=
	{
		32,1,2,3,4,5,
		4,5,6,7,8,9,
		8,9,10,11,12,13,
		12,13,14,15,16,17,
		16,17,18,19,20,21,
		20,21,22,23,24,25,
		24,25,26,27,28,29,
		28,29,30,31,32,1,
	};

	for (int i=0;i<48;++i)
	{
		result[i]=Ri[EExtensionMatrix[i]-1];
	}
}
void xorSubKey(char* data,char* subKey)
{
	int i;
	char tempData[48];

	for (i=0;i<48;++i)
	{
		tempData[i]=data[i];
	}

	for (i=0;i<48;++i)
	{
		if (tempData[i]==subKey[i])
		{
			data[i]='0';
		}
		else
		{
			data[i]='1';
		}
	}
}

void SBoxPass(char inData[48],char outData[32])
{
	int allSBox[8][4][16]=
	{
		{
			{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
			{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
			{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
			{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
		},
		{
			{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
			{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
			{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
			{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
		},
		{
			{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
			{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
			{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
			{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
		},
		{
			{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
			{13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
			{10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
			{3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
		},
		{
			{2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
			{14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
			{4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
			{11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
		},
		{
			{12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
			{10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
			{9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
			{4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
		},
		{
			{4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
			{13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
			{1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
			{6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
		},
		{
			{13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
			{1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
			{7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
			{2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
		}
	};

	for (int i=0;i<8;++i)
	{
		char x[2];
		char y[4];
		int out;

		x[0]=inData[i*6+0]-'0';
		x[1]=inData[i*6+5]-'0';

		y[0]=inData[i*6+1]-'0';
		y[1]=inData[i*6+2]-'0';
		y[2]=inData[i*6+3]-'0';
		y[3]=inData[i*6+4]-'0';

		out=allSBox[i][x[0]*2+x[1]][y[0]*8+y[1]*4+y[2]*2+y[3]];

		outData[i*4+3]=(out&1)/1+'0';
		outData[i*4+2]=(out&2)/2+'0';
		outData[i*4+1]=(out&4)/4+'0';
		outData[i*4+0]=(out&8)/8+'0';
	}
}

void PTransform(char data[32])
{
	int PTransformMatrix[32]=
	{
		16,7,20,21,
		29,12,28,17,
		1,15,23,26,
		5,18,31,10,
		2,8,24,14,
		32,27,3,9,
		19,13,30,6,
		22,11,4,25
	};
	char tempData[32];
	int i;

	for (i=0;i<32;++i)
	{
		tempData[i]=data[i];
	}

	for (i=0;i<32;++i)
	{
		data[i]=tempData[PTransformMatrix[i]-1];
	}
}

void IPRevTransform(char data[64])
{
	int IPRevTransformMatrix[64]=
	{
		40,8,48,16,56,24,64,32,
		39,7,47,15,55,23,63,31,
		38,6,46,14,54,22,62,30,
		37,5,45,13,53,21,61,29,
		36,4,44,12,52,20,60,28,
		35,3,43,11,51,19,59,27,
		34,2,42,10,50,18,58,26,
		33,1,41,9,49,17,57,25
	};
	char tempData[64];

	for (int i=0;i<64;++i)
	{
		tempData[i]=data[i];
	}

	for (int i=0;i<64;++i)
	{
		data[i]=tempData[IPRevTransformMatrix[i]-1];
	}
}

char* DESEncode(char* plaintextInBit,int plaintextLengthInBit,char subEncodeKey[16][48])
{
	int i,j,k;
	char* result=new char[plaintextLengthInBit];
	char bitGroup[64];
	char Li[32];
	char oriLi[32];
	char Ri[32];
	char oriRi[32];
	char afterEExtension[48];
	char afterSBox[32];

	//对每个64bit加密
	for (i=0;i<plaintextLengthInBit/64;++i)
	{
		for (j=0;j<64;++j)
		{
			bitGroup[j]=plaintextInBit[i*64+j];
		}

		//IP置换
		IPTransform(bitGroup);

		//得到L0与R0
		for (j=0;j<32;++j)
		{
			Li[j]=bitGroup[j];
		}
		for (j=0;j<32;++j)
		{
			Ri[j]=bitGroup[j+32];
		}

		//进行16圈的加密
		for (j=0;j<16;++j)
		{
			//对Ri进行E扩展
			EExtension(Ri,afterEExtension);
			//将结果与子密钥进行异或操作
			xorSubKey(afterEExtension,subEncodeKey[j]);
			//进入S盒进行处理
			SBoxPass(afterEExtension,afterSBox);
			//进行P置换
			PTransform(afterSBox);
			//保留原始的Li与Ri
			for (k=0;k<32;++k)
			{
				oriLi[k]=Li[k];
				oriRi[k]=Ri[k];
			}
			//新的Li即为原来的oriRi
			for (k=0;k<32;++k)
			{
				Li[k]=oriRi[k];
			}
			//新的Ri即为oriLi与此时的afterSBox异或的结果
			for (k=0;k<32;++k)
			{
				if (oriLi[k]==afterSBox[k])
				{
					Ri[k]='0';
				}
				else
				{
					Ri[k]='1';
				}
			}
		}

		//最后一次加密完成后应该将此时的Li与Ri互换
		char tempChar;
		for (j=0;j<32;++j)
		{
			tempChar=Li[j];
			Li[j]=Ri[j];
			Ri[j]=tempChar;
		}

		//进行IP逆置换
		for (j=0;j<32;++j)
		{
			bitGroup[j]=Li[j];
			bitGroup[j+32]=Ri[j];
		}
		IPRevTransform(bitGroup);

		//将已加密的数据写入结果
		for (j=0;j<64;++j)
		{
			result[i*64+j]=bitGroup[j];
		}
	}

	return result;
}
char* CStringInHexToBit_64Grouped(CString inputDataInHex,int* pOriDataInBitLength)
{
	char* result = new char[inputDataInHex.GetLength()*4];
	char* temp =(char*)(LPCTSTR)inputDataInHex;
	int i;

	*pOriDataInBitLength=inputDataInHex.GetLength()*4;

	for (i=0;i<inputDataInHex.GetLength();++i)
	{
		int tempInt;

		if ((inputDataInHex[i]>='0')&&(inputDataInHex[i]<='9'))
		{
			tempInt=temp[2*i]-'0';
		}
		else if ((inputDataInHex[i]>='A')&&(inputDataInHex[i]<='F'))
		{
			tempInt=temp[2*i]+10-'A';
		}
		else
		{
			tempInt=temp[2*i]+10-'a';
		}

		result[i*4+3]=(tempInt&1)/1+'0';
		result[i*4+2]=(tempInt&2)/2+'0';
		result[i*4+1]=(tempInt&4)/4+'0';
		result[i*4+0]=(tempInt&8)/8+'0';
	}

	return result;
}

CString HexToCString(char* dataInHex,int lengthOfDataInHex)
{
	int i;

	char* tempString=new char[lengthOfDataInHex/4+1];
	for (i=0;i<lengthOfDataInHex/4;++i)
	{
		tempString[i]=' ';
	}
	tempString[i]='\0';

	CString result(tempString);

	char* tempString2=(char*)(LPCTSTR)result;
	for (i=0;i<lengthOfDataInHex/4;++i)
	{
		int tempInt[4];

		tempInt[0]=dataInHex[i*4+0];
		tempInt[1]=dataInHex[i*4+1];
		tempInt[2]=dataInHex[i*4+2];
		tempInt[3]=dataInHex[i*4+3];

		for (int j=0;j<4;++j)
		{
			if ((tempInt[j]>='0')&&(tempInt[j]<='9'))
			{
				tempInt[j]=tempInt[j]-'0';
			}
			else
			{
				tempInt[j]=tempInt[j]-'A'+10;
			}
		}

		tempString2[i*2+0]=tempInt[0]*16+tempInt[1];
		tempString2[i*2+1]=tempInt[2]*16+tempInt[3];
	}
	delete tempString;
	return result;
}
