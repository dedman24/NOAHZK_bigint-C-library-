/*
   NOAHZK_bigint reference source code package - reference C implementations

   Copyright 2025, dedmanwalking <dedmanwalking@proton.me>.  You may use this under the
   terms of the CC0 1.0 Universal license, linked below:
   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
*/

#ifndef NOAHZK_bigint_definitions_included
#define NOAHZK_bigint_definitions_included

#include "stdint.h"

typedef uint32_t NOAHZK_limb_t;
#define NOAHZK_LIMB_MAX UINT32_MAX

#define BITS_IN_UINT64_T    64
#define BITS_IN_UINT32_T    32
#define BITS_IN_UINT8_T     8
#define BITS_IN_NOAHZK_LIMB (sizeof(NOAHZK_limb_t)*BITS_IN_UINT8_T)

#define NOAHZK_convert_from_bits_to_bytes(x) (((x)/BITS_IN_UINT8_T) + ((x)%BITS_IN_UINT8_T != 0))

#define NOAHZK_MAX(x, y) ((x) > (y)? (x): (y))
#define NOAHZK_MIN(x, y) ((x) < (y)? (x): (y))
#define NOAHZK_SWP(x, y) (x) = (x) ^ (y); (y) = (x) ^ (y); (x) = (x) ^ (y)
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE(x)     ((x). width*sizeof(NOAHZK_limb_t))
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR(x) ((x)->width*sizeof(NOAHZK_limb_t))
#define NOAHZK_GET_WIDTH_FROM_VAR_WIDTH_TYPE_PTR_BITS(x) ((x)->width*BITS_IN_NOAHZK_LIMB)

#define NOAHZK_variable_width_var_INITIALIZER 0, NULL

struct NOAHZK_variable_width_var{
    uint64_t width;
    NOAHZK_limb_t* arr;
};

#endif
