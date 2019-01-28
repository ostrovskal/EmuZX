
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentCommon.h"
#include "GotoLineDlg.h"
#include "FindReplaceDlg.h"

IMPLEMENT_DYNCREATE(CDocumentCommon, CDocument)

BEGIN_MESSAGE_MAP(CDocumentCommon, CDocument)
	ON_COMMAND(ID_UNDO, &CDocumentCommon::OnUndo)
	ON_COMMAND(ID_REDO, &CDocumentCommon::OnRedo)
	ON_COMMAND(ID_COPY, &CDocumentCommon::OnCopy)
	ON_COMMAND(ID_CUT, &CDocumentCommon::OnCut)
	ON_COMMAND(ID_PASTE, &CDocumentCommon::OnPaste)
	ON_COMMAND(ID_SELECT_ALL, &CDocumentCommon::OnSelectAll)
	ON_COMMAND(ID_GOTO_LOCATION, &CDocumentCommon::OnGotoLocation)
	ON_COMMAND(ID_DOCUMENT_SAVE, &CDocumentCommon::OnDocumentSave)
	ON_COMMAND(ID_DOCUMENT_SAVE_AS, &CDocumentCommon::OnDocumentSaveAs)
	ON_COMMAND(ID_FIND, &CDocumentCommon::OnFind)
	ON_COMMAND(ID_REPLACE, &CDocumentCommon::OnReplace)
	ON_COMMAND(ID_DOCUMENT_CLOSE, &CDocumentCommon::OnCloseDocument)
	ON_UPDATE_COMMAND_UI(ID_UNDO, &CDocumentCommon::OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_REDO, &CDocumentCommon::OnUpdateRedo)
	ON_UPDATE_COMMAND_UI(ID_COPY, &CDocumentCommon::OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_CUT, &CDocumentCommon::OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_PASTE, &CDocumentCommon::OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_DOCUMENT_SAVE, &CDocumentCommon::OnUpdateDocumentSave)
END_MESSAGE_MAP()

CView* CDocumentCommon::GetView()
{
	POSITION pos(GetFirstViewPosition());
	return (pos ? (CView*)GetNextView(pos) : NULL);
}

#ifdef _DEBUG
void CDocumentCommon::AssertValid() const
{
	CDocument::AssertValid();
}

void CDocumentCommon::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CDocumentCommon::Serialize(CArchive& ar)
{
	if(ar.IsStoring()) {} else {}
}

void CDocumentCommon::SetPathName(LPCTSTR path, BOOL bAddToMRU)
{
	if(m_strPathName == "")
	{
		CFile f;
		// проверить файл с таким именем уже существует?
		if(f.Open(path, CFile::modeRead))
		{
			f.Close();
			if(MessageBox(NULL, "Предупреждение. Файл \"" + CString(path) + "\" уже существует. Перезаписать?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDNO)
				return;
		}
		// создаем файл
		f.Open(path, CFile::modeCreate);
		f.Close();
	}
	CDocument::SetPathName(path, bAddToMRU);
}

BOOL CDocumentCommon::OnOpenDocument(LPCTSTR path)
{
	CFile f;
	CString txt;
	// открываем с диска
	if(!f.Open(path, CFile::modeRead))
		return FALSE;
	long length((long)f.GetLength());
	f.Read(txt.GetBuffer(length), length);
	txt.ReleaseBuffer();
	m_strPathName = path;
	ogx.SetText(txt, SSL_VIS);
	return TRUE;
}

BOOL CDocumentCommon::CanCloseFrame(CFrameWnd* pFrame)
{
	if(pFrame == NULL && IsModified())
	{
		switch(MessageBox(NULL, "Документ изменен. Сохранить изменения ?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNOCANCEL))
		{
			case IDYES:
				OnSaveDocument(NULL);
				break;
			case IDCANCEL:
				return FALSE;
		}
		SetModifiedFlag(FALSE);
	}
	return TRUE;
}

void CDocumentCommon::OnCloseDocument()
{
	if(CanCloseFrame(NULL))
	{
		if(elem)
		{
			elem->doc = NULL;
			elem->is_open = false;
			// если фэйковый - то стирать из проекта
			if(elem->is_fake)
				elem->prj->DeleteElement(elem->path);
			ogx.GetCaretPosition(&elem->x, &elem->y);
		}
		CDocument::OnCloseDocument();
	}
}

BOOL CDocumentCommon::OnSaveDocument(LPCTSTR pth)
{
	CFile f;
	if(pth)
	{
		// save as...
		m_strPathName = pth;
	}
	if(f.Open(m_strPathName, CFile::modeCreate | CFile::modeWrite))
	{
		CString txt(ogx.GetText());
		f.Write((void*)(const char*)txt, txt.GetLength());
		f.Close();
		SetModifiedFlag(FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CDocumentCommon::IsModified()
{
	BOOL is(ogx.m_hWnd && ogx.IsTextModify());
	if(is != m_bModified)
		SetModifiedFlag(is);
	return is;
}

void CDocumentCommon::SetModifiedFlag(BOOL bModified)
{
	// установить или убрать признак модификации
	CString titl(GetTitle());
	int pos(titl.ReverseFind('*'));
	CDocument::SetTitle(bModified ? titl + "*" : (pos == -1 ? titl : titl.Left(pos)));
	ogx.SetModifyText(bModified);
	CDocument::SetModifiedFlag(bModified);
}

void CDocumentCommon::OnUndo()
{
	ogx.TextUndo();
}

void CDocumentCommon::OnRedo()
{
	ogx.TextRedo();
}

void CDocumentCommon::OnCopy()
{
	ogx.TextCopy();
}

void CDocumentCommon::OnCut()
{
	ogx.TextCut();
}

void CDocumentCommon::OnPaste()
{
	ogx.TextPaste();
}

void CDocumentCommon::OnSelectAll()
{
	ogx.SelectAll();
}

void CDocumentCommon::OnGotoLocation()
{
	CGotoLineDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		ogx.GotoLine(dlg.line, FALSE);
		ogx.UpdateAll(0);
	}
}

void CDocumentCommon::OnDocumentSave()
{
	if(!OnSaveDocument(NULL))
		MessageBox(NULL, "Ошибка. Не удалось сохранить документ!", ASM_STUDIO, MB_ICONERROR);
}

void CDocumentCommon::OnDocumentSaveAs()
{
	String folder;
	if(Utils::dlgSaveOrOpenFile(false, "Сохранить как...", "Ассемблерные файлы|*.asm|Включаемые файлы|*.inc|Все файлы|*.*|\0\0", "asm", folder, NULL, NULL))
	{
		if(!OnSaveDocument(folder))
			MessageBox(NULL, "Ошибка. Не удалось сохранить документ!", ASM_STUDIO, MB_ICONERROR);
	}
}

void CDocumentCommon::OnFind()
{
	CFindReplaceDlg* dlg(theApp.frm->GetFindReplaceDlg());
	dlg->Show(CFindReplaceDlg::STYLE_FIND, ogx.GetWordFromCaretPosition(), ogx.IsTextCopy());
}

void CDocumentCommon::OnReplace()
{
	CFindReplaceDlg* dlg(theApp.frm->GetFindReplaceDlg());
	dlg->Show(CFindReplaceDlg::STYLE_REPLACE, ogx.GetWordFromCaretPosition(), ogx.IsTextCopy());
}

void CDocumentCommon::OnUpdateUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ogx.IsTextUndo());
}

void CDocumentCommon::OnUpdateRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ogx.IsTextRedo());
}

void CDocumentCommon::OnUpdateCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ogx.IsTextCopy());
}

void CDocumentCommon::OnUpdateCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ogx.IsTextCut());
}

void CDocumentCommon::OnUpdatePaste(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(ogx.IsTextPaste());
}

void CDocumentCommon::OnUpdateDocumentSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsModified());
}

void CDocumentCommon::SetTitle(LPCTSTR lpszTitle)
{
	CString titl(lpszTitle);
	CDocument::SetTitle(titl.Mid(titl.ReverseFind('\\') + 1));
}
