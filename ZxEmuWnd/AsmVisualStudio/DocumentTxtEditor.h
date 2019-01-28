
#pragma once

#include "DocumentCommon.h"

class CDocumentTxtEditor : public CDocumentCommon
{
public:
	// деструктор
	virtual ~CDocumentTxtEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// сериализация
	virtual void Serialize(CArchive& ar);
	// события
protected:
	// конструктор
	CDocumentTxtEditor() {}
	DECLARE_DYNCREATE(CDocumentTxtEditor)
	DECLARE_MESSAGE_MAP()
};
