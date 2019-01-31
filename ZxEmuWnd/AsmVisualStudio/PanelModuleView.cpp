
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "PanelModuleView.h"
#include "PanelPropertiesView.h"
#include "Resource.h"
#include "ModuleAddProcedureDlg.h"
#include "ModuleAddStructureDlg.h"
#include "ModuleAddEquDlg.h"

static CElement::BlockElementTypes m_cnv_modules[] = {CElement::blkProc, CElement::blkStruct, CElement::blkEqu};

BEGIN_MESSAGE_MAP(CPanelModuleView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_RENAME, &CPanelModuleView::OnRename)
	ON_COMMAND(ID_REMOVE, &CPanelModuleView::OnRemove)
	ON_COMMAND(ID_OPEN, &CPanelModuleView::OnOpen)
	ON_COMMAND(ID_PROPERTIES, &CPanelModuleView::OnProperties)
	ON_COMMAND(ID_ADD_FOLDER, &CPanelModuleView::OnAddFolder)
	ON_COMMAND(ID_ADD_EQU, &CPanelModuleView::OnAddEqu)
	ON_COMMAND(ID_ADD_STRUCTURE, &CPanelModuleView::OnAddStructure)
	ON_COMMAND(ID_ADD_PROCEDURE, &CPanelModuleView::OnAddProcedure)
	ON_COMMAND(ID_GOTO_LOCATION, &CPanelModuleView::OnGotoLocation)
	ON_COMMAND(ID_SET_STARTUP, &CPanelModuleView::OnSetStartup)
	ON_COMMAND(ID_SAVE_ALL, &CPanelModuleView::OnSaveAll)
	ON_COMMAND(ID_REBUILD, &CPanelModuleView::OnRebuild)
	ON_COMMAND(ID_BUILD_PROJECT, &CPanelModuleView::OnBuildProject)
	ON_COMMAND(ID_BUILD_SOLUTION, &CPanelModuleView::OnBuildSolution)
	ON_UPDATE_COMMAND_UI(ID_ADD_FOLDER, &CPanelModuleView::OnUpdateAddFolder)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CPanelModuleView::OnUpdateProperties)
	ON_UPDATE_COMMAND_UI(ID_ADD_EQU, &CPanelModuleView::OnUpdateModulesAddBlock)
	ON_UPDATE_COMMAND_UI(ID_ADD_STRUCTURE, &CPanelModuleView::OnUpdateModulesAddBlock)
	ON_UPDATE_COMMAND_UI(ID_ADD_PROCEDURE, &CPanelModuleView::OnUpdateModulesAddBlock)
	ON_REGISTERED_MESSAGE(WM_TREE_KEY_DELETE, &CPanelModuleView::OnTreeViewKeyDelete)
	ON_REGISTERED_MESSAGE(WM_TREE_DBLCLK, &CPanelModuleView::OnTreeViewDoubleClick)
	ON_REGISTERED_MESSAGE(WM_TREE_SELECT, &CPanelModuleView::OnTreeViewSelectItem)
	ON_REGISTERED_MESSAGE(WM_TREE_END_DRAG, &CPanelModuleView::OnTreeViewEndDrag)
	ON_REGISTERED_MESSAGE(WM_TREE_END_EDIT, &CPanelModuleView::OnTreeViewEndEdit)
	ON_COMMAND(ID_SAVE_ALL, &CPanelModuleView::OnSaveAll)
END_MESSAGE_MAP()

int CPanelModuleView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CBitmap bmp;
	BITMAP bmpObj;
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!m_wndTreeModule.Create(WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_HASLINES | TVS_EDITLABELS | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, CRect(0, 0, 1, 1), this, 2))
		return -1;
	if (!bmp.LoadBitmap(IDB_BITMAP_TREE_PANEL_MODULES))
		return -1;
	fileView = theApp.frm->GetPanelFiles();
	bmp.GetBitmap(&bmpObj);
	m_imgTreeModule.Create(16, bmpObj.bmHeight, ILC_MASK | ILC_COLOR24, 0, 0);
	m_imgTreeModule.Add(&bmp, RGB(255, 0, 0));
	m_wndTreeModule.SetImageList(&m_imgTreeModule, TVSIL_NORMAL);
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TOOLBAR_MODULES);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_MODULES, 0, 0, TRUE);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	return 0;
}

void CPanelModuleView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPanelModuleView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndTreeModule.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelModuleView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectTree;
	m_wndTreeModule.GetWindowRect(rectTree);
	ScreenToClient(rectTree);
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPanelModuleView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndTreeModule.SetFocus();
}

void CPanelModuleView::SetProperties(bool is)
{
	CElement::BLOCK_ELEMENT* blk(NULL);
	int nType(CPanelPropertiesView::IT_DEFAULT);
	switch(m_wndTreeModule.dataCurrentItem.lParam)
	{
		case TTE_ELEMENT:
			nType = CPanelPropertiesView::IT_MODULE;
			break;
		case TTE_MODULES_EQU:
			nType = CPanelPropertiesView::IT_EQU;
			break;
		case TTE_MODULES_PROC:
			nType = CPanelPropertiesView::IT_PROCEDURE;
			break;
		case TTE_MODULES_STRUCT:
			nType = CPanelPropertiesView::IT_STRUCTURE;
			break;
	}
	switch(nType)
	{
		case CPanelPropertiesView::IT_EQU:
		case CPanelPropertiesView::IT_STRUCTURE:
		case CPanelPropertiesView::IT_PROCEDURE:
			blk = elem->GetBlock(m_wndTreeModule.pathCurrentItem.Mid(m_wndTreeModule.pathCurrentItem.ReverseFind('\\') + 1), (CElement::BlockElementTypes)nType);
			break;
	}
	theApp.frm->GetPanelProperties()->UpdateAll(nType, elem, blk, is);
}

void CPanelModuleView::OnProperties()
{
	SetProperties(true);
	theApp.frm->OnProjectOptions();
}

void CPanelModuleView::OnAddFolder()
{
	long i_folder(1);
	static int iii(0);
	iii++;
	if(iii >1)
	{
		int xxx=0;
	}
	CString pathBase(m_wndTreeModule.GetItemText(m_wndTreeModule.GetItemRoot(m_wndTreeModule.dataCurrentItem.hItem)));
	if(m_wndTreeModule.pathCurrentItem != "")
		pathBase += "\\" + m_wndTreeModule.pathCurrentItem;
	CString path(pathBase + "\\Новая папка");
	while(m_wndTreeModule.GetItem(path))
	{
		path.Format("%s\\Новая папка(%i)", pathBase, i_folder);
		i_folder++;
	}
	m_wndTreeModule.StartEdit(m_wndTreeModule.AddItem(path, 0, 0, TTE_FOLDER, TTE_FOLDER, true));
}

void CPanelModuleView::OnAddEqu()
{
	CModuleAddEquDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		MessageBox(dlg.text, ASM_STUDIO, MB_OK);
	}
}

void CPanelModuleView::OnAddStructure()
{
	CModuleAddStructureDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		MessageBox(dlg.text, ASM_STUDIO, MB_OK);
	}
}

void CPanelModuleView::OnAddProcedure()
{
	CModuleAddProcedureDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		MessageBox(dlg.text, ASM_STUDIO, MB_OK);
	}
}

void CPanelModuleView::OnUpdateModulesPanel(CCmdUI *pCmdUI)
{
	DWORD_PTR lParam(m_wndTreeModule.dataCurrentItem.lParam);
	pCmdUI->Enable(lParam == TTE_ELEMENT || lParam == TTE_ROOT);
}

void CPanelModuleView::OnUpdateModulesAddBlock(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_wndTreeModule.dataCurrentItem.lParam == TTE_ELEMENT);
}

void CPanelModuleView::OnUpdateProperties(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_wndTreeModule.dataCurrentItem.lParam != -1);
}

void CPanelModuleView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt, pt1;

	GetCursorPos(&pt) ; pt1 = pt;
	m_wndTreeModule.ScreenToClient(&pt);

	if((m_wndTreeModule.HitTest(pt)))
	{
		int nTypeMenu;
		switch(m_wndTreeModule.dataCurrentItem.lParam)
		{
			case TTE_ROOT:
				nTypeMenu = IDR_MENU_POPUP_MODULES_ROOT;
				break;
			case TTE_FOLDER:
				nTypeMenu = IDR_MENU_POPUP_MODULES_FOLDER;
				break;
			case TTE_ELEMENT:
				nTypeMenu = IDR_MENU_POPUP_MODULES_MODULE;
				break;
			default:
				nTypeMenu = IDR_MENU_POPUP_MODULES_ELEMENT;
				break;
		}
		theApp.GetContextMenuManager()->ShowPopupMenu(nTypeMenu, pt1.x, pt1.y, this, TRUE);
	}
}

void CPanelModuleView::OnRename()
{
	m_wndTreeModule.StartEdit(m_wndTreeModule.dataCurrentItem.hItem);
}

void CPanelModuleView::OnRemove()
{
	DeleteItem();
}

void CPanelModuleView::OnOpen()
{
	OpenItem(m_wndTreeModule.dataCurrentItem.hItem, m_wndTreeModule.dataCurrentItem.lParam);
}

void CPanelModuleView::OnUpdateAddFolder(CCmdUI* pCmdUI)
{
	DWORD_PTR lParam(m_wndTreeModule.dataCurrentItem.lParam);
	pCmdUI->Enable(theApp.frm->GetCurrentProject() && (lParam == TTE_FOLDER || lParam == TTE_ROOT));
}

void CPanelModuleView::OnGotoLocation()
{

}

LRESULT CPanelModuleView::OnTreeViewKeyDelete(WPARAM hItem, LPARAM dataItem)
{
	return (DeleteItem() == true);
}

LRESULT CPanelModuleView::OnTreeViewDoubleClick(WPARAM hItem, LPARAM dataItem)
{
	OpenItem((HTREEITEM)hItem, (DWORD_PTR)dataItem);
	return 0;
}

LRESULT CPanelModuleView::OnTreeViewSelectItem(WPARAM hItem, LPARAM dataItem)
{
	CProject* prj(theApp.frm->GetCurrentProject());
	// получить корневой элемент
	HTREEITEM hRoot(m_wndTreeModule.GetItemRoot((HTREEITEM)hItem));
	// установить как текущий
	theApp.frm->SetCurrentProject(m_wndTreeModule.GetItemText(hRoot));
	// получить элемент
	switch(dataItem)
	{
		case TTE_ELEMENT:
			elem = prj->GetElement(NULL, NULL, m_wndTreeModule.pathCurrentItem);
			break;
		case TTE_MODULES_PROC:
		case TTE_MODULES_STRUCT:
		case TTE_MODULES_EQU:
			elem = prj->GetElement(NULL, NULL, m_wndTreeModule.GetItemPath(m_wndTreeModule.GetParentItem(m_wndTreeModule.dataCurrentItem.hItem)));
			break;
		default:
			elem = NULL;
	}
	// установить в панели свойств
	SetProperties(false);
	return 0;
}

void CPanelModuleView::OnSetStartup()
{
	CProject* prj(theApp.frm->GetSolution()->GetExecuteProject());
	if(prj)
	{
		m_wndTreeModule.SetItemBoldText(m_wndTreeModule.GetItem(prj->GetName()), FALSE);
		fileView->m_wndTreeFile.SetItemBoldText(fileView->m_wndTreeFile.GetItem(prj->GetName()), FALSE);
	}
	// установить новый
	CString name(m_wndTreeModule.GetItemText(m_wndTreeModule.dataCurrentItem.hItem));
	theApp.frm->GetSolution()->SetExecuteProject(name);
	m_wndTreeModule.SetItemBoldText(m_wndTreeModule.GetItem(name), TRUE);
	fileView->m_wndTreeFile.SetItemBoldText(fileView->m_wndTreeFile.GetItem(name), TRUE);
}

void CPanelModuleView::TreeAddBlock(CElement::BLOCK_ELEMENT* blk, const CString& name, bool is_sel)
{
	int nImage, nData;
	switch(blk->type)
	{
		case CElement::blkProc:
			nImage = 3;
			nData = TTE_MODULES_PROC;
			break;
		case CElement::blkStruct:
			nImage = 4;
			nData = TTE_MODULES_STRUCT;
			break;
		case CElement::blkEqu:
			nImage = 6;
			nData = TTE_MODULES_EQU;
			break;
	}
	m_wndTreeModule.AddItem(name + "\\" + blk->name, 2, nImage, TTE_ELEMENT, nData, is_sel);
}

void CPanelModuleView::TreeAddModule(CElement* elem, const CString& name, bool is_sel)
{
	CElement::BLOCK_ELEMENT* blk;
	size_t index_in(0);
	CString path(name + "\\" + elem->path_modules);
	// непосредственно модули
	m_wndTreeModule.AddItem(path, 0, 2, TTE_FOLDER, TTE_ELEMENT, is_sel);
	// теперь блоки (формируются на этапе парсинга)
	while((blk = elem->EnumBlocks(index_in)))
		TreeAddBlock(blk, path, false);
}

void CPanelModuleView::MakeTree(CProject* prj)
{
	HTREEITEM hItem;
	CString name(prj->GetName());
	size_t index(0);
	CElement* elem;
	// 1. создать корень проекта
	hItem = m_wndTreeModule.AddItem(name, 5, 5, TTE_ROOT, TTE_ROOT, false);
	// 2. выделить(если надо)
	if(theApp.frm->GetSolution()->GetExecuteProject() == prj)
		m_wndTreeModule.SetItemBoldText(hItem, TRUE);
	// 3. перечислить элементы и создать их (в данном случае модули)
	while((elem = prj->EnumElements(index)))
		TreeAddModule(elem, name, false);
}

void CPanelModuleView::Reset()
{
	m_wndTreeModule.Reset();
}

void CPanelModuleView::OpenItem(HTREEITEM hItem, DWORD_PTR lParamItem)
{
	switch(lParamItem)
	{
		case TTE_ELEMENT:
			if(!theApp.frm->OpenDocument(elem->path, elem, true))
				MessageBox("Ошибка. Не удалось открыть модуль!", ASM_STUDIO, MB_ICONERROR);
			break;
		case TTE_MODULES_PROC:
		case TTE_MODULES_EQU:
		case TTE_MODULES_STRUCT:
			if(theApp.frm->OpenDocument(elem->path, elem, true))
			{
				CElement::BLOCK_ELEMENT* blk;
				// перейти на блок
				if((blk = elem->GetBlock(m_wndTreeModule.GetItemText(hItem), m_cnv_modules[lParamItem])))
				{
					// перейти на позицию
//					blk->line_begin;
					return;
				}
			}
			break;
	}
	SetProperties(false);
}

LRESULT CPanelModuleView::OnTreeViewEndDrag(WPARAM hDrop, LPARAM hItem)
{
	// поменять пути
	CString path(m_wndTreeModule.GetItemPath((HTREEITEM)hDrop));
	if((elem = theApp.frm->GetCurrentProject()->GetElement(NULL, NULL, path)))
	{
		elem->path_modules = m_wndTreeModule.GetItemPath((HTREEITEM)hItem);
		SetProperties(false);
	}
	return 0;
}

void CPanelModuleView::OnTreeViewRenameInFolder(HTREEITEM hItem, const char* old_path, const char* new_path)
{
	HTREEITEM hChildItem(m_wndTreeModule.GetNextItem(hItem, TVGN_CHILD));
	while(hChildItem)
	{
		// переименовываем
		if((elem = theApp.frm->GetCurrentProject()->GetElement(NULL, NULL, m_wndTreeModule.GetItemPath(hChildItem))))
		{
			int pos(elem->path_modules.Find(old_path));
			if(pos != -1) pos += strlen(old_path);
			elem->path_modules = new_path + (elem->path_modules.Mid(pos));
		}
		// на уровень ниже
		OnTreeViewRenameInFolder(hChildItem, old_path, new_path);
		// следующий на этом уровне
		hChildItem = m_wndTreeModule.GetNextItem(hChildItem, TVGN_NEXT);
	}
	SetProperties(false);
}

LRESULT CPanelModuleView::OnTreeViewEndEdit(WPARAM hItem, LPARAM dataItem)
{
	CString name;
	m_wndTreeModule.renameEdit->GetWindowText(name);

	switch(dataItem)
	{
		case TTE_MODULES_STRUCT:
		case TTE_MODULES_PROC:
		case TTE_MODULES_EQU:
			// находим родительский элемент(модуль)
			if(elem)
			{
				CString old_name(m_wndTreeModule.GetItemText((HTREEITEM)hItem));
				CElement::BLOCK_ELEMENT* blk;
				size_t index(0);
				while((blk = elem->EnumBlocks(index)))
				{
					if(blk->type != m_cnv_modules[dataItem] && blk->name != old_name)
						continue;
					// нашли - меняем имя
					blk->name = name;
					m_wndTreeModule.SetItemText((HTREEITEM)hItem, name);
					break;
				}
			}
			break;
		case TTE_ELEMENT:
			if(elem)
			{
				elem->path_modules = (m_wndTreeModule.pathCurrentItem.Left(m_wndTreeModule.pathCurrentItem.ReverseFind('\\') + 1) + name);
				m_wndTreeModule.SetItemText((HTREEITEM)hItem, name);
			}
			break;
		case TTE_FOLDER:
			// перебрать все модули в ней и произвести изменение путей
			OnTreeViewRenameInFolder((HTREEITEM)hItem, m_wndTreeModule.pathCurrentItem, m_wndTreeModule.pathCurrentItem.Left(m_wndTreeModule.pathCurrentItem.ReverseFind('\\') + 1) + name);
			m_wndTreeModule.SetItemText((HTREEITEM)hItem, name);
			break;
		case TTE_ROOT:
			// установить новое имя у текущего и поменять его в панели файлов и модулей
			m_wndTreeModule.SetItemText(m_wndTreeModule.dataCurrentItem.hItem, name);
			fileView->m_wndTreeFile.SetItemText(fileView->m_wndTreeFile.GetItem(theApp.frm->GetCurrentProject()->GetName()), name);
			theApp.frm->GetCurrentProject()->SetName(name);
			return 1;
	}
	SetProperties(false);

	return 1;
}

void CPanelModuleView::OnSaveAll()
{
	theApp.frm->OnSaveAll();
}

bool CPanelModuleView::DeleteItem()
{
	if(MessageBox("Вы действительно хотите удалить?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDNO)
		return false;

	CProject* prj(theApp.frm->GetCurrentProject());
	Array<CDocumentCommon*> docs;
	HTREEITEM hItem(m_wndTreeModule.dataCurrentItem.hItem);

	switch(m_wndTreeModule.dataCurrentItem.lParam)
	{
		case TTE_ROOT:
			// закрыть все документы проекта, который стираем
			if(theApp.frm->GetAllDocuments(&docs, NULL, theApp.frm->GetCurrentProject(), false, false))
			{
				for(size_t i = 0 ; i < docs.size() ; i++)
					docs[i]->OnCloseDocument();
			}
			theApp.frm->GetSolution()->DeleteProject(m_wndTreeModule.GetItemText(hItem));
			m_wndTreeModule.DeleteItem(hItem);
			break;
		case TTE_FOLDER:
			m_wndTreeModule.RemoveFolder(hItem, m_wndTreeModule.GetParentItem(hItem), true);
			m_wndTreeModule.DeleteItem(hItem);
			break;
		case TTE_ELEMENT:
			if(elem)
			{
				// закрыть документ, если есть
				if(elem->doc)
				{
					elem->doc->OnCloseDocument();
					if(elem->doc)
						return false;
				}
				// стереть из панелей
				m_wndTreeModule.DeleteItem(hItem);
				fileView->m_wndTreeFile.DeleteItem(fileView->m_wndTreeFile.GetItem(prj->GetName() + "\\" + elem->path_files));
				// стереть из проекта
				prj->DeleteElement(elem->path);
			}
			break;
		case TTE_MODULES_PROC:
		case TTE_MODULES_STRUCT:
		case TTE_MODULES_EQU:
			if(elem)
			{
				CString name(m_wndTreeModule.GetItemText(hItem));
				CElement::BLOCK_ELEMENT* blk;
				size_t index(0);
				while((blk = elem->EnumBlocks(index)))
				{
					if(blk->type != m_cnv_modules[m_wndTreeModule.dataCurrentItem.lParam] && blk->name != name)
						continue;
					// нашли - удаляем
					m_wndTreeModule.DeleteItem(hItem);
					// удаляем из элемента проекта и текста
					elem->SetModify();
					break;
				}
			}
			break;
	}
	SetProperties(false);

	return true;
}

void CPanelModuleView::OnRebuild()
{
	theApp.frm->OnRebuild();
}

void CPanelModuleView::OnBuildProject()
{
	theApp.frm->OnBuildProject();
}

void CPanelModuleView::OnBuildSolution()
{
	theApp.frm->OnBuildSolution();
}
