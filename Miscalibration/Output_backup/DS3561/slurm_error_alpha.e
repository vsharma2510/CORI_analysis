
 *** Break *** segmentation violation



===========================================================
There was a crash.
This is the entire stack trace of all threads:
===========================================================
#0  0x00002aaaba7972ba in waitpid () from /lib64/libc.so.6
#1  0x00002aaaba71486f in do_system () from /lib64/libc.so.6
#2  0x00002aaab1a7eb45 in TUnixSystem::StackTrace() () from /global/common/software/m2812/soft/install-conservative/root/linux-suse-15-x86_64/5.34.38/lib/libCore.so
#3  0x00002aaab1a80aef in TUnixSystem::DispatchSignals(ESignals) () from /global/common/software/m2812/soft/install-conservative/root/linux-suse-15-x86_64/5.34.38/lib/libCore.so
#4  <signal handler called>
#5  0x000000000040278e in main ()
===========================================================


The lines below might hint at the cause of the crash.
If they do not help you then please submit a bug report at
http://root.cern.ch/bugs. Please post the ENTIRE stack trace
from above as an attachment in addition to anything else
that might help us fixing this issue.
===========================================================
#5  0x000000000040278e in main ()
===========================================================


/var/spool/slurmd/job32700144/slurm_script: line 13:  1716 Segmentation fault      /global/homes/v/vsharma2/Analysis_code/Miscalibration/Macros/ChanAmp_alpha /global/homes/v/vsharma2/Analysis_code/Miscalibration/Miscalibrated_channels/DS3561_alpha.txt
