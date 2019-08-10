/*
  Recursive implementation of the Floyd-Warshall algorithm.
  command line arguments: N, B
  N = size of graph
  B = size of submatrix when recursion stops
  works only for N, B = 2^k
*/

#include <cstdlib>
#include <iostream>

#include "tbb/task.h"
#include "tbb/task_group.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "util.h"

inline int min(int a, int b);
void FW_SR (int **A, int arow, int acol,
            int **B, int brow, int bcol,
            int **C, int crow, int ccol,
            int myN, int bsize);

int main(int argc, char **argv)
{

    if (argc != 4){
        fprintf(stdout, "Usage %s N B nthreads\n", argv[0]);
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
    FW_SR(A,0,0, A,0,0,A,0,0,N,B);
    toc = tbb::tick_count::now();

    std::cout << "FW_SR Parallel tasks\nSize : " << N << "\nBlock size: " << B
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

void FW_SR (int **A, int arow, int acol,
            int **B, int brow, int bcol,
            int **C, int crow, int ccol,
            int myN, int bsize)
{
    int k,i,j;

    if(myN<=bsize)
        for(k=0; k<myN; k++)
            for(i=0; i<myN; i++)
                for(j=0; j<myN; j++)
                    A[arow+i][acol+j]=min(A[arow+i][acol+j], B[brow+i][bcol+k]+C[crow+k][ccol+j]);
    else {
        FW_SR(A,arow, acol,B,brow, bcol,C,crow, ccol, myN/2, bsize);

        tbb::task_group g1, g2;
        g1.run( [=]{ FW_SR(A,arow, acol+myN/2,B,brow, bcol,C,crow, ccol+myN/2, myN/2, bsize); } );
        g1.run( [=]{ FW_SR(A,arow+myN/2, acol,B,brow+myN/2, bcol,C,crow, ccol, myN/2, bsize); } );
        g1.wait();

        FW_SR(A,arow+myN/2, acol+myN/2,B,brow+myN/2, bcol,C,crow, ccol+myN/2, myN/2, bsize);
        FW_SR(A,arow+myN/2, acol+myN/2,B,brow+myN/2, bcol+myN/2,C,crow+myN/2, ccol+myN/2, myN/2, bsize);

        g2.run( [=]{ FW_SR(A,arow+myN/2, acol,B,brow+myN/2, bcol+myN/2,C,crow+myN/2, ccol, myN/2, bsize); } );
        g2.run( [=]{ FW_SR(A,arow, acol+myN/2,B,brow, bcol+myN/2,C,crow+myN/2, ccol+myN/2, myN/2, bsize); } );
        g2.wait();

        FW_SR(A,arow, acol,B,brow, bcol+myN/2,C,crow+myN/2, ccol, myN/2, bsize);
    }
}

