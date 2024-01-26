#include <bits/stdc++.h>
#include "mpi.h"

using namespace std;

void backtracking(int n, int row, bool check_columns[], bool check_positive_diagonal[], bool check_negative_diagonal[], int &count) {
    if (row == n) {
        count++;
        return;
    }

    for (int col = 0; col < n; col++) {
        if (check_columns[col] || check_positive_diagonal[row - col + n - 1] || check_negative_diagonal[row + col]) {
            continue;
        }
        check_columns[col] = true;
        check_positive_diagonal[row - col + n - 1] = true;
        check_negative_diagonal[row + col] = true;

        backtracking(n, row + 1, check_columns, check_positive_diagonal, check_negative_diagonal, count);

        check_columns[col] = false;
        check_positive_diagonal[row - col + n - 1] = false;
        check_negative_diagonal[row + col] = false;
    }
}

int main(int argc, char* argv[]) {
    int task_id, num_processes;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    
    int n;
    if (task_id == 0) {
        cin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    bool check_columns[n];
    bool check_positive_diagonal[2 * n - 1];
    bool check_negative_diagonal[2 * n - 1];
    
    for (int i = 0; i < n; i++) {
        check_columns[i] = false;
    }
    for (int i = 0; i < 2 * n - 1; i++) {
        check_positive_diagonal[i] = false;
        check_negative_diagonal[i] = false;
    }

    int count = 0;

    for (int col = task_id; col < n; col += num_processes) {
        check_columns[col] = true;
        check_positive_diagonal[0 - col + n - 1] = true;
        check_negative_diagonal[0 + col] = true;

        backtracking(n, 1, check_columns, check_positive_diagonal, check_negative_diagonal, count);

        check_columns[col] = false;
        check_positive_diagonal[0 - col + n - 1] = false;
        check_negative_diagonal[0 + col] = false;
    }

    int total_count;
    MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (task_id == 0) {
        cout << total_count << endl;
    }

    MPI_Finalize();

    return 0;
}
