#ifndef _BITSTREAM_H
#define _BITSTREAM_H

//#define ALT_BITSTREAM_WRITER
//#define ALIGNED_BITSTREAM_WRITER

#define ALT_BITSTREAM_READER
//#define LIBMPEG2_BITSTREAM_READER
//#define A32_BITSTREAM_READER
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifndef __BEOS__
#    include <errno.h>
#else
#    include "berrno.h"
#endif
//#include <math.h>

#ifndef ENODATA
#    define ENODATA  61
#endif

/* windows */
#define int8_t  char
#define uint8_t unsigned char
#define int16_t  short
#define uint16_t unsigned short
#define int32_t  int
#define uint32_t unsigned int
#define int64_t  __int64
#define uint64_t unsigned __int64
/*typedef unsigned short UINT16;
typedef signed short INT16;
typedef unsigned char UINT8;
typedef unsigned int UINT32;
typedef unsigned __int64 UINT64;
typedef signed char INT8;
typedef signed int INT32;
typedef signed __int64 INT64;

typedef UINT8 uint8_t;
typedef INT8 int8_t;
typedef UINT16 uint16_t;
typedef INT16 int16_t;
typedef UINT32 uint32_t;
typedef INT32 int32_t;
typedef UINT64 uint64_t;
typedef INT64 int64_t;
*/
#define INT8  char
#define UINT8 unsigned char
#define INT16  short
#define UINT16 unsigned short
#define INT32  int
#define UINT32 unsigned int
#define UINT64  __int64
#define INT64 unsigned __int64


#define MANGLE(a) #a


//rounded divison & shift
#define RSHIFT(a,b) ((a) > 0 ? ((a) + (1<<((b)-1)))>>(b) : ((a) + (1<<((b)-1))-1)>>(b))
/* assume b>0 */
#define ROUNDED_DIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#define ABS(a) ((a) >= 0 ? (a) : (-(a)))

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

#define NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))

#define bswap_32(x) \
     ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
      (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
#define be2me_32(x) bswap_32(x)

uint32_t unaligned32(const void *v) {
    return *(const uint32_t *) v;
}
/* bit output */

struct PutBitContext;

typedef struct PutBitContext {
#ifdef ALT_BITSTREAM_WRITER
    UINT8 *buf, *buf_end;
    int index;
#else
    UINT32 bit_buf;
    int bit_left;
    UINT8 *buf, *buf_ptr, *buf_end;
#endif
    INT64 data_out_size; /* in bytes */
} PutBitContext;

void init_put_bits(PutBitContext *s, 
                   UINT8 *buffer, int buffer_size,
                   void *opaque,
                   void (*write_data)(void *, UINT8 *, int));

INT64 get_bit_count(PutBitContext *s); /* XXX: change function name */
void align_put_bits(PutBitContext *s);
void flush_put_bits(PutBitContext *s);
void put_string(PutBitContext * pbc, char *s);

/* bit input */

typedef struct GetBitContext {
    UINT8 *buffer, *buffer_end;
#ifdef ALT_BITSTREAM_READER
    int index;
#elif defined LIBMPEG2_BITSTREAM_READER
    UINT8 *buffer_ptr;
    UINT32 cache;
    int bit_count;
#elif defined A32_BITSTREAM_READER
    UINT32 *buffer_ptr;
    UINT32 cache0;
    UINT32 cache1;
    int bit_count;
#endif
    int size;
} GetBitContext;

int get_bits_count(GetBitContext *s);

#ifndef ALT_BITSTREAM_WRITER
void put_bits(PutBitContext *s, int n, unsigned int value)
{
    unsigned int bit_buf;
    int bit_left;

#ifdef STATS
    st_out_bit_counts[st_current_index] += n;
#endif
    //    printf("put_bits=%d %x\n", n, value);
    assert(n == 32 || value < (1U << n));
    
    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

    //    printf("n=%d value=%x cnt=%d buf=%x\n", n, value, bit_cnt, bit_buf);
    /* XXX: optimize */
    if (n < bit_left) {
        bit_buf = (bit_buf<<n) | value;
        bit_left-=n;
    } else {
	bit_buf<<=bit_left;
        bit_buf |= value >> (n - bit_left);
#ifdef UNALIGNED_STORES_ARE_BAD
        if (3 & (int) s->buf_ptr) {
            s->buf_ptr[0] = bit_buf >> 24;
            s->buf_ptr[1] = bit_buf >> 16;
            s->buf_ptr[2] = bit_buf >>  8;
            s->buf_ptr[3] = bit_buf      ;
        } else
#endif
        *(UINT32 *)s->buf_ptr = be2me_32(bit_buf);//
        //printf("bitbuf = %08x\n", bit_buf);
        s->buf_ptr+=4;
	bit_left+=32 - n;
        bit_buf = value;
    }

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}
#endif



uint8_t* pbBufPtr(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
	return s->buf + (s->index>>3);
#else
	return s->buf_ptr;
#endif
}

/* Bitstream reader API docs:
name
    abritary name which is used as prefix for the internal variables

gb
    getbitcontext

OPEN_READER(name, gb)
    loads gb into local variables

CLOSE_READER(name, gb)
    stores local vars in gb

UPDATE_CACHE(name, gb)
    refills the internal cache from the bitstream
    after this call at least MIN_CACHE_BITS will be available,

GET_CACHE(name, gb)
    will output the contents of the internal cache, next bit is MSB of 32 or 64 bit (FIXME 64bit)

SHOW_UBITS(name, gb, num)
    will return the nest num bits

SHOW_SBITS(name, gb, num)
    will return the nest num bits and do sign extension

SKIP_BITS(name, gb, num)
    will skip over the next num bits
    note, this is equinvalent to SKIP_CACHE; SKIP_COUNTER

SKIP_CACHE(name, gb, num)
    will remove the next num bits from the cache (note SKIP_COUNTER MUST be called before UPDATE_CACHE / CLOSE_READER)

SKIP_COUNTER(name, gb, num)
    will increment the internal bit counter (see SKIP_CACHE & SKIP_BITS)

LAST_SKIP_CACHE(name, gb, num)
    will remove the next num bits from the cache if it is needed for UPDATE_CACHE otherwise it will do nothing

LAST_SKIP_BITS(name, gb, num)
    is equinvalent to SKIP_LAST_CACHE; SKIP_COUNTER

for examples see get_bits, show_bits, skip_bits, get_vlc
*/

#ifdef ALT_BITSTREAM_READER
#   define MIN_CACHE_BITS 25

#   define OPEN_READER(name, gb)\
        int name##_index= (gb)->index;\
        int name##_cache= 0;\

#   define CLOSE_READER(name, gb)\
        (gb)->index= name##_index;\

#   define UPDATE_CACHE(name, gb)\
        name##_cache= be2me_32( unaligned32( ((uint8_t *)(gb)->buffer)+(name##_index>>3) ) ) << (name##_index&0x07);\

#   define SKIP_CACHE(name, gb, num)\
        name##_cache <<= (num);\

// FIXME name?
#   define SKIP_COUNTER(name, gb, num)\
        name##_index += (num);\

#   define SKIP_BITS(name, gb, num)\
        {\
            SKIP_CACHE(name, gb, num)\
            SKIP_COUNTER(name, gb, num)\
        }\

#   define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)
#   define LAST_SKIP_CACHE(name, gb, num) ;

#   define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache, num)

#   define SHOW_SBITS(name, gb, num)\
        NEG_SSR32(name##_cache, num)

#   define GET_CACHE(name, gb)\
        ((uint32_t)name##_cache)

int get_bits_count(GetBitContext *s){
    return s->index;
}
#endif 

unsigned int get_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
    return tmp;
}

unsigned int show_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
//    CLOSE_READER(re, s)
    return tmp;
}

void skip_bits(GetBitContext *s, int n){
 //Note gcc seems to optimize this to s->index+=n for the ALT_READER :))
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
}

unsigned int get_bits1(GetBitContext *s){
#ifdef ALT_BITSTREAM_READER
    int index= s->index;
    uint8_t result= s->buffer[ index>>3 ];
    result<<= (index&0x07);
    result>>= 8 - 1;
    index++;
    s->index= index;

    return result;
#else
    return get_bits(s, 1);
#endif
}

unsigned int show_bits1(GetBitContext *s){
    return show_bits(s, 1);
}

void skip_bits1(GetBitContext *s){
    skip_bits(s, 1);
}

void init_get_bits(GetBitContext *s,
                   UINT8 *buffer, int buffer_size);

int check_marker(GetBitContext *s, const char *msg);
void align_get_bits(GetBitContext *s);
void init_put_bits(PutBitContext *s, 
                   UINT8 *buffer, int buffer_size,
                   void *opaque,
                   void (*write_data)(void *, UINT8 *, int))
{
    s->buf = buffer;
    s->buf_end = s->buf + buffer_size;
    s->data_out_size = 0;
    if(write_data!=NULL) 
    {
    	//fprintf(stderr, "write Data callback is not supported\n");
    }
#ifdef ALT_BITSTREAM_WRITER
    s->index=0;
    ((uint32_t*)(s->buf))[0]=0;
//    memset(buffer, 0, buffer_size);
#else
    s->buf_ptr = s->buf;
    s->bit_left=32;
    s->bit_buf=0;
#endif
}

/* return the number of bits output */
INT64 get_bit_count(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    return s->data_out_size * 8 + s->index;
#else
    return (s->buf_ptr - s->buf + s->data_out_size) * 8 + 32 - (INT64)s->bit_left;
#endif
}

void align_put_bits(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    put_bits(s,(  - s->index) & 7,0);
#else
    put_bits(s,s->bit_left & 7,0);
#endif
}

/* pad the end of the output stream with zeros */
void flush_put_bits(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    align_put_bits(s);
#else
    s->bit_buf<<= s->bit_left;
    while (s->bit_left < 32) {
        /* XXX: should test end of buffer */
        *s->buf_ptr++=s->bit_buf >> 24;
        s->bit_buf<<=8;
        s->bit_left+=8;
    }
    s->bit_left=32;
    s->bit_buf=0;
#endif
}

void put_string(PutBitContext * pbc, char *s)
{
    while(*s){
        put_bits(pbc, 8, *s);
        s++;
    }
    put_bits(pbc, 8, 0);
}

/* bit input functions */

void init_get_bits(GetBitContext *s,
                   UINT8 *buffer, int buffer_size)
{
    s->buffer= buffer;
    s->size= buffer_size;
    s->buffer_end= buffer + buffer_size;
#ifdef ALT_BITSTREAM_READER
    s->index=0;
#elif defined LIBMPEG2_BITSTREAM_READER
    s->buffer_ptr = buffer;
    s->bit_count = 16;
    s->cache = 0;
#elif defined A32_BITSTREAM_READER
    s->buffer_ptr = (uint32_t*)buffer;
    s->bit_count = 32;
    s->cache0 = 0;
    s->cache1 = 0;
#endif
    {
        OPEN_READER(re, s)
        UPDATE_CACHE(re, s)
//        UPDATE_CACHE(re, s)
        CLOSE_READER(re, s)
    }
#ifdef A32_BITSTREAM_READER
    s->cache1 = 0;
#endif
}

void align_get_bits(GetBitContext *s)
{
    int n= (-get_bits_count(s)) & 7;
    if(n) skip_bits(s, n);
}

int check_marker(GetBitContext *s, const char *msg)
{
    int bit= get_bits1(s);
    //if(!bit) printf("Marker bit missing %s\n", msg);

    return bit;
}

#endif //AV_BITSTREAM_H