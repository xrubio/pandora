#------------------------------------------------------------------------------
# Author: Kalyan S. Perumalla <www.cc.gatech.edu/~kalyan> 08Jan03
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
LIBSYNK = libsynk.so
LIBOBJS = fm.o fmshm.o fmgm.o fmtcp.o fmmpi.o rm.o rmbar.o tm.o tmred.o tmnull.o 
TESTS   = gmtest fmtest rmtest

#------------------------------------------------------------------------------
# Set COMPOSITETIME=1 to enable "composite timestamps";
#                     to disable, comment out or set to zero.
# Set MPI_AVAILABLE=1 to enable MPI;
#                     to disable, comment out or set to zero.
# Also,
#      If MPI_AVAILABLE=1, then set CC to mpicc (or mpcc, etc).
#------------------------------------------------------------------------------
MORECFLAGS = -DCOMPOSITETIME=1 -DMPI_AVAILABLE=1 -DGM_AVAILABLE=0
 
#CC = mpicc
#CC = /gpfs/apps/MPICH/gm/1.2.7p1-19/64/bin/mpicc
CC = /opt/osshpc/mpich-mx/64/bin/mpicc
#CC = mpicc


# xlc_r -qmkshrobj -q64 -DUSE_STDARG -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -
#DHAVE_UNISTD_H=1 -DHAVE_STDARG_H=1 -DUSE_STDARG=1 -DMALLOC_RET_VOID=1 -Wl,-
#rpath -Wl,/opt/osshpc/mpich-mx/64/lib/shared -L/opt/osshpc/mpich-
#mx/64/lib/shared -L/opt/osshpc/mpich-mx/64/lib fm.o fmshm.o fmgm.o fmtcp.o 
#fmmpi.o rm.o rmbar.o tm.o tmred.o tmnull.o -o libsynk.a -
#L/gpfs/apps/CEPBATOOLS/mpitrace-stable-1.2-mx/lib -lmpitrace -lmpich -Wl,-
#rpath,/opt/osshpc/mx/lib64,-rpath,/opt/osshpc/mx/lib -L/opt/osshpc/mx/lib64 -
#L/opt/osshpc/mx/lib/ -lmyriexpress -lpthread -lrt

#MORELIBS = -lnsl -lsocket #For Solaris

#------------------------------------------------------------------------------
COPTS    = -O3 #-qstrict -qtune=ppc970 -qarch=ppc970 -qcache=auto
CFLAGS  = -I. $(COPTS) $(MORECFLAGS)
LDFLAGS  =  -L/gpfs/apps/CEPBATOOLS/mpitrace-stable-1.2-mx/lib -L. -lmpitrace
 
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
.c:
	$(CC) -o $@ $(CFLAGS) $<

	
#------------------------------------------------------------------------------
all: $(LIBSYNK) $(TESTS)

#------------------------------------------------------------------------------
$(LIBSYNK): $(LIBOBJS)
	$(CC) -o $@ -shared $(LDFLAGS) $(LIBOBJS)

rmtest: 
	$(CC) -o $@ rmtest.c  $(CFLAGS) -lsynk $(LDFLAGS)
fmtest: 
	$(CC) -o $@ fmtest.c  $(CFLAGS) -lsynk $(LDFLAGS)
gmtest: 
	$(CC) -o $@ gmtest.c  $(CFLAGS) -lsynk $(LDFLAGS)

#------------------------------------------------------------------------------
clean:
	rm -f $(LIBOBJS) $(LIBSYNK) $(TESTS)
	rm -f *.o *.a

#------------------------------------------------------------------------------
