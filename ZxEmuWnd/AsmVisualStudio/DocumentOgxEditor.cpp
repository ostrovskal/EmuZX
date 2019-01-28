
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentOgxEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDocumentOgxEditor, CDocument)
BEGIN_MESSAGE_MAP(CDocumentOgxEditor, CDocument)
END_MESSAGE_MAP()

void CDocumentOgxEditor::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

#ifdef _DEBUG
void CDocumentOgxEditor::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocumentOgxEditor::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CDocumentOgxEditor::IsModified()
{
	return FALSE;
}

void CDocumentOgxEditor::SetPathName(LPCTSTR path, BOOL bAddToMRU)
{
	if(m_strPathName == "")
	{
		CFile f;
		// проверить файл с таким именем уже существует?
		if(f.Open(path, CFile::modeRead))
		{
			f.Close();
			if(MessageBox(NULL, "Предупреждение. Файл \"" + CString(path) + "\" уже существует. Перезаписать?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDYES)
			{
				// создаем файл
				f.Open(path, CFile::modeCreate | CFile::modeWrite);
				f.Close();
			}
		}
	}
	CDocument::SetPathName(path, bAddToMRU);
}

BOOL CDocumentOgxEditor::OnOpenDocument(LPCTSTR path)
{
	CFile f;
	CString txt;

	if(!CDocument::OnOpenDocument(path))
		return FALSE;
	// открываем с диска
	if(!f.Open(path, CFile::modeRead))
		return FALSE;
	long length((long)f.GetLength() + 1);
	f.Read(txt.GetBuffer(length), length);
	txt.ReleaseBuffer();
	m_strPathName = path;

	return TRUE;
}

BOOL CDocumentOgxEditor::CanCloseFrame(CFrameWnd* pFrame)
{
	if(pFrame && IsModified())
	{
		switch(MessageBox(NULL, "Документ изменен. Сохранить изменения ?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNOCANCEL))
		{
			case IDYES:
				OnSaveDocument(NULL);
				break;
			case IDCANCEL:
				return FALSE;
		}
	}
	return TRUE;
}

void CDocumentOgxEditor::OnCloseDocument()
{
	if(CanCloseFrame(NULL))
		CDocument::OnCloseDocument();
}

BOOL CDocumentOgxEditor::OnSaveDocument(LPCTSTR pth)
{
	CFile f;
	if(pth)
	{
		// save as...
		m_strPathName = pth;
	}
	if(f.Open(m_strPathName, CFile::modeCreate | CFile::modeWrite))
	{
		SetModifiedFlag(FALSE);
		return TRUE;
	}
	return FALSE;
}
