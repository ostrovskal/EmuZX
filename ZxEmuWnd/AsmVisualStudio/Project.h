
#pragma once

class CDocumentCommon;
class CPlatform;
class CProject;
class CSolution;

const UINT WM_BUILD_STATUS_BAR_MSG = RegisterWindowMessage(_T("BUILD_STATUS_BAR_MSG"));

class CConfig
{
public:
	// ����������� �� ���������
	CConfig(const char* nm, CPlatform* p) : name(nm), plt(p) {}
	// ����������� �� ��������
	CConfig(const char* nm, const Array<PROPERTY*>& props_asm, const Array<PROPERTY*>& props_lnk, CPlatform* p);
	// ����������
	~CConfig() {options_asm.deleteAll() ; options_link.deleteAll();}
	// ���������� �����
	bool AddOption(PROPERTY* prop, bool is_asm)
	{
		if(GetOption(is_asm, prop->name, 0))
			return false;
		is_asm ? options_asm.add(new PROPERTY(prop)) : options_link.add(new PROPERTY(prop));
		return true;
	}
	// ������� ��� �����
	Array<PROPERTY*>* GetAllOptions(bool is_asm) {return (is_asm ? &options_asm : &options_link);}
	// ������� ��� ����� (� ���� ������)
	CString GetOptions(bool is_asm);
	// ������� ������� ������� �������
	long IsProjectExclude(CProject* prj) const {return (includes.find(prj) == -1);}
	// ���������� ������� ������� �������
	void SetProjectExclude(CProject* prj, long use);
	// ������� ��� ������� ���������� � ������ ������������
	CString GetIncludeProjects() const;
	// ������� ���
	const CString& GetName() const {return name;}
	// ������� �����
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
	// ������������ ���������
	CPlatform* plt;
	// ��� ������������
	CString name;
	// ����� �����������
	Array<PROPERTY*> options_asm;
	// ����� ������������
	Array<PROPERTY*> options_link;
	// ��� �������
	Array<CProject*> includes;
};

class CPlatform
{
public:
	// ����������� �� �����
	CPlatform(const char* nm, CSolution* p) : name(nm), sln(p) {}
	// ����������
	~CPlatform() {configs.deleteAll();}
	// ������� ������������
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
	// ������� ������������
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
	// �������� ������������
	CConfig* AddConfig(const char* name, const Array<PROPERTY*>& props_asm, const Array<PROPERTY*>& props_lnk);
	// ������� ���
	const CString& GetName() const {return name;}
	// ����������� ������������
	CConfig* EnumConfigs(size_t& index) {if(index >= 0 && index < configs.size()) {return configs[index++];} return NULL;}
	// ���������� ������� ����������� ��� �������
	void SetModify();
	// ������� ���������� ������������
	long GetCountConfigs() const {return configs.size();}
protected:
	// ��� ���������
	CString name;
	// ������������ ������
	CSolution* sln;
	// ������ ���� ������������
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
		// ����������� �� ���������
		BLOCK_ELEMENT() : line_begin(0), line_end(0), type(blkUndef) {}
		// ����������� �� ��������
		BLOCK_ELEMENT(const char* nm, const char* val, int y1, int y2, int tp) : name(nm), value(val), line_end(y2), line_begin(y1), type(tp) {}
		// ��������� ����� �����
		int line_begin;
		// �������� ����� �����
		int line_end;
		// ��� �����
		int type;
		// ��� �����
		CString name;
		// �������� �����(��������, ��� equ)
		CString value;
	};
	// �����������
	CElement(const char* pth, CProject* p) : doc(NULL), prj(p), path(pth), x(0), y(0), is_fake(FALSE), is_open(FALSE), is_exclude(FALSE) {}
	// ����������� �� �����
	CElement(const char* pth, const char* pth_files, const char* pth_module, CProject* p) : doc(NULL), prj(p), path_files(pth_files), path_modules(pth_module), path(pth), x(0), y(0), is_fake(FALSE), is_open(FALSE), is_exclude(FALSE) {}
	// ����������
	virtual ~CElement() {block_elems.deleteAll();}
	// �������� ��� ���������� ��������
	void SetBookmark(int num, int status);
	// ���������� ������� �����������
	void SetModify();
	// ������� ������� ���������� �� ������
	long IsExclude() {return is_exclude;}
	// ������� ����
	const CString& GetPath() const {return path;}
	// ���������� ������� ���������� �� ������
	void SetExclude(long is);
	// ��������� ������
	void Parser(CDocumentCommon* doc);
	// ������������ ������
	void DynamicParser(long y1, long y2, long action);
	// ������� ��� �������� � ���� ������
	CString GetBookmarks();
	// ������� ����
	BLOCK_ELEMENT* GetBlock(const char* name, BlockElementTypes type);
	// ����������� �����
	BLOCK_ELEMENT* EnumBlocks(size_t& index) {return ((index >= 0 && index < block_elems.size()) ? block_elems[index++] : NULL);}
	// ���� �� �����
	CString path;
	// ���� � ������ ������
	CString path_files;
	// ���� � ������ �������
	CString path_modules;
	// ������ �������� ������ ���������
	Array<BLOCK_ELEMENT*> block_elems;
	// ������� �������
	long x, y;
	// ������� ���������
	long is_open;
	// ������� ������������
	long is_exclude;
	// ������� ���������
	long is_fake;
	// ��������
	Array<BOOKMARK> bookmarks;
	// ������
	CProject* prj;
	// ��������
	CDocumentCommon* doc;
};

class CProject
{
public:
	friend class CSolution;
public:
	// �����������
	CProject() {init();}
	// ����������
	virtual ~CProject() {init();}
	// ���������� ���
	void SetName(const char* nm) {name = nm;}
	// ���������� ������� �����������
	void SetModify();
	// ������� ���
	const CString& GetName() const {return name;}
	// ������� ����
	const CString& GetPath() const {return path;}
	// ������� ������� ��������
	bool IsOpen() const {return is_open;}
	// �������� �� ���������� �����
	bool IsElementPath(const char* path, bool is_files);
	// ��������
	long GetCountElements() const {return elements.size();}
	// ��������
	CElement* AddElement(const char* path);
	// �������
	bool DeleteElement(const char* path);
	// �������
	CElement* GetElement(const char* path, const char* path_files, const char* path_modules, size_t* index = NULL);
	// ����������� ��������
	CElement* EnumElements(size_t& index) {return ((index >= 0 && index < elements.size()) ? elements[index++] : NULL);}
	// ������� ������ �������
	const CString& GetStartParams() const {return cmd_params;}
	// ���������� ������ �������
	void SetStartParams(const char* cmd) {cmd_params = cmd;}
protected:
	// ������� ��������������
	// �������
	bool Create(const char* path, const char* name, WIZARD* wzd);
	// �������
	bool Open(const char* path);
	// ���������
	bool Save();
	// ��������
	void MiniSave();
	// �������
	void Close() {init();}
	// ��������� �������������
	void init();
	// ������� ��������\��������
	bool is_open;
	// ���
	CString name;
	// ���� �� �����
	CString path;
	// ��� ������� (���������� ��� �������)
	CString name_wizard;
	// ��������� �������
	CString cmd_params;
	// �������� (�����)
	Array<CElement*> elements;
};

class CSolution
{
	friend DWORD WINAPI ThreadBuildProc(void* param);
	friend DWORD WINAPI ThreadExecuteProc(void* param);
public:
	// �����������
	CSolution();
	// ����������
	virtual ~CSolution() {Init();}
	// ���������� ������� �����������
	void SetModify() {is_modify = true;}
	// ������� ������� ����������� �������
	bool IsModify() const {return is_modify;}
	// ��������
	bool Open(const char* path);
	// ����������
	bool Save();
	// ����������
	void MiniSave();
	// �������
	bool Build();
	// ������� ������
	bool BuildProject(CProject* prj);
	// ���������
	bool Execute();
	// �����������
	bool Rebuild();
	// ��������
	bool Close();
	// �������� ���������
	CPlatform* AddPlatform(WIZARD* wzd);
	// ������� ���������
	bool DeletePlatform(const char* name);
	// ������� ������� �������
	CElement* GetCurrentElement() const {return current_element;}
	// ������� ���������
	CPlatform* GetPlatform(const char* name, size_t* index = NULL);
	// ������� ��� ��������� ���������
	Array<CPlatform*>* GetAllPlatforms() {return &platforms;}
	// �������� ������
	CProject* AddProject(const char* name_sln, const char* name_prj, const char* path, WIZARD* wzd, int action);
	// ������� ������
	bool DeleteProject(const char* name);
	// ���������� ������� ������
	CProject* SetCurrentProject(const char* name);
	// ���������� ������ �� ������
	void SetExecuteProject(const char* name);
	// ��������� ����������� ��������
	void ReorderProjects(const Array<CString>& arr);
	// ���������� ���� � ������
	void SetArchivePath(const char* pth) {path_archive = pth;}
	// ������� ���������� ��������
	long GetCountPlatforms() const {return platforms.size();}
	// ���������� ������� �������
	void SetCurrentElement(CElement* elem);
	// ���������� ������� ���������
	void SetCurrentPlatform(const char* name)
	{
		if(GetEnvVar(SF_PLATFORM_NAME) != name)
		{
			SetEnvVar(SF_PLATFORM_NAME, name);
		}
	}
	// ���������� ������� ������������
	void SetCurrentConfig(const char* name)
	{
		if(GetEnvVar(SF_CONFIGURATION_NAME) != name)
		{
			SetEnvVar(SF_CONFIGURATION_NAME, name);
			SetEnvVar(SF_CONFIGURATION_PATH, GetEnvVar(SF_PROJECT_PATH) + "\\" + GetEnvVar(SF_CONFIGURATION_NAME));
		}
	}
	// ������� ������� ���������
	const char* GetCurrentPlatform() const {return GetEnvVar(SF_PLATFORM_NAME);}
	// ������� ������� ������������
	const char* GetCurrentConfig() const {return GetEnvVar(SF_CONFIGURATION_NAME);}
	// ������� ���� �� ������
	const char* GetArchivePath() const {return path_archive;}
	// ������� ���
	CString GetName();
	// ������� ���������� ��������
	size_t GetCountProjects() const {return projects.size();}
	// ������������ ��������
	CProject* EnumProjects(size_t& index) {return ((index >= 0 && index < projects.size()) ? projects[index++] : NULL);}
	// ������� ������ �� ������
	CProject* GetExecuteProject() {return execute_project;}
	// �������� ������� ������
	CProject* GetCurrentProject() {return current_project;}
	// ����� ������
	CProject* GetProject(const char* name, int* index = NULL);
	// ���������� ��������� ���������
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
	// ������� ����� �� ������� ������������ � ���������
	CString GetCurrentSpecificOption(long flags);
	// ����������� ���� ���������
	CString EnumElementsPath(bool is_asm, CProject* prj);
	// ��������� �������
	void Process(const char* cmd_line, HANDLE hOut, STARTUPINFO* si);
	// ������������ ��������� �������
	void MakeFilter();
	// ������ �������� ������(���������� ���������)
	void BeginBuild(CProject* prj);
	// ���������� �������� ������(������������� ���������)
	void FinishBuild(CProject* prj, int nProjects);
	// �������������
	void Init();
	// ������� �����������
	bool is_modify;
	// ���� �� ������
	CString path_archive;
	// ����
	CString path;
	// ������� �������
	CElement* current_element;
	// ������� ������
	CProject* current_project;
	// ������ �� ������
	CProject* execute_project;
	// ������ ��������
	Array<CProject*> projects;
	// ���������
	Array<CPlatform*> platforms;
	// ������ �� ���������
	WIZARD default_wizard;
	// �������� ������
	CProject fake_prj;
	// ���������� ������ ����� ��������� ������
	long nErrors;
};

