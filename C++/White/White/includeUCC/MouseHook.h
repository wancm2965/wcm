#pragma once
#include "RightKeyUp.h"
AFX_EXT_CLASS void StartDrawLine(CRightKeyUp *pMeeting);
AFX_EXT_CLASS void StopDrawLine();
AFX_EXT_CLASS void SetPenColor(COLORREF color);
AFX_EXT_CLASS void SetPenWidth(int  nWidth);
AFX_EXT_CLASS void SetEraseRect(CRect rect);
AFX_EXT_CLASS void SetDlgChannelLocalRect(CRect rect);
AFX_EXT_CLASS void StartCaptureRightKeyUp(CRightKeyUp *pMeeting);
AFX_EXT_CLASS void StopCaptureRightKeyUp();