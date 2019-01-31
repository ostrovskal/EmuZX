
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
	// ������� ���������
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();
	m_wndPropList.SetBoolLabels("��", "���");
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
			// ������������
			grp = new CMFCPropertyGridProperty("������������");
			// ������������ �� �����
			grp->AddSubItem(new CMFCPropertyGridProperty("�� �����", elem->path, "���������� ���������� ����� �� �����"));
			// ������������ � ������ ������
			grp->AddSubItem(new CMFCPropertyGridProperty("� ������ ������", elem->path_files, "���������� ����� � ������ ������"));
			// ������������ � ������ �������
			grp->AddSubItem(new CMFCPropertyGridProperty("� ������ �������", elem->path_modules, "���������� ����� � ������ �������"));
			m_wndPropList.AddProperty(grp);
			// ������
			grp = new CMFCPropertyGridProperty("������");
			// ������� ���������� �� ������
			is = (elem->is_exclude != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("�������� �� ������", (variant_t)is, "������� ���������� ����� �� �������� ������ �������/�������"));
			// ������� ��������
			is = (elem->is_open != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("������ � ���������", (variant_t)is, "������� ����, ��� ���� ������ �� �������������� � ����"));
			// ���
			grp->AddSubItem(new CMFCPropertyGridProperty("���", elem->path.Mid(elem->path.ReverseFind('.') + 1), "��� �����"));
			m_wndPropList.AddProperty(grp);
			break;
		case IT_MODULE:
			// ���
			m_wndPropList.AddProperty(new CMFCPropertyGridProperty("���", elem->path_modules.Mid(elem->path_modules.ReverseFind('\\') + 1), "��� ������"));
			// ������������
			grp = new CMFCPropertyGridProperty("������������");
			// ������������ �� �����
			grp->AddSubItem(new CMFCPropertyGridProperty("�� �����", elem->path, "���������� ���������� ����� �� �����"));
			// ������������ � ������ ������
			grp->AddSubItem(new CMFCPropertyGridProperty("� ������ ������", elem->path_files, "���������� ����� � ������ ������"));
			// ������������ � ������ �������
			grp->AddSubItem(new CMFCPropertyGridProperty("� ������ �������", elem->path_modules, "���������� ����� � ������ �������"));
			m_wndPropList.AddProperty(grp);
			// ������
			grp = new CMFCPropertyGridProperty("������");
			// ������� ���������� �� ������
			is = (elem->is_exclude != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("�������� �� ������", (variant_t)is, "������� ���������� ����� �� �������� ������ �������/�������"));
			// ������� ��������
			is = (elem->is_open != 0);
			grp->AddSubItem(new CMFCPropertyGridProperty("������ � ���������", (variant_t)is, "������� ����, ��� ���� ������ �� �������������� � ����"));
			// ���������� ���������
			grp->AddSubItem(new CMFCPropertyGridProperty("���������� ���������", (variant_t)elem->block_elems.size(), "���������� ��������� ������ ��������"));
			m_wndPropList.AddProperty(grp);
			break;
		case IT_PROCEDURE:
			if(blk)
			{
				// �������
				grp = new CMFCPropertyGridProperty("�������");
				// ���
				grp->AddSubItem(new CMFCPropertyGridProperty("���������", blk->name, "��� ��������� � ������"));
				// ��� ������
				grp->AddSubItem(new CMFCPropertyGridProperty("������", elem->path_modules, "������, � ������� ��������� ���������"));
				// ��� �����
				grp->AddSubItem(new CMFCPropertyGridProperty("����", elem->path, "����, � ������� ��������� ���������"));
				m_wndPropList.AddProperty(grp);
				// ������
				grp = new CMFCPropertyGridProperty("������");
				// �������
				grp->AddSubItem(new CMFCPropertyGridProperty("������� � ������", (variant_t)blk->line_begin, "������� ��������� � ������"));
				// ��� �������
				grp->AddSubItem(new CMFCPropertyGridProperty("��� �������", (variant_t)blk->value, "��� ������� � ���������"));
				// ����� �����
				grp->AddSubItem(new CMFCPropertyGridProperty("���������� �����", (variant_t)(blk->line_end - blk->line_begin), "������ ���������� ����� � ���������"));
				m_wndPropList.AddProperty(grp);
			}
			break;
		case IT_STRUCTURE:
			if(blk)
			{
				// �������
				grp = new CMFCPropertyGridProperty("�������");
				// ���
				grp->AddSubItem(new CMFCPropertyGridProperty("���������", blk->name, "��� ��������� � ������"));
				// ��� ������
				grp->AddSubItem(new CMFCPropertyGridProperty("������", elem->path_modules, "������, � ������� ��������� ���������"));
				// ��� �����
				grp->AddSubItem(new CMFCPropertyGridProperty("����", elem->path, "����, � ������� ��������� ���������"));
				m_wndPropList.AddProperty(grp);
				// ������
				grp = new CMFCPropertyGridProperty("������");
				// �������
				grp->AddSubItem(new CMFCPropertyGridProperty("������� � ������", (variant_t)blk->line_begin, "������� ��������� � ������"));
				// ����� �����
				grp->AddSubItem(new CMFCPropertyGridProperty("���������� �����", (variant_t)(blk->line_end - blk->line_begin), "������ ���������� ����� � ���������"));
				m_wndPropList.AddProperty(grp);
			}
			break;
		case IT_EQU:
			if(blk)
			{
				// �������
				grp = new CMFCPropertyGridProperty("�������");
				// ���
				grp->AddSubItem(new CMFCPropertyGridProperty("�����������", blk->name, "��� ����������� � ������"));
				// ��� ������
				grp->AddSubItem(new CMFCPropertyGridProperty("������", elem->path_modules, "������, � ������� ��������� �����������"));
				// ��� �����
				grp->AddSubItem(new CMFCPropertyGridProperty("����", elem->path, "����, � ������� ��������� �����������"));
				m_wndPropList.AddProperty(grp);
				// ������
				grp = new CMFCPropertyGridProperty("������");
				// �������
				grp->AddSubItem(new CMFCPropertyGridProperty("������� � ������", (variant_t)blk->line_begin, "������� ����������� � ������"));
				// ��� �������
				grp->AddSubItem(new CMFCPropertyGridProperty("��������", (variant_t)blk->value, "�������� �����������"));
				m_wndPropList.AddProperty(grp);
			}
			break;
		case IT_CODE_EDITOR:
			// ��� ���. �������� � ������(���, ����, ����, ������������) ��� �������
			break;
		case IT_TEXT_EDITOR:
			// ��� ���. ��������
			break;
		case IT_BIN_EDITOR:
			// ��� ���. ��������
			break;
		case IT_RESOURCE:
			{
				ostd::CommonResource* r((CommonResource*)stk);
				CString nm(r->name());
				CPanelArchiveView::ACTIVEX* x(theApp.frm->GetPanelArchive()->GetResourceModule(nm.Mid(nm.ReverseFind('.') + 1)));
				// �������
				grp = new CMFCPropertyGridProperty("����������");
				// ���� �� �����
				grp->AddSubItem(new CMFCPropertyGridProperty("�� �����", r->getPath(), "���������� ���������� ������� �� �����"));
				// ���� � ������
				grp->AddSubItem(new CMFCPropertyGridProperty("� ������", nm, "���������� ������� � �������� ������� ������"));
				m_wndPropList.AddProperty(grp);
				// ������
				grp = new CMFCPropertyGridProperty("������");
				// ���
				grp->AddSubItem(new CMFCPropertyGridProperty("���", (variant_t)(x ? x->name : ""), "��� �������"));
				// ��������
				grp->AddSubItem(new CMFCPropertyGridProperty("��������", (variant_t)(x ? x->desc : ""), "�������� �������"));
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
