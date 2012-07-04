#ifndef _CCTK_LOOP_H_
#define _CCTK_LOOP_H_

/* WARNING: This file is auto-generated. Do not edit. */
/* Edit cctk_Loop.h.pl instead, and then re-generate this file via */
/*    perl cctk_Loop.h.pl.pl */
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
                       1) \

#define CCTK_ENDLOOP1_NORMAL(name) \
  CCTK_ENDLOOP1STR_NORMAL(name) \

#define CCTK_LOOP1STR_NORMAL(name, \
                             i, \
                             ni, \
                             idir, \
                             imin, \
                             imax, \
                             iash, \
                             istr) \
  do { \
    typedef int cctki0_loop1_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_imin = (imin); \
    int const cctki0_imax = (imax); \
    int const cctki0_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    assert(cctki0_istr == 1); \
    _Pragma("omp for") \
    for (int i=cctki0_imin; i<cctki0_imax; ++i) { \
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
                1) \

#define CCTK_ENDLOOP1(name) \
  CCTK_ENDLOOP1STR(name) \

#define CCTK_LOOP1STR(name, \
                      i, \
                      imin, \
                      imax, \
                      iash, \
                      istr) \
  CCTK_LOOP1STR_NORMAL(name, \
                       i, \
                       cctki1_ni, \
                       0, \
                       imin, \
                       imax, \
                       iash, \
                       istr) \

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
                         1) \

#define CCTK_ENDLOOP1_INTERIOR(name) \
  CCTK_ENDLOOP1STR_INTERIOR(name) \

#define CCTK_LOOP1STR_INTERIOR(name, cctkGH, \
                               i, \
                               iblo, \
                               ibhi, \
                               istr) \
  do { \
    typedef int cctki2_loop1_interior_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
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
                  (istr)) { \

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
                           1) \

#define CCTK_ENDLOOP1_BOUNDARIES(name) \
  CCTK_ENDLOOP1STR_BOUNDARIES(name) \

#define CCTK_LOOP1STR_BOUNDARIES(name, cctkGH, \
                                 i, \
                                 ni, \
                                 iblo, \
                                 ibhi, \
                                 ibboxlo, \
                                 ibboxhi, \
                                 istr) \
  do { \
    typedef int cctki2_loop1_boundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_BOUNDARIES can only be used in 1 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo) }; \
    int const cctki2_bhi[] = { (ibhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                              1) \

#define CCTK_ENDLOOP1_INTBOUNDARIES(name) \
  CCTK_ENDLOOP1STR_INTBOUNDARIES(name) \

#define CCTK_LOOP1STR_INTBOUNDARIES(name, cctkGH, \
                                    i, \
                                    ni, \
                                    iblo, \
                                    ibhi, \
                                    ibboxlo, \
                                    ibboxhi, \
                                    istr) \
  do { \
    typedef int cctki2_loop1_intboundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 1) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP1_INTBOUNDARIES can only be used in 1 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo) }; \
    int const cctki2_bhi[] = { (ibhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                    1) \

#define CCTK_ENDLOOP1_ALL(name) \
  CCTK_ENDLOOP1STR_ALL(name) \

#define CCTK_LOOP1STR_ALL(name, cctkGH, \
                          i, \
                          istr) \
  do { \
    typedef int cctki3_loop1_all_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                  (istr)) { \

#define CCTK_ENDLOOP1STR_ALL(name) \
    } CCTK_ENDLOOP1STR(name##_all); \
    typedef cctki3_loop1_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP1_INT(name, cctkGH, \
                       i) \
  CCTK_LOOP1STR_INT(name, (cctkGH), \
                    i, \
                    1) \

#define CCTK_ENDLOOP1_INT(name) \
  CCTK_ENDLOOP1STR_INT(name) \

#define CCTK_LOOP1STR_INT(name, cctkGH, \
                          i, \
                          istr) \
  do { \
    typedef int cctki3_loop1_int_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                           (istr)) { \

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
                    1) \

#define CCTK_ENDLOOP1_BND(name) \
  CCTK_ENDLOOP1STR_BND(name) \

#define CCTK_LOOP1STR_BND(name, cctkGH, \
                          i, \
                          ni, \
                          istr) \
  do { \
    typedef int cctki3_loop1_bnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                             (istr)) { \

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
                        1) \

#define CCTK_ENDLOOP1_INTBND(name) \
  CCTK_ENDLOOP1STR_INTBND(name) \

#define CCTK_LOOP1STR_INTBND(name, cctkGH, \
                              i, \
                              ni, \
                              istr) \
  do { \
    typedef int cctki3_loop1_intbnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                                (istr)) { \

#define CCTK_ENDLOOP1STR_INTBND(name) \
    } CCTK_ENDLOOP1STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop1_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP1_DECLARE(name) \
   && integer :: name/**/_imin \
   && integer :: name/**/_imax \
   && integer :: name/**/_istr \

#define CCTK_LOOP1_OMP_PRIVATE(name) \

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
             1) \

#define CCTK_LOOP1STR(name, \
                      i, \
                      imin, \
                      imax, \
                      iash, \
                      istr) \
   && name/**/_imin = imin \
   && name/**/_imax = imax \
   && name/**/_istr = istr \
   && do i = name/**/_imin, name/**/_imax \

#define CCTK_ENDLOOP1(name) \
  CCTK_ENDLOOP1STR(name) \

#define CCTK_ENDLOOP1STR(name) \
   && end do \



/* LOOP_ALL */

#define CCTK_LOOP1_ALL_DECLARE(name) \
   CCTK_LOOP1_DECLARE(name) \

#define CCTK_LOOP1_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP1_OMP_PRIVATE(name) \

#define CCTK_LOOP1_ALL(name, \
                       i) \
  CCTK_LOOP1STR_ALL(name, \
                 i, \
                 1) \

#define CCTK_LOOP1STR_ALL(name, \
                          i, \
                          istr) \
   CCTK_LOOP1STR(name, \
                 i, \
                 1, \
                 cctk_lsh(1), \
                 cctk_ash(1), \
                 istr) \

#define CCTK_ENDLOOP1_ALL(name) \
  CCTK_ENDLOOP1STR_ALL(name) \

#define CCTK_ENDLOOP1STR_ALL(name) \
   CCTK_ENDLOOP1(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP1_INTERIOR_DECLARE(name) \
   CCTK_LOOP1_DECLARE(name) \

#define CCTK_LOOP1_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP1_OMP_PRIVATE(name) \

#define CCTK_LOOP1_INTERIOR(name, \
                            i, \
                            iblo, \
                            ibhi) \
  CCTK_LOOP1STR_INTERIOR(name, \
                      i, \
                      iblo, \
                      ibhi, \
                      1) \

#define CCTK_LOOP1STR_INTERIOR(name, \
                               i, \
                               iblo, \
                               ibhi, \
                               istr) \
   CCTK_LOOP1STR(name, \
                 i, \
                 (iblo), \
                 cctk_lsh(1)-(ibhi), \
                 cctk_ash(1), \
                 istr) \

#define CCTK_ENDLOOP1_INTERIOR(name) \
  CCTK_ENDLOOP1STR_INTERIOR(name) \

#define CCTK_ENDLOOP1STR_INTERIOR(name) \
   CCTK_ENDLOOP1(name) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP1_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP1_DECLARE(name) \
   && integer :: lc_bmin(1), lc_bmax(1) \
   && integer :: lc_blo(1), lc_bhi(1) \
   && integer :: lc_istr \
   && integer :: lc_dir, lc_face \
   && integer :: lc_d \

#define CCTK_LOOP1_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP1_OMP_PRIVATE(name) \

#define CCTK_LOOP1_BOUNDARIES(name, \
                              i, \
                              iblo, \
                              ibhi) \
  CCTK_LOOP1STR_BOUNDARIES(name, \
                        i, \
                        iblo, \
                        ibhi, \
                        1) \

#define CCTK_LOOP1STR_BOUNDARIES(name, \
                                 i, \
                                 iblo, \
                                 ibhi, \
                                 istr) \
   && lc_blo = (/ iblo /) \
   && lc_bhi = (/ ibhi /) \
   && lc_istr = istr \
   && do lc_dir=1,1 \
   &&    do lc_face=1,2 \
   &&       do lc_d=1,1 \
   &&          lc_bmin(lc_d) = 1 \
   &&          lc_bmax(lc_d) = cctk_lsh(lc_d) \
   &&          if (lc_d<lc_dir) then \
   &&             lc_bmin(lc_d) = lc_bmin(lc_d)+lc_blo(lc_d) \
   &&             lc_bmax(lc_d) = lc_bmax(lc_d)-lc_bhi(lc_d) \
   &&          end if \
   &&       end do \
   &&       if (lc_face==1) then \
   &&          lc_bmax(lc_dir) = lc_bmin(lc_dir)+lc_blo(lc_dir) \
   &&       else \
   &&          lc_bmin(lc_dir) = lc_bmax(lc_dir)-lc_bhi(lc_dir) \
   &&       end if \
            CCTK_LOOP1STR(name, \
                          i, \
                          lc_bmin(1), \
                          lc_bmax(1), \
                          cctk_ash(1), \
                          lc_istr) \

#define CCTK_ENDLOOP1_BOUNDARIES(name) \
  CCTK_ENDLOOP1STR_BOUNDARIES(name) \

#define CCTK_ENDLOOP1STR_BOUNDARIES(name) \
            CCTK_ENDLOOP1(name) \
   &&    end do /* face */ \
   && end do /* dir */ \

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
                       1) \

#define CCTK_ENDLOOP2_NORMAL(name) \
  CCTK_ENDLOOP2STR_NORMAL(name) \

#define CCTK_LOOP2STR_NORMAL(name, \
                             i,j, \
                             ni,nj, \
                             idir,jdir, \
                             imin,jmin, \
                             imax,jmax, \
                             iash,jash, \
                             istr) \
  do { \
    typedef int cctki0_loop2_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_jdir = (jdir); \
    int const cctki0_imin = (imin); \
    int const cctki0_jmin = (jmin); \
    int const cctki0_imax = (imax); \
    int const cctki0_jmax = (jmax); \
    int const cctki0_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    assert(cctki0_istr == 1); \
    _Pragma("omp for") \
    for (int j=cctki0_jmin; j<cctki0_jmax; ++j) { \
    for (int i=cctki0_imin; i<cctki0_imax; ++i) { \
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
                1) \

#define CCTK_ENDLOOP2(name) \
  CCTK_ENDLOOP2STR(name) \

#define CCTK_LOOP2STR(name, \
                      i,j, \
                      imin,jmin, \
                      imax,jmax, \
                      iash,jash, \
                      istr) \
  CCTK_LOOP2STR_NORMAL(name, \
                       i,j, \
                       cctki1_ni,cctki1_nj, \
                       0,0, \
                       imin,jmin, \
                       imax,jmax, \
                       iash,jash, \
                       istr) \

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
                         1) \

#define CCTK_ENDLOOP2_INTERIOR(name) \
  CCTK_ENDLOOP2STR_INTERIOR(name) \

#define CCTK_LOOP2STR_INTERIOR(name, cctkGH, \
                               i,j, \
                               iblo,jblo, \
                               ibhi,jbhi, \
                               istr) \
  do { \
    typedef int cctki2_loop2_interior_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
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
                  (istr)) { \

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
                           1) \

#define CCTK_ENDLOOP2_BOUNDARIES(name) \
  CCTK_ENDLOOP2STR_BOUNDARIES(name) \

#define CCTK_LOOP2STR_BOUNDARIES(name, cctkGH, \
                                 i,j, \
                                 ni,nj, \
                                 iblo,jblo, \
                                 ibhi,jbhi, \
                                 ibboxlo,jbboxlo, \
                                 ibboxhi,jbboxhi, \
                                 istr) \
  do { \
    typedef int cctki2_loop2_boundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_BOUNDARIES can only be used in 2 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                              1) \

#define CCTK_ENDLOOP2_INTBOUNDARIES(name) \
  CCTK_ENDLOOP2STR_INTBOUNDARIES(name) \

#define CCTK_LOOP2STR_INTBOUNDARIES(name, cctkGH, \
                                    i,j, \
                                    ni,nj, \
                                    iblo,jblo, \
                                    ibhi,jbhi, \
                                    ibboxlo,jbboxlo, \
                                    ibboxhi,jbboxhi, \
                                    istr) \
  do { \
    typedef int cctki2_loop2_intboundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 2) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP2_INTBOUNDARIES can only be used in 2 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                    1) \

#define CCTK_ENDLOOP2_ALL(name) \
  CCTK_ENDLOOP2STR_ALL(name) \

#define CCTK_LOOP2STR_ALL(name, cctkGH, \
                          i,j, \
                          istr) \
  do { \
    typedef int cctki3_loop2_all_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                  (istr)) { \

#define CCTK_ENDLOOP2STR_ALL(name) \
    } CCTK_ENDLOOP2STR(name##_all); \
    typedef cctki3_loop2_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP2_INT(name, cctkGH, \
                       i,j) \
  CCTK_LOOP2STR_INT(name, (cctkGH), \
                    i,j, \
                    1) \

#define CCTK_ENDLOOP2_INT(name) \
  CCTK_ENDLOOP2STR_INT(name) \

#define CCTK_LOOP2STR_INT(name, cctkGH, \
                          i,j, \
                          istr) \
  do { \
    typedef int cctki3_loop2_int_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                           (istr)) { \

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
                    1) \

#define CCTK_ENDLOOP2_BND(name) \
  CCTK_ENDLOOP2STR_BND(name) \

#define CCTK_LOOP2STR_BND(name, cctkGH, \
                          i,j, \
                          ni,nj, \
                          istr) \
  do { \
    typedef int cctki3_loop2_bnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                             (istr)) { \

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
                        1) \

#define CCTK_ENDLOOP2_INTBND(name) \
  CCTK_ENDLOOP2STR_INTBND(name) \

#define CCTK_LOOP2STR_INTBND(name, cctkGH, \
                              i,j, \
                              ni,nj, \
                              istr) \
  do { \
    typedef int cctki3_loop2_intbnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                                (istr)) { \

#define CCTK_ENDLOOP2STR_INTBND(name) \
    } CCTK_ENDLOOP2STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop2_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP2_DECLARE(name) \
   && integer :: name/**/_imin,name/**/_jmin \
   && integer :: name/**/_imax,name/**/_jmax \
   && integer :: name/**/_istr \

#define CCTK_LOOP2_OMP_PRIVATE(name) \

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
             1) \

#define CCTK_LOOP2STR(name, \
                      i,j, \
                      imin,jmin, \
                      imax,jmax, \
                      iash,jash, \
                      istr) \
   && name/**/_imin = imin \
   && name/**/_jmin = jmin \
   && name/**/_imax = imax \
   && name/**/_jmax = jmax \
   && name/**/_istr = istr \
   && do j = name/**/_jmin, name/**/_jmax \
   && do i = name/**/_imin, name/**/_imax \

#define CCTK_ENDLOOP2(name) \
  CCTK_ENDLOOP2STR(name) \

#define CCTK_ENDLOOP2STR(name) \
   && end do \
   && end do \



/* LOOP_ALL */

#define CCTK_LOOP2_ALL_DECLARE(name) \
   CCTK_LOOP2_DECLARE(name) \

#define CCTK_LOOP2_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP2_OMP_PRIVATE(name) \

#define CCTK_LOOP2_ALL(name, \
                       i,j) \
  CCTK_LOOP2STR_ALL(name, \
                 i,j, \
                 1) \

#define CCTK_LOOP2STR_ALL(name, \
                          i,j, \
                          istr) \
   CCTK_LOOP2STR(name, \
                 i,j, \
                 1,1, \
                 cctk_lsh(1),cctk_lsh(2), \
                 cctk_ash(1),cctk_ash(2), \
                 istr) \

#define CCTK_ENDLOOP2_ALL(name) \
  CCTK_ENDLOOP2STR_ALL(name) \

#define CCTK_ENDLOOP2STR_ALL(name) \
   CCTK_ENDLOOP2(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP2_INTERIOR_DECLARE(name) \
   CCTK_LOOP2_DECLARE(name) \

#define CCTK_LOOP2_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP2_OMP_PRIVATE(name) \

#define CCTK_LOOP2_INTERIOR(name, \
                            i,j, \
                            iblo,jblo, \
                            ibhi,jbhi) \
  CCTK_LOOP2STR_INTERIOR(name, \
                      i,j, \
                      iblo,jblo, \
                      ibhi,jbhi, \
                      1) \

#define CCTK_LOOP2STR_INTERIOR(name, \
                               i,j, \
                               iblo,jblo, \
                               ibhi,jbhi, \
                               istr) \
   CCTK_LOOP2STR(name, \
                 i,j, \
                 (iblo),(jblo), \
                 cctk_lsh(1)-(ibhi), \
                 cctk_lsh(2)-(jbhi), \
                 cctk_ash(1),cctk_ash(2), \
                 istr) \

#define CCTK_ENDLOOP2_INTERIOR(name) \
  CCTK_ENDLOOP2STR_INTERIOR(name) \

#define CCTK_ENDLOOP2STR_INTERIOR(name) \
   CCTK_ENDLOOP2(name) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP2_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP2_DECLARE(name) \
   && integer :: lc_bmin(2), lc_bmax(2) \
   && integer :: lc_blo(2), lc_bhi(2) \
   && integer :: lc_istr \
   && integer :: lc_dir, lc_face \
   && integer :: lc_d \

#define CCTK_LOOP2_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP2_OMP_PRIVATE(name) \

#define CCTK_LOOP2_BOUNDARIES(name, \
                              i,j, \
                              iblo,jblo, \
                              ibhi,jbhi) \
  CCTK_LOOP2STR_BOUNDARIES(name, \
                        i,j, \
                        iblo,jblo, \
                        ibhi,jbhi, \
                        1) \

#define CCTK_LOOP2STR_BOUNDARIES(name, \
                                 i,j, \
                                 iblo,jblo, \
                                 ibhi,jbhi, \
                                 istr) \
   && lc_blo = (/ iblo,jblo /) \
   && lc_bhi = (/ ibhi,jbhi /) \
   && lc_istr = istr \
   && do lc_dir=1,2 \
   &&    do lc_face=1,2 \
   &&       do lc_d=1,2 \
   &&          lc_bmin(lc_d) = 1 \
   &&          lc_bmax(lc_d) = cctk_lsh(lc_d) \
   &&          if (lc_d<lc_dir) then \
   &&             lc_bmin(lc_d) = lc_bmin(lc_d)+lc_blo(lc_d) \
   &&             lc_bmax(lc_d) = lc_bmax(lc_d)-lc_bhi(lc_d) \
   &&          end if \
   &&       end do \
   &&       if (lc_face==1) then \
   &&          lc_bmax(lc_dir) = lc_bmin(lc_dir)+lc_blo(lc_dir) \
   &&       else \
   &&          lc_bmin(lc_dir) = lc_bmax(lc_dir)-lc_bhi(lc_dir) \
   &&       end if \
            CCTK_LOOP2STR(name, \
                          i,j, \
                          lc_bmin(1),lc_bmin(2), \
                          lc_bmax(1),lc_bmax(2), \
                          cctk_ash(1),cctk_ash(2), \
                          lc_istr) \

#define CCTK_ENDLOOP2_BOUNDARIES(name) \
  CCTK_ENDLOOP2STR_BOUNDARIES(name) \

#define CCTK_ENDLOOP2STR_BOUNDARIES(name) \
            CCTK_ENDLOOP2(name) \
   &&    end do /* face */ \
   && end do /* dir */ \

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
                       1) \

#define CCTK_ENDLOOP3_NORMAL(name) \
  CCTK_ENDLOOP3STR_NORMAL(name) \

#define CCTK_LOOP3STR_NORMAL(name, \
                             i,j,k, \
                             ni,nj,nk, \
                             idir,jdir,kdir, \
                             imin,jmin,kmin, \
                             imax,jmax,kmax, \
                             iash,jash,kash, \
                             istr) \
  do { \
    typedef int cctki0_loop3_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_jdir = (jdir); \
    int const cctki0_kdir = (kdir); \
    int const cctki0_imin = (imin); \
    int const cctki0_jmin = (jmin); \
    int const cctki0_kmin = (kmin); \
    int const cctki0_imax = (imax); \
    int const cctki0_jmax = (jmax); \
    int const cctki0_kmax = (kmax); \
    int const cctki0_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    assert(cctki0_istr == 1); \
    _Pragma("omp for") \
    for (int k=cctki0_kmin; k<cctki0_kmax; ++k) { \
    for (int j=cctki0_jmin; j<cctki0_jmax; ++j) { \
    for (int i=cctki0_imin; i<cctki0_imax; ++i) { \
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
                1) \

#define CCTK_ENDLOOP3(name) \
  CCTK_ENDLOOP3STR(name) \

#define CCTK_LOOP3STR(name, \
                      i,j,k, \
                      imin,jmin,kmin, \
                      imax,jmax,kmax, \
                      iash,jash,kash, \
                      istr) \
  CCTK_LOOP3STR_NORMAL(name, \
                       i,j,k, \
                       cctki1_ni,cctki1_nj,cctki1_nk, \
                       0,0,0, \
                       imin,jmin,kmin, \
                       imax,jmax,kmax, \
                       iash,jash,kash, \
                       istr) \

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
                         1) \

#define CCTK_ENDLOOP3_INTERIOR(name) \
  CCTK_ENDLOOP3STR_INTERIOR(name) \

#define CCTK_LOOP3STR_INTERIOR(name, cctkGH, \
                               i,j,k, \
                               iblo,jblo,kblo, \
                               ibhi,jbhi,kbhi, \
                               istr) \
  do { \
    typedef int cctki2_loop3_interior_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
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
                  (istr)) { \

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
                           1) \

#define CCTK_ENDLOOP3_BOUNDARIES(name) \
  CCTK_ENDLOOP3STR_BOUNDARIES(name) \

#define CCTK_LOOP3STR_BOUNDARIES(name, cctkGH, \
                                 i,j,k, \
                                 ni,nj,nk, \
                                 iblo,jblo,kblo, \
                                 ibhi,jbhi,kbhi, \
                                 ibboxlo,jbboxlo,kbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi, \
                                 istr) \
  do { \
    typedef int cctki2_loop3_boundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_BOUNDARIES can only be used in 3 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                              1) \

#define CCTK_ENDLOOP3_INTBOUNDARIES(name) \
  CCTK_ENDLOOP3STR_INTBOUNDARIES(name) \

#define CCTK_LOOP3STR_INTBOUNDARIES(name, cctkGH, \
                                    i,j,k, \
                                    ni,nj,nk, \
                                    iblo,jblo,kblo, \
                                    ibhi,jbhi,kbhi, \
                                    ibboxlo,jbboxlo,kbboxlo, \
                                    ibboxhi,jbboxhi,kbboxhi, \
                                    istr) \
  do { \
    typedef int cctki2_loop3_intboundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 3) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP3_INTBOUNDARIES can only be used in 3 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                    1) \

#define CCTK_ENDLOOP3_ALL(name) \
  CCTK_ENDLOOP3STR_ALL(name) \

#define CCTK_LOOP3STR_ALL(name, cctkGH, \
                          i,j,k, \
                          istr) \
  do { \
    typedef int cctki3_loop3_all_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                  (istr)) { \

#define CCTK_ENDLOOP3STR_ALL(name) \
    } CCTK_ENDLOOP3STR(name##_all); \
    typedef cctki3_loop3_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP3_INT(name, cctkGH, \
                       i,j,k) \
  CCTK_LOOP3STR_INT(name, (cctkGH), \
                    i,j,k, \
                    1) \

#define CCTK_ENDLOOP3_INT(name) \
  CCTK_ENDLOOP3STR_INT(name) \

#define CCTK_LOOP3STR_INT(name, cctkGH, \
                          i,j,k, \
                          istr) \
  do { \
    typedef int cctki3_loop3_int_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                           (istr)) { \

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
                    1) \

#define CCTK_ENDLOOP3_BND(name) \
  CCTK_ENDLOOP3STR_BND(name) \

#define CCTK_LOOP3STR_BND(name, cctkGH, \
                          i,j,k, \
                          ni,nj,nk, \
                          istr) \
  do { \
    typedef int cctki3_loop3_bnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                             (istr)) { \

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
                        1) \

#define CCTK_ENDLOOP3_INTBND(name) \
  CCTK_ENDLOOP3STR_INTBND(name) \

#define CCTK_LOOP3STR_INTBND(name, cctkGH, \
                              i,j,k, \
                              ni,nj,nk, \
                              istr) \
  do { \
    typedef int cctki3_loop3_intbnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                                (istr)) { \

#define CCTK_ENDLOOP3STR_INTBND(name) \
    } CCTK_ENDLOOP3STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop3_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP3_DECLARE(name) \
   && integer :: name/**/_imin,name/**/_jmin,name/**/_kmin \
   && integer :: name/**/_imax,name/**/_jmax,name/**/_kmax \
   && integer :: name/**/_istr \

#define CCTK_LOOP3_OMP_PRIVATE(name) \

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
             1) \

#define CCTK_LOOP3STR(name, \
                      i,j,k, \
                      imin,jmin,kmin, \
                      imax,jmax,kmax, \
                      iash,jash,kash, \
                      istr) \
   && name/**/_imin = imin \
   && name/**/_jmin = jmin \
   && name/**/_kmin = kmin \
   && name/**/_imax = imax \
   && name/**/_jmax = jmax \
   && name/**/_kmax = kmax \
   && name/**/_istr = istr \
   && do k = name/**/_kmin, name/**/_kmax \
   && do j = name/**/_jmin, name/**/_jmax \
   && do i = name/**/_imin, name/**/_imax \

#define CCTK_ENDLOOP3(name) \
  CCTK_ENDLOOP3STR(name) \

#define CCTK_ENDLOOP3STR(name) \
   && end do \
   && end do \
   && end do \



/* LOOP_ALL */

#define CCTK_LOOP3_ALL_DECLARE(name) \
   CCTK_LOOP3_DECLARE(name) \

#define CCTK_LOOP3_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP3_OMP_PRIVATE(name) \

#define CCTK_LOOP3_ALL(name, \
                       i,j,k) \
  CCTK_LOOP3STR_ALL(name, \
                 i,j,k, \
                 1) \

#define CCTK_LOOP3STR_ALL(name, \
                          i,j,k, \
                          istr) \
   CCTK_LOOP3STR(name, \
                 i,j,k, \
                 1,1,1, \
                 cctk_lsh(1),cctk_lsh(2),cctk_lsh(3), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3), \
                 istr) \

#define CCTK_ENDLOOP3_ALL(name) \
  CCTK_ENDLOOP3STR_ALL(name) \

#define CCTK_ENDLOOP3STR_ALL(name) \
   CCTK_ENDLOOP3(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP3_INTERIOR_DECLARE(name) \
   CCTK_LOOP3_DECLARE(name) \

#define CCTK_LOOP3_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP3_OMP_PRIVATE(name) \

#define CCTK_LOOP3_INTERIOR(name, \
                            i,j,k, \
                            iblo,jblo,kblo, \
                            ibhi,jbhi,kbhi) \
  CCTK_LOOP3STR_INTERIOR(name, \
                      i,j,k, \
                      iblo,jblo,kblo, \
                      ibhi,jbhi,kbhi, \
                      1) \

#define CCTK_LOOP3STR_INTERIOR(name, \
                               i,j,k, \
                               iblo,jblo,kblo, \
                               ibhi,jbhi,kbhi, \
                               istr) \
   CCTK_LOOP3STR(name, \
                 i,j,k, \
                 (iblo),(jblo),(kblo), \
                 cctk_lsh(1)-(ibhi), \
                 cctk_lsh(2)-(jbhi), \
                 cctk_lsh(3)-(kbhi), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3), \
                 istr) \

#define CCTK_ENDLOOP3_INTERIOR(name) \
  CCTK_ENDLOOP3STR_INTERIOR(name) \

#define CCTK_ENDLOOP3STR_INTERIOR(name) \
   CCTK_ENDLOOP3(name) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP3_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP3_DECLARE(name) \
   && integer :: lc_bmin(3), lc_bmax(3) \
   && integer :: lc_blo(3), lc_bhi(3) \
   && integer :: lc_istr \
   && integer :: lc_dir, lc_face \
   && integer :: lc_d \

#define CCTK_LOOP3_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP3_OMP_PRIVATE(name) \

#define CCTK_LOOP3_BOUNDARIES(name, \
                              i,j,k, \
                              iblo,jblo,kblo, \
                              ibhi,jbhi,kbhi) \
  CCTK_LOOP3STR_BOUNDARIES(name, \
                        i,j,k, \
                        iblo,jblo,kblo, \
                        ibhi,jbhi,kbhi, \
                        1) \

#define CCTK_LOOP3STR_BOUNDARIES(name, \
                                 i,j,k, \
                                 iblo,jblo,kblo, \
                                 ibhi,jbhi,kbhi, \
                                 istr) \
   && lc_blo = (/ iblo,jblo,kblo /) \
   && lc_bhi = (/ ibhi,jbhi,kbhi /) \
   && lc_istr = istr \
   && do lc_dir=1,3 \
   &&    do lc_face=1,2 \
   &&       do lc_d=1,3 \
   &&          lc_bmin(lc_d) = 1 \
   &&          lc_bmax(lc_d) = cctk_lsh(lc_d) \
   &&          if (lc_d<lc_dir) then \
   &&             lc_bmin(lc_d) = lc_bmin(lc_d)+lc_blo(lc_d) \
   &&             lc_bmax(lc_d) = lc_bmax(lc_d)-lc_bhi(lc_d) \
   &&          end if \
   &&       end do \
   &&       if (lc_face==1) then \
   &&          lc_bmax(lc_dir) = lc_bmin(lc_dir)+lc_blo(lc_dir) \
   &&       else \
   &&          lc_bmin(lc_dir) = lc_bmax(lc_dir)-lc_bhi(lc_dir) \
   &&       end if \
            CCTK_LOOP3STR(name, \
                          i,j,k, \
                          lc_bmin(1),lc_bmin(2),lc_bmin(3), \
                          lc_bmax(1),lc_bmax(2),lc_bmax(3), \
                          cctk_ash(1),cctk_ash(2),cctk_ash(3), \
                          lc_istr) \

#define CCTK_ENDLOOP3_BOUNDARIES(name) \
  CCTK_ENDLOOP3STR_BOUNDARIES(name) \

#define CCTK_ENDLOOP3STR_BOUNDARIES(name) \
            CCTK_ENDLOOP3(name) \
   &&    end do /* face */ \
   && end do /* dir */ \

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
                       1) \

#define CCTK_ENDLOOP4_NORMAL(name) \
  CCTK_ENDLOOP4STR_NORMAL(name) \

#define CCTK_LOOP4STR_NORMAL(name, \
                             i,j,k,l, \
                             ni,nj,nk,nl, \
                             idir,jdir,kdir,ldir, \
                             imin,jmin,kmin,lmin, \
                             imax,jmax,kmax,lmax, \
                             iash,jash,kash,lash, \
                             istr) \
  do { \
    typedef int cctki0_loop4_normal_##name; \
    int const cctki0_idir = (idir); \
    int const cctki0_jdir = (jdir); \
    int const cctki0_kdir = (kdir); \
    int const cctki0_ldir = (ldir); \
    int const cctki0_imin = (imin); \
    int const cctki0_jmin = (jmin); \
    int const cctki0_kmin = (kmin); \
    int const cctki0_lmin = (lmin); \
    int const cctki0_imax = (imax); \
    int const cctki0_jmax = (jmax); \
    int const cctki0_kmax = (kmax); \
    int const cctki0_lmax = (lmax); \
    int const cctki0_istr CCTK_ATTRIBUTE_UNUSED = (istr); \
    assert(cctki0_istr == 1); \
    _Pragma("omp for") \
    for (int l=cctki0_lmin; l<cctki0_lmax; ++l) { \
    for (int k=cctki0_kmin; k<cctki0_kmax; ++k) { \
    for (int j=cctki0_jmin; j<cctki0_jmax; ++j) { \
    for (int i=cctki0_imin; i<cctki0_imax; ++i) { \
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
                1) \

#define CCTK_ENDLOOP4(name) \
  CCTK_ENDLOOP4STR(name) \

#define CCTK_LOOP4STR(name, \
                      i,j,k,l, \
                      imin,jmin,kmin,lmin, \
                      imax,jmax,kmax,lmax, \
                      iash,jash,kash,lash, \
                      istr) \
  CCTK_LOOP4STR_NORMAL(name, \
                       i,j,k,l, \
                       cctki1_ni,cctki1_nj,cctki1_nk,cctki1_nl, \
                       0,0,0,0, \
                       imin,jmin,kmin,lmin, \
                       imax,jmax,kmax,lmax, \
                       iash,jash,kash,lash, \
                       istr) \

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
                         1) \

#define CCTK_ENDLOOP4_INTERIOR(name) \
  CCTK_ENDLOOP4STR_INTERIOR(name) \

#define CCTK_LOOP4STR_INTERIOR(name, cctkGH, \
                               i,j,k,l, \
                               iblo,jblo,kblo,lblo, \
                               ibhi,jbhi,kbhi,lbhi, \
                               istr) \
  do { \
    typedef int cctki2_loop4_interior_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
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
                  (istr)) { \

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
                           1) \

#define CCTK_ENDLOOP4_BOUNDARIES(name) \
  CCTK_ENDLOOP4STR_BOUNDARIES(name) \

#define CCTK_LOOP4STR_BOUNDARIES(name, cctkGH, \
                                 i,j,k,l, \
                                 ni,nj,nk,nl, \
                                 iblo,jblo,kblo,lblo, \
                                 ibhi,jbhi,kbhi,lbhi, \
                                 ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                 ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                                 istr) \
  do { \
    typedef int cctki2_loop4_boundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_BOUNDARIES can only be used in 4 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo),(lblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi),(lbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi),(lbboxlo), (lbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2],cctki2_cctkGH->cctk_lsh[3] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                              1) \

#define CCTK_ENDLOOP4_INTBOUNDARIES(name) \
  CCTK_ENDLOOP4STR_INTBOUNDARIES(name) \

#define CCTK_LOOP4STR_INTBOUNDARIES(name, cctkGH, \
                                    i,j,k,l, \
                                    ni,nj,nk,nl, \
                                    iblo,jblo,kblo,lblo, \
                                    ibhi,jbhi,kbhi,lbhi, \
                                    ibboxlo,jbboxlo,kbboxlo,lbboxlo, \
                                    ibboxhi,jbboxhi,kbboxhi,lbboxhi, \
                                    istr) \
  do { \
    typedef int cctki2_loop4_intboundaries_##name; \
    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH); \
    if (cctki2_cctkGH->cctk_dim != 4) { \
      _Pragma("omp critical") \
      CCTK_WARN(CCTK_WARN_ABORT, \
                "The macro CCTK_LOOP4_INTBOUNDARIES can only be used in 4 dimensions"); \
    } \
    int const cctki2_blo[] = { (iblo),(jblo),(kblo),(lblo) }; \
    int const cctki2_bhi[] = { (ibhi),(jbhi),(kbhi),(lbhi) }; \
    int const cctki2_bbox[] = { (ibboxlo), (ibboxhi),(jbboxlo), (jbboxhi),(kbboxlo), (kbboxhi),(lbboxlo), (lbboxhi) }; \
    int const cctki2_lsh[] = { cctki2_cctkGH->cctk_lsh[0],cctki2_cctkGH->cctk_lsh[1],cctki2_cctkGH->cctk_lsh[2],cctki2_cctkGH->cctk_lsh[3] }; \
    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr); \
    /* Loop over all faces, edges, and corners */ \
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
                             cctki2_istr1) { \

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
                    1) \

#define CCTK_ENDLOOP4_ALL(name) \
  CCTK_ENDLOOP4STR_ALL(name) \

#define CCTK_LOOP4STR_ALL(name, cctkGH, \
                          i,j,k,l, \
                          istr) \
  do { \
    typedef int cctki3_loop4_all_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                  (istr)) { \

#define CCTK_ENDLOOP4STR_ALL(name) \
    } CCTK_ENDLOOP4STR(name##_all); \
    typedef cctki3_loop4_all_##name cctki3_ensure_proper_nesting; \
  } while (0) \



/* LOOP_INT */

#define CCTK_LOOP4_INT(name, cctkGH, \
                       i,j,k,l) \
  CCTK_LOOP4STR_INT(name, (cctkGH), \
                    i,j,k,l, \
                    1) \

#define CCTK_ENDLOOP4_INT(name) \
  CCTK_ENDLOOP4STR_INT(name) \

#define CCTK_LOOP4STR_INT(name, cctkGH, \
                          i,j,k,l, \
                          istr) \
  do { \
    typedef int cctki3_loop4_int_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                           (istr)) { \

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
                    1) \

#define CCTK_ENDLOOP4_BND(name) \
  CCTK_ENDLOOP4STR_BND(name) \

#define CCTK_LOOP4STR_BND(name, cctkGH, \
                          i,j,k,l, \
                          ni,nj,nk,nl, \
                          istr) \
  do { \
    typedef int cctki3_loop4_bnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                             (istr)) { \

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
                        1) \

#define CCTK_ENDLOOP4_INTBND(name) \
  CCTK_ENDLOOP4STR_INTBND(name) \

#define CCTK_LOOP4STR_INTBND(name, cctkGH, \
                              i,j,k,l, \
                              ni,nj,nk,nl, \
                              istr) \
  do { \
    typedef int cctki3_loop4_intbnd_##name; \
    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH); \
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
                                (istr)) { \

#define CCTK_ENDLOOP4STR_INTBND(name) \
    } CCTK_ENDLOOP4STR_INTBOUNDARIES(name##_intbnd); \
    typedef cctki3_loop4_intbnd_##name cctki3_ensure_proper_nesting; \
  } while (0) \

#endif /* #ifdef CCODE */



#ifdef FCODE

/* LOOP */

#define CCTK_LOOP4_DECLARE(name) \
   && integer :: name/**/_imin,name/**/_jmin,name/**/_kmin,name/**/_lmin \
   && integer :: name/**/_imax,name/**/_jmax,name/**/_kmax,name/**/_lmax \
   && integer :: name/**/_istr \

#define CCTK_LOOP4_OMP_PRIVATE(name) \

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
             1) \

#define CCTK_LOOP4STR(name, \
                      i,j,k,l, \
                      imin,jmin,kmin,lmin, \
                      imax,jmax,kmax,lmax, \
                      iash,jash,kash,lash, \
                      istr) \
   && name/**/_imin = imin \
   && name/**/_jmin = jmin \
   && name/**/_kmin = kmin \
   && name/**/_lmin = lmin \
   && name/**/_imax = imax \
   && name/**/_jmax = jmax \
   && name/**/_kmax = kmax \
   && name/**/_lmax = lmax \
   && name/**/_istr = istr \
   && do l = name/**/_lmin, name/**/_lmax \
   && do k = name/**/_kmin, name/**/_kmax \
   && do j = name/**/_jmin, name/**/_jmax \
   && do i = name/**/_imin, name/**/_imax \

#define CCTK_ENDLOOP4(name) \
  CCTK_ENDLOOP4STR(name) \

#define CCTK_ENDLOOP4STR(name) \
   && end do \
   && end do \
   && end do \
   && end do \



/* LOOP_ALL */

#define CCTK_LOOP4_ALL_DECLARE(name) \
   CCTK_LOOP4_DECLARE(name) \

#define CCTK_LOOP4_ALL_OMP_PRIVATE(name) \
   CCTK_LOOP4_OMP_PRIVATE(name) \

#define CCTK_LOOP4_ALL(name, \
                       i,j,k,l) \
  CCTK_LOOP4STR_ALL(name, \
                 i,j,k,l, \
                 1) \

#define CCTK_LOOP4STR_ALL(name, \
                          i,j,k,l, \
                          istr) \
   CCTK_LOOP4STR(name, \
                 i,j,k,l, \
                 1,1,1,1, \
                 cctk_lsh(1),cctk_lsh(2),cctk_lsh(3),cctk_lsh(4), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3),cctk_ash(4), \
                 istr) \

#define CCTK_ENDLOOP4_ALL(name) \
  CCTK_ENDLOOP4STR_ALL(name) \

#define CCTK_ENDLOOP4STR_ALL(name) \
   CCTK_ENDLOOP4(name) \



/* LOOP_INTERIOR */

#define CCTK_LOOP4_INTERIOR_DECLARE(name) \
   CCTK_LOOP4_DECLARE(name) \

#define CCTK_LOOP4_INTERIOR_OMP_PRIVATE(name) \
   CCTK_LOOP4_OMP_PRIVATE(name) \

#define CCTK_LOOP4_INTERIOR(name, \
                            i,j,k,l, \
                            iblo,jblo,kblo,lblo, \
                            ibhi,jbhi,kbhi,lbhi) \
  CCTK_LOOP4STR_INTERIOR(name, \
                      i,j,k,l, \
                      iblo,jblo,kblo,lblo, \
                      ibhi,jbhi,kbhi,lbhi, \
                      1) \

#define CCTK_LOOP4STR_INTERIOR(name, \
                               i,j,k,l, \
                               iblo,jblo,kblo,lblo, \
                               ibhi,jbhi,kbhi,lbhi, \
                               istr) \
   CCTK_LOOP4STR(name, \
                 i,j,k,l, \
                 (iblo),(jblo),(kblo),(lblo), \
                 cctk_lsh(1)-(ibhi), \
                 cctk_lsh(2)-(jbhi), \
                 cctk_lsh(3)-(kbhi), \
                 cctk_lsh(4)-(lbhi), \
                 cctk_ash(1),cctk_ash(2),cctk_ash(3),cctk_ash(4), \
                 istr) \

#define CCTK_ENDLOOP4_INTERIOR(name) \
  CCTK_ENDLOOP4STR_INTERIOR(name) \

#define CCTK_ENDLOOP4STR_INTERIOR(name) \
   CCTK_ENDLOOP4(name) \



/* LOOP_BOUNDARIES */

#define CCTK_LOOP4_BOUNDARIES_DECLARE(name) \
   CCTK_LOOP4_DECLARE(name) \
   && integer :: lc_bmin(4), lc_bmax(4) \
   && integer :: lc_blo(4), lc_bhi(4) \
   && integer :: lc_istr \
   && integer :: lc_dir, lc_face \
   && integer :: lc_d \

#define CCTK_LOOP4_BOUNDARIES_OMP_PRIVATE(name) \
   CCTK_LOOP4_OMP_PRIVATE(name) \

#define CCTK_LOOP4_BOUNDARIES(name, \
                              i,j,k,l, \
                              iblo,jblo,kblo,lblo, \
                              ibhi,jbhi,kbhi,lbhi) \
  CCTK_LOOP4STR_BOUNDARIES(name, \
                        i,j,k,l, \
                        iblo,jblo,kblo,lblo, \
                        ibhi,jbhi,kbhi,lbhi, \
                        1) \

#define CCTK_LOOP4STR_BOUNDARIES(name, \
                                 i,j,k,l, \
                                 iblo,jblo,kblo,lblo, \
                                 ibhi,jbhi,kbhi,lbhi, \
                                 istr) \
   && lc_blo = (/ iblo,jblo,kblo,lblo /) \
   && lc_bhi = (/ ibhi,jbhi,kbhi,lbhi /) \
   && lc_istr = istr \
   && do lc_dir=1,4 \
   &&    do lc_face=1,2 \
   &&       do lc_d=1,4 \
   &&          lc_bmin(lc_d) = 1 \
   &&          lc_bmax(lc_d) = cctk_lsh(lc_d) \
   &&          if (lc_d<lc_dir) then \
   &&             lc_bmin(lc_d) = lc_bmin(lc_d)+lc_blo(lc_d) \
   &&             lc_bmax(lc_d) = lc_bmax(lc_d)-lc_bhi(lc_d) \
   &&          end if \
   &&       end do \
   &&       if (lc_face==1) then \
   &&          lc_bmax(lc_dir) = lc_bmin(lc_dir)+lc_blo(lc_dir) \
   &&       else \
   &&          lc_bmin(lc_dir) = lc_bmax(lc_dir)-lc_bhi(lc_dir) \
   &&       end if \
            CCTK_LOOP4STR(name, \
                          i,j,k,l, \
                          lc_bmin(1),lc_bmin(2),lc_bmin(3),lc_bmin(4), \
                          lc_bmax(1),lc_bmax(2),lc_bmax(3),lc_bmax(4), \
                          cctk_ash(1),cctk_ash(2),cctk_ash(3),cctk_ash(4), \
                          lc_istr) \

#define CCTK_ENDLOOP4_BOUNDARIES(name) \
  CCTK_ENDLOOP4STR_BOUNDARIES(name) \

#define CCTK_ENDLOOP4STR_BOUNDARIES(name) \
            CCTK_ENDLOOP4(name) \
   &&    end do /* face */ \
   && end do /* dir */ \

#endif /* #ifdef FCODE */



#endif /* #ifndef _CCTK_LOOP_H_ */
