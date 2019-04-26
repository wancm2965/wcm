#include "stdafx.h"
#include "DRHeader.h"

/*读取CAD Header版本信息 扩展使用*/
BOOL dxfReadHeaderData(HDXF hDxf, PDXFHEADER pHeader)
{
	PDXF pDxf;

	// Initialize pDxf ------------------
	if((pDxf = InitilizePDXF(hDxf))==NULL)
		return FALSE;

	if(!ReadHeader(pDxf, pHeader))
	{
		GlobalUnlock(hDxf);
		return FALSE;
	}

	// UnInitilize hDxf -----------------
	return UnInitilizePDXF(hDxf);
}

BOOL ReadHeader(PDXF pDxf, PDXFHEADER pHeader)
{
	int     GCode;
	char	strValue[2048];

	// Check if current section is HEADER
	if(pDxf->Read.CurrentSection!=SEC_HEADER)
	{
		// Current section is not HEADER
		return FALSE;
	}

	dxfStorePos(pDxf);
	ReadParamFromDxfFile(pDxf, GCode, strValue);
	while(GCode!=0)
	{
		switch(GCode)
		{
		case 1:
			break;
		default: 
			break;
		}

		dxfStorePos(pDxf);
		ReadParamFromDxfFile(pDxf, GCode, strValue);
	}

	if((GCode==0) && (strcmp(strValue, "ENDSEC")==0))
		pDxf->Read.CurrentSection = SEC_NOTSET; // header section has been finished
	else
		dxfRestorePos(pDxf);

	return TRUE;
}