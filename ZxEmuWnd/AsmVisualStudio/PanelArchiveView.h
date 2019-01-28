
#pragma once

#include "ViewTree.h"

class CPanelArchiveView : public CDockablePane
{
public:
	struct ACTIVEX
	{
		ACTIVEX() {}
		ACTIVEX(const char* nm, const char* prog, const char* ex, const char* dsc, const CLSID& cls, HICON h) : name(nm), progID(prog), ext(ex), desc(dsc), clsid(cls), hIcon(h) {}
		String name;
		String ext;
		String desc;
		String progID;
		CLSID clsid;
		HICON hIcon;
	};
	class CToolBarArchive : public CMFCToolBar
	{
		virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList() const {return FALSE;}
	};
	CPanelArchiveView() : arch(NULL) {}
	virtual ~CPanelArchiveView() {activex.deleteAll();}
	afx_msg void OnGotoLocation();
	afx_msg void OnProperties();
	afx_msg void OnAddFolder();
	afx_msg void OnAddResource();
	afx_msg void OnAddNewResource();
	afx_msg void OnRemove();
	afx_msg void OnRename();
	afx_msg void OnArchiveOptimized();
	afx_msg void OnCreateListResources();
	afx_msg void OnRestoreResources();
	afx_msg void OnArchiveOptions();
	afx_msg void OnArchiveNew();
	afx_msg void OnArchiveOpen();
	afx_msg void OnArchiveClose();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnUpdateArchivePanelResource(CCmdUI *pCmdUI);
	afx_msg void OnUpdateArchivePanel(CCmdUI *pCmdUI);
	afx_msg LRESULT OnTreeViewSelectItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewKeyDelete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewDoubleClick(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewEndDrag(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTreeViewEndEdit(WPARAM wParam, LPARAM lParam);
	void AdjustLayout();
	// �������� ������
	bool ArchiveOpen(const char* path, const char* sign, const char* path_lst, bool is_open);
	// ������� �����
	Archive* GetArchive() {return arch;}
	// �������� ������ ��������
	ACTIVEX* AddResourceModule(const char* name, const char* progID);
	// ������� ������ ��������
	bool RemoveResourceModule(const char* progID);
	// �����
	void Reset();
	// ����������� ������ ��������
	ACTIVEX* EnumResourceModules(size_t& index) {if(index >= 0 && index < activex.size()) return activex[index] ; return NULL;}
	// ������� ������ �������� �� ����������
	ACTIVEX* GetResourceModule(const char* ext);
protected:
	CViewTree m_wndTreeArchive;
	CImageList m_imgTreeArchive;
	CToolBarArchive m_ToolBar;
	// �����
	Archive* arch;
	// ������ ������� ��������
	Array<ACTIVEX*> activex;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
};
