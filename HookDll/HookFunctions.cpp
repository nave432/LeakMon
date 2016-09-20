#include "stdafx.h"
#include "HookFunctions.h"
#include "Tlhelp32.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MakePtr(cast, ptr, AddValue) (cast)((DWORD_PTR)(ptr)+(DWORD_PTR)(AddValue))
void FindFunction( HANDLE hModule, PIMAGE_IMPORT_DESCRIPTOR pImportDesc, UINT uiCount, 
                    LPHOOKFUNCDESC paHookArray, bool bRestore )
{
    PIMAGE_THUNK_DATA pOrigThunk = MakePtr(PIMAGE_THUNK_DATA, hModule, pImportDesc->OriginalFirstThunk);

    // Get the array pointed to by the pImportDesc->FirstThunk.  This is
    //  where I will do the actual bash.
    PIMAGE_THUNK_DATA pRealThunk = MakePtr(PIMAGE_THUNK_DATA, hModule, pImportDesc->FirstThunk);

    // Loop through and look for the one that matches the name.
    while (pOrigThunk->u1.Function)
    {
        BOOL bDoHook = FALSE;
        UINT i = 0;

        // Only look at those that are imported by name, not ordinal.
        if (IMAGE_ORDINAL_FLAG != (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG))
        {
            if( paHookArray[i].szFunc )
            {
                // Look get the name of this imported function.
                PIMAGE_IMPORT_BY_NAME pByName = MakePtr(PIMAGE_IMPORT_BY_NAME,
                                                hModule, pOrigThunk->u1.AddressOfData );

                // If the name starts with NULL, then just skip out now.
                if( 0 == pByName->Name[0])
                    continue;

                for ( i = 0; i<uiCount; i++)
                {
                    if( strcmpi(paHookArray[i].szFunc, (char*)pByName->Name) == 0 )
                    {
                        PSTR szCurrMod = MakePtr(PSTR, hModule, pImportDesc->Name );
                        CString cs = szCurrMod;
                        if( 0 == cs.CompareNoCase( paHookArray[i].lpszDllName ))
                        {
                            // If the proc is NULL, kick out, otherwise go
                            //  ahead and hook it.
                            if (paHookArray[i].pProc)
                                bDoHook = TRUE;
                            break;
                        }
                        
                    }
                }
            }
        }
        else
        {
            if( paHookArray[i].dwOrdinal )
            {
                for ( i = 0; i<uiCount; i++)
                {
                    if ( paHookArray[i].dwOrdinal == ( pOrigThunk->u1.Ordinal & 0xFFFFFFF ))
                    {
                        PSTR szCurrMod = MakePtr(PSTR, hModule, pImportDesc->Name );
                        CString cs = szCurrMod;
                        if( 0 == cs.CompareNoCase( paHookArray[i].lpszDllName ))
                        {
                            // If the proc is NULL, kick out, otherwise go
                            //  ahead and hook it.
                            if ( paHookArray[i].dwOrdinal )
                                bDoHook = TRUE;
                            break;
                        }
                        
                    }
                }
            }
        }

        if (bDoHook)
        {
            // I found it.  Now I need to change the protection to
            //  writable before I do the blast.  Note that I am now
            //  blasting into the real thunk area!
            MEMORY_BASIC_INFORMATION mbi_thunk;
            VirtualQuery(pRealThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
            VERIFY(VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, PAGE_READWRITE, &mbi_thunk.Protect));

            // Do the actual hook.
            pRealThunk->u1.Function = (DWORD_PTR)( (bRestore)?paHookArray[i].pOrigProc:paHookArray[i].pProc);

            // Change the protection back to what it was before I blasted.
            DWORD dwOldProtect;
            VERIFY(VirtualProtect(mbi_thunk.BaseAddress, mbi_thunk.RegionSize, mbi_thunk.Protect, &dwOldProtect));
        }
        // Increment both tables.
        pOrigThunk++;
        pRealThunk++;
    }
}


////////////////// Implementation //////////////////////////////

PIMAGE_IMPORT_DESCRIPTOR GetNamedImportDescriptor(HMODULE hModule, LPCSTR szImportMod)
{
  // Always check parameters.
  ASSERT(hModule);
  if (hModule == NULL)
  {   
     ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return NULL;
  }

  // Get the Dos header.
  PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER) hModule;

  // Is this the MZ header?
  if (IsBadReadPtr(pDOSHeader, sizeof(IMAGE_DOS_HEADER)) || (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return NULL;
  }

  // Get the PE header.
  PIMAGE_NT_HEADERS pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader->e_lfanew);

  // Is this a real PE image?
  if (IsBadReadPtr(pNTHeader, sizeof(IMAGE_NT_HEADERS)) || (pNTHeader->Signature != IMAGE_NT_SIGNATURE))
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_INVALID_PARAMETER, SLE_ERROR);
    return NULL;
  }

  // If there is no imports section, leave now.
  if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0)
    return NULL;

  // Get the pointer to the imports section.
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDOSHeader,
                                                 pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].
                                                 VirtualAddress);

  if( 0 == szImportMod )
  {
      return pImportDesc;
  }
  
  // Loop through the import module descriptors looking for the module whose name matches szImportMod.
  while (pImportDesc->Name)
  {
    PSTR szCurrMod = MakePtr(PSTR, pDOSHeader, pImportDesc->Name);

    if (stricmp(szCurrMod, szImportMod) == 0)
      break; // Found it.

    // Look at the next one.
    pImportDesc++;
  }

  // If the name is NULL, then the module is not imported.
  if (pImportDesc->Name == NULL)
    return NULL;

  return pImportDesc;
}

//////////////////////////////////////////////////////////////////////////

BOOL HookDynamicLoadedFun( UINT uiCount, LPHOOKFUNCDESC paHookArray, bool bRestore )
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
    MODULEENTRY32 me32; 
    // Take a snapshot of all modules in the specified process. 
    hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, GetCurrentProcessId()); 
    if( hModuleSnap == INVALID_HANDLE_VALUE ) 
    {     
        return FALSE; 
    } 

    me32.dwSize = sizeof( MODULEENTRY32 ); 
    if( !Module32First( hModuleSnap, &me32 ) ) 
    {     
        CloseHandle( hModuleSnap );
        return FALSE; 
    }
    HMODULE hIgnoreDll[3] = {0};
    hIgnoreDll[0] = LoadLibrary(  _T("dbghelp.dll"));
    hIgnoreDll[1] = LoadLibrary(  _T("dbgeng.dll"));
    hIgnoreDll[2] = LoadLibrary(  _T("Kernel32.dll"));
    do 
    {
        if( me32.hModule == hIgnoreDll[0] ||
            me32.hModule == hIgnoreDll[1] ||
            me32.hModule == hIgnoreDll[2])
        {
            continue;
        }
        
        PIMAGE_IMPORT_DESCRIPTOR pImportDesc = GetNamedImportDescriptor(me32.hModule, 0 );
        while (pImportDesc && pImportDesc->Name)
        {
            FindFunction( me32.hModule, pImportDesc, uiCount, paHookArray, bRestore );
            pImportDesc++;
        }
    }
    while( Module32Next( hModuleSnap, &me32 ) ); 
    CloseHandle( hModuleSnap ); 
    return TRUE;
}