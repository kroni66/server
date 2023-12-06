/*
* Filename: attendance.h
* Copyright (C) 2023 Owsap Development
*
* This file is part of the Attendance System
* Version 2.0
*
* Discord: Owsap#0905
* Skype: owsap.
*
* Web: https://owsap.dev/
* GitHub: https://github.com/Owsap
*/

#if !defined(_INC_ATTENDANCE_SYSTEM_H_) && defined(__ATTENDANCE_SYSTEM__)
#define _INC_ATTENDANCE_SYSTEM_H_

#include "../../common/tables.h"

class CAttendance
{
public:
	enum EItemState
	{
		ITEM_STATE_NONE,
		ITEM_STATE_OPEN,
		ITEM_STATE_SKIP,
		ITEM_STATE_TAKE
	};

public:
	CAttendance(const LPCHARACTER pkChar, const TAttendanceTable* pTable);
	~CAttendance();

	static void Create(const LPCHARACTER pkChar, TAttendanceTable* pTable);
	static void Close(const LPCHARACTER pkChar);
	static void Open(const LPCHARACTER pkChar);
	static void Receive(const LPCHARACTER pkChar);
	static void Process();

public:
	void GetCurrentDate(int* pMonth, int* pDay);
	void Load(const TAttendanceTable* pTable);
	void Take();

private:
	LPCHARACTER m_pkChar;
	LPEVENT m_pLoadEvent;
	TAttendanceTable m_Table;
};

#endif // _INC_ATTENDANCE_SYSTEM_H_
