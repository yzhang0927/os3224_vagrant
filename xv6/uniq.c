#include "types.h"
#include "user.h"
#include "stat.h"

char currChar; 
int cFlag = 0, dFlag = 0, iFlag = 0; 
char *prevLine, *currLine; 
int maxLineLength = 65536; 
/**
 * I know I shouldn't do this, 
 * but I'm too lazy to program up a dynamic sized array
 */

int
nextLine(int fd, char *buf, int lineLength) 
{
  int indexInLine = 0; 
  while (indexInLine < lineLength) {
    if (read(fd, &currChar, 1) < 1) {
      if (indexInLine) {
        buf[indexInLine++] = '\n'; 
        buf[indexInLine] = '\0';
        return lineLength; 
      } else {
        return 0; /* We are reading the last line */
      }
    }
    buf[indexInLine++] = currChar; 
    if (currChar == '\n') {
      buf[indexInLine] = '\0';
      return lineLength; 
    }
  }
  /* ran out of line length */
  return 0; 
}

void
swap()
{
  char *temp; 
  temp = prevLine; 
  prevLine = currLine; 
  currLine = temp; 
}

char
charToLower(char aChar)
{
  return ((aChar >= 'A') && (aChar <= 'Z')) ? aChar + 32 : aChar; 
}

int
strCmpIgnoreCase(const char *str1, const char *str2)
{
  while(*str1 && charToLower(*str1) == charToLower(*str2))
    str1++, str2++;
  return (uchar)*str1 - (uchar)*str2;
}

int
strEqual(const char* str1, const char* str2)
{
  // if (strlen(str1) != strlen(str2)) return 0; 
  return (iFlag) ? strCmpIgnoreCase(str1, str2) : strcmp(str1, str2); 
}

void 
uniqResultDisplay(char *line, int occurance)
{
  if (cFlag) {
    printf(1, "%d %s", occurance, line);
  } else {
    printf(1, "%s", line);
  }
}

void 
uniq(int fd) 
{

  int occurance = 1;

  prevLine = malloc(sizeof(char) * maxLineLength); 
  if (nextLine(fd, prevLine, maxLineLength) < 0) {
    free(prevLine);
    exit(); 
  } 

  currLine = malloc(sizeof(char) * maxLineLength); 
  while (nextLine(fd, currLine, maxLineLength)) {
    // printf(1, "prevLine is %s", prevLine);
    // printf(1, "currLine is %s", currLine);
    if (strEqual(prevLine, currLine)) {
      if (occurance > ((dFlag) ? 1 : 0)) {
        uniqResultDisplay(prevLine, occurance); 
      }

      /** 
       * swapping currLine and prevLine 
       * I don't wanna deal with char**
       * so I used global var
       * I know it's bad
       */

      swap(); 
      occurance = 1; 
    } else {
      ++occurance; 
    }
  }
  if (occurance > ((dFlag) ? 1 : 0)) {
    uniqResultDisplay(prevLine, occurance); 
  }
  free(prevLine); 
  free(currLine); 
}


int
checkFlag(char possibleFlag) 
{
  if (possibleFlag == 'c') {
    cFlag = 1; 
  } else if (possibleFlag == 'd') {
    dFlag = 1; 
  } else if (possibleFlag == 'i') {
    iFlag = 1; 
  } else {
    exit(); 
  }
  return 0; 
}

int
main(int argc, char *argv[]) 
{
  int fd, i;

  if(argc <= 1){
    uniq(0);
    exit();
  }

  for (i = 1; i < argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      checkFlag(argv[i][1]);
    }
    else {
      if((fd = open(argv[i], 0)) < 0){
        printf(1, "uniq: cannot open %s\n", argv[i]);
        exit();
      }
    }
  }
  if (cFlag && dFlag) {
    exit(); 
  }
  uniq(fd); 
  if(fd) {
    close(fd); 
  }
  exit(); 
}

