/*
 * Sum all numbers from 0 to N (N passed as arguments).
 * Sum is distributed among all mpi ranks.
 * */
#include <stdio.h>
#include <mpi.h>

int main (int argc, char * argv[])
{
  int i_rank, ranks;

  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD,&i_rank );
  MPI_Comm_size( MPI_COMM_WORLD,&ranks );


  if ( argc <=1) {
    fprintf (stderr , " Usage : mpi -np n %s total_size \n", argv[0] ) ;
    MPI_Finalize() ;
    exit(1) ;
  }

  int total_size = atoll(argv[1]);


  /*
  *  decompose domain
  */
  int *chunk_sizes = (int *) malloc(ranks*sizeof(int));
  int *displacements = (int *) malloc(ranks*sizeof(int));
  int *all_numbers;

  printf("domain decomposition..\n");
  if(i_rank==0){
      all_numbers  = (int *) malloc(total_size*sizeof(int));
      for(int i = 0; i < total_size; i++){
         all_numbers[i] = i;
      }

      for(int j_rank=0; j_rank<ranks; j_rank++){
        displacements[j_rank]=0;
        int j_local_size = total_size/ranks;
        if (j_rank == ranks - 1){ //we revise the decomposition for the last rank
            j_local_size = total_size - j_local_size*(ranks - 1) ;
        }
        chunk_sizes[j_rank] = j_local_size;
     }
  }

  printf("broadcast 1..task%d\n",i_rank);

  MPI_Bcast( chunk_sizes, ranks, MPI_INT, 0, MPI_COMM_WORLD);

  printf("broadcast 2..\n");


  MPI_Bcast( displacements, ranks, MPI_INT, 0, MPI_COMM_WORLD);


  int local_size = total_size/ranks;
  int *numbers = (int *) malloc(local_size*sizeof(int));
  printf("scatter..\n");


  MPI_Scatterv(all_numbers, chunk_sizes ,displacements, MPI_INT, numbers, chunk_sizes[i_rank], MPI_INT, 0, MPI_COMM_WORLD);
  /*
 * compute
 */

  printf("compute..\n");

  int sum_local = 0;
  for(int i = 0; i<local_size; i++){
    sum_local += numbers[i];
  }


  int sum_result=-1;


   MPI_Reduce(
             &sum_local,
             &sum_result,
             1,
             MPI_INT,
             MPI_SUM,
             0,
             MPI_COMM_WORLD);
  printf("sum=%d\n",sum_result);

  MPI_Finalize();

}
