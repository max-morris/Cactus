#ifndef PRESYNC_H
#define PRESYNC_H

typedef CCTK_INT (*boundary_function)(
  const void *cctkGH,
  const int num_vars,
  const int *var_indices,
  const int *faces,
  const int *widths,
  const int *table_handles);

// The upper left-hand corner of a 2-D simulation.
// +-----+-----+-----+
// |     |     |     | 
// |  B  |  B  | B+G | 
// |     |     |     | 
// +-----+-----+-----+
// |     |     |     | 
// |  B  |  I  |  G  | 
// |     |     |     | 
// +-----+-----+-----+
// |     |     |     | 
// | B+G |  G  |  G  | 
// |     |     |     | 
// +-----+-----+-----+
//  B  = Boundary 
//  I  = Interior
//  G  = Ghost
// B+G = Boundary and Ghost
//
// Read and Write directives in schedule.ccl apply to:
// Interior, Everywhere, or Invalidate.
//
// Boundary routines are registered with
// to either update B+G cells or not. If it
// updates B+G cells, it runs after synchronization.
// If it doesn't update B+G cells, it runs before.

#define WH_EVERYWHERE          0x7
#define WH_INTERIOR            0x4
#define WH_BOUNDARY            0x2 
#define WH_EXTERIOR            0x6
#define WH_GHOSTS              0x1
#define WH_NOWHERE             0x0
#endif
