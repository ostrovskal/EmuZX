
#pragma once

#include "OgxTextEditor.h"

class CDocumentCommon : public CDocument
{
public:
	// ����������
	virtual ~CDocumentCommon() {}
	// ������� ������� �����������
	virtual BOOL IsModified();
	// ��������� �������� �����������
	virtual void SetModifiedFlag(BOOL bModified = TRUE);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// ��������� ������ ��������
	virtual void FillListProcedures(CElement* elem) {}
	// ������� ����� �������, ���� �������������
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	// ��������
	virtual void OnCloseDocument();
	// ����������
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	// �������� �����
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	// ��������� ����(��� ��������)
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	// ������������
	virtual void Serialize(CArchive& ar);
	// ��������� ���������
	virtual void SetTitle(LPCTSTR lpszTitle);
	// ���������� ��������� �������������
	CView* GetView();
	// ����������� �������
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnPaste();
	afx_msg void OnSelectAll();
	afx_msg void OnGotoLocation();
	afx_msg void OnDocumentSave();
	afx_msg void OnDocumentSaveAs();
	afx_msg void OnFind();
	afx_msg void OnReplace();
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSave(CCmdUI* pCmdUI);
	COgxTextEditor* GetOgx() {return &ogx;}
	// �������
	CElement* elem;
protected:
	// �����������
	CDocumentCommon() : elem(NULL) {}
	// ��������
	COgxTextEditor ogx;
	// ������ ��������
	Array<BOOKMARK> bookmarks;
	DECLARE_DYNCREATE(CDocumentCommon)
	DECLARE_MESSAGE_MAP()
};
