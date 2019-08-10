/*
 Parallel implementation of the standard Floyd-Warshall Algorithm
*/

#include <cstdlib>
#include <cstdlib>
#include <iostream>
#include <tbb/tbb.h>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_invoke.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "tbb/partitioner.h"
#include "util.h"

inline int min(int a, int b);

int main(int argc, char **argv)
{

    if (argc != 3) {
        fprintf(stdout,"Usage: %s N nthreads\n", argv[0]);
        exit(0);
    }

    int **A;

    const int N = atoi(argv[1]);
    const int nthreads = atoi(argv[2]);

    tbb::task_scheduler_init init(nthreads);

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

    tic = tbb::tick_count::now();
    tbb::auto_partitioner pt;
    for ( int k = 0; k < N; k++ ) {
        tbb::parallel_for(
            tbb::blocked_range<size_t>(0,N),
            [=](const tbb::blocked_range<size_t> &r) {
                for ( size_t i = r.begin(); i < r.end(); ++i )
                    for ( int j = 0; j < N; ++j )
                        A[i][j]=min(A[i][j], A[i][k] + A[k][j]);;
            },
            pt
            );
    }
    toc = tbb::tick_count::now();
    std::cout << "FW Parallel lambda 1d\nSize : " << N << "\nThreads : " << nthreads << "\nTime : " << (toc-tic).seconds()
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

