
#!/bin/bash

rsync -avz --exclude 'backups/' --exclude 'bin/*' --exclude '*.dSYM' --exclude '*~' --exclude 'CMakeFiles' --exclude 'CMakeCache*' --exclude '*make' --exclude 'Makefile' --exclude '*.a' --exclude '*.o' --exclude 'TinyBRL-DDS' --exclude 'TinyBRL-export' --exclude 'TinyBRL-zlib' ../TinyBRL/ asus-laptop:~/TinyBRL/
