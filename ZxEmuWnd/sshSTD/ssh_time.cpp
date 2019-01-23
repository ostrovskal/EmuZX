
#include "stdafx.h"
#include "ssh_time.h"

namespace ssh
{
	static ssh_cws ����������[] = {L"�����������", L"�������", L"�����", L"�������", L"�������", L"�������", L"�����������"};
	static ssh_cws ����������[] = {L"��", L"��", L"��", L"��", L"��", L"��", L"��"};
	static ssh_cws �������[]	= {L"������", L"�������", L"�����", L"������", L"���", L"����", L"����", L"�������", L"��������", L"�������", L"������", L"�������"};
	static ssh_cws �������[]	= {L"���", L"���", L"���", L"���", L"���", L"���", L"���", L"���", L"���", L"���", L"���", L"���"};
	static ssh_cws �������������[] = {L"��������", L"��������", L"��������", L"�������", L"�����", L"�������", L"������", L"��������", L"���������", L"��������", L"�������", L"�������"};
	static ssh_cws to[]			= {L"%a", L"%A", L"%b", L"%B", nullptr};

	/*
	%�	����������� ������������ ��� ������*
	%�	������ ������������ ��� ������*
	%b	����������� �������� ������*
	%�	������ �������� ������*(������� �����)
	%�	����������� ������ ���� � �������*
	%d	���� ������ � ���������� ���������� (1�31)
	%�	��� ��� � �������� (0�23)
	%I	��� ��� � �������� (1�12)
	%j	���� � ���� � ���������� ������������� (1�366)
	%m	����� � ���������� ������������� (1�12)
	%�	������ � ���������� ������������� (0�59)
	%�	��������� ���������� ��� AM ��� ��
	%S	������� � ���������� ������������� (0�61)
	%u	ISO 8601 ���� ������ � ������������	(1-7)
	%W	������ � ����, ����������� ������ ������ ���� (0�53)
	%�	����������� ������ ����*
	%�	����������� ������ �������*
	%�	��� � ���������� ������������� � �������� �������� (00�99)
	%Y	���, ������� ��������, � ���������� �������������
	%z	ISO 8601 �������� �� �������� +300
	%Z	�������� ��������� ����
	*/
	String Time::fmt(String str) const
	{
		static wchar_t buf[512];
		if(*(ssh_d*)ssh_locale()->int_curr_symbol == 'BUR')
		{
			int _month(month() - 1), _dw(dayOfWeek());
			String tmp(ssh_printf(L"%s;%s;%s;%s", ����������[_dw], ����������[_dw], �������[_month], �������������[_month]));
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
		// �������� 1 ������ �������� ����
		Time bt(year(), 1, 1, 0, 0, 0);
		// ��������� ������� ����� ������� ����� � ������� ����
		time_t wd(bt.getTime() - (bt.dayOfWeek() * 24 * 60 * 60));
		// ��������� ����� ������
		return (int)((time - wd) / (7 * 24 * 60 * 60));
	}
}
