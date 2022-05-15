#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "psrs.h"

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    if (argc < 3) {
        fprintf(stderr, "%s datafile datalen\n", argv[0]);
        exit(1);
    }
    const char *input = argv[1];
    const int LEN = atoi(argv[2]);
    int rank;
    int proc_num;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);

    int *data = (int *)malloc(sizeof(int) * LEN);
    memset(data, 0, sizeof(int) * LEN);

    if (rank == 0) {
        // get input
        FILE *fp = fopen(input, "r");
        if (fp == NULL) {
            fprintf(stderr, "invalid file %s\n", input);
            exit(1);
        }
        for (int i = 0; i < LEN; i++) {
            int n = fscanf(fp, "%d", &data[i]);
            if (n != 1) {
                fprintf(stderr, "invalid input\n");
                exit(1);
            }
        }
        fclose(fp);
        printf("sorting %d elements\n", LEN);
    }

    double tic = MPI_Wtime();
    psrs(data, LEN, rank, proc_num);
    if (rank == 0) {
        double toc = MPI_Wtime();
        printf("%f ms\n", (toc - tic) * 1000);
        for (int i = 1; i < LEN; i++) {
            if (data[i - 1] > data[i]) {
                fprintf(stderr, "wrong order at %d: %d > %d\n", i - 1, data[i - 1], data[i]);
                exit(1);
            }
        }
    }
    free(data);

    MPI_Finalize();
}
