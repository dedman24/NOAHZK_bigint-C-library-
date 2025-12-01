# NOAHZK_bigint
NOAHZK_bigint is a bigint library written in C that implements the following operations on arbitrarily-long integers in constant-time:
  - unsigned addition
  - unsigned subtraction
  - unsigned multiplication (using 4-mul variant of the Karatsuba algorithm)
  - ceil logarithm base 2 of (said integer + 1)
It also implements ceil logarithm base 2 of an uint64_t in constant time.

## usage
Just download the whole NOAHZK_bigint library and include NOAHZK_bigint_lib/NOAHZK_bigint.h in your project.

## licenses
This work is released into the public domain with [CC0 1.0](https://github.com/dedman24/numhash0/blob/main/LICENSE).
