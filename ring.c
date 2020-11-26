/**
 * @brief Illustrates how to create a communicator representing a 2D torus
 * topology.
 **/
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    // Size of the default communicator
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Ask MPI to decompose our processes in a 2D cartesian grid for us
    int dims[1] = {0};
    MPI_Dims_create(size, 1, dims);

    // Make both dimensions periodic
    int periods[1] = {true};

    // Let MPI assign arbitrary ranks if it deems it necessary
    int reorder = true;

    //int reorder = false;
    // Create a communicator given the 2D torus topology.
    MPI_Comm new_communicator;
    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &new_communicator);

    // My rank in the new communicator
    int my_rank;
    MPI_Comm_rank(new_communicator, &my_rank);

    // Get my coordinates in the new communicator
    int my_coords[1];
    MPI_Cart_coords(new_communicator, my_rank, 2, my_coords);

    int source, dest;
    MPI_Cart_shift(new_communicator, 0, 1, &source, &dest );

    int sendbuf = my_rank;
    int recvbuf = 0;
    MPI_Status status;
    MPI_Sendrecv(
       &sendbuf,
       1,
       MPI_INT,
       dest,
       NULL,
       &recvbuf,
       1,
       MPI_INT,
       source,
       NULL,
       new_communicator,
       &status
     );

    // Print my location in the 2D torus.
    printf("[MPI process %d] I am located at (%d) and I received %d.\n", my_rank, my_coords[0], recvbuf);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
