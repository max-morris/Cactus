#! /bin/sh
# /*@@
#   @file      setup.sh
#   @date      Wed Oct 25 16:32:39 2000
#   @author    Tom Goodale
#   @desc 
#   Setup for compilation with pthreads
#   @enddesc 
# @@*/
    
if test -n "$PTHREADS" ; then
	
echo "Configuring with PTHREADS."

# Write the data out to the header and make files.

CCTK_WriteLine cctk_Extradefs.h "#define PTHREADS"
CCTK_WriteLine make.extra.defn "PTHREADS_LIBS = pthread"

CCTK_WriteLine make.extra.defn ""
CCTK_WriteLine make.extra.defn ""

CCTK_WriteLine make.extra.defn 'LIBS  += $(PTHREADS_LIBS)'

fi
