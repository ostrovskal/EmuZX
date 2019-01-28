
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "DocumentTxtEditor.h"
#include "GotoLineDlg.h"
#include "FindReplaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CDocumentTxtEditor, CDocumentCommon)
BEGIN_MESSAGE_MAP(CDocumentTxtEditor, CDocumentCommon)
	ON_COMMAND(ID_UNDO, &CDocumentTxtEditor::OnUndo)
	ON_COMMAND(ID_REDO, &CDocumentTxtEditor::OnRedo)
	ON_COMMAND(ID_COPY, &CDocumentTxtEditor::OnCopy)
	ON_COMMAND(ID_CUT, &CDocumentTxtEditor::OnCut)
	ON_COMMAND(ID_PASTE, &CDocumentTxtEditor::OnPaste)
	ON_COMMAND(ID_SELECT_ALL, &CDocumentTxtEditor::OnSelectAll)
	ON_COMMAND(ID_GOTO_LOCATION, &CDocumentTxtEditor::OnGotoLocation)
	ON_COMMAND(ID_DOCUMENT_SAVE, &CDocumentTxtEditor::OnDocumentSave)
	ON_COMMAND(ID_DOCUMENT_SAVE_AS, &CDocumentTxtEditor::OnDocumentSaveAs)
	ON_COMMAND(ID_FIND, &CDocumentTxtEditor::OnFind)
	ON_COMMAND(ID_REPLACE, &CDocumentTxtEditor::OnReplace)
	ON_UPDATE_COMMAND_UI(ID_UNDO, &CDocumentTxtEditor::OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_REDO, &CDocumentTxtEditor::OnUpdateRedo)
	ON_UPDATE_COMMAND_UI(ID_COPY, &CDocumentTxtEditor::OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_CUT, &CDocumentTxtEditor::OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_PASTE, &CDocumentTxtEditor::OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_DOCUMENT_SAVE, &CDocumentTxtEditor::OnUpdateDocumentSave)
END_MESSAGE_MAP()

void CDocumentTxtEditor::Serialize(CArchive& ar)
{
	if(ar.IsStoring()) {} else {}
}

#ifdef _DEBUG
void CDocumentTxtEditor::AssertValid() const
{
	CDocumentCommon::AssertValid();
}

void CDocumentTxtEditor::Dump(CDumpContext& dc) const
{
	CDocumentCommon::Dump(dc);
}
#endif //_DEBUG
