#pragma once

#include <fstream>
using namespace std;

enum DATA_ENDIAN
{
	LITTLE_ENDIAN,
	BIG_ENDIAN
};

enum DICOM_ERROR_CODE
{
	DICOM_TOBMP_SUCCEES =1,      //ת���ɹ�
	DICOM_ERROR_OPENFAILED = -1 ,//�ļ���ʧ��
	DICOM_FILEWRITEFAIL = -2  ,    //�ļ�ת��ʧ��
	FREEDISK_NOENOUGH = -3 ,  //�����ļ�ʱʣ����̿ռ䲻��
	NO_DICOM_FIEL = -4       //�򿪵Ĳ���dicom�ļ�
};


class CDicomToBmp
{
public:
	CDicomToBmp(void);
	~CDicomToBmp(void);
	int ConvertDicomToBMP(CString strFileName,CString strDesFilePath);
	void GetBMPFileName(char *pszPath,int nLen);
private:
	int	WriteBMPFile(char *pData, int nFrameSize, short nCols, short nRows, int nBytesP, char *pszPhotometric, CString strFileName, CString strDesFilePath, int nFrameNum=0);

	void removeTailingSpace(char *pszStr);
	void SwapWord(char *pArray, int nWords);

	void SwapDWord(char *pArray, int nDWords);

	int readUS(FILE *fp, DATA_ENDIAN nDataEndian);
	long int readLength(FILE *fp, BOOL bImplicitVR, DATA_ENDIAN nDataEndian);
	int readIS(FILE *fp, BOOL bImplicitVR, DATA_ENDIAN nDataEndian);
	int readString(FILE *fp, char *pszStr, BOOL bImplicitVR, DATA_ENDIAN nDataEndian);
	float readDS(FILE *fp, BOOL bImplicitVR, DATA_ENDIAN nDataEndian);
	char *convertTo8Bit(char *pData, long int nNumPixels, BOOL bSigned, short nHighBit, 
		float fRescaleSlope, float fRescaleIntercept, 
		float fWindowCenter, float fWindowWidth);
public:
	CString strDesFilePath; //ת�����ļ����õ�·����
private:
	char m_szBMPFileName[512];
	
};
