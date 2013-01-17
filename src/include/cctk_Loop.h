#ifndef _CCTK_LOOP_H_
#define _CCTK_LOOP_H_

/* WARNING: This file is auto-generated. Do not edit. */
/* Edit cctk_Loop.h.pl instead, and then re-generate this file via */
/*    perl cctk_Loop.h.pl */
/* Documentation can also be found in "cctk_Loop.h.pl". */

#ifdef CCODE
#  include <cctk_Config.h>
#  include <cctk_WarnLevel.h>
#  include <cGH.h>
#  include <assert.h>
#endif /* #ifdef CCODE */



/* 1D */

#ifdef CCODE

/* LOOP */

#define CCTK_LOOP1_NORMAL(name, \
                          i, \
                          ni, \
                          idir, \
                          imin, \
                          imax, \
                          iash) \
  CCTK_LOOP1STR_NORMAL(name, \
                       i, \
                       ni, \
                       (idir), \
                       (imin), \
                       (imax), \
                       (iash), \
                       cctki0_imin_,cctki0_imax_, 1) \

#define CCTK_ENDLOOP1_NORMAL(name) \
  CCTK_ENDLOOP1STR_NORMAL(name) \

#define CCTK_LOOP1STR_NORMAL(name, \
                             i, \
                             ni, \
                             idir, \
                             imin_, \
                             imax_, \
                             iash, \
                             imin,imax, istr) \
  do { \
    typedef int cctki0_loop1_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_imin = (imin_); \
    int const cctki0_imax = (imax_); \
    int const cctki0_iash = (iash); \
    int const cctki0_istr = (istr); \
    int const imin = cctki0_imin; \
    int const imax = cctki0_imax; \
    _Pragma("omp for") \
    for (int i=cctki0_imin - (imin) % cctki0_istr; i<cctki0_imax; i+=cctki0_istr) { \
      int const ni CCTK_ATTRIBUTE_UNUSED = cctki0_idir<0 ? i+1 : cctki0_idir==0 ? 0 : cctki0_imax-i; \
      { \

#define CCTK_ENDLOOP1STR_NORMAL(name) \
      } \
    } \
    typedef cctki0_loop1_normal_##name cctki0_ensure_proper_nesting; \
  } while (0) \



#define CCTK_LOOP1(name, \
                   i, \
                   imin, \
                   imax, \
                   iash) \
  CCTK_LOOP1STR(name, \
                i, \
                (imin), \
                (imax), \
                (iash), \
                cctki1_imin_,cctki1_imax_, 1) \

#define CCTK_ENDLOOP1(name) \
  CCTK_ENDLOOP1STR(name) \

#define CCTK_LOOP1STR(name, \
                      i, \
                      imin_, \
                      imax_, \
                      iash, \
                      imin,imax, istr) \
  CCTK_LOOP1STR_NORMAL(name, \
                       i, \
                       cctki1_ni, \
                       0, \
                       imin_, \
                       imax_, \
                       iash, \
                       imin,imax, istr) \

#define CCTK_ENDLOOP1STR(name) \
  CCTK_ENDLOOP1STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP1_INTERIOR(name, cctkGH, \
                            i, \
                            iblo, \
                            ibhi) \
  CCTK_LOOP1STR_INTERIOR(name, (cctkGH), \
                         i, \
                         (iblo), \
                         (ibhi), \
                         cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP1_INTERIOR(name) \
  CCTK_ENDLOOP1STR_INTERIOR(name) \

#define CCTK_LOOP1STR_INTERIOR(name, cctkGH, \
                               i, \
                               iblo, \
                               ibhi, \
                               imin,imax, istr) \
  do { \
    typedef int cctki2_loop1_interior_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_INTERIOR can only be used in 1 dimensions"); \
    } \
    CCTK_LOOP1STR(name##_interior, \
                  i, \
                  (iblo), \
                  cctki2_cctkGH->cctk_lsh[0]-(ibhi), \
                  cctki2_cctkGH->cctk_ash[0], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP1STR_INTERIOR(name) \
    } CCTK_ENDLOOP1STR(name##_interior); \
    typedef cctki2_loop1_interior_##name cctki2_ensure_proper_nesting; \
  } while(0) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP1_BOUNDARIES(name, cctkGH, \
                              i, \
                              ni, \
                              iblo, \
                              ibhi, \
                              ibboxlo, \
                              ibboxhi) \
  CCTK_LOOP1STR_BOUNDARIES(name, (cctkGH), \
                           i, \
                           ni, \
                           (iblo), \
                           (ibhi), \
                           (ibboxlo), \
                           (ibboxhi), \
                           cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP1_BOUNDARIES(name) \
  CCTK_ENDLOOP1STR_BOUNDARIES(name) \

#define CCTK_LOOP1STR_BOUNDARIES(name, cctkGH, \
                                 i, \
                                 ni, \
                                 iblo, \
                                 ibhi, \
                                 ibboxlo, \
                                 ibboxhi, \
                                 imin,imax, istr) \
  do { \
    typedef int cctki2_loop1_boundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_BOUNDARIES can only be used in 1 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo) }; \
    int const cctki2_bhi[] = { (ibhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0); \
      if (cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
        }; \
        CCTK_LOOP1STR_NORMAL(name##_boundaries, \
                             i, \
                             ni, \
                             cctki2_idir, \
                             cctki2_bmin[0], \
                             cctki2_bmax[0], \
                             cctki2_cctkGH->cctk_ash[0], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP1STR_BOUNDARIES(name) \
        } CCTK_ENDLOOP1STR_NORMAL(name##_boundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    typedef cctki2_loop1_boundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP1_INTBOUNDARIES(name, cctkGH, \
                                 i, \
                                 ni, \
                                 iblo, \
                                 ibhi, \
                                 ibboxlo, \
                                 ibboxhi) \
  CCTK_LOOP1STR_INTBOUNDARIES(name, (cctkGH), \
                              i, \
                              ni, \
                              (iblo), \
                              (ibhi), \
                              (ibboxlo), \
                              (ibboxhi), \
                              cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP1_INTBOUNDARIES(name) \
  CCTK_ENDLOOP1STR_INTBOUNDARIES(name) \

#define CCTK_LOOP1STR_INTBOUNDARIES(name, cctkGH, \
                                    i, \
                                    ni, \
                                    iblo, \
                                    ibhi, \
                                    ibboxlo, \
                                    ibboxhi, \
                                    imin,imax, istr) \
  do { \
    typedef int cctki2_loop1_intboundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_INTBOUNDARIES can only be used in 1 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo) }; \
    int const cctki2_bhi[] = { (ibhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0); \
      int cctki2_all_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 1) && (cctki2_idir==+1 ? cctki2_bbox[1] : 1); \
      if (cctki2_all_bbox && cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
        }; \
        CCTK_LOOP1STR_NORMAL(name##_intboundaries, \
                             i, \
                             ni, \
                             cctki2_idir, \
                             cctki2_bmin[0], \
                             cctki2_bmax[0], \
                             cctki2_cctkGH->cctk_ash[0], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP1STR_INTBOUNDARIES(name) \
        } CCTK_ENDLOOP1STR_NORMAL(name##_intboundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    typedef cctki2_loop1_intboundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_ALL */

#define CCTK_LOOP1_ALL(name, cctkGH, \
                       i) \
  CCTK_LOOP1STR_ALL(name, (cctkGH), \
                    i, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP1_ALL(name) \
  CCTK_ENDLOOP1STR_ALL(name) \

#define CCTK_LOOP1STR_ALL(name, cctkGH, \
                          i, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop1_all_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_ALL can only be used in 1 dimensions"); \
    } \
    CCTK_LOOP1STR(name##_all, \
                  i, \
                  0, \
                  cctki3_cctkGH->cctk_lsh[0], \
                  cctki3_cctkGH->cctk_ash[0], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP1STR_ALL(name) \
    } CCTK_ENDLOOP1STR(name##_all); \
    typedef cctki3_loop1_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP1_INT(name, cctkGH, \
                       i) \
  CCTK_LOOP1STR_INT(name, (cctkGH), \
                    i, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP1_INT(name) \
  CCTK_ENDLOOP1STR_INT(name) \

#define CCTK_LOOP1STR_INT(name, cctkGH, \
                          i, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop1_int_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_INT can only be used in 1 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [2]; \
    CCTK_INT cctki3_is_ghostbnd[2]; \
    CCTK_INT cctki3_is_symbnd  [2]; \
    CCTK_INT cctki3_is_physbnd [2]; \
    _Pragma("omp single copyprivate(cctki3_bndsize)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 2, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP1STR_INTERIOR(name##_int, \
                           cctki3_cctkGH, \
                           i, \
                           cctki3_bndsize[0], \
                           cctki3_bndsize[1], \
                           imin,imax, (istr)) { \

#define CCTK_ENDLOOP1STR_INT(name) \
    } CCTK_ENDLOOP1STR_INTERIOR(name##_int); \
    typedef cctki3_loop1_int_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_BND */

#define CCTK_LOOP1_BND(name, cctkGH, \
                       i, \
                       ni) \
  CCTK_LOOP1STR_BND(name, (cctkGH), \
                    i, \
                    ni, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP1_BND(name) \
  CCTK_ENDLOOP1STR_BND(name) \

#define CCTK_LOOP1STR_BND(name, cctkGH, \
                          i, \
                          ni, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop1_bnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_BND can only be used in 1 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [2]; \
    CCTK_INT cctki3_is_ghostbnd[2]; \
    CCTK_INT cctki3_is_symbnd  [2]; \
    CCTK_INT cctki3_is_physbnd [2]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 2, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP1STR_BOUNDARIES(name##_bnd, \
                             cctki3_cctkGH, \
                             i, \
                             ni, \
                             cctki3_bndsize[0], \
                             cctki3_bndsize[1], \
                             cctki3_is_physbnd[0], \
                             cctki3_is_physbnd[1], \
                             imin,imax, (istr)) { \

#define CCTK_ENDLOOP1STR_BND(name) \
    } CCTK_ENDLOOP1STR_BOUNDARIES(name##_bnd); \
    typedef cctki3_loop1_bnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBND */

#define CCTK_LOOP1_INTBND(name, cctkGH, \
                           i, \
                           ni) \
  CCTK_LOOP1STR_INTBND(name, (cctkGH), \
                        i, \
                        ni, \
                        cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP1_INTBND(name) \
  CCTK_ENDLOOP1STR_INTBND(name) \

#define CCTK_LOOP1STR_INTBND(name, cctkGH, \
                              i, \
                              ni, \
                              imin,imax, istr) \
  do { \
    typedef int cctki3_loop1_intbnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_INTBND can only be used in 1 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [2]; \
    CCTK_INT cctki3_is_ghostbnd[2]; \
    CCTK_INT cctki3_is_symbnd  [2]; \
    CCTK_INT cctki3_is_physbnd [2]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 2, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP1STR_INTBOUNDARIES(name##_intbnd, \
                                cctki3_cctkGH, \
                                i, \
                                ni, \
                                cctki3_bndsize[0], \
                                cctki3_bndsize[1], \
                                cctki3_is_physbnd[0], \
                                cctki3_is_physbnd[1], \
                                imin,imax, (istr)) { \

#define CCTK_ENDLOOP1STR_INTBND(name) \
    } CCTK_ENDLOOP1STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop1_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP1_NORMAL_DECLARE(name) \
   CCTK_LOOP1STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_imin_, name/**/0_imax_ \

#define CCTK_LOOP1_NORMAL_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP1_NORMAL(name, \
                          i, \
                          ni, \
                          idir, \
                          imin, \
                          imax, \
                          iash) \
   CCTK_LOOP1STR_NORMAL(name, \
                        i, \
                        ni, \
                        idir, \
                        imin, \
                        imax, \
                        iash, \
                        name/**/0_imin_,name/**/0_imax_, 1) \

#define CCTK_ENDLOOP1_NORMAL(name) \
   CCTK_ENDLOOP1STR_NORMAL(name) \

#define CCTK_LOOP1STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_idir \
   && integer :: name/**/0_imin \
   && integer :: name/**/0_imax \
   && integer :: name/**/0_iash \
   && integer :: name/**/0_istr \

#define CCTK_LOOP1STR_NORMAL_OMP_PRIVATE(name) \
   && !$omp private (i) \
   && !$omp private (ni) \

#define CCTK_LOOP1STR_NORMAL(name, \
                             i, \
                             ni, \
                             idir, \
                             imin_, \
                             imax_, \
                             iash, \
                             imin,imax, istr) \
   && name/**/0_idir = idir \
   && name/**/0_imin = imin_ \
   && name/**/0_imax = imax_ \
   && name/**/0_iash = iash \
   && name/**/0_istr = istr \
   && imin = name/**/0_imin \
   && imax = name/**/0_imax \
   && !$omp do \
   && do i = name/**/0_imin - modulo((imin), name/**/0_istr), name/**/0_imax \
   &&    if (name/**/0_idir< 0) ni = i \
   &&    if (name/**/0_idir==0) ni = 0 \
   &&    if (name/**/0_idir> 0) ni = name/**/0_imax+1-i \

#define CCTK_ENDLOOP1STR_NORMAL(name) \
   && end do \



#define CCTK_LOOP1_DECLARE(name) \
   CCTK_LOOP1STR_DECLARE(name) \
   && integer :: name/**/1_imin_, name/**/1_imax_ \

#define CCTK_LOOP1_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_OMP_PRIVATE(name) \

#define CCTK_LOOP1(name, \
                   i, \
                   imin, \
                   imax, \
                   iash) \
   CCTK_LOOP1STR(name, \
                 i, \
                 imin, \
                 imax, \
                 iash, \
                 name/**/1_imin_,name/**/1_imax_, 1) \

#define CCTK_ENDLOOP1(name) \
   CCTK_ENDLOOP1STR(name) \

#define CCTK_LOOP1STR_DECLARE(name) \
   CCTK_LOOP1STR_NORMAL_DECLARE(name) \
   && integer :: name/**/1_ni \

#define CCTK_LOOP1STR_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP1STR(name, \
                      i, \
                      imin_, \
                      imax_, \
                      iash, \
                      imin,imax, istr) \
   CCTK_LOOP1STR_NORMAL(name, \
                        i, \
                        name/**/1_ni, \
                        0, \
                        imin_, \
                        imax_, \
                        iash, \
                        imin,imax, istr) \

#define CCTK_ENDLOOP1STR(name) \
   CCTK_ENDLOOP1STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP1_INTERIOR_DECLARE(name) \
   CCTK_LOOP1STR_INTERIOR_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP1_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_INTERIOR_OMP_PRIVATE(name) \

#define CCTK_LOOP1_INTERIOR(name, \
                            i, \
                            iblo, \
                            ibhi) \
   CCTK_LOOP1STR_INTERIOR(name, \
                          i, \
                          iblo, \
                          ibhi, \
                          name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP1_INTERIOR(name) \
   CCTK_ENDLOOP1STR_INTERIOR(name) \

#define CCTK_LOOP1STR_INTERIOR_DECLARE(name) \
   CCTK_LOOP1STR_DECLARE(name/**/_interior) \

#define CCTK_LOOP1STR_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_OMP_PRIVATE(name/**/_interior) \

#define CCTK_LOOP1STR_INTERIOR(name, \
                               i, \
                               iblo, \
                               ibhi, \
                               imin,imax, istr) \
   CCTK_LOOP1STR(name/**/_interior, \
                 i, \
                 (iblo)+1, \
                 cctk_lsh(1)-(ibhi), \
                 cctk_ash(1), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP1STR_INTERIOR(name) \
   CCTK_ENDLOOP1STR(name/**/_interior) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP1_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP1STR_BOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP1_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_BOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP1_BOUNDARIES(name, \
                              i, \
                              ni, \
                              iblo, \
                              ibhi, \
                              ibboxlo, \
                              ibboxhi) \
   CCTK_LOOP1STR_BOUNDARIES(name, \
                            i, \
                            ni, \
                            iblo, \
                            ibhi, \
                            ibboxlo, \
                            ibboxhi, \
                            name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP1_BOUNDARIES(name) \
   CCTK_ENDLOOP1STR_BOUNDARIES(name) \

#define CCTK_LOOP1STR_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP1STR_NORMAL_DECLARE(name/**/_boundaries) \
   && integer :: name/**/2_blo(1), name/**/2_bhi(1) \
   && integer :: name/**/2_bboxlo(1), name/**/2_bboxhi(1) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && logical :: name/**/2_any_bbox \
   && integer :: name/**/2_bmin(1), name/**/2_bmax(1) \

#define CCTK_LOOP1STR_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_NORMAL_OMP_PRIVATE(name/**/_boundaries) \

#define CCTK_LOOP1STR_BOUNDARIES(name, \
                                 i, \
                                 ni, \
                                 iblo, \
                                 ibhi, \
                                 ibboxlo, \
                                 ibboxhi, \
                                 imin,imax, istr) \
   && name/**/2_blo = (/ iblo /) \
   && name/**/2_bhi = (/ ibhi /) \
   && name/**/2_bboxlo = (/ ibboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&    if (name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
            CCTK_LOOP1STR_NORMAL(name/**/_boundaries, \
                                 i, \
                                 ni, \
                                 name/**/2_idir, \
                                 name/**/2_bmin(1), \
                                 name/**/2_bmax(1), \
                                 cctk_ash(1), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP1STR_BOUNDARIES(name) \
            CCTK_ENDLOOP1STR_NORMAL(name/**/_boundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP1_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP1STR_INTBOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP1_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_INTBOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP1_INTBOUNDARIES(name, \
                                 i, \
                                 ni, \
                                 iblo, \
                                 ibhi, \
                                 ibboxlo, \
                                 ibboxhi) \
   CCTK_LOOP1STR_INTBOUNDARIES(name, \
                               i, \
                               ni, \
                               iblo, \
                               ibhi, \
                               ibboxlo, \
                               ibboxhi, \
                               name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP1_INTBOUNDARIES(name) \
   CCTK_ENDLOOP1STR_INTBOUNDARIES(name) \

#define CCTK_LOOP1STR_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP1STR_NORMAL_DECLARE(name/**/_intboundaries) \
   && integer :: name/**/2_blo(1), name/**/2_bhi(1) \
   && integer :: name/**/2_bboxlo(1), name/**/2_bboxhi(1) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && logical :: name/**/2_any_bbox,  name/**/2_all_bbox \
   && integer :: name/**/2_bmin(1), name/**/2_bmax(1) \

#define CCTK_LOOP1STR_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_NORMAL_OMP_PRIVATE(name/**/_intboundaries) \

#define CCTK_LOOP1STR_INTBOUNDARIES(name, \
                                    i, \
                                    ni, \
                                    iblo, \
                                    ibhi, \
                                    ibboxlo, \
                                    ibboxhi, \
                                    imin,imax, istr) \
   && name/**/2_blo = (/ iblo /) \
   && name/**/2_bhi = (/ ibhi /) \
   && name/**/2_bboxlo = (/ ibboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     name/**/2_all_bbox = .true. \
   &&     if (name/**/2_idir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(1) /= 0 \
   &&    if (name/**/2_all_bbox .and. name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
            CCTK_LOOP1STR_NORMAL(name/**/_intboundaries, \
                                 i, \
                                 ni, \
                                 name/**/2_idir, \
                                 name/**/2_bmin(1), \
                                 name/**/2_bmax(1), \
                                 cctk_ash(1), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP1STR_INTBOUNDARIES(name) \
            CCTK_ENDLOOP1STR_NORMAL(name/**/_intboundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \



/* LOOP_ALL */

#define CCTK_LOOP1_ALL_DECLARE(name) \
   CCTK_LOOP1STR_ALL_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP1_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_ALL_OMP_PRIVATE(name) \

#define CCTK_LOOP1_ALL(name, \
                       i) \
   CCTK_LOOP1STR_ALL(name, \
                     i, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP1_ALL(name) \
   CCTK_ENDLOOP1STR_ALL(name) \

#define CCTK_LOOP1STR_ALL_DECLARE(name) \
   CCTK_LOOP1STR_DECLARE(name/**/_all) \

#define CCTK_LOOP1STR_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_OMP_PRIVATE(name/**/_all) \

#define CCTK_LOOP1STR_ALL(name, \
                          i, \
                          imin,imax, istr) \
   CCTK_LOOP1STR(name/**/_all, \
                 i, \
                 1, \
                 cctk_lsh(1), \
                 cctk_ash(1), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP1STR_ALL(name) \
   CCTK_ENDLOOP1STR(name/**/_all) \



/* LOOP_INT */

#define CCTK_LOOP1_INT_DECLARE(name) \
   CCTK_LOOP1STR_INT_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP1_INT_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_INT_OMP_PRIVATE(name) \

#define CCTK_LOOP1_INT(name, \
                        i) \
   CCTK_LOOP1STR_INT(name, \
                      i, \
                      name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP1_INT(name) \
   CCTK_ENDLOOP1STR_INT(name) \

#define CCTK_LOOP1STR_INT_DECLARE(name) \
   CCTK_LOOP1STR_INTERIOR_DECLARE(name/**/_int) \
   && integer :: name/**/3_bndsize    (2) \
   && integer :: name/**/3_is_ghostbnd(2) \
   && integer :: name/**/3_is_symbnd  (2) \
   && integer :: name/**/3_is_physbnd (2) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP1STR_INT_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_INTERIOR_OMP_PRIVATE(name/**/_int) \

#define CCTK_LOOP1STR_INT(name, \
                          i, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 2, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize) \
      CCTK_LOOP1STR_INTERIOR(name/**/_int, \
                             i, \
                             name/**/3_bndsize(1+1), \
                             name/**/3_bndsize(2), \
                             imin,imax, (istr)) \

#define CCTK_ENDLOOP1STR_INT(name) \
      CCTK_ENDLOOP1STR_INTERIOR(name/**/int) \



/* LOOP_BND */

#define CCTK_LOOP1_BND_DECLARE(name) \
   CCTK_LOOP1STR_BND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP1_BND_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_BND_OMP_PRIVATE(name) \

#define CCTK_LOOP1_BND(name, \
                       i, \
                       ni) \
   CCTK_LOOP1STR_BND(name, \
                     i, \
                     ni, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP1_BND(name) \
   CCTK_ENDLOOP1STR_BND(name) \

#define CCTK_LOOP1STR_BND_DECLARE(name) \
   CCTK_LOOP1STR_BOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (2) \
   && integer :: name/**/3_is_ghostbnd(2) \
   && integer :: name/**/3_is_symbnd  (2) \
   && integer :: name/**/3_is_physbnd (2) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP1STR_BND_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_BOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP1STR_BND(name, \
                          i, \
                          ni, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 2, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP1STR_BOUNDARIES(name/**/_bnd, \
                               i, \
                               ni, \
                               name/**/3_bndsize(1)+1, \
                               name/**/3_bndsize(2), \
                               name/**/3_is_physbnd(1), \
                               name/**/3_is_physbnd(2), \
                               imin,imax, (istr)) \

#define CCTK_ENDLOOP1STR_BND(name) \
      CCTK_ENDLOOP1STR_BOUNDARIES(name/**/_bnd) \






/* LOOP_INTBND */

#define CCTK_LOOP1_INTBND_DECLARE(name) \
   CCTK_LOOP1STR_INTBND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP1_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_INTBND_OMP_PRIVATE(name) \

#define CCTK_LOOP1_INTBND(name, \
                          i, \
                          ni) \
   CCTK_LOOP1STR_INTBND(name, \
                        i, \
                        ni, \
                        name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP1_INTBND(name) \
   CCTK_ENDLOOP1STR_INTBND(name) \

#define CCTK_LOOP1STR_INTBND_DECLARE(name) \
   CCTK_LOOP1STR_INTBOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (2) \
   && integer :: name/**/3_is_ghostbnd(2) \
   && integer :: name/**/3_is_symbnd  (2) \
   && integer :: name/**/3_is_physbnd (2) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP1STR_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP1STR_INTBOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP1STR_INTBND(name, \
                             i, \
                             ni, \
                             imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 2, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP1STR_INTBOUNDARIES(name/**/_bnd, \
                                  i, \
                                  ni, \
                                  name/**/3_bndsize(1+1), \
                                  name/**/3_bndsize(2), \
                                  name/**/3_is_physbnd(1), \
                                  name/**/3_is_physbnd(2), \
                                  imin,imax, (istr)) \

#define CCTK_ENDLOOP1STR_INTBND(name) \
      CCTK_ENDLOOP1STR_INTBOUNDARIES(name/**/_bnd) \

#endif /* #ifdef FCODE */



/* 2D */

#ifdef CCODE

/* LOOP */

#define CCTK_LOOP2_NORMAL(name, \
                          i,j, \
                          ni,nj, \
                          idir,jdir, \
                          imin,jmin, \
                          imax,jmax, \
                          iash,jash) \
  CCTK_LOOP2STR_NORMAL(name, \
                       i,j, \
                       ni,nj, \
                       (idir),(jdir), \
                       (imin),(jmin), \
                       (imax),(jmax), \
                       (iash),(jash), \
                       cctki0_imin_,cctki0_imax_, 1) \

#define CCTK_ENDLOOP2_NORMAL(name) \
  CCTK_ENDLOOP2STR_NORMAL(name) \

#define CCTK_LOOP2STR_NORMAL(name, \
                             i,j, \
                             ni,nj, \
                             idir,jdir, \
                             imin_,jmin_, \
                             imax_,jmax_, \
                             iash,jash, \
                             imin,imax, istr) \
  do { \
    typedef int cctki0_loop2_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_jdir = (jdir); \
    int const cctki0_imin = (imin_); \
    int const cctki0_jmin = (jmin_); \
    int const cctki0_imax = (imax_); \
    int const cctki0_jmax = (jmax_); \
    int const cctki0_iash = (iash); \
    int const cctki0_jash = (jash); \
    int const cctki0_istr = (istr); \
    int const imin = cctki0_imin; \
    int const imax = cctki0_imax; \
    _Pragma("omp for collapse(1)") \
    for (int j=cctki0_jmin; j<cctki0_jmax; ++j) { \
    for (int i=cctki0_imin - (imin+cctki0_iash*(j)) % cctki0_istr; i<cctki0_imax; i+=cctki0_istr) { \
      int const ni CCTK_ATTRIBUTE_UNUSED = cctki0_idir<0 ? i+1 : cctki0_idir==0 ? 0 : cctki0_imax-i; \
      int const nj CCTK_ATTRIBUTE_UNUSED = cctki0_jdir<0 ? j+1 : cctki0_jdir==0 ? 0 : cctki0_jmax-j; \
      { \

#define CCTK_ENDLOOP2STR_NORMAL(name) \
      } \
    } \
    } \
    typedef cctki0_loop2_normal_##name cctki0_ensure_proper_nesting; \
  } while (0) \



#define CCTK_LOOP2(name, \
                   i,j, \
                   imin,jmin, \
                   imax,jmax, \
                   iash,jash) \
  CCTK_LOOP2STR(name, \
                i,j, \
                (imin),(jmin), \
                (imax),(jmax), \
                (iash),(jash), \
                cctki1_imin_,cctki1_imax_, 1) \

#define CCTK_ENDLOOP2(name) \
  CCTK_ENDLOOP2STR(name) \

#define CCTK_LOOP2STR(name, \
                      i,j, \
                      imin_,jmin_, \
                      imax_,jmax_, \
                      iash,jash, \
                      imin,imax, istr) \
  CCTK_LOOP2STR_NORMAL(name, \
                       i,j, \
                       cctki1_ni,cctki1_nj, \
                       0,0, \
                       imin_,jmin_, \
                       imax_,jmax_, \
                       iash,jash, \
                       imin,imax, istr) \

#define CCTK_ENDLOOP2STR(name) \
  CCTK_ENDLOOP2STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP2_INTERIOR(name, cctkGH, \
                            i,j, \
                            iblo,jblo, \
                            ibhi,jbhi) \
  CCTK_LOOP2STR_INTERIOR(name, (cctkGH), \
                         i,j, \
                         (iblo),(jblo), \
                         (ibhi),(jbhi), \
                         cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP2_INTERIOR(name) \
  CCTK_ENDLOOP2STR_INTERIOR(name) \

#define CCTK_LOOP2STR_INTERIOR(name, cctkGH, \
                               i,j, \
                               iblo,jblo, \
                               ibhi,jbhi, \
                               imin,imax, istr) \
  do { \
    typedef int cctki2_loop2_interior_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_INTERIOR can only be used in 2 dimensions"); \
    } \
    CCTK_LOOP2STR(name##_interior, \
                  i,j, \
                  (iblo),(jblo), \
                  cctki2_cctkGH->cctk_lsh[0]-(ibhi), \
                  cctki2_cctkGH->cctk_lsh[1]-(jbhi), \
                  cctki2_cctkGH->cctk_ash[0], \
                  cctki2_cctkGH->cctk_ash[1], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP2STR_INTERIOR(name) \
    } CCTK_ENDLOOP2STR(name##_interior); \
    typedef cctki2_loop2_interior_##name cctki2_ensure_proper_nesting; \
  } while(0) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP2_BOUNDARIES(name, cctkGH, \
                              i,j, \
                              ni,nj, \
                              iblo,jblo, \
                              ibhi,jbhi, \
                              ibboxlo,jbboxlo, \
                              ibboxhi,jbboxhi) \
  CCTK_LOOP2STR_BOUNDARIES(name, (cctkGH), \
                           i,j, \
                           ni,nj, \
                           (iblo),(jblo), \
                           (ibhi),(jbhi), \
                           (ibboxlo),(jbboxlo), \
                           (ibboxhi),(jbboxhi), \
                           cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP2_BOUNDARIES(name) \
  CCTK_ENDLOOP2STR_BOUNDARIES(name) \

#define CCTK_LOOP2STR_BOUNDARIES(name, cctkGH, \
                                 i,j, \
                                 ni,nj, \
                                 iblo,jblo, \
                                 ibhi,jbhi, \
                                 ibboxlo,jbboxlo, \
                                 ibboxhi,jbboxhi, \
                                 imin,imax, istr) \
  do { \
    typedef int cctki2_loop2_boundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_BOUNDARIES can only be used in 2 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_jdir=-1; cctki2_jdir<=+1; ++cctki2_jdir) { \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0)|| \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 0) || (cctki2_jdir==+1 ? cctki2_bbox[3] : 0); \
      if (cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
          cctki2_jdir==-1 ? 0 : cctki2_jdir==0 ? cctki2_blo[1] : cctki2_lsh[1] - cctki2_bhi[1], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
          cctki2_jdir==-1 ? cctki2_blo[1] : cctki2_jdir==0 ? cctki2_lsh[1] - cctki2_bhi[1] : cctki2_lsh[1], \
        }; \
        CCTK_LOOP2STR_NORMAL(name##_boundaries, \
                             i,j, \
                             ni,nj, \
                             cctki2_idir,cctki2_jdir, \
                             cctki2_bmin[0],cctki2_bmin[1], \
                             cctki2_bmax[0],cctki2_bmax[1], \
                             cctki2_cctkGH->cctk_ash[0], \
                             cctki2_cctkGH->cctk_ash[1], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP2STR_BOUNDARIES(name) \
        } CCTK_ENDLOOP2STR_NORMAL(name##_boundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    } /* for dir */ \
    typedef cctki2_loop2_boundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP2_INTBOUNDARIES(name, cctkGH, \
                                 i,j, \
                                 ni,nj, \
                                 iblo,jblo, \
                                 ibhi,jbhi, \
                                 ibboxlo,jbboxlo, \
                                 ibboxhi,jbboxhi) \
  CCTK_LOOP2STR_INTBOUNDARIES(name, (cctkGH), \
                              i,j, \
                              ni,nj, \
                              (iblo),(jblo), \
                              (ibhi),(jbhi), \
                              (ibboxlo),(jbboxlo), \
                              (ibboxhi),(jbboxhi), \
                              cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP2_INTBOUNDARIES(name) \
  CCTK_ENDLOOP2STR_INTBOUNDARIES(name) \

#define CCTK_LOOP2STR_INTBOUNDARIES(name, cctkGH, \
                                    i,j, \
                                    ni,nj, \
                                    iblo,jblo, \
                                    ibhi,jbhi, \
                                    ibboxlo,jbboxlo, \
                                    ibboxhi,jbboxhi, \
                                    imin,imax, istr) \
  do { \
    typedef int cctki2_loop2_intboundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_INTBOUNDARIES can only be used in 2 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_jdir=-1; cctki2_jdir<=+1; ++cctki2_jdir) { \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0)|| \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 0) || (cctki2_jdir==+1 ? cctki2_bbox[3] : 0); \
      int cctki2_all_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 1) && (cctki2_idir==+1 ? cctki2_bbox[1] : 1)&& \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 1) && (cctki2_jdir==+1 ? cctki2_bbox[3] : 1); \
      if (cctki2_all_bbox && cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
          cctki2_jdir==-1 ? 0 : cctki2_jdir==0 ? cctki2_blo[1] : cctki2_lsh[1] - cctki2_bhi[1], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
          cctki2_jdir==-1 ? cctki2_blo[1] : cctki2_jdir==0 ? cctki2_lsh[1] - cctki2_bhi[1] : cctki2_lsh[1], \
        }; \
        CCTK_LOOP2STR_NORMAL(name##_intboundaries, \
                             i,j, \
                             ni,nj, \
                             cctki2_idir,cctki2_jdir, \
                             cctki2_bmin[0],cctki2_bmin[1], \
                             cctki2_bmax[0],cctki2_bmax[1], \
                             cctki2_cctkGH->cctk_ash[0], \
                             cctki2_cctkGH->cctk_ash[1], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP2STR_INTBOUNDARIES(name) \
        } CCTK_ENDLOOP2STR_NORMAL(name##_intboundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    } /* for dir */ \
    typedef cctki2_loop2_intboundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_ALL */

#define CCTK_LOOP2_ALL(name, cctkGH, \
                       i,j) \
  CCTK_LOOP2STR_ALL(name, (cctkGH), \
                    i,j, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP2_ALL(name) \
  CCTK_ENDLOOP2STR_ALL(name) \

#define CCTK_LOOP2STR_ALL(name, cctkGH, \
                          i,j, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop2_all_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_ALL can only be used in 2 dimensions"); \
    } \
    CCTK_LOOP2STR(name##_all, \
                  i,j, \
                  0,0, \
                  cctki3_cctkGH->cctk_lsh[0], \
                  cctki3_cctkGH->cctk_lsh[1], \
                  cctki3_cctkGH->cctk_ash[0], \
                  cctki3_cctkGH->cctk_ash[1], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP2STR_ALL(name) \
    } CCTK_ENDLOOP2STR(name##_all); \
    typedef cctki3_loop2_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP2_INT(name, cctkGH, \
                       i,j) \
  CCTK_LOOP2STR_INT(name, (cctkGH), \
                    i,j, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP2_INT(name) \
  CCTK_ENDLOOP2STR_INT(name) \

#define CCTK_LOOP2STR_INT(name, cctkGH, \
                          i,j, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop2_int_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_INT can only be used in 2 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [4]; \
    CCTK_INT cctki3_is_ghostbnd[4]; \
    CCTK_INT cctki3_is_symbnd  [4]; \
    CCTK_INT cctki3_is_physbnd [4]; \
    _Pragma("omp single copyprivate(cctki3_bndsize)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 4, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP2STR_INTERIOR(name##_int, \
                           cctki3_cctkGH, \
                           i,j, \
                           cctki3_bndsize[0],cctki3_bndsize[2], \
                           cctki3_bndsize[1],cctki3_bndsize[3], \
                           imin,imax, (istr)) { \

#define CCTK_ENDLOOP2STR_INT(name) \
    } CCTK_ENDLOOP2STR_INTERIOR(name##_int); \
    typedef cctki3_loop2_int_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_BND */

#define CCTK_LOOP2_BND(name, cctkGH, \
                       i,j, \
                       ni,nj) \
  CCTK_LOOP2STR_BND(name, (cctkGH), \
                    i,j, \
                    ni,nj, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP2_BND(name) \
  CCTK_ENDLOOP2STR_BND(name) \

#define CCTK_LOOP2STR_BND(name, cctkGH, \
                          i,j, \
                          ni,nj, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop2_bnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_BND can only be used in 2 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [4]; \
    CCTK_INT cctki3_is_ghostbnd[4]; \
    CCTK_INT cctki3_is_symbnd  [4]; \
    CCTK_INT cctki3_is_physbnd [4]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 4, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP2STR_BOUNDARIES(name##_bnd, \
                             cctki3_cctkGH, \
                             i,j, \
                             ni,nj, \
                             cctki3_bndsize[0],cctki3_bndsize[2], \
                             cctki3_bndsize[1],cctki3_bndsize[3], \
                             cctki3_is_physbnd[0],cctki3_is_physbnd[2], \
                             cctki3_is_physbnd[1],cctki3_is_physbnd[3], \
                             imin,imax, (istr)) { \

#define CCTK_ENDLOOP2STR_BND(name) \
    } CCTK_ENDLOOP2STR_BOUNDARIES(name##_bnd); \
    typedef cctki3_loop2_bnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBND */

#define CCTK_LOOP2_INTBND(name, cctkGH, \
                           i,j, \
                           ni,nj) \
  CCTK_LOOP2STR_INTBND(name, (cctkGH), \
                        i,j, \
                        ni,nj, \
                        cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP2_INTBND(name) \
  CCTK_ENDLOOP2STR_INTBND(name) \

#define CCTK_LOOP2STR_INTBND(name, cctkGH, \
                              i,j, \
                              ni,nj, \
                              imin,imax, istr) \
  do { \
    typedef int cctki3_loop2_intbnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_INTBND can only be used in 2 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [4]; \
    CCTK_INT cctki3_is_ghostbnd[4]; \
    CCTK_INT cctki3_is_symbnd  [4]; \
    CCTK_INT cctki3_is_physbnd [4]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 4, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP2STR_INTBOUNDARIES(name##_intbnd, \
                                cctki3_cctkGH, \
                                i,j, \
                                ni,nj, \
                                cctki3_bndsize[0],cctki3_bndsize[2], \
                                cctki3_bndsize[1],cctki3_bndsize[3], \
                                cctki3_is_physbnd[0],cctki3_is_physbnd[2], \
                                cctki3_is_physbnd[1],cctki3_is_physbnd[3], \
                                imin,imax, (istr)) { \

#define CCTK_ENDLOOP2STR_INTBND(name) \
    } CCTK_ENDLOOP2STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop2_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP2_NORMAL_DECLARE(name) \
   CCTK_LOOP2STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_imin_, name/**/0_imax_ \

#define CCTK_LOOP2_NORMAL_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP2_NORMAL(name, \
                          i,j, \
                          ni,nj, \
                          idir,jdir, \
                          imin,jmin, \
                          imax,jmax, \
                          iash,jash) \
   CCTK_LOOP2STR_NORMAL(name, \
                        i,j, \
                        ni,nj, \
                        idir,jdir, \
                        imin,jmin, \
                        imax,jmax, \
                        iash,jash, \
                        name/**/0_imin_,name/**/0_imax_, 1) \

#define CCTK_ENDLOOP2_NORMAL(name) \
   CCTK_ENDLOOP2STR_NORMAL(name) \

#define CCTK_LOOP2STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_idir,name/**/0_jdir \
   && integer :: name/**/0_imin,name/**/0_jmin \
   && integer :: name/**/0_imax,name/**/0_jmax \
   && integer :: name/**/0_iash,name/**/0_jash \
   && integer :: name/**/0_istr \

#define CCTK_LOOP2STR_NORMAL_OMP_PRIVATE(name) \
   && !$omp private (i,j) \
   && !$omp private (ni,nj) \

#define CCTK_LOOP2STR_NORMAL(name, \
                             i,j, \
                             ni,nj, \
                             idir,jdir, \
                             imin_,jmin_, \
                             imax_,jmax_, \
                             iash,jash, \
                             imin,imax, istr) \
   && name/**/0_idir = idir \
   && name/**/0_jdir = jdir \
   && name/**/0_imin = imin_ \
   && name/**/0_jmin = jmin_ \
   && name/**/0_imax = imax_ \
   && name/**/0_jmax = jmax_ \
   && name/**/0_iash = iash \
   && name/**/0_jash = jash \
   && name/**/0_istr = istr \
   && imin = name/**/0_imin \
   && imax = name/**/0_imax \
   && !$omp do collapse(1) \
   && do j = name/**/0_jmin, name/**/0_jmax \
   && do i = name/**/0_imin - modulo((imin+name/**/0_iash*(j)), name/**/0_istr), name/**/0_imax \
   &&    if (name/**/0_idir< 0) ni = i \
   &&    if (name/**/0_jdir< 0) nj = j \
   &&    if (name/**/0_idir==0) ni = 0 \
   &&    if (name/**/0_jdir==0) nj = 0 \
   &&    if (name/**/0_idir> 0) ni = name/**/0_imax+1-i \
   &&    if (name/**/0_jdir> 0) nj = name/**/0_jmax+1-j \

#define CCTK_ENDLOOP2STR_NORMAL(name) \
   && end do \
   && end do \



#define CCTK_LOOP2_DECLARE(name) \
   CCTK_LOOP2STR_DECLARE(name) \
   && integer :: name/**/1_imin_, name/**/1_imax_ \

#define CCTK_LOOP2_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_OMP_PRIVATE(name) \

#define CCTK_LOOP2(name, \
                   i,j, \
                   imin,jmin, \
                   imax,jmax, \
                   iash,jash) \
   CCTK_LOOP2STR(name, \
                 i,j, \
                 imin,jmin, \
                 imax,jmax, \
                 iash,jash, \
                 name/**/1_imin_,name/**/1_imax_, 1) \

#define CCTK_ENDLOOP2(name) \
   CCTK_ENDLOOP2STR(name) \

#define CCTK_LOOP2STR_DECLARE(name) \
   CCTK_LOOP2STR_NORMAL_DECLARE(name) \
   && integer :: name/**/1_ni,name/**/1_nj \

#define CCTK_LOOP2STR_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP2STR(name, \
                      i,j, \
                      imin_,jmin_, \
                      imax_,jmax_, \
                      iash,jash, \
                      imin,imax, istr) \
   CCTK_LOOP2STR_NORMAL(name, \
                        i,j, \
                        name/**/1_ni,name/**/1_nj, \
                        0,0, \
                        imin_,jmin_, \
                        imax_,jmax_, \
                        iash,jash, \
                        imin,imax, istr) \

#define CCTK_ENDLOOP2STR(name) \
   CCTK_ENDLOOP2STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP2_INTERIOR_DECLARE(name) \
   CCTK_LOOP2STR_INTERIOR_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP2_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_INTERIOR_OMP_PRIVATE(name) \

#define CCTK_LOOP2_INTERIOR(name, \
                            i,j, \
                            iblo,jblo, \
                            ibhi,jbhi) \
   CCTK_LOOP2STR_INTERIOR(name, \
                          i,j, \
                          iblo,jblo, \
                          ibhi,jbhi, \
                          name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP2_INTERIOR(name) \
   CCTK_ENDLOOP2STR_INTERIOR(name) \

#define CCTK_LOOP2STR_INTERIOR_DECLARE(name) \
   CCTK_LOOP2STR_DECLARE(name/**/_interior) \

#define CCTK_LOOP2STR_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_OMP_PRIVATE(name/**/_interior) \

#define CCTK_LOOP2STR_INTERIOR(name, \
                               i,j, \
                               iblo,jblo, \
                               ibhi,jbhi, \
                               imin,imax, istr) \
   CCTK_LOOP2STR(name/**/_interior, \
                 i,j, \
                 (iblo)+1, \
                 (jblo)+1, \
                 cctk_lsh(1)-(ibhi), \
                 cctk_lsh(2)-(jbhi), \
                 cctk_ash(1),cctk_ash(2), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP2STR_INTERIOR(name) \
   CCTK_ENDLOOP2STR(name/**/_interior) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP2_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP2STR_BOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP2_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_BOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP2_BOUNDARIES(name, \
                              i,j, \
                              ni,nj, \
                              iblo,jblo, \
                              ibhi,jbhi, \
                              ibboxlo,jbboxlo, \
                              ibboxhi,jbboxhi) \
   CCTK_LOOP2STR_BOUNDARIES(name, \
                            i,j, \
                            ni,nj, \
                            iblo,jblo, \
                            ibhi,jbhi, \
                            ibboxlo,jbboxlo, \
                            ibboxhi,jbboxhi, \
                            name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP2_BOUNDARIES(name) \
   CCTK_ENDLOOP2STR_BOUNDARIES(name) \

#define CCTK_LOOP2STR_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP2STR_NORMAL_DECLARE(name/**/_boundaries) \
   && integer :: name/**/2_blo(2), name/**/2_bhi(2) \
   && integer :: name/**/2_bboxlo(2), name/**/2_bboxhi(2) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && integer :: name/**/2_jdir \
   && logical :: name/**/2_any_bbox \
   && integer :: name/**/2_bmin(2), name/**/2_bmax(2) \

#define CCTK_LOOP2STR_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_NORMAL_OMP_PRIVATE(name/**/_boundaries) \

#define CCTK_LOOP2STR_BOUNDARIES(name, \
                                 i,j, \
                                 ni,nj, \
                                 iblo,jblo, \
                                 ibhi,jbhi, \
                                 ibboxlo,jbboxlo, \
                                 ibboxhi,jbboxhi, \
                                 imin,imax, istr) \
   && name/**/2_blo = (/ iblo,jblo /) \
   && name/**/2_bhi = (/ ibhi,jbhi /) \
   && name/**/2_bboxlo = (/ ibboxlo,jbboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi,jbboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_jdir=-1, +1 \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(2) /= 0 \
   &&    if (name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_jdir==-1) name/**/2_bmin(2) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_jdir== 0) name/**/2_bmin(2) = name/**/2_blo(2)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmin(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_jdir==-1) name/**/2_bmax(2) = name/**/2_blo(2) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir== 0) name/**/2_bmax(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmax(2) = cctk_lsh(2) \
            CCTK_LOOP2STR_NORMAL(name/**/_boundaries, \
                                 i,j, \
                                 ni,nj, \
                                 name/**/2_idir,name/**/2_jdir, \
                                 name/**/2_bmin(1),name/**/2_bmin(2), \
                                 name/**/2_bmax(1),name/**/2_bmax(2), \
                                 cctk_ash(1), \
                                 cctk_ash(2), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP2STR_BOUNDARIES(name) \
            CCTK_ENDLOOP2STR_NORMAL(name/**/_boundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \
   && end do /* dir */ \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP2_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP2STR_INTBOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP2_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_INTBOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP2_INTBOUNDARIES(name, \
                                 i,j, \
                                 ni,nj, \
                                 iblo,jblo, \
                                 ibhi,jbhi, \
                                 ibboxlo,jbboxlo, \
                                 ibboxhi,jbboxhi) \
   CCTK_LOOP2STR_INTBOUNDARIES(name, \
                               i,j, \
                               ni,nj, \
                               iblo,jblo, \
                               ibhi,jbhi, \
                               ibboxlo,jbboxlo, \
                               ibboxhi,jbboxhi, \
                               name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP2_INTBOUNDARIES(name) \
   CCTK_ENDLOOP2STR_INTBOUNDARIES(name) \

#define CCTK_LOOP2STR_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP2STR_NORMAL_DECLARE(name/**/_intboundaries) \
   && integer :: name/**/2_blo(2), name/**/2_bhi(2) \
   && integer :: name/**/2_bboxlo(2), name/**/2_bboxhi(2) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && integer :: name/**/2_jdir \
   && logical :: name/**/2_any_bbox,  name/**/2_all_bbox \
   && integer :: name/**/2_bmin(2), name/**/2_bmax(2) \

#define CCTK_LOOP2STR_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_NORMAL_OMP_PRIVATE(name/**/_intboundaries) \

#define CCTK_LOOP2STR_INTBOUNDARIES(name, \
                                    i,j, \
                                    ni,nj, \
                                    iblo,jblo, \
                                    ibhi,jbhi, \
                                    ibboxlo,jbboxlo, \
                                    ibboxhi,jbboxhi, \
                                    imin,imax, istr) \
   && name/**/2_blo = (/ iblo,jblo /) \
   && name/**/2_bhi = (/ ibhi,jbhi /) \
   && name/**/2_bboxlo = (/ ibboxlo,jbboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi,jbboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_jdir=-1, +1 \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(2) /= 0 \
   &&     name/**/2_all_bbox = .true. \
   &&     if (name/**/2_idir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(2) /= 0 \
   &&    if (name/**/2_all_bbox .and. name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_jdir==-1) name/**/2_bmin(2) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_jdir== 0) name/**/2_bmin(2) = name/**/2_blo(2)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmin(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_jdir==-1) name/**/2_bmax(2) = name/**/2_blo(2) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir== 0) name/**/2_bmax(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmax(2) = cctk_lsh(2) \
            CCTK_LOOP2STR_NORMAL(name/**/_intboundaries, \
                                 i,j, \
                                 ni,nj, \
                                 name/**/2_idir,name/**/2_jdir, \
                                 name/**/2_bmin(1),name/**/2_bmin(2), \
                                 name/**/2_bmax(1),name/**/2_bmax(2), \
                                 cctk_ash(1), \
                                 cctk_ash(2), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP2STR_INTBOUNDARIES(name) \
            CCTK_ENDLOOP2STR_NORMAL(name/**/_intboundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \
   && end do /* dir */ \



/* LOOP_ALL */

#define CCTK_LOOP2_ALL_DECLARE(name) \
   CCTK_LOOP2STR_ALL_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP2_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_ALL_OMP_PRIVATE(name) \

#define CCTK_LOOP2_ALL(name, \
                       i,j) \
   CCTK_LOOP2STR_ALL(name, \
                     i,j, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP2_ALL(name) \
   CCTK_ENDLOOP2STR_ALL(name) \

#define CCTK_LOOP2STR_ALL_DECLARE(name) \
   CCTK_LOOP2STR_DECLARE(name/**/_all) \

#define CCTK_LOOP2STR_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_OMP_PRIVATE(name/**/_all) \

#define CCTK_LOOP2STR_ALL(name, \
                          i,j, \
                          imin,imax, istr) \
   CCTK_LOOP2STR(name/**/_all, \
                 i,j, \
                 1,1, \
                 cctk_lsh(1),cctk_lsh(2), \
                 cctk_ash(1),cctk_ash(2), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP2STR_ALL(name) \
   CCTK_ENDLOOP2STR(name/**/_all) \



/* LOOP_INT */

#define CCTK_LOOP2_INT_DECLARE(name) \
   CCTK_LOOP2STR_INT_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP2_INT_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_INT_OMP_PRIVATE(name) \

#define CCTK_LOOP2_INT(name, \
                        i,j) \
   CCTK_LOOP2STR_INT(name, \
                      i,j, \
                      name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP2_INT(name) \
   CCTK_ENDLOOP2STR_INT(name) \

#define CCTK_LOOP2STR_INT_DECLARE(name) \
   CCTK_LOOP2STR_INTERIOR_DECLARE(name/**/_int) \
   && integer :: name/**/3_bndsize    (4) \
   && integer :: name/**/3_is_ghostbnd(4) \
   && integer :: name/**/3_is_symbnd  (4) \
   && integer :: name/**/3_is_physbnd (4) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP2STR_INT_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_INTERIOR_OMP_PRIVATE(name/**/_int) \

#define CCTK_LOOP2STR_INT(name, \
                          i,j, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 4, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize) \
      CCTK_LOOP2STR_INTERIOR(name/**/_int, \
                             i,j, \
                             name/**/3_bndsize(1+1),name/**/3_bndsize(3+1), \
                             name/**/3_bndsize(2),name/**/3_bndsize(4), \
                             imin,imax, (istr)) \

#define CCTK_ENDLOOP2STR_INT(name) \
      CCTK_ENDLOOP2STR_INTERIOR(name/**/int) \



/* LOOP_BND */

#define CCTK_LOOP2_BND_DECLARE(name) \
   CCTK_LOOP2STR_BND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP2_BND_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_BND_OMP_PRIVATE(name) \

#define CCTK_LOOP2_BND(name, \
                       i,j, \
                       ni,nj) \
   CCTK_LOOP2STR_BND(name, \
                     i,j, \
                     ni,nj, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP2_BND(name) \
   CCTK_ENDLOOP2STR_BND(name) \

#define CCTK_LOOP2STR_BND_DECLARE(name) \
   CCTK_LOOP2STR_BOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (4) \
   && integer :: name/**/3_is_ghostbnd(4) \
   && integer :: name/**/3_is_symbnd  (4) \
   && integer :: name/**/3_is_physbnd (4) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP2STR_BND_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_BOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP2STR_BND(name, \
                          i,j, \
                          ni,nj, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 4, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP2STR_BOUNDARIES(name/**/_bnd, \
                               i,j, \
                               ni,nj, \
                               name/**/3_bndsize(1)+1,name/**/3_bndsize(3)+1, \
                               name/**/3_bndsize(2),name/**/3_bndsize(4), \
                               name/**/3_is_physbnd(1),name/**/3_is_physbnd(3), \
                               name/**/3_is_physbnd(2),name/**/3_is_physbnd(4), \
                               imin,imax, (istr)) \

#define CCTK_ENDLOOP2STR_BND(name) \
      CCTK_ENDLOOP2STR_BOUNDARIES(name/**/_bnd) \






/* LOOP_INTBND */

#define CCTK_LOOP2_INTBND_DECLARE(name) \
   CCTK_LOOP2STR_INTBND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP2_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_INTBND_OMP_PRIVATE(name) \

#define CCTK_LOOP2_INTBND(name, \
                          i,j, \
                          ni,nj) \
   CCTK_LOOP2STR_INTBND(name, \
                        i,j, \
                        ni,nj, \
                        name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP2_INTBND(name) \
   CCTK_ENDLOOP2STR_INTBND(name) \

#define CCTK_LOOP2STR_INTBND_DECLARE(name) \
   CCTK_LOOP2STR_INTBOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (4) \
   && integer :: name/**/3_is_ghostbnd(4) \
   && integer :: name/**/3_is_symbnd  (4) \
   && integer :: name/**/3_is_physbnd (4) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP2STR_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP2STR_INTBOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP2STR_INTBND(name, \
                             i,j, \
                             ni,nj, \
                             imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 4, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP2STR_INTBOUNDARIES(name/**/_bnd, \
                                  i,j, \
                                  ni,nj, \
                                  name/**/3_bndsize(1+1),name/**/3_bndsize(3+1), \
                                  name/**/3_bndsize(2),name/**/3_bndsize(4), \
                                  name/**/3_is_physbnd(1),name/**/3_is_physbnd(3), \
                                  name/**/3_is_physbnd(2),name/**/3_is_physbnd(4), \
                                  imin,imax, (istr)) \

#define CCTK_ENDLOOP2STR_INTBND(name) \
      CCTK_ENDLOOP2STR_INTBOUNDARIES(name/**/_bnd) \

#endif /* #ifdef FCODE */



/* 3D */

#ifdef CCODE

/* LOOP */

#define CCTK_LOOP3_NORMAL(name, \
                          i,j,k, \
                          ni,nj,nk, \
                          idir,jdir,kdir, \
                          imin,jmin,kmin, \
                          imax,jmax,kmax, \
                          iash,jash,kash) \
  CCTK_LOOP3STR_NORMAL(name, \
                       i,j,k, \
                       ni,nj,nk, \
                       (idir),(jdir),(kdir), \
                       (imin),(jmin),(kmin), \
                       (imax),(jmax),(kmax), \
                       (iash),(jash),(kash), \
                       cctki0_imin_,cctki0_imax_, 1) \

#define CCTK_ENDLOOP3_NORMAL(name) \
  CCTK_ENDLOOP3STR_NORMAL(name) \

#define CCTK_LOOP3STR_NORMAL(name, \
                             i,j,k, \
                             ni,nj,nk, \
                             idir,jdir,kdir, \
                             imin_,jmin_,kmin_, \
                             imax_,jmax_,kmax_, \
                             iash,jash,kash, \
                             imin,imax, istr) \
  do { \
    typedef int cctki0_loop3_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_jdir = (jdir); \
    int const cctki0_kdir = (kdir); \
    int const cctki0_imin = (imin_); \
    int const cctki0_jmin = (jmin_); \
    int const cctki0_kmin = (kmin_); \
    int const cctki0_imax = (imax_); \
    int const cctki0_jmax = (jmax_); \
    int const cctki0_kmax = (kmax_); \
    int const cctki0_iash = (iash); \
    int const cctki0_jash = (jash); \
    int const cctki0_kash = (kash); \
    int const cctki0_istr = (istr); \
    int const imin = cctki0_imin; \
    int const imax = cctki0_imax; \
    _Pragma("omp for collapse(2)") \
    for (int k=cctki0_kmin; k<cctki0_kmax; ++k) { \
    for (int j=cctki0_jmin; j<cctki0_jmax; ++j) { \
    for (int i=cctki0_imin - (imin+cctki0_iash*(j+cctki0_jash*(k))) % cctki0_istr; i<cctki0_imax; i+=cctki0_istr) { \
      int const ni CCTK_ATTRIBUTE_UNUSED = cctki0_idir<0 ? i+1 : cctki0_idir==0 ? 0 : cctki0_imax-i; \
      int const nj CCTK_ATTRIBUTE_UNUSED = cctki0_jdir<0 ? j+1 : cctki0_jdir==0 ? 0 : cctki0_jmax-j; \
      int const nk CCTK_ATTRIBUTE_UNUSED = cctki0_kdir<0 ? k+1 : cctki0_kdir==0 ? 0 : cctki0_kmax-k; \
      { \

#define CCTK_ENDLOOP3STR_NORMAL(name) \
      } \
    } \
    } \
    } \
    typedef cctki0_loop3_normal_##name cctki0_ensure_proper_nesting; \
  } while (0) \



#define CCTK_LOOP3(name, \
                   i,j,k, \
                   imin,jmin,kmin, \
                   imax,jmax,kmax, \
                   iash,jash,kash) \
  CCTK_LOOP3STR(name, \
                i,j,k, \
                (imin),(jmin),(kmin), \
                (imax),(jmax),(kmax), \
                (iash),(jash),(kash), \
                cctki1_imin_,cctki1_imax_, 1) \

#define CCTK_ENDLOOP3(name) \
  CCTK_ENDLOOP3STR(name) \

#define CCTK_LOOP3STR(name, \
                      i,j,k, \
                      imin_,jmin_,kmin_, \
                      imax_,jmax_,kmax_, \
                      iash,jash,kash, \
                      imin,imax, istr) \
  CCTK_LOOP3STR_NORMAL(name, \
                       i,j,k, \
                       cctki1_ni,cctki1_nj,cctki1_nk, \
                       0,0,0, \
                       imin_,jmin_,kmin_, \
                       imax_,jmax_,kmax_, \
                       iash,jash,kash, \
                       imin,imax, istr) \

#define CCTK_ENDLOOP3STR(name) \
  CCTK_ENDLOOP3STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP3_INTERIOR(name, cctkGH, \
                            i,j,k, \
                            iblo,jblo,kblo, \
                            ibhi,jbhi,kbhi) \
  CCTK_LOOP3STR_INTERIOR(name, (cctkGH), \
                         i,j,k, \
                         (iblo),(jblo),(kblo), \
                         (ibhi),(jbhi),(kbhi), \
                         cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP3_INTERIOR(name) \
  CCTK_ENDLOOP3STR_INTERIOR(name) \

#define CCTK_LOOP3STR_INTERIOR(name, cctkGH, \
                               i,j,k, \
                               iblo,jblo,kblo, \
                               ibhi,jbhi,kbhi, \
                               imin,imax, istr) \
  do { \
    typedef int cctki2_loop3_interior_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_INTERIOR can only be used in 3 dimensions"); \
    } \
    CCTK_LOOP3STR(name##_interior, \
                  i,j,k, \
                  (iblo),(jblo),(kblo), \
                  cctki2_cctkGH->cctk_lsh[0]-(ibhi), \
                  cctki2_cctkGH->cctk_lsh[1]-(jbhi), \
                  cctki2_cctkGH->cctk_lsh[2]-(kbhi), \
                  cctki2_cctkGH->cctk_ash[0], \
                  cctki2_cctkGH->cctk_ash[1], \
                  cctki2_cctkGH->cctk_ash[2], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP3STR_INTERIOR(name) \
    } CCTK_ENDLOOP3STR(name##_interior); \
    typedef cctki2_loop3_interior_##name cctki2_ensure_proper_nesting; \
  } while(0) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP3_BOUNDARIES(name, cctkGH, \
                              i,j,k, \
                              ni,nj,nk, \
                              iblo,jblo,kblo, \
                              ibhi,jbhi,kbhi, \
                              ibboxlo,jbboxlo,kbboxlo, \
                              ibboxhi,jbboxhi,kbboxhi) \
  CCTK_LOOP3STR_BOUNDARIES(name, (cctkGH), \
                           i,j,k, \
                           ni,nj,nk, \
                           (iblo),(jblo),(kblo), \
                           (ibhi),(jbhi),(kbhi), \
                           (ibboxlo),(jbboxlo),(kbboxlo), \
                           (ibboxhi),(jbboxhi),(kbboxhi), \
                           cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP3_BOUNDARIES(name) \
  CCTK_ENDLOOP3STR_BOUNDARIES(name) \

#define CCTK_LOOP3STR_BOUNDARIES(name, cctkGH, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 iblo,jblo,kblo, \
                                 ibhi,jbhi,kbhi, \
                                 ibboxlo,jbboxlo,kbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi, \
                                 imin,imax, istr) \
  do { \
    typedef int cctki2_loop3_boundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_BOUNDARIES can only be used in 3 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_kdir=-1; cctki2_kdir<=+1; ++cctki2_kdir) { \
    for (int cctki2_jdir=-1; cctki2_jdir<=+1; ++cctki2_jdir) { \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0)|| \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 0) || (cctki2_jdir==+1 ? cctki2_bbox[3] : 0)|| \
        (cctki2_kdir==-1 ? cctki2_bbox[4] : 0) || (cctki2_kdir==+1 ? cctki2_bbox[5] : 0); \
      if (cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
          cctki2_jdir==-1 ? 0 : cctki2_jdir==0 ? cctki2_blo[1] : cctki2_lsh[1] - cctki2_bhi[1], \
          cctki2_kdir==-1 ? 0 : cctki2_kdir==0 ? cctki2_blo[2] : cctki2_lsh[2] - cctki2_bhi[2], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
          cctki2_jdir==-1 ? cctki2_blo[1] : cctki2_jdir==0 ? cctki2_lsh[1] - cctki2_bhi[1] : cctki2_lsh[1], \
          cctki2_kdir==-1 ? cctki2_blo[2] : cctki2_kdir==0 ? cctki2_lsh[2] - cctki2_bhi[2] : cctki2_lsh[2], \
        }; \
        CCTK_LOOP3STR_NORMAL(name##_boundaries, \
                             i,j,k, \
                             ni,nj,nk, \
                             cctki2_idir,cctki2_jdir,cctki2_kdir, \
                             cctki2_bmin[0],cctki2_bmin[1],cctki2_bmin[2], \
                             cctki2_bmax[0],cctki2_bmax[1],cctki2_bmax[2], \
                             cctki2_cctkGH->cctk_ash[0], \
                             cctki2_cctkGH->cctk_ash[1], \
                             cctki2_cctkGH->cctk_ash[2], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP3STR_BOUNDARIES(name) \
        } CCTK_ENDLOOP3STR_NORMAL(name##_boundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    } /* for dir */ \
    } /* for dir */ \
    typedef cctki2_loop3_boundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP3_INTBOUNDARIES(name, cctkGH, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 iblo,jblo,kblo, \
                                 ibhi,jbhi,kbhi, \
                                 ibboxlo,jbboxlo,kbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi) \
  CCTK_LOOP3STR_INTBOUNDARIES(name, (cctkGH), \
                              i,j,k, \
                              ni,nj,nk, \
                              (iblo),(jblo),(kblo), \
                              (ibhi),(jbhi),(kbhi), \
                              (ibboxlo),(jbboxlo),(kbboxlo), \
                              (ibboxhi),(jbboxhi),(kbboxhi), \
                              cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP3_INTBOUNDARIES(name) \
  CCTK_ENDLOOP3STR_INTBOUNDARIES(name) \

#define CCTK_LOOP3STR_INTBOUNDARIES(name, cctkGH, \
                                    i,j,k, \
                                    ni,nj,nk, \
                                    iblo,jblo,kblo, \
                                    ibhi,jbhi,kbhi, \
                                    ibboxlo,jbboxlo,kbboxlo, \
                                    ibboxhi,jbboxhi,kbboxhi, \
                                    imin,imax, istr) \
  do { \
    typedef int cctki2_loop3_intboundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_INTBOUNDARIES can only be used in 3 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_kdir=-1; cctki2_kdir<=+1; ++cctki2_kdir) { \
    for (int cctki2_jdir=-1; cctki2_jdir<=+1; ++cctki2_jdir) { \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0)|| \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 0) || (cctki2_jdir==+1 ? cctki2_bbox[3] : 0)|| \
        (cctki2_kdir==-1 ? cctki2_bbox[4] : 0) || (cctki2_kdir==+1 ? cctki2_bbox[5] : 0); \
      int cctki2_all_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 1) && (cctki2_idir==+1 ? cctki2_bbox[1] : 1)&& \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 1) && (cctki2_jdir==+1 ? cctki2_bbox[3] : 1)&& \
        (cctki2_kdir==-1 ? cctki2_bbox[4] : 1) && (cctki2_kdir==+1 ? cctki2_bbox[5] : 1); \
      if (cctki2_all_bbox && cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
          cctki2_jdir==-1 ? 0 : cctki2_jdir==0 ? cctki2_blo[1] : cctki2_lsh[1] - cctki2_bhi[1], \
          cctki2_kdir==-1 ? 0 : cctki2_kdir==0 ? cctki2_blo[2] : cctki2_lsh[2] - cctki2_bhi[2], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
          cctki2_jdir==-1 ? cctki2_blo[1] : cctki2_jdir==0 ? cctki2_lsh[1] - cctki2_bhi[1] : cctki2_lsh[1], \
          cctki2_kdir==-1 ? cctki2_blo[2] : cctki2_kdir==0 ? cctki2_lsh[2] - cctki2_bhi[2] : cctki2_lsh[2], \
        }; \
        CCTK_LOOP3STR_NORMAL(name##_intboundaries, \
                             i,j,k, \
                             ni,nj,nk, \
                             cctki2_idir,cctki2_jdir,cctki2_kdir, \
                             cctki2_bmin[0],cctki2_bmin[1],cctki2_bmin[2], \
                             cctki2_bmax[0],cctki2_bmax[1],cctki2_bmax[2], \
                             cctki2_cctkGH->cctk_ash[0], \
                             cctki2_cctkGH->cctk_ash[1], \
                             cctki2_cctkGH->cctk_ash[2], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP3STR_INTBOUNDARIES(name) \
        } CCTK_ENDLOOP3STR_NORMAL(name##_intboundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    } /* for dir */ \
    } /* for dir */ \
    typedef cctki2_loop3_intboundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_ALL */

#define CCTK_LOOP3_ALL(name, cctkGH, \
                       i,j,k) \
  CCTK_LOOP3STR_ALL(name, (cctkGH), \
                    i,j,k, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP3_ALL(name) \
  CCTK_ENDLOOP3STR_ALL(name) \

#define CCTK_LOOP3STR_ALL(name, cctkGH, \
                          i,j,k, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop3_all_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_ALL can only be used in 3 dimensions"); \
    } \
    CCTK_LOOP3STR(name##_all, \
                  i,j,k, \
                  0,0,0, \
                  cctki3_cctkGH->cctk_lsh[0], \
                  cctki3_cctkGH->cctk_lsh[1], \
                  cctki3_cctkGH->cctk_lsh[2], \
                  cctki3_cctkGH->cctk_ash[0], \
                  cctki3_cctkGH->cctk_ash[1], \
                  cctki3_cctkGH->cctk_ash[2], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP3STR_ALL(name) \
    } CCTK_ENDLOOP3STR(name##_all); \
    typedef cctki3_loop3_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP3_INT(name, cctkGH, \
                       i,j,k) \
  CCTK_LOOP3STR_INT(name, (cctkGH), \
                    i,j,k, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP3_INT(name) \
  CCTK_ENDLOOP3STR_INT(name) \

#define CCTK_LOOP3STR_INT(name, cctkGH, \
                          i,j,k, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop3_int_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_INT can only be used in 3 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [6]; \
    CCTK_INT cctki3_is_ghostbnd[6]; \
    CCTK_INT cctki3_is_symbnd  [6]; \
    CCTK_INT cctki3_is_physbnd [6]; \
    _Pragma("omp single copyprivate(cctki3_bndsize)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 6, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP3STR_INTERIOR(name##_int, \
                           cctki3_cctkGH, \
                           i,j,k, \
                           cctki3_bndsize[0],cctki3_bndsize[2],cctki3_bndsize[4], \
                           cctki3_bndsize[1],cctki3_bndsize[3],cctki3_bndsize[5], \
                           imin,imax, (istr)) { \

#define CCTK_ENDLOOP3STR_INT(name) \
    } CCTK_ENDLOOP3STR_INTERIOR(name##_int); \
    typedef cctki3_loop3_int_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_BND */

#define CCTK_LOOP3_BND(name, cctkGH, \
                       i,j,k, \
                       ni,nj,nk) \
  CCTK_LOOP3STR_BND(name, (cctkGH), \
                    i,j,k, \
                    ni,nj,nk, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP3_BND(name) \
  CCTK_ENDLOOP3STR_BND(name) \

#define CCTK_LOOP3STR_BND(name, cctkGH, \
                          i,j,k, \
                          ni,nj,nk, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop3_bnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_BND can only be used in 3 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [6]; \
    CCTK_INT cctki3_is_ghostbnd[6]; \
    CCTK_INT cctki3_is_symbnd  [6]; \
    CCTK_INT cctki3_is_physbnd [6]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 6, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP3STR_BOUNDARIES(name##_bnd, \
                             cctki3_cctkGH, \
                             i,j,k, \
                             ni,nj,nk, \
                             cctki3_bndsize[0],cctki3_bndsize[2],cctki3_bndsize[4], \
                             cctki3_bndsize[1],cctki3_bndsize[3],cctki3_bndsize[5], \
                             cctki3_is_physbnd[0],cctki3_is_physbnd[2],cctki3_is_physbnd[4], \
                             cctki3_is_physbnd[1],cctki3_is_physbnd[3],cctki3_is_physbnd[5], \
                             imin,imax, (istr)) { \

#define CCTK_ENDLOOP3STR_BND(name) \
    } CCTK_ENDLOOP3STR_BOUNDARIES(name##_bnd); \
    typedef cctki3_loop3_bnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBND */

#define CCTK_LOOP3_INTBND(name, cctkGH, \
                           i,j,k, \
                           ni,nj,nk) \
  CCTK_LOOP3STR_INTBND(name, (cctkGH), \
                        i,j,k, \
                        ni,nj,nk, \
                        cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP3_INTBND(name) \
  CCTK_ENDLOOP3STR_INTBND(name) \

#define CCTK_LOOP3STR_INTBND(name, cctkGH, \
                              i,j,k, \
                              ni,nj,nk, \
                              imin,imax, istr) \
  do { \
    typedef int cctki3_loop3_intbnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_INTBND can only be used in 3 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [6]; \
    CCTK_INT cctki3_is_ghostbnd[6]; \
    CCTK_INT cctki3_is_symbnd  [6]; \
    CCTK_INT cctki3_is_physbnd [6]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 6, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP3STR_INTBOUNDARIES(name##_intbnd, \
                                cctki3_cctkGH, \
                                i,j,k, \
                                ni,nj,nk, \
                                cctki3_bndsize[0],cctki3_bndsize[2],cctki3_bndsize[4], \
                                cctki3_bndsize[1],cctki3_bndsize[3],cctki3_bndsize[5], \
                                cctki3_is_physbnd[0],cctki3_is_physbnd[2],cctki3_is_physbnd[4], \
                                cctki3_is_physbnd[1],cctki3_is_physbnd[3],cctki3_is_physbnd[5], \
                                imin,imax, (istr)) { \

#define CCTK_ENDLOOP3STR_INTBND(name) \
    } CCTK_ENDLOOP3STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop3_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP3_NORMAL_DECLARE(name) \
   CCTK_LOOP3STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_imin_, name/**/0_imax_ \

#define CCTK_LOOP3_NORMAL_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP3_NORMAL(name, \
                          i,j,k, \
                          ni,nj,nk, \
                          idir,jdir,kdir, \
                          imin,jmin,kmin, \
                          imax,jmax,kmax, \
                          iash,jash,kash) \
   CCTK_LOOP3STR_NORMAL(name, \
                        i,j,k, \
                        ni,nj,nk, \
                        idir,jdir,kdir, \
                        imin,jmin,kmin, \
                        imax,jmax,kmax, \
                        iash,jash,kash, \
                        name/**/0_imin_,name/**/0_imax_, 1) \

#define CCTK_ENDLOOP3_NORMAL(name) \
   CCTK_ENDLOOP3STR_NORMAL(name) \

#define CCTK_LOOP3STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_idir,name/**/0_jdir,name/**/0_kdir \
   && integer :: name/**/0_imin,name/**/0_jmin,name/**/0_kmin \
   && integer :: name/**/0_imax,name/**/0_jmax,name/**/0_kmax \
   && integer :: name/**/0_iash,name/**/0_jash,name/**/0_kash \
   && integer :: name/**/0_istr \

#define CCTK_LOOP3STR_NORMAL_OMP_PRIVATE(name) \
   && !$omp private (i,j,k) \
   && !$omp private (ni,nj,nk) \

#define CCTK_LOOP3STR_NORMAL(name, \
                             i,j,k, \
                             ni,nj,nk, \
                             idir,jdir,kdir, \
                             imin_,jmin_,kmin_, \
                             imax_,jmax_,kmax_, \
                             iash,jash,kash, \
                             imin,imax, istr) \
   && name/**/0_idir = idir \
   && name/**/0_jdir = jdir \
   && name/**/0_kdir = kdir \
   && name/**/0_imin = imin_ \
   && name/**/0_jmin = jmin_ \
   && name/**/0_kmin = kmin_ \
   && name/**/0_imax = imax_ \
   && name/**/0_jmax = jmax_ \
   && name/**/0_kmax = kmax_ \
   && name/**/0_iash = iash \
   && name/**/0_jash = jash \
   && name/**/0_kash = kash \
   && name/**/0_istr = istr \
   && imin = name/**/0_imin \
   && imax = name/**/0_imax \
   && !$omp do collapse(2) \
   && do k = name/**/0_kmin, name/**/0_kmax \
   && do j = name/**/0_jmin, name/**/0_jmax \
   && do i = name/**/0_imin - modulo((imin+name/**/0_iash*(j+name/**/0_jash*(k))), name/**/0_istr), name/**/0_imax \
   &&    if (name/**/0_idir< 0) ni = i \
   &&    if (name/**/0_jdir< 0) nj = j \
   &&    if (name/**/0_kdir< 0) nk = k \
   &&    if (name/**/0_idir==0) ni = 0 \
   &&    if (name/**/0_jdir==0) nj = 0 \
   &&    if (name/**/0_kdir==0) nk = 0 \
   &&    if (name/**/0_idir> 0) ni = name/**/0_imax+1-i \
   &&    if (name/**/0_jdir> 0) nj = name/**/0_jmax+1-j \
   &&    if (name/**/0_kdir> 0) nk = name/**/0_kmax+1-k \

#define CCTK_ENDLOOP3STR_NORMAL(name) \
   && end do \
   && end do \
   && end do \



#define CCTK_LOOP3_DECLARE(name) \
   CCTK_LOOP3STR_DECLARE(name) \
   && integer :: name/**/1_imin_, name/**/1_imax_ \

#define CCTK_LOOP3_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_OMP_PRIVATE(name) \

#define CCTK_LOOP3(name, \
                   i,j,k, \
                   imin,jmin,kmin, \
                   imax,jmax,kmax, \
                   iash,jash,kash) \
   CCTK_LOOP3STR(name, \
                 i,j,k, \
                 imin,jmin,kmin, \
                 imax,jmax,kmax, \
                 iash,jash,kash, \
                 name/**/1_imin_,name/**/1_imax_, 1) \

#define CCTK_ENDLOOP3(name) \
   CCTK_ENDLOOP3STR(name) \

#define CCTK_LOOP3STR_DECLARE(name) \
   CCTK_LOOP3STR_NORMAL_DECLARE(name) \
   && integer :: name/**/1_ni,name/**/1_nj,name/**/1_nk \

#define CCTK_LOOP3STR_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP3STR(name, \
                      i,j,k, \
                      imin_,jmin_,kmin_, \
                      imax_,jmax_,kmax_, \
                      iash,jash,kash, \
                      imin,imax, istr) \
   CCTK_LOOP3STR_NORMAL(name, \
                        i,j,k, \
                        name/**/1_ni,name/**/1_nj,name/**/1_nk, \
                        0,0,0, \
                        imin_,jmin_,kmin_, \
                        imax_,jmax_,kmax_, \
                        iash,jash,kash, \
                        imin,imax, istr) \

#define CCTK_ENDLOOP3STR(name) \
   CCTK_ENDLOOP3STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP3_INTERIOR_DECLARE(name) \
   CCTK_LOOP3STR_INTERIOR_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP3_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_INTERIOR_OMP_PRIVATE(name) \

#define CCTK_LOOP3_INTERIOR(name, \
                            i,j,k, \
                            iblo,jblo,kblo, \
                            ibhi,jbhi,kbhi) \
   CCTK_LOOP3STR_INTERIOR(name, \
                          i,j,k, \
                          iblo,jblo,kblo, \
                          ibhi,jbhi,kbhi, \
                          name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP3_INTERIOR(name) \
   CCTK_ENDLOOP3STR_INTERIOR(name) \

#define CCTK_LOOP3STR_INTERIOR_DECLARE(name) \
   CCTK_LOOP3STR_DECLARE(name/**/_interior) \

#define CCTK_LOOP3STR_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_OMP_PRIVATE(name/**/_interior) \

#define CCTK_LOOP3STR_INTERIOR(name, \
                               i,j,k, \
                               iblo,jblo,kblo, \
                               ibhi,jbhi,kbhi, \
                               imin,imax, istr) \
   CCTK_LOOP3STR(name/**/_interior, \
                 i,j,k, \
                 (iblo)+1, \
                 (jblo)+1, \
                 (kblo)+1, \
                 cctk_lsh(1)-(ibhi), \
                 cctk_lsh(2)-(jbhi), \
                 cctk_lsh(3)-(kbhi), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP3STR_INTERIOR(name) \
   CCTK_ENDLOOP3STR(name/**/_interior) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP3_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP3STR_BOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP3_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_BOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP3_BOUNDARIES(name, \
                              i,j,k, \
                              ni,nj,nk, \
                              iblo,jblo,kblo, \
                              ibhi,jbhi,kbhi, \
                              ibboxlo,jbboxlo,kbboxlo, \
                              ibboxhi,jbboxhi,kbboxhi) \
   CCTK_LOOP3STR_BOUNDARIES(name, \
                            i,j,k, \
                            ni,nj,nk, \
                            iblo,jblo,kblo, \
                            ibhi,jbhi,kbhi, \
                            ibboxlo,jbboxlo,kbboxlo, \
                            ibboxhi,jbboxhi,kbboxhi, \
                            name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP3_BOUNDARIES(name) \
   CCTK_ENDLOOP3STR_BOUNDARIES(name) \

#define CCTK_LOOP3STR_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP3STR_NORMAL_DECLARE(name/**/_boundaries) \
   && integer :: name/**/2_blo(3), name/**/2_bhi(3) \
   && integer :: name/**/2_bboxlo(3), name/**/2_bboxhi(3) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && integer :: name/**/2_jdir \
   && integer :: name/**/2_kdir \
   && logical :: name/**/2_any_bbox \
   && integer :: name/**/2_bmin(3), name/**/2_bmax(3) \

#define CCTK_LOOP3STR_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_NORMAL_OMP_PRIVATE(name/**/_boundaries) \

#define CCTK_LOOP3STR_BOUNDARIES(name, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 iblo,jblo,kblo, \
                                 ibhi,jbhi,kbhi, \
                                 ibboxlo,jbboxlo,kbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi, \
                                 imin,imax, istr) \
   && name/**/2_blo = (/ iblo,jblo,kblo /) \
   && name/**/2_bhi = (/ ibhi,jbhi,kbhi /) \
   && name/**/2_bboxlo = (/ ibboxlo,jbboxlo,kbboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi,jbboxhi,kbboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_kdir=-1, +1 \
   && do name/**/2_jdir=-1, +1 \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_kdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(3) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(2) /= 0 \
   &&     if (name/**/2_kdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(3) /= 0 \
   &&    if (name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_jdir==-1) name/**/2_bmin(2) = 1 \
   &&       if (name/**/2_kdir==-1) name/**/2_bmin(3) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_jdir== 0) name/**/2_bmin(2) = name/**/2_blo(2)+1 \
   &&       if (name/**/2_kdir== 0) name/**/2_bmin(3) = name/**/2_blo(3)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmin(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmin(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_jdir==-1) name/**/2_bmax(2) = name/**/2_blo(2) \
   &&       if (name/**/2_kdir==-1) name/**/2_bmax(3) = name/**/2_blo(3) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir== 0) name/**/2_bmax(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir== 0) name/**/2_bmax(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmax(2) = cctk_lsh(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmax(3) = cctk_lsh(3) \
            CCTK_LOOP3STR_NORMAL(name/**/_boundaries, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 name/**/2_idir,name/**/2_jdir,name/**/2_kdir, \
                                 name/**/2_bmin(1),name/**/2_bmin(2),name/**/2_bmin(3), \
                                 name/**/2_bmax(1),name/**/2_bmax(2),name/**/2_bmax(3), \
                                 cctk_ash(1), \
                                 cctk_ash(2), \
                                 cctk_ash(3), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP3STR_BOUNDARIES(name) \
            CCTK_ENDLOOP3STR_NORMAL(name/**/_boundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \
   && end do /* dir */ \
   && end do /* dir */ \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP3_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP3STR_INTBOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP3_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_INTBOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP3_INTBOUNDARIES(name, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 iblo,jblo,kblo, \
                                 ibhi,jbhi,kbhi, \
                                 ibboxlo,jbboxlo,kbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi) \
   CCTK_LOOP3STR_INTBOUNDARIES(name, \
                               i,j,k, \
                               ni,nj,nk, \
                               iblo,jblo,kblo, \
                               ibhi,jbhi,kbhi, \
                               ibboxlo,jbboxlo,kbboxlo, \
                               ibboxhi,jbboxhi,kbboxhi, \
                               name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP3_INTBOUNDARIES(name) \
   CCTK_ENDLOOP3STR_INTBOUNDARIES(name) \

#define CCTK_LOOP3STR_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP3STR_NORMAL_DECLARE(name/**/_intboundaries) \
   && integer :: name/**/2_blo(3), name/**/2_bhi(3) \
   && integer :: name/**/2_bboxlo(3), name/**/2_bboxhi(3) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && integer :: name/**/2_jdir \
   && integer :: name/**/2_kdir \
   && logical :: name/**/2_any_bbox,  name/**/2_all_bbox \
   && integer :: name/**/2_bmin(3), name/**/2_bmax(3) \

#define CCTK_LOOP3STR_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_NORMAL_OMP_PRIVATE(name/**/_intboundaries) \

#define CCTK_LOOP3STR_INTBOUNDARIES(name, \
                                    i,j,k, \
                                    ni,nj,nk, \
                                    iblo,jblo,kblo, \
                                    ibhi,jbhi,kbhi, \
                                    ibboxlo,jbboxlo,kbboxlo, \
                                    ibboxhi,jbboxhi,kbboxhi, \
                                    imin,imax, istr) \
   && name/**/2_blo = (/ iblo,jblo,kblo /) \
   && name/**/2_bhi = (/ ibhi,jbhi,kbhi /) \
   && name/**/2_bboxlo = (/ ibboxlo,jbboxlo,kbboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi,jbboxhi,kbboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_kdir=-1, +1 \
   && do name/**/2_jdir=-1, +1 \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_kdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(3) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(2) /= 0 \
   &&     if (name/**/2_kdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(3) /= 0 \
   &&     name/**/2_all_bbox = .true. \
   &&     if (name/**/2_idir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_kdir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(3) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(2) /= 0 \
   &&     if (name/**/2_kdir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(3) /= 0 \
   &&    if (name/**/2_all_bbox .and. name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_jdir==-1) name/**/2_bmin(2) = 1 \
   &&       if (name/**/2_kdir==-1) name/**/2_bmin(3) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_jdir== 0) name/**/2_bmin(2) = name/**/2_blo(2)+1 \
   &&       if (name/**/2_kdir== 0) name/**/2_bmin(3) = name/**/2_blo(3)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmin(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmin(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_jdir==-1) name/**/2_bmax(2) = name/**/2_blo(2) \
   &&       if (name/**/2_kdir==-1) name/**/2_bmax(3) = name/**/2_blo(3) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir== 0) name/**/2_bmax(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir== 0) name/**/2_bmax(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmax(2) = cctk_lsh(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmax(3) = cctk_lsh(3) \
            CCTK_LOOP3STR_NORMAL(name/**/_intboundaries, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 name/**/2_idir,name/**/2_jdir,name/**/2_kdir, \
                                 name/**/2_bmin(1),name/**/2_bmin(2),name/**/2_bmin(3), \
                                 name/**/2_bmax(1),name/**/2_bmax(2),name/**/2_bmax(3), \
                                 cctk_ash(1), \
                                 cctk_ash(2), \
                                 cctk_ash(3), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP3STR_INTBOUNDARIES(name) \
            CCTK_ENDLOOP3STR_NORMAL(name/**/_intboundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \
   && end do /* dir */ \
   && end do /* dir */ \



/* LOOP_ALL */

#define CCTK_LOOP3_ALL_DECLARE(name) \
   CCTK_LOOP3STR_ALL_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP3_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_ALL_OMP_PRIVATE(name) \

#define CCTK_LOOP3_ALL(name, \
                       i,j,k) \
   CCTK_LOOP3STR_ALL(name, \
                     i,j,k, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP3_ALL(name) \
   CCTK_ENDLOOP3STR_ALL(name) \

#define CCTK_LOOP3STR_ALL_DECLARE(name) \
   CCTK_LOOP3STR_DECLARE(name/**/_all) \

#define CCTK_LOOP3STR_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_OMP_PRIVATE(name/**/_all) \

#define CCTK_LOOP3STR_ALL(name, \
                          i,j,k, \
                          imin,imax, istr) \
   CCTK_LOOP3STR(name/**/_all, \
                 i,j,k, \
                 1,1,1, \
                 cctk_lsh(1),cctk_lsh(2),cctk_lsh(3), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP3STR_ALL(name) \
   CCTK_ENDLOOP3STR(name/**/_all) \



/* LOOP_INT */

#define CCTK_LOOP3_INT_DECLARE(name) \
   CCTK_LOOP3STR_INT_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP3_INT_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_INT_OMP_PRIVATE(name) \

#define CCTK_LOOP3_INT(name, \
                        i,j,k) \
   CCTK_LOOP3STR_INT(name, \
                      i,j,k, \
                      name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP3_INT(name) \
   CCTK_ENDLOOP3STR_INT(name) \

#define CCTK_LOOP3STR_INT_DECLARE(name) \
   CCTK_LOOP3STR_INTERIOR_DECLARE(name/**/_int) \
   && integer :: name/**/3_bndsize    (6) \
   && integer :: name/**/3_is_ghostbnd(6) \
   && integer :: name/**/3_is_symbnd  (6) \
   && integer :: name/**/3_is_physbnd (6) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP3STR_INT_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_INTERIOR_OMP_PRIVATE(name/**/_int) \

#define CCTK_LOOP3STR_INT(name, \
                          i,j,k, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 6, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize) \
      CCTK_LOOP3STR_INTERIOR(name/**/_int, \
                             i,j,k, \
                             name/**/3_bndsize(1+1),name/**/3_bndsize(3+1),name/**/3_bndsize(5+1), \
                             name/**/3_bndsize(2),name/**/3_bndsize(4),name/**/3_bndsize(6), \
                             imin,imax, (istr)) \

#define CCTK_ENDLOOP3STR_INT(name) \
      CCTK_ENDLOOP3STR_INTERIOR(name/**/int) \



/* LOOP_BND */

#define CCTK_LOOP3_BND_DECLARE(name) \
   CCTK_LOOP3STR_BND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP3_BND_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_BND_OMP_PRIVATE(name) \

#define CCTK_LOOP3_BND(name, \
                       i,j,k, \
                       ni,nj,nk) \
   CCTK_LOOP3STR_BND(name, \
                     i,j,k, \
                     ni,nj,nk, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP3_BND(name) \
   CCTK_ENDLOOP3STR_BND(name) \

#define CCTK_LOOP3STR_BND_DECLARE(name) \
   CCTK_LOOP3STR_BOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (6) \
   && integer :: name/**/3_is_ghostbnd(6) \
   && integer :: name/**/3_is_symbnd  (6) \
   && integer :: name/**/3_is_physbnd (6) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP3STR_BND_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_BOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP3STR_BND(name, \
                          i,j,k, \
                          ni,nj,nk, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 6, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP3STR_BOUNDARIES(name/**/_bnd, \
                               i,j,k, \
                               ni,nj,nk, \
                               name/**/3_bndsize(1)+1,name/**/3_bndsize(3)+1,name/**/3_bndsize(5)+1, \
                               name/**/3_bndsize(2),name/**/3_bndsize(4),name/**/3_bndsize(6), \
                               name/**/3_is_physbnd(1),name/**/3_is_physbnd(3),name/**/3_is_physbnd(5), \
                               name/**/3_is_physbnd(2),name/**/3_is_physbnd(4),name/**/3_is_physbnd(6), \
                               imin,imax, (istr)) \

#define CCTK_ENDLOOP3STR_BND(name) \
      CCTK_ENDLOOP3STR_BOUNDARIES(name/**/_bnd) \






/* LOOP_INTBND */

#define CCTK_LOOP3_INTBND_DECLARE(name) \
   CCTK_LOOP3STR_INTBND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP3_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_INTBND_OMP_PRIVATE(name) \

#define CCTK_LOOP3_INTBND(name, \
                          i,j,k, \
                          ni,nj,nk) \
   CCTK_LOOP3STR_INTBND(name, \
                        i,j,k, \
                        ni,nj,nk, \
                        name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP3_INTBND(name) \
   CCTK_ENDLOOP3STR_INTBND(name) \

#define CCTK_LOOP3STR_INTBND_DECLARE(name) \
   CCTK_LOOP3STR_INTBOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (6) \
   && integer :: name/**/3_is_ghostbnd(6) \
   && integer :: name/**/3_is_symbnd  (6) \
   && integer :: name/**/3_is_physbnd (6) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP3STR_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP3STR_INTBOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP3STR_INTBND(name, \
                             i,j,k, \
                             ni,nj,nk, \
                             imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 6, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP3STR_INTBOUNDARIES(name/**/_bnd, \
                                  i,j,k, \
                                  ni,nj,nk, \
                                  name/**/3_bndsize(1+1),name/**/3_bndsize(3+1),name/**/3_bndsize(5+1), \
                                  name/**/3_bndsize(2),name/**/3_bndsize(4),name/**/3_bndsize(6), \
                                  name/**/3_is_physbnd(1),name/**/3_is_physbnd(3),name/**/3_is_physbnd(5), \
                                  name/**/3_is_physbnd(2),name/**/3_is_physbnd(4),name/**/3_is_physbnd(6), \
                                  imin,imax, (istr)) \

#define CCTK_ENDLOOP3STR_INTBND(name) \
      CCTK_ENDLOOP3STR_INTBOUNDARIES(name/**/_bnd) \

#endif /* #ifdef FCODE */



/* 4D */

#ifdef CCODE

/* LOOP */

#define CCTK_LOOP4_NORMAL(name, \
                          i,j,k,l, \
                          ni,nj,nk,nl, \
                          idir,jdir,kdir,ldir, \
                          imin,jmin,kmin,lmin, \
                          imax,jmax,kmax,lmax, \
                          iash,jash,kash,lash) \
  CCTK_LOOP4STR_NORMAL(name, \
                       i,j,k,l, \
                       ni,nj,nk,nl, \
                       (idir),(jdir),(kdir),(ldir), \
                       (imin),(jmin),(kmin),(lmin), \
                       (imax),(jmax),(kmax),(lmax), \
                       (iash),(jash),(kash),(lash), \
                       cctki0_imin_,cctki0_imax_, 1) \

#define CCTK_ENDLOOP4_NORMAL(name) \
  CCTK_ENDLOOP4STR_NORMAL(name) \

#define CCTK_LOOP4STR_NORMAL(name, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             idir,jdir,kdir,ldir, \
                             imin_,jmin_,kmin_,lmin_, \
                             imax_,jmax_,kmax_,lmax_, \
                             iash,jash,kash,lash, \
                             imin,imax, istr) \
  do { \
    typedef int cctki0_loop4_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_jdir = (jdir); \
    int const cctki0_kdir = (kdir); \
    int const cctki0_ldir = (ldir); \
    int const cctki0_imin = (imin_); \
    int const cctki0_jmin = (jmin_); \
    int const cctki0_kmin = (kmin_); \
    int const cctki0_lmin = (lmin_); \
    int const cctki0_imax = (imax_); \
    int const cctki0_jmax = (jmax_); \
    int const cctki0_kmax = (kmax_); \
    int const cctki0_lmax = (lmax_); \
    int const cctki0_iash = (iash); \
    int const cctki0_jash = (jash); \
    int const cctki0_kash = (kash); \
    int const cctki0_lash = (lash); \
    int const cctki0_istr = (istr); \
    int const imin = cctki0_imin; \
    int const imax = cctki0_imax; \
    _Pragma("omp for collapse(3)") \
    for (int l=cctki0_lmin; l<cctki0_lmax; ++l) { \
    for (int k=cctki0_kmin; k<cctki0_kmax; ++k) { \
    for (int j=cctki0_jmin; j<cctki0_jmax; ++j) { \
    for (int i=cctki0_imin - (imin+cctki0_iash*(j+cctki0_jash*(k+cctki0_kash*(l)))) % cctki0_istr; i<cctki0_imax; i+=cctki0_istr) { \
      int const ni CCTK_ATTRIBUTE_UNUSED = cctki0_idir<0 ? i+1 : cctki0_idir==0 ? 0 : cctki0_imax-i; \
      int const nj CCTK_ATTRIBUTE_UNUSED = cctki0_jdir<0 ? j+1 : cctki0_jdir==0 ? 0 : cctki0_jmax-j; \
      int const nk CCTK_ATTRIBUTE_UNUSED = cctki0_kdir<0 ? k+1 : cctki0_kdir==0 ? 0 : cctki0_kmax-k; \
      int const nl CCTK_ATTRIBUTE_UNUSED = cctki0_ldir<0 ? l+1 : cctki0_ldir==0 ? 0 : cctki0_lmax-l; \
      { \

#define CCTK_ENDLOOP4STR_NORMAL(name) \
      } \
    } \
    } \
    } \
    } \
    typedef cctki0_loop4_normal_##name cctki0_ensure_proper_nesting; \
  } while (0) \



#define CCTK_LOOP4(name, \
                   i,j,k,l, \
                   imin,jmin,kmin,lmin, \
                   imax,jmax,kmax,lmax, \
                   iash,jash,kash,lash) \
  CCTK_LOOP4STR(name, \
                i,j,k,l, \
                (imin),(jmin),(kmin),(lmin), \
                (imax),(jmax),(kmax),(lmax), \
                (iash),(jash),(kash),(lash), \
                cctki1_imin_,cctki1_imax_, 1) \

#define CCTK_ENDLOOP4(name) \
  CCTK_ENDLOOP4STR(name) \

#define CCTK_LOOP4STR(name, \
                      i,j,k,l, \
                      imin_,jmin_,kmin_,lmin_, \
                      imax_,jmax_,kmax_,lmax_, \
                      iash,jash,kash,lash, \
                      imin,imax, istr) \
  CCTK_LOOP4STR_NORMAL(name, \
                       i,j,k,l, \
                       cctki1_ni,cctki1_nj,cctki1_nk,cctki1_nl, \
                       0,0,0,0, \
                       imin_,jmin_,kmin_,lmin_, \
                       imax_,jmax_,kmax_,lmax_, \
                       iash,jash,kash,lash, \
                       imin,imax, istr) \

#define CCTK_ENDLOOP4STR(name) \
  CCTK_ENDLOOP4STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP4_INTERIOR(name, cctkGH, \
                            i,j,k,l, \
                            iblo,jblo,kblo,lblo, \
                            ibhi,jbhi,kbhi,lbhi) \
  CCTK_LOOP4STR_INTERIOR(name, (cctkGH), \
                         i,j,k,l, \
                         (iblo),(jblo),(kblo),(lblo), \
                         (ibhi),(jbhi),(kbhi),(lbhi), \
                         cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP4_INTERIOR(name) \
  CCTK_ENDLOOP4STR_INTERIOR(name) \

#define CCTK_LOOP4STR_INTERIOR(name, cctkGH, \
                               i,j,k,l, \
                               iblo,jblo,kblo,lblo, \
                               ibhi,jbhi,kbhi,lbhi, \
                               imin,imax, istr) \
  do { \
    typedef int cctki2_loop4_interior_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_INTERIOR can only be used in 4 dimensions"); \
    } \
    CCTK_LOOP4STR(name##_interior, \
                  i,j,k,l, \
                  (iblo),(jblo),(kblo),(lblo), \
                  cctki2_cctkGH->cctk_lsh[0]-(ibhi), \
                  cctki2_cctkGH->cctk_lsh[1]-(jbhi), \
                  cctki2_cctkGH->cctk_lsh[2]-(kbhi), \
                  cctki2_cctkGH->cctk_lsh[3]-(lbhi), \
                  cctki2_cctkGH->cctk_ash[0], \
                  cctki2_cctkGH->cctk_ash[1], \
                  cctki2_cctkGH->cctk_ash[2], \
                  cctki2_cctkGH->cctk_ash[3], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP4STR_INTERIOR(name) \
    } CCTK_ENDLOOP4STR(name##_interior); \
    typedef cctki2_loop4_interior_##name cctki2_ensure_proper_nesting; \
  } while(0) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP4_BOUNDARIES(name, cctkGH, \
                              i,j,k,l, \
                              ni,nj,nk,nl, \
                              iblo,jblo,kblo,lblo, \
                              ibhi,jbhi,kbhi,lbhi, \
                              ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                              ibboxhi,jbboxhi,kbboxhi,lbboxhi) \
  CCTK_LOOP4STR_BOUNDARIES(name, (cctkGH), \
                           i,j,k,l, \
                           ni,nj,nk,nl, \
                           (iblo),(jblo),(kblo),(lblo), \
                           (ibhi),(jbhi),(kbhi),(lbhi), \
                           (ibboxlo),(jbboxlo),(kbboxlo),(lbboxlo), \
                           (ibboxhi),(jbboxhi),(kbboxhi),(lbboxhi), \
                           cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP4_BOUNDARIES(name) \
  CCTK_ENDLOOP4STR_BOUNDARIES(name) \

#define CCTK_LOOP4STR_BOUNDARIES(name, cctkGH, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 iblo,jblo,kblo,lblo, \
                                 ibhi,jbhi,kbhi,lbhi, \
                                 ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                                 imin,imax, istr) \
  do { \
    typedef int cctki2_loop4_boundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_BOUNDARIES can only be used in 4 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo),(lblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi),(lbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi),(lbboxlo), (lbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2],cctki2_cctkGH->cctk_lsh[3] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_ldir=-1; cctki2_ldir<=+1; ++cctki2_ldir) { \
    for (int cctki2_kdir=-1; cctki2_kdir<=+1; ++cctki2_kdir) { \
    for (int cctki2_jdir=-1; cctki2_jdir<=+1; ++cctki2_jdir) { \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0)|| \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 0) || (cctki2_jdir==+1 ? cctki2_bbox[3] : 0)|| \
        (cctki2_kdir==-1 ? cctki2_bbox[4] : 0) || (cctki2_kdir==+1 ? cctki2_bbox[5] : 0)|| \
        (cctki2_ldir==-1 ? cctki2_bbox[6] : 0) || (cctki2_ldir==+1 ? cctki2_bbox[7] : 0); \
      if (cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
          cctki2_jdir==-1 ? 0 : cctki2_jdir==0 ? cctki2_blo[1] : cctki2_lsh[1] - cctki2_bhi[1], \
          cctki2_kdir==-1 ? 0 : cctki2_kdir==0 ? cctki2_blo[2] : cctki2_lsh[2] - cctki2_bhi[2], \
          cctki2_ldir==-1 ? 0 : cctki2_ldir==0 ? cctki2_blo[3] : cctki2_lsh[3] - cctki2_bhi[3], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
          cctki2_jdir==-1 ? cctki2_blo[1] : cctki2_jdir==0 ? cctki2_lsh[1] - cctki2_bhi[1] : cctki2_lsh[1], \
          cctki2_kdir==-1 ? cctki2_blo[2] : cctki2_kdir==0 ? cctki2_lsh[2] - cctki2_bhi[2] : cctki2_lsh[2], \
          cctki2_ldir==-1 ? cctki2_blo[3] : cctki2_ldir==0 ? cctki2_lsh[3] - cctki2_bhi[3] : cctki2_lsh[3], \
        }; \
        CCTK_LOOP4STR_NORMAL(name##_boundaries, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             cctki2_idir,cctki2_jdir,cctki2_kdir,cctki2_ldir, \
                             cctki2_bmin[0],cctki2_bmin[1],cctki2_bmin[2],cctki2_bmin[3], \
                             cctki2_bmax[0],cctki2_bmax[1],cctki2_bmax[2],cctki2_bmax[3], \
                             cctki2_cctkGH->cctk_ash[0], \
                             cctki2_cctkGH->cctk_ash[1], \
                             cctki2_cctkGH->cctk_ash[2], \
                             cctki2_cctkGH->cctk_ash[3], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP4STR_BOUNDARIES(name) \
        } CCTK_ENDLOOP4STR_NORMAL(name##_boundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    } /* for dir */ \
    } /* for dir */ \
    } /* for dir */ \
    typedef cctki2_loop4_boundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP4_INTBOUNDARIES(name, cctkGH, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 iblo,jblo,kblo,lblo, \
                                 ibhi,jbhi,kbhi,lbhi, \
                                 ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi,lbboxhi) \
  CCTK_LOOP4STR_INTBOUNDARIES(name, (cctkGH), \
                              i,j,k,l, \
                              ni,nj,nk,nl, \
                              (iblo),(jblo),(kblo),(lblo), \
                              (ibhi),(jbhi),(kbhi),(lbhi), \
                              (ibboxlo),(jbboxlo),(kbboxlo),(lbboxlo), \
                              (ibboxhi),(jbboxhi),(kbboxhi),(lbboxhi), \
                              cctki2_imin_,cctki2_imax_, 1) \

#define CCTK_ENDLOOP4_INTBOUNDARIES(name) \
  CCTK_ENDLOOP4STR_INTBOUNDARIES(name) \

#define CCTK_LOOP4STR_INTBOUNDARIES(name, cctkGH, \
                                    i,j,k,l, \
                                    ni,nj,nk,nl, \
                                    iblo,jblo,kblo,lblo, \
                                    ibhi,jbhi,kbhi,lbhi, \
                                    ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                    ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                                    imin,imax, istr) \
  do { \
    typedef int cctki2_loop4_intboundaries_##name; \
    cGH const *restrict const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_INTBOUNDARIES can only be used in 4 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo),(lblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi),(lbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi),(lbboxlo), (lbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2],cctki2_cctkGH->cctk_lsh[3] }; \
    int const cctki2_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    for (int cctki2_ldir=-1; cctki2_ldir<=+1; ++cctki2_ldir) { \
    for (int cctki2_kdir=-1; cctki2_kdir<=+1; ++cctki2_kdir) { \
    for (int cctki2_jdir=-1; cctki2_jdir<=+1; ++cctki2_jdir) { \
    for (int cctki2_idir=-1; cctki2_idir<=+1; ++cctki2_idir) { \
      int cctki2_any_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 0) || (cctki2_idir==+1 ? cctki2_bbox[1] : 0)|| \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 0) || (cctki2_jdir==+1 ? cctki2_bbox[3] : 0)|| \
        (cctki2_kdir==-1 ? cctki2_bbox[4] : 0) || (cctki2_kdir==+1 ? cctki2_bbox[5] : 0)|| \
        (cctki2_ldir==-1 ? cctki2_bbox[6] : 0) || (cctki2_ldir==+1 ? cctki2_bbox[7] : 0); \
      int cctki2_all_bbox = \
        (cctki2_idir==-1 ? cctki2_bbox[0] : 1) && (cctki2_idir==+1 ? cctki2_bbox[1] : 1)&& \
        (cctki2_jdir==-1 ? cctki2_bbox[2] : 1) && (cctki2_jdir==+1 ? cctki2_bbox[3] : 1)&& \
        (cctki2_kdir==-1 ? cctki2_bbox[4] : 1) && (cctki2_kdir==+1 ? cctki2_bbox[5] : 1)&& \
        (cctki2_ldir==-1 ? cctki2_bbox[6] : 1) && (cctki2_ldir==+1 ? cctki2_bbox[7] : 1); \
      if (cctki2_all_bbox && cctki2_any_bbox) { \
        int const cctki2_bmin[] = { \
          cctki2_idir==-1 ? 0 : cctki2_idir==0 ? cctki2_blo[0] : cctki2_lsh[0] - cctki2_bhi[0], \
          cctki2_jdir==-1 ? 0 : cctki2_jdir==0 ? cctki2_blo[1] : cctki2_lsh[1] - cctki2_bhi[1], \
          cctki2_kdir==-1 ? 0 : cctki2_kdir==0 ? cctki2_blo[2] : cctki2_lsh[2] - cctki2_bhi[2], \
          cctki2_ldir==-1 ? 0 : cctki2_ldir==0 ? cctki2_blo[3] : cctki2_lsh[3] - cctki2_bhi[3], \
        }; \
        int const cctki2_bmax[] = { \
          cctki2_idir==-1 ? cctki2_blo[0] : cctki2_idir==0 ? cctki2_lsh[0] - cctki2_bhi[0] : cctki2_lsh[0], \
          cctki2_jdir==-1 ? cctki2_blo[1] : cctki2_jdir==0 ? cctki2_lsh[1] - cctki2_bhi[1] : cctki2_lsh[1], \
          cctki2_kdir==-1 ? cctki2_blo[2] : cctki2_kdir==0 ? cctki2_lsh[2] - cctki2_bhi[2] : cctki2_lsh[2], \
          cctki2_ldir==-1 ? cctki2_blo[3] : cctki2_ldir==0 ? cctki2_lsh[3] - cctki2_bhi[3] : cctki2_lsh[3], \
        }; \
        CCTK_LOOP4STR_NORMAL(name##_intboundaries, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             cctki2_idir,cctki2_jdir,cctki2_kdir,cctki2_ldir, \
                             cctki2_bmin[0],cctki2_bmin[1],cctki2_bmin[2],cctki2_bmin[3], \
                             cctki2_bmax[0],cctki2_bmax[1],cctki2_bmax[2],cctki2_bmax[3], \
                             cctki2_cctkGH->cctk_ash[0], \
                             cctki2_cctkGH->cctk_ash[1], \
                             cctki2_cctkGH->cctk_ash[2], \
                             cctki2_cctkGH->cctk_ash[3], \
                             imin,imax, cctki2_istr) { \

#define CCTK_ENDLOOP4STR_INTBOUNDARIES(name) \
        } CCTK_ENDLOOP4STR_NORMAL(name##_intboundaries); \
      } /* if bbox */ \
    } /* for dir */ \
    } /* for dir */ \
    } /* for dir */ \
    } /* for dir */ \
    typedef cctki2_loop4_intboundaries_##name cctki2_ensure_proper_nesting; \
  } while (0) \



/* LOOP_ALL */

#define CCTK_LOOP4_ALL(name, cctkGH, \
                       i,j,k,l) \
  CCTK_LOOP4STR_ALL(name, (cctkGH), \
                    i,j,k,l, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP4_ALL(name) \
  CCTK_ENDLOOP4STR_ALL(name) \

#define CCTK_LOOP4STR_ALL(name, cctkGH, \
                          i,j,k,l, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop4_all_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_ALL can only be used in 4 dimensions"); \
    } \
    CCTK_LOOP4STR(name##_all, \
                  i,j,k,l, \
                  0,0,0,0, \
                  cctki3_cctkGH->cctk_lsh[0], \
                  cctki3_cctkGH->cctk_lsh[1], \
                  cctki3_cctkGH->cctk_lsh[2], \
                  cctki3_cctkGH->cctk_lsh[3], \
                  cctki3_cctkGH->cctk_ash[0], \
                  cctki3_cctkGH->cctk_ash[1], \
                  cctki3_cctkGH->cctk_ash[2], \
                  cctki3_cctkGH->cctk_ash[3], \
                  imin,imax, (istr)) { \

#define CCTK_ENDLOOP4STR_ALL(name) \
    } CCTK_ENDLOOP4STR(name##_all); \
    typedef cctki3_loop4_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP4_INT(name, cctkGH, \
                       i,j,k,l) \
  CCTK_LOOP4STR_INT(name, (cctkGH), \
                    i,j,k,l, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP4_INT(name) \
  CCTK_ENDLOOP4STR_INT(name) \

#define CCTK_LOOP4STR_INT(name, cctkGH, \
                          i,j,k,l, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop4_int_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_INT can only be used in 4 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [8]; \
    CCTK_INT cctki3_is_ghostbnd[8]; \
    CCTK_INT cctki3_is_symbnd  [8]; \
    CCTK_INT cctki3_is_physbnd [8]; \
    _Pragma("omp single copyprivate(cctki3_bndsize)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 8, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP4STR_INTERIOR(name##_int, \
                           cctki3_cctkGH, \
                           i,j,k,l, \
                           cctki3_bndsize[0],cctki3_bndsize[2],cctki3_bndsize[4],cctki3_bndsize[6], \
                           cctki3_bndsize[1],cctki3_bndsize[3],cctki3_bndsize[5],cctki3_bndsize[7], \
                           imin,imax, (istr)) { \

#define CCTK_ENDLOOP4STR_INT(name) \
    } CCTK_ENDLOOP4STR_INTERIOR(name##_int); \
    typedef cctki3_loop4_int_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_BND */

#define CCTK_LOOP4_BND(name, cctkGH, \
                       i,j,k,l, \
                       ni,nj,nk,nl) \
  CCTK_LOOP4STR_BND(name, (cctkGH), \
                    i,j,k,l, \
                    ni,nj,nk,nl, \
                    cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP4_BND(name) \
  CCTK_ENDLOOP4STR_BND(name) \

#define CCTK_LOOP4STR_BND(name, cctkGH, \
                          i,j,k,l, \
                          ni,nj,nk,nl, \
                          imin,imax, istr) \
  do { \
    typedef int cctki3_loop4_bnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_BND can only be used in 4 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [8]; \
    CCTK_INT cctki3_is_ghostbnd[8]; \
    CCTK_INT cctki3_is_symbnd  [8]; \
    CCTK_INT cctki3_is_physbnd [8]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 8, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP4STR_BOUNDARIES(name##_bnd, \
                             cctki3_cctkGH, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             cctki3_bndsize[0],cctki3_bndsize[2],cctki3_bndsize[4],cctki3_bndsize[6], \
                             cctki3_bndsize[1],cctki3_bndsize[3],cctki3_bndsize[5],cctki3_bndsize[7], \
                             cctki3_is_physbnd[0],cctki3_is_physbnd[2],cctki3_is_physbnd[4],cctki3_is_physbnd[6], \
                             cctki3_is_physbnd[1],cctki3_is_physbnd[3],cctki3_is_physbnd[5],cctki3_is_physbnd[7], \
                             imin,imax, (istr)) { \

#define CCTK_ENDLOOP4STR_BND(name) \
    } CCTK_ENDLOOP4STR_BOUNDARIES(name##_bnd); \
    typedef cctki3_loop4_bnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INTBND */

#define CCTK_LOOP4_INTBND(name, cctkGH, \
                           i,j,k,l, \
                           ni,nj,nk,nl) \
  CCTK_LOOP4STR_INTBND(name, (cctkGH), \
                        i,j,k,l, \
                        ni,nj,nk,nl, \
                        cctki3_imin_,cctki3_imax_, 1) \

#define CCTK_ENDLOOP4_INTBND(name) \
  CCTK_ENDLOOP4STR_INTBND(name) \

#define CCTK_LOOP4STR_INTBND(name, cctkGH, \
                              i,j,k,l, \
                              ni,nj,nk,nl, \
                              imin,imax, istr) \
  do { \
    typedef int cctki3_loop4_intbnd_##name; \
    cGH const *restrict const cctki3_cctkGH = (cctkGH); \
    if (cctki3_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_INTBND can only be used in 4 dimensions"); \
    } \
    CCTK_INT cctki3_bndsize    [8]; \
    CCTK_INT cctki3_is_ghostbnd[8]; \
    CCTK_INT cctki3_is_symbnd  [8]; \
    CCTK_INT cctki3_is_physbnd [8]; \
    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)") \
    GetBoundarySizesAndTypes \
      (cctki3_cctkGH, 8, cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd); \
    CCTK_LOOP4STR_INTBOUNDARIES(name##_intbnd, \
                                cctki3_cctkGH, \
                                i,j,k,l, \
                                ni,nj,nk,nl, \
                                cctki3_bndsize[0],cctki3_bndsize[2],cctki3_bndsize[4],cctki3_bndsize[6], \
                                cctki3_bndsize[1],cctki3_bndsize[3],cctki3_bndsize[5],cctki3_bndsize[7], \
                                cctki3_is_physbnd[0],cctki3_is_physbnd[2],cctki3_is_physbnd[4],cctki3_is_physbnd[6], \
                                cctki3_is_physbnd[1],cctki3_is_physbnd[3],cctki3_is_physbnd[5],cctki3_is_physbnd[7], \
                                imin,imax, (istr)) { \

#define CCTK_ENDLOOP4STR_INTBND(name) \
    } CCTK_ENDLOOP4STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop4_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP4_NORMAL_DECLARE(name) \
   CCTK_LOOP4STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_imin_, name/**/0_imax_ \

#define CCTK_LOOP4_NORMAL_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP4_NORMAL(name, \
                          i,j,k,l, \
                          ni,nj,nk,nl, \
                          idir,jdir,kdir,ldir, \
                          imin,jmin,kmin,lmin, \
                          imax,jmax,kmax,lmax, \
                          iash,jash,kash,lash) \
   CCTK_LOOP4STR_NORMAL(name, \
                        i,j,k,l, \
                        ni,nj,nk,nl, \
                        idir,jdir,kdir,ldir, \
                        imin,jmin,kmin,lmin, \
                        imax,jmax,kmax,lmax, \
                        iash,jash,kash,lash, \
                        name/**/0_imin_,name/**/0_imax_, 1) \

#define CCTK_ENDLOOP4_NORMAL(name) \
   CCTK_ENDLOOP4STR_NORMAL(name) \

#define CCTK_LOOP4STR_NORMAL_DECLARE(name) \
   && integer :: name/**/0_idir,name/**/0_jdir,name/**/0_kdir,name/**/0_ldir \
   && integer :: name/**/0_imin,name/**/0_jmin,name/**/0_kmin,name/**/0_lmin \
   && integer :: name/**/0_imax,name/**/0_jmax,name/**/0_kmax,name/**/0_lmax \
   && integer :: name/**/0_iash,name/**/0_jash,name/**/0_kash,name/**/0_lash \
   && integer :: name/**/0_istr \

#define CCTK_LOOP4STR_NORMAL_OMP_PRIVATE(name) \
   && !$omp private (i,j,k,l) \
   && !$omp private (ni,nj,nk,nl) \

#define CCTK_LOOP4STR_NORMAL(name, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             idir,jdir,kdir,ldir, \
                             imin_,jmin_,kmin_,lmin_, \
                             imax_,jmax_,kmax_,lmax_, \
                             iash,jash,kash,lash, \
                             imin,imax, istr) \
   && name/**/0_idir = idir \
   && name/**/0_jdir = jdir \
   && name/**/0_kdir = kdir \
   && name/**/0_ldir = ldir \
   && name/**/0_imin = imin_ \
   && name/**/0_jmin = jmin_ \
   && name/**/0_kmin = kmin_ \
   && name/**/0_lmin = lmin_ \
   && name/**/0_imax = imax_ \
   && name/**/0_jmax = jmax_ \
   && name/**/0_kmax = kmax_ \
   && name/**/0_lmax = lmax_ \
   && name/**/0_iash = iash \
   && name/**/0_jash = jash \
   && name/**/0_kash = kash \
   && name/**/0_lash = lash \
   && name/**/0_istr = istr \
   && imin = name/**/0_imin \
   && imax = name/**/0_imax \
   && !$omp do collapse(3) \
   && do l = name/**/0_lmin, name/**/0_lmax \
   && do k = name/**/0_kmin, name/**/0_kmax \
   && do j = name/**/0_jmin, name/**/0_jmax \
   && do i = name/**/0_imin - modulo((imin+name/**/0_iash*(j+name/**/0_jash*(k+name/**/0_kash*(l)))), name/**/0_istr), name/**/0_imax \
   &&    if (name/**/0_idir< 0) ni = i \
   &&    if (name/**/0_jdir< 0) nj = j \
   &&    if (name/**/0_kdir< 0) nk = k \
   &&    if (name/**/0_ldir< 0) nl = l \
   &&    if (name/**/0_idir==0) ni = 0 \
   &&    if (name/**/0_jdir==0) nj = 0 \
   &&    if (name/**/0_kdir==0) nk = 0 \
   &&    if (name/**/0_ldir==0) nl = 0 \
   &&    if (name/**/0_idir> 0) ni = name/**/0_imax+1-i \
   &&    if (name/**/0_jdir> 0) nj = name/**/0_jmax+1-j \
   &&    if (name/**/0_kdir> 0) nk = name/**/0_kmax+1-k \
   &&    if (name/**/0_ldir> 0) nl = name/**/0_lmax+1-l \

#define CCTK_ENDLOOP4STR_NORMAL(name) \
   && end do \
   && end do \
   && end do \
   && end do \



#define CCTK_LOOP4_DECLARE(name) \
   CCTK_LOOP4STR_DECLARE(name) \
   && integer :: name/**/1_imin_, name/**/1_imax_ \

#define CCTK_LOOP4_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_OMP_PRIVATE(name) \

#define CCTK_LOOP4(name, \
                   i,j,k,l, \
                   imin,jmin,kmin,lmin, \
                   imax,jmax,kmax,lmax, \
                   iash,jash,kash,lash) \
   CCTK_LOOP4STR(name, \
                 i,j,k,l, \
                 imin,jmin,kmin,lmin, \
                 imax,jmax,kmax,lmax, \
                 iash,jash,kash,lash, \
                 name/**/1_imin_,name/**/1_imax_, 1) \

#define CCTK_ENDLOOP4(name) \
   CCTK_ENDLOOP4STR(name) \

#define CCTK_LOOP4STR_DECLARE(name) \
   CCTK_LOOP4STR_NORMAL_DECLARE(name) \
   && integer :: name/**/1_ni,name/**/1_nj,name/**/1_nk,name/**/1_nl \

#define CCTK_LOOP4STR_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_NORMAL_OMP_PRIVATE(name) \

#define CCTK_LOOP4STR(name, \
                      i,j,k,l, \
                      imin_,jmin_,kmin_,lmin_, \
                      imax_,jmax_,kmax_,lmax_, \
                      iash,jash,kash,lash, \
                      imin,imax, istr) \
   CCTK_LOOP4STR_NORMAL(name, \
                        i,j,k,l, \
                        name/**/1_ni,name/**/1_nj,name/**/1_nk,name/**/1_nl, \
                        0,0,0,0, \
                        imin_,jmin_,kmin_,lmin_, \
                        imax_,jmax_,kmax_,lmax_, \
                        iash,jash,kash,lash, \
                        imin,imax, istr) \

#define CCTK_ENDLOOP4STR(name) \
   CCTK_ENDLOOP4STR_NORMAL(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP4_INTERIOR_DECLARE(name) \
   CCTK_LOOP4STR_INTERIOR_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP4_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_INTERIOR_OMP_PRIVATE(name) \

#define CCTK_LOOP4_INTERIOR(name, \
                            i,j,k,l, \
                            iblo,jblo,kblo,lblo, \
                            ibhi,jbhi,kbhi,lbhi) \
   CCTK_LOOP4STR_INTERIOR(name, \
                          i,j,k,l, \
                          iblo,jblo,kblo,lblo, \
                          ibhi,jbhi,kbhi,lbhi, \
                          name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP4_INTERIOR(name) \
   CCTK_ENDLOOP4STR_INTERIOR(name) \

#define CCTK_LOOP4STR_INTERIOR_DECLARE(name) \
   CCTK_LOOP4STR_DECLARE(name/**/_interior) \

#define CCTK_LOOP4STR_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_OMP_PRIVATE(name/**/_interior) \

#define CCTK_LOOP4STR_INTERIOR(name, \
                               i,j,k,l, \
                               iblo,jblo,kblo,lblo, \
                               ibhi,jbhi,kbhi,lbhi, \
                               imin,imax, istr) \
   CCTK_LOOP4STR(name/**/_interior, \
                 i,j,k,l, \
                 (iblo)+1, \
                 (jblo)+1, \
                 (kblo)+1, \
                 (lblo)+1, \
                 cctk_lsh(1)-(ibhi), \
                 cctk_lsh(2)-(jbhi), \
                 cctk_lsh(3)-(kbhi), \
                 cctk_lsh(4)-(lbhi), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3),cctk_ash(4), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP4STR_INTERIOR(name) \
   CCTK_ENDLOOP4STR(name/**/_interior) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP4_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP4STR_BOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP4_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_BOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP4_BOUNDARIES(name, \
                              i,j,k,l, \
                              ni,nj,nk,nl, \
                              iblo,jblo,kblo,lblo, \
                              ibhi,jbhi,kbhi,lbhi, \
                              ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                              ibboxhi,jbboxhi,kbboxhi,lbboxhi) \
   CCTK_LOOP4STR_BOUNDARIES(name, \
                            i,j,k,l, \
                            ni,nj,nk,nl, \
                            iblo,jblo,kblo,lblo, \
                            ibhi,jbhi,kbhi,lbhi, \
                            ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                            ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                            name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP4_BOUNDARIES(name) \
   CCTK_ENDLOOP4STR_BOUNDARIES(name) \

#define CCTK_LOOP4STR_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP4STR_NORMAL_DECLARE(name/**/_boundaries) \
   && integer :: name/**/2_blo(4), name/**/2_bhi(4) \
   && integer :: name/**/2_bboxlo(4), name/**/2_bboxhi(4) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && integer :: name/**/2_jdir \
   && integer :: name/**/2_kdir \
   && integer :: name/**/2_ldir \
   && logical :: name/**/2_any_bbox \
   && integer :: name/**/2_bmin(4), name/**/2_bmax(4) \

#define CCTK_LOOP4STR_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_NORMAL_OMP_PRIVATE(name/**/_boundaries) \

#define CCTK_LOOP4STR_BOUNDARIES(name, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 iblo,jblo,kblo,lblo, \
                                 ibhi,jbhi,kbhi,lbhi, \
                                 ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                                 imin,imax, istr) \
   && name/**/2_blo = (/ iblo,jblo,kblo,lblo /) \
   && name/**/2_bhi = (/ ibhi,jbhi,kbhi,lbhi /) \
   && name/**/2_bboxlo = (/ ibboxlo,jbboxlo,kbboxlo,lbboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi,jbboxhi,kbboxhi,lbboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_ldir=-1, +1 \
   && do name/**/2_kdir=-1, +1 \
   && do name/**/2_jdir=-1, +1 \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_kdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(3) /= 0 \
   &&     if (name/**/2_ldir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(4) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(2) /= 0 \
   &&     if (name/**/2_kdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(3) /= 0 \
   &&     if (name/**/2_ldir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(4) /= 0 \
   &&    if (name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_jdir==-1) name/**/2_bmin(2) = 1 \
   &&       if (name/**/2_kdir==-1) name/**/2_bmin(3) = 1 \
   &&       if (name/**/2_ldir==-1) name/**/2_bmin(4) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_jdir== 0) name/**/2_bmin(2) = name/**/2_blo(2)+1 \
   &&       if (name/**/2_kdir== 0) name/**/2_bmin(3) = name/**/2_blo(3)+1 \
   &&       if (name/**/2_ldir== 0) name/**/2_bmin(4) = name/**/2_blo(4)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmin(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmin(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_ldir==+1) name/**/2_bmin(4) = cctk_lsh(4) - name/**/2_bhi(4) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_jdir==-1) name/**/2_bmax(2) = name/**/2_blo(2) \
   &&       if (name/**/2_kdir==-1) name/**/2_bmax(3) = name/**/2_blo(3) \
   &&       if (name/**/2_ldir==-1) name/**/2_bmax(4) = name/**/2_blo(4) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir== 0) name/**/2_bmax(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir== 0) name/**/2_bmax(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_ldir== 0) name/**/2_bmax(4) = cctk_lsh(4) - name/**/2_bhi(4) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmax(2) = cctk_lsh(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmax(3) = cctk_lsh(3) \
   &&       if (name/**/2_ldir==+1) name/**/2_bmax(4) = cctk_lsh(4) \
            CCTK_LOOP4STR_NORMAL(name/**/_boundaries, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 name/**/2_idir,name/**/2_jdir,name/**/2_kdir,name/**/2_ldir, \
                                 name/**/2_bmin(1),name/**/2_bmin(2),name/**/2_bmin(3),name/**/2_bmin(4), \
                                 name/**/2_bmax(1),name/**/2_bmax(2),name/**/2_bmax(3),name/**/2_bmax(4), \
                                 cctk_ash(1), \
                                 cctk_ash(2), \
                                 cctk_ash(3), \
                                 cctk_ash(4), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP4STR_BOUNDARIES(name) \
            CCTK_ENDLOOP4STR_NORMAL(name/**/_boundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \
   && end do /* dir */ \
   && end do /* dir */ \
   && end do /* dir */ \



/* LOOP_INTBOUNDARIES */

#define CCTK_LOOP4_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP4STR_INTBOUNDARIES_DECLARE(name) \
   && integer :: name/**/2_imin_, name/**/2_imax_ \

#define CCTK_LOOP4_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_INTBOUNDARIES_OMP_PRIVATE(name) \

#define CCTK_LOOP4_INTBOUNDARIES(name, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 iblo,jblo,kblo,lblo, \
                                 ibhi,jbhi,kbhi,lbhi, \
                                 ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi,lbboxhi) \
   CCTK_LOOP4STR_INTBOUNDARIES(name, \
                               i,j,k,l, \
                               ni,nj,nk,nl, \
                               iblo,jblo,kblo,lblo, \
                               ibhi,jbhi,kbhi,lbhi, \
                               ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                               ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                               name/**/2_imin_,name/**/2_imax_, 1) \

#define CCTK_ENDLOOP4_INTBOUNDARIES(name) \
   CCTK_ENDLOOP4STR_INTBOUNDARIES(name) \

#define CCTK_LOOP4STR_INTBOUNDARIES_DECLARE(name) \
   CCTK_LOOP4STR_NORMAL_DECLARE(name/**/_intboundaries) \
   && integer :: name/**/2_blo(4), name/**/2_bhi(4) \
   && integer :: name/**/2_bboxlo(4), name/**/2_bboxhi(4) \
   && integer :: name/**/2_istr \
   && integer :: name/**/2_idir \
   && integer :: name/**/2_jdir \
   && integer :: name/**/2_kdir \
   && integer :: name/**/2_ldir \
   && logical :: name/**/2_any_bbox,  name/**/2_all_bbox \
   && integer :: name/**/2_bmin(4), name/**/2_bmax(4) \

#define CCTK_LOOP4STR_INTBOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_NORMAL_OMP_PRIVATE(name/**/_intboundaries) \

#define CCTK_LOOP4STR_INTBOUNDARIES(name, \
                                    i,j,k,l, \
                                    ni,nj,nk,nl, \
                                    iblo,jblo,kblo,lblo, \
                                    ibhi,jbhi,kbhi,lbhi, \
                                    ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                    ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                                    imin,imax, istr) \
   && name/**/2_blo = (/ iblo,jblo,kblo,lblo /) \
   && name/**/2_bhi = (/ ibhi,jbhi,kbhi,lbhi /) \
   && name/**/2_bboxlo = (/ ibboxlo,jbboxlo,kbboxlo,lbboxlo /) \
   && name/**/2_bboxhi = (/ ibboxhi,jbboxhi,kbboxhi,lbboxhi /) \
   && name/**/2_istr = (istr) \
   && do name/**/2_ldir=-1, +1 \
   && do name/**/2_kdir=-1, +1 \
   && do name/**/2_jdir=-1, +1 \
   && do name/**/2_idir=-1, +1 \
   &&     name/**/2_any_bbox = .false. \
   &&     if (name/**/2_idir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_kdir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(3) /= 0 \
   &&     if (name/**/2_ldir==-1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxlo(4) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(2) /= 0 \
   &&     if (name/**/2_kdir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(3) /= 0 \
   &&     if (name/**/2_ldir==+1) name/**/2_any_bbox = name/**/2_any_bbox .or. name/**/2_bboxhi(4) /= 0 \
   &&     name/**/2_all_bbox = .true. \
   &&     if (name/**/2_idir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(1) /= 0 \
   &&     if (name/**/2_jdir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(2) /= 0 \
   &&     if (name/**/2_kdir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(3) /= 0 \
   &&     if (name/**/2_ldir==-1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxlo(4) /= 0 \
   &&     if (name/**/2_idir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(1) /= 0 \
   &&     if (name/**/2_jdir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(2) /= 0 \
   &&     if (name/**/2_kdir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(3) /= 0 \
   &&     if (name/**/2_ldir==+1) name/**/2_all_bbox = name/**/2_all_bbox .and. name/**/2_bboxhi(4) /= 0 \
   &&    if (name/**/2_all_bbox .and. name/**/2_any_bbox) then \
   &&       if (name/**/2_idir==-1) name/**/2_bmin(1) = 1 \
   &&       if (name/**/2_jdir==-1) name/**/2_bmin(2) = 1 \
   &&       if (name/**/2_kdir==-1) name/**/2_bmin(3) = 1 \
   &&       if (name/**/2_ldir==-1) name/**/2_bmin(4) = 1 \
   &&       if (name/**/2_idir== 0) name/**/2_bmin(1) = name/**/2_blo(1)+1 \
   &&       if (name/**/2_jdir== 0) name/**/2_bmin(2) = name/**/2_blo(2)+1 \
   &&       if (name/**/2_kdir== 0) name/**/2_bmin(3) = name/**/2_blo(3)+1 \
   &&       if (name/**/2_ldir== 0) name/**/2_bmin(4) = name/**/2_blo(4)+1 \
   &&       if (name/**/2_idir==+1) name/**/2_bmin(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmin(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmin(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_ldir==+1) name/**/2_bmin(4) = cctk_lsh(4) - name/**/2_bhi(4) \
   &&       if (name/**/2_idir==-1) name/**/2_bmax(1) = name/**/2_blo(1) \
   &&       if (name/**/2_jdir==-1) name/**/2_bmax(2) = name/**/2_blo(2) \
   &&       if (name/**/2_kdir==-1) name/**/2_bmax(3) = name/**/2_blo(3) \
   &&       if (name/**/2_ldir==-1) name/**/2_bmax(4) = name/**/2_blo(4) \
   &&       if (name/**/2_idir== 0) name/**/2_bmax(1) = cctk_lsh(1) - name/**/2_bhi(1) \
   &&       if (name/**/2_jdir== 0) name/**/2_bmax(2) = cctk_lsh(2) - name/**/2_bhi(2) \
   &&       if (name/**/2_kdir== 0) name/**/2_bmax(3) = cctk_lsh(3) - name/**/2_bhi(3) \
   &&       if (name/**/2_ldir== 0) name/**/2_bmax(4) = cctk_lsh(4) - name/**/2_bhi(4) \
   &&       if (name/**/2_idir==+1) name/**/2_bmax(1) = cctk_lsh(1) \
   &&       if (name/**/2_jdir==+1) name/**/2_bmax(2) = cctk_lsh(2) \
   &&       if (name/**/2_kdir==+1) name/**/2_bmax(3) = cctk_lsh(3) \
   &&       if (name/**/2_ldir==+1) name/**/2_bmax(4) = cctk_lsh(4) \
            CCTK_LOOP4STR_NORMAL(name/**/_intboundaries, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 name/**/2_idir,name/**/2_jdir,name/**/2_kdir,name/**/2_ldir, \
                                 name/**/2_bmin(1),name/**/2_bmin(2),name/**/2_bmin(3),name/**/2_bmin(4), \
                                 name/**/2_bmax(1),name/**/2_bmax(2),name/**/2_bmax(3),name/**/2_bmax(4), \
                                 cctk_ash(1), \
                                 cctk_ash(2), \
                                 cctk_ash(3), \
                                 cctk_ash(4), \
                                 imin,imax, name/**/2_istr) \

#define CCTK_ENDLOOP4STR_INTBOUNDARIES(name) \
            CCTK_ENDLOOP4STR_NORMAL(name/**/_intboundaries) \
   &&    end if /* bbox */ \
   && end do /* dir */ \
   && end do /* dir */ \
   && end do /* dir */ \
   && end do /* dir */ \



/* LOOP_ALL */

#define CCTK_LOOP4_ALL_DECLARE(name) \
   CCTK_LOOP4STR_ALL_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP4_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_ALL_OMP_PRIVATE(name) \

#define CCTK_LOOP4_ALL(name, \
                       i,j,k,l) \
   CCTK_LOOP4STR_ALL(name, \
                     i,j,k,l, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP4_ALL(name) \
   CCTK_ENDLOOP4STR_ALL(name) \

#define CCTK_LOOP4STR_ALL_DECLARE(name) \
   CCTK_LOOP4STR_DECLARE(name/**/_all) \

#define CCTK_LOOP4STR_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_OMP_PRIVATE(name/**/_all) \

#define CCTK_LOOP4STR_ALL(name, \
                          i,j,k,l, \
                          imin,imax, istr) \
   CCTK_LOOP4STR(name/**/_all, \
                 i,j,k,l, \
                 1,1,1,1, \
                 cctk_lsh(1),cctk_lsh(2),cctk_lsh(3),cctk_lsh(4), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3),cctk_ash(4), \
                 imin,imax, istr) \

#define CCTK_ENDLOOP4STR_ALL(name) \
   CCTK_ENDLOOP4STR(name/**/_all) \



/* LOOP_INT */

#define CCTK_LOOP4_INT_DECLARE(name) \
   CCTK_LOOP4STR_INT_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP4_INT_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_INT_OMP_PRIVATE(name) \

#define CCTK_LOOP4_INT(name, \
                        i,j,k,l) \
   CCTK_LOOP4STR_INT(name, \
                      i,j,k,l, \
                      name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP4_INT(name) \
   CCTK_ENDLOOP4STR_INT(name) \

#define CCTK_LOOP4STR_INT_DECLARE(name) \
   CCTK_LOOP4STR_INTERIOR_DECLARE(name/**/_int) \
   && integer :: name/**/3_bndsize    (8) \
   && integer :: name/**/3_is_ghostbnd(8) \
   && integer :: name/**/3_is_symbnd  (8) \
   && integer :: name/**/3_is_physbnd (8) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP4STR_INT_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_INTERIOR_OMP_PRIVATE(name/**/_int) \

#define CCTK_LOOP4STR_INT(name, \
                          i,j,k,l, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 8, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize) \
      CCTK_LOOP4STR_INTERIOR(name/**/_int, \
                             i,j,k,l, \
                             name/**/3_bndsize(1+1),name/**/3_bndsize(3+1),name/**/3_bndsize(5+1),name/**/3_bndsize(7+1), \
                             name/**/3_bndsize(2),name/**/3_bndsize(4),name/**/3_bndsize(6),name/**/3_bndsize(8), \
                             imin,imax, (istr)) \

#define CCTK_ENDLOOP4STR_INT(name) \
      CCTK_ENDLOOP4STR_INTERIOR(name/**/int) \



/* LOOP_BND */

#define CCTK_LOOP4_BND_DECLARE(name) \
   CCTK_LOOP4STR_BND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP4_BND_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_BND_OMP_PRIVATE(name) \

#define CCTK_LOOP4_BND(name, \
                       i,j,k,l, \
                       ni,nj,nk,nl) \
   CCTK_LOOP4STR_BND(name, \
                     i,j,k,l, \
                     ni,nj,nk,nl, \
                     name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP4_BND(name) \
   CCTK_ENDLOOP4STR_BND(name) \

#define CCTK_LOOP4STR_BND_DECLARE(name) \
   CCTK_LOOP4STR_BOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (8) \
   && integer :: name/**/3_is_ghostbnd(8) \
   && integer :: name/**/3_is_symbnd  (8) \
   && integer :: name/**/3_is_physbnd (8) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP4STR_BND_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_BOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP4STR_BND(name, \
                          i,j,k,l, \
                          ni,nj,nk,nl, \
                          imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 8, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP4STR_BOUNDARIES(name/**/_bnd, \
                               i,j,k,l, \
                               ni,nj,nk,nl, \
                               name/**/3_bndsize(1)+1,name/**/3_bndsize(3)+1,name/**/3_bndsize(5)+1,name/**/3_bndsize(7)+1, \
                               name/**/3_bndsize(2),name/**/3_bndsize(4),name/**/3_bndsize(6),name/**/3_bndsize(8), \
                               name/**/3_is_physbnd(1),name/**/3_is_physbnd(3),name/**/3_is_physbnd(5),name/**/3_is_physbnd(7), \
                               name/**/3_is_physbnd(2),name/**/3_is_physbnd(4),name/**/3_is_physbnd(6),name/**/3_is_physbnd(8), \
                               imin,imax, (istr)) \

#define CCTK_ENDLOOP4STR_BND(name) \
      CCTK_ENDLOOP4STR_BOUNDARIES(name/**/_bnd) \






/* LOOP_INTBND */

#define CCTK_LOOP4_INTBND_DECLARE(name) \
   CCTK_LOOP4STR_INTBND_DECLARE(name) \
   && integer :: name/**/3_imin_, name/**/3_imax_ \

#define CCTK_LOOP4_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_INTBND_OMP_PRIVATE(name) \

#define CCTK_LOOP4_INTBND(name, \
                          i,j,k,l, \
                          ni,nj,nk,nl) \
   CCTK_LOOP4STR_INTBND(name, \
                        i,j,k,l, \
                        ni,nj,nk,nl, \
                        name/**/3_imin_,name/**/3_imax_, 1) \

#define CCTK_ENDLOOP4_INTBND(name) \
   CCTK_ENDLOOP4STR_INTBND(name) \

#define CCTK_LOOP4STR_INTBND_DECLARE(name) \
   CCTK_LOOP4STR_INTBOUNDARIES_DECLARE(name/**/_bnd) \
   && integer :: name/**/3_bndsize    (8) \
   && integer :: name/**/3_is_ghostbnd(8) \
   && integer :: name/**/3_is_symbnd  (8) \
   && integer :: name/**/3_is_physbnd (8) \
   && integer :: name/**/3_ierr \

#define CCTK_LOOP4STR_INTBND_OMP_PRIVATE(name) \
   CCTK_LOOP4STR_INTBOUNDARIES_OMP_PRIVATE(name/**/_bnd) \

#define CCTK_LOOP4STR_INTBND(name, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             imin,imax, istr) \
   && !$omp single \
   && name/**/3_ierr = GetBoundarySizesAndTypes \
         (cctkGH, 8, name/**/3_bndsize, name/**/3_is_ghostbnd, name/**/3_is_symbnd, name/**/3_is_physbnd) \
   && !$omp end single copyprivate(name/**/3_bndsize, name/**/3_is_physbnd) \
      CCTK_LOOP4STR_INTBOUNDARIES(name/**/_bnd, \
                                  i,j,k,l, \
                                  ni,nj,nk,nl, \
                                  name/**/3_bndsize(1+1),name/**/3_bndsize(3+1),name/**/3_bndsize(5+1),name/**/3_bndsize(7+1), \
                                  name/**/3_bndsize(2),name/**/3_bndsize(4),name/**/3_bndsize(6),name/**/3_bndsize(8), \
                                  name/**/3_is_physbnd(1),name/**/3_is_physbnd(3),name/**/3_is_physbnd(5),name/**/3_is_physbnd(7), \
                                  name/**/3_is_physbnd(2),name/**/3_is_physbnd(4),name/**/3_is_physbnd(6),name/**/3_is_physbnd(8), \
                                  imin,imax, (istr)) \

#define CCTK_ENDLOOP4STR_INTBND(name) \
      CCTK_ENDLOOP4STR_INTBOUNDARIES(name/**/_bnd) \

#endif /* #ifdef FCODE */



#endif /* #ifndef _CCTK_LOOP_H_ */
