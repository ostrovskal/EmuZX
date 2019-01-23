
#pragma once

namespace ssh
{
	class SSH RTTI
	{
	public:
		RTTI() : next(root) { root = this; }
		// создать объект
		virtual RTTI* createRTTI() = 0;
		// вернуть имя класса
		String className() const { return name; }
		// создать
		static RTTI* createClass(const String& nm)
		{
			RTTI* rtti(root);
			while(rtti && rtti->className() != nm) rtti = rtti->next;
			return (rtti ? rtti->createRTTI() : nullptr);
		}
	protected:
		// имя класса
		String name;
		// следующий в списке
		RTTI* next;
		// корень списка
		static RTTI* root;
	};

	#define SSH_RTTI_DECL(cls)			class RTTI_##cls : public RTTI \
										{ \
										public: \
											RTTI_##cls() {name = L#cls;} \
											virtual RTTI* createRTTI() override {return reinterpret_cast<RTTI*>(new cls());} \
										}; \
										static RTTI_##cls cls##_rtti;
	#define SSH_RTTI_IMPL(cls)			cls::RTTI_##cls cls::cls##_rtti;
}
