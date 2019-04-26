		unsigned char*pFieldFrameData=(unsigned char*)malloc(nWidth*nHalfHeight*3);
		unsigned char*pDstY=pFieldFrameData;
		unsigned char*pDstU=pFieldFrameData+nHalfYLen;
		unsigned char*pDstV=pDstU+nHalfUVLen;

		unsigned char*pSrcY=pFrameData;
		unsigned char*pSrcU=pFrameData+nYLen;
		unsigned char*pSrcV=pFrameData+nYLen+nUVLen;

		memcpy(pDstY,pSrcY,nHalfYLen);
		memcpy(pDstU,pSrcU,nHalfUVLen);
		memcpy(pDstV,pSrcV,nHalfUVLen);

		pFieldFrameData=(unsigned char*)OnVideoFrameData(pFieldFrameData,nFrameLen,nWidth,nHeight,nFrameRate,nTimestamp,0,nInputFrameType);
		if (pFieldFrameData)
		{
			free(pFieldFrameData);
			pFieldFrameData=NULL;
		}

		pSrcU=pFrameData+nYLen;
		pSrcV=pSrcU+nUVLen;

		pDstU=pFrameData+nHalfYLen;
		pDstV=pDstU+nHalfUVLen;

		memcpy(pDstU,pSrcU,nHalfUVLen);
		memcpy(pDstV,pSrcV,nHalfUVLen);
