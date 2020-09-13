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


// Makes strings into counted strings.
// Note the length byte includes the null as part of the count

char tokens[] = "\"";
char *search = "\"";

void makeCounted(char string[],FILE *outfile)
{
    char *temp;
    char *token;
    int immediate = 0;
    int length;
    int emSlash = 0;
    char replacement;
    if(!strchr(string,'"'))
    {
        fprintf(outfile,"%s",string);
        return;
    }
    if(temp = strchr(string,'\\'), temp)
    {
        // there are embedded \ characters
        // in this case I only allow for 1
//        printf("emslash %s\n",string);
        emSlash = 1;
        // replace the character after the \ with a special symbol
        // so if it happens to be a " then the strings will parse correctely
        replacement = temp[1];
        temp[1] = 1; // a non printable character but easy to find
    }
    if(string[0] == 'I')
    {
//        printf("Found I\n");
        string[0] = ' ';
        immediate = 1;
    }
    token = strtok(string, search);
    if(token)fprintf(outfile,"%s",token);
    token = strtok(NULL, search);
    length = strlen(token)+1;
    if(emSlash)
    {
        length--;
        // find the 1
        temp = strchr(token,1);
        // replace the original character
        temp[0] = replacement;
        
    }
    if(immediate)
    {
        length |= 0x80; // set immediate bit
    }
    if(token)fprintf(outfile,"\"\\x%X\" \"%s\"",length,token);
    token = strtok(NULL, search);
    if(token)fprintf(outfile,"%s",token);
    return;
  
}



char inbuffer[512];

int main(int argc, char *argv[])
{
    FILE *infile;
    FILE *outfile;


    
    if(argc < 3)
    {
        printf("Filter filename \n");
        exit(1);
    }//
    printf("Opening %s\r\n",argv[1]);
    infile = fopen(argv[1],"rt");
    printf("Opening %s\r\n",argv[2]);
    outfile = fopen(argv[2],"w+t");
    
    while(!feof(infile))
    {
        if(fgets(inbuffer,256,infile))
        {
//            printf("%s",inbuffer);continue;
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
