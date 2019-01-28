
#pragma once

#include "ViewTree.h"
#include "Project.h"

class CPanelModuleView;
class CPanelFileView : public CDockablePane
{
	friend class CPanelModuleView;
public:
	class CToolBarFile : public CMFCToolBar
	{
		virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList() const { return FALSE; }
	};
	CPanelFileView() : moduleView(NULL), elem(NULL) {}
	virtual ~CPanelFileView() {}
	void AdjustLayout();
	afx_msg void OnSetStartup();
	afx_msg void OnProperties();
	afx_msg void OnRemove();
	afx_msg void OnRename();
	afx_msg void OnSaveAll();
	afx_msg void OnRebuild();
	afx_msg void OnBuildProject();
	afx_msg void OnBuildSolution();
	afx_msg void OnOpen();
	afx_msg void OnExcludeDocument();
	afx_msg void OnDelete();
	afx_msg void OnAddNewDocument();
	afx_msg void OnAddDocument();
	afx_msg void OnUpdateFilesRemove(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilesPanel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFilesPanelFiles(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAddDocument(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilesRename(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExcludeDocument(CCmdUI *pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnTreeViewKeyDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewDoubleClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewSelectItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewEndEdit(WPARAM wParam, LPARAM lParam);
	// создание дерева
	void MakeTree(CProject* prj);
	// сброс
	void Reset();
	// добавление файла
	void TreeAddFile(CElement* elem, const CString& name, bool is_sel);
	// удаление элемента
	bool DeleteItem();
protected:
	CElement* elem;
	CPanelModuleView* moduleView;
	CViewTree m_wndTreeFile;
	CImageList m_imgTreeFile;
	CToolBarFile m_wndToolBar;
	void SetProperties(bool is_show);
	void OpenFile(HTREEITEM hItem);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
};

