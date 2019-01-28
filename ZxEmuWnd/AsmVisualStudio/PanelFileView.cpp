
#include "stdafx.h"
#include "mainfrm.h"
#include "PanelFileView.h"
#include "Resource.h"
#include "AsmVisualStudio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPanelFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SET_STARTUP, &CPanelFileView::OnSetStartup)
	ON_COMMAND(ID_PROPERTIES, &CPanelFileView::OnProperties)
	ON_COMMAND(ID_ADD_NEW_DOCUMENT, &CPanelFileView::OnAddNewDocument)
	ON_COMMAND(ID_ADD_DOCUMENT, &CPanelFileView::OnAddDocument)
	ON_COMMAND(ID_REMOVE, &CPanelFileView::OnRemove)
	ON_COMMAND(ID_RENAME, &CPanelFileView::OnRename)
	ON_COMMAND(ID_OPEN, &CPanelFileView::OnOpen)
	ON_COMMAND(ID_EXCLUDE_DOCUMENT, &CPanelFileView::OnExcludeDocument)
	ON_COMMAND(ID_DELETE, &CPanelFileView::OnDelete)
	ON_COMMAND(ID_SAVE_ALL, &CPanelFileView::OnSaveAll)
	ON_COMMAND(ID_REBUILD, &CPanelFileView::OnRebuild)
	ON_COMMAND(ID_BUILD_PROJECT, &CPanelFileView::OnBuildProject)
	ON_COMMAND(ID_BUILD_SOLUTION, &CPanelFileView::OnBuildSolution)
	ON_UPDATE_COMMAND_UI(ID_ADD_NEW_DOCUMENT, &CPanelFileView::OnUpdateAddDocument)
	ON_UPDATE_COMMAND_UI(ID_ADD_DOCUMENT, &CPanelFileView::OnUpdateAddDocument)
	ON_UPDATE_COMMAND_UI(ID_RENAME, &CPanelFileView::OnUpdateFilesRename)
	ON_UPDATE_COMMAND_UI(ID_REMOVE, &CPanelFileView::OnUpdateFilesRemove)
	ON_UPDATE_COMMAND_UI(ID_DELETE, &CPanelFileView::OnUpdateFilesPanelFiles)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CPanelFileView::OnUpdateFilesPanel)
	ON_REGISTERED_MESSAGE(WM_TREE_KEY_DELETE, &CPanelFileView::OnTreeViewKeyDelete)
	ON_REGISTERED_MESSAGE(WM_TREE_DBLCLK, &CPanelFileView::OnTreeViewDoubleClick)
	ON_REGISTERED_MESSAGE(WM_TREE_SELECT, &CPanelFileView::OnTreeViewSelectItem)
	ON_REGISTERED_MESSAGE(WM_TREE_END_EDIT, &CPanelFileView::OnTreeViewEndEdit)
	ON_UPDATE_COMMAND_UI(ID_EXCLUDE_DOCUMENT, &CPanelFileView::OnUpdateExcludeDocument)
END_MESSAGE_MAP()

int CPanelFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CBitmap bmp;
	BITMAP bmpObj;
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!m_wndTreeFile.Create(WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_HASLINES | TVS_EDITLABELS | TVS_DISABLEDRAGDROP | TVS_LINESATROOT | TVS_HASBUTTONS, CRect(0, 0, 1, 1), this, 4))
		return -1;
	if (!bmp.LoadBitmap(IDB_BITMAP_TREE_PANEL_FILES))
		return -1;
	moduleView = theApp.frm->GetPanelModules();
	bmp.GetBitmap(&bmpObj);
	m_imgTreeFile.Create(16, bmpObj.bmHeight, ILC_MASK | ILC_COLOR24, 0, 0);
	m_imgTreeFile.Add(&bmp, RGB(255, 0, 255));
	m_wndTreeFile.SetImageList(&m_imgTreeFile, TVSIL_NORMAL);
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TOOLBAR_FILES);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_FILES, 0, 0, TRUE);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	AdjustLayout();
	return 0;
}

void CPanelFileView::OnAddNewDocument()
{
	theApp.frm->OnAddNewDocument();
}

void CPanelFileView::OnAddDocument()
{
	theApp.frm->OnAddDocument();
}

void CPanelFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPanelFileView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
		m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
		m_wndTreeFile.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelFileView::OnPaint()
{
	CPaintDC dc(this);
	CRect rectTree;
	m_wndTreeFile.GetWindowRect(rectTree);
	ScreenToClient(rectTree);
	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPanelFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndTreeFile.SetFocus();
}

void CPanelFileView::OnProperties()
{
	switch(m_wndTreeFile.dataCurrentItem.lParam)
	{
		case TTE_ELEMENT:
			SetProperties(true);
			break;
		case TTE_FOLDER:
		case TTE_ROOT:
			theApp.frm->OnProjectOptions();
			break;
	}
}

void CPanelFileView::OnRemove()
{
	DeleteItem();
}

void CPanelFileView::OnRename()
{
	m_wndTreeFile.StartEdit(m_wndTreeFile.dataCurrentItem.hItem);
}

void CPanelFileView::OnUpdateFilesRename(CCmdUI *pCmdUI)
{
	DWORD_PTR lParam(m_wndTreeFile.dataCurrentItem.lParam);
	pCmdUI->Enable(lParam != TTE_FOLDER && lParam != -1);
}

void CPanelFileView::OnUpdateFilesRemove(CCmdUI *pCmdUI)
{
	DWORD_PTR lParam(m_wndTreeFile.dataCurrentItem.lParam);
	pCmdUI->Enable(lParam == TTE_ELEMENT || lParam == TTE_ROOT);
}

void CPanelFileView::OnUpdateFilesPanelFiles(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_wndTreeFile.dataCurrentItem.lParam == TTE_ELEMENT);
}

void CPanelFileView::OnUpdateFilesPanel(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_wndTreeFile.dataCurrentItem.lParam != -1);
}

void CPanelFileView::OnUpdateAddDocument(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(theApp.frm->GetCurrentProject() != NULL);
}

void CPanelFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt, pt1;

	GetCursorPos(&pt) ; pt1 = pt;
	m_wndTreeFile.ScreenToClient(&pt);

	if((m_wndTreeFile.HitTest(pt)))
	{
		int nTypeMenu;
		switch(m_wndTreeFile.dataCurrentItem.lParam)
		{
			case TTE_ROOT:
				nTypeMenu = IDR_MENU_POPUP_FILES_ROOT;
				break;
			case TTE_FOLDER:
				nTypeMenu = IDR_MENU_POPUP_FILES_FOLDER;
				break;
			default:
				nTypeMenu = IDR_MENU_POPUP_FILES_FILE;
				break;
		}
		theApp.GetContextMenuManager()->ShowPopupMenu(nTypeMenu, pt1.x, pt1.y, this, TRUE);
	}
}

LRESULT CPanelFileView::OnTreeViewKeyDelete(WPARAM hItem, LPARAM dataItem)
{
	return (DeleteItem() == true);
}

LRESULT CPanelFileView::OnTreeViewDoubleClick(WPARAM hItem, LPARAM dataItem)
{
	if(dataItem == TTE_ELEMENT)
		OpenFile((HTREEITEM)hItem);
	return 0;
}

void CPanelFileView::SetProperties(bool is_show)
{
	theApp.frm->GetPanelProperties()->UpdateAll(m_wndTreeFile.dataCurrentItem.lParam == TTE_ELEMENT ? CPanelPropertiesView::IT_FILE : CPanelPropertiesView::IT_DEFAULT, elem, NULL, is_show);
}

LRESULT CPanelFileView::OnTreeViewSelectItem(WPARAM hItem, LPARAM dataItem)
{
	// получить текущий элемент
	elem = theApp.frm->GetCurrentProject()->GetElement(NULL, m_wndTreeFile.pathCurrentItem, NULL);
	// установить текущий проект
	theApp.frm->SetCurrentProject(m_wndTreeFile.GetItemText(m_wndTreeFile.GetItemRoot((HTREEITEM)hItem)));
	// установить в свойствах
	SetProperties(false);
	return 0;
}

void CPanelFileView::OnSetStartup()
{
	CProject* prj(theApp.frm->GetSolution()->GetExecuteProject());
	if(prj)
	{
		m_wndTreeFile.SetItemBoldText(m_wndTreeFile.GetItem(prj->GetName()), FALSE);
		moduleView->m_wndTreeModule.SetItemBoldText(moduleView->m_wndTreeModule.GetItem(prj->GetName()), FALSE);
	}
	// установить новый
	CString name(m_wndTreeFile.GetItemText(m_wndTreeFile.dataCurrentItem.hItem));
	theApp.frm->GetSolution()->SetExecuteProject(name);
	m_wndTreeFile.SetItemBoldText(m_wndTreeFile.GetItem(name), TRUE);
	moduleView->m_wndTreeModule.SetItemBoldText(moduleView->m_wndTreeModule.GetItem(name), TRUE);
}

void CPanelFileView::Reset()
{
	m_wndTreeFile.Reset();
}

void CPanelFileView::TreeAddFile(CElement* elem, const CString& name, bool is_sel)
{
	int nImage;
	// по типу файла определить его изображение и тип папки
	CString path(name + "\\" + elem->path_files);
	CString ext(path.Mid(path.ReverseFind('.') + 1));
	ext.MakeLower();
	nImage = 3;
	if(ext == "asm") nImage = 2;
	else if(ext == "inc") nImage = 4;
	m_wndTreeFile.AddItem(path, 0, nImage, TTE_FOLDER, TTE_ELEMENT, is_sel);
}

void CPanelFileView::MakeTree(CProject* prj)
{
	HTREEITEM hItem;
	CString name(prj->GetName());
	size_t index(0);
	CElement* elem;
	// 1. создать корень проекта
	hItem = m_wndTreeFile.AddItem(name, 5, 5, TTE_ROOT, TTE_ROOT, true);
	// 3. выделить(если надо)
	if(theApp.frm->GetSolution()->GetExecuteProject() == prj)
		m_wndTreeFile.SetItemBoldText(hItem, TRUE);
	// 4. создать вложенные папки (по умолчанию)
	m_wndTreeFile.AddItem(name + "\\Исходные файлы", 5, 0, TTE_ROOT, TTE_FOLDER, false);
	m_wndTreeFile.AddItem(name + "\\Подключаемые файлы", 5, 0, TTE_ROOT, TTE_FOLDER, false);
	m_wndTreeFile.AddItem(name + "\\Двоичные файлы", 5, 0, TTE_ROOT, TTE_FOLDER, false);
	m_wndTreeFile.AddItem(name + "\\Ресурсные файлы", 5, 0, TTE_ROOT, TTE_FOLDER, false);
	// 5. перечислить элементы и создать их
	while((elem = prj->EnumElements(index)))
		TreeAddFile(elem, name, false);
}

void CPanelFileView::OnOpen()
{
	OpenFile(m_wndTreeFile.dataCurrentItem.hItem);
}

void CPanelFileView::OpenFile(HTREEITEM hItem)
{
	if(elem)
	{
		if(!theApp.frm->OpenDocument(elem->path, elem, true))
			MessageBox("Ошибка. Не удалось открыть документ!", ASM_STUDIO, MB_ICONERROR);
	}
	SetProperties(false);
}

LRESULT CPanelFileView::OnTreeViewEndEdit(WPARAM hItem, LPARAM dataItem)
{
	CString name;
	m_wndTreeFile.renameEdit->GetWindowText(name);

	switch(dataItem)
	{
		case TTE_ELEMENT:
			if(elem)
			{
				CString path(elem->path);
				elem->path_files = (m_wndTreeFile.pathCurrentItem.Left(m_wndTreeFile.pathCurrentItem.ReverseFind('\\') + 1) + name);
				m_wndTreeFile.SetItemText((HTREEITEM)hItem, name);
				if(elem->doc)
					elem->doc->SetTitle(elem->path_files);
				// переименовать на диске
				CString new_path(path.Left(path.ReverseFind('\\') + 1) + name);
				::MoveFile(path, new_path);
				elem->path = new_path;
			}
			SetProperties(false);
			return 1;
		case TTE_ROOT:
			// установить новое имя у текущего и поменять его в панели файлов и модулей
			m_wndTreeFile.SetItemText((HTREEITEM)hItem, name);
			moduleView->m_wndTreeModule.SetItemText(moduleView->m_wndTreeModule.GetItem(theApp.frm->GetCurrentProject()->GetName()), name);
			theApp.frm->GetCurrentProject()->SetName(name);
			return 1;
	}
	return 1;
}

void CPanelFileView::OnExcludeDocument()
{
	if(elem) elem->SetExclude(!elem->IsExclude());
	SetProperties(false);
}

void CPanelFileView::OnUpdateExcludeDocument(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(elem && elem->IsExclude());
}

void CPanelFileView::OnDelete()
{
	CString path(elem->path);
	if(DeleteItem())
	{
		// стереть файл
		::DeleteFile(path);
	}
}

bool CPanelFileView::DeleteItem()
{
	if(MessageBox("Вы действительно хотите удалить?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDNO)
		return false;

	Array<CDocumentCommon*> docs;

	switch(m_wndTreeFile.dataCurrentItem.lParam)
	{
		case TTE_ROOT:
			// закрыть все документы проекта, который стираем
			if(theApp.frm->GetAllDocuments(&docs, NULL, theApp.frm->GetCurrentProject(), false, false))
			{
				for(size_t i = 0 ; i < docs.size() ; i++)
					docs[i]->OnCloseDocument();
			}
			theApp.frm->GetSolution()->DeleteProject(m_wndTreeFile.GetItemText(m_wndTreeFile.dataCurrentItem.hItem));
			m_wndTreeFile.DeleteItem(m_wndTreeFile.dataCurrentItem.hItem);
			break;
		case TTE_ELEMENT:
			if(elem)
			{
				if(elem->doc)
				{
					elem->doc->OnCloseDocument();
					if(elem->doc)
						return false;
				}
				m_wndTreeFile.DeleteItem(m_wndTreeFile.dataCurrentItem.hItem);
				moduleView->m_wndTreeModule.DeleteItem(moduleView->m_wndTreeModule.GetItem(elem->prj->GetName() + "\\" + elem->path_modules));
				elem->prj->DeleteElement(elem->path);
				elem = NULL;
			}
			break;
	}
	SetProperties(false);

	return true;
}

void CPanelFileView::OnSaveAll()
{
	theApp.frm->OnSaveAll();
}

void CPanelFileView::OnRebuild()
{
	theApp.frm->OnRebuild();
}

void CPanelFileView::OnBuildProject()
{
	theApp.frm->OnBuildProject();
}

void CPanelFileView::OnBuildSolution()
{
	theApp.frm->OnBuildSolution();
}
