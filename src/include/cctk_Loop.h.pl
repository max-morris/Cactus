#! /usr/bin/perl -w

# This perl script generates the include file "cctk_Loop.h".



# API:

# There is a basic looping macro (LOOP) which takes the array size and
# loop bounds as arguments. This macro can loop over all arrays, both
# grid functions and grid arrays. A second version of this macro
# (LOOP_NORMAL) also provides information about the normal to the
# boundary and distance to the boundary to the loop kernel.

# There are three specialised versions that take instead cctkGH and
# the boundary sizes as arguments (LOOP_INTERIOR, LOOP_BOUNDARIES, and
# LOOP_INTBOUNDARIES). These macros are intended for grid functions
# only, and loop over the respective subsets of points, either only
# interior points, or all physical boundaries, or the "interior"
# physical boundaries (excluding edges and corners that are also
# ghosts or symmetry points).

# Note: Edges and corners can have several boundary types at once,
# e.g. can be both a physical and a symmetry point. LOOP_BOUNDARY and
# LOOP_INTBOUNDARY treats these different: LOOP_BOUNDARY loops over
# all points that are physical boundaries (independent of whether they
# also are symmetry or ghost boundaries), while LOOP_INTBOUNARY loops
# over those points that are only physical boundaries (and excludes
# any points that belongs to a symmetry or ghost boundary).
# LOOP_BOUNDARY does not require applying a symmetry condition or
# synchronisation afterwards (but does not allow taking tangential
# derivatives); LOOP_INTBOUNDARY allows taking tangential derivatives
# (but requires applying symmetry boundaries and synchronising
# afterwards).

# There are four specialised versions that take instead a cctkGH
# (LOOP_ALL, LOOP_INT, LOOP_BND, and LOOP_INTBND). These macros are
# intended for grid functions only and obtain the boundary sizes from
# CoordBase via an aliased function. They loop over the respective
# subsets of points, either all points, or all interior points, or all
# physical boundaries, or the "interior" physical boundaries (see
# above).

# Each macro exists in two versions, a regular and a strided one
# (STR). The strided macros are intended for vectorised code, and the
# stride should be the vector size.

# Each macro exists for variable numbers of dimensions, currently 1
# through 4.



# Internals:

# Since these macros call each other, it is important that the local
# variables they declare do not conflict. Therefore, we denote four
# "levels" of macros, corresponding to the description above:
#    level 0: LOOP_NORMAL
#    level 1: LOOP
#    level 2: LOOP_INTERIOR, LOOP_BOUNDARIES, LOOP_INTBOUNDARIES
#    level 3: LOOP_ALL, LOOP_INT, LOOP_BND, LOOP_INTBND
# Each level may only call macros of a lower level. All local
# variables carry a prefix "cctki[LEVEL]_" to ensure that there are no
# naming conflicts.



use strict;



# Global variable: number of dimensions (1 to 4)
our $dim;



# Expand [DIM] to the (global) number of dimensions
sub expand ($)
{
    my ($str) = @_;
    my $dim2 = 2*$dim;
    $str =~ s{\[DIM\]}{$dim}g;
    $str =~ s{\[2\*DIM\]}{$dim2}g;
    return $str;
}

# Add a newline
sub nl ($)
{
    my ($str) = @_;
    return "$str\n";
}

# Add a backslash and newline
sub bsnl ($)
{
    my ($str) = @_;
    return "$str \\\n";
}

# Repeat a string once for each dimension, expanding 'I' and 'C' to a
# different number (0,1,2,3) and character (i,j,k,l) each time
sub rpt ($)
{
    my ($str) = @_;
    my @ret;
    for my $d (1..$dim) {
        my $int = $d-1;
        my $int1 = $int+1;
        my $int2 = 2*$int;
        my $int21 = 2*$int+1;
        my $char = ('i','j','k','l')[$d-1];
        my $tmp = $str;
        $tmp =~ s{\[I\]}{$int}g;
        $tmp =~ s{\[I\+1\]}{$int1}g;
        $tmp =~ s{\[2\*I\]}{$int2}g;
        $tmp =~ s{\[2\*I\+1\]}{$int21}g;
        $tmp =~ s{\[C\]}{$char}g;
        push @ret, $tmp;
    }
    return @ret;
}

# Terminate a list (intersperse a string, and add the string at the
# end as well)
sub term ($@)
{
    my ($str, @txt) = @_;
    return (join $str, @txt) . $str;
}

# Separate a list (intersperse a string, but not at the end)
sub sep ($@)
{
    my ($str, @txt) = @_;
    return (join $str, @txt);
}

# Repeat and separate with a comma
sub crpt ($)
{
    my ($str) = @_;
    return (sep ',', rpt $str);
}

# The first argument is a list reference to force a list context for
# wherever this argument is evaluated
sub bsnlsep ($;$$)
{
    my ($txt, $str1, $str2) = @_;
    $str1 = '' if ! defined $str1;
    $str2 = $str1 if ! defined $str2;
    my @ret;
    for my $t (@$txt[0..$#$txt-1]) {
        push @ret, "$t$str1";
    }
    if ($#$txt >= 0) {
        push @ret, "$$txt[$#$txt]$str2";
    }
    return map {bsnl $_} @ret;
}



my @lines;

# Header
push @lines, (
    (nl '#ifndef _CCTK_LOOP_H_'),
    (nl '#define _CCTK_LOOP_H_'),
    (nl ''),
    (nl '/* WARNING: This file is auto-generated. Do not edit. */'),
    (nl '/* Edit cctk_Loop.h.pl instead, and then re-generate this file via */'),
    (nl '/*    perl cctk_Loop.h.pl.pl */'),
    (nl '/* Documentation can also be found in "cctk_Loop.h.pl". */'),
    (nl ''),
    (nl '#ifdef CCODE'),
    (nl '#  include <cctk_Config.h>'),
    (nl '#  include <cctk_WarnLevel.h>'),
    (nl '#  include <cGH.h>'),
    (nl '#  include <assert.h>'),
    (nl '#endif /* #ifdef CCODE */'),
    );



for $dim (1,2,3,4) {

    push @lines, map {expand $_} (
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* [DIM]D */'),
        (nl ''),
        (nl '#ifdef CCODE'),
        (nl ''),
        (nl '/* LOOP */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_NORMAL(name,'),
        (bsnl '                          '.(crpt '[C]').','),
        (bsnl '                          '.(crpt 'n[C]').','),
        (bsnl '                          '.(crpt '[C]dir').','),
        (bsnl '                          '.(crpt '[C]min').','),
        (bsnl '                          '.(crpt '[C]max').','),
        (bsnl '                          '.(crpt '[C]ash').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_NORMAL(name,'),
        (bsnl '                       '.(crpt '[C]').','),
        (bsnl '                       '.(crpt 'n[C]').','),
        (bsnl '                       '.(crpt '([C]dir)').','),
        (bsnl '                       '.(crpt '([C]min)').','),
        (bsnl '                       '.(crpt '([C]max)').','),
        (bsnl '                       '.(crpt '([C]ash)').','),
        (bsnl '                       1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_NORMAL(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_NORMAL(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_NORMAL(name,'),
        (bsnl '                             '.(crpt '[C]').','),
        (bsnl '                             '.(crpt 'n[C]').','),
        (bsnl '                             '.(crpt '[C]dir').','),
        (bsnl '                             '.(crpt '[C]min').','),
        (bsnl '                             '.(crpt '[C]max').','),
        (bsnl '                             '.(crpt '[C]ash').','),
        (bsnl '                             istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki0_loop[DIM]_normal_##name;'),
        (rpt (bsnl '    int const cctki0_[C]dir = ([C]dir);')),
        (rpt (bsnl '    int const cctki0_[C]min = ([C]min);')),
        (rpt (bsnl '    int const cctki0_[C]max = ([C]max);')),
        (bsnl '    int const cctki0_istr CCTK_ATTRIBUTE_UNUSED = (istr);'),
        (bsnl '    assert(cctki0_istr == 1);'),
        (bsnl '    _Pragma("omp for")'), # collapse([DIM])
        (reverse (rpt (bsnl '    for (int [C]=cctki0_[C]min; [C]<cctki0_[C]max; ++[C]) {'))),
        (rpt (bsnl '      int const n[C] CCTK_ATTRIBUTE_UNUSED = cctki0_[C]dir<0 ? [C]+1 : cctki0_[C]dir==0 ? 0 : cctki0_[C]max-[C];')),
        (bsnl '      {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_NORMAL(name)'),
        (bsnl '      }'),
        (rpt (bsnl '    }')),
        (bsnl '    typedef cctki0_loop[DIM]_normal_##name cctki0_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM](name,'),
        (bsnl '                   '.(crpt '[C]').','),
        (bsnl '                   '.(crpt '[C]min').','),
        (bsnl '                   '.(crpt '[C]max').','),
        (bsnl '                   '.(crpt '[C]ash').')'),
        (bsnl '  CCTK_LOOP[DIM]STR(name,'),
        (bsnl '                '.(crpt '[C]').','),
        (bsnl '                '.(crpt '([C]min)').','),
        (bsnl '                '.(crpt '([C]max)').','),
        (bsnl '                '.(crpt '([C]ash)').','),
        (bsnl '                1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM](name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR(name,'),
        (bsnl '                      '.(crpt '[C]').','),
        (bsnl '                      '.(crpt '[C]min').','),
        (bsnl '                      '.(crpt '[C]max').','),
        (bsnl '                      '.(crpt '[C]ash').','),
        (bsnl '                      istr)'),
        (bsnl '  CCTK_LOOP[DIM]STR_NORMAL(name,'),
        (bsnl '                       '.(crpt '[C]').','),
        (bsnl '                       '.(crpt 'cctki1_n[C]').','),
        (bsnl '                       '.(crpt '0').','),
        (bsnl '                       '.(crpt '[C]min').','),
        (bsnl '                       '.(crpt '[C]max').','),
        (bsnl '                       '.(crpt '[C]ash').','),
        (bsnl '                       istr)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_NORMAL(name)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_INTERIOR */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INTERIOR(name, cctkGH,'),
        (bsnl '                            '.(crpt '[C]').','),
        (bsnl '                            '.(crpt '[C]blo').','),
        (bsnl '                            '.(crpt '[C]bhi').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_INTERIOR(name, (cctkGH),'),
        (bsnl '                         '.(crpt '[C]').','),
        (bsnl '                         '.(crpt '([C]blo)').','),
        (bsnl '                         '.(crpt '([C]bhi)').','),
        (bsnl '                         1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_INTERIOR(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_INTERIOR(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_INTERIOR(name, cctkGH,'),
        (bsnl '                               '.(crpt '[C]').','),
        (bsnl '                               '.(crpt '[C]blo').','),
        (bsnl '                               '.(crpt '[C]bhi').','),
        (bsnl '                               istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki2_loop[DIM]_interior_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki2_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_INTERIOR can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    CCTK_LOOP[DIM]STR(name##_interior,'),
        (bsnl '                  '.(crpt '[C]').','),
        (bsnl '                  '.(crpt '([C]blo)').','),
        (rpt (bsnl '                  cctki2_cctkGH->cctk_lsh[[I]]-([C]bhi),')),
        (rpt (bsnl '                  cctki2_cctkGH->cctk_ash[[I]],')),
        (bsnl '                  (istr)) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_INTERIOR(name)'),
        (bsnl '    } CCTK_ENDLOOP[DIM]STR(name##_interior);'),
        (bsnl '    typedef cctki2_loop[DIM]_interior_##name cctki2_ensure_proper_nesting;'),
        (bsnl '  } while(0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_BOUNDARIES */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_BOUNDARIES(name, cctkGH,'),
        (bsnl '                              '.(crpt '[C]').','),
        (bsnl '                              '.(crpt 'n[C]').','),
        (bsnl '                              '.(crpt '[C]blo').','),
        (bsnl '                              '.(crpt '[C]bhi').','),
        (bsnl '                              '.(crpt '[C]bboxlo').','),
        (bsnl '                              '.(crpt '[C]bboxhi').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_BOUNDARIES(name, (cctkGH),'),
        (bsnl '                           '.(crpt '[C]').','),
        (bsnl '                           '.(crpt 'n[C]').','),
        (bsnl '                           '.(crpt '([C]blo)').','),
        (bsnl '                           '.(crpt '([C]bhi)').','),
        (bsnl '                           '.(crpt '([C]bboxlo)').','),
        (bsnl '                           '.(crpt '([C]bboxhi)').','),
        (bsnl '                           1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_BOUNDARIES(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_BOUNDARIES(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_BOUNDARIES(name, cctkGH,'),
        (bsnl '                                 '.(crpt '[C]').','),
        (bsnl '                                 '.(crpt 'n[C]').','),
        (bsnl '                                 '.(crpt '[C]blo').','),
        (bsnl '                                 '.(crpt '[C]bhi').','),
        (bsnl '                                 '.(crpt '[C]bboxlo').','),
        (bsnl '                                 '.(crpt '[C]bboxhi').','),
        (bsnl '                                 istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki2_loop[DIM]_boundaries_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki2_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_BOUNDARIES can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    int const cctki2_blo[] = { '.(crpt '([C]blo)').' };'),
        (bsnl '    int const cctki2_bhi[] = { '.(crpt '([C]bhi)').' };'),
        (bsnl '    int const cctki2_bbox[] = { '.(crpt '([C]bboxlo), ([C]bboxhi)').' };'),
        (bsnl '    int const cctki2_lsh[] = { '.(crpt 'cctki2_cctkGH->cctk_lsh[[I]]').' };'),
        (bsnl '    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr);'),
        (bsnl '    /* Loop over all faces, edges, and corners */'),
        (reverse (rpt (bsnl '    for (int cctki2_[C]dir=-1; cctki2_[C]dir<=+1; ++cctki2_[C]dir) {'))),
        (bsnl '      int cctki2_any_bbox ='),
        (bsnlsep [rpt '        (cctki2_[C]dir==-1 ? cctki2_bbox[[2*I]] : 0) || (cctki2_[C]dir==+1 ? cctki2_bbox[[2*I+1]] : 0)'], '||', ';'),
        (bsnl '      if (cctki2_any_bbox) {'),
        (bsnl '        int const cctki2_bmin[] = {'),
        (rpt (bsnl '          cctki2_[C]dir==-1 ? 0 : cctki2_[C]dir==0 ? cctki2_blo[[I]] : cctki2_lsh[[I]] - cctki2_bhi[[I]],')),
        (bsnl '        };'),
        (bsnl '        int const cctki2_bmax[] = {'),
        (rpt (bsnl '          cctki2_[C]dir==-1 ? cctki2_blo[[I]] : cctki2_[C]dir==0 ? cctki2_lsh[[I]] - cctki2_bhi[[I]] : cctki2_lsh[[I]],')),
        (bsnl '        };'),
        (bsnl '        CCTK_LOOP[DIM]STR_NORMAL(name##_boundaries,'),
        (bsnl '                             '.(crpt '[C]').','),
        (bsnl '                             '.(crpt 'n[C]').','),
        (bsnl '                             '.(crpt 'cctki2_[C]dir').','),
        (bsnl '                             '.(crpt 'cctki2_bmin[[I]]').','),
        (bsnl '                             '.(crpt 'cctki2_bmax[[I]]').','),
        (rpt (bsnl '                             cctki2_cctkGH->cctk_ash[[I]],')),
        (bsnl '                             cctki2_istr1) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_BOUNDARIES(name)'),
        (bsnl '        } CCTK_ENDLOOP[DIM]STR_NORMAL(name##_boundaries);'),
        (bsnl '      } /* if bbox */'),
        (reverse (rpt (bsnl '    } /* for dir */'))),
        (bsnl '    typedef cctki2_loop[DIM]_boundaries_##name cctki2_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_INTBOUNDARIES */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INTBOUNDARIES(name, cctkGH,'),
        (bsnl '                                 '.(crpt '[C]').','),
        (bsnl '                                 '.(crpt 'n[C]').','),
        (bsnl '                                 '.(crpt '[C]blo').','),
        (bsnl '                                 '.(crpt '[C]bhi').','),
        (bsnl '                                 '.(crpt '[C]bboxlo').','),
        (bsnl '                                 '.(crpt '[C]bboxhi').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_INTBOUNDARIES(name, (cctkGH),'),
        (bsnl '                              '.(crpt '[C]').','),
        (bsnl '                              '.(crpt 'n[C]').','),
        (bsnl '                              '.(crpt '([C]blo)').','),
        (bsnl '                              '.(crpt '([C]bhi)').','),
        (bsnl '                              '.(crpt '([C]bboxlo)').','),
        (bsnl '                              '.(crpt '([C]bboxhi)').','),
        (bsnl '                              1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_INTBOUNDARIES(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_INTBOUNDARIES(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_INTBOUNDARIES(name, cctkGH,'),
        (bsnl '                                    '.(crpt '[C]').','),
        (bsnl '                                    '.(crpt 'n[C]').','),
        (bsnl '                                    '.(crpt '[C]blo').','),
        (bsnl '                                    '.(crpt '[C]bhi').','),
        (bsnl '                                    '.(crpt '[C]bboxlo').','),
        (bsnl '                                    '.(crpt '[C]bboxhi').','),
        (bsnl '                                    istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki2_loop[DIM]_intboundaries_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki2_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki2_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_INTBOUNDARIES can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    int const cctki2_blo[] = { '.(crpt '([C]blo)').' };'),
        (bsnl '    int const cctki2_bhi[] = { '.(crpt '([C]bhi)').' };'),
        (bsnl '    int const cctki2_bbox[] = { '.(crpt '([C]bboxlo), ([C]bboxhi)').' };'),
        (bsnl '    int const cctki2_lsh[] = { '.(crpt 'cctki2_cctkGH->cctk_lsh[[I]]').' };'),
        (bsnl '    int const cctki2_istr1 CCTK_ATTRIBUTE_UNUSED = (istr);'),
        (bsnl '    /* Loop over all faces, edges, and corners */'),
        (reverse (rpt (bsnl '    for (int cctki2_[C]dir=-1; cctki2_[C]dir<=+1; ++cctki2_[C]dir) {'))),
        (bsnl '      int cctki2_any_bbox ='),
        (bsnlsep [rpt '        (cctki2_[C]dir==-1 ? cctki2_bbox[[2*I]] : 0) || (cctki2_[C]dir==+1 ? cctki2_bbox[[2*I+1]] : 0)'], '||', ';'),
        (bsnl '      int cctki2_all_bbox ='),
        (bsnlsep [rpt '        (cctki2_[C]dir==-1 ? cctki2_bbox[[2*I]] : 1) && (cctki2_[C]dir==+1 ? cctki2_bbox[[2*I+1]] : 1)'], '&&', ';'),
        (bsnl '      if (cctki2_all_bbox && cctki2_any_bbox) {'),
        (bsnl '        int const cctki2_bmin[] = {'),
        (rpt (bsnl '          cctki2_[C]dir==-1 ? 0 : cctki2_[C]dir==0 ? cctki2_blo[[I]] : cctki2_lsh[[I]] - cctki2_bhi[[I]],')),
        (bsnl '        };'),
        (bsnl '        int const cctki2_bmax[] = {'),
        (rpt (bsnl '          cctki2_[C]dir==-1 ? cctki2_blo[[I]] : cctki2_[C]dir==0 ? cctki2_lsh[[I]] - cctki2_bhi[[I]] : cctki2_lsh[[I]],')),
        (bsnl '        };'),
        (bsnl '        CCTK_LOOP[DIM]STR_NORMAL(name##_intboundaries,'),
        (bsnl '                             '.(crpt '[C]').','),
        (bsnl '                             '.(crpt 'n[C]').','),
        (bsnl '                             '.(crpt 'cctki2_[C]dir').','),
        (bsnl '                             '.(crpt 'cctki2_bmin[[I]]').','),
        (bsnl '                             '.(crpt 'cctki2_bmax[[I]]').','),
        (rpt (bsnl '                             cctki2_cctkGH->cctk_ash[[I]],')),
        (bsnl '                             cctki2_istr1) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_INTBOUNDARIES(name)'),
        (bsnl '        } CCTK_ENDLOOP[DIM]STR_NORMAL(name##_intboundaries);'),
        (bsnl '      } /* if bbox */'),
        (reverse (rpt (bsnl '    } /* for dir */'))),
        (bsnl '    typedef cctki2_loop[DIM]_intboundaries_##name cctki2_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_ALL */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_ALL(name, cctkGH,'),
        (bsnl '                       '.(crpt '[C]').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_ALL(name, (cctkGH),'),
        (bsnl '                    '.(crpt '[C]').','),
        (bsnl '                    1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_ALL(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_ALL(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_ALL(name, cctkGH,'),
        (bsnl '                          '.(crpt '[C]').','),
        (bsnl '                          istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki3_loop[DIM]_all_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki3_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_ALL can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    CCTK_LOOP[DIM]STR(name##_all,'),
        (bsnl '                  '.(crpt '[C]').','),
        (bsnl '                  '.(crpt '0').','),
        (rpt (bsnl '                  cctki3_cctkGH->cctk_lsh[[I]],')),
        (rpt (bsnl '                  cctki3_cctkGH->cctk_ash[[I]],')),
        (bsnl '                  (istr)) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_ALL(name)'),
        (bsnl '    } CCTK_ENDLOOP[DIM]STR(name##_all);'),
        (bsnl '    typedef cctki3_loop[DIM]_all_##name cctki3_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_INT */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INT(name, cctkGH,'),
        (bsnl '                       '.(crpt '[C]').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_INT(name, (cctkGH),'),
        (bsnl '                    '.(crpt '[C]').','),
        (bsnl '                    1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_INT(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_INT(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_INT(name, cctkGH,'),
        (bsnl '                          '.(crpt '[C]').','),
        (bsnl '                          istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki3_loop[DIM]_int_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki3_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_INT can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    CCTK_INT cctki3_bndsize    [[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_ghostbnd[[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_symbnd  [[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_physbnd [[2*DIM]];'),
        (bsnl '    _Pragma("omp single copyprivate(cctki3_bndsize)")'),
        (bsnl '    GetBoundarySizesAndTypes'),
        (bsnl '      (cctki3_cctkGH, [2*DIM], cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd);'),
        (bsnl '    CCTK_LOOP[DIM]STR_INTERIOR(name##_int,'),
        (bsnl '                           cctki3_cctkGH,'),
        (bsnl '                           '.(crpt '[C]').','),
        (bsnl '                           '.(crpt 'cctki3_bndsize[[2*I]]').','),
        (bsnl '                           '.(crpt 'cctki3_bndsize[[2*I+1]]').','),
        (bsnl '                           (istr)) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_INT(name)'),
        (bsnl '    } CCTK_ENDLOOP[DIM]STR_INTERIOR(name##_int);'),
        (bsnl '    typedef cctki3_loop[DIM]_int_##name cctki3_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_BND */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_BND(name, cctkGH,'),
        (bsnl '                       '.(crpt '[C]').','),
        (bsnl '                       '.(crpt 'n[C]').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_BND(name, (cctkGH),'),
        (bsnl '                    '.(crpt '[C]').','),
        (bsnl '                    '.(crpt 'n[C]').','),
        (bsnl '                    1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_BND(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_BND(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_BND(name, cctkGH,'),
        (bsnl '                          '.(crpt '[C]').','),
        (bsnl '                          '.(crpt 'n[C]').','),
        (bsnl '                          istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki3_loop[DIM]_bnd_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki3_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_BND can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    CCTK_INT cctki3_bndsize    [[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_ghostbnd[[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_symbnd  [[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_physbnd [[2*DIM]];'),
        (bsnl '    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)")'),
        (bsnl '    GetBoundarySizesAndTypes'),
        (bsnl '      (cctki3_cctkGH, [2*DIM], cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd);'),
        (bsnl '    CCTK_LOOP[DIM]STR_BOUNDARIES(name##_bnd,'),
        (bsnl '                             cctki3_cctkGH,'),
        (bsnl '                             '.(crpt '[C]').','),
        (bsnl '                             '.(crpt 'n[C]').','),
        (bsnl '                             '.(crpt 'cctki3_bndsize[[2*I]]').','),
        (bsnl '                             '.(crpt 'cctki3_bndsize[[2*I+1]]').','),
        (bsnl '                             '.(crpt 'cctki3_is_physbnd[[2*I]]').','),
        (bsnl '                             '.(crpt 'cctki3_is_physbnd[[2*I+1]]').','),
        (bsnl '                             (istr)) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_BND(name)'),
        (bsnl '    } CCTK_ENDLOOP[DIM]STR_BOUNDARIES(name##_bnd);'),
        (bsnl '    typedef cctki3_loop[DIM]_bnd_##name cctki3_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_INTBND */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INTBND(name, cctkGH,'),
        (bsnl '                           '.(crpt '[C]').','),
        (bsnl '                           '.(crpt 'n[C]').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_INTBND(name, (cctkGH),'),
        (bsnl '                        '.(crpt '[C]').','),
        (bsnl '                        '.(crpt 'n[C]').','),
        (bsnl '                        1)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_INTBND(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_INTBND(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_INTBND(name, cctkGH,'),
        (bsnl '                              '.(crpt '[C]').','),
        (bsnl '                              '.(crpt 'n[C]').','),
        (bsnl '                              istr)'),
        (bsnl '  do {'),
        (bsnl '    typedef int cctki3_loop[DIM]_intbnd_##name;'),
        (bsnl '    cGH const *CCTK_RESTRICT const cctki3_cctkGH = (cctkGH);'),
        (bsnl '    if (cctki3_cctkGH->cctk_dim != [DIM]) {'),
        (bsnl '      _Pragma("omp critical")'),
        (bsnl '      CCTK_WARN(CCTK_WARN_ABORT,'),
        (bsnl '                "The macro CCTK_LOOP[DIM]_INTBND can only be used in [DIM] dimensions");'),
        (bsnl '    }'),
        (bsnl '    CCTK_INT cctki3_bndsize    [[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_ghostbnd[[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_symbnd  [[2*DIM]];'),
        (bsnl '    CCTK_INT cctki3_is_physbnd [[2*DIM]];'),
        (bsnl '    _Pragma("omp single copyprivate(cctki3_bndsize, cctki3_is_physbnd)")'),
        (bsnl '    GetBoundarySizesAndTypes'),
        (bsnl '      (cctki3_cctkGH, [2*DIM], cctki3_bndsize, cctki3_is_ghostbnd, cctki3_is_symbnd, cctki3_is_physbnd);'),
        (bsnl '    CCTK_LOOP[DIM]STR_INTBOUNDARIES(name##_intbnd,'),
        (bsnl '                                cctki3_cctkGH,'),
        (bsnl '                                '.(crpt '[C]').','),
        (bsnl '                                '.(crpt 'n[C]').','),
        (bsnl '                                '.(crpt 'cctki3_bndsize[[2*I]]').','),
        (bsnl '                                '.(crpt 'cctki3_bndsize[[2*I+1]]').','),
        (bsnl '                                '.(crpt 'cctki3_is_physbnd[[2*I]]').','),
        (bsnl '                                '.(crpt 'cctki3_is_physbnd[[2*I+1]]').','),
        (bsnl '                                (istr)) {'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_INTBND(name)'),
        (bsnl '    } CCTK_ENDLOOP[DIM]STR_INTBOUNDARIES(name##_intbnd);'),
        (bsnl '    typedef cctki3_loop[DIM]_intbnd_##name cctki3_ensure_proper_nesting;'),
        (bsnl '  } while (0)'),
        (nl ''),
        (nl '#endif /* #ifdef CCODE */'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '#ifdef FCODE'),
        (nl ''),
        (nl '/* LOOP */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_DECLARE(name)'),
        (bsnl '   && integer :: '.(crpt 'name/**/_[C]min')),
        (bsnl '   && integer :: '.(crpt 'name/**/_[C]max')),
        (bsnl '   && integer :: name/**/_istr'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_OMP_PRIVATE(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM](name,'),
        (bsnl '                   '.(crpt '[C]').','),
        (bsnl '                   '.(crpt '[C]min').','),
        (bsnl '                   '.(crpt '[C]max').','),
        (bsnl '                   '.(crpt '[C]ash').')'),
        (bsnl '  CCTK_LOOP[DIM]STR(name,'),
        (bsnl '             '.(crpt '[C]').','),
        (bsnl '             '.(crpt '[C]min').','),
        (bsnl '             '.(crpt '[C]max').','),
        (bsnl '             '.(crpt '[C]ash').','),
        (bsnl '             1)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR(name,'),
        (bsnl '                      '.(crpt '[C]').','),
        (bsnl '                      '.(crpt '[C]min').','),
        (bsnl '                      '.(crpt '[C]max').','),
        (bsnl '                      '.(crpt '[C]ash').','),
        (bsnl '                      istr)'),
        (rpt (bsnl '   && name/**/_[C]min = [C]min')),
        (rpt (bsnl '   && name/**/_[C]max = [C]max')),
        (bsnl '   && name/**/_istr = istr'),
        (reverse (rpt (bsnl '   && do [C] = name/**/_[C]min, name/**/_[C]max'))),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM](name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR(name)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR(name)'),
        (rpt (bsnl '   && end do')),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_ALL */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_ALL_DECLARE(name)'),
        (bsnl '   CCTK_LOOP[DIM]_DECLARE(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_ALL_OMP_PRIVATE(name)'),
        (bsnl '   CCTK_LOOP[DIM]_OMP_PRIVATE(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_ALL(name,'),
        (bsnl '                       '.(crpt '[C]').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_ALL(name,'),
        (bsnl '                 '.(crpt '[C]').','),
        (bsnl '                 1)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_ALL(name,'),
        (bsnl '                          '.(crpt '[C]').','),
        (bsnl '                          istr)'),
        (bsnl '   CCTK_LOOP[DIM]STR(name,'),
        (bsnl '                 '.(crpt '[C]').','),
        (bsnl '                 '.(crpt '1').','),
        (bsnl '                 '.(crpt 'cctk_lsh([I+1])').','),
        (bsnl '                 '.(crpt 'cctk_ash([I+1])').','),
        (bsnl '                 istr)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_ALL(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_ALL(name)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_ALL(name)'),
        (bsnl '   CCTK_ENDLOOP[DIM](name)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_INTERIOR */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INTERIOR_DECLARE(name)'),
        (bsnl '   CCTK_LOOP[DIM]_DECLARE(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INTERIOR_OMP_PRIVATE(name)'),
        (bsnl '   CCTK_LOOP[DIM]_OMP_PRIVATE(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_INTERIOR(name,'),
        (bsnl '                            '.(crpt '[C]').','),
        (bsnl '                            '.(crpt '[C]blo').','),
        (bsnl '                            '.(crpt '[C]bhi').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_INTERIOR(name,'),
        (bsnl '                      '.(crpt '[C]').','),
        (bsnl '                      '.(crpt '[C]blo').','),
        (bsnl '                      '.(crpt '[C]bhi').','),
        (bsnl '                      1)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_INTERIOR(name,'),
        (bsnl '                               '.(crpt '[C]').','),
        (bsnl '                               '.(crpt '[C]blo').','),
        (bsnl '                               '.(crpt '[C]bhi').','),
        (bsnl '                               istr)'),
        (bsnl '   CCTK_LOOP[DIM]STR(name,'),
        (bsnl '                 '.(crpt '[C]').','),
        (bsnl '                 '.(crpt '([C]blo)').','),
        (rpt (bsnl '                 cctk_lsh([I+1])-([C]bhi),')),
        (bsnl '                 '.(crpt 'cctk_ash([I+1])').','),
        (bsnl '                 istr)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_INTERIOR(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_INTERIOR(name)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_INTERIOR(name)'),
        (bsnl '   CCTK_ENDLOOP[DIM](name)'),
        (nl ''),
        (nl ''),
        (nl ''),
        (nl '/* LOOP_BOUNDARIES */'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_BOUNDARIES_DECLARE(name)'),
        (bsnl '   CCTK_LOOP[DIM]_DECLARE(name)'),
        (bsnl '   && integer :: lc_bmin([DIM]), lc_bmax([DIM])'),
        (bsnl '   && integer :: lc_blo([DIM]), lc_bhi([DIM])'),
        (bsnl '   && integer :: lc_istr'),
        (bsnl '   && integer :: lc_dir, lc_face'),
        (bsnl '   && integer :: lc_d'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_BOUNDARIES_OMP_PRIVATE(name)'),
        (bsnl '   CCTK_LOOP[DIM]_OMP_PRIVATE(name)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]_BOUNDARIES(name,'),
        (bsnl '                              '.(crpt '[C]').','),
        (bsnl '                              '.(crpt '[C]blo').','),
        (bsnl '                              '.(crpt '[C]bhi').')'),
        (bsnl '  CCTK_LOOP[DIM]STR_BOUNDARIES(name,'),
        (bsnl '                        '.(crpt '[C]').','),
        (bsnl '                        '.(crpt '[C]blo').','),
        (bsnl '                        '.(crpt '[C]bhi').','),
        (bsnl '                        1)'),
        (nl ''),
        (bsnl '#define CCTK_LOOP[DIM]STR_BOUNDARIES(name,'),
        (bsnl '                                 '.(crpt '[C]').','),
        (bsnl '                                 '.(crpt '[C]blo').','),
        (bsnl '                                 '.(crpt '[C]bhi').','),
        (bsnl '                                 istr)'),
        (bsnl '   && lc_blo = (/ '.(crpt '[C]blo').' /)'),
        (bsnl '   && lc_bhi = (/ '.(crpt '[C]bhi').' /)'),
        (bsnl '   && lc_istr = istr'),
        (bsnl '   && do lc_dir=1,[DIM]'),
        (bsnl '   &&    do lc_face=1,2'),
        (bsnl '   &&       do lc_d=1,[DIM]'),
        (bsnl '   &&          lc_bmin(lc_d) = 1'),
        (bsnl '   &&          lc_bmax(lc_d) = cctk_lsh(lc_d)'),
        (bsnl '   &&          if (lc_d<lc_dir) then'),
        (bsnl '   &&             lc_bmin(lc_d) = lc_bmin(lc_d)+lc_blo(lc_d)'),
        (bsnl '   &&             lc_bmax(lc_d) = lc_bmax(lc_d)-lc_bhi(lc_d)'),
        (bsnl '   &&          end if'),
        (bsnl '   &&       end do'),
        (bsnl '   &&       if (lc_face==1) then'),
        (bsnl '   &&          lc_bmax(lc_dir) = lc_bmin(lc_dir)+lc_blo(lc_dir)'),
        (bsnl '   &&       else'),
        (bsnl '   &&          lc_bmin(lc_dir) = lc_bmax(lc_dir)-lc_bhi(lc_dir)'),
        (bsnl '   &&       end if'),
        (bsnl '            CCTK_LOOP[DIM]STR(name,'),
        (bsnl '                          '.(crpt '[C]').','),
        (bsnl '                          '.(crpt 'lc_bmin([I+1])').','),
        (bsnl '                          '.(crpt 'lc_bmax([I+1])').','),
        (bsnl '                          '.(crpt 'cctk_ash([I+1])').','),
        (bsnl '                          lc_istr)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]_BOUNDARIES(name)'),
        (bsnl '  CCTK_ENDLOOP[DIM]STR_BOUNDARIES(name)'),
        (nl ''),
        (bsnl '#define CCTK_ENDLOOP[DIM]STR_BOUNDARIES(name)'),
        (bsnl '            CCTK_ENDLOOP[DIM](name)'),
        (bsnl '   &&    end do /* face */'),
        (bsnl '   && end do /* dir */'),
        (nl ''),
        (nl '#endif /* #ifdef FCODE */'),
    );
}                               # for dim



# Output footer
push @lines, (
    (nl ''),
    (nl ''),
    (nl ''),
    (nl '#endif /* #ifndef _CCTK_LOOP_H_ */'),
    );



# Write file
open FILE, '>', 'cctk_Loop.h' or die;
print FILE @lines;
close FILE;

print "Regenerated \"cctk_Loop.h\".\n";
