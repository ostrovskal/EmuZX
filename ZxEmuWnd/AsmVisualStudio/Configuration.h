
#pragma once

class CConfiguration
{
public:
	// �����������
	CConfiguration();
	// ����������
	virtual ~CConfiguration() {}
	// �������� ��������� ���� �������
	void UpdateAllFonts();
	// �������
	void Open(const char* path);
	// ���������
	void Save();
	// ��������� �������� ��������� ����� ������
	void SetBackupArchive(long is) {is_backup_archive = is;}
	// ���������� ����� ����� �������� �� ���������
	void SetDefaultProjectFolder(const char* path) {def_prj_folder = path;}
	// ���������� ����� ����� ���������� ������ �� ���������
	void SetDefaultIncludeFolder(const char* path) {def_prj_folder_inc = path;}
	// ���������� ����� ����� ������������ ������
	void SetDefaultLibraryFolder(const char* path) {def_prj_folder_lib = path;}
	// ���������� ����� ������ ������
	void SetSizeFont(int type, long sz) {fontSizes[type] = sz ; UpdateFonts(type, TRUE);}
	// ���������� ����� ��� ������
	void SetNameFont(int type, const char* name) {fontNames[type] = name ; UpdateFonts(type, TRUE);}
	// �������� �� ������������� ��������� �����
	bool IsExistKeyword(const char* str) const;
	// ������� ������ ������
	long GetSizeFont(int type) const {return fontSizes[type];}
	// ������� ��� �����
	const char* GetFindReplaceWords(bool is_find) const {return (is_find ? quick_find : quick_replace);}
	// ������� ����� �������� �� ���������
	const CString& GetDefaultProjectFolder() const {return def_prj_folder;}
	// ������� ����� ���������� ������ �� ���������
	const CString& GetDefaultIncludeFolder() const {return def_prj_folder_inc;}
	// ������� ����� ������������ ������ �� ���������
	const CString& GetDefaultLibraryFolder() const {return def_prj_folder_lib;}
	// ������� ��� ������
	const char* GetNameFont(int type) const {return fontNames[type];}
	// ������� ������� ��������� ����� ������
	long IsBackupArchive() const {return is_backup_archive;}
	// ������� ��������� �������
	GROUP_FONTS* GetGroupFonts(int type) {return &grp_fonts[type];}
	// ������� ������ ��������� ����
	GROUP_KEYWORD* GetCodeEditorGroup(int index) const {return code_grp[index];}
	// ������� ����������� ������
	STD_GROUP* GetStdGroup(int index) {return &std_group[index];}
	// �������� ����� ������
	WIZARD* AddWizard(const char* name);
	// ��������� ������� �� ��� �����
	WIZARD* GetWizard(const char* name);
	// ����������� ��� �������
	WIZARD* EnumWizards(size_t& index) {if(index >= 0 && index < wizards.size()) return wizards[index++]; return NULL;}
protected:
	// ������� �������� ��������� ����� ������
	long is_backup_archive;
	// ������� �������
	long fontSizes[5];
	// ��������� �������
	GROUP_FONTS grp_fonts[5];
	// ����� �������
	CString fontNames[5];
	// ����� ��� ��������
	CString def_prj_folder;
	// ����� ��� ���������� ������
	CString def_prj_folder_inc;
	// ����� ��� ������������ ������
	CString def_prj_folder_lib;
	// �����
	CString quick_find;
	// ������
	CString quick_replace;
	// ������ �������� ����
	GROUP_KEYWORD* code_grp[8];
	// ������� ������
	STD_GROUP std_group[5];
	// ���� � ����� ������������
	CString path;
	// ������ ��������
	Array<WIZARD*> wizards;
	// ���������� �������
	void UpdateFonts(int type, BOOL is);
};
