
#pragma once

#define CE_WIDTH_TREE			11

class CViewEditorComboBoxButton : public CMFCToolBarComboBoxButton
{
	friend class CViewAsmEditor;
	DECLARE_SERIAL(CViewEditorComboBoxButton)
public:
	CViewEditorComboBoxButton() : CMFCToolBarComboBoxButton(ID_COMBOBOX_GOTO_PROCEDURE, -1, CBS_DROPDOWNLIST, 300) {}
};

class CViewAsmEditor : public CView
{
public:
	class CToolBarEditor : public CMFCToolBar
	{
		virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
		{
			CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
		}
		virtual BOOL AllowShowOnList() const {return FALSE;}
	};
	CDocumentAsmEditor* GetDocument() const;
	virtual void OnDraw(CDC* pDC);
	virtual ~CViewAsmEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCollapseAll();
	afx_msg void OnExpandAll();
	afx_msg void OnComboboxGotoProcedure();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	void AdjustLayout();
	void TextChange(long y1, long y2, long action);
	CViewEditorComboBoxButton* m_ComboProc;
protected:
	struct NODE_ELEMENT
	{
		NODE_ELEMENT() {collapse = true;}
		NODE_ELEMENT(int ln, const CRect& rc, bool use) : rect(rc), collapse(use), line(ln) {}
		CRect rect;
		long line;
		bool collapse;
	};
	// �����������
	CViewAsmEditor() : 	colorBk(NULL), colorLines(NULL), cyTlb(0), hline(NULL), colorBookmark(NULL), colorBkBookmark(NULL), nWidthNumLines(32), ogx(NULL), doc(NULL), count_nums(0), h_client(0)
	{
		memset(num_lines, 0, sizeof(num_lines));
		memset(num_statuses, 0, sizeof(num_statuses));
	}
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	// �������� OGX
	COgxTextEditor* ogx;
	CDocumentAsmEditor* doc;
	CToolBarEditor m_ToolBar;
	CFont m_font;
	int cyTlb;
	// ������ �����
	int* hline;
	// ������ �������
	int h_client;
	// ������ ����������� ������� �����
	int nWidthNumLines;
	// ������ ������� �����
	long num_lines[256];
	// ������� ������� �����
	long num_statuses[256];
	// ���������� ������� �����
	long count_nums;
	// ����� ������� �����
	CFont fontNumLines;
	// ����� �������� ��������
	CFont fontBookmarks;
	// ���� ������ ��������
	COLORREF* colorBookmark;
	// ���� ���� ��������
	COLORREF* colorBkBookmark;
	// ���� ����
	COLORREF* colorBk;
	// ���� ������� �����
	COLORREF* colorLines;
	// ������ �������� �����
	CArray<NODE_ELEMENT> nodes;
	// ������� �������
	CRect rectClient;
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CViewAsmEditor)
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
};

#ifndef _DEBUG
inline CDocumentAsmEditor* CViewAsmEditor::GetDocument() const {return reinterpret_cast<CDocumentAsmEditor*>(m_pDocument);}
#endif

