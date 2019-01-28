
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "Project.h"
#include "ShowUnOpenProjectDlg.h"

static CString str_def_wizard = "<?xml version = \"1.0\" encoding = \"windows-1251\" ?>\
<elements>\
<element path = \"$(MainDir)\\resources\\default_wizard.asm\" path_file = \"$(ProjectName)Main.asm\" path_modules = \"�������\\$(ProjectName)Main\" is_exclude = \"0\"/>\
</elements>\
<configurations>\
<config name = \"Debug\">\
<options_asm>\
<option name = \"����������\" flags = \"path|assembler\" opt = \"$(MainDir)\\Bin\\ml.exe\" opt_val = \"����������� ���� �����������|*.exe|\" tip = \"���������� �����������\"/>\
<option name = \"������������ Include ����\" key = \"/X\" flags = \"bool\" tip = \"������������ ���������� ���� � ���������� ������\"/>\
<option name = \"�������������� ��� ������\" key = \"/WX\" flags = \"bool\" tip = \"����������� �������������� ��� ������\"/>\
<option name = \"������� ��������������\" key = \"/W\" flags = \"select\" opt = \"������� 0\" opt_val = \"������� 0,������� 1,������� 2,������� 3,������� 4\" key_val = \"0,1,2,3,4\" tip = \"���������� ������� ��������������\"/>\
<option name = \"������������ ��������\" key = \"/Zp\" flags = \"select\" opt = \"4\" opt_val = \"4,8,16\" key_val = \"4,8,16\" tip = \"���������� ������������ ��������\"/>\
<option name = \"������� ���������������\" key = \"/Cp\" lval = \"1\" flags = \"bool\" tip = \"��������� ������� ��������������� ������������\"/>\
<option name = \"�������������� � ������� �������\" key = \"/Cu\" flags = \"bool\" tip = \"������������� ��� �������������� � ������� �������\"/>\
<option name = \"������� ������� ������������\" key = \"/Cx\" lval = \"1\" flags = \"bool\" tip = \"��������� ������� � publics externs\"/>\
<option name = \"COFF ������ ���������� �����\" key = \"/coff\" flags = \"bool\" tip = \"������������ ��������� ���� � ������� COFF\"/>\
<option name = \"����� �������\" key = \"/D\" flags = \"string\" opt = \"DEBUG\" tip = \"���������� ����� �������\"/>\
<option name = \"������ �����\" key = \"/F\" flags = \"number\" tip = \"������������� ������ ����� (� ������)\"/>\
<option name = \"���� ��������\" key = \"/Fl\" flags = \"string\" tip = \"������������� ���� ��������\"/>\
<option name = \"�������� � �������\" key = \"/Sc\" flags = \"bool\" tip = \"���������� �������� � �������\"/>\
<option name = \"������� ���������� �������\" key = \"/Sn\" flags = \"bool\" tip = \"�������� ������� ������� ��������\"/>\
<option name = \"���� �����\" key = \"/Fm\" flags = \"string\" tip = \"������������� ���� ����� ������\"/>\
<option name = \"������������ ������\" key = \"/G\" flags = \"select\" opt = \"stdcall\" opt_val = \"pascal,c,stdcall\" key_val = \"c,d,z\" tip = \"��� ������ ��������\"/>\
<option name = \"�������������� Include ����\" key = \"/I\" flags = \"folder\" tip = \"�������������� ���� � ���������� ������\"/>\
<option name = \"�������\" always = \"1\" key = \"/nologo\" flags = \"bool\" tip = \"���������� �������\"/>\
<option name = \"��� ������� �������������\" key = \"/Zf\" flags = \"bool\" tip = \"������� ��� ������� ��������������\"/>\
<option name = \"������ ����� � �������\" key = \"/Zd\" flags = \"bool\" tip = \"�������� ������ ����� � ���������� ����������\"/>\
<option name = \"������� � �������\" key = \"/Zi\" lval = \"1\" flags = \"bool\" tip = \"��������� ���������� ���������� �� ��������\"/>\
<option name = \"�������������\" flags = \"string\" tip = \"�������������� �����\"/>\
</options_asm>\
<options_link>\
<option name = \"�����������\" flags = \"path|linker\" opt = \"$(MainDir)\\Bin\\link.exe\" opt_val = \"����������� ���� ������������|*.exe|\" tip = \"���������� ������������\"/>\
<option name = \"��� �����\" key = \"/OUT:\" flags = \"string|execute\" opt = \"$(ProjectPath)\\$(ConfigurationName)\\$(ProjectName).exe\" tip = \"���������� ��� ��������� �����.\"/>\
<option name = \"������� ��������������\" key = \"/WARN:\" flags = \"select\" opt = \"������� 0\" opt_val = \"������� 0,������� 1,������� 2,������� 3\" key_val = \"0,1,2,3\" tip = \"������� �������������� ���������� �������������.\"/>\
<option name = \"������������ ����������\" flags = \"string\" opt = \"kernel32.lib user32.lib gdi32.lib\" tip = \"���������� �� ���������\"/>\
<option name = \"�������� ������������ ������\" key = \"/ALIGN:\" lval = \"4096\" flags = \"number\" tip = \"������������ ������ � �������� ������.\"/>\
<option name = \"�������\" key = \"/DEBUG\" lval = \"1\" flags = \"bool\" tip = \"��������� ���������� ����������. ���������� ���������� ���������� � pdb-����.\"/>\
<option name = \"��� �������\" key = \"/DEBUGTYPE:\" flags = \"select\" opt = \"COFF\" opt_val = \"CV,COFF,BOTH\" key_val = \"CV,COFF,BOTH\" tip = \"��� ���������� ����������\"/>\
<option name = \"���������� �������\" key = \"/IMPLIB:\" flags = \"string\" tip = \"���������� ��� import-����������.\"/>\
<option name = \"���� �����������\" key = \"/DEF:\" flags = \"string\" tip = \"��� ����� �����������.\"/>\
<option name = \"���������� �� ���������\" key = \"/DEFAULTLIB:\" flags = \"string\" tip = \"�������� ���������� �� ���������.\"/>\
<option name = \"����� �����\" key = \"/ENTRY:\" flags = \"string\" tip = \"���������� ����� �����.\"/>\
<option name = \"�������������� �������\" key = \"/EXPORT:\" flags = \"string\" tip = \"���������� ����� ������� ��� ��������.\"/>\
<option name = \"������� �����\" key = \"/BASE:\" flags = \"number\" tip = \"���������� ������� ����� (����� ��������).\"/>\
<option name = \"������ ����� 2 ��\" key = \"/LARGEADDRESSAWARE\" flags = \"bool\" tip = \"���������� ��������� �������� �������� 2 ��.\"/>\
<option name = \"���� �����\" key = \"/MAP:\" flags = \"string\" tip = \"��������� ���� �����.\"/>\
<option name = \"������ ����\" key = \"/HEAP:\" flags = \"number\" tip = \"���������� ������ ���� (HEAP) � ������.\"/>\
<option name = \"��� ����� �����\" key = \"/MAPINFO:\" flags = \"select\" opt = \"EXPORTS\" opt_val = \"EXPORTS,FIXUPS,LINES\" key_val = \"EXPORTS,FIXUPS,LINES\" tip = \"��� ���������� ���������� � ���� �����.\"/>\
<option name = \"������������ ����������\" key = \"/NODEFAULTLIB:\" flags = \"string\" tip = \"������������ ��� ��� ���������� ����������.\"/>\
<option name = \"�������\" always = \"1\" key = \"/NOLOGO\" flags = \"bool\" tip = \"����� ��������.\"/>\
<option name = \"������ �����\" key = \"/STACK:\" flags = \"number\" tip = \"���������� ������ �����.\"/>\
<option name = \"���� ��������\" key = \"/STUB:\" flags = \"string\" tip = \"���������� ���� - �������� � ������� MS DOS.\"/>\
<option name = \"�������\" key = \"/SUBSYSTEM:\" flags = \"select\" opt = \"WINDOWS\" opt_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" key_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" tip = \"��� ������� ����������.\"/>\
<option name = \"�������������\" flags = \"string\" tip = \"�������������� �����\"/>\
</options_link>\
</config>\
<config name = \"Release\">\
<options_asm>\
<option name = \"����������\" flags = \"path\" opt = \"$(MainDir)\\Bin\\ml.exe\" opt_val = \"����������� ���� �����������|*.exe|\" tip = \"���������� �����������\"/>\
<option name = \"������������ Include ����\" key = \"/X\" flags = \"bool\" tip = \"������������ ���������� ���� � ���������� ������\"/>\
<option name = \"�������������� ��� ������\" key = \"/WX\" flags = \"bool\" tip = \"����������� �������������� ��� ������\"/>\
<option name = \"������� ��������������\" key = \"/W\" flags = \"select\" opt = \"������� 0\" opt_val = \"������� 0,������� 1,������� 2,������� 3,������� 4\" key_val = \"0,1,2,3,4\" tip = \"���������� ������� ��������������\"/>\
<option name = \"������������ ��������\" key = \"/Zp\" flags = \"select\" opt = \"4\" opt_val = \"4,8,16\" key_val = \"4,8,16\" tip = \"���������� ������������ ��������\"/>\
<option name = \"������� ���������������\" key = \"/Cp\" lval = \"1\" flags = \"bool\" tip = \"��������� ������� ��������������� ������������\"/>\
<option name = \"�������������� � ������� �������\" key = \"/Cu\" flags = \"bool\" tip = \"������������� ��� �������������� � ������� �������\"/>\
<option name = \"������� ������� ������������\" key = \"/Cx\" lval = \"1\" flags = \"bool\" tip = \"��������� ������� � publics externs\"/>\
<option name = \"COFF ������ ���������� �����\" key = \"/coff\" flags = \"bool\" tip = \"������������ ��������� ���� � ������� COFF\"/>\
<option name = \"����� �������\" key = \"/D\" flags = \"string\" tip = \"���������� ����� �������\"/>\
<option name = \"������ �����\" key = \"/F\" flags = \"number\" tip = \"������������� ������ ����� (� ������)\"/>\
<option name = \"���� ��������\" key = \"/Fl\" flags = \"string\" tip = \"������������� ���� ��������\"/>\
<option name = \"�������� � �������\" key = \"/Sc\" flags = \"bool\" tip = \"���������� �������� � �������\"/>\
<option name = \"������� ���������� �������\" key = \"/Sn\" flags = \"bool\" tip = \"�������� ������� ������� ��������\"/>\
<option name = \"���� �����\" key = \"/Fm\" flags = \"string\" tip = \"������������� ���� ����� ������\"/>\
<option name = \"������������ ������\" key = \"/G\" flags = \"bool\" opt = \"stdcall\" opt_val = \"pascal,c,stdcall\" key_val = \"c,d,z\" tip = \"��� ������ ��������\"/>\
<option name = \"�������������� Include ����\" key = \"/I\" flags = \"folder\" tip = \"�������������� ���� � ���������� ������\"/>\
<option name = \"�������\" always = \"1\" key = \"/nologo\" flags = \"bool\" tip = \"���������� �������\"/>\
<option name = \"��� ������� �������������\" key = \"/Zf\" flags = \"bool\" tip = \"������� ��� ������� ��������������\"/>\
<option name = \"�������������\" flags = \"string\" tip = \"�������������� �����\"/>\
</options_asm>\
<options_link>\
<option name = \"�����������\" flags = \"path\" opt = \"$(MainDir)\\Bin\\link.exe\" opt_val = \"����������� ���� ������������|*.exe|\" tip = \"���������� ������������\"/>\
<option name = \"��� �����\" key = \"/OUT:\" flags = \"string\" opt = \"$(ProjectPath)\\$(ConfigurationName)\\$(ProjectName).exe\" tip = \"���������� ��� ��������� �����.\"/>\
<option name = \"���� � �����������\" key = \"/LIBPATH:\" flags = \"folder\" opt = \"$(PathLib)\" tip = \"���������� ���� � �����������.\"/>\
<option name = \"������� ��������������\" key = \"/WARN:\" flags = \"select\" opt = \"������� 0\" opt_val = \"������� 0,������� 1,������� 2,������� 3\" key_val = \"0,1,2,3\" tip = \"������� �������������� ���������� �������������.\"/>\
<option name = \"������������ ����������\" flags = \"string\" opt = \"kernel32.lib user32.lib gdi32.lib\" tip = \"���������� �� ���������\"/>\
<option name = \"�������� ������������ ������\" key = \"/ALIGN:\" lval = \"4096\" flags = \"number\" tip = \"������������ ������ � �������� ������.\"/>\
<option name = \"���������� �������\" key = \"/IMPLIB:\" flags = \"string\" tip = \"���������� ��� import-����������.\"/>\
<option name = \"���� �����������\" key = \"/DEF:\" flags = \"string\" tip = \"��� ����� �����������.\"/>\
<option name = \"���������� �� ���������\" key = \"/DEFAULTLIB:\" flags = \"string\" tip = \"�������� ���������� �� ���������.\"/>\
<option name = \"����� �����\" key = \"/ENTRY:\" flags = \"string\" tip = \"���������� ����� �����.\"/>\
<option name = \"�������������� �������\" key = \"/EXPORT:\" flags = \"string\" tip = \"���������� ����� ������� ��� ��������.\"/>\
<option name = \"������������ ����������\" key = \"/INCREMENTAL\" flags = \"bool\" tip = \"��� ������ ����������� �������������� ���������� ����������� ������� ����������������� ���� ����.\"/>\
<option name = \"������� �����\" key = \"/BASE:\" flags = \"number\" tip = \"���������� ������� ����� (����� ��������).\"/>\
<option name = \"������ ����� 2 ��\" key = \"/LARGEADDRESSAWARE\" flags = \"bool\" tip = \"���������� ��������� �������� �������� 2 ��.\"/>\
<option name = \"���� �����\" key = \"/MAP:\" flags = \"string\" tip = \"��������� ���� �����.\"/>\
<option name = \"������ ����\" key = \"/HEAP:\" flags = \"number\" tip = \"���������� ������ ���� (HEAP) � ������.\"/>\
<option name = \"��� ����� �����\" key = \"/MAPINFO:\" flags = \"select\" opt = \"EXPORTS\" opt_val = \"EXPORTS,FIXUPS,LINES\" key_val = \"EXPORTS,FIXUPS,LINES\" tip = \"��� ���������� ���������� � ���� �����.\"/>\
<option name = \"������������ ����������\" key = \"/NODEFAULTLIB\" flags = \"string\" tip = \"������������ ��� ��� ���������� ����������.\"/>\
<option name = \"�������\" always = \"1\" key = \"/NOLOGO\" flags = \"bool\" tip = \"����� ��������.\"/>\
<option name = \"������ �����\" key = \"/STACK:\" flags = \"number\" tip = \"���������� ������ �����.\"/>\
<option name = \"���� ��������\" key = \"/STUB:\" flags = \"string\" tip = \"���������� ���� - �������� � ������� MS DOS.\"/>\
<option name = \"�������\" key = \"/SUBSYSTEM:\" flags = \"select\" opt = \"WINDOWS\" opt_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" key_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" tip = \"��� ������� ����������.\"/>\
<option name = \"�������������\" flags = \"string\" tip = \"�������������� �����\"/>\
</options_link>\
</config>\
</configurations>";

extern SOLUTION_FILTER arr_env_var[SF_LAST_ENV_VAR];

CSolution::CSolution()
{
	String temp_dir, work_dir, main_dir, sys_dir, name_prg;
	Xml xml;
	// �����
	Init();
	// ������������� ���������� ��������� ����� 
	Utils::getSysDir(&temp_dir, &work_dir, &main_dir, &sys_dir, &name_prg, FALSE);
	arr_env_var[SF_MAIN_DIR].name = SFN_MAIN_DIR;
	arr_env_var[SF_MAIN_DIR].value = main_dir;
	arr_env_var[SF_WORK_DIR].name = SFN_WORK_DIR;
	arr_env_var[SF_WORK_DIR].value = work_dir;
	arr_env_var[SF_TEMP_DIR].name = SFN_TEMP_DIR;
	arr_env_var[SF_TEMP_DIR].value = temp_dir;
	arr_env_var[SF_SYS_DIR].name = SFN_SYS_DIR;
	arr_env_var[SF_SYS_DIR].value = sys_dir;
	arr_env_var[SF_SOLUTION_NAME].name = SFN_SOLUTION_NAME;
	arr_env_var[SF_SOLUTION_PATH].name = SFN_SOLUTION_PATH;
	arr_env_var[SF_SOLUTION_EXT].name = SFN_SOLUTION_EXT;
	arr_env_var[SF_SOLUTION_EXT].value = EXT_SLN;
	arr_env_var[SF_PROJECT_NAME].name = SFN_PROJECT_NAME;
	arr_env_var[SF_PROJECT_PATH].name = SFN_PROJECT_PATH;
	arr_env_var[SF_PROJECT_EXT].name = SFN_PROJECT_EXT;
	arr_env_var[SF_PROJECT_EXT].value = EXT_PRJ;
	arr_env_var[SF_DOCUMENT_NAME].name = SFN_DOCUMENT_NAME;
	arr_env_var[SF_DOCUMENT_PATH].name = SFN_DOCUMENT_PATH;
	arr_env_var[SF_DOCUMENT_EXT].name = SFN_DOCUMENT_EXT;
	arr_env_var[SF_DOCUMENT_MODULES_PATH].name = SFN_DOCUMENT_MODULES_PATH;
	arr_env_var[SF_DOCUMENT_FILES_PATH].name = SFN_DOCUMENT_FILES_PATH;
	arr_env_var[SF_CONFIGURATION_NAME].name = SFN_CONFIGURATION_NAME;
	arr_env_var[SF_CONFIGURATION_PATH].name = SFN_CONFIGURATION_PATH;
	arr_env_var[SF_INCLUDE_PATH].name = SFN_INCLUDE_PATH;
	arr_env_var[SF_INCLUDE_PATH].value = main_dir + "\\Include";
	arr_env_var[SF_LIB_PATH].name = SFN_LIB_PATH;
	arr_env_var[SF_LIB_PATH].value = main_dir + "\\Lib";
	arr_env_var[SF_PLATFORM_NAME].name = SFN_PLATFORM_NAME;
	arr_env_var[SF_PROGRAMM_NAME].name = SFN_PROGRAMM_NAME;
	arr_env_var[SF_PROGRAMM_NAME].value = name_prg;
	arr_env_var[SF_PROJECT_EXECUTE_NAME].name = SFN_PROJECT_EXECUTE_NAME;
	arr_env_var[SF_PROJECT_EXECUTE_PATH].name = SFN_PROJECT_EXECUTE_PATH;
	// ������� ������� �� ���������
	if(xml.setXML(str_def_wizard, str_def_wizard.GetLength() + 1))
	{
		WIZARD_ELEMENT* welem;
		WIZARD_CONFIG* wcfg;
		PROPERTY* prop;
		String val;

		uintptr_t hgrp, helem, hprop, hcfg, h;

		default_wizard.name = "EXE (MASM)";
		default_wizard.path_icon = arr_env_var[SF_MAIN_DIR].value + "\\resources\\default_wizard_icon.ico";
		default_wizard.name_platform = "Win32";
		default_wizard.hIcon = (HICON)LoadImage(NULL, default_wizard.path_icon, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
		// ��������
		if((hgrp = xml.getNode(xml.getRoot(), "elements")))
		{
			size_t index_elem(0);
			while((helem = xml.getNode(hgrp, index_elem++)))
			{
				xml.getParamValue(helem, "path", val);
				welem = new WIZARD_ELEMENT;
				welem->path = val;
				xml.getParamValue(helem, "path_modules", val);
				welem->path_modules = val;
				xml.getParamValue(helem, "path_files", val);
				welem->path_files = val;
				xml.getParamValue(helem, "is_exclude", &welem->is_exclude);
				default_wizard.elems.add(welem);
			}
		}
		// ������������
		if((hgrp = xml.getNode(xml.getRoot(), "configurations")))
		{
			size_t index_cfg(0);
			while((hcfg = xml.getNode(hgrp, index_cfg++)))
			{
				xml.getParamValue(hcfg, "name", val);
				wcfg = new WIZARD_CONFIG(val);
				if((hprop = xml.getNode(hcfg, "options_asm")))
				{
					size_t index_asm(0);
					while((h = xml.getNode(hprop, index_asm++)))
					{
						xml.getParamValue(h, "name", val);
						prop = new PROPERTY();
						prop->name = val;
						if(xml.getParamValue(h, "opt", val))
							prop->opt = val;
						if(xml.getParamValue(h, "opt_val", val))
							prop->opt_val = val;
						if(xml.getParamValue(h, "key", val))
							prop->key = val;
						if(xml.getParamValue(h, "key_val", val))
							prop->key_val = val;
						if(xml.getParamValue(h, "tip", val))
							prop->tip = val;
						if(xml.getParamValue(h, "flags", val))
							prop->flags = Utils::convertStringFlagsToLong(val, 12, stk_prop_flags);
						xml.getParamValue(h, "lval", &prop->lVal);
						wcfg->asm_props.add(prop);
					}
				}
				if((hprop = xml.getNode(hcfg, "options_link")))
				{
					size_t index_lnk(0);
					while((h = xml.getNode(hprop, index_lnk++)))
					{
						xml.getParamValue(h, "name", val);
						prop = new PROPERTY();
						prop->name = val;
						if(xml.getParamValue(h, "opt", val))
							prop->opt = val;
						if(xml.getParamValue(h, "opt_val", val))
							prop->opt_val = val;
						if(xml.getParamValue(h, "key", val))
							prop->key = val;
						if(xml.getParamValue(h, "key_val", val))
							prop->key_val = val;
						if(xml.getParamValue(h, "tip", val))
							prop->tip = val;
						if(xml.getParamValue(h, "flags", val))
							prop->flags = Utils::convertStringFlagsToLong(val, 12, stk_prop_flags);
						xml.getParamValue(h, "lval", &prop->lVal);
						wcfg->lnk_props.add(prop);
					}
				}
				default_wizard.configs.add(wcfg);
			}
		}
	}
}

void CSolution::SetCurrentElement(CElement* elem)
{
	if(elem && elem->is_fake)
		elem = NULL;
	if(elem && GetEnvVar(SF_DOCUMENT_PATH) != elem->path)
	{
		CString pth(elem->path);
		int pos_n(pth.ReverseFind('\\') + 1);
		int pos_e(pth.ReverseFind('.'));

		SetEnvVar(SF_DOCUMENT_PATH, pth);
		SetEnvVar(SF_DOCUMENT_NAME, pth.Mid(pos_n, pos_e - pos_n));
		SetEnvVar(SF_DOCUMENT_EXT, pth.Mid(pos_e));
		SetEnvVar(SF_DOCUMENT_MODULES_PATH, elem->path_modules);
		SetEnvVar(SF_DOCUMENT_FILES_PATH, elem->path_files);
	}
	else if(GetEnvVar(SF_DOCUMENT_PATH) != "")
	{
		SetEnvVar(SF_DOCUMENT_PATH, "");
		SetEnvVar(SF_DOCUMENT_NAME, "");
		SetEnvVar(SF_DOCUMENT_EXT, "");
		SetEnvVar(SF_DOCUMENT_MODULES_PATH, "");
		SetEnvVar(SF_DOCUMENT_FILES_PATH, "");
	}

	current_element = elem;
}

void CSolution::Init()
{
	path = "";
	nErrors = 0;

	current_project = NULL;
	execute_project = NULL;
	current_element = NULL;

	SetEnvVar(SF_PROJECT_EXECUTE_PATH, "");
	SetEnvVar(SF_PROJECT_EXECUTE_NAME, "");
	SetEnvVar(SF_PROJECT_NAME, "");
	SetEnvVar(SF_PROJECT_PATH, "");

	SetEnvVar(SF_CONFIGURATION_PATH, "");
	SetEnvVar(SF_CONFIGURATION_NAME, "");
	SetEnvVar(SF_PLATFORM_NAME, "");

	SetEnvVar(SF_SOLUTION_NAME, "");
	SetEnvVar(SF_SOLUTION_PATH, "");

	SetEnvVar(SF_DOCUMENT_PATH, "");
	SetEnvVar(SF_DOCUMENT_NAME, "");
	SetEnvVar(SF_DOCUMENT_EXT, "");
	SetEnvVar(SF_DOCUMENT_MODULES_PATH, "");
	SetEnvVar(SF_DOCUMENT_FILES_PATH, "");

	projects.deleteAll();
	platforms.deleteAll();

	is_modify = false;

	if(theApp.frm)
		theApp.frm->SetTitle(ASM_STUDIO);
}

CProject* CSolution::GetProject(const char* name, int* index)
{
	for(size_t i = 0 ; i < projects.size() ; i++)
	{
		CProject* p(projects[i]);
		if(p->GetName() == name)
		{
			if(index) *index = i;
			return p;
		}
	}

	return NULL;
}

bool CSolution::Open(const char* pth)
{
	Xml xml;
	CProject* prj;
	CPlatform* plt;
	CConfig* cfg;
	String val;
	uintptr_t hroot, hprj, harch, h, hplts, hplt, hcfg, hopt;
	CStringArray arr;

	if(!xml.open(pth))
		return false;
	if(!(hroot = xml.getNode(xml.getRoot(), "asm_studio_solution")))
		return false;
	if((hprj = xml.getNode(hroot, "projects")))
	{
		size_t iprj(0);
		while((h = xml.getNode(hprj, iprj++)))
		{
			val = xml.getNodeName(h);
			if(val != "project")
				continue;
			xml.getValue(h, val);
			prj = new CProject();
			if(prj->Open(val))
			{
				projects.add(prj);
			}
			else
			{
				arr.Add(val);
				delete prj;
			}
		}
	}
	// ���������
	if((hplts = xml.getNode(hroot, "platforms")))
	{
		size_t iplt(0);
		while((hplt = xml.getNode(hplts, iplt++)))
		{
			val = xml.getNodeName(hplt);
			if(val != "platform")
				continue;
			if(!xml.getParamValue(hplt, "name", val))
				continue;
			if(!GetPlatform(val))
			{
				plt = new CPlatform(val, this);
				size_t index_cfg(0);
				while((hcfg = xml.getNode(hplt, index_cfg++)))
				{
					val = xml.getNodeName(hcfg);
					if(val != "config")
						continue;
					if(!xml.getParamValue(hcfg, "name", val))
						continue;
					if((cfg = plt->AddConfig(val)))
					{
						if(xml.getParamValue(hcfg, "include_projects", val))
						{
							Array<String> arr;
							Utils::explode("#", val, arr);
							for(size_t i = 0 ; i < arr.size() ; i++)
								cfg->SetProjectExclude(GetProject(arr[i]), FALSE);
						}
						// ����� ����������
						if((hopt = xml.getNode(hcfg, "options_asm")))
						{
							size_t index_asm(0);
							while((h = xml.getNode(hopt, index_asm++)))
							{
								PROPERTY prop;
								val = xml.getNodeName(h);
								if(val != "option")
									continue;
								if(xml.getParamValue(h, "name", val))
								{
									prop.name = val;
									if(xml.getParamValue(h, "opt", val))
										prop.opt = val;
									if(xml.getParamValue(h, "opt_val", val))
										prop.opt_val = val;
									if(xml.getParamValue(h, "key", val))
										prop.key = val;
									if(xml.getParamValue(h, "key_val", val))
										prop.key_val = val;
									if(xml.getParamValue(h, "tip", val))
										prop.tip = val;
									xml.getParamValue(h, "lval", &prop.lVal);
									if(xml.getParamValue(h, "flags", val))
										prop.flags = Utils::convertStringFlagsToLong(val, 12, stk_prop_flags);
									cfg->AddOption(&prop, true);
								}
							}
						}
						// ����� ������������
						if((hopt = xml.getNode(hcfg, "options_link")))
						{
							size_t index_lnk(0);
							while((h = xml.getNode(hopt, index_lnk++)))
							{
								PROPERTY prop;
								val = xml.getNodeName(h);
								if(val != "option")
									continue;
								if(xml.getParamValue(h, "name", val))
								{
									prop.name = val;
									if(xml.getParamValue(h, "opt", val))
										prop.opt = val;
									if(xml.getParamValue(h, "opt_val", val))
										prop.opt_val = val;
									if(xml.getParamValue(h, "key", val))
										prop.key = val;
									if(xml.getParamValue(h, "key_val", val))
										prop.key_val = val;
									if(xml.getParamValue(h, "tip", val))
										prop.tip = val;
									xml.getParamValue(h, "lval", &prop.lVal);
									if(xml.getParamValue(h, "flags", val))
										prop.flags = Utils::convertStringFlagsToLong(val, 12, stk_prop_flags);
									cfg->AddOption(&prop, false);
								}
							}
						}
					}
				}
			}
			platforms.add(plt);
		}
	}
	if((h = xml.getNode(hroot, "current_platform")))
	{
		xml.getValue(h, val);
		SetEnvVar(SF_PLATFORM_NAME, val);
	}
	if((h = xml.getNode(hroot, "current_config")))
	{
		xml.getValue(h, val);
		SetEnvVar(SF_CONFIGURATION_NAME, val);
	}
	if((h = xml.getNode(hroot, "current_element")))
	{
		xml.getValue(h, val);
		if(xml.getParamValue(h, "project", val))
		{
			CProject* prj;

			if((prj = GetProject(val)))
			{
				if((current_element = prj->GetElement(pth, NULL, NULL)))
				{
					CString pth(current_element->path);
					int pos_n(pth.ReverseFind('\\') + 1);
					int pos_e(pth.ReverseFind('.'));
					SetEnvVar(SF_DOCUMENT_PATH, pth);
					SetEnvVar(SF_DOCUMENT_NAME, pth.Mid(pos_n, pos_e - pos_n));
					SetEnvVar(SF_DOCUMENT_EXT, pth.Mid(pos_e));
					SetEnvVar(SF_DOCUMENT_MODULES_PATH, current_element->path_modules);
					SetEnvVar(SF_DOCUMENT_FILES_PATH, current_element->path_files);
				}
			}
		}
	}
	if((harch = xml.getNode(hroot, "archive")))
	{
		xml.getValue(harch, val);
		path_archive = val;
	}
	if(xml.getParamValue(hroot, "current_project", val))
		SetCurrentProject(val);
	else if(projects.size() > 0)
		SetCurrentProject(projects[0]->GetName());
	if(xml.getParamValue(hroot, "execute_project", val))
		SetExecuteProject(val);
	// ���� ��� �������� ������� ��:
	if(!current_project && projects.size() > 0)
		SetCurrentProject(projects[0]->GetName());
	// ���� ��� ������� ��� ������� - �� ���������� ������ �� ���������
	if(!execute_project && current_project)
		SetExecuteProject(current_project->GetName());
	path = pth;
	// ������� � ���������� ��������� �����
	int pos_n(path.ReverseFind('\\') + 1);
	int pos_e(path.ReverseFind('.'));
	SetEnvVar(SF_SOLUTION_NAME, path.Mid(pos_n, pos_e - pos_n));
	SetEnvVar(SF_SOLUTION_PATH, path.Left(pos_n));
	// ���������� ����� � ������� ����
	theApp.frm->SetTitle("AsmVisualStudio - [" + GetName() + "]");
	// ���������� ��������� � ������������
	theApp.frm->UpdatePlatforms();
	// ������ ����� ������ �� ��������� ��������
	if(arr.GetSize() > 0)
	{
		CShowUnOpenProjectDlg dlg("�� ��������� �������", "������ ��������:", "�������", &arr);
		is_modify = (dlg.DoModal() == IDOK);
	}
	return true;
}

bool CSolution::Save()
{
	Xml xml;
	CProject* prj;
	CPlatform* plt;
	CConfig* cfg;
	PROPERTY* prop;
	size_t i, k;
	uintptr_t hroot, hprj, hplts, hplt, hcfg, hopt, h;

	hroot = xml.addNode(xml.getRoot(), "asm_studio_solution", "");
	hprj = xml.addNode(hroot, "projects", "");
	xml.addNode(hroot, "archive", path_archive);
	// ������� � �� ������
	if(current_project)
		xml.addParam(hroot, "current_project", current_project->GetName());
	if(execute_project)
		xml.addParam(hroot, "execute_project", execute_project->GetName());
	xml.addNode(hroot, "current_platform", GetEnvVar(SF_PLATFORM_NAME));
	xml.addNode(hroot, "current_config", GetEnvVar(SF_CONFIGURATION_NAME));
	if(current_element)
	{
		if((h = xml.addNode(hroot, "current_element", current_element->path)))
			xml.addParam(h, "project", current_element->prj->GetName());
	}
	// ��������� � ������������
	if((hplts = xml.addNode(hroot, "platforms", "")))
	{
		for(i = 0 ; i < platforms.size() ; i++)
		{
			plt = platforms[i];
			if((hplt = xml.addNode(hplts, "platform", "")))
			{
				xml.addParam(hplt, "name", plt->GetName());
				size_t j(0);
				while((cfg = plt->EnumConfigs(j)))
				{
					if((hcfg = xml.addNode(hplt, "config", "")))
					{
						xml.addParam(hcfg, "include_projects", cfg->GetIncludeProjects());
						xml.addParam(hcfg, "name", cfg->GetName());
						// options_asm
						if((hopt = xml.addNode(hcfg, "options_asm", "")))
						{
							Array<PROPERTY*>* props(cfg->GetAllOptions(true));
							for(k = 0 ; k < props->size() ; k++)
							{
								prop = props->get(k);
								if((h = xml.addNode(hopt, "option", "")))
								{
									xml.addParam(h, "name", prop->name);
									if(prop->lVal != 0)
										xml.addParam(h, "lval", prop->lVal);
									if(prop->opt != "")
										xml.addParam(h, "opt", prop->opt);
									if(prop->opt_val != "")
										xml.addParam(h, "opt_val", prop->opt_val);
									if(prop->key != "")
										xml.addParam(h, "key", prop->key);
									if(prop->key_val != "")
										xml.addParam(h, "key_val", prop->key_val);
									xml.addParam(h, "tip", prop->tip);
									xml.addParam(h, "flags", Utils::convertLongFlagsToString(prop->flags, 12, stk_prop_flags, false));
								}
							}
						}
						// options_link
						if((hopt = xml.addNode(hcfg, "options_link", "")))
						{
							Array<PROPERTY*>* props(cfg->GetAllOptions(false));
							for(k = 0 ; k < props->size() ; k++)
							{
								prop = props->get(k);
								if((h = xml.addNode(hopt, "option", "")))
								{
									xml.addParam(h, "name", prop->name);
									if(prop->lVal != 0)
										xml.addParam(h, "lval", prop->lVal);
									if(prop->opt != "")
										xml.addParam(h, "opt", prop->opt);
									if(prop->opt_val != "")
										xml.addParam(h, "opt_val", prop->opt_val);
									if(prop->key != "")
										xml.addParam(h, "key", prop->key);
									if(prop->key_val != "")
										xml.addParam(h, "key_val", prop->key_val);
									xml.addParam(h, "tip", prop->tip);
									xml.addParam(h, "flags", Utils::convertLongFlagsToString(prop->flags, 12, stk_prop_flags, false));
								}
							}
						}
					}
				}
			}
		}
	}
	// ������ ��������
	for(i = 0 ; i < projects.size() ; i++)
	{
		// ��������� ������
		prj = projects[i];
		prj->Save();
		xml.addNode(hprj, "project", prj->GetPath());
	}
	is_modify = false;
	return xml.save(path);
}

CProject* CSolution::AddProject(const char* nm_sln, const char* nm_prj, const char* pth, WIZARD* wzd, int act)
{
	bool result = false;

	CString path_prj(pth), msg_err;
	CString name_prj(nm_prj);
	CString name_sln(nm_sln);
	CPlatform* plt;

	CProject* p(new CProject());
	bool is_old_modify = is_modify;

	if(!wzd) wzd = &default_wizard;

	switch(act)
	{
		case ACTION_NEW_PROJECT:
			// ������� ����� ������/�������
			path_prj += name_sln + "\\";
			path = path_prj + name_sln + EXT_SLN;
			// ������� ���� � �������
			path_prj += name_prj + "\\";
			path_prj += name_prj + EXT_PRJ;
			// ������� ������
			result = p->Create(path_prj, name_prj, wzd);
			break;
		case ACTION_ADD_NEW_PROJECT:
			// ������� ���� � ������� �� ���� � �������
			path_prj = Utils::extractPathFromFullPath(path, TRUE);
			path_prj += name_prj + "\\";
			path_prj += name_prj + EXT_PRJ;
			// ������� ������
			result = p->Create(path_prj, name_prj, wzd);
			break;
		case ACTION_ADD_OPEN_PROJECT:
			// ������� ������
			result = p->Open(path_prj);
			name_prj = p->GetName();
			break;
		case ACTION_OPEN_PROJECT:
			// ������� ���� � �������
			path = Utils::extractPathFromFullPath(path_prj, TRUE);
			// ������� ������
			result = p->Open(path_prj);
			name_prj = p->GetName();
			path += name_prj + EXT_SLN;
			break;
	}
	if(result)
	{
		result = !GetProject(name_prj);
		if(!result)
		{
			msg_err = "������. ������ \"" + name_prj + "\" ��� ���� � �������!";
		}
		else
		{
			// ���������� ������
			if(wzd == &default_wizard && p->name_wizard != "")
			{
				wzd = theApp.frm->GetConfiguration()->GetWizard(p->name_wizard);
				if(!wzd) wzd = &default_wizard;
			}
			// �������� ���������
			if((plt = AddPlatform(wzd)))
			{
				// ���������� ������ � ������� ����������� ��������� � �� ������������
				CConfig* cfg;
				size_t icfg(0);
				while((cfg = plt->EnumConfigs(icfg)))
					cfg->SetProjectExclude(p, FALSE);
			}
			// ���������� ��� ��� �������
			projects.add(p);
			SetCurrentProject(name_prj);
			is_modify = true;
			// ���������, ���� ���������� ����� ������/�������
			if(act == ACTION_NEW_PROJECT || act == ACTION_OPEN_PROJECT)
				Save();
			theApp.frm->UpdatePlatforms();
		}
	}
	if(!result)
	{
		if(msg_err == "")
		{
			switch(act)
			{
				case ACTION_NEW_PROJECT:
					msg_err = "������. �� ������� ������� ������ \"" + name_prj + "\" !";
					break;
				case ACTION_ADD_NEW_PROJECT:
				case ACTION_ADD_OPEN_PROJECT:
					msg_err = "������. �� ������� �������� ����� ������ \"" + name_prj + "\" � ������� !";
					break;
				case ACTION_OPEN_PROJECT:
					msg_err = "������. �� ������� ������� ������ \"" + name_prj + "\" � ������� !";
					break;
			}
		}
		MessageBox(NULL, msg_err, ASM_STUDIO, MB_ICONERROR);
		delete p ; p = NULL;
		// ������������ ������ ������� � ���������� ���������
		is_modify = is_old_modify;
		p = current_project;
		current_project = NULL;
		SetCurrentProject(p ? p->GetName() : "");
	}

	return p;
}

bool CSolution::DeleteProject(const char* name)
{
	int index;
	CConfig* cfg;
	CProject* prj;
	if(!(prj = GetProject(name, &index)))
		return false;
	// ������� ��� �� ���� �������� � ������������
	for(size_t i = 0 ; i < platforms.size() ; i++)
	{
		CPlatform* plt(platforms[i]);
		size_t icfg(0);
		while((cfg = plt->EnumConfigs(icfg)))
			cfg->SetProjectExclude(prj, TRUE);
	}
	if(current_project)
	{
		if(current_project->GetName() == name)
		{
			current_project = NULL;
			SetEnvVar(SF_PROJECT_NAME, "");
			SetEnvVar(SF_PROJECT_PATH, "");
		}
	}
	if(execute_project)
	{
		if(execute_project->GetName() == name)
		{
			execute_project = NULL;
			SetEnvVar(SF_PROJECT_EXECUTE_PATH, "");
			SetEnvVar(SF_PROJECT_EXECUTE_NAME, "");
		}
	}
	projects.del(index);
	is_modify = true;

	return true;
}

CProject* CSolution::SetCurrentProject(const char* name)
{
	CProject* prj(GetProject(name));
	if(prj && prj != current_project)
	{
		current_project = prj;
		SetEnvVar(SF_PROJECT_NAME, prj->GetName());
		SetEnvVar(SF_PROJECT_PATH, Utils::extractPathFromFullPath(prj->GetPath(), FALSE));
		SetEnvVar(SF_CONFIGURATION_PATH, GetEnvVar(SF_PROJECT_PATH) + "\\" + GetEnvVar(SF_CONFIGURATION_NAME));
	}
	return current_project;
}

void CSolution::SetExecuteProject(const char* name)
{
	CProject* p(GetProject(name));
	if(p != execute_project)
	{
		if(p)
		{
			SetEnvVar(SF_PROJECT_EXECUTE_NAME, p->GetName());
			SetEnvVar(SF_PROJECT_EXECUTE_PATH, Utils::extractPathFromFullPath(p->GetPath(), FALSE));
		}
		execute_project = p;
	}
}

void CSolution::ReorderProjects(const Array<CString>& arr)
{
	Array<CProject*> prj;
	size_t i;

	for(i = 0 ; i < arr.size() ; i++)
		prj.add(GetProject(arr[i]));
	projects.removeAll();
	for(i = 0 ; i < prj.size() ; i++)
	{
		CProject* p(prj[i]);
		projects.add(p);
	}
	is_modify = true;
}

void CSolution::Process(const char* cmd_line, HANDLE hOut, STARTUPINFO* si)
{
	static CString env;
	// ��� ���������� ��������
	DWORD exit(STILL_ACTIVE), bread, avail;
	// ��������� ���������� � �������� �������
	PROCESS_INFORMATION pi;
	// ����� �����/������
	char buffer[4097];
	// ������ �����
	Array<String> arr_out;
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	/*
	out->AddString("������ �������:");
	out->AddString(cmd_line);
	out->AddString("");
	*/
	// ��������� �������
	CString inc_path("INCLUDE=" + theApp.frm->GetConfiguration()->GetDefaultIncludeFolder() + "#");
	CString lib_path("LIB=" + theApp.frm->GetConfiguration()->GetDefaultLibraryFolder() + "#");
	env = (inc_path + lib_path);
	env.Replace('#', 0);
	if(CreateProcess(NULL, (char*)(const char*)cmd_line, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, (void*)(const char*)env, GetEnvVar(SF_CONFIGURATION_PATH) + "\\", si, &pi))
	{
		while(exit == STILL_ACTIVE)
		{
			// ���� �������� ������� �� ������
			GetExitCodeProcess(pi.hProcess, &exit); 
			PeekNamedPipe(hOut, buffer, 4095, &bread, &avail, NULL);
			// ���������, ���� �� ������ ��� ������ � stdout
			if(bread)
			{
				Sleep(10);
				ReadFile(hOut, buffer, bread, &bread, NULL);
				buffer[bread] = 0;
				// 8. ������� � ���� ������
				Utils::explode("\r\n", buffer, arr_out);
				for(size_t j = 0 ; j < arr_out.size() ; j++)
					out->AddString(arr_out[j]);
			}
		}
		// ������� �������
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}

CString CSolution::EnumElementsPath(bool is_asm, CProject* prj)
{
	CString pth(" ");
	size_t index(0);
	CElement* elem;

	while((elem = prj->EnumElements(index)))
	{
		if(!elem->IsExclude())
		{
			CString ext(Utils::extractFileExtFromFullPath(elem->path, TRUE));
			if(ext.MakeLower() == ".asm")
			{
				if(is_asm)
				{
					pth += "\"" + elem->path + "\" ";
				}
				else
				{
					CString name(Utils::extractFileTitleFromFullPath(elem->path));
					pth += "\"" + name + ".obj\" ";
				}
			}
		}
	}

	return pth;
}

static DWORD WINAPI ThreadBuildProc(void* param)
{
	// ��������� ��� ������� �������
	STARTUPINFO si;
	// ��������� ��������� ������������
	SECURITY_ATTRIBUTES sa;
	// ��������� ������������ ������
	SECURITY_DESCRIPTOR sd; 

	size_t index(0), nProjects(0);
	HANDLE hStdOut, hnStdOut;

	CPlatform* plt;
	CConfig* cfg;
	CProject* project;

	CSolution* sln(theApp.frm->GetSolution());
	// ��������� ������� ���������/�������
	plt = sln->GetPlatform(sln->GetCurrentPlatform());
	if(plt && ((cfg = plt->GetConfig(sln->GetCurrentConfig()))))
	{
		// ������������� ����������� ������������
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, true, NULL, false);
		sa.lpSecurityDescriptor = &sd;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		// ��������� ������������ ������������
		sa.bInheritHandle = true;
		// ������������� ��������� ��� ������� �������
		GetStartupInfo(&si); 
		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		// ������� �����
		if(CreatePipe(&hStdOut, &hnStdOut, &sa, 0)) 
		{
			if(param)
			{
				project = (CProject*)param;
				index = -1;
			}
			else
			{
				project = sln->EnumProjects(index);
			}
			// ��������� ����������� ��� ��������� ��������
			si.hStdOutput = hnStdOut;
			si.hStdError = hnStdOut;
			do
			{
				if(!cfg->IsProjectExclude(project))
				{
					nProjects++;
					// ������ �� ��������� ����� ����������/����������
					CString asm_cmd(ApplyFilter(cfg->GetOptions(true)));
					CString lnk_cmd(ApplyFilter(cfg->GetOptions(false)));
					if(asm_cmd != "")
					{
						// ������� ����������, ��� ���������� ����������
						asm_cmd += sln->EnumElementsPath(true, project);
						sln->Process(asm_cmd, hStdOut, &si);
					}
					if(lnk_cmd != "")
					{
						// ������� �����������, ��� ���������� ����������
						lnk_cmd += sln->EnumElementsPath(false, project);
						sln->Process(lnk_cmd, hStdOut, &si);
					}
				}
			} while((project = sln->EnumProjects(index)));
			// ������� ����
			CloseHandle(hnStdOut);
			CloseHandle(hStdOut);
		}
	}
	// ����������
	sln->FinishBuild(project, nProjects);

	return 0;
}

void CSolution::BeginBuild(CProject* prj)
{
	CDocumentCommon* doc;
	CString pth;
	Array<CDocumentCommon*> docs;
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	// �������, ���� ����������,  ����� ��� ������������
	Utils::createPath(GetEnvVar(SF_CONFIGURATION_PATH), false);
	// ���������� ������� ������
	theApp.frm->SetBuildFlag(true);
	// ��������� ��� �������� � ���������������� ���������
	theApp.frm->GetAllDocuments(&docs, NULL, prj, false, false, false);
	for(size_t i = 0 ; i < docs.size() ; i++)
	{
		doc = docs[i];
		if(doc->IsModified())
			doc->OnDocumentSave();
	}
	// ���������� ������ ������
	if(prj)
	{
		pth = "������ �������: " + prj->GetName();
	}
	else
	{
		pth = "������ ������ �������: " + GetName();
	}
	out->Reset();
	out->SetTitle(pth + ", ������������: " + GetEnvVar(SF_CONFIGURATION_NAME) + " " + GetEnvVar(SF_PLATFORM_NAME));
	theApp.frm->SendMessage(WM_BUILD_STATUS_BAR_MSG, (WPARAM)(prj ? "������ �������..." : "������ �������..."));
}

void CSolution::FinishBuild(CProject* prj, int nProjects)
{
	CString str;
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	CPanelTaskView* task(theApp.frm->GetPanelTask());
	// ���������� ���������� �������������� � ������
	nErrors = task->UpdateList(CPanelTaskView::modeError, false);
	long warnings = task->UpdateList(CPanelTaskView::modeWarning, false);
	// ������� ��������� ������
	str.Format("������ %s ��������� %s: �������� - %d, ��������� - %d, ������ - %d, �������������� - %d", (prj ? "�������" : "�������"), (nErrors > 0 ? "��������" : "�������"), nProjects, theApp.frm->GetSolution()->GetCountProjects() - nProjects, nErrors, warnings);
	out->AddString("");
	out->AddString(str);
	out->AddString("");
	// �������� ������� ������
	theApp.frm->SetBuildFlag(false);
	theApp.frm->SendMessage(WM_BUILD_STATUS_BAR_MSG, NULL);
}

bool CSolution::BuildProject(CProject* prj)
{
	// ������������� ������
	BeginBuild(prj);
	// ��������� �������� ������ � �������������� � ��������� �����
	return (CreateThread(NULL, 0, ThreadBuildProc, (void*)prj, 0, NULL) != NULL);
}

bool CSolution::Build()
{
	// ������������� ������
	BeginBuild(NULL);
	// ��������� �������� ������ � �������������� � ��������� �����
	return (CreateThread(NULL, 0, ThreadBuildProc, NULL, 0, NULL) != NULL);
}

bool CSolution::Rebuild()
{
	WIN32_FIND_DATA wfd;
	HANDLE hFile;
	// ������� ��
	CString folder(GetEnvVar(SF_CONFIGURATION_PATH));
	// ����������� � ������� ��� ����� � ���� �����
	if((hFile = FindFirstFile(folder + "\\*.*", &wfd)) != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if(wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_VIRTUAL | FILE_ATTRIBUTE_HIDDEN))
				continue;
			::DeleteFile(folder + "\\" + wfd.cFileName);
		} while(FindNextFile(hFile, &wfd));
	}
	// ������� ����������� ����
	::DeleteFile(GetCurrentSpecificOption(FOPT_EXECUTE));
	// ��������� ������
	return Build();
}

DWORD WINAPI ThreadExecuteProc(void* param)
{
	CSolution* sln(theApp.frm->GetSolution());
	// �������� ���������� ������
	while(theApp.frm->IsProcessBuild()) {Sleep(100);}
	// �������� �� ��������� ������
	if(sln->nErrors > 0)
	{
		if(MessageBox(NULL, "������ ����������� ��������. �� ����� ����������?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) != IDYES)
			return 0;
	}
	// ��������� ��� ������� �������
	STARTUPINFO si;
	// ��������� ���������� � �������� �������
	PROCESS_INFORMATION pi;
	// �������� ��������� ������� �� ���������
	GetStartupInfo(&si);
	// ������ ��������
	if(!CreateProcess(NULL, (char*)(const char*)param, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		MessageBox(NULL, "������. �� ������� ��������� �������!", ASM_STUDIO, MB_ICONERROR);
	return 0;
}

bool CSolution::Execute()
{
	static CString cmd;
	CString cmd_prg, cmd_params;
	Array<CDocumentCommon*> docs;
	// 1. ��������� �� ������� ���������������� �������� ����������
	CString msg;
	theApp.frm->GetAllDocuments(&docs, NULL, execute_project, false, true, false);
	for(size_t i = 0 ; i < docs.size() ; i++)
	{
		if(docs[i]->IsModified())
		{
			msg = "��������� ��������� ������������ ������� ��������������. �������������� ��������� ������ �������?";
			break;
		}
	}
	// 2. ��������� �� ������� ������ ����� ��������� ������
	if(nErrors > 0 || msg != "")
	{
		if(msg == "") msg = "���������� ������ ���� ��������� ��������. ��������� �� ��������?";
		// 3. ������ �� ������
		if(MessageBox(NULL, msg, ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDYES)
			theApp.frm->SendMessage(WM_COMMAND, ID_REBUILD);
	}
	// 4. ������ �������
	if((cmd_prg = GetCurrentSpecificOption(FOPT_EXECUTE)) == "")
	{
		MessageBox(NULL, "������. �� ������� ����� ������ �������!", ASM_STUDIO, MB_ICONERROR);
		return false;
	}
	cmd = "\"" + cmd_prg + "\" " + execute_project->GetStartParams();
	// 5. �������� ������ � � ��� ��������
	return (CreateThread(NULL, 0, ThreadExecuteProc, (void*)(const char*)cmd, 0, NULL) != NULL);
}

CString CSolution::GetCurrentSpecificOption(long flags)
{
	CPlatform* plt;
	CConfig* cfg;
	PROPERTY* prop;

	if(!(plt = GetPlatform(GetCurrentPlatform())))
		return NULL;
	if(!(cfg = plt->GetConfig(GetCurrentConfig())))
		return NULL;
	if(!(prop = cfg->GetOption(false, NULL, flags)))
	{
		if(!(prop = cfg->GetOption(true, NULL, flags)))
			return NULL;
	}

	return ApplyFilter(prop->opt);
}

bool CSolution::Close()
{
	if(IsModify())
	{
		switch(MessageBox(NULL, "������� ���������������. ��������� ���������?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNOCANCEL))
		{
			case IDYES:
				Save();
				break;
			case IDCANCEL:
				return false;
		}
	}
	Init();
	return true;
}

CPlatform* CSolution::AddPlatform(WIZARD* wzd)
{
	CPlatform* plt(NULL);
	WIZARD_CONFIG* wcfg;
	CConfig* cfg(NULL);
	if(wzd)
	{
		if(!(plt = GetPlatform(wzd->name_platform)))
		{
			plt = new CPlatform(wzd->name_platform, this);
			// �������� ��� ������� ���� ���������
			for(size_t i = 0 ; i < wzd->configs.size() ; i++)
			{
				wcfg = wzd->configs[i];
				CConfig* c(plt->AddConfig(wcfg->name, wcfg->asm_props, wcfg->lnk_props));
				if(c && !cfg) cfg = c;
			}
			platforms.add(plt);
		}
		// ����� ���������� ��� ��������� ��� �������
		SetEnvVar(SF_PLATFORM_NAME, wzd->name_platform);
		if(cfg) SetEnvVar(SF_CONFIGURATION_NAME, cfg->GetName());
		theApp.frm->OnPlatforms();
		is_modify = true;
	}

	return plt;
}

bool CSolution::DeletePlatform(const char* name)
{
	size_t index;
	if(!GetPlatform(name, &index))
		return false;
	platforms.del(index);
	is_modify = true;

	return true;
}

CPlatform* CSolution::GetPlatform(const char* name, size_t* index)
{
	if(name)
	{
		for(size_t i = 0 ; i < platforms.size() ; i++)
		{
			CPlatform* plt(platforms[i]);
			if(plt->GetName() == name)
			{
				if(index) *index = i;
				return plt;
			}
		}
	}

	return NULL;
}

void CSolution::MiniSave()
{
	Xml xml;
	CProject* prj;
	String val;
	uintptr_t hroot, hprj, harch, h;

	if(xml.open(path))
	{
		if((hroot = xml.getNode(xml.getRoot(), "asm_studio_solution")))
		{
			if((hprj = xml.getNode(hroot, "projects")))
			{
				size_t iprj(0);
				// ������� ��������/��������/������� ������� � ����������
				while((h = xml.getNode(hprj, iprj++)))
				{
					val = xml.getNodeName(h);
					if(val != "project")
						continue;
					xml.getValue(h, val);
					if((prj = GetProject(val)))
						prj->MiniSave();
				}
			}
			// ������� ������������/���������
			if((h = xml.getNode(hroot, "current_platform")))
				xml.setValue(h, GetEnvVar(SF_PLATFORM_NAME));
			if((h = xml.getNode(hroot, "current_config")))
				xml.setValue(h, GetEnvVar(SF_CONFIGURATION_NAME));
			// ������� ��������
			if((h = xml.getNode(hroot, "current_element")))
				xml.setValue(h, GetEnvVar(SF_DOCUMENT_PATH));
			if((harch = xml.getNode(hroot, "archive")))
				xml.setValue(harch, path_archive);
			val = (current_project ? current_project->GetName() : "");
			xml.setParamValue(hroot, "current_project", val);
			val = (execute_project ? execute_project->GetName() : "");
			xml.setParamValue(hroot, "execute_project", val);
			xml.save(path);
		}
	}
}

CString CSolution::GetName()
{
	int pos(path.ReverseFind('\\') + 1);
	int pos1(path.ReverseFind('.'));
	return path.Mid(pos, pos1 - pos);
}