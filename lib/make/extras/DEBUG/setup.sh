#! /bin/sh
# /*@@
#   @file      setup.sh
#   @date      Wed Jul 21 11:18:40 1999
#   @author    Joan Masso
#   @desc 
#   Setup MPI
#   @enddesc 
# @@*/
    
if test -n "$DEBUG" ; then
	
echo "Configuring with DEBUG. Blocks with #ifdef CCTK_DEBUG will be activated"

CCTK_WriteLine cctk_Extradefs.h "#define CCTK_DEBUG"

# Work out which variation of DEBUG
    
if test -r $srcdir/extras/DEBUG/$DEBUG ; then
	. $srcdir/extras/DEBUG/$DEBUG
else
    echo "DEBUG Selected. Assuming -g flag for all compilers"
   : ${DEBUG_C_FLAG="-g"} 
   : ${DEBUG_CXX_FLAG="-g"} 
   : ${DEBUG_F77_FLAG="-g"}
   : ${DEBUG_F90_FLAG="-g"}
   : ${DEBUG_LD_FLAG="-g"} 
fi

CCTK_WriteLine make.extra.defn "DEBUG_C = $DEBUG_C_FLAG"
CCTK_WriteLine make.extra.defn "DEBUG_CXX = $DEBUG_CXX_FLAG"
CCTK_WriteLine make.extra.defn "DEBUG_F77 = $DEBUG_F77_FLAG"
CCTK_WriteLine make.extra.defn "DEBUG_F90 = $DEBUG_F90_FLAG"
CCTK_WriteLine make.extra.defn "DEBUG_LD = $DEBUG_LD_FLAG"

fi

