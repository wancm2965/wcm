#ifndef INTREADWRITE_H
#define INTREADWRITE_H

#ifdef __GNUC__

struct unaligned_64 { uint64_t l; } __attribute__((packed));
struct unaligned_32 { uint32_t l; } __attribute__((packed));
struct unaligned_16 { uint16_t l; } __attribute__((packed));

#define LD16(a) (((const struct unaligned_16 *) (a))->l)
#define LD32(a) (((const struct unaligned_32 *) (a))->l)
#define LD64(a) (((const struct unaligned_64 *) (a))->l)

#define ST16(a, b) (((struct unaligned_16 *) (a))->l) = (b)
#define ST32(a, b) (((struct unaligned_32 *) (a))->l) = (b)

//#else /* __GNUC__ */
#elif defined(CONFIG_MSVC)

#define LD16(a) (((uint8_t*)(a))[0] | (((uint8_t*)(a))[1] << 8))
#define LD32(a) (((uint8_t*)(a))[0] | (((uint8_t*)(a))[1] << 8) | (((uint8_t*)(a))[2] << 16) | (((uint8_t*)(a))[3] << 24))
#define LD64(a) (LD32(a) | ((uint64_t)LD64(((uint8_t*)(a))+4) << 32))

#define ST16(a, b) (((uint8_t*)(a))[0] = (uint8_t)((b)>>0),\
					((uint8_t*)(a))[1] = (uint8_t)((b)>>8))

#define ST32(a, b) (((uint8_t*)(a))[0] = (uint8_t)((b)>>0),\
					((uint8_t*)(a))[1] = (uint8_t)((b)>>8),\
					((uint8_t*)(a))[2] = (uint8_t)((b)>>16),\
					((uint8_t*)(a))[3] = (uint8_t)((b)>>24))



#else

#define LD16(a) (*((uint16_t*)(a)))
#define LD32(a) (*((uint32_t*)(a)))
#define LD64(a) (*((uint64_t*)(a)))

#define ST16(a, b) *((uint16_t*)(a)) = (b)
#define ST32(a, b) *((uint32_t*)(a)) = (b)

#endif /* !__GNUC__ */

/* endian macros */
#define AV_RB8(x)  (((uint8_t*)(x))[0])
//#define AV_WB8(p, i, d)  { ((uint8_t*)(p))[(i)] = (d); }
#define AV_WB8(p, d)  do { ((uint8_t*)(p))[0] = (d); } while(0)
#define AV_RB16(x) ((((uint8_t*)(x))[0] << 8) | ((uint8_t*)(x))[1])
//#define AV_WB16(p, i, d) { \                 ((uint8_t*)(p))[(i)+1] = (d); \                   ((uint8_t*)(p))[(i)] = (d)>>8; }

#   define AV_WB16(p, d) do {                   \
	((uint8_t*)(p))[1] = (d);               \
	((uint8_t*)(p))[0] = (d)>>8;            \
    } while(0)

#define AV_RB32(x) ((((uint8_t*)(x))[0] << 24) | \
                   (((uint8_t*)(x))[1] << 16) | \
                   (((uint8_t*)(x))[2] << 8) | \
                    ((uint8_t*)(x))[3])
//#define AV_WB32(p, i, d) { \                    ((uint8_t*)(p))[(i)+3] = (d); \                    ((uint8_t*)(p))[(i)+2] = (d)>>8; \                    ((uint8_t*)(p))[(i)+1] = (d)>>16; \                    ((uint8_t*)(p))[(i)] = (d)>>24; }
#   define AV_WB32(p, d) do {                   \
	((uint8_t*)(p))[3] = (d);               \
	((uint8_t*)(p))[2] = (d)>>8;            \
	((uint8_t*)(p))[1] = (d)>>16;           \
	((uint8_t*)(p))[0] = (d)>>24;           \
    } while(0)

#define AV_RL8(x)  AV_RB8(x)
#define AV_WL8(p, i, d)  AV_WB8(p, i, d)

#define AV_RL16(x) ((((uint8_t*)(x))[1] << 8) | ((uint8_t*)(x))[0])

//#define AV_WL16(p, i, d) { \                    ((uint8_t*)(p))[(i)] = (d); \                    ((uint8_t*)(p))[(i)+1] = (d)>>8; }
#   define AV_WL16(p, d) do {                   \
	((uint8_t*)(p))[0] = (d);               \
	((uint8_t*)(p))[1] = (d)>>8;            \
    } while(0)

#define AV_RL32(x) ((((uint8_t*)(x))[3] << 24) | \
                   (((uint8_t*)(x))[2] << 16) | \
                   (((uint8_t*)(x))[1] << 8) | \
                    ((uint8_t*)(x))[0])
/*#define AV_WL32(p, i, d) { \
                    ((uint8_t*)(p))[(i)] = (d); \
                    ((uint8_t*)(p))[(i)+1] = (d)>>8; \
                    ((uint8_t*)(p))[(i)+2] = (d)>>16; \
                    ((uint8_t*)(p))[(i)+3] = (d)>>24; }*/
#define AV_WL32(p, d) do {                   \
	((uint8_t*)(p))[0] = (d);               \
	((uint8_t*)(p))[1] = (d)>>8;            \
	((uint8_t*)(p))[2] = (d)>>16;           \
	((uint8_t*)(p))[3] = (d)>>24;           \
    } while(0)

#endif /* INTREADWRITE_H */
