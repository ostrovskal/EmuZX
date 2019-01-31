
#pragma once

class CDocumentBinEditor : public CDocument
{
	// ����������
	virtual ~CDocumentBinEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// ������� ������� �����������
	virtual BOOL IsModified();
	// ������� ����� �������, ���� �������������
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	// ��������
	virtual void OnCloseDocument();
	// �������� �����
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	// ��������� ����(��� ��������)
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	// ����������
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	// ������������
	virtual void Serialize(CArchive& ar);
	// �������
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnPaste();
	afx_msg void OnDocumentSave();
	afx_msg void OnDocumentSaveAs();
	afx_msg void OnFind();
	afx_msg void OnReplace();
	afx_msg void OnUpdateCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDocumentSave(CCmdUI* pCmdUI);
protected:
	// �����������
	CDocumentBinEditor() {}
	DECLARE_DYNCREATE(CDocumentBinEditor)
	DECLARE_MESSAGE_MAP()
};
