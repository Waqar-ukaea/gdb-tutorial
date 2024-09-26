#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

int main(int argc, char *argv[]) {
  MPI_Init(NULL, NULL); // Initialize the MPI environment
  int rank;
  int nprocs;
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  // int k1, k2;
  // int position = 0;
  // MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &k1);
  // MPI_Pack_size(dynamicTaskSize, MPI_DOUBLE, MPI_COMM_WORLD, &k2);
  // std::vector<char> buffer(k1+k2);

  if (nprocs < 2) {
    if (rank == 0) {
      std::cout << "Error! This program requires at least two processes to "
                   "run. Exiting... \n";
    }
    std::exit(1);
  } else if (nprocs > 4) {
    if (rank == 0) {
      std::cout << "Expected no more than 4 processes. Exiting... \n";
    }
    std::exit(1);
  }

  srand(time(0));

  std::vector<int> list(4000);
  std::iota(list.begin(), list.end(), 0);

  int chunkSize = 500;
  const int noMoreWork = -1;

  if (rank == 0) { // handler process...

    // MPI_Recv(buffer.data(), buffer.size(), MPI_PACKED, MPI_ANY_SOURCE,
    // mpiDataTag, MPI_COMM_WORLD, &status);

    // MPI_Unpack(buffer.data(), buffer.size(), &position, &workerStartIndex, 1,
    // MPI_INT, MPI_COMM_WORLD); MPI_Unpack(buffer.data(), buffer.size(),
    // &position, workerQVals.data(), dynamicTaskSize, MPI_DOUBLE,
    // MPI_COMM_WORLD);

    int max_index = list.size();
    int indexHandled = 0;
    // Send statically the initial indexes
    for (int i = 1; i < nprocs; i++) {
      MPI_Send(&indexHandled, 1, MPI_INT, i, i, MPI_COMM_WORLD);
      indexHandled += chunkSize;
    }

    int inactiveWorkers = 0;
    int numberOfWorkers = nprocs - 1;

    do {
      int avaialbleProcess = 0;
      // call recieve from any source to get an avaialble process
      MPI_Recv(&avaialbleProcess, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,
               &status);
      if (indexHandled < max_index) {
        // send that process the next index to start on in the total list
        MPI_Send(&indexHandled, 1, MPI_INT, avaialbleProcess, 1,
                 MPI_COMM_WORLD);
        indexHandled += chunkSize;
      } else { // send message to workers to tell them no more work available
        MPI_Send(&noMoreWork, 1, MPI_INT, avaialbleProcess, 1, MPI_COMM_WORLD);
        inactiveWorkers++;
      }
    } while (inactiveWorkers <
             numberOfWorkers); // while some workers are still active
  }

  else { // worker processes...
    int startIndex = 0;
    // recieve the initial set of indexes
    MPI_Recv(&startIndex, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &status);
    // Work with the iterations startIndex

    int start = startIndex;
    int end = start + chunkSize;
    double sleepTime = rand() % 5 + 1;
    sleep(sleepTime);
    printf("Index[%d] processed by rank [%d], time taken = %f \n", startIndex,
           rank, sleepTime);

    // printf("startIndex for process [%d] = %d \n", rank, startIndex);

    do {
      MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Recv(&startIndex, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

      start = startIndex;
      end = start + chunkSize;

      if (startIndex != noMoreWork) {
        double sleepTime = rand() % 5 + 1;
        sleep(sleepTime);
        // processing the next set of available work
        printf("Index[%d] processed by rank [%d], time taken = %f \n",
               startIndex, rank, sleepTime);
      }
    } while (startIndex != noMoreWork); // while there is work available
  }

  // printf("Process {%d} -> I AM OUT\n", rank);
  MPI_Finalize();
  return 0;
}