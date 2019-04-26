
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"

#include "typedefs.h"
#include "mathevrc.h"
#include "macro.h"
#include "bqiir.h"


#include "decode.h"
#include "encode.h"
#include "ns127.h"
#include "rda.h"

#include "tty.h"
#include "tty_dbg.h"

//#define  SPEECH_BUFFER_LEN        160
//#define  LOOKAHEAD_LEN             80
//#define  BITSTREAM_BUFFER_LEN      12	/*  data + 1 word for rate  */

typedef struct
{
	char *input_filename;
	char *output_filename;
	Shortword max_frames;
	Shortword encode_only;
	Shortword decode_only;
	Shortword max_rate;
	Shortword max_rate_default;
	Shortword min_rate;
	Shortword min_rate_default;
	Shortword post_filter;
	Shortword post_filter_default;
	Shortword noise_suppression;
	Shortword noise_suppression_default;
	Shortword ibuf_len;
	Shortword obuf_len;
//	char	*signaling_filename;
//	FILE	*fp_signaling;
}
EvrcArgs;

/*
void print_eargs( FILE *fp,	EvrcArgs *e)
{
	if (e->encode_only && !e->decode_only)
	{
		e->ibuf_len = SPEECH_BUFFER_LEN;
		e->obuf_len = BITSTREAM_BUFFER_LEN - 1;
	}
	else if (e->decode_only && !e->encode_only)
	{
		e->ibuf_len = BITSTREAM_BUFFER_LEN - 1;
		e->obuf_len = SPEECH_BUFFER_LEN;
	}
	else if (!e->encode_only && !e->decode_only)
	{
		e->ibuf_len = SPEECH_BUFFER_LEN;
		e->obuf_len = SPEECH_BUFFER_LEN;
	}
}

EvrcArgs *get_eargs( int argc, char *argv[] )
{
	EvrcArgs *eargs;
	Shortword option;

	if (argc < 5)
	{
		eargs = NULL;
	}
	else
	{
		if ((eargs = (EvrcArgs *) malloc(sizeof(EvrcArgs))) == NULL)
		{
			fprintf(stderr, "%s:  ERROR - Unable to malloc arg memory.\n", argv[0]);
			exit(-1);
		}
		eargs->input_filename = NULL;
		eargs->output_filename = NULL;
		eargs->encode_only = 0;
		eargs->decode_only = 0;
		eargs->max_rate = 4;
		eargs->max_rate_default = 4;
		eargs->min_rate = 1;
		eargs->min_rate_default = 1;
		eargs->post_filter = 1;
		eargs->post_filter_default = 1;
		eargs->noise_suppression = 1;
		eargs->noise_suppression_default = 1;
		eargs->ibuf_len = SPEECH_BUFFER_LEN;
		eargs->obuf_len = SPEECH_BUFFER_LEN;
		eargs->max_frames = 0x7fff;
        tty_option = TTY_NO_GAIN;//1;
	//	eargs->signaling_filename = NULL;
	//	eargs->fp_signaling       = NULL;

        while ((option = getopt(argc, argv, "i:o:redh:l:p:n:c:f:t:D:s:")) != EOF)
		{
			switch (option)
			{
			case 'i':
				eargs->input_filename = argv[optind - 1];
				break;
			case 'o':
				eargs->output_filename = argv[optind - 1];
				break;
			case 'e':
				eargs->encode_only = 1;
				if (eargs->decode_only)
				{
					fprintf(stderr, "%s:  ERROR - Encode with decode.\n", argv[0]);
					exit(-1);
				}
				break;
			case 'd':
				eargs->decode_only = 1;
				if (eargs->encode_only)
				{
					fprintf(stderr, "%s:  ERROR - Decode with encode.\n", argv[0]);
					exit(-1);
				}
				break;
			case 'h':
				eargs->max_rate = (Shortword) atoi(argv[optind - 1]);
				if (eargs->max_rate > 4)
					eargs->max_rate = 4;
				if (eargs->max_rate < 1)
					eargs->max_rate = 1;
				eargs->max_rate_default = 0;
				break;
			case 'l':
				eargs->min_rate = (Shortword) atoi(argv[optind - 1]);
				if (eargs->min_rate > 4)
					eargs->min_rate = 4;
				if (eargs->min_rate < 1)
					eargs->min_rate = 1;
				eargs->min_rate_default = 0;
				break;
			case 'p':
				eargs->post_filter = (Shortword) atoi(argv[optind - 1]);
				if (eargs->post_filter != 0)
					eargs->post_filter = 1;
				eargs->post_filter_default = 0;
				break;
			case 'n':
				eargs->noise_suppression = (Shortword) atoi(argv[optind - 1]);
				if (eargs->noise_suppression != 0)
					eargs->noise_suppression = 1;
				eargs->noise_suppression_default = 0;
				break;
			case 'f':
				eargs->max_frames = (Shortword) atoi(argv[optind - 1]);
				break;
            case 't':
                tty_option = (short)atoi(argv[optind-1]);
                break;
            case 'D':
                tty_debug_print_flag = (short) atoi(argv[optind-1]) & TTY_DEBUG_PRINT;
                tty_debug_flag = (short) atoi(argv[optind-1]) & TTY_DEBUG_DUMP;
                break;
				
    //        case 's':
				//eargs->signaling_filename = argv[optind-1];
    //            eargs->fp_signaling = fopen(argv[optind-1],"rb");
				//if( eargs->fp_signaling == NULL )
				//{
				//	fprintf(stderr,"Error opening signaling file %s\n",argv[optind-1]);
				//	exit(-1);
				//}
				//break;
            }
		}
		if (eargs->min_rate > eargs->max_rate)
		{
			eargs->min_rate = eargs->max_rate;
		}
	}
	return (eargs);
}


Shortword GetNumFrames(	  FILE * fp,
						  Shortword blocksize)
{

	Shortword position;
	Shortword numFrames;


	position = ftell(fp);
	fseek(fp, 0L, 2);
	numFrames = ftell(fp) / blocksize;
	fseek(fp, position, 0);
	return (numFrames);
}

void Encode(Shortword rate, Shortword * obuffer)
{
	encode(rate, obuffer);
	post_encode();
}

void Decode( Shortword * ibuffer,Shortword rate, Shortword post_filter, Shortword * obuffer )
{
	decode(ibuffer, rate, post_filter, obuffer);
}*/

void main( )
{
	FILE *ifileP;
	FILE *ofileP;
	int j = 0;
	Shortword rate = 0;
	DecodeArgs * handle = NULL;
	Shortword buf[SPEECH_BUFFER_LEN];
    Shortword buf16[BITSTREAM_BUFFER_LEN];

	ifileP = fopen("enc.3gp", "rb");
	ofileP = fopen("out.pcm", "wb");
	handle = OpenEVRCDecoder( );

	while ((fread(buf16, sizeof(Shortword), BITSTREAM_BUFFER_LEN, ifileP)) == BITSTREAM_BUFFER_LEN)
	{
		DecodeOneFrame(handle, buf16, buf);
		fwrite(buf, sizeof(Shortword), SPEECH_BUFFER_LEN, ofileP);
		fprintf(stderr, "Decoding %d\r", j);
		j++;
	}

	fprintf(stderr, "\n");
	fclose(ifileP);
	fclose(ofileP);
	CloseEVRCDecoder(handle);
}

void main_enc( )
{
	FILE *ifileP;
	FILE *ofileP;
	int j = 0;
	Shortword rate = 0;
	DecodeArgs * handle = NULL;
	Shortword buf[SPEECH_BUFFER_LEN];
    Shortword buf16[BITSTREAM_BUFFER_LEN];

	ifileP = fopen("8k.pcm", "rb");
	ofileP = fopen("enc.3gp", "wb");
	handle = OpenEVRCEncoder( );

	while ((fread(buf, sizeof(Shortword), SPEECH_BUFFER_LEN, ifileP)) == SPEECH_BUFFER_LEN)
	{
		EncodeOneFrame(handle, buf, buf16);
		fwrite(buf16, sizeof(Shortword), BITSTREAM_BUFFER_LEN, ofileP);
		fprintf(stderr, "Encoding %d\r", j);
		j++;
	}

	fprintf(stderr, "\n");
	fclose(ifileP);
	fclose(ofileP);
	CloseEVRCEncoder(handle);
}

/*
// ½âÂë £º -d -i enc.3gp -o out.pcm
// ±àÂë £º -e -i 8k.pcm -o enc.3gp
int main_old(int argc, char *argv[])
{
	EvrcArgs *eargs;

	FILE *ifileP;
	FILE *ofileP;

	Shortword buf[SPEECH_BUFFER_LEN];
    Shortword buf16[BITSTREAM_BUFFER_LEN];

	Shortword buf_count;
	Shortword ibuf_len;
	Shortword obuf_len;
    Shortword j;
	Shortword rate;
	Shortword beta;
    float rate_sum;
    float avg_rate;
	Longword R[17];
    Shortword min_rate;
	Shortword max_rate;
	char signaling;

    Shortword k;

	if ((eargs = get_eargs(argc, argv)) == NULL)
	{
		exit(-1);
	}
	print_eargs(stdout, eargs);

	if ((ifileP = fopen(eargs->input_filename, "rb")) == NULL)
	{
		fprintf(stderr, "%s:  ERROR - Unable to open input file \"%s\".\n",
				argv[0],
				eargs->input_filename
			);
		exit(-1);
	}
	if ((ofileP = fopen(eargs->output_filename, "wb")) == NULL)
	{
		fprintf(stderr, "%s:  ERROR - Unable to open output file \"%s\".\n",
				argv[0],
				eargs->output_filename
			);
		exit(-1);
	}

	//
	//if (eargs->decode_only)
	//{
	//	buf_count = GetNumFrames(ifileP, sizeof(Shortword) * BITSTREAM_BUFFER_LEN);
	//}
	//else
	//{
	//	buf_count = GetNumFrames(ifileP, sizeof(Shortword) * SPEECH_BUFFER_LEN);
	//}

	InitEncoder();
	InitDecoder();

    if( tty_option == TTY_NO_GAIN )
    {
        init_tty_enc( &tty_enc_char, &tty_enc_header, &tty_enc_baud_rate);
        init_tty_dec();
        tty_debug();
    }

	rate_sum = 0.0;
	avg_rate = 0.0;

	ibuf_len = eargs->ibuf_len;
	obuf_len = eargs->obuf_len;
	j = 1;

	if (!eargs->decode_only)
	{
        while (((fread(buf, sizeof(Shortword), ibuf_len, ifileP)) == ibuf_len)
				&& (j<=eargs->max_frames))
		{
		    bqiir(buf);
            if( tty_option == TTY_NO_GAIN )
            {
                tty_enc_flag = tty_enc( &tty_enc_char,
                                        &tty_enc_header,
                                        &tty_enc_baud_rate,
                                        buf,
                                        FrameSize );
            }

            if( (tty_option == TTY_DISABLED || tty_enc_flag == 0) && eargs->noise_suppression)
            {
                noise_suprs(buf);
                noise_suprs(buf + ibuf_len / 2);
            }
            else
            {
                for (k=0; k<ibuf_len; k++)
                {
                    buf[k] = shift_r(buf[k],-1);
                }
            }

			beta = pre_encode(buf, R);

            if( tty_option == TTY_NO_GAIN && tty_enc_flag != 0 )
            {
      
                min_rate = MIN(4,eargs->max_rate);
            }
            else
            {
                min_rate = eargs->min_rate;
            }
            
			max_rate = eargs->max_rate;

			
			////if( eargs->fp_signaling != NULL )
			////{
			////	k = fread(&signaling, sizeof(char), 1, eargs->fp_signaling);
			////	if( k == 0 )
			////	{
			////		fseek(eargs->fp_signaling,0,0);
			////		k = fread(&signaling, sizeof(char), 1, eargs->fp_signaling);
			////		if( k == 0 )
			////		{
			////			fprintf(stderr,"Error reading signaling file %s: File Empty!\n",eargs->signaling_filename);
			////			exit(-1);
			////		}
			////	}
			////	if( signaling )
			////	{
			////		max_rate = MIN(3,eargs->max_rate);
			////	}
			////}
                
            rate = select_rate(R, max_rate, min_rate, beta);
			switch (rate)
			{
			case 1:
				rate_sum += 1;
				break;
			case 3:
				rate_sum += 4;
				break;
			default:
				rate_sum += 8;
				break;
			}
			avg_rate = (rate_sum / (float) j) * 1.2;

            Encode(rate, buf16);
            if (eargs->encode_only)
			{
				fwrite(&rate, sizeof(Shortword), 1, ofileP);
                fwrite(buf16, sizeof(Shortword), obuf_len, ofileP);
				//fprintf(stderr, "Encoding %d of %d   avg_rate= %6.2f\r", j, buf_count, avg_rate);
			}
			else
			{
                Decode(buf16, rate, eargs->post_filter, buf);
                fwrite(buf, sizeof(Shortword), obuf_len, ofileP);
			//	fprintf(stderr, "Encoding/Decoding %d of %d   avg_rate= %6.2f\r", j, buf_count, avg_rate);
			}
			j++;
		}
	}
	else
	{
		while ((fread(&rate, sizeof(Shortword), 1, ifileP)) == 1)
		{
            if ((fread(buf16, sizeof(Shortword), ibuf_len, ifileP)) == ibuf_len)
			{

                Decode(buf16, rate, eargs->post_filter, buf);
                fprintf(stderr, "Decoding %d\r", j);
                fwrite(buf, sizeof(Shortword), obuf_len, ofileP);
				j++;
			}
		}
	}

	//if (eargs->fp_signaling != NULL)
	//   fclose(eargs->fp_signaling);

	fprintf(stderr, "\n");
	exit(0);
}
*/
