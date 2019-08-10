/*
  Tiled version of the Floyd-Warshall algorithm.
  command-line arguments: N, B
  N = size of graph
  B = size of tile
  works only when N is a multiple of B
*/

#include <cstdlib>
#include <iostream>

#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "util.h"

inline int min(int a, int b);
inline void FW(int **A, int K, int I, int J, int N);

int main(int argc, char **argv)
{

    if (argc != 4){
        fprintf(stdout, "Usage %s N B\n", argv[0]);
        exit(0);
    }

    const int N=atoi(argv[1]);
    const int B=atoi(argv[2]);
    const int nthreads=atoi(argv[3]);
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

    tic = tbb::tick_count::now();
    for( int k = 0; k < N; k += B ){
        FW(A,k,k,k,B);

        tbb::task_group g1, g2;

        for( int i = 0; i < k; i += B ) {
            g1.run ( [=]{ FW(A,k,i,k,B); } );
        }

        for( int i = k+B; i < N; i += B ) {
            g1.run ( [=]{ FW(A,k,i,k,B); } );
        }

        for( int j = 0; j < k; j += B ) {
            g1.run ( [=]{ FW(A,k,k,j,B); } );
        }

        for( int j = k+B; j < N; j += B ) {
            g1.run ( [=]{ FW(A,k,k,j,B); } );
        }
        g1.wait();

        for( int i = 0; i < k; i += B )
            for( int j = 0; j < k; j += B ) {
                g2.run( [=]{ FW(A,k,i,j,B); } );
            }

        for( int i = 0; i < k; i += B )
            for( int j = k+B; j < N; j += B ) {
                g2.run( [=]{ FW(A,k,i,j,B); } );
            }

        for( int i = k+B; i < N; i += B )
            for( int j = 0; j < k; j += B ) {
                g2.run( [=]{ FW(A,k,i,j,B); } );
            }

        for( int i = k+B; i < N; i += B)
            for( int j = k+B; j < N; j += B ) {
                g2.run( [=]{ FW(A,k,i,j,B); } );
            }
        g2.wait();
    }
    toc = tbb::tick_count::now();

    std::cout << "FW_TILED tasks\nSize : " << N << "\nBlock size: " << B
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
