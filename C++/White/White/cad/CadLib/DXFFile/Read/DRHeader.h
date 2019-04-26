#ifndef DRHEADER_H
#define DRHEADER_H

#if _MSC_VER > 1000
#pragma once
#endif //_MSC_VER > 1000

#include "..\Base\DXFBase.h"

//---------------------------------------------------------------------------
// Function declarations
//---------------------------------------------------------------------------

BOOL  dxfReadHeaderData(
	HDXF     hDxf, // handle to the openning DXF file structure
	PDXFHEADER pHeader //pointer to header structure 
);

BOOL  ReadHeader(
	PDXF     hDxf, // handle to the openning DXF file structure
	PDXFHEADER pHeader //pointer to header structure 
);

#endif //DRHEADER_H