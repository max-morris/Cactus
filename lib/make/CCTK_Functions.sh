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

CCTK_Search()
{
  eval  $1=""
  if test $# -lt 4 ; then
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

# /*@@
#   @routine    CCTK_CreateFile
#   @date       Wed Jul 21 11:16:35 1999
#   @author     Tom Goodale
#   @desc 
#   Creates a file
#   @enddesc 
#   @calls     
#   @calledby   
#   @history 
# 
#   @endhistory 
#
#@@*/

CCTK_CreateFile()
{
  echo $2 > $1
  return
}

# /*@@
#   @routine    CCTK_WriteLine
#   @date       Wed Jul 21 11:16:35 1999
#   @author     Tom Goodale
#   @desc 
#   Writes a line to a file
#   @enddesc 
#   @calls     
#   @calledby   
#   @history 
# 
#   @endhistory 
#
#@@*/

CCTK_WriteLine()
{
  echo "$2" >> $1
  return
}

# /*@@
#   @routine    CCTK_AddPrefix
#   @date       Sat Nov  4 00:23:23 2000
#   @author     Tom Goodale
#   @desc 
#   Adds a prefix to each member of a list
#   @enddesc 
#   @calls     
#   @calledby   
#   @history 
# 
#   @endhistory 
#
#@@*/

CCTK_AddPrefix()
{
  unset _cctk_addprefix_retval
    
  for val in $2
  do
    if test -n "$val" ; then  
      _cctk_addprefix_retval="$_cctk_addprefix_retval $1$val"
    fi
  done
    
  echo "$_cctk_addprefix_retval"
    
  unset _cctk_addprefix_retval
}

# /*@@
#   @routine    CCTK_AddSuffix
#   @date       Sat Nov  4 00:23:23 2000
#   @author     Tom Goodale
#   @desc 
#   Adds a suffix to each member of a list
#   @enddesc 
#   @calls     
#   @calledby   
#   @history 
# 
#   @endhistory 
#
#@@*/

CCTK_AddSuffix()
{
  unset _cctk_addsuffix_retval
    
  for val in $2
  do
    if test -n "$val" ; then  
      _cctk_addsuffix_retval="$_cctk_addsuffix_retval $val$1"
    fi
  done
    
  echo "$_cctk_addsuffix_retval"
    
  unset _cctk_addsuffix_retval
}

# /*@@
#   @routine    CCTK_Wrap
#   @date       Sat Nov  4 00:23:23 2000
#   @author     Tom Goodale
#   @desc 
#   Adds a prefix and a suffix to each member of a list
#   @enddesc 
#   @calls     
#   @calledby   
#   @history 
# 
#   @endhistory 
#
#@@*/

CCTK_Wrap()
{
  unset _cctk_wrap_retval
    
  for val in $3
  do
    if test -n "$val" ; then  
      _cctk_wrap_retval="$_cctk_wrap_retval $1$val$2"
    fi
  done
    
  echo "$_cctk_wrap_retval"
    
  unset _cctk_wrap_retval
}
