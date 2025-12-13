/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_type_included
#define NOAHZK_bigint_type_included

#include "definitions.h"
#include "logarithms.h"
#include "string.h"
#include "stdlib.h"

void* NOAHZK_variable_width_init(struct NOAHZK_variable_width_var* toinit, const uint64_t width_in_bytes){
    if(!toinit) toinit = malloc(sizeof(struct NOAHZK_variable_width_var));

    const uint64_t width = NOAHZK_SIZE_AS_ARR_OF_TYPE(width_in_bytes, sizeof(NOAHZK_limb_t));
    if(width){
        toinit->arr = calloc(sizeof(NOAHZK_limb_t), width);
        toinit->width = width;
    }
    else{
        toinit->arr = NULL;
        toinit->width = 0;
    }
    return toinit;
}

// initializes toinit to array "arr" of width (in bytes) "width_in_bytes", or returns ptr to struct NOAHZK_variable_width_var that holds array holding the same contents of arr
void* NOAHZK_variable_width_init_arr(struct NOAHZK_variable_width_var* toinit, void* arr, const uint64_t width_in_bytes){
    if(!toinit) toinit = malloc(sizeof(struct NOAHZK_variable_width_var));

    const uint64_t width = NOAHZK_SIZE_AS_ARR_OF_TYPE(width_in_bytes, sizeof(NOAHZK_limb_t));
    if(width){
        toinit->arr = calloc(sizeof(NOAHZK_limb_t), width);
        toinit->width = width;
        memcpy(toinit->arr, arr, width_in_bytes);
    }
    else{
        toinit->arr = NULL;
        toinit->width = 0;
    }
    return toinit;
}

// initializes toinit to k, or returns ptr to struct NOAHZK_variable_width_var that holds k
void* NOAHZK_variable_width_init_constant(struct NOAHZK_variable_width_var* toinit, uint64_t k){
    if(!toinit) toinit = malloc(sizeof(struct NOAHZK_variable_width_var));

    const uint64_t width_in_bits = NOAHZK_min_bitcnt_var(k);
    const uint64_t width = NOAHZK_SIZE_AS_ARR_OF_TYPE(width_in_bits, BITS_IN_NOAHZK_LIMB);
    if(width){
        toinit->arr = malloc(sizeof(NOAHZK_limb_t)*width);
        memcpy(toinit->arr, &k, sizeof(NOAHZK_limb_t)*width);

        toinit->width = width;
    }
    else{
        toinit->arr = NULL;
        toinit->width = 0;
    }
    return toinit;
}

void* NOAHZK_variable_width_copy(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* src){
    if(!dst) dst = malloc(sizeof(struct NOAHZK_variable_width_var));

    dst->width = src->width;
    dst->arr = malloc(NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(src));

    memcpy(dst->arr, src->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(src));

    return dst;
}

void* NOAHZK_variable_width_copy_to_arr(void* dst, uint64_t width_dst, struct NOAHZK_variable_width_var* src){
    if(!width_dst) width_dst = NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(src);
    if(!dst) dst = malloc(width_dst);

    memcpy(dst, src->arr, width_dst);

    return dst;
}

// moves fields from src to dst, clears src. doesn't free it.
// allocates dst if NULL is passed
void* NOAHZK_variable_width_move(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* src){
    if(!dst) dst = malloc(sizeof(struct NOAHZK_variable_width_var));

    dst->width = src->width; src->width = 0;
    dst->arr = src->arr; src->arr = NULL;

    return dst;
}

void NOAHZK_variable_width_destroy(struct NOAHZK_variable_width_var* todestroy, int freeptr){
    if(todestroy->arr){
        memset(todestroy->arr, 0, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(todestroy));
        memset(&todestroy->width, 0, sizeof(todestroy->width));
        free(todestroy->arr);
    } 
    if(freeptr) free(todestroy);
}

#endif
