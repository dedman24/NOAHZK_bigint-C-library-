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

void* NOAHZK_init_variable_width_var(struct NOAHZK_variable_width_var* toinit, const uint64_t width_in_bytes){
    if(!toinit) toinit = malloc(sizeof(struct NOAHZK_variable_width_var));

    const uint64_t width = (width_in_bytes/sizeof(NOAHZK_limb_t)) + (width_in_bytes%sizeof(NOAHZK_limb_t) != 0);
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

// initialises toinit to k, or returns ptr to struct NOAHZK_variable_width_var that holds k
void* NOAHZK_init_variable_width_var_constant(struct NOAHZK_variable_width_var* toinit, uint64_t k){
    if(!toinit) toinit = malloc(sizeof(struct NOAHZK_variable_width_var));

    const uint64_t width_in_bits = NOAHZK_min_bitcnt_var(k);
    const uint64_t width = (width_in_bits/BITS_IN_NOAHZK_LIMB) + (width_in_bits%BITS_IN_NOAHZK_LIMB != 0);
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

void* NOAHZK_copy_variable_width_var(struct NOAHZK_variable_width_var* dst, struct NOAHZK_variable_width_var* src){
    if(!dst) dst = malloc(sizeof(struct NOAHZK_variable_width_var));

    dst->width = src->width;
    dst->arr = malloc(NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(src));

    memcpy(dst->arr, src->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(src));

    return dst;
}

void NOAHZK_destroy_variable_width_var(struct NOAHZK_variable_width_var* todestroy, int freeptr){
    if(todestroy->arr){
        secure_erase(todestroy->arr, NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(todestroy));
        secure_erase(&todestroy->width, sizeof(todestroy->width));
        free(todestroy->arr);
    } 
    if(freeptr) free(todestroy);
}

#endif
