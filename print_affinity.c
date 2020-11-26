/*


  print_affinity.c, Antonio Ragagnin <ragagnin@lrz.de>  2019.


  For each MPI rank and for each OpenMP task, the code displays the hostname,
  the CPU id and (optionally) the GPU id (of the current computing node).

  The code can be compiled without both MPI and OpenMP support.

  Known issues:
      If sched.h is not installed (rare, it usually happens on Power systems), or it does not give the right results (e.g. always zero),
      try to compile with -DPROCFILE flag (so the code will get the CPU id by parsing the output of '/proc/self/stat')


  Examples of compiling:
      MPI only:
          mpicc -D_MPI print_affinity.c -o print_affinity.exe

      OpenMP only:
          gcc -D_OPENMP  -fopenmp print_affinity.c -o print_affinity.exe

      MPI + OpenMP:
          mpicc -D_MPI -D_OPENMP  -fopenmp print_affinity.c -o print_affinity.exe

      MPI + OpenMP + OPENACC (with PGI compiler):
          mpicc -D_MPI -D_OPENMP  -D_OPENACC -mp -acc -ta=tesla:cc70  print_affinity.c -o print_affinity.exe

   Example of running:
      OMP_NUM_THREADS=4 mpirun -n 4 ./print_affinity.exe


   Fine tunings:
      Set -DHOSTNAME_MAX_LENGTH at compile time to change the max length of the hostname (e.g. -DHOSTNAME_MAX_LENGTH=200).
      When compiling with -DPROCFILE.

      Set -DCPU_ID_LINE_IN_PROC_STAT at compile time to change the column of '/proc/self/stat' that contains the CPU id
      (e.g. -DCPU_ID_LINE_IN_PROC_STAT=40).

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

#ifndef PROCFILE
#define  SCHED
#endif

#ifndef HOSTNAME_MAX_LENGTH
#define HOSTNAME_MAX_LENGTH 100
#endif

#ifndef CPU_ID_LINE_IN_PROC_STAT
#define CPU_ID_LINE_IN_PROC_STAT 40
#endif

#if defined(_MPI)
#include <mpi.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef _OPENACC
#include <openacc.h>
#endif

#ifdef SCHED
#define _GNU_SOURCE
#include <sched.h>
#endif



int get_cpu_id();

int main(int argc, char *argv[]){

  int ranks,rank, header_displayed=0;



#ifdef _MPI
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#else
  ranks=1;
  rank=0;
#endif



#ifdef _OPENACC
  acc_init(acc_device_nvidia);
#pragma omp parallel
  {
        acc_init(acc_device_nvidia);

  }
#endif


  char hostname[HOSTNAME_MAX_LENGTH+1];
  gethostname(hostname, HOSTNAME_MAX_LENGTH);

#ifdef _OPENMP
#pragma omp parallel
#endif
  {

    for(int i=0;i<ranks;i++){

      if (i==rank){
#ifdef _OPENMP
#pragma omp critical
#endif
        {

#ifdef _OPENMP
          int thread_id = omp_get_thread_num();
          int threads = omp_get_num_threads();
#else
          int thread_id = 0;
          int threads = 1;
#endif

          if(!header_displayed && i==0){
            header_displayed=1;
            printf("#MPI_rank(of %d), OpenMP_thread(of %d), hostname, CPU_id",ranks,threads);

#ifdef _OPENACC
            int gpus =  acc_get_num_devices( acc_device_nvidia );
            printf(", GPU_id(of %d)",gpus);
#endif

            printf("\n");
          }

#ifdef SCHED
          int cpu_id = sched_getcpu();
#else
          int cpu_id = get_cpu_id();
#endif
          printf("%d, %d, \"%s\", %d",rank, thread_id, hostname, cpu_id);

#ifdef _OPENACC
          int deviceid = acc_get_device_num(acc_device_nvidia);
          printf(", %d", deviceid);
#endif

          printf("\n");


        }

      }

#ifdef _MPI
#ifdef _OPENMP
#pragma omp barrier
#pragma omp master
#endif
      MPI_Barrier(MPI_COMM_WORLD);
#endif
    }
  }
#ifdef _MPI
  MPI_Finalize();
#endif
  return 0;

}

/*
   Use this function if sched.h is not available of not reliable.
   Stolen from: https://github.com/RRZE-HPC/likwid/blob/master/test/MPI_pin_test.c
*/
int get_cpu_id() {
  /* Get the the current process' stat file from the proc filesystem */
  FILE* procfile = fopen("/proc/self/stat", "r");
  long to_read = 8192;
  char buffer[to_read];
  int read = fread(buffer, sizeof(char), to_read, procfile);
  fclose(procfile);
  // Field with index 38 (zero-based counting) is the one we want
  char* line = strtok(buffer, " ");
  for (int i = 1; i < 38; i++)
    {
      line = strtok(NULL, " ");
    }

  int cpu_id = atoi(line);
  return cpu_id;
}
