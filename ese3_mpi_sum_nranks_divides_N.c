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

  if(total_size%ranks!=0){
        fprintf(stderr, "total_size (%d) must be divisible by the number of ranks (%d)\n",total_size, ranks);
        MPI_Finalize();
        exit(1);
  }


  /*
 *  decompose domain
 */
  int local_size = total_size/ranks;

  int *numbers = (int *) malloc(local_size*sizeof(int));

  for(int i = 0;  i<local_size; i++){
    numbers[i] = i_rank*local_size + i;
  }



  /*
 * compute
 */

//  printf("rank %d going to loop from 0 t\n",i_rank);
  int sum_local = 0;
  for(int i = 0;  i<local_size; i++){
    sum_local += numbers[i];
  }




  int sum_result=-1;

  printf("rank %d going to MPI_Reduce\n",i_rank);
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
