#!/bin/bash
#SBATCH --qos=regular
#SBATCH --time=24:00:00
#SBATCH --nodes=2
#SBATCH --constraint=haswell
#SBATCH -o /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3552/slurm_output.o
#SBATCH -e /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3552/slurm_error.e
#SBATCH -J DS3552

source /global/common/software/m2812/soft/cuoresw-conservative/setup.sh


/global/homes/v/vsharma2/Analysis_code/Miscalibration/Macros/ChanAmp_gamma /global/homes/v/vsharma2/Analysis_code/Miscalibration/Miscalibrated_channels/DS3552_gamma.txt

echo "Job finished"
