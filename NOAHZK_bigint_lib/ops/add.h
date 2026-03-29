/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_add_included
#define NOAHZK_bigint_add_included

#include "definitions.h"    // NOAHZK variable-width type
#include "stdint.h"         // integer types
#include "stdlib.h"         // dynamic memory operations
#include "string.h"         // memset

// do not define any of these as restrict; it is valid for them to alias

NOAHZK_limb_t NOAHZK_variable_width_add_primitive(NOAHZK_limb_t* const dst, const NOAHZK_limb_t* const rs0, const NOAHZK_limb_t* const rs1, const size_t width_dst, const size_t width0, const size_t width1, const NOAHZK_limb_t sign0, const NOAHZK_limb_t sign1){
    NOAHZK_limb_t carry = 0;

    for(size_t i = 0; i < width_dst; i++){
        NOAHZK_expanded_limb_t z = NOAHZK_variable_width_get_arr(rs0, width0, sign0, i) + NOAHZK_variable_width_get_arr(rs1, width1, sign1, i) + carry;
        dst[i] = z & NOAHZK_LIMB_MAX;
        carry = NOAHZK_variable_width_get_out(z);
    }

    return carry;
}

NOAHZK_limb_t NOAHZK_variable_width_add_constant_primitive(NOAHZK_limb_t* const dst, const NOAHZK_limb_t* const rs0, const uint64_t k, const size_t width_dst, const size_t width0, const NOAHZK_limb_t sign0){
    NOAHZK_limb_t carry = 0;

    for(size_t i = 0; i < width_dst; i++){
        NOAHZK_expanded_limb_t z = NOAHZK_variable_width_get_arr(rs0, width0, sign0, i) + (NOAHZK_expanded_limb_t)NOAHZK_get_section_from_var(k, NOAHZK_LIMB_MAX, i, NOAHZK_limb_t) + carry;
        dst[i] = z & NOAHZK_LIMB_MAX;
        carry = NOAHZK_variable_width_get_out(z);
    }

    return carry;
}

// compiles to constant-time code on any cpu with constant-time shifts.
// used for proving, so having it be constant-time is integral
void NOAHZK_variable_width_add(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const NOAHZK_variable_width_t* const rs1){
    NOAHZK_variable_width_add_primitive(dst->arr, rs0->arr, rs1->arr, dst->width, rs0->width, rs1->width, rs0->sign, rs1->sign);
    NOAHZK_variable_width_update_sign(dst);
}

void NOAHZK_variable_width_add_constant(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const uint64_t k){
    NOAHZK_variable_width_add_constant_primitive(dst->arr, rs0->arr, k, dst->width, rs0->width, rs0->sign);
    NOAHZK_variable_width_update_sign(dst);
}

void NOAHZK_variable_width_add_and_resize(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const NOAHZK_variable_width_t* const rs1){
    NOAHZK_variable_width_resize_to_largest(dst, rs0->width, rs1->width);
    const NOAHZK_limb_t cout = NOAHZK_variable_width_add_primitive(dst->arr, rs0->arr, rs1->arr, dst->width, rs0->width, rs1->width, rs0->sign, rs1->sign);
    NOAHZK_variable_width_handle_carry(dst, rs0->sign, rs1->sign, cout);
}

void NOAHZK_variable_width_add_and_resize_constant(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const uint64_t k){
    NOAHZK_variable_width_resize_to_largest(dst, rs0->width, sizeof(k)/sizeof(NOAHZK_limb_t));
    const NOAHZK_limb_t cout = NOAHZK_variable_width_add_constant_primitive(dst->arr, rs0->arr, k, dst->width, rs0->width, rs0->sign);
    NOAHZK_variable_width_handle_carry(dst, rs0->sign, 0, cout);     // constants that are added are ALWAYS positive
}

void NOAHZK_variable_width_add_with_byte_offset_byte(void* const real_dst, const void* const real_rs0, const void* const real_rs1, const size_t width0, const size_t width1, const size_t width_result, const size_t byte_offset){
    uint8_t carry = 0;
    uint8_t* const dst = real_dst;
    const uint8_t *const rs0 = real_rs0, *const rs1 = real_rs1;

    for(size_t i = 0; i < width_result; i++){
        uint16_t z = (uint16_t)(i < width0? rs0[i]: 0) + (i < byte_offset? 0: (i-byte_offset < width1? (uint16_t)rs1[i-byte_offset]: 0)) + carry;

        dst[i] = z & UINT8_MAX;
        carry = z >> BITS_IN_UINT8_T & 1;
    }
}

void NOAHZK_variable_width_add_byte(void* const real_dst, const void* const real_rs0, const void* const real_rs1, const size_t width0, const size_t width1, const size_t width_result){
    uint8_t carry = 0;
    uint8_t *dst = real_dst;
    const uint8_t *rs0 = real_rs0, *rs1 = real_rs1;

    for(size_t i = 0; i < width_result; i++){
        uint16_t z = (uint16_t)(i < width0? rs0[i]: 0) + (uint16_t)(i < width1? rs1[i]: 0) + carry;
        dst[i] = z & UINT8_MAX;
        carry = z >> BITS_IN_UINT8_T & 1;
    }
}

#endif
