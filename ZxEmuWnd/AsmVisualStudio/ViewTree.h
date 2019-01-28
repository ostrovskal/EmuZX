
#pragma once

#include "Resource.h"

const UINT WM_TREE_KEY_DELETE = RegisterWindowMessage(_T("TREE_KEY_DELETE"));
const UINT WM_TREE_DBLCLK = RegisterWindowMessage(_T("TREE_DBLCLK"));
const UINT WM_TREE_SELECT = RegisterWindowMessage(_T("TREE_SELECT"));
const UINT WM_TREE_END_DRAG = RegisterWindowMessage(_T("TREE_END_DRAG"));
const UINT WM_TREE_END_EDIT = RegisterWindowMessage(_T("TREE_END_EDIT"));

class CViewTree : public CTreeCtrl
{
public:
	// �����������
	CViewTree() : hDropRoot(NULL), hDropItem(NULL), renameEdit(NULL), hNoDragCursor(AfxGetApp()->LoadCursor(IDC_NODROP)), hDragCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DRAG)) {}
	// ����������
	virtual ~CViewTree() {::DeleteObject(hDragCursor) ; stk_tree.deleteAll();}
	// ������������� �������
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// �������
	afx_msg void OnEnUpdateEditName();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	// �����
	void Reset();
	// ������ �������������� ��������
	void StartEdit(HTREEITEM hItem);
	// ��������� ��� ����� ����������� ���������� ��������
	BOOL SetItemBoldText(HTREEITEM hItem, BOOL is);
	// ������� ���� ��������
	CString GetItemPath(HTREEITEM hItem);
	// ������� ������� �� ��� ����
	HTREEITEM GetItem(const char* path);
	// ���������� ������ ��������
	HTREEITEM AddItem(const char* path, int nFolderImage, int nImage, DWORD_PTR nFolderData, DWORD_PTR nData, bool is_expand);
	// �������� ��������
	HTREEITEM DeleteItem(HTREEITEM hItem);
	// ��������� ��������� �������� �� ��������
	HTREEITEM GetItemRoot(HTREEITEM hItem);
	// ������� ���������
	TREE_DATA* GetItemStk(HTREEITEM hItem) {return (TREE_DATA*)CTreeCtrl::GetItemData(hItem);}
	// �������� ��� �������� �����
	void RemoveFolder(HTREEITEM hDrop, HTREEITEM hItem, bool is_begin);
	// ����� ��������, ������� �������������
	HTREEITEM hDropItem;
	// ���� �������� ����������� ��������
	CString pathCurrentItem;
	// ������ �������� ��������
	TREE_DATA dataCurrentItem;
	// �������� ��� ��������������
	CEdit* renameEdit;
protected:
	// �������� ��� ��������������
	void FinishDrop(HTREEITEM hDrop, HTREEITEM hItem);
	// �������� �� ������������ ��������
	bool IsItem(HTREEITEM hParent, HTREEITEM hItem);
	// ���������� ������� ������
	void UpdateData();
	// ���������� ��������������
	BOOL FinishEdit(bool isOK);
	// ����� ������� ��� ��������������
	bool is_drop_enable;
	HTREEITEM hDropRoot;
	HCURSOR hDragCursor;
	HCURSOR hNoDragCursor;
	// ������ �������� ������ ������
	Array<TREE_DATA*> stk_tree;
	// �����������
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	// ����� �������
	DECLARE_MESSAGE_MAP()
};
