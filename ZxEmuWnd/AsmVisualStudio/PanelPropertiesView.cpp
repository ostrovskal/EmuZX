
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "PanelPropertiesView.h"
#include "Project.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CPanelPropertiesView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void CPanelPropertiesView::AdjustLayout()
{
	if(GetSafeHwnd())
	{
		CRect rectClient;
		GetClientRect(rectClient);
		m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

int CPanelPropertiesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	if(!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, 1))
		return -1;
	// базовые параметры
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	m_wndPropList.SetBoolLabels("Да", "Нет");
	AdjustLayout();
	UpdateAll(IT_DEFAULT, NULL, NULL, false);
	return 0;
}

void CPanelPropertiesView::UpdateAll(int type, void* stk, void* sub_stk, bool is_show)
{
	CMFCPropertyGridProperty* grp;
	CElement* elem((CElement*)stk);
	CElement::BLOCK_ELEMENT* blk((CElement::BLOCK_ELEMENT*)sub_stk);
	bool is;

	if(stk == NULL)
		type = IT_DEFAULT;
	nType = type;

	m_wndPropList.RemoveAll();

	switch(type)
	{
		case IT_DEFAULT:
			break;
		case IT_FILE:
			// расположение
			grp = new CMFCPropertyGridProperty("Расположение");
			// расположение на диске
			grp->AddSubItem(new CMFCPropertyGridProperty("На диске", elem->path, "Физическое размещение файла на диске"));
			// расположение в панели файлов
			grp->AddSubItem(new CMFCPropertyGridProperty("В панели файлов", elem->path_files, "Размещение файла в панели файлов"));
			// расположение в панели модулей
			grp->AddSubItem(new CMFCPropertyGridProperty("В панели модулей", elem->path_modules, "Размещение файла в панели модулей"));
			m_wndPropList.AddProperty(grp);
			// Разное
			grp = new CMFCPropertyGridProperty("Разное");
			// признак исключения из сборки
			is = (elem->is_exclude != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("Исключен из сборки", (variant_t)is, "Признак исключения файла из процесса сборки проекта/системы"));
			// признак открытия
			is = (elem->is_open != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("Открыт в редакторе", (variant_t)is, "Признак того, что файл открыт на редактирование в окне"));
			// тип
			grp->AddSubItem(new CMFCPropertyGridProperty("Тип", elem->path.Mid(elem->path.ReverseFind('.') + 1), "Тип файла"));
			m_wndPropList.AddProperty(grp);
			break;
		case IT_MODULE:
			// имя
			m_wndPropList.AddProperty(new CMFCPropertyGridProperty("Имя", elem->path_modules.Mid(elem->path_modules.ReverseFind('\\') + 1), "Имя модуля"));
			// расположение
			grp = new CMFCPropertyGridProperty("Расположение");
			// расположение на диске
			grp->AddSubItem(new CMFCPropertyGridProperty("На диске", elem->path, "Физическое размещение файла на диске"));
			// расположение в панели файлов
			grp->AddSubItem(new CMFCPropertyGridProperty("В панели файлов", elem->path_files, "Размещение файла в панели файлов"));
			// расположение в панели модулей
			grp->AddSubItem(new CMFCPropertyGridProperty("В панели модулей", elem->path_modules, "Размещение файла в панели модулей"));
			m_wndPropList.AddProperty(grp);
			// Разное
			grp = new CMFCPropertyGridProperty("Разное");
			// признак исключения из сборки
			is = (elem->is_exclude != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("Исключен из сборки", (variant_t)is, "Признак исключения файла из процесса сборки проекта/системы"));
			// признак открытия
			is = (elem->is_open != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("Открыт в редакторе", (variant_t)is, "Признак того, что файл открыт на редактирование в окне"));
			// количество элементов
			grp->AddSubItem(new CMFCPropertyGridProperty("Количество элементов", (variant_t)elem->block_elems.size(), "Количество вложенных блоков элеметов"));
			m_wndPropList.AddProperty(grp);
			break;
		case IT_PROCEDURE:
			if(blk)
			{
				// базовые
				grp = new CMFCPropertyGridProperty("Базовые");
				// имя
				grp->AddSubItem(new CMFCPropertyGridProperty("Процедура", blk->name, "Имя процедуры в модуле"));
				// имя модуля
				grp->AddSubItem(new CMFCPropertyGridProperty("Модуль", elem->path_modules, "Модуль, в котором размещена процедура"));
				// имя файла
				grp->AddSubItem(new CMFCPropertyGridProperty("Файл", elem->path, "Файл, в котором размещена процедура"));
				m_wndPropList.AddProperty(grp);
				// Разное
				grp = new CMFCPropertyGridProperty("Разное");
				// позиция
				grp->AddSubItem(new CMFCPropertyGridProperty("Позиция в тексте", (variant_t)blk->line_begin, "Позиция процедуры в тексте"));
				// тип доступа
				grp->AddSubItem(new CMFCPropertyGridProperty("Тип доступа", (variant_t)blk->value, "Тип доступа к процедуре"));
				// всего строк
				grp->AddSubItem(new CMFCPropertyGridProperty("Количетсво строк", (variant_t)(blk->line_end - blk->line_begin), "Полное количество строк в процедуре"));
				m_wndPropList.AddProperty(grp);
			}
			break;
		case IT_STRUCTURE:
			if(blk)
			{
				// базовые
				grp = new CMFCPropertyGridProperty("Базовые");
				// имя
				grp->AddSubItem(new CMFCPropertyGridProperty("Структура", blk->name, "Имя структуры в модуле"));
				// имя модуля
				grp->AddSubItem(new CMFCPropertyGridProperty("Модуль", elem->path_modules, "Модуль, в котором размещена структура"));
				// имя файла
				grp->AddSubItem(new CMFCPropertyGridProperty("Файл", elem->path, "Файл, в котором размещена структура"));
				m_wndPropList.AddProperty(grp);
				// Разное
				grp = new CMFCPropertyGridProperty("Разное");
				// позиция
				grp->AddSubItem(new CMFCPropertyGridProperty("Позиция в тексте", (variant_t)blk->line_begin, "Позиция структуры в тексте"));
				// всего строк
				grp->AddSubItem(new CMFCPropertyGridProperty("Количетсво строк", (variant_t)(blk->line_end - blk->line_begin), "Полное количество строк в структуре"));
				m_wndPropList.AddProperty(grp);
			}
			break;
		case IT_EQU:
			if(blk)
			{
				// базовые
				grp = new CMFCPropertyGridProperty("Базовые");
				// имя
				grp->AddSubItem(new CMFCPropertyGridProperty("Определение", blk->name, "Имя определения в модуле"));
				// имя модуля
				grp->AddSubItem(new CMFCPropertyGridProperty("Модуль", elem->path_modules, "Модуль, в котором размещено определение"));
				// имя файла
				grp->AddSubItem(new CMFCPropertyGridProperty("Файл", elem->path, "Файл, в котором размещено определение"));
				m_wndPropList.AddProperty(grp);
				// Разное
				grp = new CMFCPropertyGridProperty("Разное");
				// позиция
				grp->AddSubItem(new CMFCPropertyGridProperty("Позиция в тексте", (variant_t)blk->line_begin, "Позиция определения в тексте"));
				// тип доступа
				grp->AddSubItem(new CMFCPropertyGridProperty("Значение", (variant_t)blk->value, "Значение определения"));
				m_wndPropList.AddProperty(grp);
			}
			break;
		case IT_CODE_EDITOR:
			// все стд. элементы и группы(фон, цвет, болд, подчеркнутый) для каждого
			break;
		case IT_TEXT_EDITOR:
			// все стд. элементы
			break;
		case IT_BIN_EDITOR:
			// все стд. элементы
			break;
		case IT_RESOURCE:
			{
				ostd::CommonResource* r((CommonResource*)stk);
				CString nm(r->name());
				CPanelArchiveView::ACTIVEX* x(theApp.frm->GetPanelArchive()->GetResourceModule(nm.Mid(nm.ReverseFind('.') + 1)));
				// базовые
				grp = new CMFCPropertyGridProperty("Размещение");
				// путь на диске
				grp->AddSubItem(new CMFCPropertyGridProperty("На диске", r->getPath(), "Физическое размещение ресурса на диске"));
				// путь в архиве
				grp->AddSubItem(new CMFCPropertyGridProperty("В архиве", nm, "Размещение ресурса в файловой системе архива"));
				m_wndPropList.AddProperty(grp);
				// Разное
				grp = new CMFCPropertyGridProperty("Разное");
				// тип
				grp->AddSubItem(new CMFCPropertyGridProperty("Тип", (variant_t)(x ? x->name : ""), "Тип ресурса"));
				// описание
				grp->AddSubItem(new CMFCPropertyGridProperty("Описание", (variant_t)(x ? x->desc : ""), "Описание ресурса"));
				m_wndPropList.AddProperty(grp);
			}
			break;
	}
	m_wndPropList.ExpandAll();
	if(is_show)
	{
		theApp.frm->ShowPane(this, TRUE, TRUE, TRUE);
	}
}

void CPanelPropertiesView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPanelPropertiesView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}
