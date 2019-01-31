
#include "StdAfx.h"
#include "AsmVisualStudio.h"
#include "Project.h"

static bool IsProperty(Array<PROPERTY*>* prop, const char* name)
{
	for(size_t i = 0 ; i < prop->size() ; i++)
	{
		if(prop->get(i)->name == name)
			return true;
	}

	return false;
}

void CProject::SetModify()
{
	theApp.frm->GetSolution()->SetModify();
}

void CPlatform::SetModify()
{
	sln->SetModify();
}

CConfig* CPlatform::AddConfig(const char* nm, const Array<PROPERTY*>& props_asm, const Array<PROPERTY*>& props_lnk)
{
	if(GetConfig(nm))
		return NULL;
	CConfig* new_cfg(new CConfig(nm, props_asm, props_lnk, this));
	configs.add(new_cfg);
	return new_cfg;
}

void CProject::init()
{
	is_open = false;
	name = "";
	path = "";
	elements.deleteAll();
}

bool CProject::Create(const char* pth, const char* nm, WIZARD* wzd)
{
	// создать путь, если его нет
	if(!Utils::createPath(pth, true))
		return false;

	name = nm;
	path = pth;
	is_open = true;

	// используя мастер создать всю структуру проекта
	if(wzd)
	{
		WIZARD_ELEMENT* welem;
		CElement* elem;
		CSolution* sln(theApp.frm->GetSolution());

		CFile fl;
		CString tmp, pth(path.Left(path.ReverseFind('\\') + 1)), add_str;
		long i_modules(0);
		// поставить текущие переменные окружения
		SetEnvVar(SF_PROJECT_PATH, pth);
		SetEnvVar(SF_PROJECT_NAME, nm);
		// добавить документы из мастера
		for(size_t i = 0 ; i < wzd->elems.size() ; i++)
		{
			try
			{
				welem = wzd->elems[i];
				// взять имя в файлах и провести фильтрацию
				CString name(ApplyFilter(welem->path_files.Mid(welem->path_files.ReverseFind('\\') + 1)));
				// грузим в память
				if(!fl.Open(ApplyFilter(welem->path), CFile::modeRead))
					continue;
				long len(fl.GetLength());
				if(fl.Read(tmp.GetBuffer(len), len) != len) throw(0);
				tmp.ReleaseBuffer();
				// провести фильтрацию содержимого
				tmp = ApplyFilter(tmp);
				fl.Close();
				// добавить файл
				if((elem = AddElement(pth + name)))
				{
					if(!fl.Open(pth + name, CFile::modeCreate | CFile::modeWrite))
						continue;
					fl.Write(tmp, tmp.GetLength() + 1);
					fl.Close();
					elem->is_exclude = welem->is_exclude;
					// установить для него путь в модулях
					name = ApplyFilter(welem->path_modules);
					if(name != "")
					{
						if(IsElementPath(name, false))
						{
							while(true)
							{
								add_str.Format("%s(%i)", name, i_modules);
								if(!IsElementPath(add_str, true))
									break;
								i_modules++;
							}
							name = add_str;
						}
						elem->path_modules = name;
					}
				}
			}
			catch(...) {}
		}

		name_wizard = wzd->name;
	}

	return Save();
}

// структура:
// <asm_studio_project>
//	<name>""</name>
//	<name_wizard>""</name_wizard>
//	<elements>
//		<element flags="" x="" y="" is_open="" is_exlude="">
//		<bookmarks>
//			<status>""</status>
//			<number></number>
//		</bookmarks>
//		<path>""</path>
//		<path_files></path_files>
//		<path_modules></path_modules>
//		</element>
//		.......................................
//	</elements>
// <\asm_studio_project>
bool CProject::Open(const char* pth)
{
	Xml xml;
	String val;
	CElement* elem;
	size_t index;
	uintptr_t hroot, hname, helems, helem, hbmks, h;

	if(!xml.open(pth))
		return false;
	if(!(hroot = xml.getNode(xml.getRoot(), "asm_studio_project")))
		return false;
	if(!(hname = xml.getNode(hroot, "name")))
		return false;
	xml.getValue(hname, val);
	if(val == "")
		return false;
	name = val;
	if((hname = xml.getNode(hroot, "name_wizard")))
	{
		xml.getValue(hname, val);
		name_wizard = val;
	}
	if((hname = xml.getNode(hroot, "cmd_params")))
	{
		xml.getValue(hname, val);
		cmd_params = val;
	}
	// 2. элементы
	if((helems = xml.getNode(hroot, "elements")))
	{
		index = 0;
		while((helem = xml.getNode(helems, index++)))
		{
			val = xml.getNodeName(helem);
			if(val != "element")
				continue;
			if(!(h = xml.getNode(helem, "path")))
				continue;
			xml.getValue(h, val);
			if(!GetElement(val, NULL, NULL))
			{
				elem = new CElement(val, this);
				xml.getParamValue(helem, "x", &elem->x);
				xml.getParamValue(helem, "y", &elem->y);
				xml.getParamValue(helem, "is_open", &elem->is_open);
				xml.getParamValue(helem, "is_exclude", &elem->is_exclude);
				if((h = xml.getNode(helem, "path_files")))
				{
					xml.getValue(h, val);
					elem->path_files = val;
				}
				if((h = xml.getNode(helem, "path_modules")))
				{
					xml.getValue(h, val);
					elem->path_modules = val;
				}
				if((hbmks = xml.getNode(helem, "bookmarks")))
				{
					Array<String> arr_bmks;
					xml.getValue(hbmks, val);
					Utils::explode(",", val, arr_bmks);
					for(size_t i = 0 ; i < arr_bmks.size() ; i += 2)
						elem->bookmarks.add(BOOKMARK(atol(arr_bmks[i]), atol(arr_bmks[i + 1])));
				}
			}
			elements.add(elem);
		}
	}
	path = pth;
	is_open = true;
	return true;
}

bool CProject::Save()
{
	Xml xml;
	String val;
	CElement* elem;

	size_t i;
	uintptr_t hroot, helems, helem;

	if(!(hroot = xml.addNode(xml.getRoot(), "asm_studio_project", "")))
		return false;
	xml.addNode(hroot, "name", name);
	xml.addNode(hroot, "name_wizard", name_wizard);
	xml.addNode(hroot, "cmd_params", cmd_params);
	// элементы
	if((helems = xml.addNode(hroot, "elements", "")))
	{
		for(i = 0 ; i < elements.size() ; i++)
		{
			elem = elements[i];
			if((helem = xml.addNode(helems, "element", "")))
			{
				xml.addParam(helem, "x", elem->x);
				xml.addParam(helem, "y", elem->y);
				xml.addParam(helem, "is_open", elem->is_open);
				xml.addParam(helem, "is_exclude", elem->is_exclude);
				// закладки
				xml.addNode(helem, "bookmarks", elem->GetBookmarks());
				// пути
				xml.addNode(helem, "path", elem->path);
				xml.addNode(helem, "path_files", elem->path_files);
				xml.addNode(helem, "path_modules", elem->path_modules);
			}
		}
	}

	return xml.save(path);
}


bool CProject::IsElementPath(const char* path, bool is_files)
{
	CElement* elem;
	for(size_t i = 0 ; i < elements.size() ; i++)
	{
		elem = elements[i];
		if(is_files ? elem->path_files == path : elem->path_modules == path)
			return true;
	}
	return false;
}

CElement* CProject::AddElement(const char* path)
{
	CElement* elem;
	if((elem = GetElement(path, NULL, NULL)))
		return elem;
	// сформировать путь в файлах и в модулях
	CString add_str;
	CString pth(path);
	CString path_files(pth.Mid(pth.ReverseFind('\\') + 1));
	CString path_modules(path_files.Left(path_files.ReverseFind('.')));
	CString ext(path_files.Mid(path_files.ReverseFind('.') + 1));
	ext.MakeLower();
	if(ext == "asm") path_files = "Исходные файлы\\" + path_files;
	else if(ext == "inc") path_files = "Подключаемые файлы\\" + path_files;
	else if(ext == "bin") path_files = "Двоичные файлы\\" + path_files;
	else path_files = "Ресурсные файлы\\" + path_files;
	CString path_files1(path_files.Left(path_files.ReverseFind('.')));
	long i_modules(1), i_files(1);
	// сделать проверку - чтобы не было одинаковых путей(в этом случае менять путь)
	if(IsElementPath(path_files, true))
	{
		while(true)
		{
			add_str.Format("%s(%i).%s", path_files1, i_files, ext);
			if(!IsElementPath(add_str, true))
				break;
			i_files++;
		}
		path_files = add_str;
	}
	if(IsElementPath(path_modules, false))
	{
		while(true)
		{
			add_str.Format("%s(%i)", path_modules, i_modules);
			if(!IsElementPath(add_str, true))
				break;
			i_modules++;
		}
		path_modules = add_str;
	}
	elem = new CElement(path, path_files, path_modules, this);
	elements.add(elem);
	theApp.frm->GetSolution()->SetModify();
	return elem;
}

bool CProject::DeleteElement(const char* path)
{
	size_t index;
	if(!GetElement(path, NULL, NULL, &index))
		return false;
	elements.del(index);
	theApp.frm->GetSolution()->SetModify();
	return true;
}

CElement* CProject::GetElement(const char* path, const char* path_files, const char* path_modules, size_t* index)
{
	CElement* elem;
	size_t i;
	for(i = 0 ; i < elements.size() ; i++)
	{
		elem = elements[i];
		if(path)
		{
			if(elem->GetPath() == path)
				break;
		}
		else if(path_files)
		{
			if(elem->path_files == path_files)
				break;
		}
		if(path_modules)
		{
			if(elem->path_modules == path_modules)
				break;
		}
	}
	if(i < elements.size())
	{
		if(index) *index = i;
		return elem;
	}

	return NULL;
}

void CProject::MiniSave()
{
	Xml xml;
	String val;
	CElement* elem;
	uintptr_t hroot, helems, helem, h;

	if(xml.open(path))
	{
		if((hroot = xml.getNode(xml.getRoot(), "asm_studio_project")))
		{
			if((h = xml.getNode(hroot, "cmd_params")))
				xml.setValue(h, cmd_params);
			// элементы
			if((helems = xml.getNode(hroot, "elements")))
			{
				size_t index(0);
				while((helem = xml.getNode(helems, index++)))
				{
					val = xml.getNodeName(helem);
					if(val != "element")
						continue;
					if(!(h = xml.getNode(helem, "path")))
						continue;
					xml.getValue(h, val);
					if((elem = GetElement(val, NULL, NULL)))
					{
						xml.setParamValue(helem, "x", elem->x);
						xml.setParamValue(helem, "y", elem->y);
						xml.setParamValue(helem, "is_open", elem->is_open);
						xml.setParamValue(helem, "is_exclude", elem->is_exclude);
						if((h = xml.getNode(helem, "path_files")))
							xml.setValue(h, elem->path_files);
						if((h = xml.getNode(helem, "path_modules")))
							xml.setValue(h, elem->path_modules);
						if((h = xml.getNode(helem, "bookmarks")))
							xml.setValue(h, elem->GetBookmarks());
					}
				}
				xml.save(path);
			}
		}
	}
}
