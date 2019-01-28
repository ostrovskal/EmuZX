
#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#include "targetver.h"
#include <process.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _AFX_ALL_WARNINGS

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif
#include <afxcontrolbars.h>
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <common\OGX_interfaces.h>

#define OSTROV_STD_DLL
#include <ostd\std_standard.h>

#import "..\Lib\OGX_View.tlb" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search

// ���������� �������� ��������
#define SFN_MAIN_DIR					"$(MainDir)"			// ���������� ������� ���������
#define SFN_WORK_DIR					"$(WorkDir)"			// ������� ����������
#define SFN_TEMP_DIR					"$(TempDir)"			// ��������� ����������
#define SFN_SYS_DIR						"$(SysDir)"				// ��������� ����������
#define SFN_SOLUTION_NAME				"$(SolutionName)"		// ��� �������
#define SFN_SOLUTION_PATH				"$(SolutionPath)"		// ���� � �������
#define SFN_SOLUTION_EXT				"$(SolutionExt)"		// ���������� �������
#define SFN_PROJECT_NAME				"$(ProjectName)"		// ��� �������� �������
#define SFN_PROJECT_PATH				"$(ProjectPath)"		// ���� � �������� �������
#define SFN_PROJECT_EXT					"$(ProjectExt)"			// ���������� �������
#define SFN_DOCUMENT_NAME				"$(DocumentName)"		// ��� �������� ���������
#define SFN_DOCUMENT_PATH				"$(DocumentPath)"		// ���� � �������� ���������
#define SFN_DOCUMENT_EXT				"$(DocumentExt)"		// ���������� �������� ���������
#define SFN_DOCUMENT_MODULES_PATH		"$(DocumentModulesPath)"// ���� � ������� � �������� ���������
#define SFN_DOCUMENT_FILES_PATH			"$(DocumentFilesPath)"	// ���� � ������ � �������� ���������
#define SFN_CONFIGURATION_NAME			"$(ConfigurationName)"	// ��� ������� ������������
#define SFN_CONFIGURATION_PATH			"$(ConfigurationPath)"	// ���� � ������������
#define SFN_INCLUDE_PATH				"$(IncludePath)"		// ���� � ���������� �� ��������� ������
#define SFN_LIB_PATH					"$(LibPath)"			// ���� � ����������� �� ���������
#define SFN_PLATFORM_NAME				"$(PlatformName)"		// ��� ������� ���������
#define SFN_PROGRAMM_NAME				"$(ProgramName)"		// ��� ���������
#define SFN_PROJECT_EXECUTE_NAME		"$(ProjectExecuteName)"	// ��� ������� ��� �������
#define SFN_PROJECT_EXECUTE_PATH		"$(ProjectExecutePath)"	// ���� � ������� ��� �������
// �������� �������� ��������
#define SF_MAIN_DIR						0
#define SF_WORK_DIR						1
#define SF_TEMP_DIR						2
#define SF_SYS_DIR						3
#define SF_SOLUTION_NAME				4
#define SF_SOLUTION_PATH				5
#define SF_SOLUTION_EXT					6
#define SF_PROJECT_NAME					7
#define SF_PROJECT_PATH					8
#define SF_PROJECT_EXT					9
#define SF_DOCUMENT_NAME				10
#define SF_DOCUMENT_PATH				11
#define SF_DOCUMENT_EXT					12
#define SF_DOCUMENT_MODULES_PATH		13
#define SF_DOCUMENT_FILES_PATH			14
#define SF_CONFIGURATION_NAME			15
#define SF_CONFIGURATION_PATH			16
#define SF_INCLUDE_PATH					17
#define SF_LIB_PATH						18
#define SF_PLATFORM_NAME				19
#define SF_PROGRAMM_NAME				20
#define SF_PROJECT_EXECUTE_NAME			21
#define SF_PROJECT_EXECUTE_PATH			22
#define SF_LAST_ENV_VAR					23
// �������������� ���������� ogx
#define ID_OGX_CODE_EDITOR			3000
#define ID_OGX_OUTPUT_FIND			3001
#define ID_OGX_OUTPUT_WINDOW		3002
#define ID_OGX_TXT_EDITOR			3003

#define ID_PANEL_MODULE_VIEW		100
#define ID_PANEL_FILE_VIEW			101
#define ID_PANEL_OUTPUT_VIEW		102
#define ID_PANEL_FIND_VIEW			103
#define ID_PANEL_TASK_VIEW			104
#define ID_PANEL_ARCHIVE_VIEW		105
#define ID_PANEL_PROPERTIES_VIEW	106
// ���� ��������� � ���������
#define FOPT_BOOL					0x80000000
#define FOPT_PATH					0x40000000
#define FOPT_STRING					0x20000000
#define FOPT_NUMBER					0x10000000
#define FOPT_COMBO					0x08000000
#define FOPT_COLOR					0x04000000
#define FOPT_FONT					0x02000000
#define FOPT_FOLDER					0x01000000
#define FOPT_EXECUTE				0x00800000
#define FOPT_ALWAYS					0x00400000
#define FOPT_ASSEMBLER				0x00200000
#define FOPT_LINKER					0x00100000
#define FOPT_TYPE					(FOPT_BOOL | FOPT_PATH | FOPT_STRING | FOPT_NUMBER | FOPT_COMBO | FOPT_COLOR | FOPT_FONT | FOPT_FOLDER)
// ���� ����������
#define TE_OUTPUT					0
#define TE_FIND						1
#define TE_EDITOR					2
#define TE_TEXT						3
#define TE_BINARY					4

#define ACTION_NEW_DOCUMENT			0
#define ACTION_ADD_NEW_DOCUMENT		1
#define ACTION_ADD_OPEN_DOCUMENT	2
#define ACTION_NEW_PROJECT			3
#define ACTION_ADD_NEW_PROJECT		4
#define ACTION_OPEN_PROJECT			5
#define ACTION_ADD_OPEN_PROJECT		6

#define ASM_STUDIO					"AsmVisualStudio"
#define EXT_PRJ						".asmp"
#define EXT_SLN						".asms"
#define EXT_ARCH					".arch"
// ���� ��������� � ������
#define TTE_MODULES_PROC			0
#define TTE_MODULES_STRUCT			1
#define TTE_MODULES_EQU				2
#define TTE_FOLDER					3
#define TTE_ROOT					4
#define TTE_ELEMENT					5

struct PROPERTY
{
	PROPERTY() : flags(-1), lVal(0) {}
	PROPERTY(const char* nm, const char* o, const char* ov, const char* k, const char* kv, const char* tp, long lv, long f) : name(nm), opt(o), opt_val(ov), key(k), key_val(kv), tip(tp), lVal(lv), flags(f) {}
	PROPERTY(PROPERTY* p) : lVal(p->lVal), name(p->name), opt(p->opt), opt_val(p->opt_val), key(p->key), key_val(p->key_val), tip(p->tip), flags(p->flags) {}
	// ��� �����
	CString name;
	// �����
	CString key;
	// �����
	CString opt;
	// �������� �������� �����
	CString opt_val;
	// �������� �������� �����
	CString key_val;
	// ���������
	CString tip;
	// ����� ��������
	long lVal;
	// �����(��� ����������� �����)
	long flags;
};

struct BOOKMARK
{
	BOOKMARK() : number(0), status(0) {}
	BOOKMARK(int num, int sts) : number(num), status(sts) {}
	long number;
	long status;
};

struct WIZARD_CONFIG
{
	// ����������� �� ���������
	WIZARD_CONFIG() {}
	// ����������� �� �����
	WIZARD_CONFIG(const char* nm) : name(nm) {}
	// ����������
	~WIZARD_CONFIG()
	{
		asm_props.deleteAll();
		lnk_props.deleteAll();
	}
	// ���
	CString name;
	// �������� �����������
	Array<PROPERTY*> asm_props;
	// �������� ������������
	Array<PROPERTY*> lnk_props;
};

struct WIZARD_ELEMENT
{
	// ���� � ����� �� �����
	CString path;
	// ���� ����� � ������
	CString path_files;
	// ���� ����� � �������
	CString path_modules;
	// ������� ���������� �� ������
	long is_exclude;
};

struct WIZARD
{
	// ����������� �� ���������
	WIZARD() : hIcon(NULL) {}
	// ����������
	~WIZARD()
	{
		::DeleteObject(hIcon);
		configs.deleteAll();
		elems.deleteAll();
	}
	// ��� �������
	CString name;
	// ��� ���������
	CString name_platform;
	// ���� � ������
	CString path_icon;
	// ������������ � �� ��������
	Array<WIZARD_CONFIG*> configs;
	// ���������
	Array<WIZARD_ELEMENT*> elems;
	// ������ �������
	HICON hIcon;
};

struct SOLUTION_FILTER
{
	CString name;
	CString value;
};

static Utils::STRING_LONG stk_prop_flags[] =
{
	{"always", FOPT_ALWAYS},
	{"execute", FOPT_EXECUTE},
	{"assembler", FOPT_ASSEMBLER},
	{"linker", FOPT_LINKER},
	{"bool", FOPT_BOOL},
	{"path", FOPT_PATH},
	{"string", FOPT_STRING},
	{"number", FOPT_NUMBER},
	{"select", FOPT_COMBO},
	{"color", FOPT_COLOR},
	{"font", FOPT_FONT},
	{"folder", FOPT_FOLDER}
};

struct TREE_DATA
{
	HTREEITEM hItem;
	DWORD_PTR lParam;
};

CString GetEnvVar(int index);
void SetEnvVar(int index, const char* value);
CString ApplyFilter(const char* txt);

