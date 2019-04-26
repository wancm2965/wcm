#include "XMemcpy.h"
#include <smmintrin.h>
#include <intrin.h>

void* gpu_memcpy(void* d, const void* s, size_t size)
{
	static bool s_SSE4_1_enabled = IsSSE41Enabled();
	static const size_t regsInLoop = 2;

	if (d == NULL || s == NULL) return NULL;

	// If memory is not aligned, use memcpy
	bool isAligned = (((size_t)(s) | (size_t)(d)) & 0xF) == 0;
	if (!(isAligned && s_SSE4_1_enabled))
	{
		return memcpy(d, s, size);
	}

	size_t reminder = size & (regsInLoop * sizeof(__m128i) - 1); // Copy 32 bytes every loop
	size_t end = 0;

	__m128i xmm0, xmm1; // Will actually use xmm registers
	__m128i* pTrg = (__m128i*)d;
	__m128i* pTrgEnd = pTrg + ((size - reminder) >> 4);
	__m128i* pSrc = (__m128i*)s;

	// Make sure source is synced - doesn't hurt if not needed.
	_mm_sfence();

	while (pTrg < pTrgEnd)
	{
		// _mm_stream_load_si128 emits the Streaming SIMD Extensions 4 (SSE4.1) instruction MOVNTDQA
		// Fastest method for copying GPU RAM. Available since Penryn (45nm Core 2 Duo/Quad)
		xmm0  = _mm_stream_load_si128(pSrc);
		xmm1  = _mm_stream_load_si128(pSrc + 1);
		pSrc += regsInLoop;

		// _mm_store_si128 emit the SSE2 intruction MOVDQA (aligned store)
		_mm_store_si128(pTrg     , xmm0);
		_mm_store_si128(pTrg +  1, xmm1);
		_mm_store_si128(pTrg +  1, xmm1); // Not a bug - works 4.5% faster!
		_mm_store_si128(pTrg +  1, xmm1);
		pTrg += regsInLoop;
	}

	// Copy in 16 byte steps
	if (reminder >= 16)
	{
		size = reminder;
		reminder = size & 15;
		end = size >> 4;
		for (size_t i = 0; i < end; ++i)
		{
			pTrg[i] = _mm_stream_load_si128(pSrc + i);
		}
	}

	// Copy last bytes - shouldn't happen as strides are modulu 16
	if (reminder)
	{
		__m128i temp = _mm_stream_load_si128(pSrc + end);

		char* ps = (char*)(&temp);
		char* pt = (char*)(pTrg + end);

		for (size_t i = 0; i < reminder; ++i)
		{
			pt[i] = ps[i];
		}
	}

	return d;
}


bool IsSSE41Enabled()
{
	int CPUInfo[4];
	__cpuid(CPUInfo, 1);

	return 0 != (CPUInfo[2] & (1<<19)); // 19th bit of 2nd reg means sse4.1 is enabled
}