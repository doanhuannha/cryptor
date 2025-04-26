//HWND* phwnd = reinterpret_cast<HWND*>(lpParameter);
HWND hwnd = _hWnd;
char* workingFile = new char[_MAX_PATH];
char* password = new char[255];
char* keyFile = new char[_MAX_PATH];
char* maskFile = new char[_MAX_PATH];
SetEnable(false);
ParseOpt();

char* logMsg;
char* logFile =new char[_MAX_PATH];
GetTempPath(_MAX_PATH,logFile);
strcat(logFile,"~cryptorLogFile.txt");
if(_isUsePwd){ 
     GetWindowText(_hTxtPwd,password,255);
     if(strlen(password)==0) 
          if(_isOldVersion) strcpy(password," ");
          else strcpy(password,"!@#$%^&*()<>?:");
}//use password
else {GetWindowText(_hTxtKeyFile,keyFile,_MAX_PATH);}//use key file
if(_isUseMask){ GetWindowText(_hTxtMaskFile,maskFile,_MAX_PATH);}
if(ValidateInput(hwnd, keyFile, maskFile)){ _isProcessing = false; return SetEnable(true);}
int answer;
if(_isEncrypt){
    answer = MessageBox(hwnd, "Your choose **ENCRYPT** mode.\nAre you sure?", "Confirm Message", MB_YESNO | MB_ICONWARNING);
}
else{
    answer = MessageBox(hwnd, "Your choose **DECRYPT** mode.\nAre you sure?", "Confirm Message", MB_YESNO | MB_ICONWARNING);
}
if(answer!=IDYES) { _isProcessing = false; return SetEnable(true);}
SetWindowText(_hWnd,WND_TITLE_PROCESS);
int err=0;
if(_isMsgMode){ //text message                      
    wchar_t* wBuffer = new wchar_t[MSG_MAX_LENGTH];
    char* buffer = new char[2*MSG_MAX_LENGTH];
    char* out_buffer = new char[2*MSG_MAX_LENGTH];
    int msgLength = GetWindowTextW(_hTxtMsg,wBuffer,MSG_MAX_LENGTH);
    msgLength = WideCharToMultiByte(CP_UTF8,0, wBuffer, msgLength, buffer, MSG_MAX_LENGTH,NULL,NULL);
    buffer[msgLength]=0;
    if(_isEncrypt){
        text_encode(buffer,msgLength,password, out_buffer);
    }
    else{
        text_decode(buffer,msgLength,password, out_buffer);
    }
    delete wBuffer;
    wBuffer = new wchar_t[MSG_MAX_LENGTH];
    msgLength = MultiByteToWideChar(CP_UTF8,0, out_buffer,strlen(out_buffer),wBuffer,MSG_MAX_LENGTH);
    wBuffer[msgLength]=0;
    SetWindowTextW(_hTxtMsg,wBuffer);
    delete buffer;
    delete out_buffer;
}
else{
    int index = 0;
    bool isOK;
    LRESULT ret;
    do{
       ret = SendMessage(_hListInputFiles.hwnd,(UINT)LB_GETTEXT,(WPARAM)index,(LPARAM)workingFile);
       if(ret!=LB_ERR){
            //
            if(_isEncrypt){ //encrypt
                if(_isUsePwd){ //use password
                    if(_isUseMask){ //with mask                                               
                         char* tempMask = new char[_MAX_PATH];
                         GetTempPath(_MAX_PATH,tempMask);
                         strcat(tempMask,"~mask_file.tmp");
                         CopyFile(maskFile,tempMask,false);
                         if(_isOldVersion) isOK = embedFileOld(tempMask,workingFile,strrchr(workingFile,'\\') + 1,password)==0;
                         else isOK = embedFile(tempMask,workingFile,strrchr(workingFile,'\\') + 1,password)==0;
                         if(isOK){
                             char* newFileName = new char[_MAX_PATH];
                             int foundIndex = findLastString(workingFile,'.');
                             bytecpy(newFileName, workingFile,0,foundIndex);
                             newFileName[foundIndex]='\0';
                             strcat(newFileName,".");
                             strcat(newFileName,strrchr(maskFile,'.') + 1);
                             CopyFile(tempMask,newFileName,false);
                             delete newFileName;
                         }
                         else{
                              writeLog(logFile,"Can not process file: ",false);
                              writeLog(logFile,workingFile,true);
                              err++;
                         }
                         DeleteFile(tempMask);
                         delete tempMask;
                        
                    }
                    else if(_isTextCrypt){
                         isOK = cryptFileAsText(workingFile,password,true)==0;
                         if(!isOK){
                              writeLog(logFile,"Can not process file: ",false);
                              writeLog(logFile,workingFile,true);
                              err++;
                         }
                    }      
                    else{
                         if(_isOldVersion) isOK = cryptFileOld(workingFile,password,true)==0;
                         else isOK = cryptFile(workingFile,password,true)==0;
                         if(!isOK){
                              writeLog(logFile,"Can not process file: ",false);
                              writeLog(logFile,workingFile,true);
                              err++;
                         }
                    }
                }              
                else{//use key file
                    if(_isOldVersion) isOK = cryptFileByFileOld(workingFile,keyFile,true)==0;
                    else isOK = cryptFileByFile(workingFile,keyFile,true)==0;
                    if(!isOK){
                          writeLog(logFile,"Can not process file: ",false);
                          writeLog(logFile,workingFile,true);
                          err++;
                     }
                
                }
            }
            else{ //decrypt
                if(_isMsgMode){ //text message 
                	//???
                }           
                else if(_isUsePwd){ //use password
                    if(_isUseMask){ //with mask
						char* tempPath = new char[_MAX_PATH];
						char* outFileName = new char[_MAX_PATH];
						GetTempPath(_MAX_PATH,tempPath);
						strcat(tempPath,"mask_file\\");
						CreateDirectory(tempPath,NULL);
						if(_isOldVersion) isOK = splitEmbedFileOld(workingFile,tempPath,password,false,outFileName)==0;
						else isOK = splitEmbedFile(workingFile,tempPath,password,false,outFileName)==0;
						
						if(isOK){
						  strcat(tempPath,outFileName);
						  char* newFileName = new char[_MAX_PATH];
						  int foundIndex = findLastString(workingFile,'\\');
						  bytecpy(newFileName, workingFile,0,foundIndex+1);
						  newFileName[foundIndex+1]='\0';
						  strcat(newFileName,outFileName);
						  CopyFile(tempPath,newFileName,false);
						  DeleteFile(tempPath);
						  if(strcmp(newFileName,workingFile)!=0) DeleteFile(workingFile);
						  delete newFileName;
						}
						else{
						  writeLog(logFile,"Can not process file: ",false);
						  writeLog(logFile,workingFile,true);
						  err++;
						}
						GetTempPath(_MAX_PATH,tempPath);
						strcat(tempPath,"mask_file\\");
						RemoveDirectory(tempPath);
						delete tempPath;
						delete outFileName;	
                    }
                    else if(_isTextCrypt){
                         isOK = cryptFileAsText(workingFile,password,false)==0;
                         if(!isOK){
                              writeLog(logFile,"Can not process file: ",false);
                              writeLog(logFile,workingFile,true);
                              err++;
                         }
                    }         
                    else{
                         if(_isOldVersion) isOK = cryptFileOld(workingFile,password,false)==0;
                         else isOK = cryptFile(workingFile,password,false)==0;
                         if(!isOK){
                              writeLog(logFile,"Can not process file: ",false);
                              writeLog(logFile,workingFile,true);
                              err++;
                         }
                    }
                }              
                else{ //use key file
                    if(_isOldVersion) isOK = cryptFileByFileOld(workingFile,keyFile,false)==0;
                    else isOK = cryptFileByFile(workingFile,keyFile,false)==0;
                    if(!isOK){
                          writeLog(logFile,"Can not process file: ",false);
                          writeLog(logFile,workingFile,true);
                          err++;
                    }
                } 
            }
       }
       index++;
    } while(ret!=LB_ERR);
}
delete workingFile;
delete password;
delete keyFile;
delete maskFile;

if(err==0){
     MessageBox(hwnd, "Done", "Info Message", MB_OK | MB_ICONINFORMATION);
}
else{
     char* errmsg = new char[512];
     strcpy(errmsg,"Error!! View details at:\r\n");
     strcat(errmsg,logFile);
     MessageBox(hwnd, errmsg, "Info Message", MB_OK | MB_ICONINFORMATION);
     delete errmsg;
}
delete logFile;
_isProcessing = false;
SetWindowText(_hWnd,WND_TITLE);
return SetEnable(true);
