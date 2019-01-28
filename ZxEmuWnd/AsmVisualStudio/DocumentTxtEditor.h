
#pragma once

#include "DocumentCommon.h"

class CDocumentTxtEditor : public CDocumentCommon
{
public:
	// ����������
	virtual ~CDocumentTxtEditor() {}
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// ������������
	virtual void Serialize(CArchive& ar);
	// �������
protected:
	// �����������
	CDocumentTxtEditor() {}
	DECLARE_DYNCREATE(CDocumentTxtEditor)
	DECLARE_MESSAGE_MAP()
};
