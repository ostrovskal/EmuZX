
#include "stdafx.h"
#include "afxwinappex.h"
#include "AsmVisualStudio.h"

#include "ChildFrm.h"
#include "DocumentAsmEditor.h"
#include "ViewAsmEditor.h"
#include "DocumentTxtEditor.h"
#include "ViewTxtEditor.h"
#include "DocumentBinEditor.h"
#include "ViewBinEditor.h"
#include "DocumentOgxEditor.h"
#include "ViewOgxEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAsmVisualStudioApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAsmVisualStudioApp::OnAppAbout)
END_MESSAGE_MAP()

CAsmVisualStudioApp theApp;

static void AddRegistryItem(const char* key, const char* name, const char* val)
{
	CRegKey reg_key;
	// создаем ключь
	reg_key.Create(HKEY_CLASSES_ROOT, key);
	// устанавливаем значение ключа
	reg_key.SetValue(val, name);
	reg_key.Close();
}

static void RegisteredExtemsionToRegistry(HANDLE m_hInstance)
{
	CString prg_name;
	GetModuleFileName((HMODULE)m_hInstance, prg_name.GetBuffer(MAX_PATH), MAX_PATH);
	prg_name.ReleaseBuffer();
	CString cmd("\"" + prg_name + "\" %1");
	// Добавляем информацию для файла проекта
	AddRegistryItem(".asmp", "", "AsmVisualStudio project");
	AddRegistryItem(".asmp\\ShellNew", "NullFile", "");
	AddRegistryItem("AsmVisualStudio project\\DefaultIcon", "", prg_name + ",9");
	AddRegistryItem("AsmVisualStudio project\\Shell\\open\\command", "", cmd);
	// Добавляем информацию для файла системы
	AddRegistryItem(".asms", "", "AsmVisualStudio solution");
	AddRegistryItem(".asms\\ShellNew", "NullFile", "");
	AddRegistryItem("AsmVisualStudio solution\\DefaultIcon", "", prg_name + ",11");
	AddRegistryItem("AsmVisualStudio solution\\Shell\\open\\command", "", cmd);
	// Добавляем информацию для архивного файла
	AddRegistryItem(".arch", "", "AsmVisualStudio Archive");
	AddRegistryItem(".arch\\ShellNew", "NullFile", "");
	AddRegistryItem("AsmVisualStudio Archive\\DefaultIcon", "", prg_name + ",10");
	AddRegistryItem("AsmVisualStudio Archive\\Shell\\open\\command", "", cmd);
	// Добавляем информацию для файла реализации
	AddRegistryItem(".asm", "", "AsmVisualStudio source");
	AddRegistryItem(".asm\\ShellNew", "NullFile", "");
	AddRegistryItem("AsmVisualStudio source\\DefaultIcon", "", prg_name + ",12");
	AddRegistryItem("AsmVisualStudio source\\Shell\\open\\command", "", cmd);
	// Добавляем информацию для заголовочного файла
	AddRegistryItem(".inc", "", "AsmVisualStudio header");
	AddRegistryItem(".inc\\ShellNew","NullFile", "");
	AddRegistryItem("AsmVisualStudio header\\DefaultIcon", "", prg_name + ",13");
	AddRegistryItem("AsmVisualStudio header\\Shell\\open\\command", "", cmd);
}


static bool Make32BitIcons(const char* src_path, const char* dst_path)
{
	/*
	CFile f;
	if(!f.Open(src_path, CFile::modeRead))
		return false;
	DWORD len(f.GetLength());
	BYTE* src = new BYTE[len];
	f.Read(src, len);
	f.Close();
	DWORD len1 = (len - 54);
	int count = len1 / 3;
	len1 += count;
	*(BYTE*)(src + 28) = 32;
	BYTE* dst = new BYTE[len1];
	memset(dst, 255, len1);
	BYTE* src1 = src + 54;
	BYTE* dst1 = dst;
	for(int i = 0 ; i < count ; i++)
	{
		BYTE b1, b2, b3;
		b1 = *(src1);
		b2 = *(src1 + 1);
		b3 = *(src1 + 2);
		*(dst1) = b1;
		*(dst1 + 1) = b2;
		*(dst1 + 2) = b3;
		if(b1 == 0x80 && b2 == 0x80 && b3 == 0)
		{
			*(dst1 + 3) = 0;
		}
		src1 += 3;
		dst1 += 4;
	}
	if(!f.Open(dst_path, CFile::modeCreate | CFile::modeWrite))
		return false;
	f.Write(src, 54);
	f.Write(dst, len1);
	f.Close();
	delete src;
	delete dst;
	*/
	return true;
}

BOOL CAsmVisualStudioApp::InitInstance()
{
//	Make32BitIcons("c:\\bitmap1.bmp", "c:\\bitmap32.bmp");
//	return FALSE;
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinAppEx::InitInstance();
	if(!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	SetRegistryKey(_T("AsmStudio3_1"));
	LoadStdProfileSettings(15);
	InitShellManager();
	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);
	if(!(docTemplAsm = new CMultiDocTemplate(IDR_MENU_TYPE, RUNTIME_CLASS(CDocumentAsmEditor), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CViewAsmEditor))))
		return FALSE;
	if(!(docTemplTxt = new CMultiDocTemplate(IDR_MENU_TYPE, RUNTIME_CLASS(CDocumentTxtEditor), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CViewTxtEditor))))
		return FALSE;
	if(!(docTemplBin = new CMultiDocTemplate(IDR_MENU_TYPE, RUNTIME_CLASS(CDocumentBinEditor), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CViewBinEditor))))
		return FALSE;
	if(!(docTemplOgx = new CMultiDocTemplate(IDR_MENU_TYPE, RUNTIME_CLASS(CDocumentOgxEditor), RUNTIME_CLASS(CChildFrame), RUNTIME_CLASS(CViewOgxEditor))))
		return FALSE;
	AddDocTemplate(docTemplAsm);
	AddDocTemplate(docTemplTxt);
	AddDocTemplate(docTemplBin);
	AddDocTemplate(docTemplOgx);
	frm = new CMainFrame;
	if(!frm || !frm->LoadFrame(IDR_MAINFRAME))
	{
		delete frm;
		return FALSE;
	}
	m_pMainWnd = frm;
	splash = new CSplashWnd;
	splash->Create();
	splash->UpdateWindow();
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	frm->ShowWindow(SW_SHOWMAXIMIZED);
	frm->UpdateWindow();
	frm->GetConfiguration()->UpdateAllFonts();
	RegisteredExtemsionToRegistry(m_hInstance);
	CString cmd(GetCommandLine()), ext;
	// убрать из коммандной строки путь к запускаемой программе
	CString prg(GetEnvVar(SF_MAIN_DIR) + "\\" + GetEnvVar(SF_PROGRAMM_NAME) + ".exe");
	cmd = cmd.Mid(prg.GetLength() + 4);
	bool is_open;
	if(cmd.GetLength() > 0)
	{
		ext = cmd.Mid(cmd.ReverseFind('.'));
		ext.MakeLower();
		if(ext == EXT_SLN)
		{
			// открыть систему
			if((is_open = frm->GetSolution()->Open(cmd)))
				is_open = frm->OpenSolution();
			if(!is_open)
				MessageBox(NULL, "Ошибка. Не удалось открыть систему!", ASM_STUDIO, MB_ICONERROR);
		}
		else if(ext == EXT_PRJ)
		{
			// открыть проект
			if((is_open = (frm->GetSolution()->AddProject(NULL, NULL, cmd, NULL, ACTION_OPEN_PROJECT)) != NULL))
				is_open = frm->OpenSolution();
			if(!is_open)
				MessageBox(NULL, "Ошибка. Не удалось открыть проект!", ASM_STUDIO, MB_ICONERROR);
		}
		else if(ext == EXT_ARCH)
		{
			// открыть архив
			frm->OpenArchive(cmd);
		}
		else
		{
			// открыть документ
			if(!frm->OpenDocument(cmd, NULL, true))
				MessageBox(NULL, "Ошибка. Не удалось открыть документ!", ASM_STUDIO, MB_ICONERROR);
		}
	}

	return TRUE;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	enum {IDD = IDD_ABOUTBOX};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CAsmVisualStudioApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAsmVisualStudioApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu("PopupFind", IDR_MENU_POPUP_FIND);
	GetContextMenuManager()->AddMenu("PopupOutput", IDR_MENU_POPUP_OUTPUT);
	GetContextMenuManager()->AddMenu("PopupTask", IDR_MENU_POPUP_TASK);
	GetContextMenuManager()->AddMenu("PopupArchiveResource", IDR_MENU_POPUP_ARCHIVE_RESOURCE);
	GetContextMenuManager()->AddMenu("PopupArchiveRoot", IDR_MENU_POPUP_ARCHIVE_ROOT);
	GetContextMenuManager()->AddMenu("PopupFilesFile", IDR_MENU_POPUP_FILES_FILE);
	GetContextMenuManager()->AddMenu("PopupFilesRoot", IDR_MENU_POPUP_FILES_ROOT);
	GetContextMenuManager()->AddMenu("PopupFilesFolder", IDR_MENU_POPUP_FILES_FOLDER);
	GetContextMenuManager()->AddMenu("PopupModulesRoot", IDR_MENU_POPUP_MODULES_ROOT);
	GetContextMenuManager()->AddMenu("PopupModulesFolder", IDR_MENU_POPUP_MODULES_FOLDER);
	GetContextMenuManager()->AddMenu("PopupModulesElement", IDR_MENU_POPUP_MODULES_ELEMENT);
	GetContextMenuManager()->AddMenu("PopupModulesModule", IDR_MENU_POPUP_MODULES_MODULE);
	GetContextMenuManager()->AddMenu("PopupDocument", IDR_MENU_POPUP_DOCUMENT);
	GetContextMenuManager()->AddMenu("PopupCodeEditor", IDR_MENU_POPUP_CODE_EDITOR);
}

BOOL CAsmVisualStudioApp::SaveAllModified()
{
	frm->SaveModified();
	m_pRecentFileList->WriteList();
	return CWinAppEx::SaveAllModified();
}

CMFCPropertyGridProperty* CAsmVisualStudioApp::GetGridProperty(PROPERTY* o, DWORD_PTR dwData)
{
	Array<String> arr;
	CMFCPropertyGridProperty* prop = NULL;
	size_t i;

	switch(o->flags&FOPT_TYPE)
	{
		case FOPT_BOOL:
			prop = new CMFCPropertyGridProperty(o->name, (_variant_t)(bool)(o->lVal == TRUE), o->tip, dwData);
			break;
		case FOPT_COMBO:
			Utils::explode(",", o->opt_val, arr);
			prop = new CMFCPropertyGridProperty(o->name, o->opt, o->tip, dwData);
			prop->AllowEdit(FALSE);
			for(i = 0 ; i < arr.size() ; i++)
				prop->AddOption(arr[i]);
			break;
		case FOPT_STRING:
			prop = new CMFCPropertyGridProperty(o->name, o->opt, o->tip, dwData);
			break;
		case FOPT_PATH:
			// defext,filter
			prop = new CMFCPropertyGridFileProperty(o->name, TRUE, o->opt, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, o->opt_val + "\0\0", o->tip, dwData);
			break;
		case FOPT_FOLDER:
			prop = new CMFCPropertyGridFileProperty(o->name, o->opt, dwData);
			break;
		case FOPT_FONT:
			{
				LOGFONT lf;
				CFont* font(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
				font->GetLogFont(&lf);
				lstrcpy(lf.lfFaceName, o->opt);
				prop = new CMFCPropertyGridFontProperty(o->name, lf, CF_EFFECTS | CF_SCREENFONTS, o->tip, dwData);
			}
			break;
		case FOPT_COLOR:
			prop = new CMFCPropertyGridColorProperty(o->name, o->lVal, NULL, o->tip, dwData);
			prop->AllowEdit(FALSE);
			((CMFCPropertyGridColorProperty*)prop)->EnableOtherButton(o->opt_val);
			break;
		case FOPT_NUMBER:
			Utils::explode(",", o->opt_val, arr);
			prop = new CMFCPropertyGridProperty(o->name, (_variant_t)o->lVal, o->tip, dwData);
			if(arr.size() == 2 && (arr[0] != 0 && arr[1] != 0))
				prop->EnableSpinControl(TRUE, atol(arr[0]), atol(arr[1]));
			break;
	}

	return prop;
}

