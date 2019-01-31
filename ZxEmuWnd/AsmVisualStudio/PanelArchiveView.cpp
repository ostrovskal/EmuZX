
#include "stdafx.h"
#include "Resource.h"
#include "AsmVisualStudio.h"
#include "PanelArchiveView.h"
#include "AddResourceDlg.h"
#include "AddNewNameDlg.h"
#include "ArchiveNewDlg.h"
#include "ArchiveOptionsDlg.h"
#include "ShowUnOpenProjectDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPanelArchiveView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, &CPanelArchiveView::OnProperties)
	ON_COMMAND(ID_ADD_FOLDER, &CPanelArchiveView::OnAddFolder)
	ON_COMMAND(ID_ADD_RESOURCE, &CPanelArchiveView::OnAddResource)
	ON_COMMAND(ID_ADD_NEW_RESOURCE, &CPanelArchiveView::OnAddNewResource)
	ON_COMMAND(ID_REMOVE, &CPanelArchiveView::OnRemove)
	ON_COMMAND(ID_RENAME, &CPanelArchiveView::OnRename)
	ON_COMMAND(ID_GOTO_LOCATION, &CPanelArchiveView::OnGotoLocation)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CPanelArchiveView::OnUpdateArchivePanelResource)
	ON_UPDATE_COMMAND_UI(ID_REMOVE, &CPanelArchiveView::OnUpdateArchivePanelResource)
	ON_UPDATE_COMMAND_UI(ID_RENAME, &CPanelArchiveView::OnUpdateArchivePanelResource)
	ON_UPDATE_COMMAND_UI(ID_ADD_FOLDER, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_UPDATE_COMMAND_UI(ID_ADD_RESOURCE, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_UPDATE_COMMAND_UI(ID_ADD_NEW_RESOURCE, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_OPTIMIZED, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_UPDATE_COMMAND_UI(ID_CREATE_LIST_RESOURCE, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_UPDATE_COMMAND_UI(ID_RESTORE_RESOURCE, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_OPTIONS, &CPanelArchiveView::OnUpdateArchivePanel)
	ON_REGISTERED_MESSAGE(WM_TREE_KEY_DELETE, &CPanelArchiveView::OnTreeViewKeyDelete)
	ON_REGISTERED_MESSAGE(WM_TREE_DBLCLK, &CPanelArchiveView::OnTreeViewDoubleClick)
	ON_REGISTERED_MESSAGE(WM_TREE_END_DRAG, &CPanelArchiveView::OnTreeViewEndDrag)
	ON_REGISTERED_MESSAGE(WM_TREE_END_EDIT, &CPanelArchiveView::OnTreeViewEndEdit)
END_MESSAGE_MAP()

bool CPanelArchiveView::RemoveResourceModule(const char* progID)
{
	for(size_t i = 0 ; i < activex.size() ; i++)
	{
		if(activex[i]->progID == progID)
		{
			activex.del(i);
			return true;
		}
	}

	return false;
}

CPanelArchiveView::ACTIVEX* CPanelArchiveView::AddResourceModule(const char* name, const char* progID)
{
	CLSID clsid;
	IOGX_View* xView(NULL);

	if(SUCCEEDED(CLSIDFromProgID(CComBSTR(progID), &clsid)))
	{
		for(size_t i = 0 ; i < activex.size() ; i++)
		{
			if(activex[i]->clsid == clsid)
				return false;
		}
		// запустить модуль
		if(SUCCEEDED(CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IOGX_View, (void**)&xView)))
		{
			try
			{
				ACTIVEX* ax;
				HICON hIcon;
				CComBSTR bstrDesc, bstrExt;
				// взять параметры(описание, расширение, иконку)
				if(FAILED(xView->GetDescription(&bstrDesc, &bstrExt, (OLE_HANDLE*)&hIcon)))
					throw(0);
				// добавить модуль
				xView->Release();
				ax = new ACTIVEX(name, progID, CString(bstrExt), CString(bstrDesc), clsid, hIcon);
				activex.add(ax);
				m_imgTreeArchive.Add(hIcon);
				return ax;
			}
			catch(...)
			{
				xView->Release();
				xView = NULL;
			}
		}
	}
	return NULL;
}

int CPanelArchiveView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!m_wndTreeArchive.Create(WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS, CRect(0, 0, 1, 1), this, 4))
		return -1;
	m_imgTreeArchive.Create(IDB_BITMAP_TREE_PANEL_MODULES, 16, 0, RGB(255, 0, 0));
	m_wndTreeArchive.SetImageList(&m_imgTreeArchive, TVSIL_NORMAL);
	m_ToolBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY, IDR_TOOLBAR_ARCHIVE);
	m_ToolBar.LoadToolBar(IDR_TOOLBAR_ARCHIVE, 0, 0, TRUE);
	m_ToolBar.SetOwner(this);
	m_ToolBar.SetRouteCommandsViaFrame(FALSE);
	AdjustLayout();
	return 0;
}

void CPanelArchiveView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPanelArchiveView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		int cyTlb = m_ToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_ToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndTreeArchive.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelArchiveView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectTree;
	m_wndTreeArchive.GetWindowRect(rectTree);
	ScreenToClient(rectTree);
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPanelArchiveView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndTreeArchive.SetFocus();
}

void CPanelArchiveView::OnGotoLocation()
{

}

void CPanelArchiveView::OnProperties()
{
	ostd::CommonResource* r;
	switch(m_wndTreeArchive.dataCurrentItem.lParam)
	{
		case TTE_ELEMENT:
			theApp.frm->GetPanelProperties()->UpdateAll(CPanelPropertiesView::IT_RESOURCE, r, NULL, true);
			break;
		case TTE_FOLDER:
		case TTE_ROOT:
			OnArchiveOptions();
			break;
	}
}

void CPanelArchiveView::OnAddFolder()
{
	long i_folder(1);
	CString pathBase(m_wndTreeArchive.GetItemText(m_wndTreeArchive.GetItemRoot(m_wndTreeArchive.dataCurrentItem.hItem)));
	if(m_wndTreeArchive.pathCurrentItem != "")
		pathBase += "\\" + m_wndTreeArchive.pathCurrentItem;
	CString path(pathBase + "\\Новая папка");
	while(m_wndTreeArchive.GetItem(path))
	{
		path.Format("%s\\Новая папка(%i)", pathBase, i_folder);
		i_folder++;
	}
	m_wndTreeArchive.StartEdit(m_wndTreeArchive.AddItem(path, 0, 0, TTE_FOLDER, TTE_FOLDER, true));
}

void CPanelArchiveView::OnRemove()
{
	switch(m_wndTreeArchive.dataCurrentItem.lParam)
	{
		case TTE_ELEMENT:
			break;
		case TTE_FOLDER:
			break;
	}
}

void CPanelArchiveView::OnRename()
{
	switch(m_wndTreeArchive.dataCurrentItem.lParam)
	{
		case TTE_ELEMENT:
			break;
		case TTE_FOLDER:
			break;
		case TTE_ROOT:
			break;
	}
}

void CPanelArchiveView::OnUpdateArchivePanelResource(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_wndTreeArchive.dataCurrentItem.lParam == TTE_ELEMENT);
}

void CPanelArchiveView::OnUpdateArchivePanel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(arch != NULL);
}

void CPanelArchiveView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt, pt1;
	GetCursorPos(&pt) ; pt1 = pt;
	m_wndTreeArchive.ScreenToClient(&pt);

	if((m_wndTreeArchive.HitTest(pt)))
	{
		int nTypeMenu;
		switch(m_wndTreeArchive.dataCurrentItem.lParam)
		{
			case TTE_ROOT:
				nTypeMenu = IDR_MENU_POPUP_ARCHIVE_ROOT;
				break;
			case TTE_FOLDER:
				nTypeMenu = IDR_MENU_POPUP_ARCHIVE_FOLDER;
				break;
			default:
				nTypeMenu = IDR_MENU_POPUP_ARCHIVE_RESOURCE;
				break;
		}
		theApp.GetContextMenuManager()->ShowPopupMenu(nTypeMenu, pt1.x, pt1.y, this, TRUE);
	}
}

bool CPanelArchiveView::ArchiveOpen(const char* path, const char* sign, const char* path_lst, bool is_open)
{
	Archive* n_arch(NULL);
	String res_path, disk_path, name_arch(path), ext;
	bool is_begin(true), result;
	int nImage;

	n_arch = NULL;//new Archive;
	if(!(result = (is_open ? n_arch->open(path, false) : n_arch->create(path, sign, path_lst, true))))
	{
		delete n_arch;
		return false;
	}
	// закрыть старый
	OnArchiveClose();
	arch = n_arch;
	// авто сохранение старой копии
	if(is_open && result)
	{
		if(theApp.frm->GetConfiguration()->IsBackupArchive())
		{
			CString path_bak(path);
			path_bak = path_bak.Left(path_bak.ReverseFind('.')) + "bak";
			::CopyFile(path, path_bak, FALSE);
		}
	}
	// имя архива
	name_arch = name_arch.mid(name_arch.reverseFind('\\') + 1);
	m_wndTreeArchive.AddItem(name_arch, 5, 5, TTE_ROOT, TTE_ROOT, false);
	// перечислить ресурсы
	while(arch->enumerate(res_path, disk_path, is_begin))
	{
		is_begin = false;
		// по расширению определить номер изображения
		nImage = -5;
		ext = (res_path.mid(res_path.reverseFind('.' + 1)));
		ext.makeLower();
		for(size_t index = 0 ; index < activex.size() ; index++)
		{
			if(activex[index]->ext == ext)
			{
				nImage = index + 5;
				break;
			}
		}
		m_wndTreeArchive.AddItem(name_arch + "\\" + res_path, 0, nImage, TTE_FOLDER, TTE_ELEMENT, false);
	}
	return true;
}

void CPanelArchiveView::OnArchiveNew()
{
	CArchiveNewDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		OnArchiveClose();
		if(!ArchiveOpen(dlg.folder_arch + dlg.name + ".arch", dlg.signature, dlg.folder_lst, false))
			MessageBox("Ошибка. Не удалось создать архив!", "Ошибка", MB_ICONERROR);
	}
}

void CPanelArchiveView::OnArchiveOpen()
{
	String folder, cur_dir;
	if(Utils::dlgSaveOrOpenFile(true, "Открыть архива", "Файлы архивов(*.arch)|*.arch||","arch", folder, m_hWnd, NULL))
	{
		OnArchiveClose();
		Utils::getSysDir(NULL, &cur_dir);
		if(folder.reverseFind('\\') == -1)
			folder = cur_dir + folder;
		if(!ArchiveOpen(folder, NULL, NULL, TRUE))
			MessageBox("Не удалось открыть архив !", "Ошибка", MB_ICONERROR);
	}
}

void CPanelArchiveView::OnArchiveClose()
{
	if(arch)
	{
		// закрыть все документы
		theApp.docTemplOgx->CloseAllDocuments(FALSE);
		// сброс дерева
		m_wndTreeArchive.Reset();
		// стереть архив
		delete arch ; arch = NULL;
	}
}

void CPanelArchiveView::OnAddResource()
{
	CAddResourceDlg dlg;
	if(dlg.DoModal() == IDOK)
	{

	}
}

void CPanelArchiveView::OnAddNewResource()
{
	CAddResourceDlg dlg;
	if(dlg.DoModal() == IDOK)
	{

	}
}

void CPanelArchiveView::OnArchiveOptimized()
{
	if(MessageBox("Запустить оптимизацию архива ? (Эта операция может занять много времени).", "Запрос", MB_ICONQUESTION | MB_YESNO) != IDYES)
	{
		CStringArray arrTotal;
		// осуществить поиск дубликатов
		/*
		int i,j;
		cOgxFindResourceDlg dlg("Поиск дубликатов",archive->getPath());
		Array<cString> arr;
		for(i=0;i<10;i++)
		{
		dlg.compare(m_arr_opt_ext[i],&arr);
		for(j=0;j<arr.getSize();j++)
		arrTotal.Add(arr[j]);
		}
		*/
		CShowUnOpenProjectDlg dlg("Список идентичных ресурсов","Найденные дупликаты ресурсов:", "OK", &arrTotal);
		dlg.DoModal();
	}
}

void CPanelArchiveView::OnCreateListResources()
{

}

void CPanelArchiveView::OnRestoreResources()
{
	int index(0);
	CStringArray lstErr;
//	Resource* common;

	while(true)
	{
		CFile f;
		String name, disk;

		//		if(!arch->enumResources(index, name, disk))
		//			break;
		index++;
		// проверить реально ли существует файл ?
		if(!f.Open(disk, CFile::modeRead))
		{
			//			new((BaseNameObject**)&common, name) Resource();

			//			if(common->open(NULL))
			{
				//				if(common->save(disk, true))
				lstErr.Add(disk);
			}
			//REL(common);
		}
	}

	if(lstErr.GetSize())
	{
		CShowUnOpenProjectDlg dlg("Ошибка", "Ресурсы, которые физически отсутствовали на диске и были автоматически сохранены !", "OK", &lstErr);
		dlg.DoModal();
	}
}

void CPanelArchiveView::OnArchiveOptions()
{
	CArchiveOptionsDlg dlg;
	dlg.DoModal();
}

void CPanelArchiveView::Reset()
{
	m_wndTreeArchive.Reset();
}

LRESULT CPanelArchiveView::OnTreeViewSelectItem(WPARAM hItem, LPARAM dataItem)
{
	theApp.frm->GetPanelProperties()->UpdateAll(CPanelPropertiesView::IT_RESOURCE, NULL, NULL, false);
	return 0;
}

LRESULT CPanelArchiveView::OnTreeViewKeyDelete(WPARAM hItem, LPARAM dataItem)
{
	return 0;
}

LRESULT CPanelArchiveView::OnTreeViewDoubleClick(WPARAM hItem, LPARAM dataItem)
{
	return 0;
}

LRESULT CPanelArchiveView::OnTreeViewEndDrag(WPARAM hDrop, LPARAM hItem)
{
	return 0;
}

LRESULT CPanelArchiveView::OnTreeViewEndEdit(WPARAM hItem, LPARAM dataItem)
{
	return 0;
}

CPanelArchiveView::ACTIVEX* CPanelArchiveView::GetResourceModule(const char* ext)
{
	ACTIVEX* x(NULL);

	if(ext)
	{
		for(size_t i = 0 ; i < activex.size() ; i++)
		{
			x = activex[i];
			if(x->ext == ext)
				return x;
		}
	}

	return x;
}
