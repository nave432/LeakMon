#ifndef __HOOK_FUNCTIONS_H__
#define __HOOK_FUNCTIONS_H__

#if _WIN32_WINNT < 0x0600 // to compile under vs6

typedef struct _ICONINFOEXA {
    DWORD   cbSize;
    BOOL    fIcon;
    DWORD   xHotspot;
    DWORD   yHotspot;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
    WORD    wResID;
    CHAR    szModName[MAX_PATH];
    CHAR    szResName[MAX_PATH];
} ICONINFOEXA, *PICONINFOEXA;
typedef struct _ICONINFOEXW {
    DWORD   cbSize;
    BOOL    fIcon;
    DWORD   xHotspot;
    DWORD   yHotspot;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
    WORD    wResID;
    WCHAR   szModName[MAX_PATH];
    WCHAR   szResName[MAX_PATH];
} ICONINFOEXW, *PICONINFOEXW;


typedef enum _FILE_ID_TYPE {
    FileIdType,
        ObjectIdType,
        MaximumFileIdType
} FILE_ID_TYPE, *PFILE_ID_TYPE;

typedef struct FILE_ID_DESCRIPTOR {
    DWORD dwSize;  // Size of the struct
    FILE_ID_TYPE Type; // Describes the type of identifier passed in.
    union {
        LARGE_INTEGER FileId;
        GUID ObjectId;
    };
} FILE_ID_DESCRIPTOR, *LPFILE_ID_DESCRIPTOR;

typedef LONG LSTATUS;

#endif

// memory allocations

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

typedef LPVOID ( WINAPI *HeapAllocDef)( IN HANDLE hHeap,IN DWORD dwFlags,IN SIZE_T dwBytes );
typedef BOOL ( WINAPI *HeapFreeDef)(  HANDLE hHeap,DWORD dwFlags,LPVOID lpMem);
typedef LPVOID ( WINAPI *HeapReAllocDef)( HANDLE hHeap,DWORD dwFlags,LPVOID lpMem,SIZE_T dwBytes );

typedef LPVOID ( WINAPI *VirtualAllocDef)( LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect );
typedef BOOL ( WINAPI *VirtualFreeDef)( LPVOID lpAddress, SIZE_T dwSize,DWORD dwFreeType );
typedef LPVOID ( WINAPI *VirtualAllocExDef)( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect );
typedef BOOL ( WINAPI *VirtualFreeExDef)( HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize,DWORD dwFreeType );

typedef LPVOID (WINAPI *CoTaskMemAllocDef)( SIZE_T cb);
typedef LPVOID (WINAPI *CoTaskMemReallocDef)(LPVOID pv, SIZE_T cb);
typedef void   (WINAPI *CoTaskMemFreeDef)( LPVOID pv );
typedef LPVOID (WINAPI *MapViewOfFileDef)( HANDLE hFileMappingObject, DWORD dwDesiredAccess,DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap );
typedef LPVOID (WINAPI *MapViewOfFileExDef)( HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow, SIZE_T dwNumberOfBytesToMap, LPVOID lpBaseAddress );
typedef BOOL (WINAPI *UnmapViewOfFileDef)( LPCVOID lpBaseAddress );
typedef INT (*NtMapViewOfSectionDef)( HANDLE SectionHandle, HANDLE ProcessHandle, PVOID *BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize,SECTION_INHERIT InheritDisposition, ULONG AllocationType, ULONG Win32Protect );
typedef INT (*NtUnmapViewOfSectionDef)( HANDLE ProcessHandle, PVOID BaseAddress );

static HeapAllocDef   pOrgHeapAlloc = 0;
static HeapReAllocDef pOrgHeapReAlloc= 0;
static HeapFreeDef	  pOrgHeapFree= 0;
static VirtualAllocDef pOrgVirtualAlloc = 0;
static VirtualFreeDef  pOrgVirtualFree = 0;
static VirtualAllocExDef pOrgVirtualAllocEx = 0;
static VirtualFreeExDef  pOrgVirtualFreeEx = 0;
static CoTaskMemAllocDef	pOrgCoTaskMemAlloc = 0;
static CoTaskMemReallocDef	pOrgCoTaskMemRealloc = 0;
static CoTaskMemFreeDef		pOrgCoTaskMemFree = 0;
static MapViewOfFileDef		pOrgMapViewOfFile = 0;
static MapViewOfFileExDef	pOrgMapViewOfFileEx = 0;
static UnmapViewOfFileDef	pOrgUnmapViewOfFile = 0;
static NtMapViewOfSectionDef	pOrgNtMapViewOfSection = 0;
static NtUnmapViewOfSectionDef	pOrgNtUnmapViewOfSection = 0;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//GDI allocation
//bitmap - LoadBitmapA, LoadBitmapW, CreateBitmap, CreateBitmapIndirect, CreateCompatibleBitmap ,CreateDIBitmap, CreateDIBSection,LoadImage 


typedef HBITMAP (WINAPI *LoadBitmapADef)( HINSTANCE hInstance, LPCSTR lpBitmapName);
typedef HBITMAP (WINAPI *LoadBitmapWDef)( HINSTANCE hInstance, LPCWSTR lpBitmapName);
typedef HANDLE  (WINAPI *LoadImageADef)( HINSTANCE hInst, LPCSTR name, UINT type,int cx,int cy, UINT fuLoad);
typedef HANDLE  (WINAPI *LoadImageWDef)( HINSTANCE hInst, LPCWSTR name, UINT type, int cx, int cy, UINT fuLoad);
typedef HBITMAP (WINAPI *CreateBitmapDef)(  int nWidth,  int nHeight, UINT nPlanes,  UINT nBitCount,  CONST VOID *lpBits);
typedef HBITMAP (WINAPI *CreateBitmapIndirectDef)(  CONST BITMAP *pbm);
typedef HBITMAP (WINAPI *CreateCompatibleBitmapDef)(  HDC hdc,  int cx,  int cy);
typedef HBITMAP (WINAPI *CreateDIBitmapDef)(  HDC hdc,  CONST BITMAPINFOHEADER *pbmih,  DWORD flInit,  CONST VOID *pjBits,  CONST BITMAPINFO *pbmi,  UINT iUsage);
typedef HBITMAP (WINAPI *CreateDIBSectionDef)( HDC hdc,  CONST BITMAPINFO *lpbmi,  UINT usage, VOID **ppvBits,  HANDLE hSection,  DWORD offset);
typedef HBITMAP (WINAPI *CreateDiscardableBitmapDef)( HDC hdc, int cx, int cy);
typedef HANDLE  (WINAPI *CopyImageDef)( HANDLE h, UINT type, int cx, int cy, UINT flags);
typedef BOOL (WINAPI *GetIconInfoDef)( HICON hIcon, PICONINFO piconinfo);
typedef BOOL (WINAPI *GetIconInfoExADef)( HICON hicon, PICONINFOEXA piconinfo);
typedef BOOL (WINAPI *GetIconInfoExWDef)( HICON hicon,PICONINFOEXW piconinfo);
typedef BOOL (WINAPI *DeleteObjectDef)(  HGDIOBJ ho);

static LoadBitmapADef pOrgLoadBitmapA;
static LoadBitmapWDef pOrgLoadBitmapW;
static LoadImageADef pOrgLoadImageA;
static LoadImageWDef pOrgLoadImageW;
static CreateBitmapDef pOrgCreateBitmap;
static CreateBitmapIndirectDef pOrgCreateBitmapIndirect;
static CreateCompatibleBitmapDef pOrgCreateCompatibleBitmap;
static CreateDIBitmapDef pOrgCreateDIBitmap;
static CreateDIBSectionDef pOrgCreateDIBSection;
static CreateDiscardableBitmapDef pOrgCreateDiscardableBitmap;
static CopyImageDef pOrgCopyImage;
static GetIconInfoDef	 pOrgGetIconInfo;
static GetIconInfoExADef pOrgGetIconInfoExA;
static GetIconInfoExWDef pOrgGetIconInfoExW;
static DeleteObjectDef pOrgDeleteObject;

//////////////////////////////////////////////////////////////////////////
//icons
typedef HICON (WINAPI *CopyIconDef)( HICON hIcon);
typedef HICON (WINAPI *CreateIconDef)(HINSTANCE hInstance,int nWidth,int nHeight,BYTE cPlanes,BYTE cBitsPixel,CONST BYTE *lpbANDbits,CONST BYTE *lpbXORbits);
typedef HICON (WINAPI *CreateIconFromResourceDef)( PBYTE presbits, DWORD dwResSize, BOOL fIcon, DWORD dwVer);
typedef HICON (WINAPI *CreateIconFromResourceExDef)( PBYTE presbits, DWORD dwResSize,BOOL fIcon,DWORD dwVer,int cxDesired,int cyDesired,UINT Flags);
typedef HICON (WINAPI *CreateIconIndirectDef)( PICONINFO piconinfo);
typedef BOOL  (WINAPI *DestroyIconDef)(HICON hIcon);
typedef HICON (WINAPI *DuplicateIconDef)(HINSTANCE hInst, HICON hIcon);
typedef HICON (WINAPI *ExtractAssociatedIconADef)(HINSTANCE hInst,  LPSTR lpIconPath,  LPWORD lpiIcon);
typedef HICON (WINAPI *ExtractAssociatedIconWDef)(HINSTANCE hInst,  LPWSTR lpIconPath,  LPWORD lpiIcon);
typedef HICON (WINAPI *ExtractAssociatedIconExADef)(HINSTANCE hInst,LPSTR lpIconPath,  LPWORD lpiIconIndex,  LPWORD lpiIconId);
typedef HICON (WINAPI *ExtractAssociatedIconExWDef)(HINSTANCE hInst,LPWSTR lpIconPath,  LPWORD lpiIconIndex,  LPWORD lpiIconId);
typedef HICON (WINAPI *ExtractIconADef)(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex);
typedef HICON (WINAPI *ExtractIconWDef)(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex);
typedef UINT  (WINAPI *ExtractIconExADef)(LPCSTR lpszFile, int nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIcons);
typedef UINT  (WINAPI *ExtractIconExWDef)(LPCWSTR lpszFile, int nIconIndex,  HICON *phiconLarge, HICON *phiconSmall, UINT nIcons);
typedef HICON (WINAPI *LoadIconADef)( HINSTANCE hInstance, LPCSTR lpIconName );
typedef HICON (WINAPI *LoadIconWDef)( HINSTANCE hInstance, LPCWSTR lpIconName );
typedef UINT  (WINAPI *PrivateExtractIconsADef)( LPCSTR szFileName, int nIconIndex, int cxIcon, int cyIcon, HICON *phicon, UINT *piconid, UINT nIcons, UINT flags);
typedef UINT  (WINAPI *PrivateExtractIconsWDef)( LPCWSTR szFileName, int nIconIndex, int cxIcon, int cyIcon, HICON *phicon, UINT *piconid,UINT nIcons,UINT flags);

static CopyIconDef					pOrgCopyIcon;
static CreateIconDef				pOrgCreateIcon;
static CreateIconFromResourceDef	pOrgCreateIconFromResource;
static CreateIconFromResourceExDef	pOrgCreateIconFromResourceEx;
static CreateIconIndirectDef		pOrgCreateIconIndirect;
static DestroyIconDef				pOrgDestroyIcon;
static DuplicateIconDef				pOrgDuplicateIcon;
static ExtractAssociatedIconADef	pOrgExtractAssociatedIconA;
static ExtractAssociatedIconWDef	pOrgExtractAssociatedIconW;
static ExtractAssociatedIconExADef	pOrgExtractAssociatedIconExA;
static ExtractAssociatedIconExWDef	pOrgExtractAssociatedIconExW;
static ExtractIconADef				pOrgExtractIconA;
static ExtractIconWDef				pOrgExtractIconW;
static ExtractIconExADef			pOrgExtractIconExA;
static ExtractIconExWDef			pOrgExtractIconExW;
static LoadIconADef					pOrgLoadIconA;
static LoadIconWDef					pOrgLoadIconW;
static PrivateExtractIconsADef		pOrgPrivateExtractIconsA;
static PrivateExtractIconsWDef		pOrgPrivateExtractIconsW;

//////////////////////////////////////////////////////////////////////////
//cursor
typedef HCURSOR (WINAPI *CreateCursorDef)( HINSTANCE hInst, int xHotSpot, int yHotSpot,int nWidth, int nHeight, CONST VOID *pvANDPlane,CONST VOID *pvXORPlane);
typedef BOOL    (WINAPI *DestroyCursorDef)( HCURSOR hCursor);
typedef HCURSOR (WINAPI *LoadCursorADef)( HINSTANCE hInstance, LPCSTR lpCursorName);
typedef HCURSOR (WINAPI *LoadCursorWDef)( HINSTANCE hInstance, LPCWSTR lpCursorName);
typedef HCURSOR (WINAPI *LoadCursorFromFileADef)( LPCSTR lpFileName );
typedef HCURSOR (WINAPI *LoadCursorFromFileWDef)( LPCWSTR lpFileName ); 
static CreateCursorDef			pOrgCreateCursor;
static LoadCursorADef			pOrgLoadCursorA;
static LoadCursorWDef			pOrgLoadCursorW;
static LoadCursorFromFileADef	pOrgLoadCursorFromFileA;
static LoadCursorFromFileWDef	pOrgLoadCursorFromFileW;
static DestroyCursorDef			pOrgDestroyCursor;


//brush 
typedef HBRUSH  (WINAPI *CreateBrushIndirectDef)(  CONST LOGBRUSH *plbrush);
typedef HBRUSH  (WINAPI *CreateSolidBrushDef)(  COLORREF color);
typedef HBRUSH  (WINAPI *CreatePatternBrushDef)(  HBITMAP hbm);
typedef HBRUSH  (WINAPI *CreateDIBPatternBrushDef)(  HGLOBAL h,  UINT iUsage);
typedef HBRUSH  (WINAPI *CreateDIBPatternBrushPtDef)(  CONST VOID *lpPackedDIB,  UINT iUsage);
typedef HBRUSH  (WINAPI *CreateHatchBrushDef)(  int iHatch,  COLORREF color);
static CreateBrushIndirectDef pOrgCreateBrushIndirect;
static CreateSolidBrushDef pOrgCreateSolidBrush;
static CreatePatternBrushDef pOrgCreatePatternBrush;
static CreateDIBPatternBrushDef pOrgCreateDIBPatternBrush;
static CreateDIBPatternBrushPtDef pOrgCreateDIBPatternBrushPt;
static CreateHatchBrushDef pOrgCreateHatchBrush;


//device context 
typedef HDC (WINAPI *CreateCompatibleDCDef)(  HDC hdc);
typedef HDC (WINAPI *CreateDCADef)( LPCSTR pwszDriver,  LPCSTR pwszDevice,  LPCSTR pszPort,  CONST DEVMODEA * pdm);
typedef HDC (WINAPI *CreateDCWDef)( LPCWSTR pwszDriver,  LPCWSTR pwszDevice,  LPCWSTR pszPort,  CONST DEVMODEW * pdm);
typedef HDC (WINAPI *CreateICADef)( LPCSTR pszDriver,  LPCSTR pszDevice,  LPCSTR pszPort,  CONST DEVMODEA * pdm);
typedef HDC (WINAPI *CreateICWDef)( LPCWSTR pszDriver,  LPCWSTR pszDevice,  LPCWSTR pszPort,  CONST DEVMODEW * pdm);
typedef HDC (WINAPI *GetDCDef)( HWND hWnd );
typedef HDC (WINAPI *GetDCExDef)( HWND hWnd, HRGN hrgnClip, DWORD flags );
typedef HDC (WINAPI *GetWindowDCDef)( HWND hWnd ); 
typedef int (WINAPI *ReleaseDCDef)( HWND hWnd, HDC hDC);
typedef BOOL (WINAPI *DeleteDCDef)( HDC hdc);

static CreateCompatibleDCDef pOrgCreateCompatibleDC;
static CreateDCADef pOrgCreateDCA;
static CreateDCWDef pOrgCreateDCW;
static CreateICADef pOrgCreateICA;
static CreateICWDef pOrgCreateICW;
static GetDCDef pOrgGetDC;
static GetDCExDef pOrgGetDCEx;
static GetWindowDCDef pOrgGetWindowDC;
static ReleaseDCDef pOrgReleaseDC;
static DeleteDCDef pOrgDeleteDC;

//font 
typedef HFONT (WINAPI * CreateFontADef)(  int cHeight,  int cWidth,  int cEscapement,  int cOrientation,  int cWeight,  DWORD bItalic,
						    DWORD bUnderline,  DWORD bStrikeOut,  DWORD iCharSet,  DWORD iOutPrecision,  DWORD iClipPrecision,
						    DWORD iQuality,  DWORD iPitchAndFamily, LPCSTR pszFaceName);

typedef HFONT (WINAPI * CreateFontWDef)(  int cHeight,  int cWidth,  int cEscapement,  int cOrientation,  int cWeight,  DWORD bItalic,
						    DWORD bUnderline,  DWORD bStrikeOut,  DWORD iCharSet,  DWORD iOutPrecision,  DWORD iClipPrecision,
						    DWORD iQuality,  DWORD iPitchAndFamily, LPCWSTR pszFaceName);
typedef HFONT (WINAPI * CreateFontIndirectADef)(  CONST LOGFONTA *lplf);
typedef HFONT (WINAPI * CreateFontIndirectWDef)(  CONST LOGFONTW *lplf);
static CreateFontADef pOrgCreateFontA;
static CreateFontWDef pOrgCreateFontW;
static CreateFontIndirectADef pOrgCreateFontIndirectA;
static CreateFontIndirectWDef pOrgCreateFontIndirectW;

//metafile 
typedef HDC (WINAPI * CreateMetaFileADef)(  LPCSTR pszFile);
typedef HDC (WINAPI * CreateMetaFileWDef)(  LPCWSTR pszFile);
typedef HDC (WINAPI * CreateEnhMetaFileADef)(  HDC hdc,  LPCSTR lpFilename,  CONST RECT *lprc,  LPCSTR lpDesc);
typedef HDC (WINAPI * CreateEnhMetaFileWDef)(  HDC hdc,  LPCWSTR lpFilename,  CONST RECT *lprc,  LPCWSTR lpDesc);
typedef HENHMETAFILE (WINAPI * GetEnhMetaFileADef)(  LPCSTR lpName);
typedef HENHMETAFILE (WINAPI * GetEnhMetaFileWDef)(  LPCWSTR lpName);
typedef HMETAFILE (WINAPI * GetMetaFileADef)(  LPCSTR lpName);
typedef HMETAFILE (WINAPI * GetMetaFileWDef)(  LPCWSTR lpName);
typedef BOOL (WINAPI *DeleteMetaFileDef)( HMETAFILE hmf);
typedef BOOL (WINAPI *DeleteEnhMetaFileDef)( HENHMETAFILE hmf);
typedef HENHMETAFILE (WINAPI *CopyEnhMetaFileADef)( HENHMETAFILE hEnh, LPCSTR lpFileName);
typedef HENHMETAFILE (WINAPI *CopyEnhMetaFileWDef)( HENHMETAFILE hEnh, LPCWSTR lpFileName);
typedef HENHMETAFILE (WINAPI *CloseEnhMetaFileDef)( HDC hdc);
typedef HMETAFILE    (WINAPI *CloseMetaFileDef)( HDC hdc);


static CreateMetaFileADef		pOrgCreateMetaFileA;
static CreateMetaFileWDef		pOrgCreateMetaFileW;
static CreateEnhMetaFileADef	pOrgCreateEnhMetaFileA;
static CreateEnhMetaFileWDef	pOrgCreateEnhMetaFileW;
static GetEnhMetaFileADef		pOrgGetEnhMetaFileA;
static GetEnhMetaFileWDef		pOrgGetEnhMetaFileW;
static GetMetaFileADef			pOrgGetMetaFileA;
static GetMetaFileWDef			pOrgGetMetaFileW;
static DeleteMetaFileDef		pOrgDeleteMetaFile;
static DeleteEnhMetaFileDef		pOrgDeleteEnhMetaFile;
static CopyEnhMetaFileADef		pOrgCopyEnhMetaFileA;
static CopyEnhMetaFileWDef		pOrgCopyEnhMetaFileW;
static CloseEnhMetaFileDef		pOrgCloseEnhMetaFile;
static CloseMetaFileDef			pOrgCloseMetaFile;

//pen
typedef HPEN (WINAPI *CreatePenDef)( int iStyle,  int cWidth,  COLORREF color);
typedef HPEN (WINAPI *CreatePenIndirectDef)( CONST LOGPEN *plpen);
typedef HPEN (WINAPI *ExtCreatePenDef)(DWORD iPenStyle,DWORD cWidth,CONST LOGBRUSH *plbrush,DWORD cStyle,CONST DWORD *pstyle); 
static CreatePenDef pOrgCreatePen;
static CreatePenIndirectDef pOrgCreatePenIndirect;
static ExtCreatePenDef pOrgExtCreatePen;

//region 
typedef HRGN (WINAPI * PathToRegionDef)(HDC hdc);
typedef HRGN (WINAPI * CreateEllipticRgnDef)( int x1,  int y1,  int x2, int y2);
typedef HRGN (WINAPI * CreateEllipticRgnIndirectDef)( CONST RECT *lprect);
typedef HRGN (WINAPI * CreatePolygonRgnDef)(CONST POINT *pptl, int cPoint, int iMode);
typedef HRGN (WINAPI * CreatePolyPolygonRgnDef)( CONST POINT *pptl,CONST INT  *pc,int cPoly, int iMode);
typedef HRGN (WINAPI * CreateRectRgnDef)( int x1,  int y1,  int x2,  int y2);
typedef HRGN (WINAPI * CreateRectRgnIndirectDef)( CONST RECT *lprect);
typedef HRGN (WINAPI * CreateRoundRectRgnDef)( int x1,  int y1,  int x2,  int y2,  int w,  int h);
typedef HRGN (WINAPI * ExtCreateRegionDef)(CONST XFORM * lpx,  DWORD nCount, CONST RGNDATA * lpData);
static PathToRegionDef pOrgPathToRegion;
static CreateEllipticRgnDef pOrgCreateEllipticRgn;
static CreateEllipticRgnIndirectDef pOrgCreateEllipticRgnIndirect;
static CreatePolygonRgnDef pOrgCreatePolygonRgn;
static CreatePolyPolygonRgnDef pOrgCreatePolyPolygonRgn;
static CreateRectRgnDef pOrgCreateRectRgn;
static CreateRectRgnIndirectDef pOrgCreateRectRgnIndirect;
static CreateRoundRectRgnDef pOrgCreateRoundRectRgn;
static ExtCreateRegionDef pOrgExtCreateRegion;

//palette 
typedef HPALETTE (WINAPI *  CreateHalftonePaletteDef)( HDC hdc);
typedef HPALETTE (WINAPI *  CreatePaletteDef)(CONST LOGPALETTE * plpal );
static CreateHalftonePaletteDef pOrgCreateHalftonePalette;
static CreatePaletteDef pOrgCreatePalette;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Handle creation function

// sync function
typedef HANDLE (WINAPI *CreateEventADef)(LPSECURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCSTR lpName);
typedef HANDLE (WINAPI *CreateEventWDef)( LPSECURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCWSTR lpName);
typedef HANDLE (WINAPI *CreateEventExADef)( LPSECURITY_ATTRIBUTES lpEventAttributes, LPCSTR lpName, DWORD dwFlags,DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateEventExWDef)( LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess );
typedef HANDLE (WINAPI *OpenEventADef)( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName);
typedef HANDLE (WINAPI *OpenEventWDef)( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName );

typedef HANDLE (WINAPI *CreateMutexADef)(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner, LPCSTR lpName );
typedef HANDLE (WINAPI *CreateMutexWDef)(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCWSTR lpName);
typedef HANDLE (WINAPI *CreateMutexExADef)(LPSECURITY_ATTRIBUTES lpEventAttributes,LPCSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateMutexExWDef)(LPSECURITY_ATTRIBUTES lpEventAttributes,LPCWSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *OpenMutexADef)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCSTR lpName);
typedef HANDLE (WINAPI *OpenMutexWDef)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCWSTR lpName);

typedef HANDLE (WINAPI *CreateSemaphoreADef)( LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount,LPCSTR lpName );
typedef HANDLE (WINAPI *CreateSemaphoreWDef)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCWSTR lpName);
typedef HANDLE (WINAPI *CreateSemaphoreExADef)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateSemaphoreExWDef)(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCWSTR lpName,DWORD dwFlags,DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *OpenSemaphoreADef)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCSTR lpName);
typedef HANDLE (WINAPI *OpenSemaphoreWDef)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPWSTR lpName);

typedef HANDLE (WINAPI *CreateWaitableTimerADef)( LPSECURITY_ATTRIBUTES lpTimerAttributes, BOOL bManualReset, LPCSTR lpTimerName);
typedef HANDLE (WINAPI *CreateWaitableTimerWDef)(LPSECURITY_ATTRIBUTES lpTimerAttributes,BOOL bManualReset,LPCWSTR lpTimerName);
typedef HANDLE (WINAPI *CreateWaitableTimerExADef)(LPSECURITY_ATTRIBUTES lpTimerAttributes,LPCSTR lpTimerName,DWORD dwFlags,DWORD dwDesiredAccess);
typedef HANDLE (WINAPI *CreateWaitableTimerExWDef)(LPSECURITY_ATTRIBUTES lpTimerAttributes,LPCWSTR lpTimerName,DWORD dwFlags,DWORD dwDesiredAccess); 
typedef HANDLE (WINAPI *OpenWaitableTimerADef)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCSTR lpTimerName);
typedef HANDLE (WINAPI *OpenWaitableTimerWDef)(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCWSTR lpTimerName);

// file function
typedef HANDLE (WINAPI *CreateFileADef)(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
typedef HANDLE (WINAPI *CreateFileWDef)(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile); 
typedef HANDLE (WINAPI *CreateFileTransactedADef)(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile,HANDLE hTransaction,PUSHORT pusMiniVersion,PVOID  lpExtendedParameter);
typedef HANDLE (WINAPI *CreateFileTransactedWDef)(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile,HANDLE hTransaction,PUSHORT pusMiniVersion,PVOID  lpExtendedParameter );
typedef HANDLE (WINAPI *FindFirstFileADef)(LPCSTR lpFileName,LPWIN32_FIND_DATAA lpFindFileData);
typedef HANDLE (WINAPI *FindFirstFileWDef)(LPCWSTR lpFileName,LPWIN32_FIND_DATAW lpFindFileData); 
typedef HANDLE (WINAPI *FindFirstFileExADef)(LPCSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags);
typedef HANDLE (WINAPI *FindFirstFileExWDef)(LPCWSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags); 
typedef HANDLE (WINAPI *FindFirstFileNameTransactedWDef)(LPCWSTR lpFileName,DWORD dwFlags,LPDWORD StringLength,PWCHAR LinkName,HANDLE hTransaction);
typedef HANDLE (WINAPI *FindFirstFileNameWDef)(LPCWSTR lpFileName,DWORD dwFlags,LPDWORD StringLength,PWCHAR LinkName); 
typedef HANDLE (WINAPI *FindFirstFileTransactedADef)(LPCSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags,HANDLE hTransaction);
typedef HANDLE (WINAPI *FindFirstFileTransactedWDef)(LPCWSTR lpFileName,FINDEX_INFO_LEVELS fInfoLevelId,LPVOID lpFindFileData,FINDEX_SEARCH_OPS fSearchOp,LPVOID lpSearchFilter,DWORD dwAdditionalFlags,HANDLE hTransaction);
typedef HANDLE (WINAPI *FindFirstStreamTransactedWDef)(LPCWSTR lpFileName,STREAM_INFO_LEVELS InfoLevel,LPVOID lpFindStreamData,DWORD dwFlags,HANDLE hTransaction);
typedef HANDLE (WINAPI *FindFirstStreamWDef)( LPCWSTR lpFileName,STREAM_INFO_LEVELS InfoLevel,LPVOID lpFindStreamData,DWORD dwFlags);
typedef BOOL   (WINAPI *FindCloseDef)( HANDLE hFindFile);
typedef HANDLE (WINAPI *OpenFileByIdDef)(HANDLE hFile,LPFILE_ID_DESCRIPTOR lpFileID,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwFlags);
typedef HANDLE (WINAPI *ReOpenFileDef)(HANDLE hOriginalFile,DWORD dwDesiredAccess,DWORD dwShareMode,DWORD dwFlags);
typedef HANDLE (WINAPI *CreateIoCompletionPortDef)(HANDLE FileHandle,HANDLE ExistingCompletionPort,ULONG_PTR CompletionKey,DWORD NumberOfConcurrentThreads);

//Authorization function
typedef BOOL   (WINAPI *CreateRestrictedTokenDef)(HANDLE ExistingTokenHandle,DWORD Flags,DWORD DisableSidCount,PSID_AND_ATTRIBUTES SidsToDisable,DWORD DeletePrivilegeCount,PLUID_AND_ATTRIBUTES PrivilegesToDelete,DWORD RestrictedSidCount,PSID_AND_ATTRIBUTES SidsToRestrict,PHANDLE NewTokenHandle);
typedef BOOL   (WINAPI *DuplicateTokenDef)(HANDLE ExistingTokenHandle,SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,PHANDLE DuplicateTokenHandle);
typedef BOOL   (WINAPI *DuplicateTokenExDef)(HANDLE hExistingToken,DWORD dwDesiredAccess,LPSECURITY_ATTRIBUTES lpTokenAttributes,SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,TOKEN_TYPE TokenType,PHANDLE phNewToken);
typedef BOOL   (WINAPI *OpenProcessTokenDef)(HANDLE ProcessHandle,DWORD DesiredAccess,PHANDLE TokenHandle);
typedef BOOL   (WINAPI *OpenThreadTokenDef)(HANDLE ThreadHandle,DWORD DesiredAccess,BOOL OpenAsSelf,PHANDLE TokenHandle); 

//Directory management
typedef HANDLE (WINAPI *FindFirstChangeNotificationADef)(LPCSTR lpPathName,BOOL bWatchSubtree,DWORD dwNotifyFilter);
typedef HANDLE (WINAPI *FindFirstChangeNotificationWDef)(LPCWSTR lpPathName,BOOL bWatchSubtree,DWORD dwNotifyFilter); 
typedef BOOL   (WINAPI *FindCloseChangeNotificationDef)(HANDLE hChangeHandle); 

// File mapping
typedef HANDLE (WINAPI *CreateMemoryResourceNotificationDef)( MEMORY_RESOURCE_NOTIFICATION_TYPE NotificationType );
typedef HANDLE (WINAPI *CreateFileMappingADef)( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName );
typedef HANDLE (WINAPI *CreateFileMappingWDef)( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCWSTR lpName );
typedef HANDLE (WINAPI *CreateFileMappingNumaADef)( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCSTR lpName, DWORD nndPreferred );
typedef HANDLE (WINAPI *CreateFileMappingNumaWDef)( HANDLE hFile, LPSECURITY_ATTRIBUTES lpFileMappingAttributes, DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCWSTR lpName, DWORD nndPreferred );
typedef HANDLE (WINAPI *OpenFileMappingADef)( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCSTR lpName );
typedef HANDLE (WINAPI *OpenFileMappingWDef)( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCWSTR lpName );

//Memory
typedef HANDLE (WINAPI *HeapCreateDef)( DWORD flOptions, SIZE_T dwInitialSize, SIZE_T dwMaximumSize );
typedef BOOL   (WINAPI *HeapDestroyDef)(HANDLE hHeap );
typedef HANDLE (WINAPI *GlobalAllocDef)( UINT uFlags, SIZE_T dwBytes );
typedef HANDLE (WINAPI *GlobalReAllocDef)( HGLOBAL hMem, SIZE_T dwBytes, UINT uFlags );
typedef HANDLE (WINAPI *GlobalFreeDef)( HGLOBAL hMem );
typedef HLOCAL (WINAPI *LocalAllocDef)( UINT uFlags, SIZE_T uBytes );
typedef HLOCAL (WINAPI *LocalReAllocDef)( HLOCAL hMem, SIZE_T uBytes, UINT uFlags );
typedef HLOCAL (WINAPI *LocalFreeDef)(HLOCAL hMem );

//Process and thread
typedef BOOL   (WINAPI *CreateProcessADef)( LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation );
typedef BOOL   (WINAPI *CreateProcessWDef)( LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation );
typedef BOOL   (WINAPI *CreateProcessAsUserADef)(HANDLE hToken,LPCSTR lpApplicationName,LPSTR lpCommandLine,LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCSTR lpCurrentDirectory,LPSTARTUPINFOA lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL   (WINAPI *CreateProcessAsUserWDef)(HANDLE hToken,LPWSTR lpApplicationName,LPWSTR lpCommandLine,LPSECURITY_ATTRIBUTES lpProcessAttributes,LPSECURITY_ATTRIBUTES lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,LPVOID lpEnvironment,LPWSTR lpCurrentDirectory,LPSTARTUPINFOW lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL   (WINAPI *CreateProcessWithLogonWDef)(LPCWSTR lpUsername,LPCWSTR lpDomain,LPCWSTR lpPassword,DWORD dwLogonFlags,LPCWSTR lpApplicationName,LPWSTR lpCommandLine,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCWSTR lpCurrentDirectory,LPSTARTUPINFOW lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation);
typedef BOOL   (WINAPI *CreateProcessWithTokenWDef)(HANDLE hToken,DWORD dwLogonFlags,LPCWSTR lpApplicationName,LPWSTR lpCommandLine,DWORD dwCreationFlags,LPVOID lpEnvironment,LPCWSTR lpCurrentDirectory,LPSTARTUPINFOW lpStartupInfo,LPPROCESS_INFORMATION lpProcessInformation); 
typedef HANDLE (WINAPI *OpenProcessDef)( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId );
typedef HANDLE (WINAPI *CreateThreadDef)( LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId );
typedef HANDLE (WINAPI *CreateRemoteThreadDef)( HANDLE hProcess, LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId );
typedef HANDLE (WINAPI *OpenThreadDef)( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwThreadId );
typedef HANDLE (WINAPI *CreateJobObjectADef)( LPSECURITY_ATTRIBUTES lpJobAttributes, LPCSTR lpName );
typedef HANDLE (WINAPI *CreateJobObjectWDef)( LPSECURITY_ATTRIBUTES lpJobAttributes, LPCWSTR lpName );

// Mail slot
typedef HANDLE (WINAPI *CreateMailslotADef)( LPCSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
typedef HANDLE (WINAPI *CreateMailslotWDef)( LPCWSTR lpName, DWORD nMaxMessageSize, DWORD lReadTimeout, LPSECURITY_ATTRIBUTES lpSecurityAttributes );

// pipe
typedef BOOL   (WINAPI *CreatePipeDef)( PHANDLE hReadPipe, PHANDLE hWritePipe, LPSECURITY_ATTRIBUTES lpPipeAttributes, DWORD nSize ); 
typedef HANDLE (WINAPI *CreateNamedPipeADef)( LPCSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes );
typedef HANDLE (WINAPI *CreateNamedPipeWDef)( LPCWSTR lpName, DWORD dwOpenMode, DWORD dwPipeMode, DWORD nMaxInstances, DWORD nOutBufferSize, DWORD nInBufferSize, DWORD nDefaultTimeOut, LPSECURITY_ATTRIBUTES lpSecurityAttributes );

//Registry
typedef LSTATUS (WINAPI *RegCreateKeyExADef)( HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition );
typedef LSTATUS (WINAPI *RegCreateKeyExWDef)( HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition );
typedef LSTATUS (WINAPI *RegCreateKeyTransactedADef)( HKEY hKey, LPCSTR lpSubKey, DWORD Reserved, LPSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition, HANDLE hTransaction, PVOID  pExtendedParemeter );
typedef LSTATUS (WINAPI *RegCreateKeyTransactedWDef)( HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition, HANDLE hTransaction, PVOID  pExtendedParemeter ); 
typedef LSTATUS (WINAPI *RegOpenCurrentUserDef)( REGSAM samDesired, PHKEY phkResult ); 
typedef LSTATUS (WINAPI *RegOpenKeyADef)( HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult );
typedef LSTATUS (WINAPI *RegOpenKeyWDef)( HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult );
typedef LSTATUS (WINAPI *RegOpenKeyExADef)( HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult );
typedef LSTATUS (WINAPI *RegOpenKeyExWDef)( HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult ); 
typedef LSTATUS (WINAPI *RegOpenKeyTransactedADef)( HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult, HANDLE hTransaction, PVOID  pExtendedParemeter );
typedef LSTATUS (WINAPI *RegOpenKeyTransactedWDef)( HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult, HANDLE hTransaction, PVOID  pExtendedParemeter );
typedef LSTATUS (WINAPI *RegOpenUserClassesRootDef)( HANDLE hToken, DWORD dwOptions, REGSAM samDesired, PHKEY  phkResult ); 
typedef LSTATUS (WINAPI *RegCreateKeyADef)( HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult );
typedef LSTATUS (WINAPI *RegCreateKeyWDef)( HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult );
typedef LSTATUS (WINAPI *RegCloseKeyDef)( HKEY hKey ); 

////////////////////////////////start v3 additions//////////////////////////////////////////////////
// Timers
typedef HANDLE (WINAPI *CreateTimerQueueDef)(void);
typedef BOOL   (WINAPI *CreateTimerQueueTimerDef)(PHANDLE phNewTimer,HANDLE TimerQueue,WAITORTIMERCALLBACK Callback,PVOID Parameter,DWORD DueTime,DWORD Period,ULONG Flags);
typedef BOOL   (WINAPI *DeleteTimerQueueTimerDef)(HANDLE TimerQueue,HANDLE Timer,HANDLE CompletionEvent);
typedef BOOL   (WINAPI *DeleteTimerQueueExDef)(HANDLE TimerQueue,HANDLE CompletionEvent);
typedef BOOL   (WINAPI *DeleteTimerQueueDef)(HANDLE TimerQueue);

//Critical section
typedef void (WINAPI *InitializeCriticalSectionDef)(LPCRITICAL_SECTION lpCriticalSection);
typedef BOOL (WINAPI *InitializeCriticalSectionExDef)(LPCRITICAL_SECTION lpCriticalSection,DWORD dwSpinCount,DWORD Flags);
typedef BOOL (WINAPI *InitializeCriticalSectionAndSpinCountDef)(LPCRITICAL_SECTION lpCriticalSection,DWORD dwSpinCount);
typedef void (WINAPI *DeleteCriticalSectionDef)( LPCRITICAL_SECTION lpCriticalSection);

////////////////////////////////end v3 additions//////////////////////////////////////////////////


typedef BOOL   (WINAPI *DuplicateHandleDef)(HANDLE hSourceProcessHandle,HANDLE hSourceHandle,HANDLE hTargetProcessHandle,LPHANDLE lpTargetHandle,DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwOptions);
typedef BOOL   (WINAPI *CloseHandleDef)( HANDLE hObject );

static CreateEventADef pOrgCreateEventA;
static CreateEventWDef pOrgCreateEventW;
static CreateEventExADef pOrgCreateEventExA;
static CreateEventExWDef pOrgCreateEventExW;
static OpenEventADef pOrgOpenEventA;
static OpenEventWDef pOrgOpenEventW;

static CreateMutexADef pOrgCreateMutexA;
static CreateMutexWDef pOrgCreateMutexW;
static CreateMutexExADef pOrgCreateMutexExA;
static CreateMutexExWDef pOrgCreateMutexExW;
static OpenMutexADef pOrgOpenMutexA;
static OpenMutexWDef pOrgOpenMutexW;
 
static CreateSemaphoreADef pOrgCreateSemaphoreA;
static CreateSemaphoreWDef pOrgCreateSemaphoreW;
static CreateSemaphoreExADef pOrgCreateSemaphoreExA;
static CreateSemaphoreExWDef pOrgCreateSemaphoreExW;
static OpenSemaphoreADef pOrgOpenSemaphoreA;
static OpenSemaphoreWDef pOrgOpenSemaphoreW;
 
static CreateWaitableTimerADef pOrgCreateWaitableTimerA;
static CreateWaitableTimerWDef pOrgCreateWaitableTimerW;
static CreateWaitableTimerExADef pOrgCreateWaitableTimerExA;
static CreateWaitableTimerExWDef pOrgCreateWaitableTimerExW;
static OpenWaitableTimerADef pOrgOpenWaitableTimerA;
static OpenWaitableTimerWDef pOrgOpenWaitableTimerW;
 
// file function
static CreateFileADef pOrgCreateFileA;
static CreateFileWDef pOrgCreateFileW;
static CreateFileTransactedADef pOrgCreateFileTransactedA;
static CreateFileTransactedWDef pOrgCreateFileTransactedW;
static FindFirstFileADef pOrgFindFirstFileA;
static FindFirstFileWDef pOrgFindFirstFileW;
static FindFirstFileExADef pOrgFindFirstFileExA;
static FindFirstFileExWDef pOrgFindFirstFileExW;
static FindFirstFileNameTransactedWDef pOrgFindFirstFileNameTransactedW ;
static FindFirstFileNameWDef pOrgFindFirstFileNameW ;
static FindFirstFileTransactedADef pOrgFindFirstFileTransactedA;
static FindFirstFileTransactedWDef pOrgFindFirstFileTransactedW;
static FindFirstStreamTransactedWDef pOrgFindFirstStreamTransactedW;
static FindFirstStreamWDef pOrgFindFirstStreamW;
static FindCloseDef pOrgFindClose;
static OpenFileByIdDef pOrgOpenFileById;
static ReOpenFileDef pOrgReOpenFile;
static CreateIoCompletionPortDef pOrgCreateIoCompletionPort;
 
//Authorization function
static CreateRestrictedTokenDef pOrgCreateRestrictedToken;
static DuplicateTokenDef pOrgDuplicateToken;
static DuplicateTokenExDef pOrgDuplicateTokenEx;
static OpenProcessTokenDef pOrgOpenProcessToken;
static OpenThreadTokenDef pOrgOpenThreadToken;

//Directory management
static FindFirstChangeNotificationADef pOrgFindFirstChangeNotificationA;
static FindFirstChangeNotificationWDef pOrgFindFirstChangeNotificationW;
static FindCloseChangeNotificationDef pOrgFindCloseChangeNotification;
 
// File mapping
static CreateMemoryResourceNotificationDef pOrgCreateMemoryResourceNotification;
static CreateFileMappingADef pOrgCreateFileMappingA;
static CreateFileMappingWDef pOrgCreateFileMappingW;
static CreateFileMappingNumaADef pOrgCreateFileMappingNumaA;
static CreateFileMappingNumaWDef pOrgCreateFileMappingNumaW;
static OpenFileMappingADef pOrgOpenFileMappingA;
static OpenFileMappingWDef pOrgOpenFileMappingW;
 
//Memory
static HeapCreateDef pOrgHeapCreate;
static HeapDestroyDef pOrgHeapDestroy;
static GlobalAllocDef pOrgGlobalAlloc;
static GlobalReAllocDef pOrgGlobalReAlloc;
static GlobalFreeDef pOrgGlobalFree;
static LocalAllocDef pOrgLocalAlloc;
static LocalReAllocDef pOrgLocalReAlloc;
static LocalFreeDef pOrgLocalFree;

//Process and thread
static CreateProcessADef pOrgCreateProcessA;
static CreateProcessWDef pOrgCreateProcessW;
static CreateProcessAsUserADef pOrgCreateProcessAsUserA;
static CreateProcessAsUserWDef pOrgCreateProcessAsUserW;
static CreateProcessWithLogonWDef pOrgCreateProcessWithLogonW;
static CreateProcessWithTokenWDef pOrgCreateProcessWithTokenW;
static OpenProcessDef pOrgOpenProcess;
static CreateThreadDef pOrgCreateThread;
static CreateRemoteThreadDef pOrgCreateRemoteThread;
static OpenThreadDef pOrgOpenThread;
static CreateJobObjectADef pOrgCreateJobObjectA;
static CreateJobObjectWDef pOrgCreateJobObjectW;
 
// Mail slot
static CreateMailslotADef pOrgCreateMailslotA;
static CreateMailslotWDef pOrgCreateMailslotW;
 
// pipe
static CreatePipeDef pOrgCreatePipe;
static CreateNamedPipeADef pOrgCreateNamedPipeA;
static CreateNamedPipeWDef pOrgCreateNamedPipeW;

//Registry
static RegCreateKeyExADef pOrgRegCreateKeyExA;
static RegCreateKeyExWDef pOrgRegCreateKeyExW ;
static RegCreateKeyTransactedADef pOrgRegCreateKeyTransactedA;
static RegCreateKeyTransactedWDef pOrgRegCreateKeyTransactedW;
static RegOpenCurrentUserDef pOrgRegOpenCurrentUser;
static RegOpenKeyADef pOrgRegOpenKeyA ;
static RegOpenKeyWDef pOrgRegOpenKeyW ;
static RegOpenKeyExADef pOrgRegOpenKeyExA ;
static RegOpenKeyExWDef pOrgRegOpenKeyExW ;
static RegOpenKeyTransactedADef pOrgRegOpenKeyTransactedA ;
static RegOpenKeyTransactedWDef pOrgRegOpenKeyTransactedW ;
static RegOpenUserClassesRootDef pOrgRegOpenUserClassesRoot;
static RegCreateKeyADef pOrgRegCreateKeyA ;
static RegCreateKeyWDef pOrgRegCreateKeyW ;
static RegCloseKeyDef pOrgRegCloseKey ;

static DuplicateHandleDef pOrgDuplicateHandle;
static CloseHandleDef pOrgCloseHandle;

////////////////////////////////v3 additions//////////////////////////////////////////////////
// Timers
static CreateTimerQueueDef      pOrgCreateTimerQueue;
static CreateTimerQueueTimerDef pOrgCreateTimerQueueTimer;
static DeleteTimerQueueTimerDef pOrgDeleteTimerQueueTimer;
static DeleteTimerQueueExDef    pOrgDeleteTimerQueueEx;
static DeleteTimerQueueDef      pOrgDeleteTimerQueue;

//Critical section
static InitializeCriticalSectionDef             pOrgInitializeCriticalSection;
static InitializeCriticalSectionExDef           pOrgInitializeCriticalSectionEx;
static InitializeCriticalSectionAndSpinCountDef pOrgInitializeCriticalSectionAndSpinCount;
static DeleteCriticalSectionDef                 pOrgDeleteCriticalSection;
////////////////////////////////v3 additions//////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////


typedef struct tag_HOOKFUNCDESC
{
	LPCSTR szFunc; // The name of the function to hook.
	DWORD dwOrdinal;
	PROC pProc;    // The procedure to blast in.
	PROC pOrigProc;    // The procedure to blast in.
	LPCTSTR lpszDllName;
} HOOKFUNCDESC , * LPHOOKFUNCDESC;


PIMAGE_IMPORT_DESCRIPTOR GetNamedImportDescriptor( HMODULE hModule, LPCSTR szImportMod );
BOOL HookDynamicLoadedFun( UINT uiCount, LPHOOKFUNCDESC paHookArray, bool bRestore = false );

#endif //__HOOK_FUNCTIONS_H__