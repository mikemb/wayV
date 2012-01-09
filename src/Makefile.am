## Process this file with automake to produce Makefile.in

bin_PROGRAMS = wayv
data_DATA = wayv.conf DEFAULT.wkey
wayv_SOURCES = ai.c aimath.c backend.c display.c gesture.c process.c setup.c misc.c
CFLAGS = -Wall -O2 -DSETUPPATH=\"@datadir@\"
wayv_LDADD = @X_LIBS@
wayv_LDFLAGS = -I@x_includes@ -L@x_libraries@
