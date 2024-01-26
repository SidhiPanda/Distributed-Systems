#include <bits/stdc++.h>
#include "mpi.h"

using namespace std;

int countNeighbors(int row, int col, vector<vector<int> >& A, int N, int M) {
    int count = 0;

    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {
            if ((i < 0 || j < 0 || i >= N || j >= M) || (i == row && j == col)) {
                continue;
            } else if (A[i][j] == 1) {
                count++;
            }
        }
    }
    
    return count;
}

int main(int argc, char* argv[]) {
    int task_id, num_processes;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int N, M, T;

    if (task_id == 0) {
        cin >> N >> M >> T;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<vector<int> > A(N, vector<int>(M));

    if (task_id == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                cin >> A[i][j];
            }
        }
    }

    for (int i = 0; i < N; i ++) {
        MPI_Bcast(&A[i][0], M, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    for (int t = 0; t < T; t++) {
        vector<vector<int> > B(N, vector<int>(M));
        for (int i = task_id; i < N; i += num_processes) {
            for (int j = 0; j < M; j++) {
                int num_neighbors = countNeighbors(i, j, A, N, M);

                if (A[i][j] == 1) {
                    if (num_neighbors < 2 || num_neighbors > 3) {
                        B[i][j] = 0;
                    } else {
                        B[i][j] = 1;
                    }
                } else {
                    if (num_neighbors == 3) {
                        B[i][j] = 1;
                    } else {
                        B[i][j] = 0;
                    }
                }
            }
        }

        A = B;

        for (int i = 0; i < N; i ++) {
            MPI_Bcast(&A[i][0], M, MPI_INT, i % num_processes, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);

    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (task_id == 0) {
        for (int i = 0; i < N; i++) {
            cout << A[i][0];
            for (int j = 1; j < M; j++) {
                cout << " " << A[i][j];
            }
            cout << endl;
        }
    }

    MPI_Finalize();

    return 0;
}