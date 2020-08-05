#!/bin/bash
#SBATCH --qos=regular
#SBATCH --time=24:00:00
#SBATCH --nodes=2
#SBATCH --constraint=haswell
#SBATCH -o /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3564/slurm_output_alpha.o
#SBATCH -e /global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/DS3564/slurm_error_alpha.e
#SBATCH -J DS3564

source /global/common/software/m2812/soft/cuoresw-conservative/setup.sh


/global/homes/v/vsharma2/Analysis_code/Miscalibration/Macros/ChanAmp_alpha /global/homes/v/vsharma2/Analysis_code/Miscalibration/Miscalibrated_channels/DS3564_alpha.txt

echo "Job finished"
