 /*@@
   @header    cctk_PreSync.h
   @date      Fri Feb 3 12:36:00 2017 -0600
   @author    Steven R. Brandt, Roland Haas
   @desc

   @enddesc
   @version $Header$
 @@*/

#ifndef __CCTK_PRESYNC_H_
#define __CCTK_PRESYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef CCTK_INT (*boundary_function)(
  const void *cctkGH,
  const int num_vars,
  const int *var_indices,
  const int *faces,
  const int *widths,
  const int *table_handles);

int CCTK_HasAccess(const cGH *cctkGH, int var_index);

#ifdef __cplusplus
}
#endif

#endif /*__CCTK_PRESYNC_H_ */
