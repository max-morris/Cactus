#! /bin/sh
# /*@@
#   @file      setup.sh
#   @date      Fri Jul 30 1999
#   @author    Thomas Radke
#   @desc 
#              Setup HDF5
#   @enddesc 
# @@*/
    
if test -n "$HDF5" ; then
	
echo "Configuring with HDF5. Blocks with #ifdef HDF5 will be activated"

CCTK_WriteLine cctk_extradefs.h "#define HDF5"

# Work out which variation of HDF5 lib
    
if test -r $srcdir/extras/HDF5/$HDF5 ; then
	. $srcdir/extras/HDF5/$HDF5
else
    echo "HDF5 selected, but no known HDF5 method - what is $HDF5 ?"
    exit 2
fi

# Write the data out to the header and make files.

CCTK_WriteLine make.extra.defn "HDF5_LIBS     = $HDF5_LIBS"
CCTK_WriteLine make.extra.defn "HDF5_LIB_DIRS = $HDF5_LIB_DIRS"
CCTK_WriteLine make.extra.defn "HDF5_INC_DIRS = $HDF5_INC_DIRS"

CCTK_WriteLine make.extra.defn ""
CCTK_WriteLine make.extra.defn ""

CCTK_WriteLine make.extra.defn 'LIBS         += $(HDF5_LIBS)'
CCTK_WriteLine make.extra.defn 'LIBDIRS      += $(HDF5_LIB_DIRS)'
CCTK_WriteLine make.extra.defn 'SYS_INC_DIRS += $(HDF5_INC_DIRS)'

fi

