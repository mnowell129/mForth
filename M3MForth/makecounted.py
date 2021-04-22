# Python Script for converting XML database configuration to embedded C codeFile
# @author Tom Burchfield
#
# Ported from existing ANSI C database generator code

import sys
import math
import binascii
import os.path
import re

## Usage function for script call with invalid arguments.
#
def usage():
   print('Usage:\nmakecounted.py <1> <2>\n')
   print('<1> - input file\n')
   print('<2> - output file\n')



def main():
   print('string to counted string converter')
   arguments = len(sys.argv) - 1
   position = 1
   if (arguments < 2):
      usage()
   else:
      inputFileName = sys.argv[1]
      outputFileName = sys.argv[2]
      if (True): ## os.path.getmtime(xmlFile) > os.path.getmtime(codeFile)):
         with open(inputFileName, 'r') as inputFile:
            with open(outputFileName, 'w') as outputFile:
               while True:
                  inputLine = inputFile.readline()
                  if inputLine != "":
                     # find the front of the string, assume no " in the first position
                     while True:
                        match = re.search(r'[^"]+"',inputLine)
                        if match != None:
                           newString = match[0][:-1]
                           # print(newString)
                           outputFile.write(newString)
                           inputLine = inputLine[len(match[0])-1:]
                           # print("remaining input line " + inputLine)
                        else:
                           outputFile.write(inputLine)
                           # print("No match before quotes")
                           break
                        match = re.search(r'"[^"]+"',inputLine)
                        if match != None:
                           newString = match[0][1:-1]
                           # cFile.write("   [{:d}].length = {:d},\n".format(recordNumber, length))
                           outputFile.write("\"\\x{:02X}\" \"".format(len(newString)+1))
                           outputFile.write(newString+"\"")
                           # print(newString)
                           inputLine = inputLine[len(match[0]):]
                           # print("remaining input line " + inputLine)
                        else:
                           # print("No match between quotes")
                           outputFile.write(inputLine)
                           break
                  else:
                     break
               outputFile.close()
            inputFile.close()


if __name__ == '__main__':
   main()
