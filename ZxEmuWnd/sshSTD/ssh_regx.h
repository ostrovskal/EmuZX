
#pragma once

namespace ssh
{
	class SSH Regx
	{
	public:
		// конструктор по умолчанию
		Regx() { ssh_memzero(patterns, sizeof(patterns)); memset(vector, 0, sizeof(vector)); }
		// инициализирующий конструктор
		Regx(ssh_cws* pattern, ssh_u count) : Regx()
		{
			ssh_u idx(0);
			while(idx < count) set_pattern(idx, pattern[idx]), idx++;
		}
		// деструктор
		virtual ~Regx()
		{
			if(ssh_regx_free)
			{
				if(re) ssh_regx_free(re);
				for(ssh_u i = 0; i < 32; i++)
					if(patterns[i]) ssh_regx_free(patterns[i]);
			}
		}
		// запомнить паттерн в массиве
		bool set_pattern(ssh_u idx, ssh_cws pattern)
		{
			if(idx < 32) return ((patterns[idx] = compile(pattern)) != nullptr);
			return false;
		}
		// найти совпадени€ без компил€ции паттерна
		ssh_l match(const String& subject, ssh_l idx, ssh_u idx_ptrn = -1)
		{
			if(ssh_regx_exec)
			{
				subj = subject;
				return (result = ssh_regx_exec((idx_ptrn == -1 ? re : patterns[idx_ptrn]), subject, ssh_wcslen(subject), idx, 0, vector, 256));
			}
			return 0;
		}
		// найти совпадени€ с компил€цией паттерна
		ssh_l match(const String& subject, const String& pattern, ssh_l idx)
		{
			return ((re = compile(pattern)) ? match(subject, idx) : 0);
		}
		// вернуть подстроку по результатам последней операции
		String substr(ssh_l idx)
		{
			return subj.substr(vec(idx, 0), len(idx));
		}
		// заменить с компил€цией паттерна
		bool replace(String& subject, const String& pattern, ssh_cws repl, ssh_l idx)
		{
			return ((re = compile(pattern)) ? replace(subject, repl, idx, -1) : false);
		}
		// заменить без компил€ции паттерна
		bool replace(String& subject, ssh_cws repl, ssh_l idx, ssh_u idx_ptrn)
		{
			ssh_l nWcs(ssh_wcslen(repl));
			while(match(subject, idx, idx_ptrn) > 0)
			{
				idx = vector[0];
				subject.remove(idx, vector[1] - idx);
				subject.insert(idx, repl);
				idx += nWcs;
			}
			return true;
		}
		// вернуть количество найденных совпадений
		ssh_l count() const { return result; }
		// вернуть индекс в массиве совпадений
		ssh_l vec(ssh_l idx, ssh_l offs) const { return (idx < result ? vector[idx * 2 + offs] : -1); }
		// вернуть длину в массиве совпадений
		ssh_l len(ssh_l idx) const { return (idx < result ? (vector[idx * 2 + 1] - vector[idx * 2]) : 0); }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
	protected:
		// компилировать
		regex16* compile(ssh_cws pattern)
		{
			result = 0;
			if(re && ssh_regx_free) { ssh_regx_free(re); re = nullptr; }
			return (ssh_regx_compile ? ssh_regx_compile(pattern, 0) : nullptr);
		}
		// субъект поиска
		String subj;
		// найденные позиции
		ssh_l vector[256];
		// всего найденных
		ssh_l result = 0;
		// временный паттерн
		regex16* re = nullptr;
		// откомпилированные паттерны
		regex16* patterns[32];
	};
}
