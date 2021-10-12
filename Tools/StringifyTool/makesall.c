/* MIT License

Copyright (c) 2020 mnowell129
 
Charles M. Nowell Jr.
The Mouse Works, LLC 
mickeynowell@tmwfl.com 
 
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.*/
/*
MickeyForth (mForth) forth interpreter.
copyright 1998-2011
 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

///*************************************************************
///*************************************************************

/**
 * @brief funkyStrlen
 * 
 * @author byk1 (11/1/2018)
 * 
 * @param string 
 * 
 * @return int 
 */
int funkyStrlen(char *string)
{
   int count = 0;
   while(*string)
   {
      if((*string) != '\\')
      {
         count++;
      }
      string++;
   }
   return(count);
}

///*************************************************************
///*************************************************************
/// Makes strings into counted strings.
/// Note the length byte includes the null as part of the count
///*************************************************************
///*************************************************************


///*************************************************************
///*************************************************************
char tokens[] = "\"";
char *search = "\"";

///*************************************************************
///*************************************************************

/**
 * @brief makeCounted
 * 
 * @author byk1 (11/1/2018)
 * 
 * @param string 
 * @param outfile 
 */
void makeCounted(char string[],FILE *outfile)
{
   char *temp;
   char *token;
   if(!strchr(string,'"'))
   {
      fprintf(outfile,"%s",string);
      return;
   }
   // find all the first part of the string
   // up to the first quote
   token = strtok(string,search);
   // output it
   if(token) fprintf(outfile,"%s",token);
   // now find the end of the next string
   token = strtok(NULL,search);
   while(1)
   {
      if(token)
      {
         // output the string
         // count ourselves because we need to handle escapes
         fprintf(outfile,"\"\\x%X\" \"%s\"",funkyStrlen(token) + 1,token);
      }
      // now find up until the next quote
      // and output it
      token = strtok(NULL,search);
      if(token) fprintf(outfile,"%s",token);
      // now find the start of the next string
      token = strtok(NULL,search);
      // if there isn't one
      // then quit
      if(!token)
      {
         break;
      }
   }
   return;
}

///*************************************************************
///*************************************************************
char line[] = "LINE\"TO\"BE\"SEPARATED";

///*************************************************************
///*************************************************************

/**
 * @brief printIn
 * 
 * @author byk1 (11/1/2018)
 * 
 * @param string 
 */
void printIn(char string[])
{
   char *token;
   token = strtok(string,search);
   if(token) printf("%s",token);
   token = strtok(NULL,search);
   if(token) printf("%s",token);
   printf("\n");
}

///*************************************************************
///*************************************************************
char inbuffer[512];

///*************************************************************
///*************************************************************

/**
 * @brief main function
 * 
 * @author byk1 (11/1/2018)
 * 
 * @param argc 
 * @param argv 
 * 
 * @return int 
 */
int main(int argc, char *argv[])
{
   FILE *infile;
   FILE *outfile;

   if(argc < 3)
   {
      printf("Filter filename \n");
      exit(1);
   }

   printf("Opening %s\r\n",argv[1]);
   infile = fopen(argv[1],"rt");
   printf("Opening %s\r\n",argv[2]);
   outfile = fopen(argv[2],"w+t");

   while(!feof(infile))
   {
      if(fgets(inbuffer,256,infile))
      {
          //printf("%s",inbuffer);continue;
         makeCounted(inbuffer,outfile);
      }
      else
      {
         break;
      }

   }

   fclose(infile);
   fclose(outfile);
   return(0);
}
