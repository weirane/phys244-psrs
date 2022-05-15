## how to build

Dependencies:
```sh
module load slurm
module load gcc
module load openmpi
module load mpip
module load libunwind
```

Compile and run:
```sh
make
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/.local/lib srun --mpi=pmi2 --partition=compute --account=csd453 --ntasks=16 --mem=20G -t 00:02:00 --export=ALL ./psrs test/1m.in 1000000
```
