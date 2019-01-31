
#pragma once

class CDocumentCommon;
class CPlatform;
class CProject;
class CSolution;

const UINT WM_BUILD_STATUS_BAR_MSG = RegisterWindowMessage(_T("BUILD_STATUS_BAR_MSG"));

class CConfig
{
public:
	// конструктор по умолчанию
	CConfig(const char* nm, CPlatform* p) : name(nm), plt(p) {}
	// конструктор по значению
	CConfig(const char* nm, const Array<PROPERTY*>& props_asm, const Array<PROPERTY*>& props_lnk, CPlatform* p);
	// деструктор
	~CConfig() {options_asm.deleteAll() ; options_link.deleteAll();}
	// установить опцию
	bool AddOption(PROPERTY* prop, bool is_asm)
	{
		if(GetOption(is_asm, prop->name, 0))
			return false;
		is_asm ? options_asm.add(new PROPERTY(prop)) : options_link.add(new PROPERTY(prop));
		return true;
	}
	// вернуть все опции
	Array<PROPERTY*>* GetAllOptions(bool is_asm) {return (is_asm ? &options_asm : &options_link);}
	// вернуть все опции (в виде строки)
	CString GetOptions(bool is_asm);
	// вернуть признак наличия проекта
	long IsProjectExclude(CProject* prj) const {return (includes.find(prj) == -1);}
	// установить признак наличия проекта
	void SetProjectExclude(CProject* prj, long use);
	// вернуть все проекта включенные в данную конфигурацию
	CString GetIncludeProjects() const;
	// вернуть имя
	const CString& GetName() const {return name;}
	// вернуть опцию
	PROPERTY* GetOption(bool is_asm, const char* name, long flags)
	{
		PROPERTY* prop;
		Array<PROPERTY*>* props(is_asm ? &options_asm : &options_link);
		for(size_t i = 0 ; i < props->size() ; i++)
		{
			prop = props->get(i);
			bool is(false);
			if(name ? (prop->name == name) : ((prop->flags & flags) == flags))
				return prop;
		}
		return NULL;
	}
protected:
	// родительская платформа
	CPlatform* plt;
	// имя конфигурации
	CString name;
	// опции транслятора
	Array<PROPERTY*> options_asm;
	// опции компоновщика
	Array<PROPERTY*> options_link;
	// все проекты
	Array<CProject*> includes;
};

class CPlatform
{
public:
	// конструктор по имени
	CPlatform(const char* nm, CSolution* p) : name(nm), sln(p) {}
	// деструктор
	~CPlatform() {configs.deleteAll();}
	// вернуть конфигурацию
	CConfig* GetConfig(const char* name, size_t* index = NULL)
	{
		CConfig* cfg;

		if(name)
		{
			for(size_t i = 0 ; i < configs.size() ; i++)
			{
				cfg = configs[i];
				if(cfg->GetName() == name)
				{
					if(index) *index = i;
					return cfg;
				}
			}
		}
		return NULL;
	}
	// удалить конфигурацию
	bool DeleteConfig(const char* name)
	{
		size_t index;

		if(GetConfig(name, &index))
		{
			configs.del(index);
			return true;
		}
		return false;
	}
	CConfig* AddConfig(const char* nm)
	{
		CConfig* cfg;
		if(!(cfg = GetConfig(nm)))
		{
			cfg = new CConfig(nm, this);
			configs.add(cfg);
		}
		return cfg;
	}
	// добавить конфигурацию
	CConfig* AddConfig(const char* name, const Array<PROPERTY*>& props_asm, const Array<PROPERTY*>& props_lnk);
	// вернуть имя
	const CString& GetName() const {return name;}
	// перечислить конфигурации
	CConfig* EnumConfigs(size_t& index) {if(index >= 0 && index < configs.size()) {return configs[index++];} return NULL;}
	// установить признак модификации для проекта
	void SetModify();
	// вернуть количество конфигураций
	long GetCountConfigs() const {return configs.size();}
protected:
	// имя платформы
	CString name;
	// родительский проект
	CSolution* sln;
	// массив всех конфигураций
	Array<CConfig*> configs;
};

class CElement
{
public:
	enum BlockElementTypes
	{
		blkUndef,
		blkProc,
		blkStruct,
		blkEqu,
		blkEndp,
		blkEnds
	};
	struct BLOCK_ELEMENT
	{
		// конструктор по умолчанию
		BLOCK_ELEMENT() : line_begin(0), line_end(0), type(blkUndef) {}
		// конструктор по значению
		BLOCK_ELEMENT(const char* nm, const char* val, int y1, int y2, int tp) : name(nm), value(val), line_end(y2), line_begin(y1), type(tp) {}
		// начальная линия блока
		int line_begin;
		// конечная линия блока
		int line_end;
		// тип блока
		int type;
		// имя блока
		CString name;
		// значение блока(например, для equ)
		CString value;
	};
	// конструктор
	CElement(const char* pth, CProject* p) : doc(NULL), prj(p), path(pth), x(0), y(0), is_fake(FALSE), is_open(FALSE), is_exclude(FALSE) {}
	// конструктор по имени
	CElement(const char* pth, const char* pth_files, const char* pth_module, CProject* p) : doc(NULL), prj(p), path_files(pth_files), path_modules(pth_module), path(pth), x(0), y(0), is_fake(FALSE), is_open(FALSE), is_exclude(FALSE) {}
	// деструктор
	virtual ~CElement() {block_elems.deleteAll();}
	// добавить или установить закладку
	void SetBookmark(int num, int status);
	// установить признак модификации
	void SetModify();
	// вернуть признак исключения из сборки
	long IsExclude() {return is_exclude;}
	// вернуть путь
	const CString& GetPath() const {return path;}
	// установить признак исключения из сборки
	void SetExclude(long is);
	// начальный пармер
	void Parser(CDocumentCommon* doc);
	// динамический парсер
	void DynamicParser(long y1, long y2, long action);
	// вернуть все закладки в виде строки
	CString GetBookmarks();
	// вернуть блок
	BLOCK_ELEMENT* GetBlock(const char* name, BlockElementTypes type);
	// перечислить блоки
	BLOCK_ELEMENT* EnumBlocks(size_t& index) {return ((index >= 0 && index < block_elems.size()) ? block_elems[index++] : NULL);}
	// путь на диске
	CString path;
	// путь в панели файлов
	CString path_files;
	// путь в панели модулей
	CString path_modules;
	// массив структур блоков элементов
	Array<BLOCK_ELEMENT*> block_elems;
	// позиция каретки
	long x, y;
	// признак открытого
	long is_open;
	// признак исключенного
	long is_exclude;
	// признак фэйкового
	long is_fake;
	// закладки
	Array<BOOKMARK> bookmarks;
	// проект
	CProject* prj;
	// документ
	CDocumentCommon* doc;
};

class CProject
{
public:
	friend class CSolution;
public:
	// конструктор
	CProject() {init();}
	// деструктор
	virtual ~CProject() {init();}
	// установить имя
	void SetName(const char* nm) {name = nm;}
	// установить признак модификации
	void SetModify();
	// вернуть имя
	const CString& GetName() const {return name;}
	// вернуть путь
	const CString& GetPath() const {return path;}
	// вернуть признак открытия
	bool IsOpen() const {return is_open;}
	// проверка на совпадение путей
	bool IsElementPath(const char* path, bool is_files);
	// элементы
	long GetCountElements() const {return elements.size();}
	// добавить
	CElement* AddElement(const char* path);
	// удалить
	bool DeleteElement(const char* path);
	// вернуть
	CElement* GetElement(const char* path, const char* path_files, const char* path_modules, size_t* index = NULL);
	// перечислить элементы
	CElement* EnumElements(size_t& index) {return ((index >= 0 && index < elements.size()) ? elements[index++] : NULL);}
	// вернуть строку запуска
	const CString& GetStartParams() const {return cmd_params;}
	// установить строку запуска
	void SetStartParams(const char* cmd) {cmd_params = cmd;}
protected:
	// базовое взаимодействие
	// создать
	bool Create(const char* path, const char* name, WIZARD* wzd);
	// открыть
	bool Open(const char* path);
	// сохранить
	bool Save();
	// обновить
	void MiniSave();
	// закрыть
	void Close() {init();}
	// начальная инициализация
	void init();
	// признак открытия\создания
	bool is_open;
	// имя
	CString name;
	// путь на диске
	CString path;
	// имя мастера (фактически тип проекта)
	CString name_wizard;
	// параметры запуска
	CString cmd_params;
	// элементы (файлы)
	Array<CElement*> elements;
};

class CSolution
{
	friend DWORD WINAPI ThreadBuildProc(void* param);
	friend DWORD WINAPI ThreadExecuteProc(void* param);
public:
	// конструктор
	CSolution();
	// деструктор
	virtual ~CSolution() {Init();}
	// установить признак модификации
	void SetModify() {is_modify = true;}
	// вернуть признак модификации системы
	bool IsModify() const {return is_modify;}
	// открытие
	bool Open(const char* path);
	// сохранение
	bool Save();
	// обновление
	void MiniSave();
	// собрать
	bool Build();
	// собрать проект
	bool BuildProject(CProject* prj);
	// выполнить
	bool Execute();
	// пересобрать
	bool Rebuild();
	// закрытие
	bool Close();
	// добавить платформу
	CPlatform* AddPlatform(WIZARD* wzd);
	// удалить платформу
	bool DeletePlatform(const char* name);
	// вернуть текущий элемент
	CElement* GetCurrentElement() const {return current_element;}
	// вернуть платформу
	CPlatform* GetPlatform(const char* name, size_t* index = NULL);
	// вернуть все доступные платформы
	Array<CPlatform*>* GetAllPlatforms() {return &platforms;}
	// добавить проект
	CProject* AddProject(const char* name_sln, const char* name_prj, const char* path, WIZARD* wzd, int action);
	// удалить проект
	bool DeleteProject(const char* name);
	// установить текущий проект
	CProject* SetCurrentProject(const char* name);
	// установить проект на запуск
	void SetExecuteProject(const char* name);
	// изменение очередности проектов
	void ReorderProjects(const Array<CString>& arr);
	// установить путь к архиву
	void SetArchivePath(const char* pth) {path_archive = pth;}
	// вернуть количество платформ
	long GetCountPlatforms() const {return platforms.size();}
	// установить текущий элемент
	void SetCurrentElement(CElement* elem);
	// установить текущую платформу
	void SetCurrentPlatform(const char* name)
	{
		if(GetEnvVar(SF_PLATFORM_NAME) != name)
		{
			SetEnvVar(SF_PLATFORM_NAME, name);
		}
	}
	// установить текущую конфигурацию
	void SetCurrentConfig(const char* name)
	{
		if(GetEnvVar(SF_CONFIGURATION_NAME) != name)
		{
			SetEnvVar(SF_CONFIGURATION_NAME, name);
			SetEnvVar(SF_CONFIGURATION_PATH, GetEnvVar(SF_PROJECT_PATH) + "\\" + GetEnvVar(SF_CONFIGURATION_NAME));
		}
	}
	// вернуть текущую платформу
	const char* GetCurrentPlatform() const {return GetEnvVar(SF_PLATFORM_NAME);}
	// вернуть текущую конфигурацию
	const char* GetCurrentConfig() const {return GetEnvVar(SF_CONFIGURATION_NAME);}
	// вернуть путь до архива
	const char* GetArchivePath() const {return path_archive;}
	// вернуть имя
	CString GetName();
	// вернуть количество проектов
	size_t GetCountProjects() const {return projects.size();}
	// перечисление проектов
	CProject* EnumProjects(size_t& index) {return ((index >= 0 && index < projects.size()) ? projects[index++] : NULL);}
	// вернуть проект на запуск
	CProject* GetExecuteProject() {return execute_project;}
	// получить текущий проект
	CProject* GetCurrentProject() {return current_project;}
	// найти проект
	CProject* GetProject(const char* name, int* index = NULL);
	// добавление фэйкового документа
	CElement* AddFakeElement(const char* path)
	{
		CElement* elem;
		bool is(is_modify);
		elem = fake_prj.AddElement(path);
		if(elem) elem->is_fake = true;
		is_modify = is;
		return elem;
	}
protected:
	// вернуть опцию из текущей конфигурации и платформы
	CString GetCurrentSpecificOption(long flags);
	// перечислить пути элементов
	CString EnumElementsPath(bool is_asm, CProject* prj);
	// обработка консоли
	void Process(const char* cmd_line, HANDLE hOut, STARTUPINFO* si);
	// сформировать структуру фильтра
	void MakeFilter();
	// запуск процесса сборки(блокировка элементов)
	void BeginBuild(CProject* prj);
	// завершение процесса сборки(разблокировка элементов)
	void FinishBuild(CProject* prj, int nProjects);
	// инициализация
	void Init();
	// признак модицикации
	bool is_modify;
	// путь до архива
	CString path_archive;
	// путь
	CString path;
	// текущий элемент
	CElement* current_element;
	// текущий проект
	CProject* current_project;
	// проект на запуск
	CProject* execute_project;
	// массив проектов
	Array<CProject*> projects;
	// платформы
	Array<CPlatform*> platforms;
	// мастер по умолчанию
	WIZARD default_wizard;
	// фэйковый проект
	CProject fake_prj;
	// количество ошибок после последней сборки
	long nErrors;
};

