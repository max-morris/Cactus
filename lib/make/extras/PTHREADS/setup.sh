#! /bin/sh
# /*@@
#   @file      setup.sh
#   @date      Wed Oct 25 16:32:39 2000
#   @author    Tom Goodale
#   @desc 
#   Setup for compilation with pthreads
#   @enddesc 
# @@*/
    
if test "X$PTHREADS" != "Xyes"; then
  return
fi

echo "Configuring with PTHREADS"

# Write the data out to the header and make files.

CCTK_WriteLine cctk_Extradefs.h "#define CCTK_PTHREADS 1"

# the PTHREAD_xxx variables are set by configure
CCTK_WriteLine make.extra.defn "PTHREADS_CFLAGS   = $PTHREADS_CFLAGS"
CCTK_WriteLine make.extra.defn "PTHREADS_CXXFLAGS = $PTHREADS_CXXFLAGS"
CCTK_WriteLine make.extra.defn "PTHREADS_LIBS     = $PTHREAD_LIBS"
CCTK_WriteLine make.extra.defn ""
CCTK_WriteLine make.extra.defn ""
CCTK_WriteLine make.extra.defn 'CFLAGS   += $(PTHREADS_CFLAGS)'
CCTK_WriteLine make.extra.defn 'CXXFLAGS += $(PTHREADS_CXXFLAGS)'
CCTK_WriteLine make.extra.defn 'LIBS     += $(PTHREADS_LIBS)'
