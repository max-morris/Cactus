/*@@
   @file      Banner.c
   @date      July 16 00:11:26 1999
   @author    Gabrielle Allen
   @desc 
   Routines to deal with the Cactus banners.
   @enddesc 
 @@*/

/*#define DEBUG_BANNER*/

#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "cctk_parameters.h"
#include "cctk_Banner.h"
#include "cctk_FortranString.h"

static char *rcsis = "$Header$";

static int number_banners = 0;
static const char **banner_strings;

void CCTK_PrintBanners(void);



 /*@@
   @routine    CactusBanner
   @date       Wed Oct 13 21:41:28 CEST 1999
   @author     Gabrielle Allen
   @desc 
   Prints the Cactus Banner
   @enddesc 
@@*/

void CactusBanner(void)
{
  
  const char *string;

#define B_1 "       10                                  "
#define B_2 "  1   0101       ************************  "
#define B_3 "  01  1010 10      The Cactus Code V4.0    "
#define B_4 " 1010 1101 011      www.cactuscode.org     "
#define B_5 "  1001 100101    ************************  "
#define B_6 "    00010101                               "
#define B_7 "     100011     (c) Copyright The Authors  "
#define B_8 "      0100      GNU Licensed. No Warranty  "
#define B_9 "      0101                                 "


#define B_ANNERLINE B_1 "\n" B_2 "\n" B_3 "\n" B_4 "\n" B_5 "\n" B_6 "\n" B_7 "\n" B_8 "\n" B_9 "\n"
 

  string = B_ANNERLINE;

  printf("-----------------------------------------------------------\n");
  printf("%s\n",string); 
  printf("-----------------------------------------------------------\n");

}


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

#ifdef DEBUG_BANNER
  printf("Registering banner .... \n%s\n",banner_strings[number_banners-1]);
#endif

}

void  FMODIFIER FORTRAN_NAME(CCTK_RegisterBanner)(ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(message)
  CCTK_RegisterBanner(message);
  /* Don't free0 "message" or it will disappear */
}

 /*@@
   @routine    CCTK_PrintBanners
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

void CCTK_PrintBanners(void)
{

  DECLARE_CCTK_PARAMETERS

  int i;

  if (cctk_show_banners)
  {
    for (i=0;i<number_banners;i++)
    {
      if (banner_strings[i])
      {
	printf("--------------------------------------------------------------------------------\n");
	printf("%s\n",banner_strings[i]); 
      }
    }
    printf("--------------------------------------------------------------------------------\n"); 
  }
}
  



