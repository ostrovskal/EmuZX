
#include "stdafx.h"
#include "Project.h"

CConfig::CConfig(const char* nm, const Array<PROPERTY*>& props_asm, const Array<PROPERTY*>& props_lnk, CPlatform* p) : name(nm), plt(p)
{
	size_t i;

	for(i = 0 ; i < props_asm.size() ; i++)
		options_asm.add(new PROPERTY(props_asm[i]));
	for(i = 0 ; i < props_lnk.size() ; i++)
		options_link.add(new PROPERTY(props_lnk[i]));
}

void CConfig::SetProjectExclude(CProject* prj, long use)
{
	int index(includes.find(prj));
	if(use)
	{
		// удаляем его
		if(index != -1)
			includes.remove(index, 1);
	}
	else
	{
		// добавляем
		if(index == -1)
			includes.add(prj);
	}
}

CString CConfig::GetIncludeProjects() const
{
	CString str;
	for(size_t i = 0 ; i < includes.size() ; i++)
	{
		if(str != "")
			str += "#";
		str += includes[i]->GetName();
	}
	return str;
}

CString CConfig::GetOptions(bool is_asm)
{
	PROPERTY* prop;
	long flags;
	Array<String> vals;
	Array<String> kvals;
	CString opts, opt;
	size_t index;
	// ML [ /options ] filelist [ /link linkoptions ]
	// LINK [options] [files] [@commandfile]
	// 1. путь к проге
	Array<PROPERTY*>* arr(is_asm ? &options_asm : &options_link);
	if(!(prop = GetOption(is_asm, NULL, (is_asm ? FOPT_ASSEMBLER : FOPT_LINKER))))
		return "";
	CString prg("\"" + prop->opt + "\"");
	// 2. опции
	for(size_t i = 1 ; i < arr->size() ; i++)
	{
		prop = arr->get(i);
		flags = prop->flags;
		if(flags & (FOPT_ASSEMBLER | FOPT_LINKER))
			continue;
		opt = "";
		switch(prop->flags&FOPT_TYPE)
		{
			case FOPT_COMBO:
				Utils::explode(",", prop->opt_val, vals);
				Utils::explode(",", prop->key_val, kvals);
				index = vals.find(String(prop->opt));
				opt.Format(" %s%s", prop->key, (index != -1 ? kvals[index] : kvals[0]));
				break;
			case FOPT_BOOL:
				if(prop->lVal || prop->flags&FOPT_ALWAYS)
					opt = " " + prop->key;
				break;
			case FOPT_NUMBER:
				if(prop->lVal || prop->flags&FOPT_ALWAYS)
					opt.Format(" %s%d", prop->key, prop->lVal);
				break;
			case FOPT_STRING:
			case FOPT_FOLDER:
			case FOPT_PATH:
				if(prop->opt != "" || prop->flags&FOPT_ALWAYS)
					opt.Format(" %s\"%s\"", prop->key, prop->opt);
				break;
		}
		opts += opt;
	}
	return prg + opts;
}
