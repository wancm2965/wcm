#ifndef __AVC_CODEC_H__
#define __AVC_CODEC_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagH263RTPCONTEXT{
    unsigned char *pbuf;	
	int mb_w;  //macro-block width;
	int mb_h; //macro-block height;
    int *mba_index;//macro-block adress indices;
    int *mb_quant; //macro-block quant;
	short *mv_mode;//moving vector
	short *mx;
	short *my;//
	short mb_count;
 	unsigned long ts_;		/* timestamp [in] */
} H263RTPCONTEXT,*PH263RTPCONTEXT;


typedef void (*H263_RTP_CALLBACK)(void * user_data,PH263RTPCONTEXT pH263RTPContext,void *data,int size,int keyframe,int gob_number,int mb_count);

typedef enum{
	AVC_CODEC_H263=0,
	AVC_CODEC_H263P,
	AVC_CODEC_H264,
	AVC_CODEC_MJPEG,
	AVC_CODEC_COUNT
}AVC_CODEC_TYPE;

void avc_init(void);
void avc_uninit(void);

void*avc_encoder_open(AVC_CODEC_TYPE codecType,int nWidth,int nHeight,int nMaxRTPFrameSize,H263_RTP_CALLBACK H263RTPCallback,void*pUserData);
void avc_encoder_close(void*pHandle);
int avc_encoder_encode(void*pHandle,unsigned char*pYV12[],unsigned char*pStreamBuf,int nBufLen,int nQuant,int* pbKeyFrame);

void*avc_decoder_open(AVC_CODEC_TYPE codecType,int nWidth,int nHeight);
void avc_decoder_close(void*pHandle);
int avc_decoder_decode(void*pHandle,unsigned char*pStreamData,int nStreamLen,unsigned char*pYV12[],int stride[],char** ppqscale_table,int* pqstride,int*pnWidth,int*pnHeight,int*pnUsedBytes,int*pnKeyFrame);


#ifdef __cplusplus
}
#endif

#endif
