#include "stdafx.h"
#include "DicomToBmp.h"


enum COMPRESSION_MODE
{
	COMPRESS_NONE = 0,
};

CDicomToBmp::CDicomToBmp(void)
{

}

CDicomToBmp::~CDicomToBmp(void)
{

}

void CDicomToBmp::removeTailingSpace(char *pszStr)
{
	char *cc = pszStr + strlen(pszStr) - 1;

	while (((*cc == ' ') || (*cc == '\t')) && (cc != pszStr))
	{
		*cc -- = '\0';
	}
}
void CDicomToBmp::SwapWord(char *pArray, int nWords)
{
	char *cc = pArray, c0;
	int i;

	// Swap every two bytes
	for (i = 0; i < nWords; i ++)
	{
		c0 = *cc;
		*cc = *(cc + 1);
		*(cc + 1)  = c0;

		cc += 2;
	}
}
void CDicomToBmp::SwapDWord(char *pArray, int nDWords)
{
	char *cc = pArray, c0;
	int i;

	// Rotate every four bytes
	for (i = 0; i < nDWords; i ++)
	{

		// Swap first and last bytes
		c0 = *cc;
		*cc = *(cc + 3);
		*(cc + 3)  = c0;

		// Swap middle two bytes
		c0 = *(cc + 2);
		*(cc + 2) = *(cc + 1);
		*(cc + 1)  = c0;

		cc += 4;
	}
}
int CDicomToBmp::readUS(FILE *fp, DATA_ENDIAN nDataEndian)
{
	unsigned short nVal;

	fseek(fp, 4, SEEK_CUR); // Skip VR and Length bytes (4)
	fread(&nVal, 1, sizeof(short), fp); // read the unsigned short value
	if (nDataEndian == BIG_ENDIAN)
		SwapWord((char *) &nVal, 1);

	return (int) nVal;

}
long int CDicomToBmp::readLength(FILE *fp, BOOL bImplicitVR, DATA_ENDIAN nDataEndian)
{
	long int nValLength = 0;
	short int nsLength;

	if (bImplicitVR)
	{
		fread(&nValLength, sizeof(long), 1, fp);
		if (nDataEndian == BIG_ENDIAN)
			SwapDWord((char *) &nValLength, 1);
	}
	else
	{
		fseek(fp, 2, SEEK_CUR); // Skip 2 VR bytes

		fread(&nsLength, sizeof(short), 1, fp);
		if (nDataEndian == BIG_ENDIAN)
			SwapWord((char *) &nsLength, 1);

		nValLength = nsLength;
	}

	return nValLength;
}
int CDicomToBmp::readIS(FILE *fp, BOOL bImplicitVR, DATA_ENDIAN nDataEndian)
{
	char szTemp[64]="";
	int nVal = 0;

	if (readString(fp, szTemp, bImplicitVR, nDataEndian) == 0)
	{
		sscanf_s(szTemp, "%d", &nVal);
	}

	return nVal;
}
float CDicomToBmp::readDS(FILE *fp, BOOL bImplicitVR, DATA_ENDIAN nDataEndian)
{
	char szTemp[64]="";
	float fVal = 0;

	if (readString(fp, szTemp, bImplicitVR, nDataEndian) == 0)
		sscanf_s(szTemp, "%f", &fVal);

	return fVal;
}
int CDicomToBmp::readString(FILE *fp, char *pszStr, BOOL bImplicitVR, DATA_ENDIAN nDataEndian)
{
	long int nValLength = 0;

	nValLength = readLength(fp, bImplicitVR, nDataEndian);

	if ((nValLength > 64) || (nValLength < 0))
		return -1;

	fread(pszStr, 1, nValLength, fp); 
	pszStr[nValLength] = '\0';
	removeTailingSpace(pszStr);

	return 0;
}
char *CDicomToBmp::convertTo8Bit(char *pData, long int nNumPixels, BOOL bIsSigned, short nHighBit, 
	float fRescaleSlope, float fRescaleIntercept, 
	float fWindowCenter, float fWindowWidth)
{
	unsigned char *pNewData = 0;
	long int nCount;
	short *pp;

	// 1. Clip the high bits.
	if (nHighBit < 15)
	{
		short nMask;
		short nSignBit;

		pp = (short *)pData;
		nCount = nNumPixels;

		if(bIsSigned == 0 ) // Unsigned integer
		{ 
			nMask = 0xffff << (nHighBit + 1);

			while( nCount-- > 0 )
				*(pp ++) &= ~nMask;
		}
		else
		{		
			// 1's complement representation

			nSignBit = 1 << nHighBit;
			nMask = 0xffff << (nHighBit + 1);
			while( nCount -- > 0 )
			{
				if ((*pp & nSignBit) != 0)
					*(pp ++) |= nMask;
				else
					*(pp ++) &= ~nMask;
			}
		}
	}

	// 2. Rescale if needed (especially for CT)
	if ((fRescaleSlope != 1.0f) || (fRescaleIntercept != 0.0f))
	{
		float fValue;

		pp = (short *)pData;
		nCount = nNumPixels;

		while( nCount-- > 0 ) 
		{
			fValue = (*pp) * fRescaleSlope + fRescaleIntercept;
			*pp ++ = (short)fValue;
		}

	}

	// 3. Window-level or rescale to 8-bit
	if ((fWindowCenter != 0) || (fWindowWidth != 0)) 
	{
		float fSlope;
		float fShift;
		float fValue;
		unsigned char *np = new unsigned char[nNumPixels+4];

		pNewData = np;

		// Since we have window level info, we will only map what are
		// within the Window.

		fShift = fWindowCenter - fWindowWidth / 2.0f;
		fSlope = 255.0f / fWindowWidth;

		nCount = nNumPixels;
		pp = (short *)pData;

		while (nCount-- > 0) 
		{
			fValue = ((*pp ++) - fShift) * fSlope;
			if (fValue < 0)
				fValue = 0;
			else if (fValue > 255)
				fValue = 255;

			*np ++ = (unsigned char) fValue;
		}

	}
	else
	{
		// We will map the whole dynamic range.
		float fSlope;
		float fValue;
		int nMin, nMax;
		unsigned char *np = new unsigned char[nNumPixels+4];

		pNewData = np;

		// First compute the min and max.
		nCount = nNumPixels;
		pp = (short *)pData;
		nMin = nMax = *pp;
		while (nCount-- > 0)
		{
			if (*pp < nMin)
				nMin = *pp;

			if (*pp > nMax)
				nMax = *pp;

			pp ++;
		}

		// Calculate the scaling factor.
		if (nMax != nMin)
			fSlope = 255.0f / (nMax - nMin);
		else
			fSlope = 1.0f;

		nCount = nNumPixels;
		pp = (short *)pData;
		while (nCount-- > 0) 
		{
			fValue = ((*pp ++) - nMin) * fSlope;
			if (fValue < 0)
				fValue = 0;
			else if (fValue > 255)
				fValue = 255;

			*np ++ = (unsigned char) fValue;
		}
	}

	return (char *)pNewData;
}



int CDicomToBmp::ConvertDicomToBMP(CString strFileName,CString strDesFilePath)
{

	short int nCols = 0, nRows = 0;
	short int nBitsAllocated, nSamplesPerPixel = 1;
	short int nHighBit = 0;
	float fWindowWidth = 0, fWindowCenter = 0 , fRescaleSlope = 1, fRescaleIntercept = 0;
	BOOL bIsSigned = FALSE;

	BOOL bGroup2Done = FALSE, bGroup28Done = FALSE, bPixelDataDone = FALSE;

	int nBytesP = 0;
	int nFrameSize = 0;
	long int nLength;
	const char *pszFileName =(char *) strFileName.GetBuffer(3);
	char szPhotometric[32]="", szTemp[32]="", szTransferSyntaxUID[80]="";

	BOOL bImplicitVR = TRUE;
	COMPRESSION_MODE nCompressionMode = COMPRESS_NONE;
	DATA_ENDIAN nDataEndian = LITTLE_ENDIAN;
	int i;
	int nBytes;


	FILE *fp;
	char *pData = 0;
	short int gTag, eTag;
	int nNumFrames = 1;
	//CString strFile;

	//FileName = FileDlg.GetPathName();
	fstream files;
	WCHAR filen[256];
	::MultiByteToWideChar(CP_ACP,0,(char *)strFileName.GetBuffer(), -1,filen, 256);//一个字符串到一个宽字符（unicode）的字符串

	files.open(filen);
	char cdicom[4];
	files.seekg(128);
	files.read(cdicom,4);
	if (cdicom[0] != 'D'
		||cdicom[1] != 'I'
		||cdicom[2] != 'C'
		||cdicom[3] != 'M')
	{
		//AfxMessageBox("不是dicom文件");
		return NO_DICOM_FIEL;
	}

	/*if (strcmp(cdicom,"DICM"))
		{
			AfxMessageBox("不是dicom文件");
		}*/

	fp = fopen(pszFileName, "rb");
	if (!fp)
	{
		//AfxMessageBox("Failed to open file for read.");
		return DICOM_ERROR_OPENFAILED;
	}
	
	while(fread(&gTag, sizeof(short), 1, fp) == 1)
	{
		if (nDataEndian == BIG_ENDIAN)
			SwapWord((char *) &gTag, 1);

		switch(gTag)
		{
		case 0x0002: // Meta header. 

			if (bGroup2Done)
				break;

			fread(&eTag, sizeof(short), 1, fp);
			// Meta header is always in Little Endian Explicit VR syntax.
			switch(eTag)
			{
			case 0x0010: // Transfer syntax UID
				if (readString(fp, szTransferSyntaxUID, FALSE, LITTLE_ENDIAN) != 0)
					break;

				// Check data endian.
				if (!strcmp(szTransferSyntaxUID, "1.2.840.10008.1.2.2")) // Explicit VR Big Endian 
					nDataEndian = BIG_ENDIAN; // Big Endian
				else
					nDataEndian = LITTLE_ENDIAN; // Little Endian
				break;
			}
			break;

		case 0x0008: // First non-Meta group
			fread(&eTag, sizeof(short), 1, fp);
			if (nDataEndian == BIG_ENDIAN)
				SwapWord((char *) &eTag, 1);

			if ((eTag == 0x0005) || (eTag == 0x0008))
				bGroup2Done = TRUE;
			break;

		case 0x0028: // Image pixel data info group
			fread(&eTag, sizeof(short), 1, fp);

			if (bGroup28Done)
				break;

			if (nDataEndian == BIG_ENDIAN)
				SwapWord((char *) &eTag, 1);

			switch(eTag)
			{
			case 0x0002: // Samples per Pixel
				nSamplesPerPixel = readUS(fp, nDataEndian);
				break;

			case 0x0004:  // Photometric Interpolation
				readString(fp, szPhotometric, bImplicitVR, nDataEndian);
				break;

			case 0x0008:  // Number of frames
				nNumFrames = readIS(fp, bImplicitVR, nDataEndian);
				break;

			case 0x0010: // Rows
				nRows = readUS(fp, nDataEndian);
				break;

			case 0x0011: // Columns
				nCols = readUS(fp, nDataEndian);
				break;

			case 0x0100: // Bits allocated
				nBitsAllocated = readUS(fp, nDataEndian);
				break;

			case 0x0102: // High bit
				nHighBit = readUS(fp, nDataEndian);
				break;

			case 0x0103: // Is signed?
				bIsSigned = readUS(fp, nDataEndian);
				break;

			case 0x1050: // Window Center
				fWindowCenter = readDS(fp, bImplicitVR, nDataEndian);
				break;

			case 0x1051: // Window Width
				fWindowWidth = readDS(fp, bImplicitVR, nDataEndian);
				break;

			case 0x1052: // Rescale intercept
				fRescaleIntercept = readDS(fp, bImplicitVR, nDataEndian);
				break;

			case 0x1053: // Rescale slope
				fRescaleSlope = readDS(fp, bImplicitVR, nDataEndian);
				bGroup28Done = TRUE;
				break;

			default:
				// do nothing
				break;
			}
			break;
		case 0x7fe0:
			fread(&eTag, sizeof(short), 1, fp);
			if (bPixelDataDone)
				break;

			if (nDataEndian == BIG_ENDIAN)
				SwapWord((char *) &eTag, 1);

			if (eTag == 0x0010)
			{
				nBytesP = nSamplesPerPixel*nBitsAllocated/8;
				nFrameSize = nCols * nRows * nBytesP;
				nLength = nNumFrames * nFrameSize;

				// Don't try to parse grup 2 and group 28 any more
				bGroup2Done = TRUE;
				bGroup28Done = TRUE;

				// Parse pixel data
				if(nCompressionMode == COMPRESS_NONE)
				{
					pData = new char[nLength + 16];
					fseek(fp, 4, SEEK_CUR); // Skip 4 bytes (length bytes)
					nBytes = fread(pData, 1, nLength, fp); 

					if (nBytes != nLength)
					{
						//AfxMessageBox("Failed to read all pixel data.");
						return DICOM_ERROR_OPENFAILED;
					}
					bPixelDataDone = TRUE;
				}

			}
			break;
		}

		if (pData) 
			break; // We are done.
	}
	strFileName.ReleaseBuffer();
	fclose(fp);

	if (pData) // Have we got the pixel data?
	{
		// Need to do byte swap?
		if (nDataEndian == BIG_ENDIAN)
		{
			if (nBitsAllocated > 8)
				SwapWord(pData, nLength/2);
		}

		if (nBitsAllocated > 8)
		{
			// We need to convert it to 8-bit.
			char *pNewData;

			pNewData = convertTo8Bit(pData, nLength/2, bIsSigned, nHighBit,
				fRescaleSlope, fRescaleIntercept, fWindowCenter, fWindowWidth);

			// Use the new 8-bit data.
			if (pNewData)
			{
				delete [] pData;
				pData = pNewData;
				nBytesP = 1;
				nFrameSize /= 2;

				nLength /= 2;
			}
		}

		// Write BMP files
		for (i = 0; i < nNumFrames; i++)
			WriteBMPFile(pData + nFrameSize*i, nFrameSize, nCols, nRows, nBytesP, szPhotometric, strFileName, strDesFilePath, i+1);

		// Free the memory.
		delete [] pData;
		return DICOM_TOBMP_SUCCEES;
	}
	return DICOM_TOBMP_SUCCEES;

}
int	CDicomToBmp::WriteBMPFile(char *pData, int nFrameSize, short nCols, short nRows, int nBytesP, char *pszPhotometric, CString strFileName, CString strDesFilePath, int nFrameNum)
{
	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;

	CString str;
	int nPaletteSize = sizeof(RGBQUAD)*256;
	char *cc;
	
	FILE *fp;
	int i, j, nBytes, nRowPadding, nRowBytes;
	unsigned char oneLutSlot[4];
	char szPadding[4] = {'\0', '\0', '\0', '\0'};

	ULARGE_INTEGER nFreeBytesAvailable;
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;

	ULONGLONG presize = 0;//保存剩余磁盘文件的大小
	if (!strncmp(pszPhotometric, "RGB", 3) || !strncmp(pszPhotometric, "YBR", 3))
		nPaletteSize = 0;

	// Attention:
	//  1. BMP image rows need to be 4-byte aligned.
	//  2. BMP image is usually bottom up in reverse direction to DICOM images.
	nRowBytes = nCols*nBytesP;
	nRowPadding = nRowBytes%4;

	//here fill in the two info structure
	bf.bfType = 0x4D42; 
	bf.bfSize= (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nPaletteSize + nFrameSize + nRowPadding*nRows)/1024;//数据的大小，转换成M
	bf.bfReserved1 = 0; 
	bf.bfReserved2 = 0; 
	bf.bfOffBits=sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nPaletteSize; 

	bi.biSize=sizeof(BITMAPINFOHEADER);
	bi.biWidth = nCols; 
	bi.biHeight = nRows; 
	bi.biPlanes = 1;
	if (nPaletteSize == 0)
	{
		bi.biBitCount = 24;
		bi.biClrUsed = 0; 
		bi.biClrImportant = 0;
	}
	else
	{
		bi.biBitCount = 8;
		bi.biClrUsed = 256; 
		bi.biClrImportant = 256;
	}
	bi.biCompression = 0; 
	bi.biSizeImage = nFrameSize;

	//
	// Resolutions are in pixels per meter.  Our IMAGES file uses
	// a resolution of 72 pixels per inch, which when multiplied by
	// 39.37 inches per meter, gives us 2835 pixels per meter.
	//
	bi.biXPelsPerMeter = 2835; 
	bi.biYPelsPerMeter = 2835;

	// Make file name and give it a .BMP extension.

	//strcpy_s(m_szBMPFileName, strDesFilePath.GetBuffer(3));
	strcpy(m_szBMPFileName, (char *)strDesFilePath.GetBuffer(3));
	str += strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind('\\') - 1);
	str = str.Left(str.ReverseFind('.'));    //获得文件的名字，去除它的文件后缀名

	if (!(cc = strrchr(m_szBMPFileName, '\0'))) //将cc指针移动到m_szBMPFileName的尾部
		cc = m_szBMPFileName + strlen(m_szBMPFileName);
	sprintf(cc, "\\%s.BMP", str.GetBuffer()); // 给转换后的文件命名；

	// Open the file for wirte.
	fp = fopen(m_szBMPFileName, "wb");
	if (fp == NULL)
		//AfxMessageBox("Failed to open file for write.");
	      return DICOM_ERROR_OPENFAILED;


	GetDiskFreeSpaceEx(strDesFilePath, &nFreeBytesAvailable, &nTotalNumberOfBytes, &nTotalNumberOfFreeBytes);//剩余的磁盘空间
	presize = nTotalNumberOfFreeBytes.QuadPart/1024;//转换成相应的M；
	
	if (presize>= bf.bfSize)
	{
		//AfxMessageBox("文件可以顺利的保存");
		// Write the BMP and DIB file headers.
		nBytes = fwrite((void *)&bf, 1, sizeof(BITMAPFILEHEADER), fp);
		if (nBytes != sizeof(BITMAPFILEHEADER))
		{
			//AfxMessageBox("文件写入失败");
			return DICOM_FILEWRITEFAIL;
		}
		//    post your error message here.	
		nBytes = fwrite((void *)&bi, 1, sizeof(BITMAPINFOHEADER), fp);
		if (nBytes != sizeof(BITMAPINFOHEADER))
		{
			//AfxMessageBox("文件写入失败");
			return DICOM_FILEWRITEFAIL;
		}
		//    post your error message here.
	}
	else
	{
		//AfxMessageBox("磁盘空间不足");
		return FREEDISK_NOENOUGH;
	}

	// Write the color palette if 8-bit
	if (nPaletteSize != 0)
		for (i = 0; i < 256; i++)
		{
			oneLutSlot[0] = oneLutSlot[1] = oneLutSlot[2] = i; // Grayscale LUT.
			oneLutSlot[3] = 0;
			fwrite(oneLutSlot, 1, 4, fp); // Write one color palette slot.
		}

		// Now write the actual pixel data.
		cc = pData + (nRows - 1)*nRowBytes;
		if (nPaletteSize != 0) // Grayscale
			for (i = 0; i < nRows; i ++)
			{
				nBytes = fwrite(cc, 1, nRowBytes, fp); // Write a row.
				//if (nBytes != nRowBytes)
				//    post your error message here.

				if (nRowPadding) // Pad the row.
					fwrite(szPadding, 1, nRowPadding, fp);

				cc -= nRowBytes;
			}
		else
			for (i = 0; i < nRows; i ++)
			{
				for (j = 0; j < nCols; j++, cc += 3)
				{
					fputc(*(cc + 2), fp); // B
					fputc(*(cc + 1), fp); // G
					fputc(*cc, fp);       // R
				}

				if (nRowPadding) // Pad the row.
					fwrite(szPadding, 1, nRowPadding, fp);

				cc -= 2*nRowBytes;
			}
			strDesFilePath.ReleaseBuffer();
			str.ReleaseBuffer();
			fclose(fp);

			return 0;
}

void CDicomToBmp::GetBMPFileName(char *pszPath,int nLen )
{
	
	strncpy(pszPath , m_szBMPFileName,nLen);
}

