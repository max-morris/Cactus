#! /bin/sh
# /*@@
#   @file      CCTK_Functions.sh
#   @date      Wed Jul 21 11:16:06 1999
#   @author    Tom Goodale
#   @desc 
#   
#   @enddesc 
#   @version $Header$
#
# @@*/


# /*@@
#   @routine    CCTK_Search
#   @date       Wed Jul 21 11:16:35 1999
#   @author     Tom Goodale
#   @desc 
#   Used to search for something in various directories
#   @enddesc 
#   @calls     
#   @calledby   
#   @history 
# 
#   @endhistory 
#
#@@*/

function CCTK_Search()
{
  eval  $1=""
  if test -z $4 ; then
    cctk_basedir=""
  else
    cctk_basedir="$4/"
  fi
  for cctk_place in $2  
    do
      echo $ac_n "Looking in $cctk_place""...$ac_c" #1>&6
      if test -r "$cctk_basedir$cctk_place/$3" ; then
        echo "$ac_t""...Found" #1>&6
        eval $1="$cctk_place"
        break
      fi
      if test -d "$cctk_basedir$cctk_place/$3" ; then
        echo "$ac_t""...Found" #1>&6
        eval $1="$cctk_place"
        break
      fi
      echo "$ac_t""No" #1>&6
    done

  return
}

function CCTK_CreateFile
{
  echo $2 > $1
  return
}

function CCTK_WriteLine
{
  echo $2 >> $1
  return
}
