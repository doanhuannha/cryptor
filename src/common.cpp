#include "..\inc\common.h"
unsigned long getStrLength(const char* str)
{
	long i=0;
	while(str[i]!=0)
	{
		i++;
	}
	return i;
}
void addStr(char* str,const char* strAdd)
{
	int i=0;
	int j=0;
	while(str[i]!=0)
	{
		i++;
	}

	while(strAdd[j]!=0)
	{
		*(str+i)=*(strAdd+j);
		i++;
		j++;
	}
	str[i]=0;
	return;
}
void joinbyte(char* str,int lengthStr,char* strAdd,int lengthStrAdd)
{
	int i=lengthStr;
	int j=0;
	

	while(j<lengthStrAdd)
	{
		*(str+i)=*(strAdd+j);
		i++;
		j++;
	}
	return;
}
void bytecpy(char* destStr, char* sourceStr,int startIndex, int length)
{
	int i=0;
	int j=startIndex;
	while(j<=startIndex+length-1)
	{
		//*(destStr+i)=*(sourceStr+j);
		destStr[i]=sourceStr[j];
		i++;
		j++;
	}
	return;
}
int findLastString(char const* str,char strToFind){
    long strLen=getStrLength(str);
	for (int i=strLen;i>=0;i--)
	{
		if(str[i]==strToFind)
		{
			return i;
		}
	}
	return -1;
}
int findString(char const* str,char strToFind){
	long strLen=getStrLength(str);
	for (int i=0;i<strLen;i++)
	{
		if(str[i]==strToFind)
		{
			return i;
		}
	}
	return -1;
}
void findString(char const* str,char const* strToFind, long* &returnValue)
{

	long strLen=getStrLength(str);
	long strToFindLen=getStrLength(strToFind);
	long i=0,j=0;
	long count=0;
	bool isSame;
	returnValue = new long[1024];
	for (i=0;i<strLen;i++)
	{
		if(str[i]==strToFind[0])
		{
			isSame=true;
			for (j=1;j<strToFindLen;j++)
			{
				if (str[i+j]!=strToFind[j])
				{
					isSame=false;
					break;
				}
			}
			if (isSame)
			{
				count++;
				returnValue[count]=i;
				i=i+strToFindLen-1;
			}
		}
	}
	returnValue[0]=count;
}

////////////////////////////////
//bit function
////////////////////////////////
void exchangeBit(char* address,int bitIndexA,int bitIndexB){
	char const BIT_INDEX[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	int indexA = bitIndexA / 8;
	bitIndexA = bitIndexA % 8;
	bool setBitA = false;
	char bitTocheckA = 0;

	int indexB = bitIndexB / 8;
	bitIndexB = bitIndexB % 8;
	bool setBitB = false;
	char bitTocheckB = 0;

	
	//check bit A
	bitTocheckA = BIT_INDEX[bitIndexA];
	if ((bitTocheckA & *(address+indexA)) != 0)
	{
		setBitA = true;
	}

	//check bit B
	bitTocheckB = BIT_INDEX[bitIndexB];
	if ((bitTocheckB & *(address+indexB)) != 0)
	{
		setBitB = true;
	}
	//cout<<"indexA-bitIndexA-setBitA-bitTocheckA:"<<indexA<<"-"<<bitIndexA<<"-"<<(int)setBitA<<"-"<<(int)bitTocheckA<<endl;
	//cout<<"indexB-bitIndexB-setBitB-bitTocheckB:"<<indexB<<"-"<<bitIndexB<<"-"<<(int)setBitB<<"-"<<(int)bitTocheckB<<endl;

	//assign A value for B pos
	if(setBitA) *(address+indexB) = *(address+indexB) | bitTocheckB;
	else  *(address+indexB) = *(address+indexB) & (bitTocheckB ^ 0xFF);

	//assign B value for A pos
	if(setBitB) *(address+indexA) = *(address+indexA) | bitTocheckA;
	else  *(address+indexA) = *(address+indexA) & (bitTocheckA ^ 0xFF);

}
void copyBit(char const* source,int sourceBitIndex,char* dest,int destBitIndex){
	char const BIT_INDEX[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	int sourceIndex = sourceBitIndex / 8;
	sourceBitIndex = sourceBitIndex % 8;

	int destIndex = destBitIndex / 8;
	destBitIndex = destBitIndex % 8;


	
	//check source bit
	if ((BIT_INDEX[sourceBitIndex]  & *(source+sourceIndex)) != 0){//set 1
		*(dest+destIndex) = *(dest+destIndex) | BIT_INDEX[destBitIndex];
	}
	else{//set 0
		 *(dest+destIndex) = *(dest+destIndex) & (BIT_INDEX[destBitIndex] ^ 0xFF);
	}
}

void leftShift(char* address,int bitIndexStart,int bitIndexEnd){
	if(bitIndexStart>=bitIndexEnd)return;
	char const BIT_INDEX[] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
	int indexStart = bitIndexStart / 8;
	bitIndexStart = bitIndexStart % 8;


	int indexEnd = bitIndexEnd / 8;
	bitIndexEnd = bitIndexEnd % 8;

	bool nextFirstBit = false;
	bool firstBit = false;

	if (( BIT_INDEX[bitIndexStart] & *(address+indexStart)) != 0){
		firstBit = true;
	}
	char tmp = 0x0;
	if(indexStart==indexEnd){//in the same char
		tmp = 0x0;
		for(char j=0;j<=8;j++){
			if(j==bitIndexEnd){
				if(firstBit) tmp = tmp | BIT_INDEX[bitIndexEnd]; 
				//else tmp = tmp | 0x0;
			}
			else if(bitIndexStart <=j && j<= bitIndexEnd){
				copyBit(address,(indexStart*8)+j+1,&tmp,j);
			}
			else{
				copyBit(address,(indexStart*8)+j,&tmp,j);
			}
		}
		*(address+indexStart) = tmp;
		return;
	}
	for(int i=indexStart;i<=indexEnd;i++){
		
		if(i<indexEnd && i > indexStart){
			*(address+i) = *(address+i) << 1;
			if ((0x80 & *(address+i+1)) != 0){
				*(address+i) = *(address+i) | 0x01;
			}
		}
		else if(i==indexStart){//first char
			if(bitIndexStart==0){//bit 0
				*(address+indexStart) = *(address+indexStart) << 1;
				if ((0x80 & *(address+indexStart+1)) != 0){
					*(address+indexStart) = *(address+indexStart) | 0x01;
				}
			}
			else{//middle bit
				char tmp = 0x0;
				for(char j=0;j<8;j++){
					if(j>=bitIndexStart){
						copyBit(address,(indexStart*8)+j+1,&tmp,j);
					}
					else{
						copyBit(address,(indexStart*8)+j,&tmp,j);
					}
				}
				*(address+indexStart) = tmp;
				if ((0x80 & *(address+indexStart+1)) != 0){
					*(address+indexStart) = *(address+indexStart) | 0x01;
				}
			}
		}
		else{// last char
			char tmp = 0x0;
			for(char j=0;j<8;j++){
				if(j==bitIndexEnd){
					if(firstBit) tmp = tmp | BIT_INDEX[bitIndexEnd]; 
					//else tmp = tmp | 0x0;
				}
				else if(j<bitIndexEnd){
					copyBit(address,(indexEnd*8)+j+1,&tmp,j);
				}
				else{
					copyBit(address,(indexEnd*8)+j,&tmp,j);
				}
			}
			*(address+indexEnd) = tmp;
		}
	}
}
void rightShift(char* address,int bitIndexStart,int bitIndexEnd){
	for(int i =0;i<bitIndexEnd-bitIndexStart;i++){
		leftShift(address,bitIndexStart,bitIndexEnd);
	}
}
void rotateBit(char& byte){
     exchangeBit(&byte,0,7);
     exchangeBit(&byte,1,6);
     exchangeBit(&byte,2,5);
     exchangeBit(&byte,3,4);
}
char * longToChar(unsigned long value)
{
	char * returnStr =new char[MAX_CHAR];
	long result=value;
	int i=0;
	do
	{
		returnStr[i]= result%10+48;//neu muon hien dang ascii thi + 48
		result= result/10;
		i++;
	}while(result!=0);
	returnStr[i]=0;
	return returnStr;

}
void splitXML(char *str,const char *splitStr1,const char *splitStr2,char *result)
{
	long *pos1=findStr(str,splitStr1);
	if (pos1[0]!=0)
	{
		long *pos2=findStr(str,splitStr2);
		if (pos2[0]!=0)
		{
			long startPos,stopPos;
			if (pos1[1]==pos2[1])
			{
				result[0]=0;
			}
			
			if (pos1[1]<pos2[1])
			{
				startPos=pos1[1]+getStrLength(splitStr1);;
				stopPos=pos2[1];			
			}
			else
			{
				startPos=pos2[1]+getStrLength(splitStr2);
				stopPos=pos1[1];
			}
			
			int i=0,j=0;
			for(i=startPos;i<stopPos;i++)
			{
				result[j]=str[i];
				j++;
			}
			result[j]=0;
			
		}
	}
	else
	{
		result[0]=0;
	}
	return;
}
long lt(int x,int y)
{
	long value=1;
	for(int i=1;i<=y;i++)
	{
		value=value*x;
	}
	return value;

}

long * findStr(char *str,const char *strToFind)
{
	long *returnValue=new long;
	long strLen=getStrLength(str);
	long strToFindLen=getStrLength(strToFind);
	long i=0,j=0;
	long count=0;
	bool isSame;
	for (i=0;i<strLen;i++)
	{
		if(str[i]==strToFind[0])
		{
			isSame=true;
			for (j=1;j<strToFindLen;j++)
			{
				if (str[i+j]!=strToFind[j])
				{
					isSame=false;
					break;
				}
			}
			if (isSame)
			{
				count++;
				returnValue[count]=i;
				i=i+strToFindLen-1;
			}
		}
	}
	returnValue[0]=count;
	return returnValue;

}
int writeLog(const char* logFile,const char* msg,bool newLine){
 	HANDLE hFile = CreateFile(logFile,   // open TWO.TXT 
	GENERIC_WRITE,                // open for writing 
	1,                            // share 
	NULL,                         // no security 
	OPEN_ALWAYS,                  // open or create 
	FILE_ATTRIBUTE_NORMAL,        // normal file 
	NULL);
    if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		return 1; 
	}
	DWORD dwBytesWritten, dwPos, dwMsgLen;
	dwPos = SetFilePointer(hFile, 0, NULL, FILE_END);
    dwMsgLen = strlen(msg);
    /*
    SYSTEMTIME* now = new SYSTEMTIME;
    GetLocalTime(now);
    delete now;
    */
	LockFile(hFile, dwPos, 0, dwPos + dwMsgLen + (newLine?2:0), 0);
	WriteFile(hFile, msg, dwMsgLen, &dwBytesWritten, NULL);
	if(newLine) WriteFile(hFile, "\r\n", 2, &dwBytesWritten, NULL);
	UnlockFile(hFile, dwPos, 0, dwPos + dwMsgLen + (newLine?2:0), 0);
	CloseHandle(hFile);
	return 0;
}
int RegWrite(HKEY hRootKey,const char* subKey,const char* keyValName,DWORD regType,const BYTE *lpData,DWORD cbData){
    DWORD lRv;
    HKEY hKey;
    hRootKey = HKEY_CURRENT_USER;
    lRv = RegOpenKeyEx(hRootKey, subKey, 0, KEY_WRITE, &hKey);
    
    if (lRv != ERROR_SUCCESS)
    {
    DWORD dwDisposition;
    
    // Create a key if it did not exist
    lRv = RegCreateKeyEx(hRootKey,
        subKey,
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
        &hKey, &dwDisposition);
    }
    lRv = RegSetValueEx(hKey, keyValName, 0, regType, lpData, cbData);
    lRv = RegCloseKey(hKey);

}
int RegDelete(HKEY hRootKey,const char* subKey,const char* delKey){
    DWORD lRv;
    HKEY hKey;
    hRootKey = HKEY_CURRENT_USER;
    lRv = RegOpenKeyEx(hRootKey, subKey, 0, KEY_WRITE, &hKey);
    
    if (lRv == ERROR_SUCCESS)
    {
       RegDeleteKey(hKey,delKey);
    }
    lRv = RegCloseKey(hKey);
}
int RegDeleteVal(HKEY hRootKey,const char* subKey,const char* delVal){
    DWORD lRv;
    HKEY hKey;
    hRootKey = HKEY_CURRENT_USER;
    lRv = RegOpenKeyEx(hRootKey, subKey, 0, KEY_WRITE, &hKey);
    
    if (lRv == ERROR_SUCCESS)
    {
       RegDeleteValue(hKey,delVal);
    }
    lRv = RegCloseKey(hKey);
}
int RegRead(HKEY hRootKey,const char* subKey,const char* keyValName,LPDWORD pdwType,LPBYTE lpData,LPDWORD lpcbData){
    DWORD lRv;
    HKEY hKey;
    hRootKey = HKEY_CURRENT_USER;
    lRv = RegOpenKeyEx(hRootKey, subKey, 0, KEY_QUERY_VALUE, &hKey);
    
    if (lRv == ERROR_SUCCESS)
    {
       RegQueryValueEx(hKey,keyValName,NULL,pdwType,lpData,lpcbData);
    }
    lRv = RegCloseKey(hKey);
}
unsigned char* loadFile(const char* filename, int& fileSize) {
    // Open the file using CreateFile
    HANDLE hFile = CreateFile(
        filename, // File name
        GENERIC_READ,      // Desired access: read
        0,                 // Share mode: no sharing
        NULL,              // Security attributes: NULL for default
        OPEN_EXISTING,     // Open an existing file
        FILE_ATTRIBUTE_NORMAL, // Normal file attributes
        NULL               // No template file
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    // Get the file size
    fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE) {
        CloseHandle(hFile);
        return NULL;
    }

    // Allocate memory for the file contents
    unsigned char* buffer = new unsigned char[fileSize];

    // Read the file content into the buffer
    DWORD bytesRead;
    if (!ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
        delete[] buffer;
        CloseHandle(hFile);
        return NULL;
    }
    // Close the file
    CloseHandle(hFile);
    // Return the pointer to the file content
    return buffer;
}
