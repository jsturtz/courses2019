To compile, invoke make:
$ make

This should execute just fine on the os1.engr.oregonstate.edu server.
If for some reason that doesn't work, manually type these instructions:

$ gcc smallsh.c -o smallsh -std=c99 -D_POSIX_C_SOURCE

NOTE: Executing the bash script as per assignment description does not
work in tcsh due to "ambiguous redirect" error. Use bash instead.
