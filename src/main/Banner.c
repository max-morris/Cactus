 /*@@
   @file      Banner.c
   @date      July 16 00:11:26 1999
   @author    Gabrielle Allen
   @desc 
   Routines to deal with the Cactus banners.
   @enddesc 
 @@*/

#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "cctk_parameters.h"

static int number_banners = 0;
static const char **banner_strings;

void CCTKi_PrintBanners(void);


 /*@@
   @routine    CCTK_RegisterBanner
   @date       July 16 00:11:26 1999
   @author     Gabrielle Allen
   @desc 
   Registers a string as a banner
   @enddesc 
   @var     string
   @vdesc   The banner as a C string
   @vtype   const char *
   @vio     
   @vcomment 
   @endvar 
@@*/

void CCTK_RegisterBanner(const char *string)
{

  const char **temp = NULL;
  number_banners++;

  if (number_banners == 1)
  {
    banner_strings = (const char **)malloc( number_banners*sizeof(const char *));  
    temp = banner_strings;
  }
  else
  {
    temp = realloc( banner_strings, number_banners*sizeof(const char *));  

    if(temp)
    {
      banner_strings = temp;
    }
  }

  if(temp)
  {
    banner_strings[number_banners-1] = string;
  }

}


 /*@@
   @routine    CCTKi_PrintBanners
   @date       July 16 00:11:26 1999
   @author     Gabrielle Allen
   @desc 
   Print all registered banners
   @enddesc 
   @var     string
   @vdesc   The banner as a C string
   @vtype   const char *
   @vio     
   @vcomment 
   @endvar 
@@*/

void CCTKi_PrintBanners(void)
{

  DECLARE_CCTK_PARAMETERS

  int i;

  if (cctk_show_banners)
  {
    for (i=0;i<number_banners;i++)
    {
      if (banner_strings[i])
      {
	printf("-------------------------------------------------------------------------------------\n");
	printf("%s\n",banner_strings[i]); 
	printf("-------------------------------------------------------------------------------------\n");
      }
    }
  }
}
  



