/***************************************************************************
Copyright (c) 2022, The OpenBLAS Project
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the OpenBLAS project nor the names of
its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "common.h"

#if !defined(DOUBLE)
#define VSETVL(n) vsetvl_e32m4(n)
#define FLOAT_V_T vfloat32m4_t
#define VLEV_FLOAT vle32_v_f32m4
#define VSEV_FLOAT vse32_v_f32m4
#define VSSEG2_FLOAT vsseg2e32_v_f32m4
#else
#define VSETVL(n) vsetvl_e64m4(n)
#define FLOAT_V_T vfloat64m4_t
#define VLEV_FLOAT vle64_v_f64m4
#define VSEV_FLOAT vse64_v_f64m4
#define VSSEG2_FLOAT vsseg2e64_v_f64m4
#endif

// Optimizes the implementation in ../generic/gemm_ncopy_2.c

int CNAME(BLASLONG m, BLASLONG n, IFLOAT *a, BLASLONG lda, IFLOAT *b)
{
    BLASLONG i, j;
    IFLOAT *a_offset, *a_offset1, *a_offset2;
    IFLOAT *b_offset;
    FLOAT_V_T v1, v2;
    size_t vl;

    //fprintf(stderr, "gemm_ncopy_2 m=%ld n=%ld lda=%ld\n", m, n, lda); // KU

    a_offset = a;
    b_offset = b;

    for(j = (n >> 1); j > 0; j--) {

        a_offset1 = a_offset;
        a_offset2 = a_offset + lda;
        a_offset += 2 * lda;

        for(i = m; i > 0; i -= vl) {
            vl = VSETVL(i);

            v1 = VLEV_FLOAT(a_offset1, vl);
            v2 = VLEV_FLOAT(a_offset2, vl);
            VSSEG2_FLOAT(b_offset, v1, v2, vl);

	        a_offset1 += vl;
	        a_offset2 += vl;
	        b_offset += vl*2;
        }
    }

    if (n & 1) {

        for(i = m; i > 0; i -= vl) {
            vl = VSETVL(i);

            v1 = VLEV_FLOAT(a_offset, vl);
            VSEV_FLOAT(b_offset, v1, vl);

	        a_offset += vl;
	        b_offset += vl;
        }
	}

    return 0;
}
