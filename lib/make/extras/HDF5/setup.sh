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

CCTK_WriteLine cctk_Extradefs.h "#define HDF5"

# Work out which variation of HDF5 lib
    
if test -z "$HDF5_DIR" ; then
   echo "HDF5 selected but no HDF5_DIR set... Checking some places"
   CCTK_Search HDF5_DIR "/usr /usr/local /usr/local/hdf5 /usr/local/packages/hdf5 /usr/local/apps/hdf5 c:/packages/hdf5" include/hdf5.h
   if test -z "$HDF5_DIR" ; then
       echo "Unable to locate the HDF5 directory - please set HDF5_DIR"
       exit 2
   fi
   echo "Found an HDF5 package in $HDF5_DIR"
fi

grep -qe '#define HAVE_PARALLEL 1' ${HDF5_DIR}/include/H5config.h 2> /dev/null
test_phdf5=$?

if [ -n "$MPI" ] ; then
   if [ $test_phdf5 -eq 0 ] ; then
      echo "Found parallel HDF5 library, so Cactus will make use of PHDF5 support."
   else
      echo "Found serial HDF5 library, so Cactus can't make use of PHDF5 support."
   fi
else
   if [ $test_phdf5 -eq 0 ] ; then
      echo "Found parallel HDF5 library, but Cactus wasn't configured with MPI."
      echo "Please set HDF5_DIR to point to a serial HDF5 package, or configure Cactus with MPI."
      exit 2
   fi
fi

# Set the HDF5 libs, libdirs and includedirs

HDF5_LIBS=hdf5
if test `uname` = "Linux" ; then
  HDF5_LIBS="hdf5 z"
fi
HDF5_LIB_DIRS="$HDF5_DIR/lib"
HDF5_INC_DIRS="$HDF5_DIR/include"

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
