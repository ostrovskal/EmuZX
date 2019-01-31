
#pragma once

#include "ViewTree.h"
#include "Project.h"

class CPanelFileView;
class CPanelModuleView : public CDockablePane
{
	friend class CPanelFileView;
public:
	class CToolBarModule : public CMFCToolBar
	{
		virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList() const {return FALSE;}
	};
	CPanelModuleView() : fileView(NULL), elem(NULL) {}
	virtual ~CPanelModuleView() {}
	void AdjustLayout();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnSetStartup();
	afx_msg void OnGotoLocation();
	afx_msg void OnRename();
	afx_msg void OnRemove();
	afx_msg void OnOpen();
	afx_msg void OnProperties();
	afx_msg void OnAddFolder();
	afx_msg void OnAddEqu();
	afx_msg void OnAddStructure();
	afx_msg void OnAddProcedure();
	afx_msg void OnSaveAll();
	afx_msg void OnRebuild();
	afx_msg void OnBuildProject();
	afx_msg void OnBuildSolution();
	afx_msg void OnUpdateModulesAddBlock(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModulesPanel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAddFolder(CCmdUI* pCmdUI);
	afx_msg LRESULT OnTreeViewKeyDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewDoubleClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewSelectItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewEndDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewEndEdit(WPARAM wParam, LPARAM lParam);
	// создание дерева
	void MakeTree(CProject* prj);
	// сброс
	void Reset();
	// добавление модуля
	void TreeAddModule(CElement* elem, const CString& name, bool is_sel);
	// добавление блока
	void TreeAddBlock(CElement::BLOCK_ELEMENT* blk, const CString& name, bool is_sel);
	// удаление элемента
	bool DeleteItem();
protected:
	CElement* elem;
	CPanelFileView* fileView;
	CToolBarModule m_wndToolBar;
	CViewTree m_wndTreeModule;
	CImageList m_imgTreeModule;
	void SetProperties(bool is);
	void OnTreeViewRenameInFolder(HTREEITEM hItem, const char* old_path, const char* new_path);
	void OpenItem(HTREEITEM hItem, DWORD_PTR lParamItem);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
};
