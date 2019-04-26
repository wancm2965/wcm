////////////////////////////////////////////////////////////////////////////////////////
// PLC.cpp

#include "plc.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	

#define MAX(x,y) ((x)>(y) ?(x):(y))
#define MIN(x,y) ((x)<(y) ?(x):(y))

/* Conversion constants */
#define Nhc       8
#define Na        7
#define Np       (Nhc+Na)
#define Npc      (1<<Nhc)
#define Amask    ((1<<Na)-1)
#define Pmask    ((1<<Np)-1)
#define Nh       16
#define Nb       16
#define Nhxn     14
#define Nhg      (Nh-Nhxn)
#define NLpScl   13

#   include "smallfilter.h"

#define PITCH_MIN     40							/* minimum allowed pitch, 200 Hz */
#define PITCH_MAX     120							/* maximum allowed pitch, 66 Hz */
#define PITCHDIFF     (PITCH_MAX - PITCH_MIN)
#define POVERLAPMAX   (PITCH_MAX >> 2)              /* maximum pitch OLA window */
#define HISTORYLEN    (PITCH_MAX * 3 + POVERLAPMAX) /* history buff length*/
#define NDEC          2                             /* 2:1 decimation */
#define CORRLEN       160                           /* 20 msec correlation length */
#define CORRBUFLEN    (CORRLEN + PITCH_MAX)         /* correlation buffer length */
#define CORRMINPOWER  ((float)250.0)                 /* minimum power */
#define EOVERLAPINCR  32							/* end OLA increment per frame,4 ms */
#define FRAMESZ       80                            /* 10 msec at 8 KHz */
#define ATTENFAC      ((float)0.2)                   /* attenu. factor per 10 ms frame */
#define ATTENINCR     (ATTENFAC/FRAMESZ)            /* attenuation per sample */


short PLC::WordToHword(int v, int scl)
{
    short out;
	short tmp = 0;
    int llsb = (1<<(scl-1));
    v += llsb;		/* round */
    v >>= scl;
	
	// Ryan liu add, to up 1-2dB volume
	tmp = (v>>3); tmp = tmp - (tmp>>1); v = v + tmp;
	
    if (v>32767) 
	{
		v = 32767;
    } 
	else if (v < -32768) 
	{
		v = -32768;
    }	
    out = (short) v;
    return out;
}

/* Sampling rate conversion using linear interpolation for maximum speed.
*/
int PLC::SrcLinear(const short X[], short Y[], double pFactor, unsigned short nx)
{
    short iconst;
    unsigned int time = 0;
    const short *xp;
    short *Ystart, *Yend;
    int v,x1,x2;
    
    double dt;						/* Step through input signal */ 
    unsigned int dtb;                  /* Fixed-point version of Dt */
    unsigned int endTime;              /* When time reaches EndTime, return to user */
    
    dt = 1.0/pFactor;				/* Output sampling period */
    dtb = (unsigned int)(dt*(1<<Np) + 0.5);			/* Fixed-point representation */
    
    Ystart = Y;
    Yend = Ystart + (unsigned)(nx * pFactor);
    endTime = time + (1<<Np)*(int)nx;
    while (time < endTime)
    {
		iconst = (time) & Pmask;
		xp = &X[(time)>>Np];			/* Ptr to current input sample */
		x1 = *xp++;
		x2 = *xp;
		x1 *= ((1<<Np)-iconst);
		x2 *= iconst;
		v = x1 + x2;
		*Y++ = WordToHword(v,Np);		/* Deposit output */
		time += dtb;					/* Move to next sample by time increment */
    }
    return (Y - Ystart);            /* Return number of output samples */
}

int PLC::FilterUp(const short Imp[], const short ImpD[], 
					   unsigned short Nwing, char Interp,
					   const short *Xp, short Ph, short Inc)
{
    const short *Hp;
    const short *Hdp = NULL;
    const short *End;
    short a = 0;
    int v, t;
    
    v=0;
    Hp = &Imp[Ph>>Na];
    End = &Imp[Nwing];
    if (Interp) 
	{
		Hdp = &ImpD[Ph>>Na];
		a = Ph & Amask;
    }
    if (Inc == 1)		/* If doing right wing...              */
    {				/* ...drop extra coeff, so when Ph is  */
		End--;			/*    0.5, we don't do too many mult's */
		if (Ph == 0)		/* If the phase is zero...           */
		{			/* ...then we've already skipped the */
			Hp += Npc;		/*    first sample, so we must also  */
			Hdp += Npc;		/*    skip ahead in Imp[] and ImpD[] */
		}
    }
    if (Interp)
	{
		while (Hp < End) 
		{
			t = *Hp;		/* Get filter coeff */
			t += (((int)*Hdp)*a)>>Na; /* t is now interp'd filter coeff */
			Hdp += Npc;		/* Filter coeff differences step */
			t *= *Xp;		/* Mult coeff by input sample */
			if (t & (1<<(Nhxn-1)))  /* Round, if needed */
				t += (1<<(Nhxn-1));
			t >>= Nhxn;		/* Leave some guard bits, but come back some */
			v += t;			/* The filter output */
			Hp += Npc;		/* Filter coeff step */
			
			Xp += Inc;		/* Input signal step. NO CHECK ON BOUNDS */
		} 
	}
	else 
	{
		while (Hp < End) 
		{
			t = *Hp;		/* Get filter coeff */
			t *= *Xp;		/* Mult coeff by input sample */
			if (t & (1<<(Nhxn-1)))  /* Round, if needed */
				t += (1<<(Nhxn-1));
			t >>= Nhxn;		/* Leave some guard bits, but come back some */
			v += t;			/* The filter output */
			Hp += Npc;		/* Filter coeff step */
			Xp += Inc;		/* Input signal step. NO CHECK ON BOUNDS */
		}
	}
	return(v);
}

/* Sampling rate up-conversion only subroutine;
* Slightly faster than down-conversion;
*/
int PLC::SrcUp(const short X[], short Y[], double pFactor, 
					unsigned short nx, unsigned short pNwing, unsigned short pLpScl,
					const short pImp[], const short pImpD[], char Interp)
{
    const short *xp;
    short *Ystart, *Yend;
    int v;
    
    double dt;                  /* Step through input signal */ 
    unsigned int dtb;                  /* Fixed-point version of Dt */
    unsigned int time = 0;
    unsigned int endTime;              /* When time reaches EndTime, return to user */
    
    dt = 1.0/pFactor;            /* Output sampling period */
    dtb = (unsigned int)(dt*(1<<Np) + 0.5);     /* Fixed-point representation */
    
    Ystart = Y;
    Yend = Ystart + (unsigned)(nx * pFactor);
    endTime = time + (1<<Np)*(int)nx;
    while (time < endTime)
    {
		xp = &X[time>>Np];      /* Ptr to current input sample */
		/* Perform left-wing inner product */
		v = 0;
		v = FilterUp(pImp, pImpD, pNwing, Interp, xp, (short)(time&Pmask),-1);
		
		/* Perform right-wing inner product */
		v += FilterUp(pImp, pImpD, pNwing, Interp, xp+1,  (short)((-time)&Pmask),1);
		
		v >>= Nhg;		/* Make guard bits */
		v *= pLpScl;		/* Normalize for unity filter gain */
		*Y++ = WordToHword(v,NLpScl);   /* strip guard bits, deposit output */
		time += dtb;		/* Move to next sample by time increment */
    }
    return (Y - Ystart);        /* Return the number of output samples */
}

void PLC::convertsf(short *f, float *t, int cnt)
{
	for (int i = 0; i < cnt; i++)
		t[i] = (float)f[i];
}

void PLC::convertfs(float *f, short *t, int cnt)
{
	for (int i = 0; i < cnt; i++){
		t[i] = (short)f[i];
	}
}

void PLC::copyf(float *f, float *t, int cnt)
{
	for (int i = 0; i < cnt; i++)
		t[i] = f[i];
}

void PLC::copys(short *f, short *t, int cnt)
{
	for (int i = 0; i < cnt; i++)
		t[i] = f[i];
}

PLC::PLC(): erasecnt(0), pitchbufend(0)
{	
	m_nSampleRate = 8000;
	
	m_factor_in=1.0;
    m_xoff_in=0;		
	
	m_factor_out=1.0;
    m_xoff_out=0;		
    m_buffer_out=NULL;

	m_nFrameSize = 0;
}

PLC::~PLC()
{
	
}


int PLC::Open(int SampleRate, int nSamplesPerFrame)
{
	int outsize = 0;
	if(!( SampleRate == 8000 || SampleRate == 32000 || SampleRate == 16000 ))
		return -1;
	if( nSamplesPerFrame > 960 )  // 30ms 32k
		return -1;

	m_nSampleRate = SampleRate;
	m_nSamplesPerFrame = nSamplesPerFrame;
	m_nFrameSize = (nSamplesPerFrame*8000)/SampleRate;
	
	pitchbuf = (float*)malloc(HISTORYLEN*sizeof(float));//new float[HISTORYLEN];
	lastq = (float*)malloc(POVERLAPMAX*sizeof(float));//new float[POVERLAPMAX];
	history = (short*)malloc(HISTORYLEN*sizeof(short));//new short[HISTORYLEN];
	
	memset(pitchbuf, 0, sizeof(float)*HISTORYLEN);
	memset(lastq, 0, sizeof(float)*POVERLAPMAX);
	memset(history, 0, HISTORYLEN*sizeof(short));
	
	if( SampleRate > 8000 )
	{
		unsigned size;
		
		m_factor_in = 8000 * 1.0 / SampleRate;	
		m_xoff_in = 0;
		
		outsize = (nSamplesPerFrame*8000)/SampleRate;
		m_factor_out = SampleRate * 1.0 / 8000;
		
		m_xoff_out = (SMALL_FILTER_NMULT + 1) / 2.0  *  MAX(1.0, 1.0/m_factor_out);
		
		size = (outsize + 2*m_xoff_out) * sizeof(short);
		m_buffer_out = (short*)malloc(size);
		if (m_buffer_out==NULL)
			return -1;
		memset(m_buffer_out, 0, size);
	}	
	return 0;
}

void PLC::Close()
{
	if( m_buffer_out != NULL )
		free(m_buffer_out);
	
	if(pitchbuf != NULL)
		free(pitchbuf);
	if(lastq != NULL)
		free(lastq);
	if(history != NULL)
		free(history);
	
	m_buffer_out=NULL;
	pitchbuf = NULL;
	lastq = NULL;
	history = NULL;
}

void PLC::savespeech(short *s)
{
	/* make room for new signal */
	copys(&history[FRAMESZ], history, HISTORYLEN - FRAMESZ);
	/* copy in the new frame */
	copys(s, &history[HISTORYLEN - FRAMESZ], FRAMESZ);
	/* copy out the delayed frame */
	copys(&history[HISTORYLEN - FRAMESZ - POVERLAPMAX], s, FRAMESZ);
}

int PLC::ConvertDown(int quality, short*pInSamples,short*pOutSamples,int&nSamplesPerFrame)
{
	nSamplesPerFrame=SrcLinear( pInSamples, pOutSamples, m_factor_in, m_nSamplesPerFrame);
	return 0;
}

int PLC::ConvertUp(int quality, short*pInSamples,short*pOutSamples, int&nSamplesPerFrame)
{
	int frameSize = m_nFrameSize;//160;
    if (quality) 
	{
		short *dst_buf;
		const short *src_buf;
		
		dst_buf = m_buffer_out + m_xoff_out*2;
		memcpy(dst_buf, pInSamples, frameSize<<1);
		
		if (m_factor_out >= 1) 
		{
			nSamplesPerFrame=SrcUp(m_buffer_out + m_xoff_out, pOutSamples,
				m_factor_out, frameSize,
				SMALL_FILTER_NWING, SMALL_FILTER_SCALE,
				SMALL_FILTER_IMP, SMALL_FILTER_IMPD,
				1);
		} 
		
		dst_buf = m_buffer_out;
		src_buf = pInSamples + frameSize - m_xoff_out*2;
		memcpy(dst_buf, src_buf, m_xoff_out * 2 * 2);
    } 
	else 
	{
		nSamplesPerFrame=SrcLinear( pInSamples, pOutSamples, m_factor_out, frameSize);
    }
	return 0;
}

int PLC::Save(short *pSamples, short * pTmpBuf)
{
	int len = 0;
	short * data = pSamples;
	
	if( m_nSampleRate > 8000 )
	{
		ConvertDown(0, pSamples, pTmpBuf, len);
		data = pTmpBuf;
	}
	
	SaveFrame(data);
	if( m_nFrameSize > FRAMESZ )
		SaveFrame(&data[FRAMESZ]);
	if( m_nFrameSize > 2*FRAMESZ)
		SaveFrame(&data[FRAMESZ*2]);
	
	if( m_nSampleRate > 8000 )
		ConvertUp(1, pTmpBuf, pSamples, len);

	return 0;
}

int PLC::Recover(short *pSamples, short * pTmpBuf)
{
	short * data = pSamples;
	int	len = 0;
	
	if( m_nSampleRate > 8000 )
		data = pTmpBuf;
	
	GetFrame(data);
	if( m_nFrameSize > FRAMESZ )
		GetFrame(&data[FRAMESZ]);
	if( m_nFrameSize > 2*FRAMESZ)
		GetFrame(&data[FRAMESZ*2]);
	
	if( m_nSampleRate > 8000 )
		ConvertUp(1, pTmpBuf, pSamples, len);
	
	return 0;
}

void PLC::SaveFrame(short *s)
{
	if (erasecnt) 
	{
		short overlapbuf[FRAMESZ];
		/*
		* longer erasures require longer overlaps
		* to smooth the transition between the synthetic
		* and real signal.
		*/
		int olen = poverlap + (erasecnt - 1) * EOVERLAPINCR;
		if (olen > FRAMESZ)
			olen = FRAMESZ;
		getfespeech(overlapbuf, olen);
		overlapaddatend(s, overlapbuf, olen);
		erasecnt = 0;
	}
	savespeech(s);
}

void PLC::GetFrame(short *out)
{
	pitchbufend = &pitchbuf[HISTORYLEN];
	
	if (erasecnt == 0) 
	{
		convertsf(history, pitchbuf, HISTORYLEN); /* get history */
		pitch = findpitch(); /* find pitch */
		poverlap = pitch >> 2; /* OLA 1/4 wavelength */
		
		/* save original last poverlap samples */
		copyf(pitchbufend - poverlap, lastq, poverlap);
		poffset = 0; /* create pitch buffer with 1 period */
		pitchblen = pitch;
		pitchbufstart = pitchbufend - pitchblen;
		overlapadd(lastq, pitchbufstart - poverlap,
			pitchbufend - poverlap, poverlap);
		
		/* update last 1/4 wavelength in history buffer */
		convertfs(pitchbufend - poverlap, &history[HISTORYLEN-poverlap],
			poverlap);
		getfespeech(out, FRAMESZ); /* get synthesized speech */
	} 
	else if (erasecnt == 1 || erasecnt == 2) 
	{
		/* tail of previous pitch estimate */
		short tmp[POVERLAPMAX];
		int saveoffset = poffset; /* save offset for OLA */
		getfespeech(tmp, poverlap); /* continue with old pitchbuf */
		
		/* add periods to the pitch buffer */
		poffset = saveoffset;
		while (poffset > pitch)
			poffset -= pitch;
		pitchblen += pitch; /* add a period */
		pitchbufstart = pitchbufend - pitchblen;
		overlapadd(lastq, pitchbufstart - poverlap,
			pitchbufend - poverlap, poverlap);
		
		/* overlap add old pitchbuffer with new */
		getfespeech(out, FRAMESZ);
		overlapadd(tmp, out, out, poverlap);
		scalespeech(out);
	}
	else if (erasecnt == 3 || erasecnt == 4 || erasecnt == 5) 
	{
		getfespeech(out, FRAMESZ);
		scalespeech(out);
	}
	else //if(erasecnt > 5)
	{
		convertsf(history, pitchbuf, HISTORYLEN); 
		pitch = findpitch(); 
		poverlap = pitch >> 2; 
		
		copyf(pitchbufend - poverlap, lastq, poverlap);
		poffset = 0; 
		pitchblen = pitch;
		pitchbufstart = pitchbufend - pitchblen;
		overlapadd(lastq, pitchbufstart - poverlap,
			pitchbufend - poverlap, poverlap);
		
		convertfs(pitchbufend - poverlap, &history[HISTORYLEN-poverlap],
			poverlap);
		getfespeech(out, FRAMESZ);
	}
	
	erasecnt++;
	savespeech(out);
}


/*
* Estimate the pitch.
* l - pointer to first sample in last 20 msec of speech.
* r - points to the sample PITCH_MAX before l
*/
int PLC::findpitch()
{
	int i, j, k;
	int bestmatch;
	float bestcorr;
	float corr; /* correlation */
	float energy; /* running energy */
	float scale; /* scale correlation by average power */
	float *rp; /* segment to match */
	float *l = pitchbufend - CORRLEN;
	float *r = pitchbufend - CORRBUFLEN;
	
	/* coarse search */
	rp = r;
	energy = 0.f;
	corr = 0.f;
	for (i = 0; i < CORRLEN; i += NDEC) 
	{
		energy += rp[i] * rp[i];
		corr += rp[i] * l[i];
	}
	scale = energy;
	if (scale < CORRMINPOWER)
	{
		scale = CORRMINPOWER;
	}
	corr = corr / (float)sqrt(scale);
	bestcorr = corr;
	bestmatch = 0;
	
	for (j = NDEC; j <= PITCHDIFF; j += NDEC)
	{
		energy -= rp[0] * rp[0];
		energy += rp[CORRLEN] * rp[CORRLEN];
		rp += NDEC;
		corr = 0.f;
		for (i = 0; i < CORRLEN; i += NDEC)
			corr += rp[i] * l[i];
		scale = energy;
		if (scale < CORRMINPOWER)
			scale = CORRMINPOWER;
		corr /= (float)sqrt(scale);
		if (corr >= bestcorr) {
			bestcorr = corr;
			bestmatch = j;
		}
	}
	/* fine search */
	j = bestmatch - (NDEC - 1);
	if (j < 0)
		j = 0;
	k = bestmatch + (NDEC - 1);
	if (k > PITCHDIFF)
		k = PITCHDIFF;
	rp = &r[j];
	energy = 0.f;
	corr = 0.f;
	for (i = 0; i < CORRLEN; i++) 
	{
		energy += rp[i] * rp[i];
		corr += rp[i] * l[i];
	}
	scale = energy;
	if (scale < CORRMINPOWER)
		scale = CORRMINPOWER;
	
	corr = corr / (float)sqrt(scale);
	bestcorr = corr;
	bestmatch = j;
	for (j++; j <= k; j++) 
	{
		energy -= rp[0] * rp[0];
		energy += rp[CORRLEN] * rp[CORRLEN];
		rp++;
		corr = 0.f;
		for (i = 0; i < CORRLEN; i++)
			corr += rp[i] * l[i];
		scale = energy;
		if (scale < CORRMINPOWER)
			scale = CORRMINPOWER;
		corr = corr / (float)sqrt(scale);
		if (corr > bestcorr) {
			bestcorr = corr;
			bestmatch = j;
		}
	}
	return PITCH_MAX - bestmatch;
}

/*
* Get samples from the circular pitch buffer. Update poffset so
* when subsequent frames are erased the signal continues.
*/
void PLC::getfespeech(short *out, int sz)
{
	while (sz) 
	{
		int cnt = pitchblen - poffset;
		if (cnt > sz)
			cnt = sz;
		convertfs(&pitchbufstart[poffset], out, cnt);
		poffset += cnt;
		if (poffset == pitchblen)
			poffset = 0;
		out += cnt;
		sz -= cnt;
	}
}

void PLC::scalespeech(short *out)
{
	float g = (float)1. - (erasecnt - 1) * ATTENFAC;
	for (int i = 0; i < FRAMESZ; i++) 
	{
		out[i] = (short)(out[i] * g);
		g -= ATTENINCR;
	}
}

/*
* Overlap add left and right sides
*/
void PLC::overlapadd(float *l, float *r, float *o, int cnt)
{
	float incr = (float)1. / cnt;
	float lw = (float)1. - incr;
	float rw = incr;
	for (int i = 0; i < cnt; i++) 
	{
		float t = lw * l[i] + rw * r[i];
		if (t > 32767.)
			t = 32767.;
		else if (t < -32768.)
			t = -32768.;
		o[i] = t;
		lw -= incr;
		rw += incr;
	}
}

void PLC::overlapadd(short *l, short *r, short *o, int cnt)
{
	float incr = (float)1. / cnt;
	float lw = (float)1. - incr;
	float rw = incr;
	for (int i = 0; i < cnt; i++) 
	{
		float t = lw * l[i] + rw * r[i];
		if (t > 32767.)
			t = 32767.;
		else if (t < -32768.)
			t = -32768.;
		o[i] = (short)t;
		lw -= incr;
		rw += incr;
	}
}

/*
* Overlap add the erasure tail with the start of the first good frame
* Scale the synthetic speech by the gain factor before the OLA.
*/
void PLC::overlapaddatend(short *s, short *f, int cnt)
{
	float incr = (float)1. / cnt;
	float gain = (float)1. - (erasecnt - 1) * ATTENFAC;
	if (gain < 0.)
		gain = (float)0.;
	float incrg = incr * gain;
	float lw = ((float)1. - incr) * gain;
	float rw = incr;
	for (int i = 0; i < cnt; i++) 
	{
		float t = lw * f[i] + rw * s[i];
		if (t > 32767.)
			t = 32767.;
		else if (t < -32768.)
			t = -32768.;
		s[i] = (short)t;
		lw -= incrg;
		rw += incr;
	}
}


