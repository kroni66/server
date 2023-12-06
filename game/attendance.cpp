/*
* Filename: attendance.cpp
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

#include "stdafx.h"

#if defined(__ATTENDANCE_SYSTEM__)
#include "attendance.h"
#include "char.h"
#include "item_manager.h"
#include "questmanager.h"
#include "desc.h"
#include "desc_client.h"
#include "buffer_manager.h"
#include "config.h"

/*
* NOTE: While the test server is enable the duration of the
* attendance is 28 minutes simulating the 28 days, each day
* in the test server is equal to one minute.
*/

static constexpr int s_iLoadPulse = 60;

EVENTINFO(AttendanceEventInfo)
{
	const LPCHARACTER m_pkChar;
	AttendanceEventInfo(const LPCHARACTER pkChar) : m_pkChar(pkChar) {}
};

EVENTFUNC(AttendanceEvent)
{
	const AttendanceEventInfo* pkInfo = reinterpret_cast<AttendanceEventInfo*>(event->info);
	if (pkInfo == nullptr)
	{
		sys_err("AttendanceEventInfo> <Factor> nullptr");
		return 0;
	}

	const LPCHARACTER pkChar = pkInfo->m_pkChar;
	if (pkChar == nullptr)
		return 0;

	const LPDESC pkDesc = pkChar->GetDesc();
	if (pkDesc == nullptr)
		return 0;

	TAttendanceTable AttendanceTable = {};
	AttendanceTable.dwPID = pkChar->GetPlayerID();
	db_clientdesc->DBPacket(HEADER_GD_ATTENDANCE_LOAD, pkDesc->GetHandle(), &AttendanceTable, sizeof(AttendanceTable));

	return PASSES_PER_SEC(s_iLoadPulse);
}

CAttendance::CAttendance(const LPCHARACTER pkChar, const TAttendanceTable* pTable)
	: m_pkChar(pkChar)
{
	Load(pTable);

	if (m_pLoadEvent)
		event_cancel(&m_pLoadEvent);

	m_pLoadEvent = event_create(AttendanceEvent, new AttendanceEventInfo(pkChar), PASSES_PER_SEC(s_iLoadPulse));
}

CAttendance::~CAttendance()
{
	std::memset(&m_Table, 0, sizeof(m_Table));

	if (m_pLoadEvent)
		event_cancel(&m_pLoadEvent);
}

void CAttendance::Create(const LPCHARACTER pkChar, TAttendanceTable* pTable)
{
	if (pkChar == nullptr)
		return;

	const LPDESC pkDesc = pkChar->GetDesc();
	if (pkDesc == nullptr)
		return;

	if (pkChar->GetAttendance())
		return;

	if (pTable == nullptr)
	{
		sys_err("CAttendance::Create> <Character> (%s %p) <Table> (nullptr)", pkChar->GetName(), get_pointer(pkChar));
		return;
	}

	pkChar->SetAttendance(new CAttendance(pkChar, pTable));
}

void CAttendance::Close(const LPCHARACTER pkChar)
{
	pkChar->SetAttendance(nullptr);
}

void CAttendance::Open(const LPCHARACTER pkChar)
{
	if (pkChar == nullptr)
		return;

	const LPDESC pkDesc = pkChar->GetDesc();
	if (pkDesc == nullptr)
		return;

	if (pkChar->GetAttendance())
		return;

	TAttendanceTable AttendanceTable = {};
	AttendanceTable.dwPID = pkChar->GetPlayerID();
	db_clientdesc->DBPacket(HEADER_GD_ATTENDANCE_LOAD, pkDesc->GetHandle(), &AttendanceTable, sizeof(AttendanceTable));
}

void CAttendance::Receive(const LPCHARACTER pkChar)
{
	if (pkChar == nullptr)
		return;

	CAttendance* pAttendance = pkChar->GetAttendance();
	if (pAttendance != nullptr)
		pAttendance->Take();
}

void CAttendance::Take()
{
	if (m_pkChar == nullptr)
	{
		sys_err("CAttendance::Receive <Character> (nullptr)");
		return;
	}

	const LPDESC pkDesc = m_pkChar->GetDesc();
	if (pkDesc == nullptr)
		return;

	int iMonth, iDay;
	GetCurrentDate(&iMonth, &iDay);
	if (iDay > ATTENDANCE_DAYS)
		return;

	if (m_Table.bState[iDay] == ITEM_STATE_TAKE)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "<Attendance> You have already collected todays reward.");
		return;
	}

	const CAttendanceGroup* pkGroup = ITEM_MANAGER::instance().GetAttendanceGroup(iMonth);
	if (!pkGroup)
		return;

	int iEmptyPos = m_pkChar->GetEmptyInventory(1);
	if (iEmptyPos != -1)
		m_pkChar->AutoGiveItem(pkGroup->GetVNum(iDay), pkGroup->GetCount(iDay), -1, false);
	else
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You don't have enough space in your inventory.");
		return;
	}

	m_Table.bState[iDay] = ITEM_STATE_TAKE;

	db_clientdesc->DBPacket(HEADER_GD_ATTENDANCE_SAVE, pkDesc->GetHandle(), &m_Table, sizeof(m_Table));
	Load(&m_Table);
}

void CAttendance::Load(const TAttendanceTable* pTable)
{
	if (m_pkChar == nullptr)
	{
		sys_err("CAttendance::Load> <Character> (nullptr)");
		return;
	}

	const LPDESC pkDesc = m_pkChar->GetDesc();
	if (pkDesc == nullptr)
		return;

	if (pTable == nullptr)
	{
		sys_err("CAttendance::Load> <Character> (%s %p) <Table> (nullptr)", m_pkChar->GetName(), get_pointer(m_pkChar));
		return;
	}

	if (pTable->dwPID != m_pkChar->GetPlayerID())
		return;

	int iMonth, iDay;
	GetCurrentDate(&iMonth, &iDay);

	const CAttendanceGroup* pkGroup = ITEM_MANAGER::instance().GetAttendanceGroup(iMonth);
	if (!pkGroup)
		return;

	std::memcpy(&m_Table, pTable, sizeof(m_Table));

	TEMP_BUFFER TempBuffer;
	for (BYTE bAttendanceDay = 0; bAttendanceDay < pkGroup->GetSize(); ++bAttendanceDay)
	{
		TPacketGCAttendanceItem AttendanceItem;
		AttendanceItem.dwVNum = pkGroup->GetVNum(bAttendanceDay);
		AttendanceItem.bCount = pkGroup->GetCount(bAttendanceDay);
		if (bAttendanceDay == iDay && m_Table.bState[bAttendanceDay] != ITEM_STATE_TAKE)
		{
			AttendanceItem.bState = ITEM_STATE_OPEN;
			m_Table.bState[bAttendanceDay] = ITEM_STATE_OPEN;
		}
		else if (bAttendanceDay < iDay && m_Table.bState[bAttendanceDay] != ITEM_STATE_TAKE)
		{
			AttendanceItem.bState = ITEM_STATE_SKIP;
			m_Table.bState[bAttendanceDay] = ITEM_STATE_SKIP;
		}
		else
		{
			AttendanceItem.bState = m_Table.bState[bAttendanceDay];
			m_Table.bState[bAttendanceDay] = m_Table.bState[bAttendanceDay];
		}
		TempBuffer.write(&AttendanceItem, sizeof(AttendanceItem));
	}

	TPacketGCAttendance AttendancePacket;
	AttendancePacket.bHeader = HEADER_GC_ATTENDANCE;
	AttendancePacket.wSize = sizeof(AttendancePacket) + TempBuffer.size();
	if (TempBuffer.size())
	{
		pkDesc->BufferedPacket(&AttendancePacket, sizeof(AttendancePacket));
		pkDesc->Packet(TempBuffer.read_peek(), TempBuffer.size());
	}
	else
		pkDesc->Packet(&AttendancePacket, sizeof(AttendancePacket));

	db_clientdesc->DBPacket(HEADER_GD_ATTENDANCE_SAVE, pkDesc->GetHandle(), &m_Table, sizeof(m_Table));
}

void CAttendance::Process()
{
	quest::CQuestManager& rkQmgr = quest::CQuestManager::instance();
	if (!rkQmgr.GetEventFlag("attendance"))
		return;

	std::time_t lCurrentTime = std::time(nullptr);
	if (lCurrentTime > rkQmgr.GetEventFlag("attendance_end"))
	{
		int iMonth = rkQmgr.GetEventFlag("attendance_month");
		bool bValidGroup = false;
		while (!bValidGroup)
		{
			++iMonth;

			if (iMonth > 12)
				iMonth = 1;

			const CAttendanceGroup* pkGroup = ITEM_MANAGER::instance().GetAttendanceGroup(iMonth);
			if (pkGroup)
			{
				bValidGroup = true;
				break;
			}
		}

		long lDuration = (test_server ? 60 * ATTENDANCE_DAYS : static_cast<long>(ATTENDANCE_DURATION));

		rkQmgr.SetEventFlag("attendance_begin", lCurrentTime);
		rkQmgr.SetEventFlag("attendance_end", lCurrentTime + lDuration);
		rkQmgr.SetEventFlag("attendance_month", iMonth);

		// GD Save
		rkQmgr.RequestSetEventFlag("attendance_begin", rkQmgr.GetEventFlag("attendance_begin"));
		rkQmgr.RequestSetEventFlag("attendance_end", rkQmgr.GetEventFlag("attendance_end"));
		rkQmgr.RequestSetEventFlag("attendance_month", rkQmgr.GetEventFlag("attendance_month"));

		db_clientdesc->DBPacket(HEADER_GD_ATTENDANCE_DUMP, 0, nullptr, 0);
	}
}

void CAttendance::GetCurrentDate(int* pMonth, int* pDay)
{
	quest::CQuestManager& rkQmgr = quest::CQuestManager::instance();

	int iBeginTime = rkQmgr.GetEventFlag("attendance_begin");
	int iEndTime = rkQmgr.GetEventFlag("attendance_end");
	int iMonth = rkQmgr.GetEventFlag("attendance_month");

	int iCurTime = std::time(nullptr);
	int iTimeUnit = test_server ? 60 : 86400;

	int iElapsedSeconds = iCurTime - iBeginTime;
	int iElapsedDays = iElapsedSeconds / iTimeUnit;

	*pMonth = iMonth;
	*pDay = iElapsedDays;
}
#endif // __ATTENDANCE_SYSTEM__
