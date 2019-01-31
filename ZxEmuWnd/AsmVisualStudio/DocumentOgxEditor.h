
#pragma once

class CDocumentOgxEditor : public CDocument
{
public:
	// деструктор
	virtual ~CDocumentOgxEditor() {}
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
protected:
	// конструктор
	CDocumentOgxEditor() {}
	DECLARE_DYNCREATE(CDocumentOgxEditor)
	DECLARE_MESSAGE_MAP()
};
