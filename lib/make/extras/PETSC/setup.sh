#! /bin/sh
#/*@@
#  @file   setup.sh
#  @date   Fri 29 Aug 2003
#  @author Thomas Radke
#  @desc
#          Setup for an external PETSc installation
#  @enddesc
# @@*/

choose_petsc=`echo $PETSC | tr '[:upper:]' '[:lower:]'`

if test "X$choose_petsc" = "Xyes" ; then

echo "Configuring with PETSc"

# Check that MPI is there
if test -z "$MPI" -o "$MPI" = 'none' ; then
  echo "  PETSc requires MPI - please configure with MPI"
  exit 2
fi

# Work out PETSc's installation directory
if test -z "$PETSC_DIR" ; then
  echo "  PETSc selected but no PETSC_DIR set. Checking some places..."
  CCTK_Search PETSC_DIR "/usr /usr/local /usr/local/petsc /usr/local/packages/petsc /usr/local/apps/petsc" include/petsc.h
  if [ -z "$PETSC_DIR" ] ; then
     echo "  Unable to locate the PETSc directory - please set PETSC_DIR"
     exit 2
  fi
  echo "  Found a PETSc package in $PETSC_DIR"
else
  echo "  Using PETSc package in $PETSC_DIR"
fi


# Check what architecture is available
if test -z "$PETSC_ARCH" ; then
  echo "  PETSC_ARCH not set... Determining suitable PETSc architecture"
  PETSC_ARCH=`/bin/ls -1 $PETSC_DIR/lib/libO | head -n1`
  echo "  Found PETSc architecture '$PETSC_ARCH'"
else
  echo "  Using PETSc architecture '$PETSC_ARCH'"
fi


# Set platform-specific libraries
if test -z "$PETSC_ARCH_LIBS" ; then
  case "$PETSC_ARCH" in
    IRIX64) PETSC_ARCH_LIBS='fpe blas complib.sgimath' ;;
    linux)  PETSC_ARCH_LIBS='flapack fblas g2c mpich'  ;;
    linux_intel) PETSC_ARCH_LIBS='mkl_lapack mkl_def guide' ;;
    linux64_intel) PETSC_ARCH_LIBS='mkl_lapack mkl guide' ;;
    rs6000_64) PETSC_ARCH_LIBS='essl' ;;
    *)           echo "  No PETSc support for architecture '$PETSC_ARCH' !"
                 echo '  Please file a bug report to cactusmaint@cactuscode.org.'
                 exit 2
  esac
else
  echo "  Using PETSc architecture-specific libraries '$PETSC_ARCH_LIBS'"
fi

# Set the PETSc libs, libdirs and includedirs
PETSC_LIB_DIRS='$(PETSC_DIR)/lib/libO/$(PETSC_ARCH)'
PETSC_INC_DIRS='$(PETSC_DIR)/include $(PETSC_DIR)/bmake/$(PETSC_ARCH)'
PETSC_LIBS="petscts petscsnes petscsles petscdm petscmat petscvec petsc $PETSC_ARCH_LIBS"


# Write the data out to the header and make files.
CCTK_WriteLine cctk_Extradefs.h "#define CCTK_PETSC 1"

CCTK_WriteLine make.extra.defn "PETSC_DIR      = $PETSC_DIR"
CCTK_WriteLine make.extra.defn "PETSC_ARCH     = $PETSC_ARCH"
CCTK_WriteLine make.extra.defn "PETSC_LIBS     = $PETSC_LIBS"
CCTK_WriteLine make.extra.defn "PETSC_LIB_DIRS = $PETSC_LIB_DIRS"
CCTK_WriteLine make.extra.defn "PETSC_INC_DIRS = $PETSC_INC_DIRS"
CCTK_WriteLine make.extra.defn ''
CCTK_WriteLine make.extra.defn 'LIBS         += $(PETSC_LIBS) X11 $(MPI_LIBS)'
CCTK_WriteLine make.extra.defn 'LIBDIRS      += $(PETSC_LIB_DIRS) $(X_LIB_DIR)'
CCTK_WriteLine make.extra.defn 'SYS_INC_DIRS += $(PETSC_INC_DIRS)'

elif test "X$choose_petsc" != "Xno" -a "X$choose_petsc" != "X"; then

  echo "  Don't understand the setting \"PETSC=$PETSC\" !"
  echo '  Please set it to either "yes" or "no", or leave it blank (same as "no") !'
  exit 1

fi
