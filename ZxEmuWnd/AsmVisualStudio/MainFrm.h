
#pragma once

#include "PanelFileView.h"
#include "PanelModuleView.h"
#include "PanelTaskView.h"
#include "PanelFindView.h"
#include "PanelArchiveView.h"
#include "PanelOutputView.h"
#include "PanelPropertiesView.h"
#include "GotoLineDlg.h"
#include "FindReplaceDlg.h"
#include "Configuration.h"
#include "Project.h"
#include "DocumentCommon.h"
#include "BackgroundText.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() : is_build(false), find_repl_dlg(NULL), is_start_solution(false) {}
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~CMainFrame() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnCloseAllButThis();
	afx_msg void OnAddResource();
	afx_msg void OnAddNewResource();
	afx_msg void OnArchiveOptimized();
	afx_msg void OnCreateListResources();
	afx_msg void OnRestoreResources();
	afx_msg void OnArchiveOptions();
	afx_msg void OnArchiveNew();
	afx_msg void OnArchiveOpen();
	afx_msg void OnArchiveClose();
	afx_msg void OnUpdateIsExistArchive(CCmdUI* pCmdUI);
	afx_msg void OnDocumentNew();
	afx_msg void OnProjectNew();
	afx_msg void OnDocumentOpen();
	afx_msg void OnProjectOpen();
	afx_msg void OnAddDocument();
	afx_msg void OnAddNewDocument();
	afx_msg void OnAddProject();
	afx_msg void OnAddNewProject();
	afx_msg void OnSaveAll();
	afx_msg void OnCloseAll();
	afx_msg void OnQuickFind();
	afx_msg void OnConfigurations();
	afx_msg void OnPlatforms();
	afx_msg void OnOptions();
	afx_msg void OnProjectOptions();
	afx_msg void OnFindInDocuments();
	afx_msg void OnReplaceInDocuments();
	afx_msg void OnRebuild();
	afx_msg void OnBuildProject();
	afx_msg void OnBuildSolution();
	afx_msg void OnExecute();
	afx_msg void OnPanelArchive();
	afx_msg void OnPanelOutput();
	afx_msg void OnPanelFind();
	afx_msg void OnPanelFiles();
	afx_msg void OnPanelModules();
	afx_msg void OnPanelTask();
	afx_msg void OnPanelProperties();
	afx_msg void OnUpdateBuild(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExecute(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIsExistSolution(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIsExistDocument(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelArchive(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelOutput(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelFind(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelFiles(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelModules(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelTask(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePanelProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateArchiveIsExist(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSetStatusBarMessage(WPARAM msg, LPARAM lParam);
	void OnDropFiles(HDROP hDropInfo);
	// интерфейсные методы
	CMFCRibbonStatusBar* GetStatusBar() {return &m_wndStatusBar;}
	CPanelFileView* GetPanelFiles() {return &m_wndFileView;}
	CPanelModuleView* GetPanelModules() {return &m_wndModuleView;}
	CPanelFindView* GetPanelFind() {return &m_wndFindView;}
	CPanelOutputView* GetPanelOutput() {return &m_wndOutputView;}
	CPanelTaskView* GetPanelTask() {return &m_wndTaskView;}
	CPanelArchiveView* GetPanelArchive() {return &m_wndArchiveView;}
	CPanelPropertiesView* GetPanelProperties() {return &m_wndPropertiesView;}
	CMFCRibbonComboBox* GetWndConfig() {return m_wndConfig;}
	CMFCRibbonComboBox* GetWndPlatform() {return m_wndPlatform;}
	CMFCRibbonComboBox* GetWndQuickFind() {return m_wndQuickFind;}
	CSolution* GetSolution() {return &solution;}
	CConfiguration* GetConfiguration() {return &configuration;}
	CFindReplaceDlg* GetFindReplaceDlg() {return find_repl_dlg;}
	CDocumentCommon* GetDocument();
	CProject* GetCurrentProject() {return solution.GetCurrentProject();}
	CProject* SetCurrentProject(const char* name) {return solution.SetCurrentProject(name);}
	COgxTextEditor* GetFakeOgx() {return &fakeOgx;}
	CDocumentCommon* GotoDocument(const CString& pth, long num);
	void SetBuildFlag(bool use) {is_build = use;}
	bool IsProcessBuild() const {return is_build;}
	bool IsStartSolution() const {return is_start_solution;}
	// обновить платформы
	void UpdatePlatforms();
	// открытие документа
	CDocumentCommon* OpenDocument(const char* path, CElement* elem, bool is_open);
	void SaveModified();
	bool OpenSolution();
	bool OpenArchive(const char* path);
	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bTabDrop);
	// вернуть все открытые документы
	int GetAllDocuments(Array<CDocumentCommon*>* all_docs, CDocumentCommon* cur_doc, CProject* prj, bool is_sln, bool is_find, bool is_prompt = true);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void InitializeRibbon();
	bool is_start_solution;
	bool is_build;
	// лента
	CMFCRibbonComboBox* m_wndConfig;
	CMFCRibbonComboBox* m_wndPlatform;
	CMFCRibbonComboBox* m_wndQuickFind;
	CMFCRibbonBar m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	// панели
	CMFCRibbonStatusBar m_wndStatusBar;
	CPanelFileView m_wndFileView;
	CPanelModuleView m_wndModuleView;
	CPanelFindView m_wndFindView;
	CPanelOutputView m_wndOutputView;
	CPanelArchiveView m_wndArchiveView;
	CPanelPropertiesView m_wndPropertiesView;
	CPanelTaskView m_wndTaskView;
	// формы
	CFindReplaceDlg* find_repl_dlg;
	// базовые элементы
	CBackgroundText m_bk_text;
	CSolution solution;
	CConfiguration configuration;
	// "ложный" объект редактора
	COgxTextEditor fakeOgx;
	DECLARE_MESSAGE_MAP()
};
