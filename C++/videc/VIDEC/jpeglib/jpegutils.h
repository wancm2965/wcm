// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jpegutils.h:	part of the mjpgTools Library																			 //
//				Some Utility programs for dealing with JPEG encoded images.												 //
//																														 //
//				!!! It seems the function <decode_jpeg_raw> isn't working properly - I (laurens) don't know why !!!		 // 
// --------------------------------------------------------------------------------------------------------------------- //
// Copyleft(c)2002 by Laurens KOEHOORN <https://sourceforge.net/sendmessage.php?touser=240832>							 //
//																														 //
// This library is free software; you can redistribute it and/or modify it under the terms of the						 //
// GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License,	 //
// or (at your option) any later version.																				 //
//																														 //
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied	 //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.														 //
// See the GNU Lesser General Public License for more details.															 //
//																														 //
// You should have received a copy of the GNU Lesser General Public License along with this library; if not,			 //
// write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA					 //
// --------------------------------------------------------------------------------------------------------------------- //
// This sub-project is a corrected version of :																			 //
//	MJPEGtools v1.6.0 (of the linux group)																				 //
//																														 //
//	Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de>																 //
//	Copyright (C) 2001 pHilipp Zabel  <pzabel@gmx.de>																	 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __JPEGUTILS_H__
#define __JPEGUTILS_H__

 /*
 * jpeg_data:       buffer with input / output jpeg
 * len:             Length of jpeg buffer
 * itype:           LAV_INTER_NONE: Not interlaced
 *                  LAV_INTER_TOP_FIRST: Interlaced, top-field-first
 *                  LAV_INTER_BOTTOM_FIRST: Interlaced, bottom-field-first
 * ctype            Chroma format for decompression.
 *                  Currently always 420 and hence ignored.
 * raw0             buffer with input / output raw Y channel
 * raw1             buffer with input / output raw U/Cb channel
 * raw2             buffer with input / output raw V/Cr channel
 * width            width of Y channel (width of U/V is width/2)
 * height           height of Y channel (height of U/V is height/2)
 */

#ifdef __cplusplus
extern "C" {
#endif


INT32	decode_jpeg_raw(UINT8* jpeg_data, INT32 len, INT32 width, INT32 height,
						UINT8* raw0, UINT8* raw1, UINT8* raw2);

INT32	encode_jpeg_raw(UINT8* jpeg_data, INT32 len, INT32 quality, INT32 width, INT32 height,
						UINT8* raw0, UINT8* raw1, UINT8* raw2);

INT32 WriteRGBBytesIntoJpegFile(const char*cszOutFileName, const int nWidth, const int nHeight, const int nQuality, UINT8* pBuffer);
INT32 WriteYUVBytesIntoJpegFile(const char*cszOutFileName, const int nWidth, const int nHeight, const int nQuality, UINT8* pY, UINT8* pU, UINT8* pV);


#ifdef __cplusplus
}
#endif

#endif // __JPEGUTILS_H__
