
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "NewDocumentDlg.h"
#include "MainFrm.h"

IMPLEMENT_DYNAMIC(CNewDocumentDlg, CDialog)

static char* m_elems_file[] = 
{
	"Исходный (asm)", ".asm",
	"Заголовочный (inc)", ".inc",
	"Текстовый (txt)", ".txt",
	"Бинарный (bin)", ".bin",
	NULL
};

void CNewDocumentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_EDIT_FOLDER, m_folder);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_LIST_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_FOLDER, m_s_folder);
}

BEGIN_MESSAGE_MAP(CNewDocumentDlg, CDialog)
	ON_EN_UPDATE(IDC_EDIT_FOLDER, &CNewDocumentDlg::OnEnUpdateEditFolder)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CNewDocumentDlg::OnEnUpdateEditName)
END_MESSAGE_MAP()

BOOL CNewDocumentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LVITEMA item;
	CBitmap bmp;
	BITMAP bmpObj;

	if(!bmp.LoadBitmap(IDB_BITMAP_ICONS_FILES))
		return -1;
	bmp.GetBitmap(&bmpObj);
	img_list.Create(32, bmpObj.bmHeight, ILC_MASK | ILC_COLOR24, 0, 0);
	img_list.Add(&bmp, RGB(255, 0, 0));
	m_folder.EnableWindow(action == ACTION_NEW_DOCUMENT);
	m_s_folder.EnableWindow(action == ACTION_NEW_DOCUMENT);
	// установить элементы списка
	m_type.SetImageList(&img_list, LVSIL_NORMAL);
	item.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
	item.iIndent = 2;
	item.iSubItem = 0;
	for(int i = 0 ; i < 8 ; i += 2)
	{
		item.pszText = (char*)m_elems_file[i];
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

void CNewDocumentDlg::OnEnUpdateEditFolder()
{
	m_folder.GetWindowText(folder);
	m_ok.EnableWindow(name != "" && folder != "");
}

void CNewDocumentDlg::OnEnUpdateEditName()
{
	m_name.GetWindowText(name);
	m_ok.EnableWindow(name != "" && folder != "");
}

void CNewDocumentDlg::OnOK()
{
	int nType = 0;
	POSITION pos(m_type.GetFirstSelectedItemPosition());
	if(pos)
		nType = m_type.GetNextSelectedItem(pos);
	ext = m_elems_file[nType * 2 + 1];

	CDialog::OnOK();
}

void CNewDocumentDlg::OnEnUpdateEditNameSolution()
{
	m_sln.GetWindowText(name_sln);
	m_ok.EnableWindow(name != "" && folder != "");
}
