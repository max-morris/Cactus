! $Header$

! Fortran 90 interface declarations for the routines
! which have their C declarations in cctk_Banner.h

#include "cctk.h"

module cctk_Banner
  implicit none

  interface
     
     subroutine CCTK_RegisterBanner (ierr, banner)
       implicit none
       integer      ierr
       character(*) banner
     end subroutine CCTK_RegisterBanner

  end interface

end module cctk_Banner
