
#	Use '--delete' option to allow the deletion of the extra files
#	on the destination (e.g. if you remove a file from the source,
#	the synchronization won't remove it from the destination if '--delete'
#	option is not used).
#
#	We did not use it here because we want to keep some extra files related
#	to the compilation performed on the server side.
#!/bin/bash

rsync -avz --exclude 'backups/' --exclude 'bin/*' --exclude '*.dSYM' --exclude '*~' --exclude 'CMakeFiles' --exclude 'CMakeCache*' --exclude '*make' --exclude 'Makefile' --exclude '*.a' --exclude '*.o' --exclude 'TinyBRL-DDS' --exclude 'TinyBRL-export' --exclude 'TinyBRL-zlib' ../TinyBRL/ ms801:~/TinyBRL/


ssh -t ms801 'rsync -avz TinyBRL/ dragon1:~/TinyBRL/'
