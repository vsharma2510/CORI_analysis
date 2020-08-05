#!/bin/bash
#SBATCH --qos=debug
#SBATCH --time=00:30:00
#SBATCH --nodes=2
#SBATCH --constraint=haswell
#SBATCH -o /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3519/slurm_output_debug.o
#SBATCH -e /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3519/slurm_error_debug.e
#SBATCH -J DS3519

source /global/common/software/m2812/soft/cuoresw-conservative/setup.sh

printenv

/global/homes/v/vsharma2/Analysis_code/Miscalibration/Macros/ChanAmp /global/homes/v/vsharma2/Analysis_code/Miscalibration/Miscalibrated_channels/DS3519_gamma.txt

echo "Job finished"
