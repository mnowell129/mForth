/**
 ***************************************************************
 ***************************************************************
 * MEDTRONIC CONFIDENTIAL - This document is the property of 
 * Medtronic, Inc.,and must be accounted for. Information herein 
 * is confidential. Do not reproduce it, reveal it to 
 * unauthorized persons, or send it outside Medtronic without 
 * proper authorization.
 * 
 * Copyright Medtronic, Inc. 2018
 ***************************************************************
 ***************************************************************
 * 
 * @file    makes.c
 *  
 * @brief   makes files
 * 
 * @author nowelc2 
 * 
 ***************************************************************
 ***************************************************************
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
    token = strtok(string, search);
    if(token)fprintf(outfile,"%s",token);
    token = strtok(NULL, search);
    if(token)
    {
       // output the string
       // count ourselves because we need to handle escapes
       fprintf(outfile,"\"\\x%X\" \"%s\"",funkyStrlen(token)+1,token);
    }
    token = strtok(NULL, search);
    if(token)fprintf(outfile,"%s",token);
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
    token = strtok(string, search);
    if(token)printf("%s",token);
    token = strtok(NULL, search);
    if(token)printf("%s",token);
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
