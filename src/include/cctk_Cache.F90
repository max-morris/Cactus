/*@@
  @file   $RCSfile$
  @author $Author$
  @date   $Date$
  @desc
          Fortran 90 interface declarations for the routines
          which have their C declarations in cctk_Cache.h
  @enddesc
@@*/

! $Header$

#include "cctk.h"

module cctk_Cache
  implicit none

  interface

     subroutine Util_CacheMalloc (pointer, index, size, realstart)
       implicit none
       CCTK_POINTER pointer
       integer      index
       integer      size
       CCTK_POINTER realstart
     end subroutine Util_CacheMalloc

  end interface

end module cctk_Cache
