#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
#define MSGLEN 2048
  int ITAG_A = 100,ITAG_B = 200;
  int irank, i, idest, isrc, istag, iretag;
  float rmsg1[MSGLEN];
  float *b;
  float rmsg2[MSGLEN];




  MPI_Status recv_status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &irank);

  for (i = 1; i < MSGLEN; i++)
    {
      rmsg1[i] = irank;
      rmsg2[i] = -100;
    }
  if ( irank == 0 )
    {
      idest  = 1;
      isrc   = 1;
      istag  = ITAG_A;
      iretag = ITAG_B;
    }
  else if ( irank == 1 )
    {
      idest  = 0;
      isrc   = 0;
      istag  = ITAG_B;
      iretag = ITAG_A;
    }

  printf("Task %d has sent the message\n", irank);


  b = (float *)malloc( MSGLEN*sizeof(float)+MPI_BSEND_OVERHEAD);

  MPI_Buffer_attach( b, MSGLEN*sizeof(float)+MPI_BSEND_OVERHEAD );
  //memcpy(b, rmsg1,  MSGLEN*sizeof(float));

  MPI_Bsend(rmsg1, MSGLEN, MPI_FLOAT, idest, istag, MPI_COMM_WORLD);
  MPI_Recv(rmsg2, MSGLEN, MPI_FLOAT, isrc, iretag, MPI_COMM_WORLD, &recv_status);

  for (i = 1; i < MSGLEN; i++)
    {
      if(rmsg1[i] != irank) printf("%d error in rmsg1[%d] = %f!=%f! \n",irank, i, rmsg1[i] , (float)irank);
      if(rmsg2[i] != idest) printf("%d error in rmsg2[%d] = %f!=%f! \n",irank, i , rmsg2[i] , (float)idest);

    }
  int bsize;
  MPI_Buffer_detach( rmsg1, &bsize);
  printf("Task %d has received the message. bszie=%d\n", irank,bsize);
  MPI_Finalize();
}
