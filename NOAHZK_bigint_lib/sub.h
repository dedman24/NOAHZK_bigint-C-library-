/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_add_included
#define NOAHZK_bigint_add_included

#include "stdint.h"
#include "definitions.h"

void NOAHZK_variable_width_sub(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, struct NOAHZK_variable_width_var* rs1){
    NOAHZK_limb_t borrow = 0;

    for(uint64_t i = 0; i < dst->width; i++){
// carry-out goes into the 33rd bit, which can be extracted in constant-time assuming constant-time shifts
        uint64_t z = (uint64_t)(i < rs0->width? rs0->arr[i]: 0) - (uint64_t)(i < rs1->width? rs1->arr[i]: 0) - borrow;
        dst->arr[i] = z & NOAHZK_LIMB_MAX;
        borrow = z >> BITS_IN_NOAHZK_LIMB & 1;
    }
}

void NOAHZK_variable_width_sub_constant(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, const uint64_t k){
    NOAHZK_limb_t borrow = 0;

    for(uint64_t i = 0; i < dst->width; i++){
// carry-out goes into the 33rd bit, which can be extracted in constant-time assuming constant-time shifts
        uint64_t z = (uint64_t)(i < rs0->width? rs0->arr[i]: 0) - (uint64_t)(i < sizeof(uint64_t)/sizeof(NOAHZK_limb_t)? (k >> (i*BITS_IN_NOAHZK_LIMB)) & NOAHZK_LIMB_MAX: 0) - borrow;
        dst->arr[i] = z & NOAHZK_LIMB_MAX;
        borrow = z >> BITS_IN_NOAHZK_LIMB & 1;
    }
}

// compiles to constant-time code on any cpu with constant-time shifts.
void NOAHZK_variable_width_neg_byte(void* real_dst, void* real_src, const uint64_t width){
    uint8_t carry = 1;
    uint8_t *dst = real_dst, *src = real_src;

    for(uint64_t i = 0; i < width; i++){
// if subtraction overflows and requires a borrow, z (when interpreted as a signed type) is negative and thus bit 15 is set. this also means that bit 8 is set, so we can check that for borrow. 
        uint16_t z = (uint16_t)(~src[i]) + carry;
        dst[i] = z & UINT8_MAX;
        carry = z >> BITS_IN_UINT8_T;
    }
}

void NOAHZK_variable_width_sub_with_bit_offset_byte(void* real_dst, void* real_rs0, void* real_rs1, const uint64_t width0, const uint64_t width1, const uint64_t width_result, uint64_t bit_offset){
    int8_t borrow = 0;
    uint8_t *dst = real_dst, *rs0 = real_rs0, *rs1 = real_rs1;

    const uint64_t byte_offset = bit_offset/BITS_IN_UINT8_T;

    for(uint64_t i = byte_offset; i < width_result; i++){
// runs in constant time regardless of conditional statements because those statements are NOT DATA-DEPENDENT; widths of operands in NOAHZK are public.
// correctly applies offset because shifts rs1 by bit_offset modulo BITS_IN_UINT8_T
// the top 8 bits of z are set to the "overflow/shifted data" that doesn't fit inside the bottom 8 bits, to which the carry out from the previous addition is also added
// the top 8 bits are set as the carry for the next repetition and this is repeated
// we have the top bits that didn't fit in 8 bits of the previous addtition stored inside carry and added to the bottom part of the next addition
// idk how good this explanation is.
        uint16_t z = (uint16_t)(i < width0? rs0[i]: 0) - (i-byte_offset < width1? (uint16_t)rs1[i-byte_offset] << (bit_offset % BITS_IN_UINT8_T): 0) + (int16_t)borrow;

        dst[i] = z & UINT8_MAX;
        borrow = z >> BITS_IN_UINT8_T;
    }
}

// compiles to constant-time code on any cpu with constant-time shifts.
void NOAHZK_variable_width_sub_byte(void* real_dst, void* real_rs0, void* real_rs1, const uint64_t width){
    uint8_t borrow = 0;
    uint8_t *dst = real_dst, *rs0 = real_rs0, *rs1 = real_rs1;

    for(uint64_t i = 0; i < width; i++){
// if subtraction overflows and requires a borrow, z (when interpreted as a signed type) is negative and thus bit 15 is set. this also means that bit 8 is set, so we can check that for borrow. 
        uint16_t z = (uint16_t)rs0[i] - (uint16_t)rs1[i] - borrow;
        dst[i] = z & UINT8_MAX;
        borrow = z >> BITS_IN_UINT8_T & 1;
    }
}

// compiles to constant-time code on any cpu with constant-time shifts.
void NOAHZK_variable_width_sub_constant_byte(void* real_dst, void* real_rs0, uint64_t k, const uint64_t width){
    uint8_t borrow = 0;
    uint8_t *dst = real_dst, *rs0 = real_rs0;

    for(uint64_t i = 0; i < width; i++){
// if subtraction overflows and requires a borrow, z (when interpreted as a signed type) is negative and thus bit 15 is set. this also means that bit 8 is set, so we can check that for borrow. 
        uint16_t z = (uint16_t)rs0[i] - (uint16_t)(i < sizeof(uint64_t)? (k >> (i*BITS_IN_UINT8_T)) & UINT8_MAX: 0) - borrow;
        dst[i] = z & UINT8_MAX;
        borrow = z >> BITS_IN_UINT8_T & 1;
    }
}

// compiles to constant-time code on any cpu with constant-time shifts.
void NOAHZK_variable_width_both_sub_byte(void* real_dst, void* real_rs0, void* real_rs1, const uint64_t width0, const uint64_t width1, const uint64_t width_result){
    uint8_t borrow = 0;
    uint8_t *dst = real_dst, *rs0 = real_rs0, *rs1 = real_rs1;

    for(uint64_t i = 0; i < width_result; i++){
// if subtraction overflows and requires a borrow, z (when interpreted as a signed type) is negative and thus bit 15 is set. this also means that bit 8 is set, so we can check that for borrow. 
// runs in constant time regardless of conditional statements because those statements are NOT DATA-DEPENDENT; widths of operands in NOAHZK are public.
        uint16_t z = (uint16_t)(i < width0? rs0[i]: 0) - (uint16_t)(i < width1? rs1[i]: 0) - borrow;
        dst[i] = z & UINT8_MAX;
        borrow = z >> BITS_IN_UINT8_T & 1;
    }
}

#endif
