
#pragma once

class CDocumentBinEditor : public CDocument
{
	// деструктор
	virtual ~CDocumentBinEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// вернуть признак модификации
	virtual BOOL IsModified();
	// вызвать форму запроса, если модифицирован
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	// закрытие
	virtual void OnCloseDocument();
	// открытие файла
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	// установка пути(при создании)
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	// сохранение
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	// сериализация
	virtual void Serialize(CArchive& ar);
	// события
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
	// конструктор
	CDocumentBinEditor() {}
	DECLARE_DYNCREATE(CDocumentBinEditor)
	DECLARE_MESSAGE_MAP()
};
