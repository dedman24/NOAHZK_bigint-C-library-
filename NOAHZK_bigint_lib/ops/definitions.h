/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_definitions_included
#define NOAHZK_bigint_definitions_included

#include "stdint.h"     // integer types
#include "stdlib.h"     // dynamic memory handling
#include "string.h"     // memset, memcpy & so on
#include "stdio.h"      // DEBUG
#include "../../../utils.h"     // DEBUG

#define NOAHZK_BIGINT_OP_ADD 0
#define NOAHZK_BIGINT_OP_SUB 1

typedef uint32_t NOAHZK_limb_t;
typedef const NOAHZK_limb_t NOAHZK_op_t;        // NOAHZK_bigint relies on NOAHZK_op_t secretely being NOAHZK_limb_t, so do not change this!
typedef uint64_t NOAHZK_expanded_limb_t;        // supposed to be smallest type larger than NOAHZK_limb_t
#define NOAHZK_LIMB_MAX UINT32_MAX

#define BITS_IN_UINT64_T    64
#define BITS_IN_UINT32_T    32
#define BITS_IN_UINT8_T     8
#define BITS_IN_NOAHZK_LIMB (sizeof(NOAHZK_limb_t)*BITS_IN_UINT8_T)

#define NOAHZK_convert_from_bits_to_bytes(x) (((x)/BITS_IN_UINT8_T) + ((x)%BITS_IN_UINT8_T != 0))
// gets section from variable; say variable is 0x01234567; NOAHZK_get_section_from_var(variable, UINT8_MAX, 0, uint8_t) will return a value of the same type as var holding 0x67
#define NOAHZK_get_section_from_var(var, section_mask, section, section_type)          ((section) < sizeof(var)/sizeof(section_type)? (section_type)((var) >> (section)*sizeof(section_type)*BITS_IN_UINT8_T & (section_mask)): 0)
#define NOAHZK_get_section_from_var_inverted(var, section_mask, section, section_type) (~NOAHZK_get_section_from_var(var, section_mask, section, section_type))

#define NOAHZK_MAX(x, y) ((x) > (y)? (x): (y))
#define NOAHZK_MIN(x, y) ((x) < (y)? (x): (y))
#define NOAHZK_SWP(x, y) (x) = (x) ^ (y); (y) = (x) ^ (y); (x) = (x) ^ (y)
// I think this is portable, compilers will optimize this anyways.
#define NOAHZK_SWP_PTR(x, y)    {                       \
                                    void* temp = x;     \
                                    x = (void*)y;       \
                                    y = temp;           \
                                }
#define NOAHZK_SIZE_AS_ARR_OF_TYPE(size, size_type) (((size)/size_type) + ((size)%size_type != 0))
// gets width in limbs of a variable of width x
#define NOAHZK_GET_LIMB_WIDTH_FROM_INT(x)           ((x)/sizeof(NOAHZK_limb_t) + ((x)%sizeof(NOAHZK_limb_t) != 0))
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE(x)     ((x). width*sizeof(NOAHZK_limb_t))
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(x) ((x)->width*sizeof(NOAHZK_limb_t))
// width of said var-width type in number of limbs is passed directly
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_INT(x) ((x)       *sizeof(NOAHZK_limb_t))
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR_BITS(x) ((x)->width*BITS_IN_NOAHZK_LIMB)

#define NOAHZK_variable_width_INITIALISER {0, NULL, 0}

typedef struct{
    size_t width;
    NOAHZK_limb_t* arr;
    NOAHZK_limb_t sign; 
} NOAHZK_variable_width_t;

// SIGN HANDLING RULES:
//      when an add_and_resize overflows, it sets the extra space to whatever's appropriate:
//          ADD: (1 == negative, 0 == positive)
//              A B | CARRY OUT (cout == 1)  | NO CARRY OUT (cout == 0)
//              0 0 | positive, extend by c  | positive, do nothing
//              0 1 | positive, do nothing   | negative, do nothing
//              1 0 | positive, do nothing   | negative, do nothing
//              1 1 | extend by -c if dst is positive (highest bit is 0) & set dst to negative, otherwise do nothing. 
//              if(A && B) sign = NOAHZK_variable_width_get_sign(dst);
//              else sign = (A | B) & (cout ^ 1);
//      when a sub_and_resize overflows, it sets the extra space to whatever's appropriate: (TODO: THIS TABLE'S 100% WRONG)
//          SUB: add with B inverted.
//              A B | CARRY OUT (cout == 1)  | NO CARRY OUT (cout == 0)
//              0 0 | positive, do nothing   | negative, do nothing
//              0 1 | positive, extend by c  | positive, do nothing
//              1 0 | extend by -c if dst is positive (highest bit is 0) & set dst to negative, otherwise do nothing. 
//              1 1 | positive, do nothing   | negative, do nothing
//      in both ADD and SUB, sign of dst is set by looking at dst

// occasionally useful 

// guaranteed to be constant-time

// gets carry/borrow
NOAHZK_limb_t NOAHZK_variable_width_get_out(const NOAHZK_expanded_limb_t z){
    return z >> BITS_IN_NOAHZK_LIMB & 1;
}

// guaranteed to be constant-time as long as src->width is constant

NOAHZK_expanded_limb_t NOAHZK_variable_width_get_arr(const NOAHZK_limb_t* const arr, const size_t width, const NOAHZK_limb_t sign, const size_t index){
// has to cast to NOAHZK_limb_t due to integer promotion & how different machines may handle it.
// not really relevant if int is 32-bit and NOAHZK_limb_t is defined as an uint32_t, but if one of these two changes, then functions might break.
    return (NOAHZK_limb_t)(index < width? arr[index]: -sign);
}

NOAHZK_expanded_limb_t NOAHZK_variable_width_get_arr_inverted(const NOAHZK_limb_t* const arr, const size_t width, const NOAHZK_limb_t sign, const size_t index){
    return (NOAHZK_limb_t)~(index < width? arr[index]: -sign);
}

NOAHZK_limb_t NOAHZK_variable_width_get_sign(const NOAHZK_variable_width_t* const src){
    return src->arr[src->width - 1] >> (BITS_IN_NOAHZK_LIMB - 1) & 1;
}

void NOAHZK_variable_width_update_sign(NOAHZK_variable_width_t* const src){
    src->sign = NOAHZK_variable_width_get_sign(src);
}

// not guaranteed to be constant-time even under assumptions aboce 

void NOAHZK_variable_width_resize_to_largest(NOAHZK_variable_width_t* const dst, const size_t width0, const size_t width1){
    const size_t largest_width = NOAHZK_MAX(width0, width1); 
// expands dst to size of largest operand, initializing new space to 0 
    if(dst->width < largest_width){
        dst->arr = realloc(dst->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_INT(largest_width));
        memset(dst->arr + dst->width, -dst->sign, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_INT(largest_width - dst->width));
        dst->width = largest_width;
    }
}

// specifically for mul ops
// where width0, width1 are widths in number of limbs
size_t NOAHZK_variable_width_resize_to_sum(NOAHZK_variable_width_t* const dst, const size_t width0, const size_t width1){
    const size_t new_width = width0 + width1;
    dst->arr = realloc(dst->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_INT(new_width));
    return new_width;
}

void NOAHZK_variable_width_resize_by_one(NOAHZK_variable_width_t* const toresize, const NOAHZK_limb_t toput){
    toresize->arr = realloc(toresize->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_INT(toresize->width + 1));
    toresize->arr[toresize->width] = toput;
    toresize->width++;
}

void NOAHZK_variable_width_print(const NOAHZK_variable_width_t* const var){
    printf("%u %lu ", var->sign, var->width); print_hex_reverse(var->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(var), '\n');
}

void NOAHZK_variable_width_print_nonewline(const NOAHZK_variable_width_t* const var){
    printf("%u %lu ", var->sign, var->width); print_hex_reverse(var->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(var), ' ');
}

void NOAHZK_variable_width_handle_carry(NOAHZK_variable_width_t* const dst, const NOAHZK_limb_t rs0_sign, const NOAHZK_limb_t rs1_sign, const NOAHZK_limb_t cout){
// this is to allow unsigned & signed resize ops
// see table above
    if(rs0_sign && rs1_sign) dst->sign = NOAHZK_variable_width_get_sign(dst);
    else dst->sign = (rs0_sign | rs1_sign) & (cout ^ 1);

    if(cout && rs0_sign == rs1_sign){
        if(rs0_sign && !dst->sign){
            NOAHZK_variable_width_resize_by_one(dst, -cout);
            dst->sign = 1;
        } 
        else if(!rs0_sign) NOAHZK_variable_width_resize_by_one(dst, cout);
    }
}

#endif
