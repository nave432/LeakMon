// Callfunctions.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Callfunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "../../HookDll/HookDll.h"

#define VERIFY_LEAK(pVoid)\
    {\
        if(!IsLeakDetected(pVoid))\
        {\
            std::cout<<"Leak check failed "<<std::endl;\
        }\
    }

// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(NULL);

    if (hModule != NULL)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            _tprintf(_T("Fatal Error: MFC initialization failed\n"));
            nRetCode = 1;
        }
        else
        {
            // TODO: code your application's behavior here.
        }
    }
    else
    {
        // TODO: change error code to suit your needs
        _tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
        nRetCode = 1;
    }
    Sleep(5000);

    void* p = HeapAlloc(GetProcessHeap(), 0, 10 );
    VERIFY_LEAK(p);
    
    std::cout<<"Test Completed"<<std::endl;
    int n;
    std::cin>>n;
    return nRetCode;
}
