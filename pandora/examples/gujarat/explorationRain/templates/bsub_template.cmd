#!/bin/bash
#BSUB -J gujaratINDEX
#BSUB -cwd ./INITIALDIR
#BSUB -W 24:00
#BSUB -e gujarat.err
#BSUB -o gujarat.log
#BSUB -n 4
#BSUB -R"span[ptile=16]"

export OMP_NUM_THREADS=4
# You can choose the parallel environment through modules
module load gnu openmpi
mpirun -np 1 -npernode 4 ../../gujarat

