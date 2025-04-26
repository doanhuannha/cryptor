switch (WPARAM(wParam)){
        case BT_CANCEL:
             {
                 if(!_isProcessing) PostQuitMessage (0);
             }
             break;
        case OPT_TXTMSG:
             {
                 SetEnable(true);
                 if(_isMsgMode){
                      ShowWindow(_hTxtMsg, SW_SHOW);
                      ShowWindow(_hListInputFiles.hwnd, SW_HIDE);
                      ShowWindow(_hBtBrowseFile1.hwnd, SW_HIDE);
                      
                      
                 }
                 else{
                      ShowWindow(_hTxtMsg, SW_HIDE);
                      ShowWindow(_hListInputFiles.hwnd, SW_SHOW);
                      ShowWindow(_hBtBrowseFile1.hwnd, SW_SHOW);
                 }
                 
             }
             break;
        case OPT_ASSOCIATEFILES:
             {
                                
                   char* keyValue = new char[_MAX_PATH+3];
                   GetModuleFileName(NULL,keyValue,_MAX_PATH);
                   strcat(keyValue," %L");
                   if(lParam==0xFF){
                       DWORD dwType;
                       LPBYTE lpData;
                       DWORD cbData;
                       lpData = new BYTE[_MAX_PATH];
                       cbData = _MAX_PATH;
                       RegRead(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\*\\shell\\Add to cryptor\\Command",NULL,&dwType,lpData,&cbData);
                       if(dwType==REG_SZ){
                           if(strcmp(keyValue,(char*)lpData)==0){//true
                                SendMessage(_hOptFileAss.hwnd,BM_SETCHECK,BST_CHECKED,0);
                           }
                       }
                       delete lpData;
                   }
                   else{
                        if(SendMessage(_hOptFileAss.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED){
                              RegWrite(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\*\\shell\\Add to cryptor\\Command",NULL,REG_SZ,(BYTE*)keyValue,strlen(keyValue)); 
                              RegWrite(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\Folder\\shell\\Add to cryptor\\Command",NULL,REG_SZ,(BYTE*)keyValue,strlen(keyValue)); 
                        }
                        else{
                              RegDelete(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\*\\shell\\Add to cryptor","Command");
                              RegDelete(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\*\\shell","Add to cryptor");
                              RegDelete(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\Folder\\shell\\Add to cryptor","Command");
                              RegDelete(HKEY_CURRENT_USER,"SOFTWARE\\Classes\\Folder\\shell","Add to cryptor");
                        }
                   }
                   delete keyValue;
                  
             }
             break;
        case BT_VIEW:
        	{
        		
				int fileCount = SendMessage(_hListInputFiles.hwnd, (UINT)LB_GETCOUNT, 0, 0);
				char** selectedFiles = new char*[fileCount];//max 128
				for (int i = 0; i < fileCount; ++i) {
				    selectedFiles[i] = new char[_MAX_PATH];
				    SendMessage(_hListInputFiles.hwnd,(UINT)LB_GETTEXT,(WPARAM)i,(LPARAM)selectedFiles[i]);
				}
				
				
				
				char* password = new char[255];
				GetWindowText(_hTxtPwd,password,255);
				//ImageWindow viewer(_hThisInstance, itemArray, count);
        		
             	
             	const char* className = "ImageViewerWindowClass";
			    WNDCLASS wc = {};
			    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(EYE_ICON));
			    wc.lpfnWndProc = ImageWindowProc;
			    wc.hInstance = _hThisInstance;//GetModuleHandle(NULL);
			    wc.lpszClassName = className;
			    //wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			    RegisterClass(&wc);
			    
			    
			
			    HWND hwnd = CreateWindowEx(
			        0,
			        className,
			        "View",
			        WS_OVERLAPPEDWINDOW,
			        CW_USEDEFAULT, CW_USEDEFAULT,
			        800, 600,
			        NULL, NULL,
			        _hThisInstance,
			        NULL
			    );
			    //ShowWindow(hwnd, SW_MAXIMIZE);
        		_viewer.Intialize(hwnd, selectedFiles, fileCount, password,_isOldVersion);							
			}
			break;
		case BT_OK:
             {
                   if(_isProcessing){ 
                       MessageBox(hwnd, "Thread is running", "Info Example", MB_OK | MB_ICONINFORMATION);
                       break;
                   }
                   _isProcessing = true;
                   DWORD threadId;
                   CreateThread(NULL, 0, ThreadProc, NULL, 0, &threadId);
                  
             }
             break;
        \
        case BT_SHOWPWD:
             {
               if(SendMessage(_hTxtPwd ,EM_GETPASSWORDCHAR,0,0) == '*')
                    SendMessage(_hTxtPwd,EM_SETPASSWORDCHAR,0,0);
               else
                    SendMessage(_hTxtPwd,EM_SETPASSWORDCHAR,(WPARAM)'*',0);
               SetFocus(_hTxtPwd);
               SetFocus(_hOptShowPwd.hwnd);
             }
             break;
        case BT_BROWSEFILES:
             {
                 OPENFILENAME ofn;
                 char szFile[_MAX_PATH]; 
                 HANDLE hf;
                 
                 ZeroMemory(&ofn, sizeof(ofn));
                 ofn.lStructSize = sizeof(ofn);
                 ofn.hwndOwner = hwnd;
                 ofn.lpstrFile = szFile;
                 ofn.nMaxFile = sizeof(szFile);
                 ofn.lpstrFilter = "All\0*.*\0\0";
                 ofn.nFilterIndex = 1;
                 ofn.lpstrFile[0] = '\0';
                 ofn.lpstrFileTitle = NULL;
                 ofn.nMaxFileTitle = 0;
                 ofn.lpstrInitialDir = NULL;
                 ofn.lpstrTitle = "Select files";
                 ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;
                 
                 if (GetOpenFileName(&ofn)){ 
                        //SetWindowText(_hTxtKeyFile,ofn.lpstrFile);
                        if(ofn.nFileOffset>strlen(ofn.lpstrFile)){
                            char* path = new char[MAX_CHAR];
                            char* fileName = &ofn.lpstrFile[ofn.nFileOffset];
                            while(*fileName)
                            {
                                strcpy(path,ofn.lpstrFile);
                                strcat(path,"\\");
                                strcat(path,fileName);
                                if(SendMessage(_hListInputFiles.hwnd,(UINT)LB_FINDSTRINGEXACT,(WPARAM)0,(LPARAM)path)==LB_ERR )
                                      SendMessage(_hListInputFiles.hwnd,(UINT)LB_ADDSTRING,(WPARAM)0,(LPARAM)path);
                                fileName += strlen(fileName) + 1;
                                
                            }
                            delete path;
                        }
                        else{
                             if(SendMessage(_hListInputFiles.hwnd,(UINT)LB_FINDSTRINGEXACT,(WPARAM)0,(LPARAM)ofn.lpstrFile)==LB_ERR )
                                      SendMessage(_hListInputFiles.hwnd,(UINT)LB_ADDSTRING,(WPARAM)0,(LPARAM)ofn.lpstrFile);
                        }
                        RefreshScrollList(_hListInputFiles);
                 }
             }
             break;
        case BT_BROWSEKEYFILE:
             {
                              
                 OPENFILENAME ofn;
                 char szFile[_MAX_PATH]; 
                 HANDLE hf;
                 
                 ZeroMemory(&ofn, sizeof(ofn));
                 ofn.lStructSize = sizeof(ofn);
                 ofn.hwndOwner = hwnd;
                 ofn.lpstrFile = szFile;
                 ofn.nMaxFile = sizeof(szFile);
                 ofn.lpstrFilter = "All\0*.*\0\0";
                 ofn.nFilterIndex = 1;
                 ofn.lpstrFile[0] = '\0';
                 ofn.lpstrFileTitle = NULL;
                 ofn.nMaxFileTitle = 0;
                 ofn.lpstrInitialDir = NULL;
                 ofn.lpstrTitle = "Select a key file";
                 ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                 if (GetOpenFileName(&ofn)){ 
                       SetWindowText(_hTxtKeyFile,ofn.lpstrFile);
                 }
             }
             break;
        case BT_BROWSEMASKFILE:
             {
                 OPENFILENAME ofn;
                 char szFile[_MAX_PATH]; 
                 HANDLE hf;
                 
                 ZeroMemory(&ofn, sizeof(ofn));
                 ofn.lStructSize = sizeof(ofn);
                 ofn.hwndOwner = hwnd;
                 ofn.lpstrFile = szFile;
                 ofn.nMaxFile = sizeof(szFile);
                 ofn.lpstrFilter = "All\0*.*\0\0";
                 ofn.nFilterIndex = 1;
                 ofn.lpstrFile[0] = '\0';
                 ofn.lpstrFileTitle = NULL;
                 ofn.nMaxFileTitle = 0;
                 ofn.lpstrInitialDir = NULL;
                 ofn.lpstrTitle = "Select a mask file";
                 ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                 if (GetOpenFileName(&ofn)){ 
                       SetWindowText(_hTxtMaskFile,ofn.lpstrFile);
                 }
             }
             break;
        case BT_MASKFILEOPT:
             if(SendMessage(_hOptMaskFile.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED){
                   SendMessage(_hOptTextFile.hwnd,BM_SETCHECK,BST_UNCHECKED,0);
             }
             SetEnable(true);
        case BT_CRYPTTEXTOPT:
             if(SendMessage(_hOptTextFile.hwnd, BM_GETCHECK, 0, 0)==BST_CHECKED){
                   SendMessage(_hOptMaskFile.hwnd,BM_SETCHECK,BST_UNCHECKED,0);
             }
             SetEnable(true);
             break;
             
        case BT_KEYFILEOPT:
        case BT_PWDOPT:
        case BT_ENCRYPTOPT:
        case BT_DECRYPTOPT:
        case BT_OLDVERSIONOPT:           
             SetEnable(true);
             break;
 }
