#include <emmintrin.h> // SSE2 intrinsics

inline int32_t hsum_epi32_sse2(__m128i x) {
    __m128i hi64 = _mm_unpackhi_epi64(x, x);
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32 = _mm_shuffle_epi32(sum64, _MM_SHUFFLE(2, 3, 0, 1));
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);
}

int32_t IM_Conv_SIMD(uint8_t* pCharKernel, uint8_t* pCharConv, int iLength) {
    const int iBlockSize = 16, Block = iLength / iBlockSize;
    __m128i SumV = _mm_setzero_si128();

    for (int Y = 0; Y < Block * iBlockSize; Y += iBlockSize) {
        __m128i SrcK = _mm_loadu_si128((__m128i*)(pCharKernel + Y));
        __m128i SrcC = _mm_loadu_si128((__m128i*)(pCharConv + Y));
        
        __m128i SrcK_L = _mm_unpacklo_epi8(SrcK, _mm_setzero_si128());
        __m128i SrcK_H = _mm_unpackhi_epi8(SrcK, _mm_setzero_si128());
        __m128i SrcC_L = _mm_unpacklo_epi8(SrcC, _mm_setzero_si128());
        __m128i SrcC_H = _mm_unpackhi_epi8(SrcC, _mm_setzero_si128());

        __m128i SumL = _mm_madd_epi16(SrcK_L, SrcC_L);
        __m128i SumH = _mm_madd_epi16(SrcK_H, SrcC_H);

        __m128i SumT = _mm_add_epi32(SumL, SumH);
        SumV = _mm_add_epi32(SumV, SumT);
    }

    int32_t Sum = hsum_epi32_sse2(SumV);

    for (int Y = Block * iBlockSize; Y < iLength; Y++) {
        Sum += pCharKernel[Y] * pCharConv[Y];
    }

    return Sum;
}