# MPI easy exercises

This is a set of excercises on MPI prepared for the students of [Foundations of HPC](https://dssc.units.it/foundations-high-performance-computing) held by [DSSC @ UniTS](https://dssc.units.it).

## Exercise 1: Fix deadlock

 - Fix  [Foundations_of_HPC_2020/day11/mpi_codes/deadlock.c](https://github.com/Foundations-of-HPC/Foundations_of_HPC_2020/blob/master/day11/mpi_codes/deadlock.c) in order to remove the deadlock. 
 
 Solution: `ese1_fix_deadlock.c`.

## Exercise 2: play with `mpi_pi.c` send/recv types

Edit [Foundations_of_HPC_2020/day11/mpi_codes/mpi_c.c](https://github.com/Foundations-of-HPC/Foundations_of_HPC_2020/blob/master/day11/mpi_codes/mpi_pi.c) and test 4 various possible MPI Send calls (Send, SSend, BSend, RSend):

Solutions: `ese_2_mpi_pi_rsend.c`, `ese_2_mpi_pi_bsend.c`, `ese_2_mpi_pi_ssend.c`, `ese_2_mpi_pi_send.c`

## Exercise 3: write `mpi_pi.c` with MPI reduce operation

Edit  [Foundations_of_HPC_2020/day11/mpi_codes/mpi_c.c](https://github.com/Foundations-of-HPC/Foundations_of_HPC_2020/blob/master/day11/mpi_codes/mpi_pi.c) and use a MPI reduce function instead of Send and Recv.

Solution: `ese3_mpi_pi_sum_reduce.c`.

## Exercise 4: write `mpi_sum.c`

Write a parallel algorithm to sum all numbers between `1` and `N`, where `N` is provided as a command line argument.

- Solution 1: algorithm works only if `N` is divisble by `nranks` (see  `ese4_mpi_sum_nranks_divides_N.c`).
- Solution 2: algorithm works  for all `N>nranks` (see  `ese4_mpi_sum_any_N.c`).
- Solution 3: as solution 1 but with a proto-domain-decomposition where data is initialised on rank==0 and scattered to all other MPI processes ( see `ese4_mpi_sum_nranks_divides_N_domain_decomposition.c`).
- Solution 4: as solution 2 but with a proto-domain-decomposition (see `ese4_mpi_sum_any_N_domain_decomposition.c`)

## Exercise 5: topologies

- Implement a periodic 1D ring  and send data to the next MPI rank (thus the last will send to the first). See `ese5_ring1d.c`

- **difficult:** Implement a periodic 2D topology and use it to transpoe a matrix `ese5_ring2d.c`

## Exercise 6: mpi binding

- map each MPI rank to a different core with `mpirun --map-by core`  (check with `print_affinity.c`)

- map each MPI rank to a different socket with `mpi-run --map-by socket` (check with `print_affinity.c`)

