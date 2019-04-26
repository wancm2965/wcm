/*

	audio.c

	use pcm1801u chipset for audio capture

*/
#include <std.h>
#include <stdio.h>
#include <tsk.h>
#include <sys.h>
#include <sio.h>
#include <async.h>
#include <assert.h>
#include <csl.h>
#include <csl_dat.h>
#include <csl_cache.h>

#include "video_proc.h"
#include "pci_util.h"

extern Bool		g_TransOn;
/* 
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for 
 * the cache work optimally on the C6x.
 */
#define BUFLEN 			1280 * 2      /* number of samples in the frame */
#define BUFALIGN 		128           /* alignment of buffer to allow use of L2 cache */
#define BUFSIZE 		(BUFLEN * sizeof(MdUns)) 
#define AUDIO_CHANNEL	3
#define NUM_AUDIO_BUF	3

#define AUDIO_CHANNEL0	0
#define AUDIO_CHANNEL1	1

/* inStream and outStream are SIO handles created in main */
typedef struct 
{
	SIO_Handle inStream;
	PPCI_CONTROL pPCIControl;

} AUDIO_CHAN_INFO;

AUDIO_CHAN_INFO ai;

/* Function prototype */
static Void createStreams();
static Void prime();

static Void _callback(Ptr arg, Int status, Ptr reqp, Uns size)
{
	*(Int*)arg = 0;
}
/*
 * ======== createStreams ========
 */
static Void createStreams()
{
    SIO_Attrs attrs;
	Uint8 *p;
	PVIDEO_FRAME pFrame;

    /* align the buffer to allow it to be used with L2 cache */
    attrs = SIO_ATTRS;
    attrs.align = BUFALIGN;
    attrs.model = SIO_ISSUERECLAIM;
	attrs.nbufs = NUM_AUDIO_BUF;

    /* open the I/O streams */
    ai.inStream = SIO_create("/dioCodec", SIO_INPUT, BUFSIZE, &attrs);
    if (ai.inStream == NULL) {
        SYS_abort("Create input stream FAILED.");
    }

	// malloc pci control block
	p = (Uint8 *)MEM_alloc(EXTERNALHEAP, BUFSIZE + 128, 128);
	assert(p != MEM_ILLEGAL);
	ai.pPCIControl = (PPCI_CONTROL)p;
	ai.pPCIControl->data = p + 128;
	ai.pPCIControl->inUse = 0;

	// preset frame header
	pFrame = (PVIDEO_FRAME)(p + 128);

	pFrame->base_frame.version		= FRAME_VERSION;
	pFrame->base_frame.frame_type	= FRAME_AUDIO;
	pFrame->base_frame.framerate	= 0;
	pFrame->base_frame.channel		= AUDIO_CHANNEL;
}

/*
 * ======== prime ========
 */
static Void prime()
{
	int i;
	Ptr buf[NUM_AUDIO_BUF];

    /* Allocate buffers for the SIO buffer exchanges */
	for (i = 0; i < NUM_AUDIO_BUF; i++)
	{
	    buf[i] = (Ptr)MEM_calloc(EXTERNALHEAP, BUFSIZE, BUFALIGN);
    	if (buf[i] == NULL) {
        	SYS_abort("MEM_calloc failed.");
    	} 
	}
    
    /* Issue the first & second empty buffers to the input stream */
	for (i = 0; i < NUM_AUDIO_BUF; i++)
	{
	    if (SIO_issue(ai.inStream, buf[i], SIO_bufsize(ai.inStream), NULL) != SYS_OK) {
    	    SYS_abort("Error issuing buffer to the input stream");
    	}
	}
}

#define TOTAL_AUDIO_CHANNEL_COUNT	4
void _process(int channel, Ptr sample, int size, int order)
{
	MdUns *in, *out;
	int i, j;
	PVIDEO_FRAME pFrame;
	PPCI_CONTROL pControl = ai.pPCIControl;
	
	while (pControl->inUse == 1)
	{
		TSK_sleep(1);
	}

	out = (MdUns *)((Uint8 *)pControl->data + sizeof(VIDEO_FRAME));
	in = (MdUns *)sample;

	// Extract 16KHz 16bit sample from two 32KHz 2 channal 16bit sample
	for (i = 0; i < size / (sizeof(MdUns) * TOTAL_AUDIO_CHANNEL_COUNT); i++)
	{
		out[i] = in[TOTAL_AUDIO_CHANNEL_COUNT * i + order];
	}

	// set pci control block
	pControl->inUse				= 1;
    pControl->request.srcAddr	= (Uint8 *)pControl->data;
    pControl->request.byteCnt	= size / TOTAL_AUDIO_CHANNEL_COUNT + sizeof(VIDEO_FRAME); 
	pControl->callback.fxn		= (GIO_TappCallback)_callback;
	pControl->callback.arg		= (Ptr)&(pControl->inUse);

	pFrame 						= (PVIDEO_FRAME)((Uint8 *)pControl->data);
	pFrame->base_frame.channel	= channel;

	pci_write_ex(pControl);
}

void tskAudio(int audioNum)
{
    Int nmadus;         /* number of minimal addressable units */
    MdUns *inbuf;

    /* Call createStream function to create I/O streams */
    createStreams();
    
    /* Call prime function to do priming */
    prime();

    /* Loop forever looping back buffers */
    for (;;) {

        /* Reclaim full buffer from the input stream */
        if ((nmadus = SIO_reclaim(ai.inStream, (Ptr *)&inbuf, NULL)) < 0)
        {
            SYS_abort("Error reclaiming full buffer from the input stream");
			printf("error\n");
        }

		if (g_TransOn)
		{
			if (audioNum > 1)
			{
				int i;
				for (i = 0; i < 4; i++)
					_process(i, inbuf, nmadus, i);

			}
			else
				_process(0, inbuf, nmadus, 1);
		}	

        /* Issue an empty buffer to the input stream */
        if (SIO_issue(ai.inStream, inbuf, SIO_bufsize(ai.inStream), NULL) != SYS_OK) {
            SYS_abort("Error issuing empty buffer to the input stream");
        }

    }
}
