/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_sub_included
#define NOAHZK_bigint_sub_included

#include "definitions.h"    // NOAHZK variable-width type
#include "stdint.h"         // integer types

// BE WARY OF HOW C CONVERTS BETWEEN UNSIGNED TYPES!
// it's not the fault of C moreso I'm just dumb

// negates src according to the contents of op in constant time.
// returns two's complement of src unsigned if op is 1, otherwise src unmodified if it is 0.
NOAHZK_limb_t NOAHZK_variable_width_conditionally_invert(const NOAHZK_limb_t src, NOAHZK_op_t op){
    return src ^ -(NOAHZK_limb_t)op;
}

// dst = rs0 + or - (by virtue of op) rs1; constant-time regardless of what the value of 'op' is. 
void NOAHZK_variable_width_add_or_sub(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const NOAHZK_variable_width_t* const rs1, NOAHZK_op_t op){
    NOAHZK_limb_t carry = op;
// in LISP (used as pseudocode here), subtraction may be defined as (+ being addition, ~ being bitwise negation)
// (define (- a b) (+ a (~ b) 1))    
// we want to invert b if op is 1, and add opposite along with it, in which case we treat carry specially.
// to negate b we have to:
//      extend b to occupy the whole byte somehow and XOR it with b
// (define (cond-not x op) (^ x (- op)))
// (define (plus-or-minus a b op) (+ a (cond-not b op) op))

    for(size_t i = 0; i < dst->width; i++){
        const NOAHZK_limb_t rs1_limb = NOAHZK_variable_width_get_arr(rs1->arr, rs1->width, rs1->sign, i);
        const NOAHZK_limb_t negated_rs1_limb = NOAHZK_variable_width_conditionally_invert(rs1_limb, op);

        NOAHZK_expanded_limb_t z = NOAHZK_variable_width_get_arr(rs0->arr, rs0->width, rs0->sign, i) + (NOAHZK_expanded_limb_t)negated_rs1_limb + (NOAHZK_expanded_limb_t)carry;
        dst->arr[i] = z & NOAHZK_LIMB_MAX;
        carry = NOAHZK_variable_width_get_out(z);
    }

    NOAHZK_variable_width_update_sign(dst);
}

// dst = rs0 + or - (by virtue of op) k; constant-time regardless of what the value of 'op' is.
void NOAHZK_variable_width_add_or_sub_constant(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const uint64_t k, NOAHZK_op_t op){
    NOAHZK_limb_t carry = op;

    for(size_t i = 0; i < dst->width; i++){
        const NOAHZK_limb_t k_limb = NOAHZK_get_section_from_var(k, NOAHZK_LIMB_MAX, i, NOAHZK_limb_t);
        const NOAHZK_limb_t negated_k_limb = NOAHZK_variable_width_conditionally_invert(k_limb, op);

        const NOAHZK_expanded_limb_t z = NOAHZK_variable_width_get_arr(rs0->arr, rs0->width, rs0->sign, i) + (NOAHZK_expanded_limb_t)negated_k_limb + (NOAHZK_expanded_limb_t)carry;
        dst->arr[i] = z & NOAHZK_LIMB_MAX;
        carry = NOAHZK_variable_width_get_out(z);
    }

    NOAHZK_variable_width_update_sign(dst);
}

NOAHZK_limb_t NOAHZK_variable_width_sub_primitive(NOAHZK_limb_t* const dst, const NOAHZK_limb_t* const rs0, const NOAHZK_limb_t* const rs1, const size_t width_dst, const size_t width0, const size_t width1, const NOAHZK_limb_t sign0, const NOAHZK_limb_t sign1){
    NOAHZK_limb_t carry = 1;
// TODO: SEE IF WORKS WITH PARENTHESES!
    for(size_t i = 0; i < width_dst; i++){
        const NOAHZK_expanded_limb_t z = NOAHZK_variable_width_get_arr(rs0, width0, sign0, i) + NOAHZK_variable_width_get_arr_inverted(rs1, width1, sign1, i) + (NOAHZK_expanded_limb_t)carry;
        dst[i] = z & NOAHZK_LIMB_MAX;
        carry = NOAHZK_variable_width_get_out(z);
    }
    return carry;
}

NOAHZK_limb_t NOAHZK_variable_width_sub_constant_primitive(NOAHZK_limb_t* const dst, const NOAHZK_limb_t* const rs0, const uint64_t k, const size_t width_dst, const size_t width0, const NOAHZK_limb_t sign0){
    NOAHZK_limb_t carry = 1;

    for(size_t i = 0; i < width_dst; i++){
        const NOAHZK_expanded_limb_t z = NOAHZK_variable_width_get_arr(rs0, width0, sign0, i) + (NOAHZK_expanded_limb_t)NOAHZK_get_section_from_var_inverted(k, NOAHZK_LIMB_MAX, i, NOAHZK_limb_t) + (NOAHZK_expanded_limb_t)carry;
        dst[i] = z & NOAHZK_LIMB_MAX;
        carry = NOAHZK_variable_width_get_out(z);
    }
    return carry;
}

void NOAHZK_variable_width_sub(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const NOAHZK_variable_width_t* const rs1){
    NOAHZK_variable_width_sub_primitive(dst->arr, rs0->arr, rs1->arr, dst->width, rs0->width, rs1->width, rs0->sign, rs1->sign);
    NOAHZK_variable_width_update_sign(dst);
}

void NOAHZK_variable_width_sub_constant(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const uint64_t k){
    NOAHZK_variable_width_sub_constant_primitive(dst->arr, rs0->arr, k, dst->width, rs0->width, rs0->sign);
    NOAHZK_variable_width_update_sign(dst);
}

// for sub ops where dst may have a size of 0, initialises dst's width to the width of the smallest src operand.
void NOAHZK_variable_width_sub_and_resize(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const NOAHZK_variable_width_t* const rs1){
    NOAHZK_variable_width_resize_to_largest(dst, rs0->width, rs1->width);
    const NOAHZK_limb_t cout = NOAHZK_variable_width_sub_primitive(dst->arr, rs0->arr, rs1->arr, dst->width, rs0->width, rs1->width, rs0->sign, rs1->sign);
    NOAHZK_variable_width_handle_carry(dst, rs0->sign, rs1->sign ^ 1, cout);
}

void NOAHZK_variable_width_sub_and_resize_constant(NOAHZK_variable_width_t* const dst, const NOAHZK_variable_width_t* const rs0, const uint64_t k){
    NOAHZK_variable_width_resize_to_largest(dst, rs0->width, sizeof(k)/sizeof(NOAHZK_limb_t));
    const NOAHZK_limb_t cout = NOAHZK_variable_width_sub_constant_primitive(dst->arr, rs0->arr, k, dst->width, rs0->width, rs0->sign);
    NOAHZK_variable_width_handle_carry(dst, rs0->sign, 1, cout);        // constants that are subtracted are ALWAYS negative
}

#endif
