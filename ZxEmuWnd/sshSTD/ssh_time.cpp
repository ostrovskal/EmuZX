
#include "stdafx.h"
#include "ssh_time.h"

namespace ssh
{
	static ssh_cws ДниНеделиД[] = {L"Понедельник", L"Вторник", L"Среда", L"Четверг", L"Пятница", L"Суббота", L"Воскресенье"};
	static ssh_cws ДниНеделиК[] = {L"Пн", L"Вт", L"Ср", L"Чт", L"Пт", L"Сб", L"Вс"};
	static ssh_cws МесяцыД[]	= {L"Января", L"Февраля", L"Марта", L"Апреля", L"Мая", L"Июня", L"Июля", L"Августа", L"Сентября", L"Октября", L"Ноября", L"Декабря"};
	static ssh_cws МесяцыК[]	= {L"Янв", L"Фев", L"Мар", L"Апр", L"Май", L"Июн", L"Июл", L"Авг", L"Сен", L"Окт", L"Ноя", L"Дек"};
	static ssh_cws СмешныеМесяцы[] = {L"Пьянваря", L"Фигвраля", L"Кошмарта", L"Сопреля", L"Сымая", L"Теплюня", L"Жарюля", L"Авгрустя", L"Свистября", L"Моктября", L"Гноября", L"Дубабря"};
	static ssh_cws to[]			= {L"%a", L"%A", L"%b", L"%B", nullptr};

	/*
	%а	Сокращенное наименование дня недели*
	%А	Полное наименование дня недели*
	%b	Сокращенное название месяца*
	%В	Полное название месяца*(смешной месяц)
	%с	Стандартная строка даты и времени*
	%d	День месяца в десятичном исчислении (1—31)
	%Н	Час дня в пределах (0—23)
	%I	Час дня в пределах (1—12)
	%j	День в году в десятичном представлении (1—366)
	%m	Месяц в десятичном представлении (1—12)
	%М	Минута в десятичном представлении (0—59)
	%р	Локальный эквивалент для AM или РМ
	%S	Секунды в десятичном представлении (0—61)
	%u	ISO 8601 день недели с понедельника	(1-7)
	%W	Неделя в году, понедельник служит первым днем (0—53)
	%х	Стандартная строка даты*
	%Х	Стандартная строка времени*
	%у	Год в десятичном представлении в пределах столетия (00—99)
	%Y	Год, включая столетия, в десятичном представлении
	%z	ISO 8601 смещение по гринвичу +300
	%Z	Название временной зоны
	*/
	String Time::fmt(String str) const
	{
		static wchar_t buf[512];
		if(*(ssh_d*)ssh_locale()->int_curr_symbol == 'BUR')
		{
			int _month(month() - 1), _dw(dayOfWeek());
			String tmp(ssh_printf(L"%s;%s;%s;%s", ДниНеделиК[_dw], ДниНеделиД[_dw], МесяцыК[_month], СмешныеМесяцы[_month]));
			tmp.replace(L';', L'\0');
			str.replace(to, tmp);
		}
		return String(buf, wcsftime(buf, 512, str.str(), local()));
	}

	int	Time::dayOfWeek() const
	{
		static int arr_week_days[] = {6, 0, 1, 2, 3, 4, 5};
		return arr_week_days[local()->tm_wday];
	}

	Time::Time(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST)
	{
		struct tm atm { nSec, nMin, nHour, nDay, nMonth - 1, nYear - 1900, 0, 0, nDST };
		time = _mktime64(&atm);
	}

	Time::Time(const SYSTEMTIME& sysTime, int nDST)
	{
		Time timeT(sysTime.wYear < 1900 ? 0L : ((int)sysTime.wYear, (int)sysTime.wMonth, (int)sysTime.wDay, (int)sysTime.wHour, (int)sysTime.wMinute, (int)sysTime.wSecond, nDST));
		*this = timeT;
	}

	Time::Time(const FILETIME& fileTime, int nDST)
	{
		FILETIME localTime;
		SYSTEMTIME sysTime;
		if(FileTimeToLocalFileTime(&fileTime, &localTime) && FileTimeToSystemTime(&localTime, &sysTime))
			*this = Time(sysTime, nDST);
	}

	struct tm* Time::local() const
	{
		static struct tm ptm;
		localtime_s(&ptm, &time);
		return &ptm;
	}

	struct tm* Time::gmt() const
	{
		static struct tm ptm;
		gmtime_s(&ptm, &time);
		return &ptm;
	}

	SYSTEMTIME Time::getAsSystemTime() const
	{
		struct tm* ptm(local());
		SYSTEMTIME timeDest;
		timeDest.wYear = (WORD)(1900 + ptm->tm_year);
		timeDest.wMonth = (WORD)(1 + ptm->tm_mon);
		timeDest.wDayOfWeek = (WORD)ptm->tm_wday;
		timeDest.wDay = (WORD)ptm->tm_mday;
		timeDest.wHour = (WORD)ptm->tm_hour;
		timeDest.wMinute = (WORD)ptm->tm_min;
		timeDest.wSecond = (WORD)ptm->tm_sec;
		timeDest.wMilliseconds = 0;
		return timeDest;
	}

	int Time::weekOfYear() const
	{
		// получаем 1 января текущего года
		Time bt(year(), 1, 1, 0, 0, 0);
		// вычисляем разницу между текущей датой и началом года
		time_t wd(bt.getTime() - (bt.dayOfWeek() * 24 * 60 * 60));
		// вычисляем номер недели
		return (int)((time - wd) / (7 * 24 * 60 * 60));
	}
}
