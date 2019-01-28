
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "NewElementDlg.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CNewElementDlg, CDialog)

static char* m_elems_file[] = 
{
	"Исходный (asm)", ".asm",
	"Заголовочный (inc)", ".inc",
	"Текстовый (txt)", ".txt",
	"Бинарный (bin)", ".bin",
	NULL
};

static char* m_elems_project[] = 
{
	"EXE Win32", "EXE Win32",
	"EXE Win64", "EXE Win64",
	"DLL Win32", "DLL Win32",
	"DLL Win64", "DLL Win64",
	"EXE DOS", "EXE DOS",
	"LIB DOS", "LIB DOS",
	NULL
};

void CNewElementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_EDIT_FOLDER, m_folder);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_LIST_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_s_type);
	DDX_Control(pDX, IDC_STATIC_NAME, m_s_name);
	DDX_Control(pDX, IDC_STATIC_FOLDER, m_s_folder);
	DDX_Control(pDX, IDC_EDIT_NAME_SOLUTION, m_sln);
	DDX_Control(pDX, IDC_STATIC_NAME_SOLUTION, m_s_sln);
}

BEGIN_MESSAGE_MAP(CNewElementDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_FOLDER, &CNewElementDlg::OnEnUpdateEditFolder)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CNewElementDlg::OnEnUpdateEditName)
	ON_EN_UPDATE(IDC_EDIT_NAME_SOLUTION, &CNewElementDlg::OnEnUpdateEditNameSolution)
END_MESSAGE_MAP()

BOOL CNewElementDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nIDD = -1, i;
	LVITEMA item;
	CBitmap bmp;
	BITMAP bmpObj;

	switch(action)
	{
		case ACTION_ADD_NEW_DOCUMENT:
		case ACTION_NEW_DOCUMENT:
			SetWindowText("Новый файл");
			m_s_name.SetWindowText("Имя файла:");
			m_s_type.SetWindowText("Типы файлов:");
			nIDD = IDB_BITMAP_ICONS_FILES;
			elems = m_elems_file;
			break;
		case ACTION_NEW_PROJECT:
			m_sln.ShowWindow(SW_NORMAL);
			m_s_sln.ShowWindow(SW_NORMAL);
		case ACTION_ADD_NEW_PROJECT:
			SetWindowText("Новый проект");
			m_s_name.SetWindowText("Имя проекта:");
			m_s_type.SetWindowText("Тип проекта:");
			nIDD = IDB_BITMAP_ICONS_PROJECTS;
			elems = m_elems_project;
			break;
	}
	if (!bmp.LoadBitmap(nIDD))
		return -1;
	bmp.GetBitmap(&bmpObj);
	img_list.Create(32, bmpObj.bmHeight, ILC_MASK | ILC_COLOR24, 0, 0);
	img_list.Add(&bmp, RGB(255, 0, 0));
	m_folder.EnableWindow(action == ACTION_NEW_PROJECT || action == ACTION_NEW_DOCUMENT);
	m_s_folder.EnableWindow(action == ACTION_NEW_PROJECT || action == ACTION_NEW_DOCUMENT);
	// установить элементы списка
	m_type.SetImageList(&img_list, LVSIL_NORMAL);
	item.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
	item.iIndent = 2;
	item.iSubItem = 0;
	for(i = 0 ; i < 20 ; i += 2)
	{
		if(!elems[i])
			break;
		item.pszText = (char*)elems[i];
		item.iItem = i / 2;
		item.iImage = i / 2;
		m_type.InsertItem(&item);
	}
	m_type.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_folder.EnableFolderBrowseButton();
	folder = theApp.frm->GetConfiguration()->GetDefaultProjectFolder();
	m_folder.SetWindowText(folder);
	return TRUE;
}

void CNewElementDlg::OnEnUpdateEditFolder()
{
	m_folder.GetWindowText(folder);
	m_ok.EnableWindow(name != "" && folder != "" && (action == ACTION_NEW_PROJECT ? name_sln != "" : TRUE));
}

void CNewElementDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	if(action == ACTION_NEW_PROJECT)
		m_sln.SetWindowText(name);
	m_ok.EnableWindow(name != "" && folder != "" && (action == ACTION_NEW_PROJECT ? name_sln != "" : TRUE));
}

void CNewElementDlg::OnOK()
{
	int nType = 0;
	POSITION pos(m_type.GetFirstSelectedItemPosition());
	if(pos)
		nType = m_type.GetNextSelectedItem(pos);
	ext = elems[nType * 2 + 1];

	CDialog::OnOK();
}

void CNewElementDlg::OnEnUpdateEditNameSolution()
{
	m_sln.GetWindowText(name_sln);
	m_ok.EnableWindow(name != "" && folder != "" && (action == ACTION_NEW_PROJECT ? name_sln != "" : TRUE));
}
