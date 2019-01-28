
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "MainFrm.h"
#include "BaseOptionsDlg.h"
#include "ProjectOptionsDlg.h"
#include "NewDocumentDlg.h"
#include "NewProjectDlg.h"
#include "ShowUnOpenProjectDlg.h"
#include "DocumentAsmEditor.h"
#include "DocumentTxtEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_REGISTERED_MESSAGE(WM_BUILD_STATUS_BAR_MSG, &CMainFrame::OnSetStatusBarMessage)
	ON_COMMAND(ID_ADD_RESOURCE, &CMainFrame::OnAddResource)
	ON_COMMAND(ID_ADD_NEW_RESOURCE, &CMainFrame::OnAddNewResource)
	ON_COMMAND(ID_ARCHIVE_OPTIMIZED, &CMainFrame::OnArchiveOptimized)
	ON_COMMAND(ID_CREATE_LIST_RESOURCE, &CMainFrame::OnCreateListResources)
	ON_COMMAND(ID_RESTORE_RESOURCE, &CMainFrame::OnRestoreResources)
	ON_COMMAND(ID_ARCHIVE_OPTIONS, &CMainFrame::OnArchiveOptions)
	ON_COMMAND(ID_ARCHIVE_CLOSE, &CMainFrame::OnArchiveClose)
	ON_COMMAND(ID_ARCHIVE_NEW, &CMainFrame::OnArchiveNew)
	ON_COMMAND(ID_ARCHIVE_OPEN, &CMainFrame::OnArchiveOpen)
	ON_COMMAND(ID_DOCUMENT_NEW, &CMainFrame::OnDocumentNew)
	ON_COMMAND(ID_PROJECT_NEW, &CMainFrame::OnProjectNew)
	ON_COMMAND(ID_DOCUMENT_OPEN, &CMainFrame::OnDocumentOpen)
	ON_COMMAND(ID_PROJECT_OPEN, &CMainFrame::OnProjectOpen)
	ON_COMMAND(ID_ADD_DOCUMENT, &CMainFrame::OnAddDocument)
	ON_COMMAND(ID_ADD_NEW_DOCUMENT, &CMainFrame::OnAddNewDocument)
	ON_COMMAND(ID_ADD_PROJECT, &CMainFrame::OnAddProject)
	ON_COMMAND(ID_ADD_NEW_PROJECT, &CMainFrame::OnAddNewProject)
	ON_COMMAND(ID_SAVE_ALL, &CMainFrame::OnSaveAll)
	ON_COMMAND(ID_CLOSE_ALL, &CMainFrame::OnCloseAll)
	ON_COMMAND(ID_QUICK_FIND, &CMainFrame::OnQuickFind)
	ON_COMMAND(ID_CONFIGURATIONS, &CMainFrame::OnConfigurations)
	ON_COMMAND(ID_PLATFORMS, &CMainFrame::OnPlatforms)
	ON_COMMAND(ID_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_PROJECT_OPTIONS, &CMainFrame::OnProjectOptions)
	ON_COMMAND(ID_FIND_IN_DOCUMENTS, &CMainFrame::OnFindInDocuments)
	ON_COMMAND(ID_REPLACE_IN_DOCUMENTS, &CMainFrame::OnReplaceInDocuments)
	ON_COMMAND(ID_REBUILD, &CMainFrame::OnRebuild)
	ON_COMMAND(ID_BUILD_PROJECT, &CMainFrame::OnBuildProject)
	ON_COMMAND(ID_BUILD_SOLUTION, &CMainFrame::OnBuildSolution)
	ON_COMMAND(ID_EXECUTE, &CMainFrame::OnExecute)
	ON_COMMAND(ID_PANEL_ARCHIVE, &CMainFrame::OnPanelArchive)
	ON_COMMAND(ID_PANEL_OUTPUT, &CMainFrame::OnPanelOutput)
	ON_COMMAND(ID_PANEL_FIND, &CMainFrame::OnPanelFind)
	ON_COMMAND(ID_PANEL_FILES, &CMainFrame::OnPanelFiles)
	ON_COMMAND(ID_PANEL_MODULES, &CMainFrame::OnPanelModules)
	ON_COMMAND(ID_PANEL_TASK, &CMainFrame::OnPanelTask)
	ON_COMMAND(ID_PANEL_PROPERTIES, &CMainFrame::OnPanelProperties)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_ADD_DOCUMENT, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_ADD_NEW_DOCUMENT, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_ADD_PROJECT, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_ADD_NEW_PROJECT, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_SAVE_ALL, &CMainFrame::OnUpdateIsExistSolution)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_ALL, &CMainFrame::OnUpdateIsExistSolution)
	ON_UPDATE_COMMAND_UI(ID_QUICK_FIND, &CMainFrame::OnUpdateIsExistDocument)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURATIONS, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_PLATFORMS, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_PROJECT_OPTIONS, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_FIND_IN_DOCUMENTS, &CMainFrame::OnUpdateIsExistSolution)
	ON_UPDATE_COMMAND_UI(ID_REPLACE_IN_DOCUMENTS, &CMainFrame::OnUpdateIsExistSolution)
	ON_UPDATE_COMMAND_UI(ID_REBUILD, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_BUILD_PROJECT, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_BUILD_SOLUTION, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_EXECUTE, &CMainFrame::OnUpdateBuild)
	ON_UPDATE_COMMAND_UI(ID_PANEL_ARCHIVE, &CMainFrame::OnUpdatePanelArchive)
	ON_UPDATE_COMMAND_UI(ID_PANEL_OUTPUT, &CMainFrame::OnUpdatePanelOutput)
	ON_UPDATE_COMMAND_UI(ID_PANEL_FIND, &CMainFrame::OnUpdatePanelFind)
	ON_UPDATE_COMMAND_UI(ID_PANEL_FILES, &CMainFrame::OnUpdatePanelFiles)
	ON_UPDATE_COMMAND_UI(ID_PANEL_MODULES, &CMainFrame::OnUpdatePanelModules)
	ON_UPDATE_COMMAND_UI(ID_PANEL_TASK, &CMainFrame::OnUpdatePanelTask)
	ON_UPDATE_COMMAND_UI(ID_PANEL_PROPERTIES, &CMainFrame::OnUpdatePanelProperties)
	ON_UPDATE_COMMAND_UI(ID_ADD_RESOURCE, &CMainFrame::OnUpdateArchiveIsExist)
	ON_UPDATE_COMMAND_UI(ID_ADD_NEW_RESOURCE, &CMainFrame::OnUpdateArchiveIsExist)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_OPTIMIZED, &CMainFrame::OnUpdateArchiveIsExist)
	ON_UPDATE_COMMAND_UI(ID_CREATE_LIST_RESOURCE, &CMainFrame::OnUpdateArchiveIsExist)
	ON_UPDATE_COMMAND_UI(ID_RESTORE_RESOURCE, &CMainFrame::OnUpdateArchiveIsExist)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_CLOSE, &CMainFrame::OnUpdateArchiveIsExist)
	ON_COMMAND(ID_CLOSE_ALL_BUT_THIS, &CMainFrame::OnCloseAllButThis)
END_MESSAGE_MAP()

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CDockablePane* pTabbedBar = NULL;
	String main_folder;
	Array<String> arr_find;

	if(CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles();
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_VS2005;
	mdiTabParams.m_bActiveTabCloseButton = TRUE;
	mdiTabParams.m_bTabIcons = FALSE;
	mdiTabParams.m_bAutoColor = FALSE;
	mdiTabParams.m_bDocumentMenu = FALSE;
	mdiTabParams.m_bEnableTabSwap = TRUE;
	mdiTabParams.m_nTabBorderSize = 3;
	EnableMDITabbedGroups(TRUE, mdiTabParams);
	m_wndRibbonBar.Create(this);
	InitializeRibbon();
	// ����� ������/������
	find_repl_dlg = new CFindReplaceDlg(this);
	find_repl_dlg->Create(CFindReplaceDlg::IDD, this);
	// �������� �������
	if(!m_wndStatusBar.Create(this))
		return -1;
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "�����", TRUE), "�����");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, "", TRUE), "");
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, IDB_BITMAP_MENU_IMAGES);
	// ������� ������
	if(!m_wndModuleView.Create("������", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_MODULE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndArchiveView.Create("�����", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_ARCHIVE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndPropertiesView.Create("��������", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_PROPERTIES_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		return FALSE;
	// �������� ������������
	Utils::getSysDir(NULL, NULL, &main_folder);
	configuration.Open(main_folder + "config_asm_studio.xml");
	if(!m_wndFileView.Create("�����", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_FILE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndOutputView.Create("�����", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_OUTPUT_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndFindView.Create("�����", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_FIND_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndTaskView.Create("������", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_TASK_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		return FALSE;
	// ������������� ������ ��� �������
	m_wndModuleView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_MODULES), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	m_wndFileView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_FILES), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	m_wndTaskView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_TASK), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	m_wndOutputView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_OUTPUT), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	m_wndFindView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_FIND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	m_wndArchiveView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_ARCHIVE), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	m_wndPropertiesView.SetIcon((HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PANEL_PROPERTIES), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0), FALSE);
	UpdateMDITabbedBarsIcons();
	m_wndArchiveView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFindView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndTaskView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndModuleView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutputView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPropertiesView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndPropertiesView);
	DockPane(&m_wndArchiveView);
	DockPane(&m_wndFindView);
	DockPane(&m_wndFileView);
	DockPane(&m_wndTaskView);
	DockPane(&m_wndOutputView);
	m_wndModuleView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndFindView.AttachToTabWnd(&m_wndOutputView, DM_SHOW, TRUE, &pTabbedBar);
	// ������� �������� ��������
	fakeOgx.Create(NULL, NULL, WS_CHILD, CRect(0, 0, 120, 120), this, 15);
	// ������� �����
	CString str_finds(configuration.GetFindReplaceWords(true));
	Utils::explode("$$$$", str_finds, arr_find);
	for(size_t i = 0 ; i < arr_find.size() ; i++)
		m_wndQuickFind->AddItem(arr_find[i]);
	m_wndQuickFind->SelectItem(0);
//	m_bk_text.SubclassWindow(m_hWnd);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CMDIFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

void CMainFrame::InitializeRibbon()
{
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BITMAP_RIBBON_SMALL);
	m_MainButton.SetImage(IDB_RIBBON_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText("����");
	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	// �������� �������� �������
	CList<UINT, UINT> lstQATCmds;
	lstQATCmds.AddTail(ID_PROJECT_NEW);
	lstQATCmds.AddTail(ID_DOCUMENT_NEW);
	lstQATCmds.AddTail(ID_ARCHIVE_NEW);
	lstQATCmds.AddTail(ID_PROJECT_OPEN);
	lstQATCmds.AddTail(ID_DOCUMENT_OPEN);
	lstQATCmds.AddTail(ID_ARCHIVE_OPEN);
	lstQATCmds.AddTail(ID_DOCUMENT_SAVE);
	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);
	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, NULL, m_PanelImages.ExtractIcon(40)));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory("�������� ����", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE);
	pMainPanel->Add(new CMFCRibbonButton(ID_PROJECT_NEW, "������� ������/�������", 24, 24));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_NEW, "������� ��������", 23, 23));
	pMainPanel->Add(new CMFCRibbonButton(ID_ARCHIVE_NEW, "������� �����", 8, 8));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_PROJECT_OPEN, "������� ������/�������", 26, 26));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_OPEN, "������� ��������", 25, 25));
	pMainPanel->Add(new CMFCRibbonButton(ID_ARCHIVE_OPEN, "������� �����", 9, 9));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_SAVE, "��������� ��������", 36, 36));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_SAVE_AS, "��������� �������� ���...", 35, 35));
	pMainPanel->Add(new CMFCRibbonButton(ID_SAVE_ALL, "��������� ��", 34, 34));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_ARCHIVE_CLOSE, "������� �����", 6, 6));
	pMainPanel->Add(new CMFCRibbonButton(ID_CLOSE_ALL, "������� �������", 16, 16));
	pMainPanel->AddRecentFilesList("����� �������� ���������");
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, "�����", 39));
	// ������� ��������� - ������
	CMFCRibbonCategory* pCategoryProject = m_wndRibbonBar.AddCategory("�������", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE);
	// ������� ������
	CMFCRibbonPanel* pPanelProjectContent(pCategoryProject->AddPanel("����������"));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_NEW_DOCUMENT, "�������� ����� ����...", 1, 1));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_NEW_PROJECT, "�������� ����� ������...", 2, 2));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_DOCUMENT, "��������/������� ����...", 0, 0));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_PROJECT, "��������/������� ������...", 4, 4));
	// ������� ������
	CMFCRibbonPanel* pPanelProjectBuild(pCategoryProject->AddPanel("������"));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_EXECUTE, "���������", 19, 19));
	pPanelProjectBuild->Add(new CMFCRibbonSeparator(TRUE));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_BUILD_SOLUTION, "������� �������", 15, 15));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_BUILD_PROJECT, "������� ������", 14, 14));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_REBUILD, "����������� �������", 30, 30));
	// ������� ������
	CMFCRibbonPanel* pPanelProjectConfig(pCategoryProject->AddPanel("������������"));
	pPanelProjectConfig->Add(m_wndPlatform = new CMFCRibbonComboBox(ID_PLATFORMS, FALSE, 130, NULL, 43));
	pPanelProjectConfig->Add(m_wndConfig = new CMFCRibbonComboBox(ID_CONFIGURATIONS, FALSE, 130, NULL, 42));
	pPanelProjectConfig->Add(m_wndQuickFind = new CMFCRibbonComboBox(ID_QUICK_FIND, FALSE, 130, NULL, 41));
	m_wndConfig->EnableDropDownListResize();
	m_wndPlatform->EnableDropDownListResize();
	m_wndQuickFind->EnableDropDownListResize();
	// ������� ������
	CMFCRibbonPanel* pPanelProjectOptions(pCategoryProject->AddPanel("���������"));
	pPanelProjectOptions->Add(new CMFCRibbonButton(ID_PROJECT_OPTIONS, "�������� �������", 28, 28));
	pPanelProjectOptions->Add(new CMFCRibbonButton(ID_OPTIONS, "�����", 27, 27));
	// ������� ��������� - ��������������
	CMFCRibbonCategory* pCategoryEdit(m_wndRibbonBar.AddCategory("��������������", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE));
	// ������� ������
	CMFCRibbonPanel* pPanelClipboard(pCategoryEdit->AddPanel("������"));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_UNDO, "��������", 38, 38));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_REDO, "���������", 31, 31));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_PASTE, "��������", 29, 29));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_CUT, "��������", 18, 18));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_COPY, "����������", 17, 17));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_SELECT_ALL, "�������� ��", 37, 37));
	// ������� ������
	CMFCRibbonPanel* pPanelEditFindReplace(pCategoryEdit->AddPanel("����� � ������"));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_FIND, "�����", 20, 20));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_REPLACE, "������", 32, 32));
	pPanelEditFindReplace->Add(new CMFCRibbonSeparator(TRUE));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_FIND_IN_DOCUMENTS, "����� � ������", 21, 21));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_REPLACE_IN_DOCUMENTS, "������ � ������", 32, 32));
	pPanelEditFindReplace->Add(new CMFCRibbonSeparator(TRUE));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_GOTO_LOCATION, "�������", 22, 22));
	// ������� ������
	CMFCRibbonPanel* pPanelView(pCategoryEdit->AddPanel("������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_OUTPUT, "������ ������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_FIND, "������ ������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_TASK, "������ �����"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_MODULES, "������ �������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_FILES, "������ ������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_ARCHIVE, "������ ������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_PROPERTIES, "������ �������"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, "������ �������"));
	// ������� ��������� - �����
	CMFCRibbonCategory* pCategoryArchive(m_wndRibbonBar.AddCategory("�����", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE));
	CMFCRibbonPanel* pPanelArchiveMain(pCategoryArchive->AddPanel("�������"));
	pPanelArchiveMain->Add(new CMFCRibbonButton(ID_ADD_NEW_RESOURCE, "�������� ����� ������", 3, 3));
	pPanelArchiveMain->Add(new CMFCRibbonButton(ID_ADD_RESOURCE, "�������� ������", 5, 5));
	CMFCRibbonPanel* pPanelArchiveTools(pCategoryArchive->AddPanel("�����������"));
	pPanelArchiveTools->Add(new CMFCRibbonButton(ID_ARCHIVE_OPTIMIZED, "��������������", 10, 10));
	pPanelArchiveTools->Add(new CMFCRibbonButton(ID_CREATE_LIST_RESOURCE, "������� ������", 7, 7));
	pPanelArchiveTools->Add(new CMFCRibbonButton(ID_RESTORE_RESOURCE, "������������ �������", 12, 12));
	CMFCRibbonPanel* pPanelArchiveOptions(pCategoryArchive->AddPanel("���������"));
	pPanelArchiveOptions->Add(new CMFCRibbonButton(ID_ARCHIVE_OPTIONS, "���������", 11, 11));
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif

void CMainFrame::SaveModified()
{
	// 1. ��������� ������������
	configuration.Save();
	// 2. ���������
	GetAllDocuments(NULL, NULL, NULL, true, false);
}

void CMainFrame::OnDocumentNew()
{
	CNewDocumentDlg dlg(ACTION_NEW_DOCUMENT, this);
	if(dlg.DoModal() == IDOK)
	{
		CString folder(Utils::addSlashToFullPath(dlg.folder));
		folder += dlg.name + dlg.ext;
		if(!OpenDocument(folder, NULL, false))
			MessageBox("������. �� ������� ������� ��������!", ASM_STUDIO, MB_ICONERROR);
	}
}

void CMainFrame::OnProjectNew()
{
	CNewProjectDlg dlg(ACTION_NEW_PROJECT, this);
	if(dlg.DoModal() == IDOK)
	{
		CString folder(Utils::addSlashToFullPath(dlg.folder));
		OnCloseAll();
		if(solution.AddProject(dlg.name_sln, dlg.name, folder, dlg.wzd, ACTION_NEW_PROJECT))
		{
			OpenSolution();
		}
		else
		{
			MessageBox("������. �� ������� ������� ������!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnDocumentOpen()
{
	Array<String> arr;
	String folder, cur_dir;

	if(Utils::dlgSaveOrOpenFile(true, "������� ����", "������������ �����|*.asm|���������� �����|*.inc|��� �����|*.*|\0\0", "asm", folder, m_hWnd, &arr))
	{
		Utils::getSysDir(NULL, &cur_dir);
		for(size_t i = 0 ; i < arr.size() ; i++)
		{
			CString filename(arr[i]);
			if(filename.ReverseFind('\\') == -1)
				filename = cur_dir + arr[i];
			if(!OpenDocument(filename, NULL, true))
				MessageBox("������. �� ������� ������� ���� \"" + filename + "\"!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnProjectOpen()
{
	String folder, cur_dir;
	if(Utils::dlgSaveOrOpenFile(true, "������� ������", "������� � �������|*.asmp;*.asms|\0\0", "asmp", folder, m_hWnd, NULL))
	{
		OnCloseAll();
		Utils::getSysDir(NULL, &cur_dir);
		if(folder.reverseFind('\\') == -1)
			folder = cur_dir + folder;
		CString ext(folder.mid(folder.reverseFind('.')));
		ext.MakeLower();
		if((ext == ".asmp" ? (solution.AddProject(NULL, NULL, folder, NULL, ACTION_OPEN_PROJECT) != NULL) : solution.Open(folder)))
		{
			OpenSolution();
		}
		else
		{
			MessageBox("������. �� ������� ������� �������!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnAddDocument()
{
	Array<String> arr;
	String folder, cur_dir;
	CElement* elem(NULL), *elem1(NULL);
	CProject* prj(GetCurrentProject());

	if(Utils::dlgSaveOrOpenFile(true, "�������� ����", "������������ �����|*.asm|���������� �����|*.inc|��� �����|*.*|\0\0", "asm", folder, NULL, &arr))
	{
		Utils::getSysDir(NULL, &cur_dir);
		is_start_solution = (arr.size() > 1);
		for(size_t i = 0 ; i < arr.size() ; i++)
		{
			CString filename(arr[i]);
			if(filename.ReverseFind('\\') == -1)
				filename = cur_dir + arr[i];
			if(!elem1) elem1 = elem;
			if((elem = prj->AddElement(filename)))
			{
				// �������� � ������ ������ � �������
				m_wndFileView.TreeAddFile(elem, prj->GetName(), true);
				m_wndModuleView.TreeAddModule(elem, prj->GetName(), true);
				// ������� ��������
				if(!OpenDocument(filename, elem, true))
					MessageBox("������. �� ������� ������� �������� \"" + filename + "\"!", ASM_STUDIO, MB_ICONERROR);
			}
		}
		is_start_solution = false;
		if(elem1)
		{
			OpenDocument(elem1->path, elem1, true);
		}
	}
}

void CMainFrame::OnAddNewDocument()
{
	CNewDocumentDlg dlg(ACTION_ADD_NEW_DOCUMENT);

	if(dlg.DoModal() == IDOK)
	{
		CElement* elem;
		CProject* prj(GetCurrentProject());
		CString folder(prj->GetPath());
		folder = (folder.Left(folder.ReverseFind('\\') + 1) + dlg.name + dlg.ext);
		// �������� �������� � ������
		if((elem = prj->AddElement(folder)))
		{
			// �������� � ������ ������ � �������
			m_wndFileView.TreeAddFile(elem, prj->GetName(), true);
			m_wndModuleView.TreeAddModule(elem, prj->GetName(), true);
			// ������� ��������
			if(!OpenDocument(folder, elem, false))
				MessageBox("������. �� ������� ������� ����!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnAddProject()
{
	String folder, cur_dir;
	Array<String> arr;
	CProject* prj;

	if(Utils::dlgSaveOrOpenFile(true, "�������� ������", "�������|*.asmp|\0\0", "asmp", folder, m_hWnd, &arr))
	{
		Utils::getSysDir(NULL, &cur_dir);
		for(size_t i = 0 ; i < arr.size() ; i++)
		{
			CString filename(arr[i]);
			if(filename.ReverseFind('\\') == -1)
				filename = cur_dir + arr[i];
			if((prj = solution.AddProject(NULL, NULL, filename, NULL, ACTION_ADD_OPEN_PROJECT)))
			{
				// �������� ��� � ������ ������ � �������
				m_wndFileView.MakeTree(prj);
				m_wndModuleView.MakeTree(prj);
			}
		}
	}
}

void CMainFrame::OnAddNewProject()
{
	CNewProjectDlg dlg(ACTION_ADD_NEW_PROJECT, this);
	if(dlg.DoModal() == IDOK)
	{
		CString folder(Utils::addSlashToFullPath(dlg.folder));
		CProject* prj;
		if((prj = solution.AddProject(dlg.name_sln, dlg.name, folder, dlg.wzd, ACTION_ADD_NEW_PROJECT)))
		{
			// �������� ��� � ������ ������ � �������
			m_wndFileView.MakeTree(prj);
			m_wndModuleView.MakeTree(prj);
		}
		else
		{
			MessageBox("������. �� ������� ������� ������!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnSaveAll()
{
	// 1. ��������� �������
	solution.Save();
	// 2. ��������� ��� ���������������� ���������
	GetAllDocuments(NULL, NULL, NULL, false, false, false);
}

void CMainFrame::OnCloseAll()
{
	if(solution.Close())
	{
		Array<CDocumentCommon*> docs;
		// ������� ��� ���������
		GetAllDocuments(&docs, NULL, NULL, true, false);
		for(size_t i = 0 ; i < docs.size() ; i++)
			docs[i]->OnCloseDocument();
		// �������� ��� ������
		m_wndFileView.Reset();
		m_wndModuleView.Reset();
		m_wndFindView.Reset();
		m_wndOutputView.Reset();
		m_wndTaskView.Reset();
		m_wndArchiveView.Reset();
		// ������ �� ������� ��������� � �������
		m_wndConfig->RemoveAllItems();
		m_wndPlatform->RemoveAllItems();
	}
}

void CMainFrame::OnQuickFind()
{
	GetFindReplaceDlg()->str_find = m_wndQuickFind->GetItem(m_wndQuickFind->GetCurSel());
	GetFindReplaceDlg()->OnAction(CFindReplaceDlg::ACT_FIND, CFindReplaceDlg::BAR_CURRENT_DOCUMENT);
}

void CMainFrame::OnConfigurations()
{
	solution.SetCurrentConfig(m_wndConfig->GetItem(m_wndConfig->GetCurSel()));
}

void CMainFrame::OnPlatforms()
{
	CConfig* cfg;
	size_t index(0);
	int cur_plt(m_wndPlatform->GetCurSel());
	m_wndConfig->RemoveAllItems();
	if(cur_plt != -1)
	{
		CString nm_platform(m_wndPlatform->GetItem(cur_plt));
		CPlatform* plt(solution.GetPlatform(nm_platform));
		if(plt)
		{
			solution.SetCurrentPlatform(nm_platform);
			// � ����������� �� ��������� ��������� - ���������� ��� �� ������������ � ������� ������������
			while((cfg = plt->EnumConfigs(index)))
				m_wndConfig->AddItem(cfg->GetName());
			m_wndConfig->SelectItem(solution.GetCurrentConfig());
		}
	}
}

void CMainFrame::OnOptions()
{
	CBaseOptionsDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnProjectOptions()
{
	CProjectOptionsDlg dlg;
	dlg.DoModal();
}

void CMainFrame::OnFindInDocuments()
{
	// ���������� ������� ��������
	CDocument* doc(GetDocument());
	COgxTextEditor* ogx(NULL);
	if(doc->IsKindOf(RUNTIME_CLASS(CDocumentAsmEditor)))
		ogx = ((CDocumentAsmEditor*)doc)->GetOgx();
	else if(doc->IsKindOf(RUNTIME_CLASS(CDocumentTxtEditor)))
		ogx = ((CDocumentTxtEditor*)doc)->GetOgx();
	// ����� ����� ��� ��������
	find_repl_dlg->Show(CFindReplaceDlg::STYLE_FIND_IN_FILES, ogx ? ogx->GetWordFromCaretPosition() : "", FALSE);
}

void CMainFrame::OnReplaceInDocuments()
{
	// ���������� ������� ��������
	CDocument* doc(GetDocument());
	COgxTextEditor* ogx(NULL);
	if(doc->IsKindOf(RUNTIME_CLASS(CDocumentAsmEditor)))
		ogx = ((CDocumentAsmEditor*)doc)->GetOgx();
	else if(doc->IsKindOf(RUNTIME_CLASS(CDocumentTxtEditor)))
		ogx = ((CDocumentTxtEditor*)doc)->GetOgx();
	// ����� ����� ��� ��������
	find_repl_dlg->Show(CFindReplaceDlg::STYLE_REPLACE_IN_FILES, ogx ? ogx->GetWordFromCaretPosition() : "", FALSE);
}

void CMainFrame::OnRebuild()
{
	solution.Rebuild();
}

void CMainFrame::OnBuildProject()
{
	solution.BuildProject(GetCurrentProject());
}

void CMainFrame::OnBuildSolution()
{
	solution.Build();
}

void CMainFrame::OnExecute()
{
	solution.Execute();
}

void CMainFrame::OnPanelArchive()
{
	BOOL is = !m_wndArchiveView.IsPaneVisible();
	ShowPane(&m_wndArchiveView, is, is, is);
}

void CMainFrame::OnPanelOutput()
{
	BOOL is = !m_wndOutputView.IsPaneVisible();
	ShowPane(&m_wndOutputView, is, is, is);
}

void CMainFrame::OnPanelFind()
{
	BOOL is = !m_wndFindView.IsPaneVisible();
	ShowPane(&m_wndFindView, is, is, is);
}

void CMainFrame::OnPanelFiles()
{
	BOOL is = !m_wndFileView.IsPaneVisible();
	ShowPane(&m_wndFileView, is, is, is);
}

void CMainFrame::OnPanelModules()
{
	BOOL is = !m_wndModuleView.IsPaneVisible();
	ShowPane(&m_wndModuleView, is, is, is);
}

void CMainFrame::OnPanelTask()
{
	BOOL is = !m_wndTaskView.IsPaneVisible();
	ShowPane(&m_wndTaskView, is, is, is);
}

void CMainFrame::OnPanelProperties()
{
	BOOL is = !m_wndPropertiesView.IsPaneVisible();
	ShowPane(&m_wndPropertiesView, is, is, is);
}

void CMainFrame::OnUpdateBuild(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetCurrentProject() != NULL && is_build == false);
}

void CMainFrame::OnUpdateExecute(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(solution.GetExecuteProject() != NULL && is_build == false);
}

void CMainFrame::OnUpdateIsExistSolution(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetCurrentProject() != NULL);
}

void CMainFrame::OnUpdateIsExistDocument(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument() != NULL);
}

void CMainFrame::OnUpdatePanelArchive(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndArchiveView.IsPaneVisible());
}

void CMainFrame::OnUpdatePanelOutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutputView.IsPaneVisible());
}

void CMainFrame::OnUpdatePanelFind(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndFindView.IsPaneVisible());
}

void CMainFrame::OnUpdatePanelFiles(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndFileView.IsPaneVisible());
}

void CMainFrame::OnUpdatePanelModules(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndModuleView.IsPaneVisible());
}

void CMainFrame::OnUpdatePanelTask(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndTaskView.IsPaneVisible());
}

void CMainFrame::OnUpdatePanelProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropertiesView.IsPaneVisible());
}

void CMainFrame::OnArchiveNew()
{
	m_wndArchiveView.OnArchiveNew();
}

void CMainFrame::OnArchiveOpen()
{
	m_wndArchiveView.OnArchiveOpen();
}

void CMainFrame::OnArchiveClose()
{
	m_wndArchiveView.OnArchiveClose();
}

void CMainFrame::OnUpdateArchiveIsExist(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_wndArchiveView.GetArchive() != NULL);
}

BOOL CMainFrame::OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bTabDrop)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_POPUP_DOCUMENT, point.x, point.y, this, TRUE);
	return TRUE;
}

CDocumentCommon* CMainFrame::GetDocument()
{
	CMDIChildWnd* child(MDIGetActive());
	return (child ? (CDocumentCommon*)child->GetActiveDocument() : NULL);
}

void CMainFrame::OnAddNewResource()
{
	m_wndArchiveView.OnAddNewResource();
}

void CMainFrame::OnAddResource()
{
	m_wndArchiveView.OnAddResource();
}

void CMainFrame::OnArchiveOptimized()
{
	m_wndArchiveView.OnArchiveOptimized();
}

void CMainFrame::OnCreateListResources()
{
	m_wndArchiveView.OnCreateListResources();
}

void CMainFrame::OnRestoreResources()
{
	m_wndArchiveView.OnRestoreResources();
}

void CMainFrame::OnArchiveOptions()
{
	m_wndArchiveView.OnArchiveOptions();
}

void CMainFrame::OnCloseAllButThis() 
{
	// ���� ��� � ��� �������� ������� - ����� ��� ����������, ����� �������� � ����� �� ������
	Array<CDocumentCommon*> docs;
	GetAllDocuments(&docs, GetDocument(), NULL, false, false);
	for(size_t i = 0 ; i < docs.size() ; i++)
		docs[i]->OnCloseDocument();
}

bool CMainFrame::OpenSolution()
{
	CProject* prj;
	CElement* elem;
	CElement* active_elem(NULL);
	size_t index(0);
	// 1. ������� ���� ��������
	while((prj = solution.EnumProjects(index)))
	{
		size_t index_f(0);
		// 2. ������������ ������ ������
		m_wndFileView.MakeTree(prj);
		// 3. ������������ ������ �������
		m_wndModuleView.MakeTree(prj);
		// 4. �������� ����������
		index_f = 0;
		is_start_solution = (prj->GetCountElements() > 1);
		while((elem = prj->EnumElements(index_f)))
		{
			if(elem->is_open)
			{
				if(!active_elem) active_elem = elem;
				OpenDocument(elem->path, elem, true);
			}
		}
		is_start_solution = false;
	}
	// 5. ��������� ��������� ���������
	elem = solution.GetCurrentElement();
	if(!elem) elem = active_elem;
	if(elem)
	{
		OpenDocument(elem->path, elem, true);
	}
	// 6. ������� �����
	if(!OpenArchive(solution.GetArchivePath()))
		solution.SetArchivePath("");
	return true;
}

bool CMainFrame::OpenArchive(const char* path)
{
	/*
	if(!m_wndArchiveView.ArchiveOpen(path, NULL, NULL, true))
	{
		MessageBox("������. �� ������� ������� �����!", ASM_STUDIO, MB_ICONERROR);
		return false;
	}
	*/
	return true;
}

CDocumentCommon* CMainFrame::OpenDocument(const char* path, CElement* elem, bool is_open)
{
	CMultiDocTemplate* templ;
	CDocumentCommon* doc(NULL);
	COgxTextEditor* ogx(NULL);

	CString filename(path);
	CString ext(filename.Mid(filename.ReverseFind('.')));
	ext.MakeLower();

	if(elem == NULL)
	{
		elem = solution.AddFakeElement(path);
	}
	if(ext == ".asm" || ext == ".inc")
		templ = theApp.docTemplAsm;
	else if(ext == ".txt")
		templ = theApp.docTemplTxt;
	else
		templ = theApp.docTemplBin;
	// ���������, ���� ��� ������ - �� ������� �������
	if(elem && elem->doc)
	{
		doc = elem->doc;
		MDIActivate(doc->GetView()->GetParentFrame());
	}
	else if((doc = (is_open ? (CDocumentCommon*)templ->OpenDocumentFile(filename) : (CDocumentCommon*)templ->OpenDocumentFile(NULL))))
	{
		if(!is_open)
		{
			doc->SetPathName(filename);
		}
		if(elem)
		{
			elem->is_open = true;
			elem->doc = doc;
			doc->elem = elem;
			// ��������� ��������� ����� �� ������ ������
			doc->SetTitle(elem->path_modules);
			if((ogx = doc->GetOgx()))
			{
				// ���������� ������ ��������
				doc->FillListProcedures(elem);
				// ���������� ��������
				for(size_t i = 0 ; i < elem->bookmarks.size() ; i++)
					ogx->SetStatusLine(elem->bookmarks[i].number, ogx->GetStatusLine(elem->bookmarks[i].number) | elem->bookmarks[i].status, FALSE);
				// ���������� ������� �������
				ogx->SetCaretPosition(elem->x, elem->y, FALSE);
				ogx->UpdateAll(0);
			}
		}
	}
	return doc;
}

static CDocumentCommon* EnumDocuments(CMultiDocTemplate* templ, POSITION& pos)
{
	CDocumentCommon* doc((CDocumentCommon*)templ->GetNextDoc(pos));
	return doc;
}

int CMainFrame::GetAllDocuments(Array<CDocumentCommon*>* all_docs, CDocumentCommon* cur_doc, CProject* prj, bool is_sln, bool is_find, bool is_prompt)
{
	CStringArray arr;
	size_t i;
	Array<CDocumentCommon*> mdocs;
	CDocumentCommon* doc;
	POSITION pos;
	if(is_sln)
	{
		if(solution.IsModify())
			arr.Add("�������");
	}
	// ������� ��� �������� ���������
	// �������, ����� ��� ���������������� �/��� ����������� � ����������� �������
	// 1.1. ��������
	pos = theApp.docTemplAsm->GetFirstDocPosition();
	while(pos)
	{
		if((doc = EnumDocuments(theApp.docTemplAsm, pos)) == cur_doc)
			continue;
		if(prj && doc->elem && doc->elem->prj != prj)
			continue;
		if(all_docs)
			all_docs->add(doc);
		if(!is_find && doc->IsModified())
		{
			arr.Add(doc->GetTitle());
			mdocs.add(doc);
		}
	}
	// 1.2. ���������
	pos = theApp.docTemplTxt->GetFirstDocPosition();
	while(pos)
	{
		if((doc = EnumDocuments(theApp.docTemplTxt, pos)) == cur_doc)
			continue;
		if(prj && doc->elem && doc->elem->prj != prj)
			continue;
		if(all_docs)
			all_docs->add(doc);
		if(!is_find && doc->IsModified())
		{
			arr.Add(doc->GetTitle());
			mdocs.add(doc);
		}
	}
	// 1.3. ��������
	pos = theApp.docTemplBin->GetFirstDocPosition();
	while(pos)
	{
		if((doc = EnumDocuments(theApp.docTemplBin, pos)) == cur_doc)
			continue;
		if(prj && doc->elem && doc->elem->prj != prj)
			continue;
		if(all_docs)
			all_docs->add(doc);
		if(!is_find && doc->IsModified())
		{
			arr.Add(doc->GetTitle());
			mdocs.add(doc);
		}
	}
	// 1.3. �������
	pos = theApp.docTemplOgx->GetFirstDocPosition();
	while(pos)
	{
		if((doc = EnumDocuments(theApp.docTemplOgx, pos)) == cur_doc)
			continue;
		if(prj && doc->elem && doc->elem->prj != prj)
			continue;
		if(all_docs)
			all_docs->add(doc);
		if(!is_find && doc->IsModified())
		{
			arr.Add(doc->GetTitle());
			mdocs.add(doc);
		}
	}
	// 2. ������ ������ �������������
	if(arr.GetSize() > 0)
	{
		bool is(true);
		if(is_prompt)
		{
			CShowUnOpenProjectDlg dlg("���������� ���������", "����������������:", "���������", &arr);
			is = (dlg.DoModal() == IDOK);
		}
		if(is)
		{
			// 3. ��������� ����������������
			if(is_sln)
			{
				if(solution.IsModify()) solution.Save(); else solution.MiniSave();
			}
			// 3.1. ���������
			for(i = 0 ; i < mdocs.size() ; i++)
				mdocs[i]->OnSaveDocument(NULL);
		}
		else
		{
			for(i = 0 ; i < mdocs.size() ; i++)
				mdocs[i]->SetModifiedFlag(FALSE);
		}
	}
	return (all_docs ? all_docs->size() : 0);
}

void CMainFrame::UpdatePlatforms()
{
	Array<CPlatform*>* plts(solution.GetAllPlatforms());
	m_wndPlatform->RemoveAllItems();
	// ���������� � ������ ��� ���������
	for(size_t i = 0 ; i < plts->size() ; i++)
		m_wndPlatform->AddItem(plts->get(i)->GetName());
	m_wndPlatform->SelectItem(solution.GetCurrentPlatform());
	OnPlatforms();
}

CDocumentCommon* CMainFrame::GotoDocument(const CString& pth, long num)
{
	CProject* prj;
	CDocumentCommon* doc;
	// ����� �������
	size_t index(0);
	bool is = false;
	while((prj = solution.EnumProjects(index)))
	{
		CElement* elem;
		if((elem = prj->GetElement(pth, NULL, NULL)))
		{
			// ������� ��������
			doc = OpenDocument(elem->path, elem, true);
			is = true;
			break;
		}
	}
	if(!is)
	{
		doc = OpenDocument(pth, NULL, true);
	}
	if(doc)
	{
		COgxTextEditor* ogx(doc->GetOgx());
		// ��������� ������� �� ��������� ������
		ogx->GotoLine(num, FALSE);
		// ���������� ����� �� ���������
		ogx->SetFocus();
	}

	return doc;
}

LRESULT CMainFrame::OnSetStatusBarMessage(WPARAM msg, LPARAM lParam)
{
	m_wndStatusBar.SetInformation((const char*)msg);
	return 0;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	SetActiveWindow();
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	for(UINT iFile = 0; iFile < nFiles; iFile++)
	{
		CString filename;
		::DragQueryFile(hDropInfo, iFile, filename.GetBuffer(MAX_PATH), MAX_PATH);
		filename.ReleaseBuffer();
		// ��������� �� ����������
		String ext(Utils::extractFileExtFromFullPath(filename, TRUE));
		if(ext == ".asm" || ext == ".inc" || ext == ".txt")
			OpenDocument(filename, NULL, true);
	}
	::DragFinish(hDropInfo);
}
