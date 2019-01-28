
#include "stdafx.h"
#include "Project.h"

void CElement::Parser(CDocumentCommon* doc)
{

}

void CElement::DynamicParser(long y1, long y2, long action)
{

}

void CElement::SetBookmark(int num, int status)
{
	for(size_t i = 0 ; i < bookmarks.size() ; i++)
	{
		if(bookmarks[i].number == num)
		{
			status == SSL_BMD ? bookmarks.remove(i, 1) : bookmarks[i].status = status;
			return;
		}
	}
	if(status != SSL_BMD)
		bookmarks.add(BOOKMARK(num, status));
}

CElement::BLOCK_ELEMENT* CElement::GetBlock(const char* name, BlockElementTypes type)
{
	BLOCK_ELEMENT* blk;
	for(size_t i = 0 ; i < block_elems.size() ; i++)
	{
		blk = block_elems[i];
		if(blk->name == name && blk->type == type)
			return blk;
	}
	return NULL;
}

void CElement::SetModify()
{
	prj->SetModify();
}

void CElement::SetExclude(long is)
{
	if(is_exclude != is)
	{
		is_exclude = is;
		prj->SetModify();
	}
}

CString CElement::GetBookmarks()
{
	CString str, fmt;
	for(size_t i = 0 ; i < bookmarks.size() ; i++)
	{
		if(str != "") str += ",";
		fmt.Format("%i,%i", bookmarks[i].number, bookmarks[i].status);
		str += fmt;
	}

	return str;
}
