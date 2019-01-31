
#pragma once

class CConfiguration
{
public:
	// конструктор
	CConfiguration();
	// деструктор
	virtual ~CConfiguration() {}
	// обновить параметры всех шрифтов
	void UpdateAllFonts();
	// открыть
	void Open(const char* path);
	// сохранить
	void Save();
	// установка признака резервной копии архива
	void SetBackupArchive(long is) {is_backup_archive = is;}
	// установить новую папку проектов по умолчанию
	void SetDefaultProjectFolder(const char* path) {def_prj_folder = path;}
	// установить новую папку включаемых файлов по умолчанию
	void SetDefaultIncludeFolder(const char* path) {def_prj_folder_inc = path;}
	// установить новую папку библиотечных файлов
	void SetDefaultLibraryFolder(const char* path) {def_prj_folder_lib = path;}
	// установить новый размер шрифта
	void SetSizeFont(int type, long sz) {fontSizes[type] = sz ; UpdateFonts(type, TRUE);}
	// установить новое имя шрифта
	void SetNameFont(int type, const char* name) {fontNames[type] = name ; UpdateFonts(type, TRUE);}
	// проверка на существование ключевого слова
	bool IsExistKeyword(const char* str) const;
	// вернуть размер шрифта
	long GetSizeFont(int type) const {return fontSizes[type];}
	// вернуть все слова
	const char* GetFindReplaceWords(bool is_find) const {return (is_find ? quick_find : quick_replace);}
	// вернуть папку проектов по умолчанию
	const CString& GetDefaultProjectFolder() const {return def_prj_folder;}
	// вернуть папку включаемых файлов по умолчанию
	const CString& GetDefaultIncludeFolder() const {return def_prj_folder_inc;}
	// вернуть папку библиотечных файлов по умолчанию
	const CString& GetDefaultLibraryFolder() const {return def_prj_folder_lib;}
	// вернуть имя шрифта
	const char* GetNameFont(int type) const {return fontNames[type];}
	// вернуть признак резервной копии архива
	long IsBackupArchive() const {return is_backup_archive;}
	// вернуть структуру шрифтов
	GROUP_FONTS* GetGroupFonts(int type) {return &grp_fonts[type];}
	// вернуть группу редактора кода
	GROUP_KEYWORD* GetCodeEditorGroup(int index) const {return code_grp[index];}
	// вернуть стандартную группу
	STD_GROUP* GetStdGroup(int index) {return &std_group[index];}
	// добавить новый мастер
	WIZARD* AddWizard(const char* name);
	// получение мастера по его имени
	WIZARD* GetWizard(const char* name);
	// перечислить все мастера
	WIZARD* EnumWizards(size_t& index) {if(index >= 0 && index < wizards.size()) return wizards[index++]; return NULL;}
protected:
	// признак создания резервной копии архива
	long is_backup_archive;
	// размеры шрифтов
	long fontSizes[5];
	// структура шрифтов
	GROUP_FONTS grp_fonts[5];
	// имена шрифтов
	CString fontNames[5];
	// папка для проектов
	CString def_prj_folder;
	// папка для включаемых файлов
	CString def_prj_folder_inc;
	// папка для библиотечных файлов
	CString def_prj_folder_lib;
	// поиск
	CString quick_find;
	// замена
	CString quick_replace;
	// группы ключевых слов
	GROUP_KEYWORD* code_grp[8];
	// базовая группа
	STD_GROUP std_group[5];
	// путь к файлу конфигурации
	CString path;
	// массив мастеров
	Array<WIZARD*> wizards;
	// обновление шрифтов
	void UpdateFonts(int type, BOOL is);
};
