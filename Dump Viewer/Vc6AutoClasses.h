// Declaration of dual interfaces to objects found in the type library
//  VISUAL STUDIO 97 SHARED OBJECTS (SharedIDE\bin\devshl.dll)

#ifndef __Vc6AutoClasses_H_
#define __Vc6AutoClasses_H_

//#include "appdefs.h"

#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#endif

/////////////////////////////////////////////////////////////////////////////
// Interfaces declared in this file:

interface IApplication;
interface IApplicationEvents;

interface IGenericDocument;
interface IDocuments;

interface IGenericWindow;
interface IWindows;

interface IGenericProject;
interface IProjects;


/////////////////////////////////////////////////////////////////////////////
// Application object

// IApplication interface


///////////////////////////////////////////////////////////////////////////////

#define DsWindowState DWORD
#define DsSaveStatus DWORD
///////////////////////////////////////////////////////////////////////////////
#undef INTERFACE
#define INTERFACE IApplication

DECLARE_INTERFACE_(IApplication, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IApplication methods */
    STDMETHOD(get_Height)(THIS_ long FAR* Height) PURE;
    STDMETHOD(put_Height)(THIS_ long Height) PURE;
    STDMETHOD(get_Width)(THIS_ long FAR* Width) PURE;
    STDMETHOD(put_Width)(THIS_ long Width) PURE;
    STDMETHOD(get_Top)(THIS_ long FAR* Top) PURE;
    STDMETHOD(put_Top)(THIS_ long Top) PURE;
    STDMETHOD(get_Left)(THIS_ long FAR* Left) PURE;
    STDMETHOD(put_Left)(THIS_ long Left) PURE;
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_TextEditor)(THIS_ IDispatch * FAR* texteditor) PURE;
    STDMETHOD(get_Version)(THIS_ BSTR FAR* Version) PURE;
    STDMETHOD(get_Path)(THIS_ BSTR FAR* Path) PURE;
    STDMETHOD(get_CurrentDirectory)(THIS_ BSTR FAR* CurrentDirectory) PURE;
    STDMETHOD(put_CurrentDirectory)(THIS_ BSTR CurrentDirectory) PURE;
    STDMETHOD(get_FullName)(THIS_ BSTR FAR* FullName) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
    STDMETHOD(get_ActiveDocument)(THIS_ IDispatch * FAR* ActiveDocument) PURE;
    STDMETHOD(get_Windows)(THIS_ IDispatch * FAR* Windows) PURE;
    STDMETHOD(get_Documents)(THIS_ IDispatch * FAR* Documents) PURE;
    STDMETHOD(get_ActiveWindow)(THIS_ IDispatch * FAR* ActiveWindow) PURE;
    STDMETHOD(get_WindowState)(THIS_ DsWindowState FAR* windowstate) PURE;
    STDMETHOD(put_WindowState)(THIS_ DsWindowState windowstate) PURE;
    STDMETHOD(get_Debugger)(THIS_ IDispatch * FAR* ppDebugger) PURE;
    STDMETHOD(get_Projects)(THIS_ IDispatch * FAR* Projects) PURE;
    STDMETHOD(get_ActiveConfiguration)(THIS_ IDispatch * FAR* ActiveConfiguration) PURE;
    STDMETHOD(put_ActiveConfiguration)(THIS_ IDispatch * ActiveConfiguration) PURE;
    STDMETHOD(get_Visible)(THIS_ VARIANT_BOOL FAR* Visible) PURE;
    STDMETHOD(put_Visible)(THIS_ VARIANT_BOOL Visible) PURE;
    STDMETHOD(get_ActiveProject)(THIS_ IDispatch * FAR* ActiveProject) PURE;
    STDMETHOD(put_ActiveProject)(THIS_ IDispatch * ActiveProject) PURE;
    STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) PURE;
    STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) PURE;
    STDMETHOD(GetPackageExtension)(THIS_ BSTR szExtensionName, IDispatch * FAR* pExt) PURE;
    STDMETHOD(Quit)(THIS) PURE;
    STDMETHOD(PrintToOutputWindow)(THIS_ BSTR Message) PURE;
    STDMETHOD(ExecuteCommand)(THIS_ BSTR szCommandName) PURE;
    STDMETHOD(AddCommandBarButton)(THIS_ long nButtonType, BSTR szCmdName, long dwCookie) PURE;
    STDMETHOD(AddKeyBinding)(THIS_ BSTR szKey, BSTR szCommandName, BSTR szEditor) PURE;
    STDMETHOD(Build)(THIS_ VARIANT Configuration) PURE;
    STDMETHOD(RebuildAll)(THIS_ VARIANT configuration) PURE;
    STDMETHOD(ExecuteConfiguration)(THIS_ VARIANT Reserved) PURE;
    STDMETHOD(SetAddInInfo)(THIS_ long nInstanceHandle, IDispatch * pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie) PURE;
    STDMETHOD(AddCommand)(THIS_ BSTR szCmdName, BSTR szMethodName, long nBitmapOffset, long dwCookie, VARIANT_BOOL FAR* pbResult) PURE;
    STDMETHOD(EnableModeless)(THIS_ VARIANT_BOOL bEnable) PURE;
    STDMETHOD(Clean)(THIS_ VARIANT configuration) PURE;
    STDMETHOD(get_Errors)(THIS_ long FAR* nErrors) PURE;
    STDMETHOD(get_Warnings)(THIS_ long FAR* nWarnings) PURE;
    STDMETHOD(AddProject)(THIS_ BSTR szName, BSTR szPath, BSTR szType, VARIANT_BOOL bAddDefaultFolders) PURE;
};

// IApplicationEvents interface

#undef INTERFACE
#define INTERFACE IApplicationEvents

DECLARE_INTERFACE_(IApplicationEvents, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IApplicationEvents methods */
    STDMETHOD(BeforeBuildStart)(THIS) PURE;
    STDMETHOD(BuildFinish)(THIS_ long nNumErrors, long nNumWarnings) PURE;
    STDMETHOD(BeforeApplicationShutDown)(THIS) PURE;
    STDMETHOD(DocumentOpen)(THIS_ IDispatch * theDocument) PURE;
    STDMETHOD(BeforeDocumentClose)(THIS_ IDispatch * theDocument) PURE;
    STDMETHOD(DocumentSave)(THIS_ IDispatch * theDocument) PURE;
    STDMETHOD(NewDocument)(THIS_ IDispatch * theDocument) PURE;
    STDMETHOD(WindowActivate)(THIS_ IDispatch * theWindow) PURE;
    STDMETHOD(WindowDeactivate)(THIS_ IDispatch * theWindow) PURE;
    STDMETHOD(WorkspaceOpen)(THIS) PURE;
    STDMETHOD(WorkspaceClose)(THIS) PURE;
    STDMETHOD(NewWorkspace)(THIS) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Document object

// IGenericDocument interface

#undef INTERFACE
#ifndef __IGenericDocument_INTERFACE_DEFINED__
#define __IGenericDocument_INTERFACE_DEFINED__
#define INTERFACE IGenericDocument

DECLARE_INTERFACE_(IGenericDocument, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IGenericDocument methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* pName) PURE;
    STDMETHOD(get_FullName)(THIS_ BSTR FAR* pName) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) PURE;
    STDMETHOD(get_Path)(THIS_ BSTR FAR* pPath) PURE;
    STDMETHOD(get_Saved)(THIS_ VARIANT_BOOL FAR* pSaved) PURE;
    STDMETHOD(get_ActiveWindow)(THIS_ IDispatch * FAR* ppWindow) PURE;
    STDMETHOD(get_ReadOnly)(THIS_ VARIANT_BOOL FAR* pReadOnly) PURE;
    STDMETHOD(put_ReadOnly)(THIS_ VARIANT_BOOL ReadOnly) PURE;
    STDMETHOD(get_Type)(THIS_ BSTR FAR* pType) PURE;
    STDMETHOD(get_Windows)(THIS_ IDispatch * FAR* ppWindows) PURE;
	STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) PURE;
	STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) PURE;
    STDMETHOD(NewWindow)(THIS_ IDispatch * FAR* ppWindow) PURE;
    STDMETHOD(Save)(THIS_ VARIANT vFilename, VARIANT vBoolPrompt, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(Undo)(THIS_ VARIANT_BOOL FAR* pSuccess) PURE;
    STDMETHOD(Redo)(THIS_ VARIANT_BOOL FAR* pSuccess) PURE;
    STDMETHOD(PrintOut)(THIS_ VARIANT_BOOL FAR* pSuccess) PURE;
    STDMETHOD(Close)(THIS_ VARIANT vSaveChanges, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(Reserved1)(THIS) PURE;
    STDMETHOD(Reserved2)(THIS) PURE;
    STDMETHOD(Reserved3)(THIS) PURE;
    STDMETHOD(Reserved4)(THIS) PURE;
    STDMETHOD(Reserved5)(THIS) PURE;
    STDMETHOD(Reserved6)(THIS) PURE;
    STDMETHOD(Reserved7)(THIS) PURE;
    STDMETHOD(Reserved8)(THIS) PURE;
    STDMETHOD(Reserved9)(THIS) PURE;
    STDMETHOD(Reserved10)(THIS) PURE;
};

#endif // __IGenericDocument_INTERFACE_DEFINED__


/////////////////////////////////////////////////////////////////////////////
// Documents collection object

// IDocuments interface

#undef INTERFACE
#define INTERFACE IDocuments

DECLARE_INTERFACE_(IDocuments, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IDocuments methods */
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item) PURE;
    STDMETHOD(SaveAll)(THIS_ VARIANT vtBoolPrompt, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(CloseAll)(THIS_ VARIANT vtSaveChanges, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(Add)(THIS_ BSTR docType, VARIANT vtReserved, IDispatch * FAR* ppDocument) PURE;
    STDMETHOD(Open)(THIS_ BSTR filename, VARIANT vtDocType, VARIANT vtBoolReadOnly, IDispatch * FAR* ppDocument) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Window object

// IGenericWindow interface

#undef INTERFACE
#ifndef __IGenericWindow_INTERFACE_DEFINED__
#define __IGenericWindow_INTERFACE_DEFINED__
#define INTERFACE IGenericWindow

DECLARE_INTERFACE_(IGenericWindow, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IGenericWindow methods */
    STDMETHOD(get_Caption)(THIS_ BSTR FAR* pbstrCaption) PURE;
    STDMETHOD(get_Type)(THIS_ BSTR FAR* pbstrCaption) PURE;
    STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) PURE;
    STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) PURE;
    STDMETHOD(put_Left)(THIS_ long lVal) PURE;
    STDMETHOD(get_Left)(THIS_ long FAR* plVal) PURE;
    STDMETHOD(put_Top)(THIS_ long lVal) PURE;
    STDMETHOD(get_Top)(THIS_ long FAR* plVal) PURE;
    STDMETHOD(put_Height)(THIS_ long lVal) PURE;
    STDMETHOD(get_Height)(THIS_ long FAR* plVal) PURE;
    STDMETHOD(put_Width)(THIS_ long lVal) PURE;
    STDMETHOD(get_Width)(THIS_ long FAR* plVal) PURE;
    STDMETHOD(get_Index)(THIS_ long FAR* plVal) PURE;
    STDMETHOD(get_Next)(THIS_ IDispatch * FAR* ppDispatch) PURE;
    STDMETHOD(get_Previous)(THIS_ IDispatch * FAR* ppDispatch) PURE;
    STDMETHOD(put_WindowState)(THIS_ DsWindowState lVal) PURE;
    STDMETHOD(get_WindowState)(THIS_ DsWindowState FAR* plVal) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppDispatch) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppDispatch) PURE;
    STDMETHOD(Close)(THIS_ VARIANT vtSaveChanges, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(Reserved1)(THIS) PURE;
    STDMETHOD(Reserved2)(THIS) PURE;
    STDMETHOD(Reserved3)(THIS) PURE;
    STDMETHOD(Reserved4)(THIS) PURE;
    STDMETHOD(Reserved5)(THIS) PURE;
    STDMETHOD(Reserved6)(THIS) PURE;
    STDMETHOD(Reserved7)(THIS) PURE;
    STDMETHOD(Reserved8)(THIS) PURE;
    STDMETHOD(Reserved9)(THIS) PURE;
    STDMETHOD(Reserved10)(THIS) PURE;
};

#endif // __IGenericWindow_INTERFACE_DEFINED__


/////////////////////////////////////////////////////////////////////////////
// Windows collection object

// IWindows interface

#undef INTERFACE
#define INTERFACE IWindows

DECLARE_INTERFACE_(IWindows, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IWindows methods */
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item) PURE;
    STDMETHOD(Arrange)(THIS_ VARIANT vtArrangeStyle) PURE;
    STDMETHOD(CloseAll)(THIS_ VARIANT vtSaveChanges, DsSaveStatus FAR* pSaved) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Project object

// IGenericProject interface

#undef INTERFACE
#ifndef __IGenericProject_INTERFACE_DEFINED__
#define __IGenericProject_INTERFACE_DEFINED__
#define INTERFACE IGenericProject

DECLARE_INTERFACE_(IGenericProject, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IGenericProject methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_FullName)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
    STDMETHOD(get_Type)(THIS_ BSTR FAR* pType) PURE;
    STDMETHOD(Reserved1)(THIS) PURE;
    STDMETHOD(Reserved2)(THIS) PURE;
    STDMETHOD(Reserved3)(THIS) PURE;
    STDMETHOD(Reserved4)(THIS) PURE;
    STDMETHOD(Reserved5)(THIS) PURE;
    STDMETHOD(Reserved6)(THIS) PURE;
    STDMETHOD(Reserved7)(THIS) PURE;
    STDMETHOD(Reserved8)(THIS) PURE;
    STDMETHOD(Reserved9)(THIS) PURE;
    STDMETHOD(Reserved10)(THIS) PURE;
};

#endif // __IGenericProject_INTERFACE_DEFINED__


/////////////////////////////////////////////////////////////////////////
// Projects collection object

// IProjects interface

DECLARE_INTERFACE_(IProjects, IDispatch)
{
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IProjects methods */
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT Index, IGenericProject FAR* FAR* Item) PURE;
};

// text classes

interface ITextDocument;
interface ITextSelection;
interface ITextEditor;


// to remove any redefinitions by the Windows headers
#undef FindText
#undef ReplaceText


/////////////////////////////////////////////////////////////////////////////
// TextDocument object

// ITextDocument interface

#undef INTERFACE
#define INTERFACE ITextDocument

DECLARE_INTERFACE_(ITextDocument, IGenericDocument)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;

    /* IGenericDocument methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* pName) PURE;
    STDMETHOD(get_FullName)(THIS_ BSTR FAR* pName) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) PURE;
    STDMETHOD(get_Path)(THIS_ BSTR FAR* pPath) PURE;
    STDMETHOD(get_Saved)(THIS_ VARIANT_BOOL FAR* pSaved) PURE;
    STDMETHOD(get_ActiveWindow)(THIS_ IDispatch * FAR* ppWindow) PURE;
    STDMETHOD(get_ReadOnly)(THIS_ VARIANT_BOOL FAR* pReadOnly) PURE;
    STDMETHOD(put_ReadOnly)(THIS_ VARIANT_BOOL ReadOnly) PURE;
    STDMETHOD(get_Type)(THIS_ BSTR FAR* pType) PURE;
    STDMETHOD(get_Windows)(THIS_ IDispatch * FAR* ppWindows) PURE;
	STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) PURE;
    STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) PURE;
    STDMETHOD(NewWindow)(THIS_ IDispatch * FAR* ppWindow) PURE;
    STDMETHOD(Save)(THIS_ VARIANT vFilename, VARIANT vBoolPrompt, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(Undo)(THIS_ VARIANT_BOOL FAR* pSuccess) PURE;
    STDMETHOD(Redo)(THIS_ VARIANT_BOOL FAR* pSuccess) PURE;
    STDMETHOD(PrintOut)(THIS_ VARIANT_BOOL FAR* pSuccess) PURE;
    STDMETHOD(Close)(THIS_ VARIANT vSaveChanges, DsSaveStatus FAR* pSaved) PURE;
    STDMETHOD(Reserved1)(THIS) PURE;
    STDMETHOD(Reserved2)(THIS) PURE;
    STDMETHOD(Reserved3)(THIS) PURE;
    STDMETHOD(Reserved4)(THIS) PURE;
    STDMETHOD(Reserved5)(THIS) PURE;
    STDMETHOD(Reserved6)(THIS) PURE;
    STDMETHOD(Reserved7)(THIS) PURE;
    STDMETHOD(Reserved8)(THIS) PURE;
    STDMETHOD(Reserved9)(THIS) PURE;
    STDMETHOD(Reserved10)(THIS) PURE;
#endif

    /* ITextDocument methods */
    STDMETHOD(get_Selection)(THIS_ IDispatch * FAR* ppSelection) PURE;
    STDMETHOD(put_IndentSize)(THIS_ long Size) PURE;
    STDMETHOD(get_IndentSize)(THIS_ long FAR* pSize) PURE;
    STDMETHOD(put_TabSize)(THIS_ long Size) PURE;
    STDMETHOD(get_TabSize)(THIS_ long FAR* pSize) PURE;
    STDMETHOD(put_Language)(THIS_ BSTR Language) PURE;
    STDMETHOD(get_Language)(THIS_ BSTR FAR* pLanguage) PURE;
    STDMETHOD(ReplaceText)(THIS_ BSTR FindText, BSTR ReplaceText, VARIANT Flags, VARIANT_BOOL FAR* pbRetVal) PURE;
    STDMETHOD(ClearBookmarks)(THIS) PURE;
    STDMETHOD(MarkText)(THIS_ BSTR FindText, VARIANT Flags, VARIANT_BOOL FAR* pbRetVal) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// TextSelection object

// ITextSelection interface

enum DsCaseOptions
{
};


#undef INTERFACE
#define INTERFACE ITextSelection

DECLARE_INTERFACE_(ITextSelection, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* ITextSelection methods */
    STDMETHOD(put_Text)(THIS_ BSTR newText) PURE;
    STDMETHOD(get_Text)(THIS_ BSTR FAR* pText) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApp) PURE;
    STDMETHOD(get_CurrentLine)(THIS_ long FAR* plLine) PURE;
    STDMETHOD(get_CurrentColumn)(THIS_ long FAR* plCol) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) PURE;
    STDMETHOD(get_BottomLine)(THIS_ long FAR* plLine) PURE;
    STDMETHOD(get_TopLine)(THIS_ long FAR* plLine) PURE;
	STDMETHOD(Delete)(THIS_ VARIANT Count) PURE;
    STDMETHOD(SelectLine)(THIS) PURE;
    STDMETHOD(Backspace)(THIS_ VARIANT Count) PURE;
    STDMETHOD(StartOfDocument)(THIS_ VARIANT Extend) PURE;
    STDMETHOD(Copy)(THIS) PURE;
    STDMETHOD(Cut)(THIS) PURE;
    STDMETHOD(Paste)(THIS) PURE;
    STDMETHOD(EndOfDocument)(THIS_ VARIANT Extend) PURE;
    STDMETHOD(SelectAll)(THIS) PURE;
    STDMETHOD(Tabify)(THIS) PURE;
    STDMETHOD(Untabify)(THIS) PURE;
    STDMETHOD(Indent)(THIS_ VARIANT Reserved) PURE;
    STDMETHOD(Unindent)(THIS_ VARIANT Reserved) PURE;
    STDMETHOD(CharLeft)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(CharRight)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(LineUp)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(LineDown)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(PageUp)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(PageDown)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(WordLeft)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(WordRight)(THIS_ VARIANT Extend, VARIANT Count) PURE;
    STDMETHOD(EndOfLine)(THIS_ VARIANT Extend) PURE;
    STDMETHOD(StartOfLine)(THIS_ VARIANT MoveTo, VARIANT Extend) PURE;
    STDMETHOD(SmartFormat)(THIS) PURE;
    STDMETHOD(ChangeCase)(THIS_ DsCaseOptions Type) PURE;
    STDMETHOD(DeleteWhitespace)(THIS_ VARIANT Direction) PURE;
    STDMETHOD(Cancel)(THIS) PURE;
    STDMETHOD(GoToLine)(THIS_ long Line, VARIANT Select) PURE;
    STDMETHOD(MoveTo)(THIS_ long Line, long Column, VARIANT Extend) PURE;
    STDMETHOD(FindText)(THIS_ BSTR FindString, VARIANT Flags, VARIANT_BOOL FAR* pbRet) PURE;
    STDMETHOD(PreviousBookmark)(THIS_ VARIANT_BOOL FAR* pbRet) PURE;
    STDMETHOD(NextBookmark)(THIS_ VARIANT_BOOL FAR* pbRet) PURE;
    STDMETHOD(SetBookmark)(THIS) PURE;
    STDMETHOD(ClearBookmark)(THIS_ VARIANT_BOOL FAR* pbRet) PURE;
	STDMETHOD(NewLine)(THIS_ VARIANT Reserved) PURE;
    STDMETHOD(ReplaceText)(THIS_ BSTR FindText, BSTR ReplaceText, VARIANT Flags, VARIANT_BOOL FAR* pbRetVal) PURE;
	STDMETHOD(DestructiveInsert)(THIS_ BSTR szText) PURE;
};










#endif //__Vc6AutoClasses_H_
