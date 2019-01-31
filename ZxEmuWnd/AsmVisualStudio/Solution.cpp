
#include "stdafx.h"
#include "AsmVisualStudio.h"
#include "Project.h"
#include "ShowUnOpenProjectDlg.h"

static CString str_def_wizard = "<?xml version = \"1.0\" encoding = \"windows-1251\" ?>\
<elements>\
<element path = \"$(MainDir)\\resources\\default_wizard.asm\" path_file = \"$(ProjectName)Main.asm\" path_modules = \"Базовые\\$(ProjectName)Main\" is_exclude = \"0\"/>\
</elements>\
<configurations>\
<config name = \"Debug\">\
<options_asm>\
<option name = \"Транслятор\" flags = \"path|assembler\" opt = \"$(MainDir)\\Bin\\ml.exe\" opt_val = \"Исполняемый файл транслятора|*.exe|\" tip = \"Размещение транслятора\"/>\
<option name = \"Игнорировать Include путь\" key = \"/X\" flags = \"bool\" tip = \"Игнорировать глобальный путь к включаемым файлам\"/>\
<option name = \"Предупреждения как ошибки\" key = \"/WX\" flags = \"bool\" tip = \"Расценивать предупреждения как ошибки\"/>\
<option name = \"Уровень предупреждений\" key = \"/W\" flags = \"select\" opt = \"Уровень 0\" opt_val = \"Уровень 0,Уровень 1,Уровень 2,Уровень 3,Уровень 4\" key_val = \"0,1,2,3,4\" tip = \"Установить уровень предупреждений\"/>\
<option name = \"Выравнивание структур\" key = \"/Zp\" flags = \"select\" opt = \"4\" opt_val = \"4,8,16\" key_val = \"4,8,16\" tip = \"Установить выравнивание структур\"/>\
<option name = \"Регистр идентификаторов\" key = \"/Cp\" lval = \"1\" flags = \"bool\" tip = \"Сохранить регистр идентификаторов пользователя\"/>\
<option name = \"Идентификаторы в верхний регистр\" key = \"/Cu\" flags = \"bool\" tip = \"Преобразовать все идентификаторы в верхний регистр\"/>\
<option name = \"Регистр внешних зависимостей\" key = \"/Cx\" lval = \"1\" flags = \"bool\" tip = \"Сохранить регистр в publics externs\"/>\
<option name = \"COFF формат объектного файла\" key = \"/coff\" flags = \"bool\" tip = \"Генерировать объектный файл в формате COFF\"/>\
<option name = \"Текст макроса\" key = \"/D\" flags = \"string\" opt = \"DEBUG\" tip = \"Определить текст макроса\"/>\
<option name = \"Размер стека\" key = \"/F\" flags = \"number\" tip = \"Устанавливает размер стека (в байтах)\"/>\
<option name = \"Файл листинга\" key = \"/Fl\" flags = \"string\" tip = \"Сгенерировать файл листинга\"/>\
<option name = \"Тайминги в листинг\" key = \"/Sc\" flags = \"bool\" tip = \"Показывать тайминги в листинг\"/>\
<option name = \"Листинг символьной таблицы\" key = \"/Sn\" flags = \"bool\" tip = \"Подавить листинг таблицы символов\"/>\
<option name = \"Файл карты\" key = \"/Fm\" flags = \"string\" tip = \"Сгенерировать файл карты памяти\"/>\
<option name = \"Спецификатор вызова\" key = \"/G\" flags = \"select\" opt = \"stdcall\" opt_val = \"pascal,c,stdcall\" key_val = \"c,d,z\" tip = \"Тип вызова процедур\"/>\
<option name = \"Дополнительный Include путь\" key = \"/I\" flags = \"folder\" tip = \"Дополнительный путь к включаемым файлам\"/>\
<option name = \"Логотип\" always = \"1\" key = \"/nologo\" flags = \"bool\" tip = \"Отображать логотип\"/>\
<option name = \"Все символы общедоступные\" key = \"/Zf\" flags = \"bool\" tip = \"Сделать все символы общедоступными\"/>\
<option name = \"Номера строк в отладку\" key = \"/Zd\" flags = \"bool\" tip = \"Добавить номера строк в отладочную информацию\"/>\
<option name = \"Символы в отладку\" key = \"/Zi\" lval = \"1\" flags = \"bool\" tip = \"Добавлять отладочную информацию об символах\"/>\
<option name = \"Дополнительно\" flags = \"string\" tip = \"Дополнительные опции\"/>\
</options_asm>\
<options_link>\
<option name = \"Компоновщик\" flags = \"path|linker\" opt = \"$(MainDir)\\Bin\\link.exe\" opt_val = \"Исполняемый файл компоновщика|*.exe|\" tip = \"Размещение компоновщика\"/>\
<option name = \"Имя файла\" key = \"/OUT:\" flags = \"string|execute\" opt = \"$(ProjectPath)\\$(ConfigurationName)\\$(ProjectName).exe\" tip = \"Установить имя выходного файла.\"/>\
<option name = \"Уровень предупреждений\" key = \"/WARN:\" flags = \"select\" opt = \"Уровень 0\" opt_val = \"Уровень 0,Уровень 1,Уровень 2,Уровень 3\" key_val = \"0,1,2,3\" tip = \"Уровень предупреждений выдаваемых компоновщиком.\"/>\
<option name = \"Используемые библиотеки\" flags = \"string\" opt = \"kernel32.lib user32.lib gdi32.lib\" tip = \"Библиотеки по умолчанию\"/>\
<option name = \"Значение выравнивания секций\" key = \"/ALIGN:\" lval = \"4096\" flags = \"number\" tip = \"Выравнивание секций в линейной модели.\"/>\
<option name = \"Отладка\" key = \"/DEBUG\" lval = \"1\" flags = \"bool\" tip = \"Создавать отладочную информацию. Отладочная информация помещается в pdb-файл.\"/>\
<option name = \"Тип отладки\" key = \"/DEBUGTYPE:\" flags = \"select\" opt = \"COFF\" opt_val = \"CV,COFF,BOTH\" key_val = \"CV,COFF,BOTH\" tip = \"Тип отладочной информации\"/>\
<option name = \"Библиотека импорта\" key = \"/IMPLIB:\" flags = \"string\" tip = \"Установить имя import-библиотеки.\"/>\
<option name = \"Файл определений\" key = \"/DEF:\" flags = \"string\" tip = \"Имя файла определений.\"/>\
<option name = \"Библиотека по умолчанию\" key = \"/DEFAULTLIB:\" flags = \"string\" tip = \"Добавить библиотеку по умолчанию.\"/>\
<option name = \"Точка входа\" key = \"/ENTRY:\" flags = \"string\" tip = \"Установить точку входа.\"/>\
<option name = \"Экспортируемые функции\" key = \"/EXPORT:\" flags = \"string\" tip = \"Установить имена функций для экспорта.\"/>\
<option name = \"Базовый адрес\" key = \"/BASE:\" flags = \"number\" tip = \"Определяет базовый адрес (адрес загрузки).\"/>\
<option name = \"Адреса более 2 ГБ\" key = \"/LARGEADDRESSAWARE\" flags = \"bool\" tip = \"Приложение оперирует адресами большими 2 Гб.\"/>\
<option name = \"Файл карты\" key = \"/MAP:\" flags = \"string\" tip = \"Создавать файл карты.\"/>\
<option name = \"Размер хипа\" key = \"/HEAP:\" flags = \"number\" tip = \"Установить размер кучи (HEAP) в байтах.\"/>\
<option name = \"Тип файла карты\" key = \"/MAPINFO:\" flags = \"select\" opt = \"EXPORTS\" opt_val = \"EXPORTS,FIXUPS,LINES\" key_val = \"EXPORTS,FIXUPS,LINES\" tip = \"Тип включаемой информации в файл карты.\"/>\
<option name = \"Игнорировать библиотеки\" key = \"/NODEFAULTLIB:\" flags = \"string\" tip = \"Игнорировать все или конкретную библиотеку.\"/>\
<option name = \"Логотип\" always = \"1\" key = \"/NOLOGO\" flags = \"bool\" tip = \"Вывод логотипа.\"/>\
<option name = \"Размер стека\" key = \"/STACK:\" flags = \"number\" tip = \"Установить размер стека.\"/>\
<option name = \"Файл заглушки\" key = \"/STUB:\" flags = \"string\" tip = \"Установить файл - заглушку в системе MS DOS.\"/>\
<option name = \"Система\" key = \"/SUBSYSTEM:\" flags = \"select\" opt = \"WINDOWS\" opt_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" key_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" tip = \"Тип запуска приложения.\"/>\
<option name = \"Дополнительно\" flags = \"string\" tip = \"Дополнительные опции\"/>\
</options_link>\
</config>\
<config name = \"Release\">\
<options_asm>\
<option name = \"Транслятор\" flags = \"path\" opt = \"$(MainDir)\\Bin\\ml.exe\" opt_val = \"Исполняемый файл транслятора|*.exe|\" tip = \"Размещение транслятора\"/>\
<option name = \"Игнорировать Include путь\" key = \"/X\" flags = \"bool\" tip = \"Игнорировать глобальный путь к включаемым файлам\"/>\
<option name = \"Предупреждения как ошибки\" key = \"/WX\" flags = \"bool\" tip = \"Расценивать предупреждения как ошибки\"/>\
<option name = \"Уровень предупреждений\" key = \"/W\" flags = \"select\" opt = \"Уровень 0\" opt_val = \"Уровень 0,Уровень 1,Уровень 2,Уровень 3,Уровень 4\" key_val = \"0,1,2,3,4\" tip = \"Установить уровень предупреждений\"/>\
<option name = \"Выравнивание структур\" key = \"/Zp\" flags = \"select\" opt = \"4\" opt_val = \"4,8,16\" key_val = \"4,8,16\" tip = \"Установить выравнивание структур\"/>\
<option name = \"Регистр идентификаторов\" key = \"/Cp\" lval = \"1\" flags = \"bool\" tip = \"Сохранить регистр идентификаторов пользователя\"/>\
<option name = \"Идентификаторы в верхний регистр\" key = \"/Cu\" flags = \"bool\" tip = \"Преобразовать все идентификаторы в верхний регистр\"/>\
<option name = \"Регистр внешних зависимостей\" key = \"/Cx\" lval = \"1\" flags = \"bool\" tip = \"Сохранить регистр в publics externs\"/>\
<option name = \"COFF формат объектного файла\" key = \"/coff\" flags = \"bool\" tip = \"Генерировать объектный файл в формате COFF\"/>\
<option name = \"Текст макроса\" key = \"/D\" flags = \"string\" tip = \"Определить текст макроса\"/>\
<option name = \"Размер стека\" key = \"/F\" flags = \"number\" tip = \"Устанавливает размер стека (в байтах)\"/>\
<option name = \"Файл листинга\" key = \"/Fl\" flags = \"string\" tip = \"Сгенерировать файл листинга\"/>\
<option name = \"Тайминги в листинг\" key = \"/Sc\" flags = \"bool\" tip = \"Показывать тайминги в листинг\"/>\
<option name = \"Листинг символьной таблицы\" key = \"/Sn\" flags = \"bool\" tip = \"Подавить листинг таблицы символов\"/>\
<option name = \"Файл карты\" key = \"/Fm\" flags = \"string\" tip = \"Сгенерировать файл карты памяти\"/>\
<option name = \"Спецификатор вызова\" key = \"/G\" flags = \"bool\" opt = \"stdcall\" opt_val = \"pascal,c,stdcall\" key_val = \"c,d,z\" tip = \"Тип вызова процедур\"/>\
<option name = \"Дополнительный Include путь\" key = \"/I\" flags = \"folder\" tip = \"Дополнительный путь к включаемым файлам\"/>\
<option name = \"Логотип\" always = \"1\" key = \"/nologo\" flags = \"bool\" tip = \"Отображать логотип\"/>\
<option name = \"Все символы общедоступные\" key = \"/Zf\" flags = \"bool\" tip = \"Сделать все символы общедоступными\"/>\
<option name = \"Дополнительно\" flags = \"string\" tip = \"Дополнительные опции\"/>\
</options_asm>\
<options_link>\
<option name = \"Компоновщик\" flags = \"path\" opt = \"$(MainDir)\\Bin\\link.exe\" opt_val = \"Исполняемый файл компоновщика|*.exe|\" tip = \"Размещение компоновщика\"/>\
<option name = \"Имя файла\" key = \"/OUT:\" flags = \"string\" opt = \"$(ProjectPath)\\$(ConfigurationName)\\$(ProjectName).exe\" tip = \"Установить имя выходного файла.\"/>\
<option name = \"Путь к библиотекам\" key = \"/LIBPATH:\" flags = \"folder\" opt = \"$(PathLib)\" tip = \"Установить путь к библиотекам.\"/>\
<option name = \"Уровень предупреждений\" key = \"/WARN:\" flags = \"select\" opt = \"Уровень 0\" opt_val = \"Уровень 0,Уровень 1,Уровень 2,Уровень 3\" key_val = \"0,1,2,3\" tip = \"Уровень предупреждений выдаваемых компоновщиком.\"/>\
<option name = \"Используемые библиотеки\" flags = \"string\" opt = \"kernel32.lib user32.lib gdi32.lib\" tip = \"Библиотеки по умолчанию\"/>\
<option name = \"Значение выравнивания секций\" key = \"/ALIGN:\" lval = \"4096\" flags = \"number\" tip = \"Выравнивание секций в линейной модели.\"/>\
<option name = \"Библиотека импорта\" key = \"/IMPLIB:\" flags = \"string\" tip = \"Установить имя import-библиотеки.\"/>\
<option name = \"Файл определений\" key = \"/DEF:\" flags = \"string\" tip = \"Имя файла определений.\"/>\
<option name = \"Библиотека по умолчанию\" key = \"/DEFAULTLIB:\" flags = \"string\" tip = \"Добавить библиотеку по умолчанию.\"/>\
<option name = \"Точка входа\" key = \"/ENTRY:\" flags = \"string\" tip = \"Установить точку входа.\"/>\
<option name = \"Экспортируемые функции\" key = \"/EXPORT:\" flags = \"string\" tip = \"Установить имена функций для экспорта.\"/>\
<option name = \"Наращиваемая компоновка\" key = \"/INCREMENTAL\" flags = \"bool\" tip = \"При выборе добавляется дополнительная информация позволяющая быстрее перекомпилировать этот файл.\"/>\
<option name = \"Базовый адрес\" key = \"/BASE:\" flags = \"number\" tip = \"Определяет базовый адрес (адрес загрузки).\"/>\
<option name = \"Адреса более 2 ГБ\" key = \"/LARGEADDRESSAWARE\" flags = \"bool\" tip = \"Приложение оперирует адресами большими 2 Гб.\"/>\
<option name = \"Файл карты\" key = \"/MAP:\" flags = \"string\" tip = \"Создавать файл карты.\"/>\
<option name = \"Размер хипа\" key = \"/HEAP:\" flags = \"number\" tip = \"Установить размер кучи (HEAP) в байтах.\"/>\
<option name = \"Тип файла карты\" key = \"/MAPINFO:\" flags = \"select\" opt = \"EXPORTS\" opt_val = \"EXPORTS,FIXUPS,LINES\" key_val = \"EXPORTS,FIXUPS,LINES\" tip = \"Тип включаемой информации в файл карты.\"/>\
<option name = \"Игнорировать библиотеки\" key = \"/NODEFAULTLIB\" flags = \"string\" tip = \"Игнорировать все или конкретную библиотеку.\"/>\
<option name = \"Логотип\" always = \"1\" key = \"/NOLOGO\" flags = \"bool\" tip = \"Вывод логотипа.\"/>\
<option name = \"Размер стека\" key = \"/STACK:\" flags = \"number\" tip = \"Установить размер стека.\"/>\
<option name = \"Файл заглушки\" key = \"/STUB:\" flags = \"string\" tip = \"Установить файл - заглушку в системе MS DOS.\"/>\
<option name = \"Система\" key = \"/SUBSYSTEM:\" flags = \"select\" opt = \"WINDOWS\" opt_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" key_val = \"NATIVE,WINDOWS,CONSOLE,WINDOWSCE,POSIX\" tip = \"Тип запуска приложения.\"/>\
<option name = \"Дополнительно\" flags = \"string\" tip = \"Дополнительные опции\"/>\
</options_link>\
</config>\
</configurations>";

extern SOLUTION_FILTER arr_env_var[SF_LAST_ENV_VAR];

CSolution::CSolution()
{
	String temp_dir, work_dir, main_dir, sys_dir, name_prg;
	Xml xml;
	// сброс
	Init();
	// устанавливаем переменные окружения среды 
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
	// создаем мастера по умолчанию
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
		// элементы
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
		// конфигурации
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
	// платформы
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
						// опции траслятора
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
						// опции компоновщика
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
	// если нет текущего проекта то:
	if(!current_project && projects.size() > 0)
		SetCurrentProject(projects[0]->GetName());
	// если нет проекта для запуска - то установить проект по умолчанию
	if(!execute_project && current_project)
		SetExecuteProject(current_project->GetName());
	path = pth;
	// занести в переменные окружения среды
	int pos_n(path.ReverseFind('\\') + 1);
	int pos_e(path.ReverseFind('.'));
	SetEnvVar(SF_SOLUTION_NAME, path.Mid(pos_n, pos_e - pos_n));
	SetEnvVar(SF_SOLUTION_PATH, path.Left(pos_n));
	// установить шапку в главном окне
	theApp.frm->SetTitle("AsmVisualStudio - [" + GetName() + "]");
	// установить платформы и конфигурации
	theApp.frm->UpdatePlatforms();
	// выдать форму показа не найденных проектов
	if(arr.GetSize() > 0)
	{
		CShowUnOpenProjectDlg dlg("Не найденные проекты", "Список проектов:", "Удалить", &arr);
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
	// текущий и на запуск
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
	// платформы и конфигурации
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
	// список проектов
	for(i = 0 ; i < projects.size() ; i++)
	{
		// сохранить проект
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
			// создать новый проект/решение
			path_prj += name_sln + "\\";
			path = path_prj + name_sln + EXT_SLN;
			// создать путь к проекту
			path_prj += name_prj + "\\";
			path_prj += name_prj + EXT_PRJ;
			// создать проект
			result = p->Create(path_prj, name_prj, wzd);
			break;
		case ACTION_ADD_NEW_PROJECT:
			// создать путь к проекту из пути к решению
			path_prj = Utils::extractPathFromFullPath(path, TRUE);
			path_prj += name_prj + "\\";
			path_prj += name_prj + EXT_PRJ;
			// создать проект
			result = p->Create(path_prj, name_prj, wzd);
			break;
		case ACTION_ADD_OPEN_PROJECT:
			// открыть проект
			result = p->Open(path_prj);
			name_prj = p->GetName();
			break;
		case ACTION_OPEN_PROJECT:
			// создать путь к решению
			path = Utils::extractPathFromFullPath(path_prj, TRUE);
			// открыть проект
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
			msg_err = "Ошибка. Проект \"" + name_prj + "\" уже есть в системе!";
		}
		else
		{
			// определить мастер
			if(wzd == &default_wizard && p->name_wizard != "")
			{
				wzd = theApp.frm->GetConfiguration()->GetWizard(p->name_wizard);
				if(!wzd) wzd = &default_wizard;
			}
			// добавить платформу
			if((plt = AddPlatform(wzd)))
			{
				// установить проект в конфиги добавленной платформы и ее конфигураций
				CConfig* cfg;
				size_t icfg(0);
				while((cfg = plt->EnumConfigs(icfg)))
					cfg->SetProjectExclude(p, FALSE);
			}
			// установить его как текущий
			projects.add(p);
			SetCurrentProject(name_prj);
			is_modify = true;
			// сохранить, если создавался новый проект/система
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
					msg_err = "Ошибка. Не удалось создать проект \"" + name_prj + "\" !";
					break;
				case ACTION_ADD_NEW_PROJECT:
				case ACTION_ADD_OPEN_PROJECT:
					msg_err = "Ошибка. Не удалось добавить новый проект \"" + name_prj + "\" в систему !";
					break;
				case ACTION_OPEN_PROJECT:
					msg_err = "Ошибка. Не удалось отркыть проект \"" + name_prj + "\" в систему !";
					break;
			}
		}
		MessageBox(NULL, msg_err, ASM_STUDIO, MB_ICONERROR);
		delete p ; p = NULL;
		// восстановить старый текущий и переменные окружения
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
	// удалить его из всех платформ и конфигураций
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
	// код завершения процесса
	DWORD exit(STILL_ACTIVE), bread, avail;
	// структура информации о процессе консоли
	PROCESS_INFORMATION pi;
	// буфер ввода/вывода
	char buffer[4097];
	// массив строк
	Array<String> arr_out;
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	/*
	out->AddString("Строка запуска:");
	out->AddString(cmd_line);
	out->AddString("");
	*/
	// запустить процесс
	CString inc_path("INCLUDE=" + theApp.frm->GetConfiguration()->GetDefaultIncludeFolder() + "#");
	CString lib_path("LIB=" + theApp.frm->GetConfiguration()->GetDefaultLibraryFolder() + "#");
	env = (inc_path + lib_path);
	env.Replace('#', 0);
	if(CreateProcess(NULL, (char*)(const char*)cmd_line, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, (void*)(const char*)env, GetEnvVar(SF_CONFIGURATION_PATH) + "\\", si, &pi))
	{
		while(exit == STILL_ACTIVE)
		{
			// пока дочерний процесс не закрыт
			GetExitCodeProcess(pi.hProcess, &exit); 
			PeekNamedPipe(hOut, buffer, 4095, &bread, &avail, NULL);
			// проверяем, есть ли данные для чтения в stdout
			if(bread)
			{
				Sleep(10);
				ReadFile(hOut, buffer, bread, &bread, NULL);
				buffer[bread] = 0;
				// 8. вывести в окно вывода
				Utils::explode("\r\n", buffer, arr_out);
				for(size_t j = 0 ; j < arr_out.size() ; j++)
					out->AddString(arr_out[j]);
			}
		}
		// закрыть процесс
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
	// структура для запуска консоли
	STARTUPINFO si;
	// структура атрибутов безопасности
	SECURITY_ATTRIBUTES sa;
	// структура безопасности пайпов
	SECURITY_DESCRIPTOR sd; 

	size_t index(0), nProjects(0);
	HANDLE hStdOut, hnStdOut;

	CPlatform* plt;
	CConfig* cfg;
	CProject* project;

	CSolution* sln(theApp.frm->GetSolution());
	// получение текущей платформы/конфига
	plt = sln->GetPlatform(sln->GetCurrentPlatform());
	if(plt && ((cfg = plt->GetConfig(sln->GetCurrentConfig()))))
	{
		// инициализация дескриптора безопасности
		InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd, true, NULL, false);
		sa.lpSecurityDescriptor = &sd;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		// разрешаем наследование дескрипторов
		sa.bInheritHandle = true;
		// инициализация структуры для запуска консоли
		GetStartupInfo(&si); 
		si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		// создаем канал
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
			// подменяем дескрипторы для дочернего процесса
			si.hStdOutput = hnStdOut;
			si.hStdError = hnStdOut;
			do
			{
				if(!cfg->IsProjectExclude(project))
				{
					nProjects++;
					// запрос на получение опций трансляции/компоновки
					CString asm_cmd(ApplyFilter(cfg->GetOptions(true)));
					CString lnk_cmd(ApplyFilter(cfg->GetOptions(false)));
					if(asm_cmd != "")
					{
						// запусть транслятор, как консольное приложение
						asm_cmd += sln->EnumElementsPath(true, project);
						sln->Process(asm_cmd, hStdOut, &si);
					}
					if(lnk_cmd != "")
					{
						// запусть компоновщик, как консольное приложение
						lnk_cmd += sln->EnumElementsPath(false, project);
						sln->Process(lnk_cmd, hStdOut, &si);
					}
				}
			} while((project = sln->EnumProjects(index)));
			// закрыть пайп
			CloseHandle(hnStdOut);
			CloseHandle(hStdOut);
		}
	}
	// завершение
	sln->FinishBuild(project, nProjects);

	return 0;
}

void CSolution::BeginBuild(CProject* prj)
{
	CDocumentCommon* doc;
	CString pth;
	Array<CDocumentCommon*> docs;
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	// создать, если необходимо,  папку для конфигурации
	Utils::createPath(GetEnvVar(SF_CONFIGURATION_PATH), false);
	// установить признак сборки
	theApp.frm->SetBuildFlag(true);
	// сохранить все открытые и модифицированные документы
	theApp.frm->GetAllDocuments(&docs, NULL, prj, false, false, false);
	for(size_t i = 0 ; i < docs.size() ; i++)
	{
		doc = docs[i];
		if(doc->IsModified())
			doc->OnDocumentSave();
	}
	// активируем панель вывода
	if(prj)
	{
		pth = "Сборка проекта: " + prj->GetName();
	}
	else
	{
		pth = "Запуск сборки системы: " + GetName();
	}
	out->Reset();
	out->SetTitle(pth + ", Конфигурация: " + GetEnvVar(SF_CONFIGURATION_NAME) + " " + GetEnvVar(SF_PLATFORM_NAME));
	theApp.frm->SendMessage(WM_BUILD_STATUS_BAR_MSG, (WPARAM)(prj ? "Сборка проекта..." : "Сборка системы..."));
}

void CSolution::FinishBuild(CProject* prj, int nProjects)
{
	CString str;
	CPanelOutputView* out(theApp.frm->GetPanelOutput());
	CPanelTaskView* task(theApp.frm->GetPanelTask());
	// подсчитать количество предупреждений и ошибок
	nErrors = task->UpdateList(CPanelTaskView::modeError, false);
	long warnings = task->UpdateList(CPanelTaskView::modeWarning, false);
	// вывести результат сборки
	str.Format("Сборка %s завершена %s: Проектов - %d, Пропущено - %d, Ошибок - %d, Предупреждений - %d", (prj ? "проекта" : "системы"), (nErrors > 0 ? "неудачно" : "успешно"), nProjects, theApp.frm->GetSolution()->GetCountProjects() - nProjects, nErrors, warnings);
	out->AddString("");
	out->AddString(str);
	out->AddString("");
	// сбросить признак сборки
	theApp.frm->SetBuildFlag(false);
	theApp.frm->SendMessage(WM_BUILD_STATUS_BAR_MSG, NULL);
}

bool CSolution::BuildProject(CProject* prj)
{
	// инициализация сборки
	BeginBuild(prj);
	// запустить механизм пайпов с прослушиванием в отдельном треде
	return (CreateThread(NULL, 0, ThreadBuildProc, (void*)prj, 0, NULL) != NULL);
}

bool CSolution::Build()
{
	// инициализация сборки
	BeginBuild(NULL);
	// запустить механизм пайпов с прослушиванием в отдельном треде
	return (CreateThread(NULL, 0, ThreadBuildProc, NULL, 0, NULL) != NULL);
}

bool CSolution::Rebuild()
{
	WIN32_FIND_DATA wfd;
	HANDLE hFile;
	// стереть всё
	CString folder(GetEnvVar(SF_CONFIGURATION_PATH));
	// перечислить и стереть все файлы в этой папке
	if((hFile = FindFirstFile(folder + "\\*.*", &wfd)) != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if(wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_VIRTUAL | FILE_ATTRIBUTE_HIDDEN))
				continue;
			::DeleteFile(folder + "\\" + wfd.cFileName);
		} while(FindNextFile(hFile, &wfd));
	}
	// стереть исполняемый файл
	::DeleteFile(GetCurrentSpecificOption(FOPT_EXECUTE));
	// запустить сборку
	return Build();
}

DWORD WINAPI ThreadExecuteProc(void* param)
{
	CSolution* sln(theApp.frm->GetSolution());
	// ожидание завершения сборки
	while(theApp.frm->IsProcessBuild()) {Sleep(100);}
	// проверка на результат сборки
	if(sln->nErrors > 0)
	{
		if(MessageBox(NULL, "Сборка завершилась неудачно. Всё равно продолжить?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) != IDYES)
			return 0;
	}
	// структура для запуска консоли
	STARTUPINFO si;
	// структура информации о процессе консоли
	PROCESS_INFORMATION pi;
	// получить параметры запуска по умолчанию
	GetStartupInfo(&si);
	// запуск процесса
	if(!CreateProcess(NULL, (char*)(const char*)param, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		MessageBox(NULL, "Ошибка. Не удалось запустить процесс!", ASM_STUDIO, MB_ICONERROR);
	return 0;
}

bool CSolution::Execute()
{
	static CString cmd;
	CString cmd_prg, cmd_params;
	Array<CDocumentCommon*> docs;
	// 1. проверить на наличие модифицированных открытых документов
	CString msg;
	theApp.frm->GetAllDocuments(&docs, NULL, execute_project, false, true, false);
	for(size_t i = 0 ; i < docs.size() ; i++)
	{
		if(docs[i]->IsModified())
		{
			msg = "Некоторые документы запускаемого проекта модифицированы. Предварительно выполнить сборку проекта?";
			break;
		}
	}
	// 2. проверить на наличие ошибок после последней сборки
	if(nErrors > 0 || msg != "")
	{
		if(msg == "") msg = "Предыдущая сборка была завершена неудачно. Выполнить ее повторно?";
		// 3. запрос на сборку
		if(MessageBox(NULL, msg, ASM_STUDIO, MB_ICONQUESTION | MB_YESNO) == IDYES)
			theApp.frm->SendMessage(WM_COMMAND, ID_REBUILD);
	}
	// 4. строка запуска
	if((cmd_prg = GetCurrentSpecificOption(FOPT_EXECUTE)) == "")
	{
		MessageBox(NULL, "Ошибка. Не удалось найти строку запуска!", ASM_STUDIO, MB_ICONERROR);
		return false;
	}
	cmd = "\"" + cmd_prg + "\" " + execute_project->GetStartParams();
	// 5. создание потока и в нем процесса
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
		switch(MessageBox(NULL, "Система модифицированна. Сохранить изменения?", ASM_STUDIO, MB_ICONQUESTION | MB_YESNOCANCEL))
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
			// добавить все конфиги этой платформы
			for(size_t i = 0 ; i < wzd->configs.size() ; i++)
			{
				wcfg = wzd->configs[i];
				CConfig* c(plt->AddConfig(wcfg->name, wcfg->asm_props, wcfg->lnk_props));
				if(c && !cfg) cfg = c;
			}
			platforms.add(plt);
		}
		// сразу установить эту платформу как текущую
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
				// признак открытия/закладки/позиция каретки в документах
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
			// текущая конфигурация/платформа
			if((h = xml.getNode(hroot, "current_platform")))
				xml.setValue(h, GetEnvVar(SF_PLATFORM_NAME));
			if((h = xml.getNode(hroot, "current_config")))
				xml.setValue(h, GetEnvVar(SF_CONFIGURATION_NAME));
			// текущий документ
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