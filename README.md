## build

Dependencies:
```sh
module load slurm
module load gcc
module load openmpi
module load mpip
module load libunwind
```

Install mpiP locally to get the `libmpiP.so` file, or copy one from the lustre
filesystem:
```bash
mkdir -p ~/.local/lib
cp /expanse/lustre/projects/csd453/wangrc/libmpiP.so ~/.local/lib
```

## compile and run

Generate test data and compile:
```sh
make
```

Run interactively:
```sh
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/.local/lib srun --mpi=pmi2 --partition=compute --account=csd453 --nodes=1 --ntasks=16 --mem=20G -t 00:02:00 --export=ALL ./psrs test/40m.in 1000000
```

Run the batch job:
```sh
sbatch run.sb
```
