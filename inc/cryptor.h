#include "common.h"
#include <windows.h>
static const char* base_6_bits= "@ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz$";
int _stdcall text_encode(const char* xData,int data_len,const char* key, char* retValue);
int _stdcall text_decode(const char* xData,int data_len,const char* key, char* retValue);
int _stdcall cryptFile(char *inFile,char *pwd,bool isCrypted);
int _stdcall cryptFileByFile(char *inFile,char *pwdFile,bool isCrypted);
int _stdcall splitEmbedFile(char *embedFile,char * outPath,char * pwd, bool split,char *outFileName);
int _stdcall embedFile(char *frontFile,char * behideFile, char * behideFileName,char * pwd);
int _stdcall cryptFileAsText(char *inFile,char *pwd,bool isCrypted);

int _stdcall cryptFileOld(char *inFile,char *pwd,bool isCrypted);
int _stdcall cryptFileByFileOld(char *inFile,char *pwdFile,bool isCrypted);
int _stdcall splitEmbedFileOld(char *embedFile,char * outPath,char * pwd, bool split,char *outFileName);
int _stdcall embedFileOld(char *frontFile,char * behideFile, char * behideFileName,char * pwd);
