/**
 * @author RookieHPC
 * @brief Original source code at
 *https://rookiehpc.org/mpi/docs/mpi_gatherv/index.html
 **/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

/**
 * @brief Illustrates how to use the variable version of a gather.
 * @details Every MPI process begins with a value, the MPI process 0 will gather
 * all these values and print them. The example is designed to cover all cases:
 * - Different displacements
 * - Different receive counts
 * It can be visualised as follows:
 * This application is meant to be run with 3 processes.
 *
 * +-----------+ +-----------+ +-------------------+
 * | Process 0 | | Process 1 | |     Process 2     |
 * +-+-------+-+ +-+-------+-+ +-+-------+-------+-+
 *   | Value |     | Value |     | Value | Value |
 *   |  100  |     |  101  |     |  102  |  103  |
 *   +-------+     +-------+     +-------+-------+
 *      |                |            |     |
 *      |                |            |     |
 *      |                |            |     |
 *      |                |            |     |
 *      |                |            |     |
 *      |                |            |     |
 *   +-----+-----+-----+-----+-----+-----+-----+
 *   | 100 |  0  |  0  | 101 |  0  | 102 | 103 |
 *   +-----+-----+-----+-----+-----+-----+-----+
 *   |                Process 0                |
 *   +-----------------------+-----+-----+-----+
 **/
int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  // Get number of processes and check only 3 processes are used
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  if (size != 3) {
    printf("This application is meant to be run with 3 processes.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  // Get my rank
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // Determine root's process rank
  int root_rank = 0;

  switch (my_rank) {
  case 0: {
    // Define my value
    std::vector<int> my_value = {98, 99, 100};

    // Define the receive counts
    std::vector<int> counts = {3, 2, 4};

    // Define the displacements
    std::vector<int> displacements = {0, 3, 5};

    std::vector<int> buffer(9);

    MPI_Gatherv(my_value.data(), my_value.size(), MPI_INT, buffer.data(),
                counts.data(), displacements.data(), MPI_INT, root_rank,
                MPI_COMM_WORLD);
    printf("Values gathered in the buffer on process %d:", my_rank);
    for (int i = 0; i < buffer.size(); i++) {
      printf(" %d", buffer[i]);
    }
    printf("\n");
    break;
  }
  case 1: {
    int finalInt = my_rank * 111;
    std::vector<int> my_value = {101, finalInt};

    MPI_Gatherv(my_value.data(), my_value.size(), MPI_INT, NULL, NULL, NULL,
                MPI_INT, root_rank, MPI_COMM_WORLD);
    break;
  }
  case 2: {
    int finalInt = my_rank * 111;
    std::vector<int> my_values = {103, 104, 105, finalInt};

    MPI_Gatherv(my_values.data(), my_values.size(), MPI_INT, NULL, NULL, NULL,
                MPI_INT, root_rank, MPI_COMM_WORLD);
    break;
  }
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}
