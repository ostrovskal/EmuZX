
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
	// конструктор
	CViewTree() : hDropRoot(NULL), hDropItem(NULL), renameEdit(NULL), hNoDragCursor(AfxGetApp()->LoadCursor(IDC_NODROP)), hDragCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DRAG)) {}
	// деструктор
	virtual ~CViewTree() {::DeleteObject(hDragCursor) ; stk_tree.deleteAll();}
	// предобработка события
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// события
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
	// сброс
	void Reset();
	// запуск редактирования элемента
	void StartEdit(HTREEITEM hItem);
	// установка или сброс полужирного начертания элемента
	BOOL SetItemBoldText(HTREEITEM hItem, BOOL is);
	// возврат пути элемента
	CString GetItemPath(HTREEITEM hItem);
	// вернуть элемент по его пути
	HTREEITEM GetItem(const char* path);
	// добавление нового элемента
	HTREEITEM AddItem(const char* path, int nFolderImage, int nImage, DWORD_PTR nFolderData, DWORD_PTR nData, bool is_expand);
	// удаление элемента
	HTREEITEM DeleteItem(HTREEITEM hItem);
	// получение корневого элемента из элемента
	HTREEITEM GetItemRoot(HTREEITEM hItem);
	// вернуть структуру
	TREE_DATA* GetItemStk(HTREEITEM hItem) {return (TREE_DATA*)CTreeCtrl::GetItemData(hItem);}
	// рекурсия при удалении папки
	void RemoveFolder(HTREEITEM hDrop, HTREEITEM hItem, bool is_begin);
	// хэндл элемента, который перетаскиваем
	HTREEITEM hDropItem;
	// путь текущего выделенного элемента
	CString pathCurrentItem;
	// данные текущего элемента
	TREE_DATA dataCurrentItem;
	// редактор при переименовании
	CEdit* renameEdit;
protected:
	// рекурсия при перетаскивании
	void FinishDrop(HTREEITEM hDrop, HTREEITEM hItem);
	// проверка на вложеннность элемента
	bool IsItem(HTREEITEM hParent, HTREEITEM hItem);
	// обноеление базовых данных
	void UpdateData();
	// завершение переименования
	BOOL FinishEdit(bool isOK);
	// хэндл курсора при перетаскивании
	bool is_drop_enable;
	HTREEITEM hDropRoot;
	HCURSOR hDragCursor;
	HCURSOR hNoDragCursor;
	// массив структур данных дерева
	Array<TREE_DATA*> stk_tree;
	// уведомление
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	// карта событий
	DECLARE_MESSAGE_MAP()
};
