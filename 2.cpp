#include <bits/stdc++.h>
#include "mpi.h"

using namespace std;

void floydWarshall(int &N, int task_id, int num_processes, vector<vector<int> > &A)
{
    for (int k = 0; k < N; k++) {
        for (int u = task_id; u < N; u += num_processes) {
            for (int v = 0; v < N; v++) {
                if (A[u][k] == -1 || A[k][v] == -1) {
                    continue;
                } else if (A[u][v] == -1) {
                    if (A[u][k] == -1 || A[k][v] == -1) {
                        A[u][v] = -1;
                    } else {
                        A[u][v] = A[u][k] + A[k][v];
                    }
                } else {
                    A[u][v] = min(A[u][v], A[u][k] + A[k][v]);
                }
            }
        }

        for (int i = 0; i < N; i ++) {
            MPI_Bcast(&A[i][0], N, MPI_INT, i % num_processes, MPI_COMM_WORLD);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }
}

int main(int argc, char* argv[]) {
    int task_id, num_processes;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int N;

    if (task_id == 0) {
        cin >> N;
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<vector<int> > A(N, vector<int>(N));

    if (task_id == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cin >> A[i][j];
            }
        }
    }

    for (int i = 0; i < N; i++) {
        MPI_Bcast(&A[i][0], N, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    floydWarshall(N, task_id, num_processes, A);

    MPI_Barrier(MPI_COMM_WORLD);

    if (task_id == 0) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N - 1; j++) {
                cout << A[i][j] << " ";
            }
            cout << A[i][N - 1] << "\n";
        }
    }

    MPI_Finalize();

    return 0;
}