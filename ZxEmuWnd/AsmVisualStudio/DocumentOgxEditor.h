
#pragma once

class CDocumentOgxEditor : public CDocument
{
public:
	// ����������
	virtual ~CDocumentOgxEditor() {}
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
protected:
	// �����������
	CDocumentOgxEditor() {}
	DECLARE_DYNCREATE(CDocumentOgxEditor)
	DECLARE_MESSAGE_MAP()
};
