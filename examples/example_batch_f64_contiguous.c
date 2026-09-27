/*
 * Fast Walsh-Hadamard Transform - batch_f64_contiguous Example
 *
 * Demonstrates computing the spectrum of a small Boolean function and
 * reporting its best linear approximation.
 *
 * Copyright (C) 2025 Hosein Hadipour
 *
 * Author: Hosein Hadipour <hsn.hadipour@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <fwht.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef FWHT_WITH_CALIPER
#include <caliper/cali.h>
#endif

void
set_mat_values( double *mat, int nele, int ncols )
{
    int i = 0;
    for ( ; i < nele; ++i ) {
        mat[i] = ( ( (double)i ) / ( (double)10 * (double)ncols ) ) + (double)( nele - i );
    }
}

int main(void) {
    #ifdef FWHT_WITH_CALIPER
    cali_config_set("CALI_CALIPER_ATTRIBUTE_DEFAULT_SCOPE", "process");
    CALI_MARK_BEGIN("example");
    #endif
    printf("=================================================================\n");
    printf("FWHT Library - batch_f64_contiguous Example\n");
    printf("=================================================================\n\n");
    
    /* ------------------------------------------------------------ */
    /* Example 2: parallel batch transforms (f64)      */
    /* ------------------------------------------------------------ */
    printf("Example 2: fwht_batch_f64_contiguous\n");

    int      nrows        = (int)4096;
    int      ncols        = (int)10;
    int      num_elements = nrows * ncols;
    fwht_status_t   status       = FWHT_SUCCESS;
    fwht_config_t   config       = { .backend = FWHT_BACKEND_OPENMP, .num_threads = 6, .gpu_device = 0, .normalize = true };
    fwht_context_t *ctx          = fwht_create_context( &config );
    size_t          array_size   = (size_t)( num_elements * sizeof( double ) );
    double         *data         = NULL;

    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_BEGIN("example - allocation");
    #endif
    posix_memalign( (void **)&( data ), 64, array_size);
    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_END("example - allocation");
    #endif

    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_BEGIN("example - set");
    #endif
    set_mat_values( data, num_elements, ncols );
    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_END("example - set");
    #endif
    
    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_BEGIN("example - batch_f64_contiguous");
    #endif
    status = fwht_batch_f64_contiguous( ctx, data, nrows, ncols );
    fwht_destroy_context( ctx );
    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_END("example - batch_f64_contiguous");
    #endif
    
    free( data );
    data = NULL;
    
    printf("\n");
    printf("=================================================================\n");
    printf("All examples completed successfully!\n");
    printf("=================================================================\n");
    
    #ifdef FWHT_WITH_CALIPER
    CALI_MARK_END("example");
    #endif
    return 0;
}
