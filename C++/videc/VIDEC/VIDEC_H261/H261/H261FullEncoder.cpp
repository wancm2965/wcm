// H261FullEncoder.cpp: implementation of the H261FullEncoder class.
//
//////////////////////////////////////////////////////////////////////


#include "H261FullEncoder.h"


#include <assert.h>
#include "dct.h"
#include "..\dct\fdct.h"
#include "..\dct\idct.h"
#include "..\utils\timer.h"
#include "..\quant\quant_h263.h"
#include "..\utils\emms.h"
#include "..\utils\mem_transfer.h"
#include "..\bitstream\vlc_ctables.h"
#define  PRECISE_IDCT
#define  NORMAL_VLC
//#define  V12_VLC
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define VERSION_NUMBER "3.0.3.2"

H261FullEncoder::H261FullEncoder(Transmitter *T): H261Encoder(T),m_pCurFrameInfo(NULL),m_pRefFrameInfo(NULL)
{
	gNbytes = 0;  //number of bytes in previous packet
	gDbase = 0; //offset from gData where valid data starts
	nbb_ = 0; //# of valid bits in bb_
	bb_ = 0; //intermediate working space
	bc_ = gData; //where to put encoded bits
	sbit_ = 0;
	
	
	quant_required_ = 1;
	m_nTemporalRef=0;
	m_bIntra=TRUE;
	memset(&m_MBParam,0,sizeof(MBParam));
	InitParam();
	InitEnc();
}

H261FullEncoder::~H261FullEncoder()
{
	end_timer();
	if(m_pCurFrameInfo)
	{
		ImageDestory(m_pCurFrameInfo);
		m_pCurFrameInfo=NULL;
	}
	if(m_pRefFrameInfo)
	{
		ImageDestory(m_pRefFrameInfo);
		m_pRefFrameInfo=NULL;
	}
	UnInitEnc();
}

void
H261FullEncoder::SetSize(int w, int h)
{
	if(width_!=w){
		Encoder::SetSize(w, h);
		if (w == CIF_WIDTH && h == CIF_HEIGHT) {
			/* CIF */
			cif_ = 1;
			ngob_ = 12;
			bstride_ = 11;
			lstride_ = 16 * CIF_WIDTH - CIF_WIDTH / 2;
			cstride_ = 8 * 176 - 176 / 2;
			loffsize_ = 16;
			coffsize_ = 8;
			bloffsize_ = 1;
		} else if (w == QCIF_WIDTH && h == QCIF_HEIGHT) {
			/* QCIF */
			cif_ = 0;
			ngob_ = 6; /* not really number of GOBs, just loop limit */
			bstride_ = 0;
			lstride_ = 16 * QCIF_WIDTH - QCIF_WIDTH;
			cstride_ = 8 * 88 - 88;
			loffsize_ = 16;
			coffsize_ = 8;
			bloffsize_ = 1;
		} else {
			//cerr << "H261PixelEncoder: H.261 bad geometry: " << w << 'x' << h << endl;
			return;
		}
		u_int loff = 0;
		u_int coff = 0;
		u_int blkno = 0;
		for (u_int gob = 0; gob < ngob_; gob += 2) {
			loff_[gob] = loff;
			coff_[gob] = coff;
			blkno_[gob] = blkno;
			/* width of a GOB (these aren't ref'd in QCIF case) */
			loff_[gob + 1] = loff + 11 * 16;
			coff_[gob + 1] = coff + 11 * 8;
			blkno_[gob + 1] = blkno + 11;
			
			/* advance to next GOB row */
			loff += (16 * 16 * MBPERGOB) << cif_;
			coff += (8 * 8 * MBPERGOB) << cif_;
			blkno += MBPERGOB << cif_;
		}
		
		
		m_MBParam.height=h;
		m_MBParam.width=w;
		m_MBParam.stride=w;
		m_MBParam.mb_width=(m_MBParam.width + 15)/ 16;
		m_MBParam.mb_height=(m_MBParam.height+15)/16;
		if(m_pCurFrameInfo)
		{
			ImageDestory(m_pCurFrameInfo);
			m_pCurFrameInfo=NULL;
		}
		m_pCurFrameInfo=ImageCreate(m_MBParam);
		
		if(m_pRefFrameInfo)
		{
			ImageDestory(m_pRefFrameInfo);
			m_pRefFrameInfo=NULL;
		}
		m_pRefFrameInfo=ImageCreate(m_MBParam);
		
	}//if(width!=w)
}

int H261FullEncoder::consume(const VideoFrame *vf)
{
	return -1;
	//暂时不实现
	if (!SameSize(vf))
		SetSize(vf->width, vf->height);
	return(encode(vf, vf->crvec));
}

/*
* H261FullEncoder::encode_mb
*	encode a macroblock given a set of input YUV pixels
*/
void
H261FullEncoder::encode_mb(u_int mba, // address of macroblock to encode
						   const u_char* frm, // address of YUV pixels
                           u_int loff, // Luminance offset from frm
                           u_int coff, // Chrominance offset from frm
                           int how)
{
	register int q;
	float* qt;
	if (how == CR_MOTION) {
		q = lq_;
		qt = lqt_;
	} else if (how == CR_BG) {
		q = hq_;
		qt = hqt_; 
	} else {
		/* must be at age threshold */
		q = mq_;
		qt = mqt_; 
	}
	
	/*
	* encode all 6 blocks of the macro block to find the largest
	* coef (so we can pick a new quantizer if gquant doesn't have
	* enough range).
	*/
	/*XXX this can be u_char instead of short but need smarts in fdct */
	short blk[64 * 6];
	register int stride = width_;
	/* luminance */
	const u_char* p = &frm[loff];
	fdct(p, stride, blk + 0, qt);
	fdct(p + 8, stride, blk + 64, qt);
	fdct(p + 8 * stride, stride, blk + 128, qt);
	fdct(p + (8 * stride + 8), stride, blk + 192, qt);
	/* chominance */
	int fs = framesize_;
	p = &frm[fs + coff];
	stride >>= 1;
	fdct(p, stride, blk + 256, qt);
	fdct(p + (fs >> 2), stride, blk + 320, qt);
	
	/*
	* if the default quantizer is too small to handle the coef.
	* dynamic range, spin through the blocks and see if any
	* coef. would significantly overflow.
	*/
	if (q < 8) {
		register int cmin = 0, cmax = 0;
		register short* bp = blk;
		for (register int i = 6; --i >= 0; ) {
			++bp;	// ignore dc coef
			for (register int j = 63; --j >= 0; ) {
				register int v = *bp++;
				if (v < cmin)
					cmin = v;
				else if (v > cmax)
					cmax = v;
			}
		}
		if (cmax < -cmin)
			cmax = -cmin;
		if (cmax >= 128) {
			/* need to re-quantize */
			register int s;
			for (s = 1; cmax >= (128 << s); ++s) {
			}
			q <<= s;
			register short* bp = blk;
			for (register int i = 6; --i >= 0; ) {
				++bp;	// ignore dc coef
				for (register int j = 63; --j >= 0; ) {
					register int v = *bp;
					*bp++ = v >> s;
				}
			}
		}
	}
	
	u_int m = mba - mba_; // set macroblock address difference 
	mba_ = mba; // set last absolute macroblock address transmitted
	huffent* he = &hte_mba[m - 1]; // Huffman coded macroblock address
	/* MBA */
	PUT_BITS(he->val, he->nb, nbb_, bb_, bc_);
	if (q != mquant_) {
		/* MTYPE = INTRA + TC + MQUANT */
		PUT_BITS(1, 7, nbb_, bb_, bc_);
		PUT_BITS(q, 5, nbb_, bb_, bc_);
		mquant_ = q;
	} else {
		/* MTYPE = INTRA + TC (no quantizer) */
		PUT_BITS(1, 4, nbb_, bb_, bc_);
	}
	
	/* luminance */
	const char* lm = llm_[q];
	if (lm == 0) {
		lm = make_level_map(q, 1);
		llm_[q] = lm;
		clm_[q] = make_level_map(q, 2);
	}
	encode_blk(blk + 0, lm);
	encode_blk(blk + 64, lm);
	encode_blk(blk + 128, lm);
	encode_blk(blk + 192, lm);
	/* chominance */
	lm = clm_[q];
	encode_blk(blk + 256, lm);
	encode_blk(blk + 320, lm);
}



FRAMEINFO* H261FullEncoder::ImageCreate(MBParam mbParam)
{
	FRAMEINFO* frame=(FRAMEINFO*)malloc(sizeof(FRAMEINFO));
	memset(frame,0,sizeof(FRAMEINFO));
	frame->image.y=(u_char*)malloc(mbParam.width*mbParam.height);
	frame->image.u=(u_char*)malloc(mbParam.width*mbParam.height/4);
	frame->image.v=(u_char*)malloc(mbParam.width*mbParam.height/4);
	frame->mbs=(MACROBLOCK *)malloc(m_MBParam.mb_width*m_MBParam.mb_height*sizeof(MACROBLOCK));
	memset(frame->mbs,0,m_MBParam.mb_width*m_MBParam.mb_height*sizeof(MACROBLOCK));

	return frame;
}

void H261FullEncoder::ImageDestory(FRAMEINFO* frame)
{
	if(NULL==frame)return;
	
	if(frame->image.y)
	{
		free(frame->image.y);
		frame->image.y=NULL;
	}
	if(frame->image.u)
	{
		free(frame->image.u);
		frame->image.u=NULL;
	}
	if(frame->image.v)
	{
		free(frame->image.v);
		frame->image.v=NULL;
	}
	if(frame->mbs)
	{
		free(frame->mbs);
		frame->mbs=NULL;
	}
	free(frame);
	
}


void H261FullEncoder::InitEnc()
{
	MBAEHuff = MakeEhuff(40);
	MVDEHuff = MakeEhuff(40);
	CBPEHuff = MakeEhuff(70);
	T3EHuff = MakeEhuff(20);
	
	LoadETable(MBACoeff,MBAEHuff);
	
	LoadETable(MVDCoeff,MVDEHuff);
	
	LoadETable(CBPCoeff,CBPEHuff);
	
	LoadETable(MTypeCoeff,T3EHuff);
	
	int cpu_flags=0;
	cpu_flags = check_cpu_features_h261();
	
#if defined(EXPERIMENTAL_SSE2_CODE)
	if ((cpu_flags & XVID_CPU_SSE) && sigill_check_h261(sse_os_trigger_h261))
		cpu_flags &= ~XVID_CPU_SSE;
	
	if ((cpu_flags & XVID_CPU_SSE2) && sigill_check_h261(sse2_os_trigger_h261))
		cpu_flags &= ~XVID_CPU_SSE2;
#endif
	
	idct_h261_int32_init();
	/* Fixed Point Forward/Inverse DCT transformations */
	xvid_fdct = fdct_h261_int32;
#ifndef PRECISE_IDCT
	xvid_idct = idct_h261_int32;
#else
	xvid_idct=simple_idct_h261_c;
#endif
	/* Only needed on PPC Altivec archs */
	sadInit = 0;
	
	/* Restore FPU context : emms_c is a nop functions */
	emms = emms_h261_c;
	
	/* Quantization functions */
	quant_intra   = quant_intra_h261_c;
	prequant_intra = prequant_intra_h261_c;
	dequant_intra = dequant_intra_h261_c;
	quant_inter   = quant_inter_h261_c;
	prequant_inter = prequant_inter_h261_c;
	dequant_inter = dequant_inter_h261_c;
	
	transfer_8to16copy=transfer_8to16copy_h261_c;
	transfer_16to8copy=transfer_16to8copy_h261_c;
	transfer_8to16sub=transfer_8to16sub_h261_c;
	transfer_8to16sub_ext=transfer_8to16sub_ext_h261_c;
	transfer_16to8add=transfer_16to8add_h261_c;
	transfer8x8_copy=transfer8x8_copy_h261_c;
	
	sad16=sad16_h261_c;
	dev16=dev16_h261_c;	
	if ((cpu_flags & XVID_CPU_MMX) > 0) {
		
		/* Forward and Inverse Discrete Cosine Transformation functions */
		xvid_fdct = fdct_h261_mmx;
	#ifndef PRECISE_IDCT
		xvid_idct = idct_h261_mmx;
	#else
		xvid_idct  =simple_idct_h261_mmx;
	#endif
		/* To restore FPU context after mmx use */
		emms = emms_h261_mmx;
		
		/* Quantization related functions */
		quant_intra   = quant_intra_h261_mmx;
		dequant_intra = dequant_intra_h261_mmx;
		quant_inter   = quant_inter_h261_mmx;
		dequant_inter = dequant_inter_h261_mmx;
		
		transfer_8to16copy=transfer_8to16copy_h261_mmx;
		transfer_16to8copy=transfer_16to8copy_h261_mmx;
		transfer_8to16sub=transfer_8to16sub_h261_mmx;
		transfer_8to16sub_ext=transfer_8to16sub_ext_h261_mmx;
		transfer_16to8add=transfer_16to8add_h261_mmx;
		transfer8x8_copy=transfer8x8_copy_h261_mmx;
		sad16    = sad16_h261_mmx;
		dev16    = dev16_h261_mmx;
	}
	if ((cpu_flags & XVID_CPU_MMXEXT) > 0) {
		
		/* Inverse DCT */
	#ifndef PRECISE_IDCT
		xvid_idct = idct_h261_xmm;
	#endif
		/* Quantization */
		dequant_intra = dequant_intra_h261_xmm;
		dequant_inter = dequant_inter_h261_xmm;
		
		/* Colorspace transformation */
		/* ME functions */
		sad16 = sad16_h261_xmm;
		dev16 = dev16_h261_xmm;
		
	}
	
	if ((cpu_flags & XVID_CPU_SSE2) > 0) {
#ifdef EXPERIMENTAL_SSE2_CODE		
		/* Quantization */
		quant_intra   = quant_intra_h261_sse2;
		dequant_intra = dequant_intra_h261_sse2;
		quant_inter   = quant_inter_h261_sse2;
		dequant_inter = dequant_inter_h261_sse2;
		
		/* ME */
		sad16    = sad16_h261_sse2;
		dev16    = dev16_h261_sse2;
		
		/* Forward and Inverse DCT */
	#ifndef PRECISE_IDCT
		xvid_idct  = idct_h261_sse2;
	#endif
		xvid_fdct = fdct_h261_sse2;
#endif
	}

}

void H261FullEncoder::UnInitEnc()
{
	UnMakeEhuff(MBAEHuff);
	UnMakeEhuff(MVDEHuff);
	UnMakeEhuff(CBPEHuff); 
	UnMakeEhuff(T3EHuff); 
	
	MBAEHuff =NULL;
	MVDEHuff =NULL;
	CBPEHuff =NULL;
	T3EHuff = NULL;
}

void H261FullEncoder::InitParam()
{
	LastMVDV=0;
	LastMVDH=0;
	CBP=0x3f;
	gHdrMVDH=0;
	gHdrMVDV=0;
	LastMVDH=0;
	LastMVDV=0;
    MVDH=0;
    MVDV=0;
	MBA=0;
	LastMBA=0;
	m_neblk_=0;
}


int H261FullEncoder::PreIncEncodeSetup(const VideoFrame *vf)
{
	gVf = vf;
	if (!SameSize(vf))
		SetSize(vf->width, vf->height);
	
	gPicture = TRUE; //if TRUE, send picture layer header
/*
	gNbytes = 0;  //number of bytes in previous packet
	gDbase = 0; //offset from gData where valid data starts
	nbb_ = 0; //# of valid bits in bb_
	bb_ = 0; //intermediate working space
	bc_ = gData; //where to put encoded bits
	sbit_ = 0;
*/
	gStep = cif_ ? 1 : 2; //Macro Block step size
	gGobMax = cif_ ? 12 : 5; //how many GOB per frame
	
	gSendGOBhdr = TRUE; //must send GOB hdr before sending MB
	gGOBhdrNxt = TRUE; //will start out with GOB header
	//because gGOBhdrNxt == TRUE, no need to initialize the following 2 header variables:
	//gHdrGOBN; // next GOB number for last encoded MB
	//gHdrMBAP; // address of last macroblock encoded in previous packet 1..32
	//mba_= 0; // should not be necessary to initialize this
	gHdrQUANT = lq_; // QUANT in effect for next MB to be encoded
	gNxtMBA = 1; // address of next macroblock to be considered for encoding 1..33
	gNxtGOB = 1; // start encoding at GOB 1
	gDone = FALSE;
	
	mquant_=lq_;
	InitParam();	
	
	memcpy(m_pCurFrameInfo->image.y,gVf->frameptr,framesize_);
	memcpy(m_pCurFrameInfo->image.u,gVf->frameptr+framesize_,framesize_/4);
	memcpy(m_pCurFrameInfo->image.v,gVf->frameptr+framesize_*5/4,framesize_/4);
	init_timer();
	start_global_timer();
	
	start_timer();
	if(!m_bIntra)//对P帧进行运动估计
	{
		
		m_pCurFrameInfo->quant=m_pRefFrameInfo->quant=gHdrQUANT;
		MotionEstimation_h261(&m_MBParam,m_pCurFrameInfo,m_pRefFrameInfo);	
	}
	stop_motion_timer();
	return 1;
} 

int H261FullEncoder::ReadCompressMDU()
{
	
	int c,j,h,v;
	short *input,*output;
	int pmask,acsum;
	int hpos,vpos;
	pmask=0;
	int CBPThreshold=0; 
	int nResult=1;
	u_char* cur;
	u_char* ref;
	int curStride;
	int refStride;
	u_int curOff;
	u_int refOff;
	// READ AND COMPRESS 
	if (QuantMType[MType])
	{
		UseQuant=mquant_;
		gHdrQUANT=mquant_; // Future MB Quant is now MQuant 
	}
	else UseQuant=mquant_;
	for(c=0;c<6;c++)
	{
		input = &inputbuf[c][0];
		output=&outputbuf[c][0];
		j = BlockJ[c];
		v = BlockV[c];
		h = BlockH[c];
		
		
		
		if(0==j)
		{
			cur=m_pCurFrameInfo->image.y;
			curStride=width_;
			ref=m_pRefFrameInfo->image.y;
			refStride=width_;
		}
		else	
		{
			curStride=width_>>1;
			refStride=width_>>1;
			if(1==j)
			{
				cur=m_pCurFrameInfo->image.u;
				ref=m_pRefFrameInfo->image.u;
			}
			else 
			{
				cur=m_pCurFrameInfo->image.v;
				ref=m_pRefFrameInfo->image.v;
			}
		}
		start_timer();
		GetPos(cif_,gNxtGOB-1,gNxtMBA-1,h,v,j,&hpos,&vpos);
		curOff=(vpos)*BLOCKHEIGHT*curStride+ (hpos)*BLOCKWIDTH;
		
		if(IntraMType[MType])
			ReadBlock(input,cur+curOff,curStride);
		else
		{	
			if (j)
				refOff=(((vpos *  BLOCKHEIGHT) + MVDV/2)*refStride)+ (hpos * BLOCKWIDTH) + MVDH/2;
			else
				refOff=(((vpos *  BLOCKHEIGHT) + MVDV)*refStride)+ (hpos * BLOCKWIDTH) + MVDH;
			if (TCoeffMType[MType])
			{
				if (FilterMType[MType])
					SubFCompensate(input,cur+curOff,ref+refOff,refStride);
				else
					SubCompensate(input,cur+curOff,ref+refOff,refStride);
			}
			else
			{
				transfer8x8_copy(cur+curOff,ref+refOff,refStride);
				memset(input,0,64*sizeof(short));
			}
			
		}
		stop_prediction_timer();
		
		start_timer();
		xvid_fdct(input);
		BoundDctMatrix(input);
		stop_dct_timer();
	}

	if ((UseQuant<8)&&ReQuantMType[MType])//requant
	{
		start_timer();
		if (IntraMType[MType])
		{
			prequant_intra(inputbuf,&UseQuant);
			if(UseQuant!=mquant_)
			{
				mquant_ = UseQuant;
				MType=1;
			}
		}
		else 
		{
			prequant_inter(inputbuf,&UseQuant);
			if(UseQuant!=mquant_)
			{
				mquant_ = UseQuant;
				if (!QuantMType[MType]) //MType should be 2,5,8
						MType++;
			}
		}
		stop_requant_timer();
	}

	for(c=0;c<6;c++)
	{
		input = &inputbuf[c][0];
		output=&outputbuf[c][0];
		start_timer();
		if (IntraMType[MType])
		{
			quant_intra(output,input,UseQuant,8);
		}
		else
		{
			acsum=quant_inter(output,input,UseQuant);
			if(acsum>CBPThreshold)
				pmask|=bit_set_mask[5-c];
		}
		stop_quant_timer();
	}
	if (!CBPMType[MType]) 
		CBP = 0x3f;  
	else
	{
		if (pmask) CBP=pmask;
		else
		{
			if(2==MType)
				nResult=0;
			if (!FilterMType[MType])
			{
				CBP=0x3f;
				MType=4;
			}
			else 
			{
				CBP=0x3f;
				MType=7;
			}
			
		}
	}
	return nResult;
}


void H261FullEncoder::WriteMBHeader()
{
	int TempH,TempV;
	int val;
	EHUFF *huff ;
	
	huff=MBAEHuff;
	val=MBA;
	/* MBA */
	PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
	
	huff=T3EHuff;
	val=MType;
	PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
	
	if (QuantMType[MType])
	{
		PUT_BITS(UseQuant,5, nbb_, bb_, bc_);
	}

	if (MFMType[MType])
    {
		assert(MVDH>-16&&MVDH<16);
		assert(MVDV>-16&&MVDV<16);
		huff=MVDEHuff;
		if ((!MFMType[LastMType])||(MBA!=1)||
			(LastMBA==-1)||(LastMBA==10)||(LastMBA==21))
		{
			val=MVDH&0x1f;
			PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
			val=MVDV&0x1f;
			PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
			
		}
		else
		{
			TempH = MVDH - LastMVDH;
			if (TempH < -16) 
				TempH += 32;
			if (TempH > 15) 
				TempH -= 32;
			TempV = MVDV - LastMVDV;
			if (TempV < -16) 
				TempV += 32;
			if (TempV > 15) 
				TempV -= 32;
			assert(TempH>-17&&TempH<16);
			assert(TempV>-17&&TempV<16);
			val=TempH&0x1f;
			PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
			val=TempV&0x1f;
			PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
			HdrMVDV=LastMVDV;
			HdrMVDH=LastMVDH;
		}
		LastMVDV = MVDV;
		LastMVDH = MVDH;
    }
	else
    {
		LastMVDV=LastMVDH=MVDV=MVDH=0; // Redundant in most cases 
    }
	if (CBPMType[MType])
    {
		huff=CBPEHuff;
		val=CBP;
		PUT_BITS(huff->Hcode[val],huff->Hlen[val], nbb_, bb_, bc_);
    }
	
}

void H261FullEncoder::CBPEncodeAC(int index, short *matrix)
{
	
	short k,r,l,ovfl;
	BB_INT bb = bb_;
	u_int nbb = nbb_;
	u_char* bc = bc_;
	for(ovfl=1,r=0,k=index-1;++k<BLOCKSIZE;)
    {
		l = matrix[scan_tables[k]];//&0xff;
		assert(l>-128&&l<128);
		if (!l) {r++;}
		else
		{
			int val, nb;
			huffent* he; 
			if (u_int(l + 15) <= 30 &&
				(nb = (he = &hte_tc[((l&0x1f) << 6)|r])->nb))
			{
				/* we can use a VLC. */
				val = he->val;
				if(3==nb)
				{
					val=val&0x3;
					nb=2;
				}
				
			}
			else {
				/* Can't use a VLC.  Escape it. */
				val = (1 << 14) | (r << 8) | (l & 0xff);
				nb = 20;
			}
			PUT_BITS(val, nb, nbb, bb, bc);
			r = 0;
			ovfl=0;
			break;
		}
    }
	if (ovfl)
    {
		//	printf("CBP block without any coefficients.\n");
		assert(0);
		return;
    }
	
	for(r=0;++k<BLOCKSIZE;)
    {
		l = matrix[scan_tables[k]];//&0xff;
		assert(l>-128&&l<128);
		if (!l) {r++;}
		else
		{	
			huffent* he; 
			int val, nb;
			if (u_int(l + 15) <= 30 &&
				(nb = (he = &hte_tc[((l&0x1f) << 6)|r])->nb))
				// we can use a VLC. 
				val = he->val;
			else {
				// Can't use a VLC.  Escape it. 
				val = (1 << 14) | (r << 8) | (l & 0xff);
				nb = 20;
			}
			PUT_BITS(val, nb, nbb, bb, bc);
			r = 0;
		}
    }
	PUT_BITS(2, 2, nbb, bb, bc);
	bb_ = bb;
	nbb_ = nbb;
	bc_ = bc;
}

void H261FullEncoder::EncodeDC(int coef)
{
	if (coef > 254) {coef=254;}
	if (coef < 1) {coef=1;}
	if (coef==128) {coef=255;}
	PUT_BITS(coef,8, nbb_, bb_, bc_);
	
}

void H261FullEncoder::EncodeAC(int index, short *matrix)
{
    int lm_incs=0;
	BB_INT bb = bb_;
	u_int nbb = nbb_;
	u_char* bc = bc_;
	
	short k,r,l;
	for(r=0,k=index-1;++k<BLOCKSIZE;)
    {
		l = matrix[scan_tables[k]];//& 0xff;
		assert(l>-128&&l<128);
		if (!l) {r++;}
		else
		{
			huffent* he; 
			int val, nb;
			if (u_int(l + 15) <= 30 &&
				(nb = (he = &hte_tc[((l&0x1f) << 6)|r])->nb))
				// we can use a VLC. 
				val = he->val;
			else {
				// Can't use a VLC.  Escape it. 
				val = (1 << 14) | (r << 8) | (l & 0xff);
				nb = 20;
			}
			PUT_BITS(val, nb, nbb, bb, bc);
			r = 0;
		}
    }
	// EOB 
	PUT_BITS(2, 2, nbb, bb, bc);
	bb_ = bb;
	nbb_ = nbb;
	bc_ = bc;
	
}

int H261FullEncoder::WriteMDU()
{
	
	int c,j;
	short *input,*output;
	
	int q=mquant_;
	
	MBA = (gNxtMBA-1-LastMBA);     // WRITE 
	WriteMBHeader();
	LastMBA = gNxtMBA-1;
	for(c=0;c<6;c++)
    {
		j = BlockJ[c];
		input = &inputbuf[c][0];
		output= &outputbuf[c][0];
		if ((CBP & bit_set_mask[5-c])&&(TCoeffMType[MType]))
		{
			start_timer();
			if (CBPMType[MType])
			{
				CBPEncodeAC(0,output);
			}
			else
			{	
				if (*output > 254) {*output=254;}
				if (*output < 1) {*output=1;}
				EncodeDC(*output);
				EncodeAC(1,output);
				
			}
			stop_coding_timer();
			
			
			start_timer();
			if (IntraMType[MType])
				dequant_intra(input,output,UseQuant,8);
			else
				dequant_inter(input,output,UseQuant);
			stop_iquant_timer();
			
			start_timer();
			xvid_idct(input);
			stop_idct_timer();
		}
		else 
			memset(input,0,64*sizeof(short));
    }
	
	return 0;
}

int H261FullEncoder::DecodeSaveMDU()
{
	
	int c,j,h,v;
	short *input;
    int hpos,vpos;
	u_char* rec;
	int recStride;
	u_int recOff;
	for(c=0;c<6;c++)
    {
		j = BlockJ[c];
		v = BlockV[c];
		h = BlockH[c];
		input = &inputbuf[c][0];
		
		
		if(0==j)
		{
			rec=m_pCurFrameInfo->image.y;
			recStride=width_;
		}
		else	
		{
			recStride=width_>>1;
			if(1==j)
				rec=m_pCurFrameInfo->image.u;
			else 
				rec=m_pCurFrameInfo->image.v;
		}
		GetPos(cif_,gNxtGOB-1,gNxtMBA-1,h,v,j,&hpos,&vpos);
		recOff=(vpos)*BLOCKHEIGHT*recStride+ (hpos)*BLOCKWIDTH;
		
		start_timer();
		if (IntraMType[MType])     // DECODE 
			WriteBlock(input,rec+recOff,recStride);
		else
			AddCompensate(input,rec+recOff,recStride);
		stop_comp_timer();
		
    }
	return 0;
}

int H261FullEncoder::EncodeMDU()
{
	int nResult=1;
	nResult=ReadCompressMDU();
	if(nResult>0)
	{
		WriteMDU();
		DecodeSaveMDU();
		m_neblk_++;	
	}
	return nResult;
}

void H261FullEncoder::IncEncodeAndGetPacket(
											u_char * buffer,    // returns buffer of encoded data
											unsigned & length ) // returns actual length of encoded data buffer
{
	u_int previousBitCount=0;
	u_int currentBitCount=0;
	h261hdr_t h261hdr;
	u_int nPos=0;
	u_int nMDU=0;
	u_int nblk=m_neblk_;
	if (gDone) {
		length = 0;
		return;
	}
	
	// TESTING
	if (!gGOBhdrNxt && ((33 <= gHdrMBAP) || (0 >= gHdrMBAP))) 
	{
		//cerr << __FILE__<< "(" << __LINE__ << ") " <<
		//	"illegal gHdrMBAP value when gGOBhdrNxt is FALSE = " << gHdrMBAP << endl;
		gHdrMBAP = 1;
	}
	//  unsigned t1 = sbit_ << 29;
	//  unsigned t2 = (gGOBhdrNxt?0:gHdrGOBN) << 20;
	//  unsigned t3 = (gGOBhdrNxt||(0==gHdrMBAP)?0:gHdrMBAP-1) << 15;
	//  unsigned t4 = (gGOBhdrNxt?0:gHdrQUANT) << 10;
	
	// set the H.261 header (32 bits) bits that we know now
	
	
	h261hdr = 1 << 24 | // V = 1, I = 0, HMVD = 0, VMVD = 0
		sbit_ << 29 | // SBIT
		(gGOBhdrNxt?0:gHdrGOBN) << 20 | // GOBN
		(gGOBhdrNxt?0:gHdrMBAP-1) << 15 | // MBAP 
		(gGOBhdrNxt?0:gHdrQUANT) << 10|  // QUANT
		(gGOBhdrNxt?0:(gHdrMVDH&0x1f))<<5|
		(gGOBhdrNxt?0:(gHdrMVDV&0x1f));
	// gHdrQUANT << 10;  // QUANT set the old & incorrect way
	
	// if any, move unsent bits encoded during previous packet encode
	unsigned usedBB_INTs;
	u_char* msrc;
	unsigned m1;
	if (sbit_) gNbytes--; // last byte of previous packet has bits needed for this packet
	usedBB_INTs = (gNbytes + gDbase)/sizeof(BB_INT);
	msrc = gData + usedBB_INTs*sizeof(BB_INT);
	gDbase = (gDbase + gNbytes) % sizeof(BB_INT);
	m1 = bc_ - msrc;
	if (m1) memcpy(gData, msrc, m1);
	bc_ = gData + m1; // set starting bc_ address
	
	// encode H.261 stream
	if (gPicture) { // only ever sent at start of 1st packet 
		/* PSC Picture Start Code */
		PUT_BITS(0x0001, 16, nbb_, bb_, bc_);
		/* GOB 0 -> picture header Finishes PSC */
		PUT_BITS(0, 4, nbb_, bb_, bc_);
		/* TR Temporal Reference (XXX should do this right) */
		//modifed by lixq 2004-7-14 (FOR VCON)
		PUT_BITS(m_nTemporalRef, 5, nbb_, bb_, bc_);
		//	PUT_BITS(20, 5, nbb_, bb_, bc_);
		/* PTYPE = CIF */
		//modifed by lixq 2004-7-14 (FOR VCON)
		int pt;
	//	if(m_bIntra)
			pt = cif_ ? 15 : 11;
	//	else
	//		pt = cif_ ? 7 : 3;
		
		PUT_BITS(pt, 6, nbb_, bb_, bc_);
		/* PEI Picture Extra Information = none */
		PUT_BITS(0, 1, nbb_, bb_, bc_);
		gloff = loff_[0];
		gcoff = coff_[0];
		gblkno = blkno_[0];
		gline = 11;
		gPicture = FALSE;
	}
  unsigned bitLimit = 8*(H261_RTP_MTU - sizeof(h261hdr));
  u_char* bbase = gData + gDbase;
  BOOL bNewPacket=FALSE;
  if (!(gNxtGOB > gGobMax)) {
    while ((currentBitCount = nbb_ + 8*(bc_ - bbase)) <= bitLimit) {
      // everything encoded up till now fits within the H261_RTP_MTU buffer
      // test to see if the packet can be broken here
      if (gSendGOBhdr ||(nMDU>64)/*(0 != mba_) */){ // if packet can be broken,
        // record conditions that will be needed to construct the h261 header
        // for the next packet if the packet breaks here
        gHdrGOBN = gNxtGOB; // GOB number in effect for next MB to be encoded
        gHdrMBAP = mba_; // MBA of the last encoded MB & now in the buffer
        gHdrQUANT = mquant_; // QUANT in effect for next MB to be encoded
        gGOBhdrNxt = gSendGOBhdr; // is GOB header next?
        previousBitCount = currentBitCount; // encoded bits now in the buffer
		nblk=m_neblk_;
		bNewPacket=TRUE;
	  }
      if (gSendGOBhdr) { // need to send GOB header before can send MB ?
        /* GSC/GN */
        PUT_BITS(0x10 | gNxtGOB, 20, nbb_, bb_, bc_);
        /* GQUANT/GEI */
        mquant_ = lq_;
        PUT_BITS(mquant_ << 1, 6, nbb_, bb_, bc_);
        mba_= 0;
		nMDU=0;
        gSendGOBhdr = FALSE;
		
		LastMBA = -1;
		MType=0;
      }
      LastMType=MType;	  
	  //根据intra和inter决定当前macroblock的类型
	  if (m_bIntra)   // Intra vs. Inter decision 
		 MType = 0; // We always start with Intramode 
	  else
	  {
		  nPos=Bpos(cif_,gNxtGOB-1,gNxtMBA-1);
		  assert(nPos<=m_MBParam.mb_width*m_MBParam.mb_height);
		  MACROBLOCK* pMB=&(m_pCurFrameInfo->mbs[nPos]);
		  MType=pMB->mode;
		  MVDH=pMB->mv16.x;
		  MVDV=pMB->mv16.y;	 
	  }
	  HdrMVDH=HdrMVDV=0;
	  nMDU=nbb_ + 8*(bc_ - bbase);
	  EncodeMDU();
	  nMDU=nbb_ + 8*(bc_ - bbase)-nMDU;
	  if(bNewPacket)
	  {
		  gHdrMVDV=HdrMVDV;
		  gHdrMVDH=HdrMVDH;
		  bNewPacket=FALSE;
	  } 
	  mba_= gNxtMBA;
	  gNxtMBA++;
      if (gNxtMBA > 33) {
        gNxtGOB += gStep;
        if (gNxtGOB > gGobMax)
		{
          gDone = TRUE;
		  //update frame for P frame coding
		  UpdateRefFrame();
          break; // out of while(), done encoding frame
        }
        gNxtMBA = 1;
        gSendGOBhdr = TRUE; // must send GOB hdr before sending MB
        gloff = loff_[gNxtGOB-1];
        gcoff = coff_[gNxtGOB-1];
        gblkno = blkno_[gNxtGOB-1];
        gline = 11;
      }
      else {
        gloff += loffsize_;
        gcoff += coffsize_;
        gblkno += bloffsize_;
        if (--gline <= 0) {
          gline = 11;
          gblkno += bstride_;
          gloff += lstride_;
          gcoff += cstride_;
        }
      }
    }
  }
  else 
    gDone = TRUE;
  // have full packet now, finish & set up for next packet
  // break packet at end of previous MB unless this is end of frame
  // flush bits from bb_
  STORE_BITS(bb_, bc_); // necessary when gDbase != 0
  if(32==nbb_)
  {	
	  bc_ += sizeof(BB_INT); 
	  bb_ = 0; 
	  nbb_ = 0;
  }
  if (gDone) {
	  unsigned totalBits = nbb_ + 8*(bc_ - bbase);
	  if (totalBits <= bitLimit) { // would packet be too big?
		  previousBitCount = totalBits;
		  nblk=m_neblk_;
	  }
	  else { // need another packet to finish frame
		  gDone = FALSE;
	  }
	  
  }
//  printf("\nencode nblk:%d total:%d\n",nblk, m_neblk_);
  gNbytes = previousBitCount / 8;
  sbit_ = previousBitCount % 8; // SBIT for next packet = good bits for this packet
  if (sbit_) gNbytes++; // include last bits for this frame
  
  // set the H.261 header (32 bits) bits that we know now
  h261hdr |= ((8 - sbit_) & 7) << 26; //EBIT = 8 - SBIT for next packet


  h261hdr = htonl(h261hdr);
  *(h261hdr_t*)buffer = h261hdr;
  memcpy(buffer+sizeof(h261hdr), gData+gDbase, gNbytes);
  length = gNbytes+sizeof(h261hdr);

  if (H261_RTP_MTU < length) 
  { // test for too large packet (I haven't seen one yet)
    //cerr << __FILE__<< "(" << __LINE__ << ") " <<
    //  "packet size of " << length << " > H261_RTP_MTU(" << H261_RTP_MTU << ")" << endl;
  }

}

void H261FullEncoder::UpdateRefFrame()
{
	//以后帧使用P帧编码方式
	m_bIntra=FALSE;
	m_nTemporalRef=(++m_nTemporalRef)%32;
	//参考帧交换
	FRAMEINFO* p;
	p=m_pRefFrameInfo;
	m_pRefFrameInfo=m_pCurFrameInfo;
	m_pCurFrameInfo=p;
	emms();
	stop_global_timer();
	write_timer();
	
#ifdef OUT_REC_YUV		
	fwrite(m_pRefFrameInfo->image.y,1,framesize_,m_pRecFile);
	fwrite(m_pRefFrameInfo->image.u,1,framesize_/4,m_pRecFile);
	fwrite(m_pRefFrameInfo->image.v,1,framesize_/4,m_pRecFile);
#endif
	
}

void H261FullEncoder::SkipCurrentFrame()
{
	m_nTemporalRef=(++m_nTemporalRef)%32;
}
