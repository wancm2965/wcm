/////////////////////////////////////////////////////////////////////////////////
//

#ifndef __PLC_H__
#define __PLC_H__

class PLC
{
public:
	PLC();
	virtual~PLC();
public: 
	int Open(int SampleRate, int nSamplesPerFrame);  // 1 is ok , else is failed
	void Close();

	int Save(short *pSamples, short * pTmpBuf);
	int Recover(short *pSamples, short * pTmpBuf);
protected:
	int erasecnt;					/* consecutive erased frames */
	int poverlap;					/* overlap based on pitch */
	int poffset;					/* offset into pitch period */
	int pitch;			/* pitch estimate */
	int pitchblen;		/* current pitch buffer length */

	float *pitchbufend;	/* end of pitch buffer */
	float *pitchbufstart; /* start of pitch buffer */
	float *pitchbuf;		/* buffer for cycles of speech */
	float *lastq;			/* saved last quarter wavelength */

	short *history;		/* history buffer */

	double		m_factor_in;	/* Conversion factor = rate_out / rate_in.  */
	unsigned		m_xoff_in;		/* History and lookahead size, in samples   */
	// short*		m_buffer_in;	/* Input buffer.			    */

	double		m_factor_out;	
	unsigned		m_xoff_out;	
	short*		m_buffer_out;
	int			m_nSamplesPerFrame;
	int			m_nSampleRate;
	int			m_nPos;
	int			m_nFrameSize;
	// short*		m_SmallBuf;

	void GetFrame(short *s);	/* synthesize speech for erasure */
	void SaveFrame(short *s);	/* add a good frame to history buf */

	void scalespeech(short *out);
	void getfespeech(short *out, int sz);
	void savespeech(short *s);
	int findpitch();

	void overlapadd(float *l, float *r, float *o, int cnt);
	void overlapadd(short *l, short *r, short *o, int cnt);
	void overlapaddatend(short *s, short *f, int cnt);
	void convertsf(short *f, float *t, int cnt);
	void convertfs(float *f, short *t, int cnt);
	void copyf(float *f, float *t, int cnt);
	void copys(short *f, short *t, int cnt);

	int  ConvertDown(int quality, short*pInSamples,short*pOutSamples,int&nSamplesPerFrame);
	int  ConvertUp(int quality, short*pInSamples,short*pOutSamples, int&nSamplesPerFrame);
	short WordToHword(int v, int scl);
	int SrcLinear(const short X[], short Y[], double pFactor, unsigned short nx);
	int FilterUp(const short Imp[], const short ImpD[], unsigned short Nwing, char Interp,
		const short *Xp, short Ph, short Inc);
	int SrcUp(const short X[], short Y[], double pFactor, unsigned short nx, unsigned short pNwing, unsigned short pLpScl,
		const short pImp[], const short pImpD[], char Interp);
};


#endif
