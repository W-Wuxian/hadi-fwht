fwht_i32_batch fwht_f64_batch is Using SIMD

/*
 * Vectorized batch WHT for int32 arrays.
 * Processes batch_size transforms of size n in parallel using SIMD.
 * 
 * Parameters:
 *   data_array - Array of pointers to transforms
 *   n          - Size of each transform (must be power of 2)
 *   batch_size - Number of transforms to process
 * 
 * Performance:
 *   - n ≤ 256:  3-5× faster than sequential (uses AVX2/NEON)
 *   - n > 256:  1.2-1.5× faster (memory-bound, less SIMD benefit)
 */
fwht_status_t fwht_i32_batch(int32_t** data_array, size_t n, size_t batch_size);

/*
 * Vectorized batch WHT for float64 arrays.
 * Same as fwht_i32_batch but for double precision.
 */
fwht_status_t fwht_f64_batch(double** data_array, size_t n, size_t batch_size);








/* ============================================================================
 * ADVANCED API - BATCH PROCESSING
 * 
 * Compute multiple WHTs in parallel (optimal for GPU).
 * All arrays must have the same size.
 * ============================================================================ */

/*
 * Batch transform: compute WHT for multiple arrays in parallel.
 * 
 * Parameters:
 *   ctx        - Context (use NULL for default)
 *   data_array - Array of pointers to data arrays
 *   n          - Size of each array (must be same for all)
 *   batch_size - Number of arrays to process
 * 
 * This is significantly faster than calling fwht_i32 in a loop,
 * especially on GPU where batch operations amortize transfer costs.
 */
fwht_status_t fwht_batch_i32(fwht_context_t* ctx, int32_t** data_array, 
                             size_t n, int batch_size);
fwht_status_t fwht_batch_f64(fwht_context_t* ctx, double** data_array,
                             size_t n, int batch_size);
