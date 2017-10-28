#ifndef PRESYNC_H
#define PRESYNC_H

typedef void (*boundary_function)(
  const cGH *cctkGH,
  int num_vars,
  int *var_indices,
  int *faces,
  int *widths,
  int *table_handles);

#ifdef __cplusplus
extern "C" {
#endif

void RegisterPhysicalBC(
    const cGH *cctkGH,
    boundary_function func,
    const char *bc_name,
    int before);

void RegisterSymmetryBC(
    const cGH *cctkGH,
    boundary_function func,
    const char *bc_name,
    int before);

#ifdef __cplusplus
}
#endif

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
#define WH_BOUNDARY            0x2 /* Describes B+G cells */
#define WH_GHOSTS              0x1
#define WH_NOWHERE             0x0
#endif
