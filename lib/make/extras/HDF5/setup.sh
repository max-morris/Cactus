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

# Work out which variation of HDF5 lib
    
if test -z "$HDF5_DIR" ; then
   echo "HDF5 selected but no HDF5_DIR set... Checking some places"
   CCTK_Search HDF5_DIR "/usr /usr/local /usr/local/hdf5 /usr/local/packages/hdf5 /usr/local/apps/hdf5 /usr/local/hdf5/serial c:/packages/hdf5" include/hdf5.h
   if test -z "$HDF5_DIR" ; then
       echo "Unable to locate the HDF5 directory - please set HDF5_DIR"
       exit 2
   fi
   echo "Found an HDF5 package in $HDF5_DIR"
fi


# Check what version we found

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
HDF5_LIB_DIRS="$HDF5_DIR/lib"
HDF5_INC_DIRS="$HDF5_DIR/include"


# Check whether we have to link with libz.a

# this is for 1.3.x versions of HDF5
grep -qe '#define H5_HAVE_COMPRESS2 1' ${HDF5_DIR}/include/H5pubconf.h 2> /dev/null
test_compress2=$?

# this is for 1.2.x versions of HDF5
if [ $test_compress2 -ne 0 ] ; then
  grep -qe '#define HAVE_COMPRESS2 1' ${HDF5_DIR}/include/H5config.h 2> /dev/null
  test_compress2=$?
fi

# this is for old 1.0.x versions of HDF5 where they used different defines for zlib
grep -qe '#define HAVE_LIBZ 1' ${HDF5_DIR}/include/H5config.h 2> /dev/null
test_zlib=$?

if [ $test_compress2 -eq 0 -o $test_zlib -eq 0 ] ; then
   if test -z "$LIBZ_DIR" -a ! -r /usr/lib/libz.a ; then
      echo "HDF5 library was compiled with libz, searching for libz.a ..."
      CCTK_Search LIBZ_DIR "/usr/local/lib c:/packages/libz" libz.a
      if test -z "$LIBZ_DIR" ; then
          echo "Unable to locate the library libz.a - please set LIBZ_DIR"
          exit 2
      fi
      echo "Found library libz.a in $LIBZ_DIR"
   fi
   HDF5_LIBS="$HDF5_LIBS z"
   HDF5_LIB_DIRS="$HDF5_LIB_DIRS $LIBZ_DIR"
fi

# Finally, add the math lib which might not be linked against by default
HDF5_LIBS="$HDF5_LIBS m"

# Write the data out to the header and make files.

CCTK_WriteLine cctk_Extradefs.h "#define HDF5"
CCTK_WriteLine make.extra.defn "HDF5_LIBS     = $HDF5_LIBS"
CCTK_WriteLine make.extra.defn "HDF5_LIB_DIRS = $HDF5_LIB_DIRS"
CCTK_WriteLine make.extra.defn "HDF5_INC_DIRS = $HDF5_INC_DIRS"

CCTK_WriteLine make.extra.defn ""
CCTK_WriteLine make.extra.defn ""

CCTK_WriteLine make.extra.defn 'LIBS         += $(HDF5_LIBS)'
CCTK_WriteLine make.extra.defn 'LIBDIRS      += $(HDF5_LIB_DIRS)'
CCTK_WriteLine make.extra.defn 'SYS_INC_DIRS += $(HDF5_INC_DIRS)'

fi
