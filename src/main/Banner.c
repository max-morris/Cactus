/*@@
   @file      Banner.c
   @date      July 16 00:11:26 1999
   @author    Gabrielle Allen
   @desc 
   Routines to deal with the Cactus banners.
   @enddesc 
   @version $Header$
 @@*/

/*#define DEBUG_BANNER*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cctk_Banner.h"
#include "cctk_Config.h"
#include "cctk_Flesh.h"
#include "cctk_FortranString.h"
#include "cctk_Parameter.h"

static const char *rcsid = "$Header$";

CCTK_FILEVERSION(main_Banner_c)

/********************************************************************
 *********************     Local Data Types   ***********************
 ********************************************************************/

/********************************************************************
 ********************* Local Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 ********************* Other Routine Prototypes *********************
 ********************************************************************/

/********************************************************************
 *********************     Local Data   *****************************
 ********************************************************************/

static int number_banners = 0;
static char **banner_strings = NULL;

/********************************************************************
 *********************     External Routines   **********************
 ********************************************************************/


 /*@@
   @routine    CCTKi_CactusBanner
   @date       Wed Oct 13 21:41:28 CEST 1999
   @author     Gabrielle Allen
   @desc 
   Prints the Cactus Banner
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
@@*/

void CCTKi_CactusBanner(void)
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
  printf("--------------------------------------------------------------------------------\n");
  printf("%s\n",string); 
  printf("--------------------------------------------------------------------------------\n");

}


 /*@@
   @routine    CCTK_RegisterBanner
   @date       July 16 00:11:26 1999
   @author     Gabrielle Allen
   @desc 
   Registers a string as a banner
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 
   @var     string
   @vdesc   The banner as a C string
   @vtype   const char *
   @vio     in
   @vcomment 

   @endvar 

   @returntype int
   @returndesc
   0 -- success
   @endreturndesc
@@*/

int CCTK_RegisterBanner(const char *string)
{
  char **temp = NULL;
  char *newstring;
  number_banners++;

  /* Resize the array of banner strings */
  if (number_banners == 1)
  {
    banner_strings = (char **)malloc( number_banners*sizeof(char *));  
    temp = banner_strings;
  }
  else
  {
    temp = (char **)realloc( banner_strings, number_banners*sizeof(char *));  

    if(temp)
    {
      banner_strings = temp;
    }
    else
    {
      number_banners--;
    }
  }

  /* If this was succesful, copy the data into the array */
  if(temp)
  {
    newstring = (char *)malloc((strlen(string)+1)*sizeof(char));
    if(newstring)
    {
      strcpy(newstring, string);
      banner_strings[number_banners-1] = newstring;
    }
    else
    {
      number_banners--;
    }
  }

#ifdef DEBUG_BANNER
  printf("Registering banner .... \n%s\n",banner_strings[number_banners-1]);
#endif

  return 0;

}

void  CCTK_FCALL CCTK_FNAME(CCTK_RegisterBanner)
     (int *ierr,ONE_FORTSTRING_ARG)
{
  ONE_FORTSTRING_CREATE(message)
  *ierr=CCTK_RegisterBanner(message);
  free(message);
}

 /*@@
   @routine    CCTKi_PrintBanners
   @date       July 16 00:11:26 1999
   @author     Gabrielle Allen
   @desc 
   Print all registered banners
   @enddesc 
   @calls     
   @calledby   
   @history 
 
   @endhistory 

   @returntype int
   @returndesc
   0 -- success
   @endreturndesc
@@*/

int CCTKi_PrintBanners(void)
{
  int i;
  int param_type;
  int cctk_show_banners;

  cctk_show_banners = (*(CCTK_INT *)CCTK_ParameterGet("cctk_show_banners",
						 "Cactus",&param_type));

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

  return 0;

}
  
/********************************************************************
 *********************     Local Routines   *************************
 ********************************************************************/



