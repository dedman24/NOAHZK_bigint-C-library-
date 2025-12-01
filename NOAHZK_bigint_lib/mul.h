/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_mul_included
#define NOAHZK_bigint_mul_included

#include "stdint.h"
#include "definitions.h"
#include "logarithms.h"
#include "type.h"
#include "add.h"
#include "string.h"
#include "stdlib.h"

// not constant time, no clue on how one would implement this in constant time
void NOAHZK_variable_width_shift_right(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* src, uint64_t shamt){
    NOAHZK_limb_t dst_arr[dst->width];
    memset(dst_arr, 0, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(dst));
    if(shamt == 0) memcpy(dst_arr, dst->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(dst));
    else if(shamt != NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR_BITS(dst)){
        for(uint64_t i = NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR_BITS(dst) - 1; i >= shamt; i--)
            dst_arr[(i - shamt)/BITS_IN_NOAHZK_LIMB] |= (src->arr[i/BITS_IN_NOAHZK_LIMB] >> (i % BITS_IN_NOAHZK_LIMB) & 1) << ((i - shamt)%BITS_IN_NOAHZK_LIMB);
    }

    memcpy(dst->arr, dst_arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(dst));
}

// basically 4-mul version of Karatsuba algorithm
// had problems trying to implement 3-mul version
// constant-time 
void NOAHZK_variable_width_mul_byte(void* dst, void* rs0, void* rs1, uint64_t width0, uint64_t width1){
// handles common cases quickly
    if(width0 == 0 && width1 == 0) return;
    if(width0 == 0 || width1 == 0) return;
    if(width0 == sizeof(uint8_t) && width1 == sizeof(uint8_t)){
        *(uint16_t*)dst = *(uint8_t*)rs0 * *(uint8_t*)rs1; 
        return;
    }
    if(width0 == sizeof(uint8_t)){
        uint16_t product = 0;
        for(uint64_t i = 0; i < width1; i++){
            product += ((uint8_t*)rs1)[i] * *(uint8_t*)rs0;
            ((uint8_t*)dst)[i] += product;
            product >>= BITS_IN_UINT8_T;        // this way if dst == rs1 dst[i] isn't overwritten until after we're done with rs1[i]
        }
        return;
    }
    if(width1 == sizeof(uint8_t)){
        uint16_t product = 0;
        for(uint64_t i = 0; i < width0; i++){
            product = ((uint8_t*)rs0)[i] * *(uint8_t*)rs1;
            ((uint8_t*)dst)[i] += product;
            product >>= BITS_IN_UINT8_T;        // this way if dst == rs0 dst[i] isn't overwritten until after we're done with rs0[i]
        }
        return;
    }

    uint64_t n = width0/2, m = width1/2;

    uint64_t width_X1Y1 = width0-n + width1-m;
    uint8_t X1Y1[width_X1Y1];
    NOAHZK_variable_width_mul_byte(X1Y1, (uint8_t*)rs0 + n, (uint8_t*)rs1 + m, width0-n, width1-m);

    uint64_t width_X1Y0 = width0-n + m;
    uint8_t X1Y0[width_X1Y0];
    NOAHZK_variable_width_mul_byte(X1Y0, (uint8_t*)rs0 + n, rs1, width0-n, m);
    
    uint64_t width_X0Y1 = n + width1-m;
    uint8_t X0Y1[width_X0Y1];
    NOAHZK_variable_width_mul_byte(X0Y1, rs0, (uint8_t*)rs1 + m, n, width1-m);
    
    uint64_t width_X0Y0 = n + m;
    uint8_t X0Y0[width_X0Y0];
    NOAHZK_variable_width_mul_byte(X0Y0, rs0, rs1, n, m);
// HAS to be done at the end because, if we do this at the start and dst == rs0, then we'd be setting rs0 to 0, which is bad.
    memset(dst, 0, width0+width1);
    uint64_t width_dst = width0 + width1;
    NOAHZK_variable_width_add_with_byte_offset_byte(dst, X0Y0, X0Y1, width_X0Y0, width_X0Y1, width_dst, m);
    NOAHZK_variable_width_add_with_byte_offset_byte(dst, dst,  X1Y0, width_dst,  width_X1Y0, width_dst, n);
    NOAHZK_variable_width_add_with_byte_offset_byte(dst, dst,  X1Y1, width_dst,  width_X1Y1, width_dst, n+m);
}

void NOAHZK_variable_width_mul_constant_byte(void* dst, void* rs0, uint64_t k, uint64_t width0){
    uint64_t bytes_k = NOAHZK_min_bytecnt_var(k);
    NOAHZK_variable_width_mul_byte(dst, rs0, &k, width0, bytes_k);
}

// multiplies two variable width variables together, returns the result in dst
// constant time
void NOAHZK_variable_width_mul(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, struct NOAHZK_variable_width_var* rs1){
    dst->width = rs0->width + rs1->width;
    dst->arr = realloc(dst->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(dst));

    NOAHZK_variable_width_mul_byte(dst->arr, rs0->arr, rs1->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(rs0), NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(rs1));
}

void NOAHZK_variable_width_mul_constant(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, uint64_t k){
    uint64_t bytes_k = NOAHZK_min_bytecnt_var(k);
    dst->width = rs0->width + bytes_k;
    dst->arr = realloc(dst->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(dst));

    NOAHZK_variable_width_mul_byte(dst->arr, rs0->arr, &k, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(rs0), bytes_k);
}

void NOAHZK_variable_width_mul_both_constants(struct NOAHZK_variable_width_var* dst, uint64_t k0, uint64_t k1){
    uint64_t bytes_k0 = NOAHZK_min_bytecnt_var(k0);
    uint64_t bytes_k1 = NOAHZK_min_bytecnt_var(k1);

    dst->width = bytes_k0 + bytes_k1;
    dst->arr = realloc(dst->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(dst));

    NOAHZK_variable_width_mul_byte(dst->arr, &k0, &k1, bytes_k0, bytes_k1);
}

// dst = rs0*rs1*rs1
void NOAHZK_variable_width_mul_by_square(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, struct NOAHZK_variable_width_var* rs1){
    struct NOAHZK_variable_width_var* product = NOAHZK_init_variable_width_var(alloca(sizeof(struct NOAHZK_variable_width_var)), 0);
    NOAHZK_variable_width_mul(product, rs1, rs1);

    NOAHZK_variable_width_add_and_resize(dst, rs0, product);
    NOAHZK_destroy_variable_width_var(product, 0);
}

void NOAHZK_variable_width_square(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* src){
    NOAHZK_variable_width_mul(dst, src, src);
}

void NOAHZK_variable_width_square_constant(struct NOAHZK_variable_width_var* dst, uint64_t k){
    NOAHZK_variable_width_mul_both_constants(dst, k, k);
}

void NOAHZK_variable_width_mul_by_square_constant(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, uint64_t k){
    struct NOAHZK_variable_width_var* product = NOAHZK_init_variable_width_var(alloca(sizeof(struct NOAHZK_variable_width_var)), 0);
    NOAHZK_variable_width_square(product, k);

    NOAHZK_variable_width_add_and_resize(dst, rs0, product);
    NOAHZK_destroy_variable_width_var(product, 0);
}

// dst = rs0 * k**power, where power >= 0 (because it's an unsigned 64-bit integer)
void NOAHZK_variable_width_mul_by_constant_to_power_constant(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* rs0, uint64_t k, uint64_t power){
    struct NOAHZK_variable_width_var* product = NOAHZK_init_variable_width_var_constant(alloca(sizeof(struct NOAHZK_variable_width_var)), 1);
    for(uint64_t i = 0; i < power; i++); NOAHZK_variable_width_mul_constant(product, product, k);

    NOAHZK_variable_width_add_and_resize(dst, rs0, product);
    NOAHZK_destroy_variable_width_var(product, 0);
}

// dst += src*k
void NOAHZK_variable_width_madd_constant(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* src, uint64_t k){
    struct NOAHZK_variable_width_var* product = NOAHZK_init_variable_width_var(alloca(sizeof(struct NOAHZK_variable_width_var)), 0);
    NOAHZK_variable_width_mul_constant(product, src, k);

    NOAHZK_variable_width_add_and_resize(dst, dst, product);
    NOAHZK_destroy_variable_width_var(product, 0);
}

// not constant-time
void* NOAHZK_variable_width_nth_triangle_number(struct NOAHZK_variable_width_var* dst, uint64_t n){
    if(!dst) dst = NOAHZK_init_variable_width_var(NULL, 0);
    NOAHZK_variable_width_square_constant(dst, n);

    NOAHZK_variable_width_add_and_resize_constant(dst, dst, n);
    NOAHZK_variable_width_shift_right(dst, dst, 1);

    return dst;
}

#endif
