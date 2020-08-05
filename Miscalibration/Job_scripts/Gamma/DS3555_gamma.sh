#!/bin/bash
#SBATCH --qos=regular
#SBATCH --time=24:00:00
#SBATCH --nodes=2
#SBATCH --constraint=haswell
#SBATCH -o /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3555/slurm_output.o
#SBATCH -e /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3555/slurm_error.e
#SBATCH -J DS3555

source /global/common/software/m2812/soft/cuoresw-conservative/setup.sh


/global/homes/v/vsharma2/Analysis_code/Miscalibration/Macros/ChanAmp /global/homes/v/vsharma2/Analysis_code/Miscalibration/Miscalibrated_channels/DS3555_gamma.txt

echo "Job finished"
