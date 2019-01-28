
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
	// форма поиска/замены
	find_repl_dlg = new CFindReplaceDlg(this);
	find_repl_dlg->Create(CFindReplaceDlg::IDD, this);
	// создание панелей
	if(!m_wndStatusBar.Create(this))
		return -1;
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, "Готов", TRUE), "Готов");
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, "", TRUE), "");
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, IDB_BITMAP_MENU_IMAGES);
	// создаем панели
	if(!m_wndModuleView.Create("Модули", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_MODULE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndArchiveView.Create("Архив", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_ARCHIVE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndPropertiesView.Create("Свойства", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_PROPERTIES_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		return FALSE;
	// загрузка конфигурации
	Utils::getSysDir(NULL, NULL, &main_folder);
	configuration.Open(main_folder + "config_asm_studio.xml");
	if(!m_wndFileView.Create("Файлы", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_FILE_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndOutputView.Create("Вывод", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_OUTPUT_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndFindView.Create("Поиск", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_FIND_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
		return FALSE;
	if(!m_wndTaskView.Create("Задачи", this, CRect(0, 0, 200, 200), TRUE, ID_PANEL_TASK_VIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
		return FALSE;
	// устанавливаем иконки для панелей
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
	// создать фейковый редактор
	fakeOgx.Create(NULL, NULL, WS_CHILD, CRect(0, 0, 120, 120), this, 15);
	// быстрый поиск
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
	m_MainButton.SetToolTipText("Файл");
	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	// комманды быстрого доступа
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
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory("Основное меню", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE);
	pMainPanel->Add(new CMFCRibbonButton(ID_PROJECT_NEW, "Создать проект/систему", 24, 24));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_NEW, "Создать документ", 23, 23));
	pMainPanel->Add(new CMFCRibbonButton(ID_ARCHIVE_NEW, "Создать архив", 8, 8));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_PROJECT_OPEN, "Открыть проект/систему", 26, 26));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_OPEN, "Открыть документ", 25, 25));
	pMainPanel->Add(new CMFCRibbonButton(ID_ARCHIVE_OPEN, "Открыть архив", 9, 9));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_SAVE, "Сохранить документ", 36, 36));
	pMainPanel->Add(new CMFCRibbonButton(ID_DOCUMENT_SAVE_AS, "Сохранить документ как...", 35, 35));
	pMainPanel->Add(new CMFCRibbonButton(ID_SAVE_ALL, "Сохранить всё", 34, 34));
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));
	pMainPanel->Add(new CMFCRibbonButton(ID_ARCHIVE_CLOSE, "Закрыть архив", 6, 6));
	pMainPanel->Add(new CMFCRibbonButton(ID_CLOSE_ALL, "Закрыть систему", 16, 16));
	pMainPanel->AddRecentFilesList("Ранее открытые документы");
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, "Выход", 39));
	// создаем категорию - Проект
	CMFCRibbonCategory* pCategoryProject = m_wndRibbonBar.AddCategory("Главная", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE);
	// создаем панель
	CMFCRibbonPanel* pPanelProjectContent(pCategoryProject->AddPanel("Содержимое"));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_NEW_DOCUMENT, "Добавить новый файл...", 1, 1));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_NEW_PROJECT, "Добавить новый проект...", 2, 2));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_DOCUMENT, "Добавить/открыть файл...", 0, 0));
	pPanelProjectContent->Add(new CMFCRibbonButton(ID_ADD_PROJECT, "Добавить/открыть проект...", 4, 4));
	// создаем панель
	CMFCRibbonPanel* pPanelProjectBuild(pCategoryProject->AddPanel("Сборка"));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_EXECUTE, "Выполнить", 19, 19));
	pPanelProjectBuild->Add(new CMFCRibbonSeparator(TRUE));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_BUILD_SOLUTION, "Собрать систему", 15, 15));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_BUILD_PROJECT, "Собрать проект", 14, 14));
	pPanelProjectBuild->Add(new CMFCRibbonButton(ID_REBUILD, "Пересобрать систему", 30, 30));
	// создаем панель
	CMFCRibbonPanel* pPanelProjectConfig(pCategoryProject->AddPanel("Конфигурация"));
	pPanelProjectConfig->Add(m_wndPlatform = new CMFCRibbonComboBox(ID_PLATFORMS, FALSE, 130, NULL, 43));
	pPanelProjectConfig->Add(m_wndConfig = new CMFCRibbonComboBox(ID_CONFIGURATIONS, FALSE, 130, NULL, 42));
	pPanelProjectConfig->Add(m_wndQuickFind = new CMFCRibbonComboBox(ID_QUICK_FIND, FALSE, 130, NULL, 41));
	m_wndConfig->EnableDropDownListResize();
	m_wndPlatform->EnableDropDownListResize();
	m_wndQuickFind->EnableDropDownListResize();
	// создаем панель
	CMFCRibbonPanel* pPanelProjectOptions(pCategoryProject->AddPanel("Настройки"));
	pPanelProjectOptions->Add(new CMFCRibbonButton(ID_PROJECT_OPTIONS, "Свойства системы", 28, 28));
	pPanelProjectOptions->Add(new CMFCRibbonButton(ID_OPTIONS, "Опции", 27, 27));
	// создать категорию - Редактирование
	CMFCRibbonCategory* pCategoryEdit(m_wndRibbonBar.AddCategory("Редактирование", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE));
	// создаем панель
	CMFCRibbonPanel* pPanelClipboard(pCategoryEdit->AddPanel("Правка"));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_UNDO, "Отменить", 38, 38));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_REDO, "Повторить", 31, 31));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_PASTE, "Вставить", 29, 29));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_CUT, "Вырезать", 18, 18));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_COPY, "Копировать", 17, 17));
	pPanelClipboard->Add(new CMFCRibbonButton(ID_SELECT_ALL, "Выделить всё", 37, 37));
	// создаем панель
	CMFCRibbonPanel* pPanelEditFindReplace(pCategoryEdit->AddPanel("Поиск и Замена"));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_FIND, "Поиск", 20, 20));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_REPLACE, "Замена", 32, 32));
	pPanelEditFindReplace->Add(new CMFCRibbonSeparator(TRUE));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_FIND_IN_DOCUMENTS, "Поиск в файлах", 21, 21));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_REPLACE_IN_DOCUMENTS, "Замена в файлах", 32, 32));
	pPanelEditFindReplace->Add(new CMFCRibbonSeparator(TRUE));
	pPanelEditFindReplace->Add(new CMFCRibbonButton(ID_GOTO_LOCATION, "Перейти", 22, 22));
	// создаем панель
	CMFCRibbonPanel* pPanelView(pCategoryEdit->AddPanel("Панели"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_OUTPUT, "Панель вывода"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_FIND, "Панель поиска"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_TASK, "Панель задач"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_MODULES, "Панель модулей"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_FILES, "Панель файлов"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_ARCHIVE, "Панель архива"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_PANEL_PROPERTIES, "Панель свойств"));
	pPanelView->Add(new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, "Панель статуса"));
	// создать категорию - Архив
	CMFCRibbonCategory* pCategoryArchive(m_wndRibbonBar.AddCategory("Архив", IDB_BITMAP_RIBBON_SMALL, IDB_BITMAP_RIBBON_LARGE));
	CMFCRibbonPanel* pPanelArchiveMain(pCategoryArchive->AddPanel("Главные"));
	pPanelArchiveMain->Add(new CMFCRibbonButton(ID_ADD_NEW_RESOURCE, "Добавить новый ресурс", 3, 3));
	pPanelArchiveMain->Add(new CMFCRibbonButton(ID_ADD_RESOURCE, "Добавить ресурс", 5, 5));
	CMFCRibbonPanel* pPanelArchiveTools(pCategoryArchive->AddPanel("Инструменты"));
	pPanelArchiveTools->Add(new CMFCRibbonButton(ID_ARCHIVE_OPTIMIZED, "Оптимизировать", 10, 10));
	pPanelArchiveTools->Add(new CMFCRibbonButton(ID_CREATE_LIST_RESOURCE, "Создать список", 7, 7));
	pPanelArchiveTools->Add(new CMFCRibbonButton(ID_RESTORE_RESOURCE, "Восстановить ресурсы", 12, 12));
	CMFCRibbonPanel* pPanelArchiveOptions(pCategoryArchive->AddPanel("Настройка"));
	pPanelArchiveOptions->Add(new CMFCRibbonButton(ID_ARCHIVE_OPTIONS, "Параметры", 11, 11));
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
	// 1. сохранить конфигурацию
	configuration.Save();
	// 2. документы
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
			MessageBox("Ошибка. Не удалось создать документ!", ASM_STUDIO, MB_ICONERROR);
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
			MessageBox("Ошибка. Не удалось создать проект!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnDocumentOpen()
{
	Array<String> arr;
	String folder, cur_dir;

	if(Utils::dlgSaveOrOpenFile(true, "Открыть файл", "Ассемблерные файлы|*.asm|Включаемые файлы|*.inc|Все файлы|*.*|\0\0", "asm", folder, m_hWnd, &arr))
	{
		Utils::getSysDir(NULL, &cur_dir);
		for(size_t i = 0 ; i < arr.size() ; i++)
		{
			CString filename(arr[i]);
			if(filename.ReverseFind('\\') == -1)
				filename = cur_dir + arr[i];
			if(!OpenDocument(filename, NULL, true))
				MessageBox("Ошибка. Не удалось открыть файл \"" + filename + "\"!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnProjectOpen()
{
	String folder, cur_dir;
	if(Utils::dlgSaveOrOpenFile(true, "Открыть проект", "Проекты и Системы|*.asmp;*.asms|\0\0", "asmp", folder, m_hWnd, NULL))
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
			MessageBox("Ошибка. Не удалось открыть систему!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnAddDocument()
{
	Array<String> arr;
	String folder, cur_dir;
	CElement* elem(NULL), *elem1(NULL);
	CProject* prj(GetCurrentProject());

	if(Utils::dlgSaveOrOpenFile(true, "Добавить файл", "Ассемблерные файлы|*.asm|Включаемые файлы|*.inc|Все файлы|*.*|\0\0", "asm", folder, NULL, &arr))
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
				// добавить в панель файлов и модулей
				m_wndFileView.TreeAddFile(elem, prj->GetName(), true);
				m_wndModuleView.TreeAddModule(elem, prj->GetName(), true);
				// открыть документ
				if(!OpenDocument(filename, elem, true))
					MessageBox("Ошибка. Не удалось открыть документ \"" + filename + "\"!", ASM_STUDIO, MB_ICONERROR);
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
		// добавить документ в проект
		if((elem = prj->AddElement(folder)))
		{
			// добавить в панель файлов и модулей
			m_wndFileView.TreeAddFile(elem, prj->GetName(), true);
			m_wndModuleView.TreeAddModule(elem, prj->GetName(), true);
			// открыть документ
			if(!OpenDocument(folder, elem, false))
				MessageBox("Ошибка. Не удалось создать файл!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnAddProject()
{
	String folder, cur_dir;
	Array<String> arr;
	CProject* prj;

	if(Utils::dlgSaveOrOpenFile(true, "Добавить проект", "Проекты|*.asmp|\0\0", "asmp", folder, m_hWnd, &arr))
	{
		Utils::getSysDir(NULL, &cur_dir);
		for(size_t i = 0 ; i < arr.size() ; i++)
		{
			CString filename(arr[i]);
			if(filename.ReverseFind('\\') == -1)
				filename = cur_dir + arr[i];
			if((prj = solution.AddProject(NULL, NULL, filename, NULL, ACTION_ADD_OPEN_PROJECT)))
			{
				// добавить его в панель файлов и модулей
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
			// добавить его в панель файлов и модулей
			m_wndFileView.MakeTree(prj);
			m_wndModuleView.MakeTree(prj);
		}
		else
		{
			MessageBox("Ошибка. Не удалось создать проект!", ASM_STUDIO, MB_ICONERROR);
		}
	}
}

void CMainFrame::OnSaveAll()
{
	// 1. сохранить систему
	solution.Save();
	// 2. сохранить все модифицированные документы
	GetAllDocuments(NULL, NULL, NULL, false, false, false);
}

void CMainFrame::OnCloseAll()
{
	if(solution.Close())
	{
		Array<CDocumentCommon*> docs;
		// закрыть все документы
		GetAllDocuments(&docs, NULL, NULL, true, false);
		for(size_t i = 0 ; i < docs.size() ; i++)
			docs[i]->OnCloseDocument();
		// сбросить все панели
		m_wndFileView.Reset();
		m_wndModuleView.Reset();
		m_wndFindView.Reset();
		m_wndOutputView.Reset();
		m_wndTaskView.Reset();
		m_wndArchiveView.Reset();
		// убрать из риббона платформы и конфиги
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
			// в зависимости от выбранной платформы - установить все ее конфигурации и текущую конфигурацию
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
	// определить текущий документ
	CDocument* doc(GetDocument());
	COgxTextEditor* ogx(NULL);
	if(doc->IsKindOf(RUNTIME_CLASS(CDocumentAsmEditor)))
		ogx = ((CDocumentAsmEditor*)doc)->GetOgx();
	else if(doc->IsKindOf(RUNTIME_CLASS(CDocumentTxtEditor)))
		ogx = ((CDocumentTxtEditor*)doc)->GetOgx();
	// взять слово под кареткой
	find_repl_dlg->Show(CFindReplaceDlg::STYLE_FIND_IN_FILES, ogx ? ogx->GetWordFromCaretPosition() : "", FALSE);
}

void CMainFrame::OnReplaceInDocuments()
{
	// определить текущий документ
	CDocument* doc(GetDocument());
	COgxTextEditor* ogx(NULL);
	if(doc->IsKindOf(RUNTIME_CLASS(CDocumentAsmEditor)))
		ogx = ((CDocumentAsmEditor*)doc)->GetOgx();
	else if(doc->IsKindOf(RUNTIME_CLASS(CDocumentTxtEditor)))
		ogx = ((CDocumentTxtEditor*)doc)->GetOgx();
	// взять слово под кареткой
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
	// тоже как и при закрытии проекта - найти все измененные, кроме текущего и выдаь их список
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
	// 1. перебор всех проектов
	while((prj = solution.EnumProjects(index)))
	{
		size_t index_f(0);
		// 2. формирование дерева файлов
		m_wndFileView.MakeTree(prj);
		// 3. формирование дерева модулей
		m_wndModuleView.MakeTree(prj);
		// 4. открытие документов
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
	// 5. установка активного документа
	elem = solution.GetCurrentElement();
	if(!elem) elem = active_elem;
	if(elem)
	{
		OpenDocument(elem->path, elem, true);
	}
	// 6. открыть архив
	if(!OpenArchive(solution.GetArchivePath()))
		solution.SetArchivePath("");
	return true;
}

bool CMainFrame::OpenArchive(const char* path)
{
	/*
	if(!m_wndArchiveView.ArchiveOpen(path, NULL, NULL, true))
	{
		MessageBox("Ошибка. Не удалось открыть архив!", ASM_STUDIO, MB_ICONERROR);
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
	// проверить, если уже открыт - то сделать текущим
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
			// заголовок документа брать из панели файлов
			doc->SetTitle(elem->path_modules);
			if((ogx = doc->GetOgx()))
			{
				// установить список процедур
				doc->FillListProcedures(elem);
				// установить закладки
				for(size_t i = 0 ; i < elem->bookmarks.size() ; i++)
					ogx->SetStatusLine(elem->bookmarks[i].number, ogx->GetStatusLine(elem->bookmarks[i].number) | elem->bookmarks[i].status, FALSE);
				// установить позицию каретки
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
			arr.Add("Система");
	}
	// находим все открытые документы
	// находим, среди них модифицированные и/или относящмеся к конкретному проекту
	// 1.1. исходные
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
	// 1.2. текстовые
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
	// 1.3. бинарные
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
	// 1.3. ресурсы
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
	// 2. выдать диалог подтверждения
	if(arr.GetSize() > 0)
	{
		bool is(true);
		if(is_prompt)
		{
			CShowUnOpenProjectDlg dlg("Сохранение элементов", "Модифицированные:", "Сохранить", &arr);
			is = (dlg.DoModal() == IDOK);
		}
		if(is)
		{
			// 3. сохранить модифицированные
			if(is_sln)
			{
				if(solution.IsModify()) solution.Save(); else solution.MiniSave();
			}
			// 3.1. документы
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
	// установить в риббон все платформы
	for(size_t i = 0 ; i < plts->size() ; i++)
		m_wndPlatform->AddItem(plts->get(i)->GetName());
	m_wndPlatform->SelectItem(solution.GetCurrentPlatform());
	OnPlatforms();
}

CDocumentCommon* CMainFrame::GotoDocument(const CString& pth, long num)
{
	CProject* prj;
	CDocumentCommon* doc;
	// найти элемент
	size_t index(0);
	bool is = false;
	while((prj = solution.EnumProjects(index)))
	{
		CElement* elem;
		if((elem = prj->GetElement(pth, NULL, NULL)))
		{
			// открыть документ
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
		// выполнить переход на требуемую строку
		ogx->GotoLine(num, FALSE);
		// установить фокус на документе
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
		// проверить на расширение
		String ext(Utils::extractFileExtFromFullPath(filename, TRUE));
		if(ext == ".asm" || ext == ".inc" || ext == ".txt")
			OpenDocument(filename, NULL, true);
	}
	::DragFinish(hDropInfo);
}
