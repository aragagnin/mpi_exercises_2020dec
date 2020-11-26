/**
 * @author RookieHPC
 * @brief Original source code at https://www.rookiehpc.com/mpi/docs/mpi_cart_create.php
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>

/**
 * @brief Illustrates how to create a communicator representing a 2D torus
 * topology.
 **/



void print_matrix_mpi(int *m,int k, int N, int *dims, int *coords, int my_rank){

/*    if(my_rank==0)   printf("\n\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);
    fflush(stdout);
*/
    //loop over the MPI  x-coordinates 0..dims[0]
    for(int i=0; i<dims[0]; i++){
       //MPI_Barrier(MPI_COMM_WORLD);
       //loop over the matrix x-coordinates  in the MPI coordinate
       for(int ik=i*k, im=0; ik<(i+1)*k; ik++,im++){
          // loop over the MPI y-coordinates
           for(int j=0; j<dims[1]; j++){
               MPI_Barrier(MPI_COMM_WORLD);
               // loop over matrix y- coordinates inside MPI coorindates (i,j)
               for(int jk=j*k,jm=0; jk<(j+1)*k; jk++,jm++){
                //if it's me, I print
                if(i==coords[0] && j==coords[1]){
                     printf("%03d, ",m[im*k+jm]);
                     if((jm==k-1) && (j==dims[1]-1)) printf("\n");
                    fflush(stdout);

                }
              }
               MPI_Barrier(MPI_COMM_WORLD);

           }


        }
    }

    if(my_rank==0)   printf("\n\n");
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

}



int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);


    // Size of the default communicator
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);




    // Ask MPI to decompose our processes in a 2D cartesian grid for us
    int dims[2] = {0, 0};
    MPI_Dims_create(size, 2, dims);


    if(dims[0]!=dims[1]){
      printf("number of MPI ranks must be a square\n");
      exit(1);
    }
    int k=2; //number of elements in each MPI rank sub matrix
    int N = dims[0];
    int *m = (int *) malloc(k*k*sizeof(int));

    int *recv_m = (int *) malloc(k*k*sizeof(int)); //the sub-matrix element of the transposed result;

    //the  matrix that is scattered around the MPI ranks  have (dims[0]*N)^2  = (k*N)^2 elements
    //but wedo not need to allocate it!
    // Make both dimensions periodic
    int periods[2] = {true, true};
    //int periods[2] = {false, false};
    // Let MPI assign arbitrary ranks if it deems it necessary
    int reorder = false; //true;

    // Create a communicator given the 2D torus topology.
    MPI_Comm new_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, reorder, &new_communicator);

    // My rank in the new communicator
    int my_rank;
    MPI_Comm_rank(new_communicator, &my_rank);


    for(int i=0; i<k;i++)
            for(int j=0; j<k;j++)
                m[i*N+j]=my_rank+i+j;


    // Get my coordinates in the new communicator
    int my_coords[2];
    MPI_Cart_coords(new_communicator, my_rank, 2, my_coords);

     print_matrix_mpi(m,k, N, dims, my_coords,  my_rank);


    int transpose_coords[2];

    transpose_coords[0] = my_coords[1];
    transpose_coords[1] = my_coords[0];

    int transpose_rank; //the rank of the MPI task with the element we will transpose

    MPI_Cart_rank(new_communicator, transpose_coords, &transpose_rank);
    printf("my coords=(%d, %d); my_rank=%d; transpose coords =(%d, %d); transpose_rank=%d, \n",my_coords[0], my_coords[1], my_rank, transpose_coords[0], transpose_coords[1], transpose_rank);

    // Print my location in the 2D torus.
    //printf("[MPI process %d] I am located at (%d, %d). transpose rank=%d\n", my_rank, my_coords[0],my_coords[1], transpose_rank);

   //exchange sub-matrixes
    MPI_Status status;
    MPI_Sendrecv(
          m,
          k*k,
          MPI_INT,
          transpose_rank,
          NULL,
          recv_m,
          k*k,
          MPI_INT,
          transpose_rank,
          NULL,
          new_communicator,
          &status
  );


   //transpose local matrix
    for(int i=0; i<k;i++)
            for(int j=0; j<k;j++){
                printf("%d %d %d %d %03d\n",my_rank, i,j, j*k+1, recv_m[j*k+i]);
                m[i*k+j]=recv_m[j*k+i];
             }

    //print it
     print_matrix_mpi(m,k, N, dims, my_coords,  my_rank);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
