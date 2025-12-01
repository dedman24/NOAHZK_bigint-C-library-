# NOAHZK_bigint
NOAHZK_bigint is a bigint library written in C that implements the following operations on arbitrarily-long integers in constant-time:
  - unsigned addition
  - unsigned subtraction
  - unsigned multiplication (using 4-mul variant of the Karatsuba algorithm)
  - ceil logarithm base 2 of (said integer + 1)

It also implements ceil logarithm base 2 of an uint64_t in constant time.
Both ceil_log2 (called NOAHZK_ceil_log2) and ceil_log2(x+1) (called NOAHZK_min_bitcnt_var for the uint64_t version and NOAHZK_variable_width_min_bitcnt for the variable width type version) use GCC's __builtin* family of functions.

## usage
Just download the [NOAHZK_bigint_lib](https://github.com/dedman24/NOAHZK_bigint-c-library-/blob/main/NOAHZK_bigint_lib) folder and include [NOAHZK_bigint_lib/NOAHZK_bigint.h](https://github.com/dedman24/NOAHZK_bigint-c-library-/blob/main/NOAHZK_bigint_lib/noahzk_bigint.h) in your project.
The bigint type itself is defined in [definitions.h](https://github.com/dedman24/NOAHZK_bigint-c-library-/blob/main/NOAHZK_bigint_lib/definitions.h) as _struct NOAHZK_variable_width_var_. The functions to initialize it are defined in [type.h](https://github.com/dedman24/NOAHZK_bigint-c-library-/blob/main/NOAHZK_bigint_lib/type.h) and they are:
  - NOAHZK_init_variable_width_var            ~ initializes variable-width var (or creates a new one if none is passed) so that it is a certain amount of bytes wide (allowed to be 0). Its contents are initialized to 0.
  - NOAHZK_init_variable_width_var_constant   ~ initializes variable-width var (or creates a new one if none is passed) to the uint64_t passed to it, so it uses the minimum number of limbs necessary to represent it.
  - NOAHZK_copy_variable_width_var            ~ copies a variable-width var to another, or creates a new, distinct copy if none to copy to is passed.

To free any variable-width var, just call NOAHZK_destroy_variable_width_var, with, as first argument, the variable to free, and as second, whether to free only the buffer it occupies in memory and erase it (0) or whether to also free the variable (1).

## licenses
This work is released into the public domain with [CC0 1.0](https://github.com/dedman24/NOAHZK_bigint-c-library-/blob/main/LICENSE).
