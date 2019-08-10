/*
  Tiled version of the Floyd-Warshall algorithm.
  command-line arguments: N, B
  N = size of graph
  B = size of tile
  works only when N is a multiple of B
*/

#include <cstdlib>
#include <iostream>

#include "tbb/partitioner.h"

#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/blocked_range2d.h"
#include "tbb/parallel_invoke.h"
#include "tbb/parallel_for.h"
#include "tbb/tick_count.h"
#include "util.h"

inline int min(int a, int b);
inline void FW(int **A, int K, int I, int J, int N);

int main(int argc, char **argv)
{

    if (argc != 6){
        fprintf(stdout, "Usage %s N B nThreads Gx Gy\n", argv[0]);
        exit(0);
    }

    const int N=atoi(argv[1]);
    const int B=atoi(argv[2]);
    const int nthreads=atoi(argv[3]);
    const int Gx=atoi(argv[4]);
    const int Gy=atoi(argv[5]);
    int **A;

    A = new int *[N];
    for ( int i = 0; i < N; i++ ) A[i] = new int [N];

    graph_init_random(A,-1,N,128*N);

#ifdef DEBUG
    int **CHECK;
    CHECK = new int *[N];
    for ( int i = 0; i < N; i++ ) CHECK[i] = new int [N];

    for ( int i = 0; i < N; i++)
        for ( int j = 0; j < N; j++ )
            CHECK[i][j] = A[i][j];
#endif

    tbb::tick_count tic,toc;
    tbb::task_scheduler_init init(nthreads);

#ifdef DEBUG
    tic = tbb::tick_count::now();
    for( int k = 0; k < N; k++ )
        for( int i = 0; i < N ; i++ )
           for( int j = 0; j < N ; j++ )
                CHECK[i][j]=min(CHECK[i][j], CHECK[i][k] + CHECK[k][j]);
    toc = tbb::tick_count::now();
    std::cout << "Serial : " << (toc-tic).seconds()
              << " seconds" << std::endl;
#endif

    tbb::auto_partitioner pt;
    tic = tbb::tick_count::now();
    for( int k = 0; k < N; k += B ){
        FW(A,k,k,k,B);

        tbb::parallel_invoke(
            [=]() {
            tbb::parallel_for(
                tbb::blocked_range<int>(0, k/B, Gx),
                [=](const tbb::blocked_range<int> &r) {
                    for( int i = r.begin(), i_end = r.end(); i < i_end; i++ ) {
                        FW(A,k,i*B,k,B);
                    }
                },
                pt
                );
            },

            [=]() {
            tbb::parallel_for(
                tbb::blocked_range<int>(0, (N-k-B)/B, Gx),
                [=](const tbb::blocked_range<int> &r) {
                    for( int i = r.begin(), i_end = r.end(); i < i_end; i++ )
                        FW(A,k,k+B+i*B,k,B);
                },
                pt
                );
            },

            [=]() {
            tbb::parallel_for(
                tbb::blocked_range<int>(0, k/B, Gy),
                [=](const tbb::blocked_range<int> &r) {
                    for( int j = r.begin(), j_end = r.end(); j < j_end; j++ )
                        FW(A,k,k,j*B,B);
                },
                pt
                );
            },

            [=]() {
            tbb::parallel_for(
                tbb::blocked_range<int>(0, (N-k-B)/B, Gy),
                [=](const tbb::blocked_range<int> &r) {
                    for( int j = r.begin(), j_end = r.end(); j < j_end; j++ )
                        FW(A,k,k,k+B+j*B,B);
                },
                pt
                );
            } );

        tbb::parallel_invoke(
            [=]() {
            tbb::parallel_for(
                tbb::blocked_range2d<int, int>(0, k/B, Gx, 0, k/B, Gy),
                [=](const tbb::blocked_range2d<int, int> &r) {
                    for( int i = r.rows().begin(), i_end = r.rows().end(); i < i_end; i++ )
                        for( int j = r.cols().begin(), j_end = r.cols().end(); j < j_end; j++ )
                            FW(A,k,i*B,j*B,B);
                },
                pt
                );
            },

            [=]() {
            tbb::parallel_for(
                tbb::blocked_range2d<int, int>(0, k/B, Gx, 0, (N-k-B)/B, Gy),
                [=](const tbb::blocked_range2d<int, int> &r) {
                    for( int i = r.rows().begin(), i_end = r.rows().end(); i < i_end; i++ )
                        for( int j = r.cols().begin(), j_end = r.cols().end(); j < j_end; j++ )
                            FW(A,k,i*B,k+B+j*B,B);
                },
                pt
                );
            },

            [=]() {
            tbb::parallel_for(
                tbb::blocked_range2d<int, int>(0, (N-k-B)/B, Gx, 0, k/B, Gy),
                [=](const tbb::blocked_range2d<int, int> &r) {
                    for( int i = r.rows().begin(), i_end = r.rows().end(); i < i_end; i++ )
                        for( int j = r.cols().begin(), j_end = r.cols().end(); j < j_end; j++ )
                            FW(A,k,k+B+i*B,j*B,B);
                },
                pt
                );
            },

            [=]() {
            tbb::parallel_for(
                tbb::blocked_range2d<int, int>(0, (N-k-B)/B, Gx, 0, (N-k-B)/B, Gy),
                [=](const tbb::blocked_range2d<int, int> &r) {
                    for( int i = r.rows().begin(), i_end = r.rows().end(); i < i_end; i++ )
                        for( int j = r.cols().begin(), j_end = r.cols().end(); j < j_end; j++ )
                            FW(A,k,k+B+i*B,k+B+j*B,B);
                },
                pt
                );
            } );
    }
    toc = tbb::tick_count::now();

    std::cout << "FW_TILED parallel for\nSize : " << N << "\nBlock size: " << B
              << "\nThreads : " << nthreads << "\nTime : " << (toc-tic).seconds()
              << " seconds" << std::endl;

#ifdef DEBUG
    for ( int i = 0; i < N; i++ )
        for ( int j = 0; j < N; j++ )
            if (A[i][j] != CHECK[i][j]) {
                std::cout << "Something went wrong" << std::endl;
                return 0;
            }

    std::cout << "Same result with serial algorithm" << std::endl;
    for ( int i = 0; i < N; i++ ) delete CHECK[i];
    delete CHECK;
#endif

    for ( int i = 0; i < N; i++ ) delete A[i];
    delete A;

    return 0;

}

inline int min(int a, int b)
{
    if(a<=b)return a;
    else return b;
}

inline void FW(int **A, int K, int I, int J, int N)
{
    for( int k = K; k < K+N; k++ )
        for( int i = I; i < I+N; i++ )
            for( int j = J; j < J+N; j++)
                A[i][j]=min(A[i][j], A[i][k]+A[k][j]);

}
