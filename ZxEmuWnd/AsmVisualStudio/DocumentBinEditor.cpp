
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentBinEditor.h"
#include "GotoLineDlg.h"
#include "FindReplaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDocumentBinEditor, CDocument)
BEGIN_MESSAGE_MAP(CDocumentBinEditor, CDocument)
	ON_COMMAND(ID_COPY, &CDocumentBinEditor::OnCopy)
	ON_COMMAND(ID_CUT, &CDocumentBinEditor::OnCut)
	ON_COMMAND(ID_PASTE, &CDocumentBinEditor::OnPaste)
	ON_COMMAND(ID_DOCUMENT_SAVE, &CDocumentBinEditor::OnDocumentSave)
	ON_COMMAND(ID_DOCUMENT_SAVE_AS, &CDocumentBinEditor::OnDocumentSaveAs)
	ON_COMMAND(ID_FIND, &CDocumentBinEditor::OnFind)
	ON_COMMAND(ID_REPLACE, &CDocumentBinEditor::OnReplace)
	ON_UPDATE_COMMAND_UI(ID_COPY, &CDocumentBinEditor::OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_CUT, &CDocumentBinEditor::OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_PASTE, &CDocumentBinEditor::OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_DOCUMENT_SAVE, &CDocumentBinEditor::OnUpdateDocumentSave)
END_MESSAGE_MAP()

void CDocumentBinEditor::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

#ifdef _DEBUG
void CDocumentBinEditor::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocumentBinEditor::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CDocumentBinEditor::IsModified()
{
	return FALSE;
}

void CDocumentBinEditor::SetPathName(LPCTSTR path, BOOL bAddToMRU)
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

BOOL CDocumentBinEditor::OnOpenDocument(LPCTSTR path)
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

BOOL CDocumentBinEditor::CanCloseFrame(CFrameWnd* pFrame)
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

void CDocumentBinEditor::OnCloseDocument()
{
	if(CanCloseFrame(NULL))
		CDocument::OnCloseDocument();
}

BOOL CDocumentBinEditor::OnSaveDocument(LPCTSTR pth)
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

void CDocumentBinEditor::OnCopy()
{

}

void CDocumentBinEditor::OnCut()
{

}

void CDocumentBinEditor::OnPaste()
{

}

void CDocumentBinEditor::OnDocumentSave()
{
	if(!OnSaveDocument(NULL))
		MessageBox(NULL, "Ошибка. Не удалось сохранить документ!", ASM_STUDIO, MB_ICONERROR);
}

void CDocumentBinEditor::OnDocumentSaveAs()
{
	String folder;
	if(Utils::dlgSaveOrOpenFile(false, "Сохранить как...", "Ассемблерные файлы|*.asm|Включаемые файлы|*.inc|Все файлы|*.*|\0\0", "asm", folder, NULL, NULL))
	{
		if(!OnSaveDocument(folder))
			MessageBox(NULL, "Ошибка. Не удалось сохранить документ!", ASM_STUDIO, MB_ICONERROR);
	}
}

void CDocumentBinEditor::OnFind()
{
	CFindReplaceDlg* dlg(theApp.frm->GetFindReplaceDlg());
	dlg->Show(CFindReplaceDlg::STYLE_FIND, /*ogx.GetWordFromCaretPosition()*/"", FALSE);
}

void CDocumentBinEditor::OnReplace()
{
	CFindReplaceDlg* dlg(theApp.frm->GetFindReplaceDlg());
	dlg->Show(CFindReplaceDlg::STYLE_REPLACE, /*ogx.GetWordFromCaretPosition()*/"", FALSE);
}

void CDocumentBinEditor::OnUpdateCopy(CCmdUI* pCmdUI)
{

}

void CDocumentBinEditor::OnUpdateCut(CCmdUI* pCmdUI)
{

}

void CDocumentBinEditor::OnUpdatePaste(CCmdUI* pCmdUI)
{

}

void CDocumentBinEditor::OnUpdateDocumentSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsModified());
}
