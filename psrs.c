#include <mpi.h>
#include <stdlib.h>
#include "merge.h"
#include "psrs.h"

static int compare(const void *a, const void *b) {
    int ai = *(int *)a;
    int bi = *(int *)b;
    return (ai > bi) - (ai < bi);
}

void psrs(int data[], int length, int rank, int proc_num) {
    if (proc_num == 1) {
        qsort(data, length, sizeof(int), compare);
        return;
    }

    // Phase 2
    // distribute work
    int recvcounts[proc_num];
    int displs[proc_num];
    for (int i = 0; i < proc_num; i++) {
        recvcounts[i] = length / proc_num + (i < length % proc_num ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + recvcounts[i - 1];
    }
    MPI_Scatterv(data, recvcounts, displs, MPI_INT, rank == 0 ? MPI_IN_PLACE : data,
                 recvcounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    qsort(data, recvcounts[rank], sizeof(int), compare);

    // Phase 3
    int pivot[proc_num * proc_num];
    for (int i = 0; i < proc_num; i++) {
        pivot[i] = data[i * recvcounts[rank] / proc_num];
    }
    MPI_Gather(rank == 0 ? MPI_IN_PLACE : pivot, proc_num, MPI_INT, pivot, proc_num, MPI_INT, 0,
               MPI_COMM_WORLD);
    if (rank == 0) {
        int *starts[proc_num];
        int lengths[proc_num];
        for (int i = 0; i < proc_num; i++) {
            starts[i] = &pivot[i * proc_num];
            lengths[i] = proc_num;
        }
        int sorted[proc_num * proc_num];
        multimerge(starts, lengths, proc_num, sorted, proc_num * proc_num);
        for (int i = 0; i < proc_num - 1; i++) {
            pivot[i] = sorted[(i + 1) * proc_num];
        }
    }
    MPI_Bcast(pivot, proc_num - 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Phase 4
    int cstart[proc_num];
    int clength[proc_num];
    for (int i = 0, di = 0; i < proc_num; i++) {
        cstart[i] = di;
        if (i == proc_num - 1) {
            clength[i] = recvcounts[rank] - di;
            break;
        }
        clength[i] = 0;
        while (di < recvcounts[rank] && data[di] < pivot[i]) {
            clength[i]++;
            di++;
        }
    }

    // Phase 5
    int *rbuf = (int *)malloc(sizeof(int) * length);
    int rlen[proc_num];
    int rstart[proc_num];
    for (int i = 0; i < proc_num; i++) {
        MPI_Gather(&clength[i], 1, MPI_INT, rlen, 1, MPI_INT, i, MPI_COMM_WORLD);
        if (rank == i) {
            rstart[0] = 0;
            for (int i = 1; i < proc_num; i++) {
                rstart[i] = rstart[i - 1] + rlen[i - 1];
            }
        }
        MPI_Gatherv(&data[cstart[i]], clength[i], MPI_INT, rbuf, rlen, rstart, MPI_INT, i,
                    MPI_COMM_WORLD);
    }
    int *mmstarts[proc_num];
    for (int i = 0; i < proc_num; i++) {
        mmstarts[i] = &rbuf[rstart[i]];
    }
    multimerge(mmstarts, rlen, proc_num, data, length);

    // Phase 6
    int slen[proc_num];
    int sstart[proc_num];
    int send_len = rstart[proc_num - 1] + rlen[proc_num - 1];
    MPI_Gather(&send_len, 1, MPI_INT, slen, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        sstart[0] = 0;
        for (int i = 1; i < proc_num; i++) {
            sstart[i] = sstart[i - 1] + slen[i - 1];
        }
    }
    MPI_Gatherv(rank == 0 ? MPI_IN_PLACE : data, send_len, MPI_INT, data, slen, sstart, MPI_INT, 0,
                MPI_COMM_WORLD);

    free(rbuf);
}
