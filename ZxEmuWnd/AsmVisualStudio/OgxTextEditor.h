
#pragma once

class COgxTextEditor : public CWnd
{
protected:
	DECLARE_DYNCREATE(COgxTextEditor)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x9809C162, 0xA477, 0x4E5D, { 0xAE, 0xD2, 0x3C, 0x7C, 0xF6, 0xDE, 0x15, 0x25 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}
	void SetText(LPCTSTR text, long status_lines)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4 ;
		InvokeHelper(0x1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, text, status_lines);
	}
	long IsTextCopy()
	{
		long result;
		InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long IsTextCut()
	{
		long result;
		InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long IsTextPaste()
	{
		long result;
		InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void TextCopy()
	{
		InvokeHelper(0x5, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TextCut()
	{
		InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TextPaste()
	{
		InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long IsTextModify()
	{
		long result;
		InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long IsTextReadOnly()
	{
		long result;
		InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void GetOffsetXY(long * x_offs, long * y_offs)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 ;
		InvokeHelper(10, DISPATCH_METHOD, VT_EMPTY, NULL, parms, x_offs, y_offs);
	}
	void SetGroup(long index, long group)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(11, DISPATCH_METHOD, VT_EMPTY, NULL, parms, index, group);
	}
	void SetStdGroup(long std_group)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(12, DISPATCH_METHOD, VT_EMPTY, NULL, parms, std_group);
	}
	CString GetText()
	{
		CString result;
		InvokeHelper(13, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	CString GetTextFromPoint(long y)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(14, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, y);
		return result;
	}
	void Initialize(long x, long y, long lang, long readonly)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(15, DISPATCH_METHOD, VT_EMPTY, NULL, parms, x, y, lang, readonly);
	}
	long IsTextUndo()
	{
		long result;
		InvokeHelper(16, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long IsTextRedo()
	{
		long result;
		InvokeHelper(17, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	void TextUndo()
	{
		InvokeHelper(18, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void TextRedo()
	{
		InvokeHelper(19, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void GotoLine(long y, long is_selected)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(20, DISPATCH_METHOD, VT_EMPTY, NULL, parms, y, is_selected);
	}
	void SelectAll()
	{
		InvokeHelper(21, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetStatusLine(LONG line, LONG status, LONG update)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4;
		InvokeHelper(22, DISPATCH_METHOD, VT_EMPTY, NULL, parms, line, status, update);
	}
	long GetStatusLine(LONG line)
	{
		long result;
		static BYTE parms[] = VTS_I4;
		InvokeHelper(23, DISPATCH_METHOD, VT_I4, (void*)&result, parms, line);
		return result;
	}
	long GetCountLines()
	{
		long result;
		InvokeHelper(24, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	long GetLineWords(long y, long arr)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_HANDLE;
		InvokeHelper(25, DISPATCH_METHOD, VT_I4, (void*)&result, parms, y, arr);
		return result;
	}
	void SetStatusNode(LONG line, LONG status)
	{
		static BYTE parms[] = VTS_I4 VTS_I4;
		InvokeHelper(26, DISPATCH_METHOD, VT_EMPTY, NULL, parms, line, status);
	}
	long GetPageLines(LONG* lines, LONG* statuses)
	{
		long result;
		static BYTE parms[] = VTS_PI4 VTS_PI4;
		InvokeHelper(27, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lines, statuses);
		return result;
	}
	void SetFontParams(long grp_fonts)
	{
		static BYTE parms[] = VTS_I4;
		InvokeHelper(28, DISPATCH_METHOD, VT_EMPTY, NULL, parms, grp_fonts);
	}
	CString GetWordFromCaretPosition()
	{
		CString result;
		InvokeHelper(29, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
		return result;
	}
	long FindReplace(LPCTSTR find_wrd, LPCTSTR repl_wrd, long flags)
	{
		long result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4;
		InvokeHelper(30, DISPATCH_METHOD, VT_I4, (void*)&result, parms, find_wrd, repl_wrd, flags);
		return result;
	}
	void SetModifyText(long status)
	{
		static BYTE parms[] = VTS_I4;
		InvokeHelper(31, DISPATCH_METHOD, VT_EMPTY, NULL, parms, status);
	}
	void AddString(LPCTSTR line, LONG status)
	{
		static BYTE parms[] = VTS_BSTR VTS_I4;
		InvokeHelper(32, DISPATCH_METHOD, VT_EMPTY, NULL, parms, line, status);
	}
	void GetCaretPosition(LONG* x, LONG* y)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4;
		InvokeHelper(33, DISPATCH_METHOD, VT_EMPTY, NULL, parms, x, y);
	}
	void UpdateAll(LONG flags)
	{
		static BYTE parms[] = VTS_I4;
		InvokeHelper(34, DISPATCH_METHOD, VT_EMPTY, NULL, parms, flags);
	}
	void SetCaretPosition(LONG x, LONG y, LONG is_update)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4;
		InvokeHelper(35, DISPATCH_METHOD, VT_EMPTY, NULL, parms, x, y, is_update);
	}
	void FormatText(LONG is_update)
	{
		static BYTE parms[] = VTS_I4;
		InvokeHelper(36, DISPATCH_METHOD, VT_EMPTY, NULL, parms, is_update);
	}
	CString GetString(LONG y)
	{
		CString result;
		static BYTE parms[] = VTS_I4;
		InvokeHelper(37, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, y);
		return result;
	}
};
