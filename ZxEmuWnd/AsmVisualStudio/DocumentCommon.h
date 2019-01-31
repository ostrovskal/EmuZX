
#pragma once

#include "OgxTextEditor.h"

class CDocumentCommon : public CDocument
{
public:
	// деструктор
	virtual ~CDocumentCommon() {}
	// вернуть признак модификации
	virtual BOOL IsModified();
	// установка признака модификации
	virtual void SetModifiedFlag(BOOL bModified = TRUE);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// установка списка процедур
	virtual void FillListProcedures(CElement* elem) {}
	// вызвать форму запроса, если модифицирован
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	// закрытие
	virtual void OnCloseDocument();
	// сохранение
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	// открытие файла
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	// установка пути(при создании)
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	// сериализация
	virtual void Serialize(CArchive& ar);
	// установка заголовка
	virtual void SetTitle(LPCTSTR lpszTitle);
	// возвращает связанное представление
	CView* GetView();
	// стандартные события
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
	// элемент
	CElement* elem;
protected:
	// конструктор
	CDocumentCommon() : elem(NULL) {}
	// редактор
	COgxTextEditor ogx;
	// массив закладок
	Array<BOOKMARK> bookmarks;
	DECLARE_DYNCREATE(CDocumentCommon)
	DECLARE_MESSAGE_MAP()
};
