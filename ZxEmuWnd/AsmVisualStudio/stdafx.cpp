
#include "stdafx.h"

// массив переменных окружения среды
SOLUTION_FILTER arr_env_var[SF_LAST_ENV_VAR];

CString GetEnvVar(int index)
{
	return ((index >= 0 && index < SF_LAST_ENV_VAR) ? arr_env_var[index].value : "");
}

void SetEnvVar(int index, const char* value)
{
	if(index >= 0 && index < SF_LAST_ENV_VAR)
		arr_env_var[index].value = value;
}

CString ApplyFilter(const char* txt)
{
	CString ntxt(txt);
	// прогоняем текст на замену
	for(int i = 0 ; i < SF_LAST_ENV_VAR ; i++)
		ntxt.Replace(arr_env_var[i].name, arr_env_var[i].value);
	return ntxt;
}
