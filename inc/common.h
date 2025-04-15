#include <windows.h>
#include "config.h"
unsigned long getStrLength(const char* str);
void addStr(char* str,const char* strAdd);

void joinbyte(char* str,int lengthStr,char* strAdd,int lengthStrAdd);

void bytecpy(char* destStr, char* sourceStr,int startIndex, int length);
int findString(char const* str,char strToFind);
int findLastString(char const* str,char strToFind);
void findString(char const* str,char const* strToFind, long* &returnValue);
////////////////////////////////
//bit function
////////////////////////////////
void exchangeBit(char* address,int bitIndexA,int bitIndexB);
void copyBit(char const* source,int sourceBitIndex,char* dest,int destBitIndex);
void leftShift(char* address,int bitIndexStart,int bitIndexEnd);
void rightShift(char* address,int bitIndexStart,int bitIndexEnd);
char * longToChar(unsigned long value);
void splitXML(char *str,const char *splitStr1,const char *splitStr2,char *result);
long lt(int x,int y);
long * findStr(char *str,const char *strToFind);
void rotateBit(char& byte);
int writeLog(const char* logFile,const char* msg,bool newLine);


int RegRead(HKEY hRootKey,const char* subKey,const char* keyValName,LPDWORD pdwType,LPBYTE lpData,LPDWORD lpcbData);
int RegWrite(HKEY hRootKey,const char* subKey,const char* keyValName,DWORD regType,const BYTE *lpData,DWORD cbData);
int RegDelete(HKEY hRootKey,const char* subKey,const char* delKey);
int RegDeleteVal(HKEY hRootKey,const char* subKey,const char* delVal);

