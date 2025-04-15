#include "..\inc\cryptor.h"
int text_encode(const char* xData,int data_len,const char* key, char* retValue){
	/*
	Description: 
	Root number (1 byte): create by joining the last bit of 8 byte of the key, if the key is greater than 8, cut it off or else padding bit 1 into the root number in order to have a byte ==> the new byte then will be module with 8, that the shift round value for the first byte.
	Each byte of the data will be shifted left (around) with the previous plain byte module with 8, except the first byte which is shifted left (around) with the "root number". The new output byte will xored with the key's byte by sequence.

	Change the new data into 6-bit-base byte => that the final cyphered text
	*/
	
	
	char FF = 0xFF;
	unsigned char rootNumber = 0x0;
	int key_len = getStrLength(key);
	//int data_len = getStrLength(xData);
	char* data = new char[data_len];
	for(int i=0;i<data_len;i++){
		data[i] = xData[i];
	}

	//create root key
	for(int i=0;i<key_len;i++){
		rootNumber ^= key[i];
	}
	
	rootNumber = rootNumber % 8;
	int preChar=0;
	int curChar=0;
	int key_step = 0;
	

	preChar = data[0];
	for(int j=0;j<rootNumber;j++) leftShift(data,0,7);
	data[0] = data[0] ^ key[key_step];

	
	for(int i=1;i<data_len;i++){
		curChar = preChar;
		preChar = data[i];
		for(int j=0;j<curChar%8;j++) leftShift(data,i*8,i*8+7);
		data[i] = data[i] ^ key[key_step];
		key_step++;
		if(key_step==key_len) key_step= 0;
		
	}
	//from this point, key_len does not mean length of key anymore, it become the length of retValue
	data_len = data_len*8;
	key_len = data_len/6;
	if(data_len%6>0) key_len++;
	
	//retValue = new char[key_len+1];
	for(int i=0;i<data_len;i++){
		if(i%6==0)retValue[i/6]=0;
		copyBit(data,i,retValue,i/6*8+2+i%6);
	}
	for(int i=0;i<key_len;i++){
		retValue[i] = base_6_bits[(unsigned char)retValue[i]];
	}
	retValue[key_len]=0;
	//data = retValue;
	delete data;
	return key_len;
}

int text_decode(const char* xData,int data_len,const char* key, char* retValue){
	char FF = 0xFF;
	unsigned char rootNumber = 0x0;
	int key_len = getStrLength(key);

	
	
	//int data_len = getStrLength(xData);
	char* data = new char[data_len];
	for(int i=0;i<data_len;i++){
		data[i] = xData[i];
	}
	
	for(int i=0;i<data_len;i++){
		data[i] = (unsigned char)findString(base_6_bits,data[i]);
	}
	data_len = data_len*6/8;

	//retValue = new char[data_len+1];
	data_len = data_len * 8;
	for(int i =0;i<data_len;i++){
		copyBit(data,i/6*8+2+i%6,retValue,i);
	}
	
	//create root key
	for(int i=0;i<key_len;i++){
		rootNumber ^= key[i];
	}
	
	rootNumber = rootNumber % 8;

	//decode
	int key_step = 0;

	data_len = data_len / 8;
	retValue[0] = retValue[0] ^ key[key_step];
	for(int j=0;j<rootNumber;j++) rightShift(retValue,0,7);
	

	
	for(int i=1;i<data_len;i++){

		retValue[i] = retValue[i] ^ key[key_step];
		for(int j=0;j<retValue[i-1]%8;j++) rightShift(retValue,i*8,i*8+7);
		key_step++;
		if(key_step==key_len) key_step= 0;
		
		
	}
	retValue[data_len]=0;
	//data = retValue;
	delete data;
	return data_len;
}
int _stdcall cryptFileAsText(char* inFile,char* pwd,bool isCrypted)
{
    
	//long lngPwd = getStrLength(pwd);
	HANDLE hInFile;
	HANDLE hOutFile;
	DWORD  dwBytesRead, dwBytesWritten;
	
    char* outFile = new char[_MAX_PATH];
	hInFile = CreateFile(inFile,     // open inFile
    GENERIC_READ,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hInFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		// process error
		return 1;
	}
	
	
	

	GetTempPath(_MAX_PATH,outFile);
	strcat(outFile,"~cfat.tmp");
  
	hOutFile = CreateFile(outFile,     // open inFile
    GENERIC_READ|GENERIC_WRITE,                 // open for writing 
    0,                            // do not share 
    NULL,                         // no security 
    CREATE_ALWAYS,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hOutFile == INVALID_HANDLE_VALUE) 
	{
        delete outFile;
		CloseHandle(hOutFile); 
		// process error
		return 2;
	}


	//Starting crypt File here///////////////////////////////////////
	char* buff=new char [MAX_BUFFER];
	char* buffEn=new char [MAX_BUFFER];

	do 
	{
		
		if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{
            if(isCrypted) dwBytesWritten = text_encode(buff,dwBytesRead,pwd, buffEn); 
            else dwBytesWritten = text_decode(buff,dwBytesRead,pwd, buffEn); 
			
			//////////////////////
			WriteFile(hOutFile, buffEn, dwBytesWritten, &dwBytesWritten, NULL);
 
		}
	} while (dwBytesRead == MAX_BUFFER); 

	/////////////////////////////////////////////////////////////////
    CloseHandle(hOutFile);
	CloseHandle(hInFile);
	
	delete []buff;
	delete []buffEn;
    CopyFile(outFile,inFile,false);
    DeleteFile(outFile);
    delete []outFile;
	return 0;

}
int _stdcall cryptFile(char *inFile,char *pwd,bool isCrypted)
{
	long lngPwd = getStrLength(pwd);
	HANDLE hInFile;
	DWORD i=0;
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	

	hInFile = CreateFile(inFile,     // open inFile
    GENERIC_READ|GENERIC_WRITE,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hInFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		// process error
		return 1;
	}


	//Starting crypt File here///////////////////////////////////////
	int j=0;
	char *buff;
	buff=new char [MAX_BUFFER];
	long byteCount = 0;
	char* pByteCount;
	pByteCount = (char*)&byteCount;
	do 
	{
		
		if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{ 
			//crypt here
			for (i=0;i<dwBytesRead;i++)
			{
                byteCount++;
                if(isCrypted){
                    rotateBit(buff[i]);
                    leftShift(&buff[i],0,7);
                    buff[i] = buff[i]^pByteCount[0];
                    buff[i] = buff[i]^pByteCount[1];
                    buff[i] = buff[i]^pByteCount[2];
                    buff[i] = buff[i]^pByteCount[3];
    				if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
                }
                else{
                    
    				if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
                    buff[i] = buff[i]^pByteCount[0];
                    buff[i] = buff[i]^pByteCount[1];
                    buff[i] = buff[i]^pByteCount[2];
                    buff[i] = buff[i]^pByteCount[3];
                    rightShift(&buff[i],0,7);
                    rotateBit(buff[i]);
                }
				j++;
				if (j>=lngPwd) j=0;
			  	
			}
			//////////////////////
			dwPos = SetFilePointer(hInFile,0-dwBytesRead, NULL, FILE_CURRENT); 
			WriteFile(hInFile, buff, dwBytesRead, 
				&dwBytesWritten, NULL);
 
		}
	} while (dwBytesRead == MAX_BUFFER); 
	/////////////////////////////////////////////////////////////////

	CloseHandle(hInFile);
	delete []buff;
	return 0;

}
int _stdcall embedFile(char *frontFile,char * behideFile, char * behideFileName,char * pwd)
{
	long lngPwd=getStrLength(pwd);
	//crypt behideFile
	//Add behideFile after frontFile
	HANDLE hFile; 
 
	HANDLE hAppend; 
 
	DWORD  dwBytesRead, dwBytesWritten, dwPos; 
 
	
 
	// Open the existing file. 
 
	hFile = CreateFile(behideFile,     // open ONE.TXT 
		GENERIC_READ,                 // open for reading 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_EXISTING,                // existing file only 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		// process error
		
		return 1;
	} 
	DWORD fileLng=GetFileSize(hFile,0);
 
	// Open the existing file, or if the file does not exist, 
	// create a new file. 
 
	hAppend = CreateFile(frontFile,   // open TWO.TXT 
		GENERIC_WRITE,                // open for writing 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_ALWAYS,                  // open or create 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hAppend == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		CloseHandle(hAppend);
		
		return 2; 
	} 
 
	// Append the first file to the end of the second file. 
	// Lock the second file to prevent another process from 
	// accessing it while writing to it. Unlock the 
	// file when writing is finished. 
	char *buff;
	buff=new char [MAX_BUFFER];
	int j=0;
	long byteCount = 0;
	char* pByteCount;
	pByteCount = (char*)&byteCount;
	do 
	{ 
		if (ReadFile(hFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{ 
			dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END); 
			LockFile(hAppend, dwPos, 0, dwPos + dwBytesRead, 0);

			//crypt here
			for (DWORD i=0;i<dwBytesRead;i++)
			{
                byteCount++;
				rotateBit(buff[i]);
                leftShift(&buff[i],0,7);
                buff[i] = buff[i]^pByteCount[0];
                buff[i] = buff[i]^pByteCount[1];
                buff[i] = buff[i]^pByteCount[2];
                buff[i] = buff[i]^pByteCount[3];
				if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
				j++;
				if (j>=lngPwd) j=0;
			}
			//////////////////////
			WriteFile(hAppend, buff, dwBytesRead, 
				&dwBytesWritten, NULL); 
			UnlockFile(hAppend, dwPos, 0, dwPos + dwBytesRead, 0); 
		} 
	} while (dwBytesRead == MAX_BUFFER); 
	delete []buff;
	// Close behideFile First;
	CloseHandle(hFile);
	DeleteFile(behideFile);
	//Add information about the tag file
	char tag[MAX_CHAR];
	tag[0]=0;
	addStr(tag,"<>FN");
	addStr(tag,behideFileName);
	addStr(tag,"</FN>");
	addStr(tag,"<FL>");
	addStr(tag,longToChar(fileLng));
	addStr(tag,"</FL>");
	//dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END); //set current pointer
	//crypt tag
	j=0;
	long tagLen=getStrLength(tag);
	for (DWORD i=0;i<(DWORD)tagLen;i++)
	{
		tag[i]=tag[i]^pwd[j];
		j++;
		if (j>=lngPwd) j=0;
	}

	///////////
	LockFile(hAppend, dwPos, 0, dwPos + tagLen+1, 0); //
	WriteFile(hAppend, tag, tagLen, 
		&dwBytesWritten, NULL);
	tag[0]=(char)tagLen;
	tag[1]=0;
	WriteFile(hAppend, tag, 1, 
		&dwBytesWritten, NULL);
	UnlockFile(hAppend, dwPos, 0, dwPos + tagLen+1, 0);
	CloseHandle(hAppend); 
	return 0;

}
int _stdcall splitEmbedFile(char *embedFile,char * outPath,char * pwd, bool split,char *outFileName)
{
	long lngPwd=getStrLength(pwd);
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	HANDLE hFile = CreateFile(embedFile,     // open ONE.TXT 
		GENERIC_READ|GENERIC_WRITE,                 // open for reading and writing 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_EXISTING,                // existing file only 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile);
		// process error
		
		return 1;
	}
	dwPos = SetFilePointer(hFile, -1, NULL, FILE_END); //set current pointer to End
	char offset;
	char tag[MAX_CHAR];
	ReadFile(hFile, &offset, 1, &dwBytesRead, NULL);// read the last byte
	if (offset <0) //error on file
	{ 
		CloseHandle(hFile);	
		return 2;
	}
	dwPos = SetFilePointer(hFile, -(offset+1), NULL, FILE_CURRENT);
	ReadFile(hFile, tag, offset, &dwBytesRead, NULL);
	tag[dwBytesRead]=0;
	// crypt tag
	int j=0;
	DWORD i;
	for (i=0;i<(DWORD)offset;i++)
	{
		tag[i]=tag[i]^pwd[j];
		j++;
		if (j>=lngPwd) j=0;
	}
	//get size of behide file
	char len[MAX_CHAR];
	len[0]=0;
	splitXML(tag,"<FL>","</FL>",len);
	long fileLen=0;
	for(i=0;i<getStrLength(len);i++)
	{
		fileLen=(len[i]-48)*lt(10,i)+fileLen;
	}
	//get name of behide file
	len[0]=0;
	splitXML(tag,"<FN>","</FN>",len);
	//create hanlde for out file
	char outFile[MAX_CHAR];
	outFile[0]=0;
	addStr(outFile,outPath);
	addStr(outFile,"/");
	addStr(outFile,len);
	HANDLE hOutFile = CreateFile(outFile,   // open outFile 
    GENERIC_WRITE,                // open for writing 
    0,                            // do not share 
    NULL,                         // no security 
    CREATE_ALWAYS,                  // open or create 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hOutFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		CloseHandle(hOutFile);
		return 3;
	}
	//put cursor at the begin of the behide file
	dwPos = SetFilePointer(hFile, -(fileLen+offset), NULL, FILE_CURRENT);
	//Read and write file
	char *buff;
	buff=new char [MAX_BUFFER];
	j=0;
	long byteCount = 0;
	char* pByteCount;
	pByteCount = (char*)&byteCount;
	do 
	{
		
		if (ReadFile(hFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{ 
			dwPos = SetFilePointer(hOutFile, 0, NULL, FILE_END); 
			LockFile(hOutFile, dwPos, 0, dwPos + dwBytesRead, 0);
			//crypt here
			
			for (i=0;i<dwBytesRead;i++)
			{
                byteCount++;
				if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
				buff[i] = buff[i]^pByteCount[0];
                buff[i] = buff[i]^pByteCount[1];
                buff[i] = buff[i]^pByteCount[2];
                buff[i] = buff[i]^pByteCount[3];
                rightShift(&buff[i],0,7);
                rotateBit(buff[i]);
				j++;
				if (j>=lngPwd) j=0;
			}
			//////////////////////
			WriteFile(hOutFile, buff, dwBytesRead, 
				&dwBytesWritten, NULL); 
			UnlockFile(hOutFile, dwPos, 0, dwPos + dwBytesRead, 0); 
		} 
	} while (dwBytesRead == MAX_BUFFER); 
	delete []buff;
	dwPos = SetFilePointer(hOutFile, -(offset+1), NULL, FILE_CURRENT);
	SetEndOfFile(hOutFile);
	/////////////////////////////////////////////////////////////////

	CloseHandle(hOutFile);
	////split embed file out of front file
	if (split) 
	{
		dwPos = SetFilePointer(hFile, -(fileLen+offset+1), NULL, FILE_CURRENT);
		SetEndOfFile(hFile);
	}
	CloseHandle(hFile);
	for(i=0;i<getStrLength(len);i++)
	{
		outFileName[i]=len[i];
	}
	outFileName[i]=0;
	return 0;
}
int _stdcall cryptFileByFile(char *inFile,char *pwdFile,bool isCrypted)
{
	HANDLE hInFile,hPwdFile;
	DWORD i=0;
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	DWORD  dwPwdBytesRead, dwPwdPos;
	

	hInFile = CreateFile(inFile,     // open inFile
    GENERIC_READ|GENERIC_WRITE,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hInFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		// process error
		
		return 1;
	}

	hPwdFile = CreateFile(pwdFile,     // open inFile
    GENERIC_READ,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hPwdFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		CloseHandle(hPwdFile);
		// process error
		
		return 2;
	}
	//Starting crypt File here///////////////////////////////////////
	DWORD j;
	char *buff;
	char *buffPwd;

	buff=new char [MAX_BUFFER];
	buffPwd=new char [MAX_BUFFER];
	if (GetFileSize(hInFile,0)>GetFileSize(hPwdFile,0))
	{
		//pwdFile is smaller
		do 
		{
			
			if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
			{
				i=0;
				do
				{
					ReadFile(hPwdFile, buffPwd, MAX_BUFFER, &dwPwdBytesRead, NULL);
					//crypt here
					if(dwPwdBytesRead!=MAX_BUFFER) 
						dwPwdPos = SetFilePointer(hPwdFile,0, NULL, FILE_BEGIN);
					for (j=0;j<dwPwdBytesRead;j++)
					{
                        if(i<dwBytesRead&&buff[i]!=0){
                            if(isCrypted){
                                rotateBit(buff[i]);
                                leftShift(&buff[i],0,7);
                				if(buff[i]!=0 && buff[i]!=buffPwd[j]) buff[i]=buff[i]^buffPwd[j];
                				
                            }
                            else{
                                
                				if(buff[i]!=0 && buff[i]!=buffPwd[j]) buff[i]=buff[i]^buffPwd[j];
                                rightShift(&buff[i],0,7);
                                rotateBit(buff[i]);
                            }
                        }
						i++;
					}

				}while(i<dwBytesRead);
				
				
				//////////////////////
				dwPos = SetFilePointer(hInFile,0-dwBytesRead, NULL, FILE_CURRENT); 
				WriteFile(hInFile, buff, dwBytesRead, 
					&dwBytesWritten, NULL);
			}
		} while (dwBytesRead == MAX_BUFFER);

	}
	else
	{
		//pwdFile is bigger
		do 
		{
			
			if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
			{
				ReadFile(hPwdFile, buffPwd, MAX_BUFFER, &dwPwdBytesRead, NULL);
				//crypt here
				for (i=0;i<dwBytesRead;i++)
				{
                    if(isCrypted){
                        rotateBit(buff[i]);
                        leftShift(&buff[i],0,7);
                        buff[i]=buff[i]^buffPwd[i];
                    }
                    else{
                        buff[i]=buff[i]^buffPwd[i];
                        rightShift(&buff[i],0,7);
                        rotateBit(buff[i]);
                    }
				}
				
				//////////////////////
				dwPos = SetFilePointer(hInFile,0-dwBytesRead, NULL, FILE_CURRENT); 
				WriteFile(hInFile, buff, dwBytesRead, 
					&dwBytesWritten, NULL);
			}
		} while (dwBytesRead == MAX_BUFFER);

	}
	 
	/////////////////////////////////////////////////////////////////
	delete []buff;
	delete []buffPwd;
	CloseHandle(hInFile); 
	CloseHandle(hPwdFile);
	return 0;
}
//old version
int _stdcall cryptFileOld(char *inFile,char *pwd,bool isCrypted)
{
	long lngPwd = getStrLength(pwd);
	HANDLE hInFile;
	DWORD i=0;
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	

	hInFile = CreateFile(inFile,     // open inFile
    GENERIC_READ|GENERIC_WRITE,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hInFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		// process error
		return 1;
	}


	//Starting crypt File here///////////////////////////////////////
	int j=0;
	char *buff;
	buff=new char [MAX_BUFFER];
	do 
	{
		
		if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{ 
			//crypt here
			for (i=0;i<dwBytesRead;i++)
			{
                if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
				j++;
				if (j>=lngPwd) j=0;
			  	
			}
			//////////////////////
			dwPos = SetFilePointer(hInFile,0-dwBytesRead, NULL, FILE_CURRENT); 
			WriteFile(hInFile, buff, dwBytesRead, 
				&dwBytesWritten, NULL);
 
		}
	} while (dwBytesRead == MAX_BUFFER); 
	/////////////////////////////////////////////////////////////////

	CloseHandle(hInFile);
	delete []buff;
	return 0;

}
int _stdcall embedFileOld(char *frontFile,char * behideFile, char * behideFileName,char * pwd)
{
	long lngPwd=getStrLength(pwd);
	//crypt behideFile
	//Add behideFile after frontFile
	HANDLE hFile; 
 
	HANDLE hAppend; 
 
	DWORD  dwBytesRead, dwBytesWritten, dwPos; 
 
	
 
	// Open the existing file. 
 
	hFile = CreateFile(behideFile,     // open ONE.TXT 
		GENERIC_READ,                 // open for reading 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_EXISTING,                // existing file only 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		// process error
		
		return 1;
	} 
	DWORD fileLng=GetFileSize(hFile,0);
 
	// Open the existing file, or if the file does not exist, 
	// create a new file. 
 
	hAppend = CreateFile(frontFile,   // open TWO.TXT 
		GENERIC_WRITE,                // open for writing 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_ALWAYS,                  // open or create 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hAppend == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		CloseHandle(hAppend);
		
		return 2; 
	} 
 
	// Append the first file to the end of the second file. 
	// Lock the second file to prevent another process from 
	// accessing it while writing to it. Unlock the 
	// file when writing is finished. 
	char *buff;
	buff=new char [MAX_BUFFER];
	int j=0;
	do 
	{ 
		if (ReadFile(hFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{ 
			dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END); 
			LockFile(hAppend, dwPos, 0, dwPos + dwBytesRead, 0);

			//crypt here
			for (DWORD i=0;i<dwBytesRead;i++)
			{
				if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
				j++;
				if (j>=lngPwd) j=0;
			}
			//////////////////////
			WriteFile(hAppend, buff, dwBytesRead, 
				&dwBytesWritten, NULL); 
			UnlockFile(hAppend, dwPos, 0, dwPos + dwBytesRead, 0); 
		} 
	} while (dwBytesRead == MAX_BUFFER); 
	delete []buff;
	// Close behideFile First;
	CloseHandle(hFile);
	DeleteFile(behideFile);
	//Add information about the tag file
	char tag[MAX_CHAR];
	tag[0]=0;
	addStr(tag,"<FN>");
	addStr(tag,behideFileName);
	addStr(tag,"</FN>");
	addStr(tag,"<FL>");
	addStr(tag,longToChar(fileLng));
	addStr(tag,"</FL>");
	//dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END); //set current pointer
	//crypt tag
	j=0;
	long tagLen=getStrLength(tag);
	for (DWORD i=0;i<(DWORD)tagLen;i++)
	{
		tag[i]=tag[i]^pwd[j];
		j++;
		if (j>=lngPwd) j=0;
	}

	///////////
	LockFile(hAppend, dwPos, 0, dwPos + tagLen+1, 0); //
	WriteFile(hAppend, tag, tagLen, 
		&dwBytesWritten, NULL);
	tag[0]=(char)tagLen;
	tag[1]=0;
	WriteFile(hAppend, tag, 1, 
		&dwBytesWritten, NULL);
	UnlockFile(hAppend, dwPos, 0, dwPos + tagLen+1, 0);
	CloseHandle(hAppend); 
	return 0;

}
int _stdcall splitEmbedFileOld(char *embedFile,char * outPath,char * pwd, bool split,char *outFileName)
{
	long lngPwd=getStrLength(pwd);
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	HANDLE hFile = CreateFile(embedFile,     // open ONE.TXT 
		GENERIC_READ|GENERIC_WRITE,                 // open for reading and writing 
		0,                            // do not share 
		NULL,                         // no security 
		OPEN_EXISTING,                // existing file only 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);                        // no attr. template 

	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile);
		// process error
		
		return 1;
	}
	dwPos = SetFilePointer(hFile, -1, NULL, FILE_END); //set current pointer to End
	char offset;
	char tag[MAX_CHAR];
	ReadFile(hFile, &offset, 1, &dwBytesRead, NULL);// read the last byte
	if (offset <0) //error on file
	{ 
		CloseHandle(hFile);	
		return 2;
	}
	dwPos = SetFilePointer(hFile, -(offset+1), NULL, FILE_CURRENT);
	ReadFile(hFile, tag, offset, &dwBytesRead, NULL);
	tag[dwBytesRead]=0;
	// crypt tag
	int j=0;
	DWORD i;
	for (i=0;i<(DWORD)offset;i++)
	{
		tag[i]=tag[i]^pwd[j];
		j++;
		if (j>=lngPwd) j=0;
	}
	//get size of behide file
	char len[MAX_CHAR];
	len[0]=0;
	splitXML(tag,"<FL>","</FL>",len);
	long fileLen=0;
	for(i=0;i<getStrLength(len);i++)
	{
		fileLen=(len[i]-48)*lt(10,i)+fileLen;
	}
	//get name of behide file
	len[0]=0;
	splitXML(tag,"<FN>","</FN>",len);
	//create hanlde for out file
	char outFile[MAX_CHAR];
	outFile[0]=0;
	addStr(outFile,outPath);
	addStr(outFile,"/");
	addStr(outFile,len);
	HANDLE hOutFile = CreateFile(outFile,   // open outFile 
    GENERIC_WRITE,                // open for writing 
    0,                            // do not share 
    NULL,                         // no security 
    CREATE_ALWAYS,                  // open or create 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hOutFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hFile); 
		CloseHandle(hOutFile);
		return 3;
	}
	//put cursor at the begin of the behide file
	dwPos = SetFilePointer(hFile, -(fileLen+offset), NULL, FILE_CURRENT);
	//Read and write file
	char *buff;
	buff=new char [MAX_BUFFER];
	j=0;
	do 
	{
		
		if (ReadFile(hFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
		{ 
			dwPos = SetFilePointer(hOutFile, 0, NULL, FILE_END); 
			LockFile(hOutFile, dwPos, 0, dwPos + dwBytesRead, 0);
			//crypt here
			
			for (i=0;i<dwBytesRead;i++)
			{
				if(buff[i]!=0 && buff[i]!=pwd[j]) buff[i]=buff[i]^pwd[j];
				j++;
				if (j>=lngPwd) j=0;
			}
			//////////////////////
			WriteFile(hOutFile, buff, dwBytesRead, 
				&dwBytesWritten, NULL); 
			UnlockFile(hOutFile, dwPos, 0, dwPos + dwBytesRead, 0); 
		} 
	} while (dwBytesRead == MAX_BUFFER); 
	delete []buff;
	dwPos = SetFilePointer(hOutFile, -(offset+1), NULL, FILE_CURRENT);
	SetEndOfFile(hOutFile);
	/////////////////////////////////////////////////////////////////

	CloseHandle(hOutFile);
	////split embed file out of front file
	if (split) 
	{
		dwPos = SetFilePointer(hFile, -(fileLen+offset+1), NULL, FILE_CURRENT);
		SetEndOfFile(hFile);
	}
	CloseHandle(hFile);
	for(i=0;i<getStrLength(len);i++)
	{
		outFileName[i]=len[i];
	}
	outFileName[i]=0;
	return 0;
}
int _stdcall cryptFileByFileOld(char *inFile,char *pwdFile,bool isCrypted)
{
	HANDLE hInFile,hPwdFile;
	DWORD i=0;
	DWORD  dwBytesRead, dwBytesWritten, dwPos;
	DWORD  dwPwdBytesRead, dwPwdPos;
	

	hInFile = CreateFile(inFile,     // open inFile
    GENERIC_READ|GENERIC_WRITE,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hInFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		// process error
		
		return 1;
	}

	hPwdFile = CreateFile(pwdFile,     // open inFile
    GENERIC_READ,                 // open for reading 
    0,                            // do not share 
    NULL,                         // no security 
    OPEN_EXISTING,                // existing file only 
    FILE_ATTRIBUTE_NORMAL,        // normal file 
    NULL);                        // no attr. template 

	if (hPwdFile == INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(hInFile); 
		CloseHandle(hPwdFile);
		// process error
		
		return 2;
	}
	//Starting crypt File here///////////////////////////////////////
	DWORD j;
	char *buff;
	char *buffPwd;

	buff=new char [MAX_BUFFER];
	buffPwd=new char [MAX_BUFFER];
	if (GetFileSize(hInFile,0)>GetFileSize(hPwdFile,0))
	{
		//pwdFile is smaller
		do 
		{
			
			if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
			{
				i=0;
				do
				{
					ReadFile(hPwdFile, buffPwd, MAX_BUFFER, &dwPwdBytesRead, NULL);
					//crypt here
					if(dwPwdBytesRead!=MAX_BUFFER) 
						dwPwdPos = SetFilePointer(hPwdFile,0, NULL, FILE_BEGIN);
					for (j=0;j<dwPwdBytesRead;j++)
					{
                        if(buff[i]!=0 && buff[i]!=buffPwd[j]) buff[i]=buff[i]^buffPwd[j];
						i++;
					}

				}while(i<dwBytesRead);
				
				
				//////////////////////
				dwPos = SetFilePointer(hInFile,0-dwBytesRead, NULL, FILE_CURRENT); 
				WriteFile(hInFile, buff, dwBytesRead, 
					&dwBytesWritten, NULL);
			}
		} while (dwBytesRead == MAX_BUFFER);

	}
	else
	{
		//pwdFile is bigger
		do 
		{
			
			if (ReadFile(hInFile, buff, MAX_BUFFER, &dwBytesRead, NULL)) 
			{
				ReadFile(hPwdFile, buffPwd, MAX_BUFFER, &dwPwdBytesRead, NULL);
				//crypt here
				for (i=0;i<dwBytesRead;i++)
				{
                    buff[i]=buff[i]^buffPwd[i];
				}
				
				//////////////////////
				dwPos = SetFilePointer(hInFile,0-dwBytesRead, NULL, FILE_CURRENT); 
				WriteFile(hInFile, buff, dwBytesRead, 
					&dwBytesWritten, NULL);
			}
		} while (dwBytesRead == MAX_BUFFER);

	}
	 
	/////////////////////////////////////////////////////////////////
	delete []buff;
	delete []buffPwd;
	CloseHandle(hInFile); 
	CloseHandle(hPwdFile);
	return 0;
}
