
#include "StdAfx.h"
#include "AsmVisualStudio.h"
#include "Configuration.h"
#include "FindReplaceDlg.h"
#include "ShowUnOpenProjectDlg.h"

/*
static PROPERTY m_arr_dos_debug_asm[] =
{
	<option key="masm" name="Транслятор" opt="$(MainDir)Bin\ml.exe" tip="Размещение транслятора" flags="folder" />
	<option key="include" name="Базовый Include путь" opt="$(MainDir)Include\" tip="Размещение включаемых файлов" flags="folder" />
	<option key="AT" name="Com файл" tip="Разрешить сверхмаленькую модель памяти tiny" flags="bool" />
	<option key="X" name="Игнорировать Include путь" tip="Игнорировать глобальный путь к включаемым файлам" flags="bool" />
	<option key="WX" name="Предупреждения как ошибки" tip="Расценивать предупреждения как ошибки" flags="bool" />
	<option key="W" key_val="0,1,2,3,4" name="Уровень предупреждений" opt="Уровень 0" opt_val="Уровень 0,Уровень 1,Уровень 2,Уровень 3,Уровень 4" tip="Установить уровень предупреждений" flags="choise" />
	<option key="Zp" key_val="4,8,16" name="Выравнивание структур" opt="4" opt_val="4,8,16" tip="Установить выравнивание структур" flags="choise" />
	<option key="Cp" name="Регистр идентификаторов" lval="1" tip="Сохранить регистр идентификаторов пользователя" flags="bool" />
	<option key="Cu" name="Идентификаторы в верхний регистр" tip="Преобразовать все идентификаторы в верхний регистр" flags="bool" />
	<option key="Cx" name="Регистр внешних зависимостей" lval="1" tip="Сохранить регистр в publics externs" flags="bool" />
	<option key="coff" name="COFF формат объектного файла" tip="Генерировать объектный файл в формате COFF" flags="bool" />
	<option key="D" name="Текст макроса" opt="DEBUG" tip="Определить текст макроса" flags="string" />
	<option key="F" name="Размер стека" tip="Устанавливает размер стека (в байтах)" flags="number" />
	<option key="Fl" name="Файл листинга" tip="Сгенерировать файл листинга" flags="string" />
	<option key="Sc" name="Тайминги в листинг" tip="Показывать тайминги в листинг" flags="bool" />
	<option key="Sn" name="Листинг символьной таблицы" tip="Подавить листинг таблицы символов" flags="bool" />
	<option key="Fm" name="Файл карты" tip="Сгенерировать файл карты памяти" flags="string" />
	<option key="G" key_val="c,d,z" name="Спецификатор вызова" opt="stdcall" opt_val="pascal,c,stdcall" tip="Тип вызова процедур" flags="choise" />
	<option key="I" name="Дополнительный Include путь" tip="Дополнительный путь к включаемым файлам" flags="folder" />
	<option key="nologo" name="Логотип" tip="Отображать логотип" flags="bool" />
	<option key="Zf" name="Все символы общедоступные" tip="Сделать все символы общедоступными" flags="bool" />
	<option key="Zd" name="Номера строк в отладку" tip="Добавить номера строк в отладочную информацию" flags="bool" />
	<option key="Zi" name="Символы в отладку" lval="1" tip="Добавлять отладочную информацию об символах" flags="bool" />
	<option name="Дополнительно" tip="Дополнительные опции" flags="string" />
};

static PROPERTY m_arr_dos_debug_link[] =
{
	<option key="link" name="Компоновщик" opt="$(MainDir)Bin\link.exe" tip="Размещение компоновщика" flags="folder" />
	<option key="OUT" name="Имя файла" opt="$(ProjectPath)$(ConfigurationName)\$(ProjectName).exe" tip="Установить имя выходного файла." flags="string" />
	<option key="LIBPATH" name="Путь к библиотекам" opt="$(MainDir)Lib\" tip="Установить путь к библиотекам." flags="folder" />
	<option key="WARN" key_val="0,1,2,3" name="Уровень предупреждений" opt="Уровень 0" opt_val="Уровень 0,Уровень 1,Уровень 2,Уровень 3" tip="Уровень предупреждений выдаваемых компоновщиком." flags="choise" />
	<option name="Используемые библиотеки" tip="Библиотеки по умолчанию" flags="string" />
	<option key="ALIGN" name="Значение выравнивания секций" lval="4096" tip="Выравнивание секций в линейной модели." flags="number" />
	<option key="DEBUG" name="Отладка" lval="1" tip="Создавать отладочную информацию. Отладочная информация помещается в pdb-файл." flags="bool" />
	<option key="DEBUGTYPE" key_val="CV,COFF,BOTH" name="Тип отладки" opt="COFF" opt_val="CV,COFF,BOTH" tip="Тип отладочной информации" flags="choise" />
	<option key="IMPLIB" name="Библиотека импорта" tip="Установить имя import-библиотеки." flags="string" />
	<option key="DEF" name="Файл определений" tip="Имя файла определений." flags="string" />
	<option key="DEFAULTLIB" name="Библиотека по умолчанию" tip="Добавить библиотеку по умолчанию." flags="string" />
	<option key="EXPORT" name="Экспортируемые функции" tip="Установить имена функций для экспорта." flags="string" />
	<option key="INCREMENTAL" name="Наращиваемая компоновка" tip="При выборе добавляется дополнительная информация позволяющая быстрее перекомпилировать этот файл." flags="bool" />
	<option key="MAP" name="Файл карты" tip="Создавать файл карты." flags="string" />
	<option key="MAPINFO" key_val="EXPORTS,FIXUPS,LINES" name="Тип файла карты" opt="EXPORTS" opt_val="EXPORTS,FIXUPS,LINES" tip="Тип включаемой информации в файл карты." flags="choise" />
	<option key="NODEFAULTLIB" name="Игнорировать библиотеки" tip="Игнорировать все или конкретную библиотеку." flags="string" />
	<option key="NOLOGO" name="Логотип" tip="Вывод логотипа." flags="bool" />
	<option key="STACK" name="Размер стека" tip="Установить размер стека." flags="number" />
	<option name="Дополнительно" tip="Дополнительные опции" flags="string" />
};

static PROPERTY m_arr_dos_release_asm[] =
{
	<option key="masm" name="Транслятор" opt="$(MainDir)Bin\\ml.exe" tip="Размещение транслятора" flags="folder" />
	<option key="include" name="Базовый Include путь" opt="$(MainDir)Include\\" tip="Размещение включаемых файлов" flags="folder" />
	<option key="AT" name="Com файл" tip="Разрешить сверхмаленькую модель памяти tiny" flags="bool" />
	<option key="X" name="Игнорировать Include путь" tip="Игнорировать глобальный путь к включаемым файлам" flags="bool" />
	<option key="WX" name="Предупреждения как ошибки" tip="Расценивать предупреждения как ошибки" flags="bool" />
	<option key="W" key_val="0,1,2,3,4" name="Уровень предупреждений" opt="Уровень 0" opt_val="Уровень 0,Уровень 1,Уровень 2,Уровень 3,Уровень 4" tip="Установить уровень предупреждений" flags="choise" />
	<option key="Zp" key_val="4,8,16" name="Выравнивание структур" opt="4" opt_val="4,8,16" tip="Установить выравнивание структур" flags="choise" />
	<option key="Cp" name="Регистр идентификаторов" lval="1" tip="Сохранить регистр идентификаторов пользователя" flags="bool" />
	<option key="Cu" name="Идентификаторы в верхний регистр" tip="Преобразовать все идентификаторы в верхний регистр" flags="bool" />
	<option key="Cx" name="Регистр внешних зависимостей" lval="1" tip="Сохранить регистр в publics externs" flags="bool" />
	<option key="coff" name="COFF формат объектного файла" tip="Генерировать объектный файл в формате COFF" flags="bool" />
	<option key="D" name="Текст макроса" tip="Определить текст макроса" flags="string" />
	<option key="F" name="Размер стека" tip="Устанавливает размер стека (в байтах)" flags="number" />
	<option key="Fl" name="Файл листинга" tip="Сгенерировать файл листинга" flags="string" />
	<option key="Sc" name="Тайминги в листинг" tip="Показывать тайминги в листинг" flags="bool" />
	<option key="Sn" name="Листинг символьной таблицы" tip="Подавить листинг таблицы символов" flags="bool" />
	<option key="Fm" name="Файл карты" tip="Сгенерировать файл карты памяти" flags="string" />
	<option key="G" key_val="c,d,z" name="Спецификатор вызова" opt="stdcall" opt_val="pascal,c,stdcall" tip="Тип вызова процедур" flags="choise" />
	<option key="I" name="Дополнительный Include путь" tip="Дополнительный путь к включаемым файлам" flags="folder" />
	<option key="nologo" name="Логотип" tip="Отображать логотип" flags="bool" />
	<option key="Zf" name="Все символы общедоступные" tip="Сделать все символы общедоступными" flags="bool" />
	<option name="Дополнительно" tip="Дополнительные опции" flags="string" />
};

static PROPERTY m_arr_dos_release_link[] =
{
	<option key="link" name="Компоновщик" opt="$(MainDir)Bin\link.exe" tip="Размещение компоновщика" flags="folder" />
	<option key="OUT" name="Имя файла" opt="$(ProjectPath)$(ConfigurationName)\$(ProjectName).exe" tip="Установить имя выходного файла." flags="string" />
	<option key="LIBPATH" name="Путь к библиотекам" opt="$(MainDir)Lib\" tip="Установить путь к библиотекам." flags="folder" />
	<option key="WARN" key_val="0,1,2,3" name="Уровень предупреждений" opt="Уровень 0" opt_val="Уровень 0,Уровень 1,Уровень 2,Уровень 3" tip="Уровень предупреждений выдаваемых компоновщиком." flags="choise" />
	<option key="" name="Используемые библиотеки" tip="Библиотеки по умолчанию" flags="string" />
	<option key="ALIGN" name="Значение выравнивания секций" lval="4096" tip="Выравнивание секций в линейной модели." flags="number" />
	<option key="IMPLIB" name="Библиотека импорта" tip="Установить имя import-библиотеки." flags="string" />
	<option key="DEF" name="Файл определений" tip="Имя файла определений." flags="string" />
	<option key="DEFAULTLIB" name="Библиотека по умолчанию" tip="Добавить библиотеку по умолчанию." flags="string" />
	<option key="EXPORT" name="Экспортируемые функции" tip="Установить имена функций для экспорта." flags="string" />
	<option key="INCREMENTAL" name="Наращиваемая компоновка" tip="При выборе добавляется дополнительная информация позволяющая быстрее перекомпилировать этот файл." flags="bool" />
	<option key="MAP" name="Файл карты" tip="Создавать файл карты." flags="string" />
	<option key="MAPINFO" key_val="EXPORTS,FIXUPS,LINES" name="Тип файла карты" opt="EXPORTS" opt_val="EXPORTS,FIXUPS,LINES" tip="Тип включаемой информации в файл карты." flags="choise" />
	<option key="NODEFAULTLIB" name="Игнорировать библиотеки" tip="Игнорировать все или конкретную библиотеку." flags="string" />
	<option key="NOLOGO" name="Логотип" tip="Вывод логотипа." flags="bool" />
	<option key="STACK" name="Размер стека" tip="Установить размер стека." flags="number" />
	<option name="Дополнительно" tip="Дополнительные опции" flags="string" />
};
*/
static long m_arr_keys_col[] = 
{
	RGB(40, 40, 40), RGB(180, 0, 0), SI_BOLD_UNDERLINE,
	RGB(40, 40, 40), RGB(180, 0, 0), SI_NORMAL,
	RGB(40, 40, 40), RGB(200, 200, 200), SI_BOLD,
	RGB(40, 40, 40), RGB(200, 200, 200), SI_NORMAL,
	RGB(40, 40, 40), RGB(255, 255, 255), SI_NORMAL,
	RGB(40, 40, 40), RGB(255, 255, 255), SI_NORMAL,
	RGB(40, 40, 40), RGB(255, 255, 255), SI_NORMAL,
	RGB(40, 40, 40), RGB(255, 255, 255), SI_NORMAL
};

static const char* m_arr_keywords[] =
{
	"eax,ebx,ecx,edx,esi,edi,esp,ebp,al,ah,ax,bl,bh,bx,cl,ch,cx,dl,dh,dx,si,di,bp,sp,st0,st1,st2,st3,st4,st5,st6,st7,r8,r9,r10,r11,r12,r13,r14,r15,rax,rbx,rcx,rdx,rsi,rdi,rbp,rsp,rpi,epi,cx,ds,es,fs,gs,ss",// asm standard registers
	"xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7,xmm8,xmm9,xmm10,xmm11,xmm12,xmm13,xmm14,xmm15,mm0,mm1,mm2,mm3,mm4,mm5,mm6,mm7",// asm extended registers
	"local,uses,model,type,width,mask,proc,public,private,endp,ends,struct,stdcall,proto,union",// asm keywords
	"db,dw,dd,df,dq,dt,dup",// asm declarators
	"",// asm standard instructions
	"",// asm extended instructions
	"",
	"",
	""
};

CConfiguration::CConfiguration()
{
	fontNames[0] = "Courier";
	fontNames[1] = "Courier";
	fontNames[2] = "Courier";
	fontNames[3] = "Courier";
	fontNames[4] = "Courier";
	fontSizes[0] = 10;
	fontSizes[1] = 10;
	fontSizes[2] = 10;
	fontSizes[3] = 10;
	fontSizes[4] = 10;
	def_prj_folder = GetEnvVar(SF_MAIN_DIR) + "\\Projects\\";
	def_prj_folder_inc = GetEnvVar(SF_MAIN_DIR) + "\\Include\\";
	def_prj_folder_lib = GetEnvVar(SF_MAIN_DIR) + "\\Lib\\";
	// сформировать группы ключевых слов для редакторов кода
	for(int i = 0 ; i < 8 ; i++)
	{
		Array<String> arr;
		Utils::explode(",", m_arr_keywords[i], arr);
		code_grp[i] = new GROUP_KEYWORD;
		code_grp[i]->c_bk_text = m_arr_keys_col[i * 3];
		code_grp[i]->c_text = m_arr_keys_col[i * 3 + 1];
		code_grp[i]->style = m_arr_keys_col[i * 3 + 2];
		for(size_t j = 0 ; j < arr.size() ; j++)
			code_grp[i]->keys.Add(arr[j]);
	}
	is_backup_archive = TRUE;
	// базовая группа
	// редактор кода
	std_group[TE_EDITOR].c_text			= RGB(160, 180, 255);
	std_group[TE_EDITOR].c_sel_text		= RGB(255, 255, 255);
	std_group[TE_EDITOR].c_comment		= RGB(0,255,0);
	std_group[TE_EDITOR].c_bookmark		= RGB(255,0,255);
	std_group[TE_EDITOR].c_number		= RGB(255,255,0);
	std_group[TE_EDITOR].c_ops			= RGB(0,255,255);
	std_group[TE_EDITOR].c_string		= RGB(255,0,255);
	std_group[TE_EDITOR].c_bk_text		= RGB(40, 40, 40);
	std_group[TE_EDITOR].c_bk_sel_text	= RGB(120, 120, 120);
	std_group[TE_EDITOR].c_bk_comment	= RGB(40, 40, 40);
	std_group[TE_EDITOR].c_bk_bookmark	= RGB(40, 40, 40);
	std_group[TE_EDITOR].c_bk_number	= RGB(40, 40, 40);
	std_group[TE_EDITOR].c_bk_ops		= RGB(40, 40, 40);
	std_group[TE_EDITOR].c_bk_string	= RGB(40, 40, 40);
	std_group[TE_EDITOR].s_text			= SI_NORMAL;
	std_group[TE_EDITOR].s_sel_text		= SI_NORMAL;
	std_group[TE_EDITOR].s_comment		= SI_UNDERLINE;
	std_group[TE_EDITOR].s_bookmark		= SI_NORMAL;
	std_group[TE_EDITOR].s_number		= SI_NORMAL;
	std_group[TE_EDITOR].s_ops			= SI_NORMAL;
	std_group[TE_EDITOR].s_string		= SI_NORMAL;
	// редактор текста
	std_group[TE_TEXT].c_text			= RGB(160, 180, 255);
	std_group[TE_TEXT].c_sel_text		= RGB(255, 255, 255);
	std_group[TE_TEXT].c_comment		= RGB(0,255,0);
	std_group[TE_TEXT].c_number			= RGB(255,255,0);
	std_group[TE_TEXT].c_ops			= RGB(0,255,255);
	std_group[TE_TEXT].c_string			= RGB(255,0,255);
	std_group[TE_TEXT].c_bk_text		= RGB(40, 40, 40);
	std_group[TE_TEXT].c_bk_sel_text	= RGB(120, 120, 120);
	std_group[TE_TEXT].c_bk_comment		= RGB(40, 40, 40);
	std_group[TE_TEXT].c_bk_number		= RGB(40, 40, 40);
	std_group[TE_TEXT].c_bk_ops			= RGB(40, 40, 40);
	std_group[TE_TEXT].c_bk_string		= RGB(40, 40, 40);
	std_group[TE_TEXT].s_text			= SI_NORMAL;
	std_group[TE_TEXT].s_sel_text		= SI_NORMAL;
	std_group[TE_TEXT].s_comment		= SI_NORMAL;
	std_group[TE_TEXT].s_number			= SI_NORMAL;
	std_group[TE_TEXT].s_ops			= SI_NORMAL;
	std_group[TE_TEXT].s_string			= SI_NORMAL;
	// двоичный редактор
	std_group[TE_BINARY].c_text			= RGB(160, 180, 255);
	std_group[TE_BINARY].c_sel_text		= RGB(255, 255, 255);
	std_group[TE_BINARY].c_comment		= RGB(0,255,0);
	std_group[TE_BINARY].c_number		= RGB(255,255,0);
	std_group[TE_BINARY].c_ops			= RGB(0,255,255);
	std_group[TE_BINARY].c_string		= RGB(255,0,255);
	std_group[TE_BINARY].c_bk_text		= RGB(40, 40, 40);
	std_group[TE_BINARY].c_bk_sel_text	= RGB(120, 120, 120);
	std_group[TE_BINARY].c_bk_comment	= RGB(40, 40, 40);
	std_group[TE_BINARY].c_bk_number	= RGB(40, 40, 40);
	std_group[TE_BINARY].c_bk_ops		= RGB(40, 40, 40);
	std_group[TE_BINARY].c_bk_string	= RGB(40, 40, 40);
	std_group[TE_BINARY].s_text			= SI_NORMAL;
	std_group[TE_BINARY].s_sel_text		= SI_NORMAL;
	std_group[TE_BINARY].s_comment		= SI_NORMAL;
	std_group[TE_BINARY].s_number		= SI_NORMAL;
	std_group[TE_BINARY].s_ops			= SI_NORMAL;
	std_group[TE_BINARY].s_string		= SI_NORMAL;
	// окно вывода
	std_group[TE_OUTPUT].c_text			= RGB(160, 180, 255);
	std_group[TE_OUTPUT].c_sel_text		= RGB(255, 255, 255);
	std_group[TE_OUTPUT].c_comment		= RGB(160, 180, 255);
	std_group[TE_OUTPUT].c_number		= RGB(160, 180, 255);
	std_group[TE_OUTPUT].c_ops			= RGB(160, 180, 255);
	std_group[TE_OUTPUT].c_string		= RGB(160, 180, 255);
	std_group[TE_OUTPUT].c_bk_text		= RGB(40, 40, 40);
	std_group[TE_OUTPUT].c_bk_sel_text	= RGB(120, 120, 120);
	std_group[TE_OUTPUT].c_bk_comment	= RGB(40, 40, 40);
	std_group[TE_OUTPUT].c_bk_number	= RGB(40, 40, 40);
	std_group[TE_OUTPUT].c_bk_ops		= RGB(40, 40, 40);
	std_group[TE_OUTPUT].c_bk_string	= RGB(40, 40, 40);
	std_group[TE_OUTPUT].s_text			= SI_NORMAL;
	std_group[TE_OUTPUT].s_sel_text		= SI_NORMAL;
	std_group[TE_OUTPUT].s_comment		= SI_NORMAL;
	std_group[TE_OUTPUT].s_number		= SI_NORMAL;
	std_group[TE_OUTPUT].s_ops			= SI_NORMAL;
	std_group[TE_OUTPUT].s_string		= SI_NORMAL;
	// поиск
	std_group[TE_FIND].c_text			= RGB(160, 180, 255);
	std_group[TE_FIND].c_sel_text		= RGB(255, 255, 255);
	std_group[TE_FIND].c_comment		= RGB(160, 180, 255);
	std_group[TE_FIND].c_number			= RGB(160, 180, 255);
	std_group[TE_FIND].c_ops			= RGB(160, 180, 255);
	std_group[TE_FIND].c_string			= RGB(160, 180, 255);
	std_group[TE_FIND].c_bk_text		= RGB(40, 40, 40);
	std_group[TE_FIND].c_bk_sel_text	= RGB(120, 120, 120);
	std_group[TE_FIND].c_bk_comment		= RGB(40, 40, 40);
	std_group[TE_FIND].c_bk_number		= RGB(40, 40, 40);
	std_group[TE_FIND].c_bk_ops			= RGB(40, 40, 40);
	std_group[TE_FIND].c_bk_string		= RGB(40, 40, 40);
	std_group[TE_FIND].s_text			= SI_NORMAL;
	std_group[TE_FIND].s_sel_text		= SI_NORMAL;
	std_group[TE_FIND].s_comment		= SI_NORMAL;
	std_group[TE_FIND].s_number			= SI_NORMAL;
	std_group[TE_FIND].s_ops			= SI_NORMAL;
	std_group[TE_FIND].s_string			= SI_NORMAL;

	memset(grp_fonts, 0, sizeof(grp_fonts));
}

bool CConfiguration::IsExistKeyword(const char* str) const
{
	GROUP_KEYWORD* grp;
	for(int i = 0 ; i < 8 ; i++)
	{
		grp = code_grp[i];
		for(int j = 0 ; j < grp->keys.GetSize() ; j++)
		{
			if(grp->keys[j] == str)
				return true;
		}
	}

	return false;
}

// структура
// <asm_studio_config>
//	<backup_archive>""</backup_archive>
//	<font_names>""</font_names>
//	<font_sizes>""</font_sizes>
//	<find_replace_dlg no_case="" only_word="" is_selection="" is_hide_text="" >
//	<std_groups>
//		<std_group>
//			<selection color="" bk_color="" style="" />
//			<text color="" bk_color="" style="" />
//			<string color="" bk_color="" style="" />
//			<ops color="" bk_color="" style="" />
//			<number color="" bk_color="" style="" />
//			<comment color="" bk_color="" style="" />
//		</std_group>
//		.............
//	</std_groups>
//	<group_keywords>
//		<group_keyword color="" bk_color="" style="">""</group_keyword>
//		................
//	</group_keywords>
//	<wizards>
//		<wizard name = "" path_icon = "" name_platform = "">
//			<configurations>
//				<config name = "">
//					<options_asm>
//						<prop name = "" tip = "" opt = "" opt_val = "" key = "" key_val = "" lval = "" flags = "" always = "" />
//						.....................
//					</options_asm>
//					<options_link>
//						<prop name = "" tip = "" opt = "" opt_val = "" key = "" key_val = "" lval = "" flags = "" always = "" />
//						.........................
//					</options_link>
//				</config>
//			</configurations>
//			<elements>
//				<element path="" path_modules="" is_exclude="" />
//				..................................................
//			</elements>
//		</wizard>
//	</wizards>
// <\asm_studio_config>

void CConfiguration::Open(const char* pth)
{
	Xml xml;
	WIZARD* wzd;
	WIZARD_CONFIG* wcfg;
	WIZARD_ELEMENT* welem;
	PROPERTY* prop;
	uintptr_t hroot, hnode, hstd, hgrp, hwzds, hwzd, hcfg, hprop, helem, h;
	size_t index;
	String val, val1;
	Hex<int> hex;
	Array<String> arr;

	if(xml.open(pth))
	{
		if((hroot = xml.getNode(xml.getRoot(), "asm_studio_config")))
		{
			if((hnode = xml.getNode(hroot, "font_names")))
			{
				xml.getValue(hnode, val);
				Utils::explode(",", val, arr);
				// сделать проверку на совпадение размера массива
				fontNames[TE_OUTPUT] = (arr.size() > 0 ? arr[0] : "Courier");
				fontNames[TE_FIND] = (arr.size() > 1 ? arr[1] : "Courier");
				fontNames[TE_EDITOR] = (arr.size() > 2 ? arr[2] : "Courier");
				fontNames[TE_TEXT] = (arr.size() > 3 ? arr[3] : "Courier");
				fontNames[TE_BINARY] = (arr.size() > 4 ? arr[4] : "Courier");
			}
			if((hnode = xml.getNode(hroot, "font_sizes")))
			{
				xml.getValue(hnode, val);
				Utils::explode(",", val, arr);
				// сделать проверку на совпадение размера массива
				fontSizes[TE_OUTPUT] = (arr.size() > 0 ? atol(arr[0]) : 14);
				fontSizes[TE_FIND] = (arr.size() > 1 ? atol(arr[1]) : 14);
				fontSizes[TE_EDITOR] = (arr.size() > 2 ? atol(arr[2]) : 14);
				fontSizes[TE_TEXT] = (arr.size() > 3 ? atol(arr[3]) : 14);
				fontSizes[TE_BINARY] = (arr.size() > 4 ? atol(arr[4]) : 14);
			}
			if((hnode = xml.getNode(hroot, "backup_archive")))
				xml.getValue(hnode, &is_backup_archive);
			if((hnode = xml.getNode(hroot, "quick_find")))
			{
				xml.getValue(hnode, val);
				quick_find = val;
			}
			if((hnode = xml.getNode(hroot, "quick_replace")))
			{
				xml.getValue(hnode, val);
				quick_replace = val;
			}
			if((hnode = xml.getNode(hroot, "find_replace_dlg")))
			{
				long no_case(0), only_word(0), is_hide_text(0);
				xml.getParamValue(hnode, "no_case", &no_case);
				xml.getParamValue(hnode, "only_word", &only_word);
				xml.getParamValue(hnode, "is_hide_text", &is_hide_text);
				theApp.frm->GetFindReplaceDlg()->Initialize(quick_find, quick_replace, no_case, is_hide_text, only_word );
			}
			if((hnode = xml.getNode(hroot, "def_project_folder")))
			{
				xml.getValue(hnode, val);
				def_prj_folder = val;
			}
			if((hnode = xml.getNode(hroot, "def_project_folder_inc")))
			{
				xml.getValue(hnode, val);
				def_prj_folder_inc = val;
			}
			if((hnode = xml.getNode(hroot, "def_project_folder_lib")))
			{
				xml.getValue(hnode, val);
				def_prj_folder_lib = val;
			}
			if(def_prj_folder == "")
				def_prj_folder = GetEnvVar(SF_MAIN_DIR) + "\\Projects\\";
			if(def_prj_folder_inc == "")
				def_prj_folder_inc = GetEnvVar(SF_MAIN_DIR) + "\\Include\\";
			if(def_prj_folder_lib == "")
				def_prj_folder_lib = GetEnvVar(SF_MAIN_DIR) + "\\Lib\\";
			if((hstd = xml.getNode(hroot, "std_groups")))
			{
				index = 0;
				while((hnode = xml.getNode(hstd, index)))
				{
					val = xml.getNodeName(hnode);
					if(val == "std_group")
					{
						if((h = xml.getNode(hnode, "selection")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_sel_text = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_sel_text = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_sel_text = hex.val;
						}
						if((h = xml.getNode(hnode, "text")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_text = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_text = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_text = hex.val;
						}
						if((h = xml.getNode(hnode, "string")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_string = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_string = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_string = hex.val;
						}
						if((h = xml.getNode(hnode, "ops")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_ops = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_ops = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_ops = hex.val;
						}
						if((h = xml.getNode(hnode, "number")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_number = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_number = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_number = hex.val;
						}
						if((h = xml.getNode(hnode, "comment")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_comment = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_comment = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_comment = hex.val;
						}
						if((h = xml.getNode(hnode, "bookmarks")))
						{
							if(xml.getParam(h, "color", hex))
								std_group[index].c_bookmark = hex.val;
							if(xml.getParam(h, "bk_color", hex))
								std_group[index].c_bk_bookmark = hex.val;
							if(xml.getParam(h, "style", hex))
								std_group[index].s_bookmark = hex.val;
						}
					}
					index++;
				}
			}
			if((hgrp = xml.getNode(hroot, "group_keywords")))
			{
				index = 0;
				while((hnode = xml.getNode(hgrp, index)))
				{
					val = xml.getNodeName(hnode);
					if(val == "group_keyword")
					{
						if(xml.getParam(hnode, "color", hex))
							code_grp[index]->c_text = hex.val;
						if(xml.getParam(hnode, "bk_color", hex))
							code_grp[index]->c_bk_text = hex.val;
						if(xml.getParam(hnode, "style", hex))
							code_grp[index]->style = hex.val;
						xml.getValue(hnode, val);
						Utils::explode("#", val, arr);
						code_grp[index]->keys.RemoveAll();
						for(size_t i = 0 ; i < arr.size() ; i++)
							code_grp[index]->keys.Add(arr[i]);
					}
					index++;
				}
			}
			// мастеры
			if((hwzds = xml.getNode(hroot, "wizards")))
			{
				index = 0;
				while((hwzd = xml.getNode(hwzds, index++)))
				{
					val = xml.getNodeName(hwzd);
					if(val != "wizard")
						continue;
					if(!xml.getParamValue(hwzd, "name", val))
						continue;
					if((wzd = AddWizard(val)))
					{
						if(xml.getParamValue(hwzd, "path_icon", val))
						{
							wzd->path_icon = val;
							wzd->hIcon = (HICON)LoadImage(NULL, ApplyFilter(val), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
						}
						if(xml.getParamValue(hwzd, "name_platform", val))
							wzd->name_platform = val;
						// элементы
						if((hgrp = xml.getNode(hwzd, "elements")))
						{
							size_t index_elem(0);
							while((helem = xml.getNode(hgrp, index_elem++)))
							{
								val = xml.getNodeName(helem);
								if(val != "element")
									continue;
								if(!xml.getParamValue(helem, "path", val))
									continue;
								welem = new WIZARD_ELEMENT;
								welem->path = val;
								if(xml.getParamValue(helem, "path_files", val))
									welem->path_files = val;
								if(xml.getParamValue(helem, "path_modules", val))
									welem->path_modules = val;
								xml.getParamValue(helem, "is_exclude", &welem->is_exclude);
								wzd->elems.add(welem);
							}
						}
						// конфигурации
						if((hgrp = xml.getNode(hwzd, "configurations")))
						{
							size_t index_cfg(0);
							while((hcfg = xml.getNode(hgrp, index_cfg++)))
							{
								val = xml.getNodeName(hcfg);
								if(val != "config")
									continue;
								if(!xml.getParamValue(hcfg, "name", val))
									continue;
								wcfg = new WIZARD_CONFIG(val);
								if((hprop = xml.getNode(hcfg, "options_asm")))
								{
									size_t index_asm(0);
									while((h = xml.getNode(hprop, index_asm++)))
									{
										val = xml.getNodeName(h);
										if(val != "option")
											continue;
										if(xml.getParamValue(h, "name", val))
										{
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
								}
								if((hprop = xml.getNode(hcfg, "options_link")))
								{
									size_t index_lnk(0);
									while((h = xml.getNode(hprop, index_lnk++)))
									{
										val = xml.getNodeName(h);
										if(val != "option")
											continue;
										if(xml.getParamValue(h, "name", val))
										{
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
								}
								wzd->configs.add(wcfg);
							}
						}
					}
				}
			}
			// модули ресурсов
			if((hgrp = xml.getNode(hroot, "module_resources")))
			{
				index = 0;
				CPanelArchiveView* arch_view(theApp.frm->GetPanelArchive());
				CStringArray arr;
				while((hnode = xml.getNode(hgrp, index++)))
				{
					val = xml.getNodeName(hnode);
					if(val == "module")
					{
						xml.getParamValue(hnode, "name", val);
						xml.getParamValue(hnode, "progID", val1);
						if(!arch_view->AddResourceModule(val, val1))
							arr.Add(val);
					}
				}
				if(arr.GetSize() > 0)
				{
					CShowUnOpenProjectDlg dlg("Не найденные модули ресурсов", "Список модулей:", "ОК", &arr);
					dlg.DoModal();
				}
			}
		}
	}
	path = pth;
	UpdateFonts(TE_OUTPUT, FALSE);
	UpdateFonts(TE_FIND, FALSE);
	UpdateFonts(TE_BINARY, FALSE);
	UpdateFonts(TE_TEXT, FALSE);
	UpdateFonts(TE_EDITOR, FALSE);
}

void CConfiguration::Save()
{
	Xml xml;
	WIZARD* wzd;
	WIZARD_CONFIG* wcfg;
	WIZARD_ELEMENT* welem;
	PROPERTY* prop;
	uintptr_t hroot, hnode, hstd, hgrp, hwzds, hwzd, hprop, hcfg, helem, h;
	size_t i, index;
	String val;

	hroot = xml.addNode(xml.getRoot(), "asm_studio_config", "");
	xml.addNode(hroot, "font_names", val.format("%s,%s,%s,%s,%s", fontNames[TE_OUTPUT], fontNames[TE_FIND], fontNames[TE_EDITOR], fontNames[TE_TEXT], fontNames[TE_BINARY]));
	xml.addNode(hroot, "font_sizes", val.format("%d,%d,%d,%d,%d", fontSizes[TE_OUTPUT], fontSizes[TE_FIND], fontSizes[TE_EDITOR], fontSizes[TE_TEXT], fontSizes[TE_BINARY]));
	xml.addNode(hroot, "def_project_folder", def_prj_folder);
	xml.addNode(hroot, "def_project_folder_inc", def_prj_folder_inc);
	xml.addNode(hroot, "def_project_folder_lib", def_prj_folder_lib);
	xml.addNode(hroot, "backup_archive", is_backup_archive);
	if((hnode = xml.addNode(hroot, "find_replace_dlg", "")))
	{
		long is_case, is_hide, is_word;
		theApp.frm->GetFindReplaceDlg()->Uninitialize(quick_find, quick_replace, is_case, is_hide, is_word);
		xml.addParam(hnode, "no_case", is_case);
		xml.addParam(hnode, "only_word", is_word);
		xml.addParam(hnode, "is_hide_text", is_hide);
	}
	xml.addNode(hroot, "quick_find", quick_find);
	xml.addNode(hroot, "quick_replace", quick_replace);
	if((hstd = xml.addNode(hroot, "std_groups", "")))
	{
		for(i = 0 ; i < 5 ; i++)
		{
			if((hnode = xml.addNode(hstd, "std_group", "")))
			{
				if((h = xml.addNode(hnode, "selection", "")))
				{
					xml.addParam(h, "color", Hex<long>(std_group[i].c_sel_text));
					xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_sel_text));
					xml.addParam(h, "style", Hex<long>(std_group[i].s_sel_text));
				}
				if((h = xml.addNode(hnode, "text", "")))
				{
					xml.addParam(h, "color", Hex<long>(std_group[i].c_text));
					xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_text));
					xml.addParam(h, "style", Hex<long>(std_group[i].s_text));
				}
				if((h = xml.addNode(hnode, "string", "")))
				{
					xml.addParam(h, "color", Hex<long>(std_group[i].c_string));
					xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_string));
					xml.addParam(h, "style", Hex<long>(std_group[i].s_string));
				}
				if((h = xml.addNode(hnode, "ops", "")))
				{
					xml.addParam(h, "color", Hex<long>(std_group[i].c_ops));
					xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_ops));
					xml.addParam(h, "style", Hex<long>(std_group[i].s_ops));
				}
				if((h = xml.addNode(hnode, "number", "")))
				{
					xml.addParam(h, "color", Hex<long>(std_group[i].c_number));
					xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_number));
					xml.addParam(h, "style", Hex<long>(std_group[i].s_number));
				}
				if((h = xml.addNode(hnode, "comment", "")))
				{
					xml.addParam(h, "color", Hex<long>(std_group[i].c_comment));
					xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_comment));
					xml.addParam(h, "style", Hex<long>(std_group[i].s_comment));
				}
				if(i == TE_EDITOR)
				{
					if((h = xml.addNode(hnode, "bookmarks", "")))
					{
						xml.addParam(h, "color", Hex<long>(std_group[i].c_bookmark));
						xml.addParam(h, "bk_color", Hex<long>(std_group[i].c_bk_bookmark));
						xml.addParam(h, "style", Hex<long>(std_group[i].s_bookmark));
					}
				}
			}
		}
		if((hgrp = xml.addNode(hroot, "group_keywords", "")))
		{
			for(i = 0 ; i < 8 ; i++)
			{
				val = "";
				GROUP_KEYWORD* grpk(code_grp[i]);
				for(int j = 0 ; j < code_grp[i]->keys.GetSize() ; j++)
				{
					if(val != "")
						val += "#";
					val += code_grp[i]->keys[j];
				}
				if((hnode = xml.addNode(hgrp, "group_keyword", val)))
				{
					xml.addParam(hnode, "color", Hex<long>(code_grp[i]->c_text));
					xml.addParam(hnode, "bk_color", Hex<long>(code_grp[i]->c_bk_text));
					xml.addParam(hnode, "style", Hex<long>(code_grp[i]->style));
				}
			}
		}
		// мастеры
		if((hwzds = xml.addNode(hroot, "wizards", "")))
		{
			index = 0;
			while((wzd = EnumWizards(index)))
			{
				if((hwzd = xml.addNode(hwzds, "wizard", "")))
				{
					xml.addParam(hwzd, "name", wzd->name);
					xml.addParam(hwzd, "path_icon", wzd->path_icon);
					xml.addParam(hwzd, "name_platform", wzd->name_platform);
					if((hgrp = xml.addNode(hwzd, "elements", "")))
					{
						for(i = 0 ; i < wzd->elems.size() ; i++)
						{
							welem = wzd->elems[i];
							if((helem = xml.addNode(hgrp, "element", "")))
							{
								xml.addParam(helem, "path", welem->path);
								xml.addParam(helem, "path_modules", welem->path_modules);
								xml.addParam(helem, "path_files", welem->path_files);
								xml.addParam(helem, "is_exclude", welem->is_exclude);
							}
						}
					}
					if((hgrp = xml.addNode(hwzd, "configurations", "")))
					{
						for(i = 0 ; i < wzd->configs.size() ; i++)
						{
							wcfg = wzd->configs[i];
							if((hcfg = xml.addNode(hgrp, "config", "")))
							{
								xml.addParam(hcfg, "name", wcfg->name);
								if((hprop = xml.addNode(hcfg, "options_asm", "")))
								{
									for(size_t j = 0 ; j < wcfg->asm_props.size() ; j++)
									{
										prop = wcfg->asm_props[j];
										if((h = xml.addNode(hprop, "option", "")))
										{
											xml.addParam(h, "name", prop->name);
											if(prop->key != "")
												xml.addParam(h, "key", prop->key);
											if(prop->lVal)
												xml.addParam(h, "lval", prop->lVal);
											xml.addParam(h, "flags", Utils::convertLongFlagsToString(prop->flags, 12, stk_prop_flags, false));
											if(prop->opt != "")
												xml.addParam(h, "opt", prop->opt);
											if(prop->opt_val != "")
												xml.addParam(h, "opt_val", prop->opt_val);
											if(prop->key_val != "")
												xml.addParam(h, "key_val", prop->key_val);
											xml.addParam(h, "tip", prop->tip);
										}
									}
								}
								if((hprop = xml.addNode(hcfg, "options_link", "")))
								{
									for(size_t j = 0 ; j < wcfg->lnk_props.size() ; j++)
									{
										prop = wcfg->lnk_props[j];
										if((h = xml.addNode(hprop, "option", "")))
										{
											xml.addParam(h, "name", prop->name);
											if(prop->key != "")
												xml.addParam(h, "key", prop->key);
											if(prop->lVal)
												xml.addParam(h, "lval", prop->lVal);
											xml.addParam(h, "flags", Utils::convertLongFlagsToString(prop->flags, 12, stk_prop_flags, false));
											if(prop->opt != "")
												xml.addParam(h, "opt", prop->opt);
											if(prop->opt_val != "")
												xml.addParam(h, "opt_val", prop->opt_val);
											if(prop->key_val != "")
												xml.addParam(h, "key_val", prop->key_val);
											xml.addParam(h, "tip", prop->tip);
										}
									}
								}
							}
						}
					}
				}
			}
		}
		// модули ресурсов
		if((hgrp = xml.addNode(hroot, "module_resources", "")))
		{
			index = 0;
			CPanelArchiveView* arch_view(theApp.frm->GetPanelArchive());
			CPanelArchiveView::ACTIVEX* ax;
			while((ax = arch_view->EnumResourceModules(index)))
			{
				if((hnode = xml.addNode(hgrp, "module", "")))
				{
					xml.addParam(hnode, "name", ax->name);
					xml.addParam(hnode, "progID", ax->progID);
				}
			}
		}
	}
	xml.save(path);
}

void CConfiguration::UpdateFonts(int type, BOOL is)
{
	if(grp_fonts[type].hFont[0])
	{
		::DeleteObject(grp_fonts[type].hFont[0]);
		::DeleteObject(grp_fonts[type].hFont[1]);
		::DeleteObject(grp_fonts[type].hFont[2]);
		::DeleteObject(grp_fonts[type].hFont[3]);
	}
	grp_fonts[type].hFont[0] = ::CreateFont(-fontSizes[type], 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, fontNames[type]);
	grp_fonts[type].hFont[1] = ::CreateFont(-fontSizes[type], 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, fontNames[type]);
	grp_fonts[type].hFont[2] = ::CreateFont(-fontSizes[type], 0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, fontNames[type]);
	grp_fonts[type].hFont[3] = ::CreateFont(-fontSizes[type], 0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE, RUSSIAN_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, fontNames[type]);

	if(is)
	{
		CDC* dc(AfxGetMainWnd()->GetDC());
		CSize sz;

		dc->SelectObject(grp_fonts[type].hFont[0]);
		sz = dc->GetTextExtent("1");
		grp_fonts[type].hnf = sz.cy;
		grp_fonts[type].wnf = sz.cx;
		dc->SelectObject(grp_fonts[type].hFont[1]);
		sz = dc->GetTextExtent("1");
		grp_fonts[type].wbf = sz.cx;
		dc->SelectObject(grp_fonts[type].hFont[2]);
		sz = dc->GetTextExtent("1");
		grp_fonts[type].wuf = sz.cx;
		dc->SelectObject(grp_fonts[type].hFont[3]);
		sz = dc->GetTextExtent("1");
		grp_fonts[type].wbuf = sz.cx;
	}
	else
	{
		grp_fonts[type].hnf = 10;
		grp_fonts[type].wnf = 10;
		grp_fonts[type].wbf = 10;
		grp_fonts[type].wuf = 10;
		grp_fonts[type].wbuf = 10;
	}
}

void CConfiguration::UpdateAllFonts()
{
	UpdateFonts(TE_OUTPUT, TRUE);
	UpdateFonts(TE_FIND, TRUE);
	UpdateFonts(TE_BINARY, TRUE);
	UpdateFonts(TE_TEXT, TRUE);
	UpdateFonts(TE_EDITOR, TRUE);
}

WIZARD* CConfiguration::GetWizard(const char* name)
{
	WIZARD* wzd;

	if(name)
	{
		for(size_t i = 0 ; i < wizards.size() ; i++)
		{
			wzd = wizards[i];
			if(wzd->name == name)
				return wzd;
		}
	}

	return NULL;
}

WIZARD* CConfiguration::AddWizard(const char* name)
{
	if(GetWizard(name))
		return NULL;
	WIZARD* wzd = new WIZARD();
	wzd->name = name;
	wizards.add(wzd);
	return wzd;
}
