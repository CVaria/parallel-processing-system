/*
 *  dmv_gpu.cu -- Template for DMV GPU kernels
 *
 *  Copyright (C) 2010-2013, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2010-2013, Vasileios Karakasis
 */
#include <stdio.h>
#include "dmv.h"

/*
 *  Utility function to get the thread ID within the
 *  global working space.
 */
__device__ int get_global_tid()
{
    return (gridDim.x*blockIdx.y + blockIdx.x)*blockDim.x*blockDim.y +
        blockDim.x*threadIdx.y + threadIdx.x;
}

/*
 *  Utility function to get the thread ID within the
 *  local/block working space.
 */
__device__ int get_local_tid()
{
    return blockDim.x*threadIdx.y + threadIdx.x;
}

/*
 *  Naive kernel
 */
__global__ void dmv_gpu_naive(const value_t *a, const value_t *x, value_t *y,
                              size_t n)
{
    int i, j, a_j, limitj;
    value_t _yi = 0;

    j = blockIdx.x*blockDim.x;
    limitj = j + blockDim.x;
    i = threadIdx.x + blockIdx.y*blockDim.x;
    a_j = i*n + j;

    for (; j < limitj; ++j, ++a_j)
        _yi += a[a_j]*x[j];
    atomicAdd(&y[i], _yi);
}
/*
 *  Naive kernel 1-dim grid
 */
/*
__global__ void dmv_gpu_naive(const value_t *a, const value_t *x, value_t *y,
                              size_t n
{
   int tid = get_global_tid();
   size_t j, a_j;
   register value_t _yi = 0;
   if (tid < n) {
       for (j = 0, a_j=tid*n; j < n; ++j, ++a_j) {
           _yi += a[a_j]*x[j];
       }
       y[tid] = _yi;
   }
}
 */

/*
 *  Coalesced memory acceses
 */
__global__ void dmv_gpu_coalesced(const value_t *a, const value_t *x,
                                  value_t *y, size_t n)
{
    int i, j, a_j, limitj;
    value_t _yi = 0;

    j = blockIdx.x*blockDim.x;
    limitj = j + blockDim.x;
    i = threadIdx.x + blockIdx.y*blockDim.x;
    a_j = j*n + i;

    for (; j < limitj; ++j, a_j += n)
        _yi += a[a_j]*x[j];
    atomicAdd(&y[i], _yi);
}
/*
 *  Coalesced memory acceses 1-dim grid
 */
/*
__global__ void dmv_gpu_coalesced(const value_t *a, const value_t *x,
                                  value_t *y, size_t n)
{
   int tid = get_global_tid();
   size_t j, a_j;
   register value_t _yi = 0;
   if (tid < n) {
       for (j = 0, a_j=tid; j < n; ++j, a_j += n) {
           _yi += a[a_j]*x[j];
       }
       y[tid] = _yi;
   }
}
 */


/*
 *  Use of shared memory
 */
__global__ void dmv_gpu_shmem(const value_t *a, const value_t *x, value_t *y,
                              size_t n)
{
    int i, j, a_j, limitj;
    value_t _yi = 0;
    extern __shared__ value_t sh_x[];

    j = blockIdx.x*blockDim.x;
    limitj = j + blockDim.x;
    i = threadIdx.x + blockIdx.y*blockDim.x;
    a_j = j*n + i;

    sh_x[threadIdx.x] = x[j + threadIdx.x];
    __syncthreads();
    for (; j < limitj; ++j, a_j += n)
        _yi += a[a_j]*x[j];
    atomicAdd(&y[i], _yi);
}
/*
 *  Use of shared memory 1-dim grid
 */
/*
__global__ void dmv_gpu_shmem(const value_t *a, const value_t *x, value_t *y,
                              size_t n)
{
    int loc_tid = get_local_tid();
    int tid = get_global_tid();
    size_t i, j, a_j;
    register value_t _yi = 0;
    extern __shared__ value_t sh_x[];
    for (i = 0; i < n; i += blockDim.x) {
        sh_x[loc_tid] = x[i+loc_tid];
        __syncthreads();
        for (j = 0, a_j = tid + i*n; j < blockDim.x; ++j, a_j += n) {
            _yi += a[a_j]*sh_x[j];
        }
        __syncthreads();
    }
    y[tid] = _yi;
}
 */

