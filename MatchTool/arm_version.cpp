#include <arm_neon.h>

inline int32_t vaddvq_s32(int32x4_t v) {
    int32x2_t tmp = vpadd_s32(vget_low_s32(v), vget_high_s32(v));
    return vget_lane_s32(vpadd_s32(tmp, tmp), 0);
}

inline int32_t IM_Conv_SIMD(uint8_t* pCharKernel, uint8_t* pCharConv, int iLength) {
    const int iBlockSize = 16, Block = iLength / iBlockSize;
    int32x4_t SumV = vdupq_n_s32(0);
    uint8x16_t Zero = vdupq_n_u8(0);

    for (int Y = 0; Y < Block * iBlockSize; Y += iBlockSize) {
        uint8x16_t SrcK = vld1q_u8(pCharKernel + Y);
        uint8x16_t SrcC = vld1q_u8(pCharConv + Y);
        int16x8_t SrcK_L = vmovl_u8(vget_low_u8(SrcK));
        int16x8_t SrcK_H = vmovl_u8(vget_high_u8(SrcK));
        int16x8_t SrcC_L = vmovl_u8(vget_low_u8(SrcC));
        int16x8_t SrcC_H = vmovl_u8(vget_high_u8(SrcC));
        int32x4_t SumT = vaddq_s32(vmull_s16(vget_low_s16(SrcK_L), vget_low_s16(SrcC_L)), vmull_s16(vget_low_s16(SrcK_H), vget_low_s16(SrcC_H)));
        SumV = vaddq_s32(SumV, SumT);
    }

    int32_t Sum = vaddvq_s32(SumV);

    for (int Y = Block * iBlockSize; Y < iLength; Y++) {
        Sum += pCharKernel[Y] * pCharConv[Y];
    }
    
    return Sum;
}