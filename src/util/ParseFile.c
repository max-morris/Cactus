 /*@@
   @file      ParseFile.c
   @date      Tue Jan 12 15:58:31 1999
   @author    Tom Goodale
   @desc 
   Routines to read in a parameter file and pass the resulting data
   to a user-supplied subroutine.
   Currently taken from the old cactus ones and slightly modifed.
   @enddesc 
 @@*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static char *rcsid = "$Header$";

/* Local definitions and functions */
#ifndef WIN32
#define BOLDON   "\033[1m"
#define BOLDOFF  "\033[0m"
#else
#define BOLDON   ""
#define BOLDOFF  ""
#endif

#define BUF_SZ 1024

static void CheckBuf(int, int);
static void removeSpaces(char *stripMe);



/*@@
  @routine ParseFile
  @author Paul Walker
  @desc The meat of the matter!
  This routine actually parses the parameter file. The
  syntax we allow is
  <ul>
    <li>a = b
        <li>a,b,c = d,e,f
        <li># rest of the line is ignored
        <li>x = "string value"
  </ul>
  So it is easy to parse
  <p>
  We go through the file looking for stuff and then set
  it in the global database using calls to @seeroutine 
  setIntVal, @seeroutine setFloatVal, @seeroutine setDoubleVal
  and @seeroutine setStringVal.
  @enddesc
  @history
  @hdate Tue Jan 12 16:41:36 1999 @hauthor Tom Goodale
  @hdesc Moved to CCTK.
         Changed to pass data to arbitrary function.
         Changed to take a file descriptor rather than a filename.
  @endhistory
  @@*/


int ParseFile(FILE *ifp, 
              int (*set_function)(const char *, const char *)) 
{
  /* Buffers for parsing from the file */
  char tokens[BUF_SZ], value[BUF_SZ];
  /* Positions in the buffers */
  int ntokens;
  /* Status flags */
  int intoken, inval;
  /* Current char. Have to make it an int so we can compare with
     EOF. See man 3 fgetc
   */
  int c;
  /* line number */
  int lineno = 1;

  intoken = 0; inval = 0;

  while ((c=fgetc(ifp)) != EOF) 
  {
    /* Main Loop */
    while (c == '#' || c == '!' ) 
    {
      /* Comment line.  So forget rest of line */
      while ((c=fgetc(ifp)) != '\n' && c != EOF);
      c = fgetc(ifp);
    }

    /* End of line */
    if (c == '\n') 
    {
      if(intoken)
      {
        fprintf(stderr, "Parse error at line %d.  No value supplied.\n", lineno);
        intoken = 0;
      }

      lineno ++;
#ifdef DEBUG
      printf ("LINE %d\n",lineno);
#endif

    }

    /* Token character */
    if (intoken && c != '=') 
    {
      tokens[intoken++] = c;
      CheckBuf(intoken,lineno);
    }

                
    /* Start of a new token */
    if (c != ' ' && c != '\n' && !inval && !intoken) 
    {
      intoken = 0;
      tokens[intoken++] = c;
    }

    /* End of a token signified by an = */
    if (c == '=')
    {
      if (intoken) 
      {
        unsigned int ll;
        tokens[intoken] = '\0';  /* Very important! */
        intoken = 0;
        inval = 0;
        removeSpaces(tokens);
        ntokens = 1;
        for (ll=0;ll < strlen(tokens); ll++)
          if (tokens[ll] == ',') ntokens++;
#ifdef DEBUG
        printf ("New token! >>%s<<\n",tokens);
        printf ("%d token elements\n",ntokens);
#endif

        /* Scan ahead to the beginning of the value
         * and check if the value is a string or
         * not. UNLIKE shalfs parser, this one DOES
         * strip quotes off of the strings. Bye bye
         * to StripQuotes routines...
         */
        while ((c = fgetc(ifp)) == ' ' || c == '\n' || c == '\t');
        if (c == '"') 
        {
          /* Just handle the damn thing. */
          int p = 0;
          if (ntokens > 1) 
          {
            fprintf (stderr, "%s%s%s\n",
                     "WARNING: Multiple string ",
                     "tokens not supported for ",
                     tokens);
            fprintf(stderr, "This is a fatal error");
            return 1;
          }
          while ((c = fgetc(ifp)) != '"') 
          {
            /* Make an important decision NOT to include 
             * line feeds in the string parameters
             */
            if (c != '\n') value[p++] = c;
            if (c == '\n') 
            {
              printf ("%sWarning:%s Quoted string contains newline for token %s\n",
                      BOLDON, BOLDOFF, tokens);
              printf ("This could indicated a parameter file error or missing quote\n");
            }
            CheckBuf(p,lineno);
          }
          value[p] = '\0';
#ifdef DEBUG
          printf ("String %s -> %s\n",
                  tokens,value);
#endif
          set_function(tokens,value);
          
          
        } 
        else 
        {
          int p = 0;
          value[p++] = c;
          if (ntokens == 1) 
          {
            /* Simple case. We have an int
               or a double which contain no
               spaces! */
            c = fgetc(ifp);
            while (!(c==' ' || c=='\t' || c == '\n' || c == EOF)) 
            {
              value[p++] = c;
              CheckBuf(p,lineno);
              c = fgetc(ifp);
            }
            value[p] = '\0';
#ifdef DEBUG
            printf ("Parsed %d characters\n", p);
            printf("Float/Int: %s -> %s\n", tokens,value);
#endif
            set_function(tokens,value);

          } 
          else 
          {
            /* Harder case of multiple tokens */
            int ncommas = 0;
            int p=0, i;
            char subtoken[BUF_SZ], subvalue[BUF_SZ];
            int pt, pv;

            value[p++] = c;
            /* OK, since we only have numbers in the
               old input stream, we can go along getting
               ntokens-1 commas, stripping spaces, and
               make a nice little string. 
               */
            c = fgetc(ifp);
            while (ncommas < ntokens-1 && c != EOF) 
            {
              if (!(c == ' ' || c == '\t' || c == '\n')) 
              {
                value[p++] = c;
                CheckBuf(p,lineno);
              }
              if (c == ',') ncommas ++;
              c = fgetc(ifp);
            }
            if (c == ' ' || c == '\t')
            {
              /* Great now strip out the spaces */
              while((c = fgetc(ifp)) == ' ' || c=='\t' || c == '\n');
            }
            
            /* And tack the rest on */
            value[p++] = c;
            CheckBuf(p,lineno);

            c = fgetc(ifp);
            while (c != ' ' && c != '\t' && c != '\n' && c != EOF) 
            {
              value[p++] = c;
              CheckBuf(p,lineno);
              c = fgetc(ifp);
            }
            value[p] = '\0';
#ifdef DEBUG
            printf("Comma list: %s -> %s\n",
                   tokens,value);
#endif
            /* So parse out the tokens */
            pt = 0;
            pv = 0;
            for (i=0;i<ncommas;i++) 
            {
              p = 0;
              while (tokens[pt] != ',') 
              {
                subtoken[p++] = tokens[pt++];
                CheckBuf(p,lineno);
              }
              subtoken[p] = '\0';
              p = 0;
              while (value[pv] != ',') 
              {
                subvalue[p++] = value[pv++];
                CheckBuf(p,lineno);
              }
              subvalue[p] = '\0';

              set_function(subtoken,subvalue);
#ifdef DEBUG
              printf("Setting sub-token %s -> %s\n",
                     subtoken, subvalue);
#endif
              /* Now remeber we are sitting on a comma
               * in both our input strings, so bump by one
               */
              pv ++; pt ++;
            }
            /* And OK, so now we have one parameter left
             * so lets handle that 
             */ 
            p = 0;
            while (tokens[pt] != '\0') 
            {
              subtoken[p++] = tokens[pt++];
              CheckBuf(p,lineno);
            }
            subtoken[p] = '\0';
            p = 0;
            while (value[pv] != '\0') 
            {
              subvalue[p++] = value[pv++];
              CheckBuf(p,lineno);
            }
            subvalue[p] = '\0';

            set_function(subtoken,subvalue);
          }
        }
      } 
      else 
      {
        fprintf (stderr, "Parser failed at = on line %d\n",
                 lineno);
      }
    }
  }

  return 0;
}

/*@@
  @routine CheckBuf
  @author Paul Walker
  @desc
  A simple description and warning message in case of
  a fixed parse buffer overflow.
  @enddesc
  @@*/

static void CheckBuf(int p, int l) 
{
  if (p >= BUF_SZ) {
    fprintf(stderr,"WARNING: Parser buffer overflow on line %d\n",
            l);
    fprintf(stderr,"This indicates either an incorrect parm file or\n");
    fprintf(stderr,"the need to recompile doParse.C with a bigger\n");
    fprintf(stderr,"BUF_SZ parm.\n");

    assert(0);
    exit(1);
  }
}

  
/*@@
  @routine removeSpaces
  @author Paul Walker
  @desc
  removes the spaces from a char * <b>in place</b>. Beware
  that this function will change the input value and if you
  want to keep a copy you have to do so yourself!
  @enddesc
  @comment
  The silly user (eg, me) may think to call @seeroutine trimString
  then this routine in order to remove all spaces from a string.
  But, duh, @seeroutine trimString is a <i>subset</i> of this, you
  dummy!
  @endcomment
  @@*/

static void removeSpaces(char *stripMe) 
{
        char *s;
        unsigned int i,j;
        s = (char *)malloc((strlen(stripMe)+2)*sizeof(char));
        strcpy(s,stripMe);
        for (i=0,j=0;i<strlen(s);i++)
                if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
                        stripMe[j++] = s[i];
        stripMe[j] = '\0';
        free(s);
}


/* #define TEST_ParseFile */

#ifdef TEST_ParseFile

int parameter_printer(const char *param, const char *val)
{
  printf("Parameter %s has value %s\n", param, val);

  return 0;
}

int main(int argc, char *argv[])
{
  int retval;
  FILE *parameter_file;

  if(argc > 1)
  {
    parameter_file = fopen(argv[1], "r");
    if(parameter_file)
    {
      ParseFile(parameter_file, parameter_printer);
      fclose(parameter_file);
      retval = 0;
    }
    else
    {
      retval=2;
    }
  }
  else
  {
    printf("Usage: %s <filename>\n", argv[0]);
    retval = 1;
  };

  return 0;
}

#endif
