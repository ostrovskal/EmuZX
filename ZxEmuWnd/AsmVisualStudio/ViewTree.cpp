
#include "stdafx.h"
#include "ViewTree.h"
#include "AsmVisualStudio.h"
#include "PanelArchiveView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CViewTree::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CViewTree::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CLICK, &CViewTree::OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnNMDblclk)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, &CViewTree::OnTvnKeydown)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CViewTree::OnTvnBegindrag)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CViewTree::OnTvnEndlabeledit)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

int __stdcall ProcedureSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CViewTree* view((CViewTree*)lParamSort);
	TREE_DATA* td1((TREE_DATA*)lParam1);
	TREE_DATA* td2((TREE_DATA*)lParam2);

	if(td1->lParam == TTE_FOLDER || td2->lParam == TTE_FOLDER)
	{
		if(td1->lParam != TTE_FOLDER)
			return 1;
		if(td2->lParam != TTE_FOLDER)
			return -1;
	}

	return strcmp(view->GetItemText(td1->hItem), view->GetItemText(td2->hItem));
}

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);
	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);
	if(pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}
	return bRes;
}

void CViewTree::Reset()
{
	ShowWindow(SW_HIDE);
	DeleteAllItems();
	ShowWindow(SW_NORMAL);
	// �������� ������� ������
	hDropItem = NULL;
	hDropRoot = NULL;
	is_drop_enable = false;
	pathCurrentItem = "";
	dataCurrentItem.lParam = -1;
}

BOOL CViewTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if(hDropItem)
	{
		::SetCursor(is_drop_enable ? hDragCursor : hNoDragCursor);
		return 1;
	}
	return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void CViewTree::RemoveFolder(HTREEITEM hDrop, HTREEITEM hItem, bool is_begin)
{
	int nImage, i_name(1);
	if(!is_begin)
	{
		// ���������� ����������� ���������������� ��������
		GetItemImage(hDrop, nImage, nImage);
		// ������������ ����� ����
		CString basePath(GetItemPath(hItem)), path, name(GetItemText(hDrop));
		if(basePath != "") basePath += "\\";
		basePath = GetItemText(hDropRoot) + "\\" +  basePath;
		path =  basePath + name;
		// ��������� ����� ��� ����?
		while(GetItem(path))
		{
			path.Format("%s%s(%i)", basePath, name, i_name);
			i_name++;
		}
		// �������� �������
		hItem = AddItem(path, 0, nImage, TTE_FOLDER, GetItemStk(hDrop)->lParam, is_begin);
		// ������� ��������� ��������
		GetParent()->SendMessage(WM_TREE_END_DRAG, (WPARAM)hDrop, (LPARAM)hItem);
	}
	else
	{
		hDropRoot = GetItemRoot(dataCurrentItem.hItem);
	}
	// ��������� �� ��������
	HTREEITEM hChildItem(GetNextItem(hDrop, TVGN_CHILD));
	while(hChildItem)
	{
		RemoveFolder(hChildItem, hItem, false);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CViewTree::FinishDrop(HTREEITEM hDrop, HTREEITEM hItem)
{
	int nImage;
	TREE_DATA* td;
	// ���������� ����������� ���������������� ��������
	GetItemImage(hDrop, nImage, nImage);
	// �������� �������
	CString path(GetItemPath(hItem));
	if(path != "") path += "\\";
	path = GetItemText(hDropRoot) + "\\" +  path + GetItemText(hDrop);
	hItem = AddItem(path, 0, nImage, TTE_FOLDER, GetItemStk(hDrop)->lParam, true);
	// ������� ��������� ��������
	GetParent()->SendMessage(WM_TREE_END_DRAG, (WPARAM)hDrop, (LPARAM)hItem);
	// ������� ���������(���� �� ��������)
	if(hDrop != hDropItem && (td = GetItemStk(hDrop)))
		stk_tree.del(stk_tree.find(td));
	// ��������� �� ��������
	HTREEITEM hChildItem(GetNextItem(hDrop, TVGN_CHILD));
	while(hChildItem)
	{
		FinishDrop(hChildItem, hItem);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CViewTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnLButtonUp(nFlags, point);
	HTREEITEM hItem;

	if(hDropItem)
	{
		// �������� ��������� �����
		SelectDropTarget(NULL);
		// �������������� ���������?
		if(is_drop_enable)
		{
			UINT flags;
			if((hItem = HitTest(point, &flags)))
			{
				// ��������� ��������, �������� ��� ��� ����� ���� �����
				// ��������� �� ��������
				CString path(GetItemPath(hItem));
				if(path != "") path += "\\";
				path = GetItemText(hDropRoot) + "\\" + path + GetItemText(hDropItem);
				if(!GetItem(path))
				{
					// ������� ����������� ������� ������ ���� ��������� ���������
					FinishDrop(hDropItem, hItem);
					// ������ ���� �������
					DeleteItem(hDropItem);
				}
			}
		}
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW)));
		hDropItem = NULL;
		hDropRoot = NULL;
	}
}

CString CViewTree::GetItemPath(HTREEITEM hItem)
{
	// �������� �� �������� �� �����
	CString path;
	HTREEITEM hParent(NULL);
	while(hParent = GetParentItem(hItem))
	{
		path = GetItemText(hItem) + "\\" + path;
		hItem = hParent;
	}
	return path.Left(path.ReverseFind('\\'));
}

HTREEITEM CViewTree::AddItem(const char* path, int nFolderImage, int nImage, DWORD_PTR nFolderData, DWORD_PTR nData, bool is_sel)
{
	Array<String> arr;
	TREE_DATA* td;
	// 1. ������� ���� �� �����
	Utils::explode("\\", path, arr);
	// 2. �������� �������� �������
	HTREEITEM hItem(NULL), hParent(NULL);
	// 3. �������� �� ���� ������ ����
	for(size_t posItem = 0 ; posItem < arr.size() ; posItem++)
	{
		BOOL is(FALSE);
		CString strItem(arr[posItem]);
		HTREEITEM hChildItem(hItem);
		// 4. ���� ����� �������� ������� � �������
		hChildItem = GetNextItem(hItem, TVGN_CHILD);
		while(hChildItem)
		{
			if((is = (GetItemText(hChildItem) == strItem)))
				break;
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
		// 4. ���� ���������� �������� ��� - �� �������� ���
		if(!is)
		{
			if(!hParent)
				hParent = hItem;
			td = new TREE_DATA;
			td->lParam = (((posItem == arr.size() - 1) ? nData : nFolderData));
			int nImg(((posItem == arr.size() - 1) ? nImage : nFolderImage));
			hChildItem = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM, strItem, nImg, nImg, 0, 0, (LPARAM)td, hItem, TVI_LAST);
			td->hItem = hChildItem;
			stk_tree.add(td);
			// 5. ������ � ������������� ������� ������� ��������
			TVITEM item;
			item.mask = TVIF_CHILDREN;
			item.hItem = hItem;
			item.cChildren = 1;
			SetItem(&item);
		}
		if(is_sel)
		{
			// ���������� � ��������
			Expand(hItem, TVE_EXPAND);
		}
		hItem = hChildItem;
	}
	if(is_sel)
	{
		SelectItem(hItem);
		UpdateData();
	}

	static TVSORTCB m_s;

	m_s.hParent = hParent;
	m_s.lParam = (LPARAM)this;
	m_s.lpfnCompare = ProcedureSort;
	SortChildrenCB(&m_s);

	return hItem;
}

HTREEITEM CViewTree::DeleteItem(HTREEITEM hItem)
{
	HTREEITEM hSelect(NULL);
	TREE_DATA* td;
	// 1. �������� ������������ �������
	HTREEITEM hParent(GetParentItem(hItem));
	// 2. ����� ������� ��� ���������
	if(!(hSelect = GetNextItem(hItem, TVGN_NEXT)))
		hSelect = GetNextItem(hItem, TVGN_PREVIOUS);
	// 3. ������� ���������
	// ������� ���������
	if((td = GetItemStk(hItem)))
		stk_tree.del(stk_tree.find(td));
	CTreeCtrl::DeleteItem(hItem);
	// 4. ��������� � ������������� ��� ���� ��������?
	if(!GetChildItem(hParent))
	{
		TVITEM item;
		int nImage(-1);
		// 5. ��� - ����� ������� ������������ � �������� ��� ��� ������
		item.mask = TVIF_CHILDREN;
		item.cChildren = 0;
		item.hItem = hParent;
		SetItem(&item);
		// ���� ������ - ��� �������� ����� - �� ���������� �������� �����
		if(GetItemImage(hParent, nImage, nImage) && nImage == 1)
			SetItemImage(hParent, 0, 0);
		hSelect = hParent;
	}
	// ������� �����
	SelectItem(hSelect);
	UpdateData();
	// 6. ������� ����� ����������
	return hSelect;
}

void CViewTree::StartEdit(HTREEITEM hItem)
{
	renameEdit = EditLabel(hItem);
}

HTREEITEM CViewTree::GetItem(const char* path)
{
	Array<String> arr;
	// 1. ������� ���� �� �����
	Utils::explode("\\", path, arr);
	// 2. �������� �������� �������
	HTREEITEM hItem(NULL);
	// 3. ���������� �������� �� ���� ������ ���� �� ��������� ��������
	for(size_t posItem = 0 ; posItem < arr.size() ; posItem++)
	{
		BOOL is(FALSE);
		CString strItem(arr[posItem]);
		// 4. ���� ����� �������� ������� � �������
		HTREEITEM hChildItem(GetNextItem(hItem, TVGN_CHILD));
		while(hChildItem)
		{
			if((is = GetItemText(hChildItem) == strItem))
				break;
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
		}
		if(!is)
		{
			// �� �����
			return NULL;
		}
		// ���� ���������
		hItem = hChildItem;
	}
	return hItem;
}

void CViewTree::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	int nImage(pNMTreeView->itemNew.iImage);
	HTREEITEM hItem(pNMTreeView->itemNew.hItem);
	switch(pNMTreeView->action)
	{
		case TVE_EXPAND:
			if(nImage == 0)
				SetItemImage(hItem, 1, 1);
			break;
		case TVE_COLLAPSE: 
			if(nImage == 1)
				SetItemImage(hItem, 0, 0);
			break;
	}
	*pResult = 0;
}

void CViewTree::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem;
	CPoint pt;

	GetCursorPos(&pt);
	ScreenToClient(&pt);

	if((hItem = HitTest(pt)))
	{
		SelectItem(hItem);
		UpdateData();
		GetParent()->SendMessage(WM_TREE_SELECT, (WPARAM)hItem, (LPARAM)dataCurrentItem.lParam);
	}
	*pResult = 0;
}

void CViewTree::UpdateData()
{
	HTREEITEM hItem;
	if((hItem = GetSelectedItem()))
	{
		TREE_DATA* td(GetItemStk(hItem));
		dataCurrentItem.lParam = td->lParam;
		dataCurrentItem.hItem = td->hItem;
		pathCurrentItem = GetItemPath(dataCurrentItem.hItem);
	}
	else
	{
		dataCurrentItem.lParam = -1;
		dataCurrentItem.hItem = NULL;
		pathCurrentItem = "";
	}
}

BOOL CViewTree::SetItemBoldText(HTREEITEM hItem, BOOL is)
{
	return SetItemState(hItem, is ? TVIS_BOLD : 0, TVIS_BOLD);
}

void CViewTree::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	GetParent()->SendMessage(WM_TREE_DBLCLK, (WPARAM)dataCurrentItem.hItem, (LPARAM)dataCurrentItem.lParam);
	*pResult = 0;
}

void CViewTree::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	if(pTVKeyDown->wVKey == VK_DELETE)
		GetParent()->SendMessage(WM_TREE_KEY_DELETE, (WPARAM)dataCurrentItem.hItem, (LPARAM)dataCurrentItem.lParam);
	else if(pTVKeyDown->wVKey == VK_RETURN)
		GetParent()->SendMessage(WM_TREE_DBLCLK, (WPARAM)dataCurrentItem.hItem, (LPARAM)dataCurrentItem.lParam);
	*pResult = 0;
}

HTREEITEM CViewTree::GetItemRoot(HTREEITEM hItem)
{
	// �������� �� �������� �� �����
	HTREEITEM hRoot(hItem);
	while((hItem = GetParentItem(hItem)))
		hRoot = hItem;
	return hRoot;
}

void CViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	hDropItem = pNMTreeView->itemNew.hItem;
	// ��������� �� ����������� ��������������(����� ��� ��������)
	DWORD_PTR nData(GetItemStk(hDropItem)->lParam);
	hDropRoot = ((nData == TTE_ELEMENT || nData == TTE_FOLDER) ? GetItemRoot(hDropItem) : NULL);
	SelectItem(hDropItem);
	UpdateData();
	is_drop_enable = false;
	*pResult = 0;
}

void CViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	FinishEdit(true);
	*pResult = 0;
}

BOOL CViewTree::FinishEdit(bool isOK)
{
	if(renameEdit)
	{
		if(isOK)
		{
			// 1. ��������� ������ ���� � ����� - ����������?
			CString name;
			renameEdit->GetWindowText(name);
			CString path(pathCurrentItem.Left(pathCurrentItem.ReverseFind('\\') + 1) + name), npath;
			if(isOK = (path != pathCurrentItem))
			{
				npath = path;
				if(GetItemRoot(dataCurrentItem.hItem) != dataCurrentItem.hItem)
					path = theApp.frm->GetCurrentProject()->GetName() + "\\" + path;
				// ��������� ����� ������� ��� ���� (��� ��������� ����������)
				if(GetItem(path))
				{
					MessageBox("������� � ����� ������ ��� ����!", ASM_STUDIO, MB_ICONWARNING);
					SetFocus();
				}
				else
				{
					// ����� ������� ��������
					isOK = !GetParent()->SendMessage(WM_TREE_END_EDIT, (WPARAM)dataCurrentItem.hItem, (LPARAM)dataCurrentItem.lParam);
					pathCurrentItem = npath;
				}
			}
		}
		if(!isOK)
		{
			renameEdit = NULL;
			EndEditLabelNow(TRUE);
		}
	}
	return TRUE;
}

BOOL CViewTree::PreTranslateMessage(MSG* pMsg)
{
	if(renameEdit && pMsg->hwnd == renameEdit->m_hWnd && (pMsg->message == WM_KEYDOWN || pMsg->message == WM_CHAR))
	{
		if(pMsg->wParam == VK_RETURN)
		{
			return FinishEdit(true);
		}
		else if(pMsg->wParam == VK_ESCAPE)
			return FinishEdit(false);
	}
	return CTreeCtrl::PreTranslateMessage(pMsg);
}

bool CViewTree::IsItem(HTREEITEM hParent, HTREEITEM hItem)
{
	if(hParent == hItem)
		return true;
	HTREEITEM hChildItem(GetNextItem(hParent, TVGN_CHILD));
	while(hChildItem)
	{
		if(IsItem(hChildItem, hItem))
			return true;
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
	return false;
}

void CViewTree::OnMouseMove(UINT nFlags, CPoint pt)
{
	if(hDropItem)
	{
		HTREEITEM hItem;
		UINT flags;
		if((hItem = HitTest(pt, &flags)) && flags && TVHT_ONITEM)
		{
			// ����� ������������� ������ � ����� � ������ �������� �������
			DWORD_PTR nData(GetItemStk(hItem)->lParam);
			if((is_drop_enable = ((nData == TTE_ROOT || nData == TTE_FOLDER) && (GetItemRoot(hItem) == hDropRoot)) && (hItem != GetParentItem(hDropItem))))
			{
				// ��������� �� ����������� � ����
				// ���� hItem ������ � ���� ������� � hDropItem �� �����������
				is_drop_enable = !IsItem(hDropItem, hItem);
			}
			SelectDropTarget(hItem);
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, pt);
}
