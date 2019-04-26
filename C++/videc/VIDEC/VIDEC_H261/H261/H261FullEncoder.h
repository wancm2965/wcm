// H261FullEncoder.h: interface for the H261FullEncoder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_H261FULLENCODER_H__C07F88E2_FD2B_4C13_BC60_F59531D19AD2__INCLUDED_)
#define AFX_H261FULLENCODER_H__C07F88E2_FD2B_4C13_BC60_F59531D19AD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <stdio.h>
#include "encoder-h261.h"
#include "videoframe.h"
#include "../motion/motion_comp.h"
#include "../motion/motion_est.h"
//#define  OUT_REC_YUV
class H261FullEncoder : public H261Encoder  
{
public:
	H261FullEncoder(Transmitter *T);
	virtual ~H261FullEncoder();
public:
	void SkipCurrentFrame();

    int consume(const VideoFrame*);
    void SetSize(int w, int h);
    int PreIncEncodeSetup(const VideoFrame*);
	void IncEncodeAndGetPacket(u_char * buffer, unsigned & length );
    /** When incrementally encoding, return true if there is still 
        more encoding waiting to be done for the current video frame.
    */ 
    int MoreToIncEncode() 
		{ return !gDone; }
	void FastUpdatePicture()
		{m_bIntra=TRUE;}
  protected:
	  void encode_mb(u_int mba, const u_char* frm,
		  u_int loff, u_int coff, int how);
	 	 	 
  protected:
	//to do 需要优化huffman表的产生
	FRAMEINFO* ImageCreate(MBParam mbParam);
	void ImageDestory(FRAMEINFO* frame);
	void InitEnc();
	void UnInitEnc();
	void InitParam();
	void WriteMBHeader();
	void EncodeAC(int index,short* matrix);
	void EncodeDC(int coef);
	void CBPEncodeAC(int index,short* matrix);
	int ReadCompressMDU();
	int WriteMDU();
	int DecodeSaveMDU();
	int EncodeMDU();
	void UpdateRefFrame();
  protected: 
	
	int m_neblk_;		/* # of encoded macroblocks */
	int MType;
	int LastMType;
	u_int UseQuant;
	int gHdrMVDV;
	int gHdrMVDH;
	int HdrMVDV;
	int HdrMVDH;
	int LastMVDV;
	int LastMVDH;
	int MVDH;
	int MVDV;
	int CBP;
	int MBA;
	int LastMBA;
	EHUFF *MBAEHuff ;
	EHUFF *MVDEHuff ;
	EHUFF *CBPEHuff ;
	EHUFF *T3EHuff;

	MBParam     m_MBParam;
	FRAMEINFO*  m_pCurFrameInfo;
	FRAMEINFO*  m_pRefFrameInfo;
	short inputbuf[6][64];
	short outputbuf[6][64];
  protected: 
	u_int			m_nTemporalRef;
	BOOL			m_bIntra;
	u_char gData[4 * H261_RTP_MTU];

#ifdef OUT_REC_IMAGE
	FILE*			m_pRecFile;
#endif



};

#endif // !defined(AFX_H261FULLENCODER_H__C07F88E2_FD2B_4C13_BC60_F59531D19AD2__INCLUDED_)
