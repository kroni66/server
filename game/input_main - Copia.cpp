#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "DragonSoul.h"
#ifdef __NEWPET_SYSTEM__
#include "New_PetSystem.h"
#endif
#include "belt_inventory_helper.h" // @fixme119
#include "../../common/CommonDefines.h"

#include "input.h"
#ifdef ENABLE_SWITCHBOT
#include "new_switchbot.h"
#endif

#ifdef ENABLE_BATTLE_PASS
#include "battle_pass.h"
#endif

#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	#include "refine.h"
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	#include "whisper_admin.h"
#endif
#ifdef __INGAME_WIKI__
#include "mob_manager.h"
#endif

#define ENABLE_CHECK_GHOSTMODE

#ifdef ENABLE_CHAT_LOGGING
static char	__escape_string[1024];
static char	__escape_string2[1024];
#endif
static int __deposit_limit()
{
	return (1000*10000); // 1천만
}
#ifdef __SEND_TARGET_INFO__
void CInputMain::TargetInfoLoad(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGTargetInfoLoad* p = (TPacketCGTargetInfoLoad*)c_pData;
	TPacketGCTargetInfo pInfo;
	pInfo.header = HEADER_GC_TARGET_INFO;
	static std::vector<LPITEM> s_vec_item;
	s_vec_item.clear();
	LPITEM pkInfoItem;
	LPCHARACTER m_pkChrTarget = CHARACTER_MANAGER::instance().Find(p->dwVID);

	if (!ch || !m_pkChrTarget)
		return;

	// if (m_pkChrTarget && (m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()))
	// {
		// if (thecore_heart->pulse - (int) ch->GetLastTargetInfoPulse() < passes_per_sec * 3)
			// return;

		// ch->SetLastTargetInfoPulse(thecore_heart->pulse);

	if (ITEM_MANAGER::instance().CreateDropItemVector(m_pkChrTarget, ch, s_vec_item) && (m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()))
	{
		if (s_vec_item.size() == 0);
		else if (s_vec_item.size() == 1)
		{
			pkInfoItem = s_vec_item[0];
			pInfo.dwVID	= m_pkChrTarget->GetVID();
			pInfo.race = m_pkChrTarget->GetRaceNum();
			pInfo.dwVnum = pkInfoItem->GetVnum();
			pInfo.count = pkInfoItem->GetCount();
			ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
		}
		else
		{
			int iItemIdx = s_vec_item.size() - 1;
			while (iItemIdx >= 0)
			{
				pkInfoItem = s_vec_item[iItemIdx--];

				if (!pkInfoItem)
				{
					sys_err("pkInfoItem null in vector idx %d", iItemIdx + 1);
					continue;
				}

					pInfo.dwVID	= m_pkChrTarget->GetVID();
					pInfo.race = m_pkChrTarget->GetRaceNum();
					pInfo.dwVnum = pkInfoItem->GetVnum();
					pInfo.count = pkInfoItem->GetCount();
					ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
			}
		}
	}
	// }
}
#endif
void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 473, "");
#endif
		return;
	}

#ifdef TEXTS_IMPROVEMENT
	long hour = sec / 3600;
	sec -= hour * 3600;
	long min = (sec / 60);
	sec -= min * 60;
	if (hour > 0 && min > 0) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 475, "%d#%d#%d", hour, min, sec);
	}
	else if (hour > 0 && min == 0) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 476, "%d#%d", hour, sec);
	}
	else if (hour == 0 && min > 0) {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 477, "%d#%d", min, sec);
	}
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 478, "%d", sec);
	}
#endif
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH+1 );
	}
};

typedef boost::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.insert(std::make_pair(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL)));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|') // ||는 |로 표시한다.
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H') // hyperlink |Hitem:10000:0:0:0:0|h[이름]|h
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	//개인상점중에 금강경을 사용할 경우
	//0 : 정상적으로 사용
	//1 : 금강경 부족
	//2 : 금강경이 있으나, 개인상점에서 사용중
	//3 : 기타
	//4 : 에러
	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

	if (colored == true && hyperlinks == 0)
		return 4;

#ifdef ENABLE_NEWSTUFF
	if (g_bDisablePrismNeed)
		return 0;
#endif
	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 1;


	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	} else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		} else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 639, "");
#endif
		return (iExtraLen);
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
#if defined(BL_OFFLINE_MESSAGE)
			const BYTE bDelay = 10;
			char msg[64];
			if (get_dword_time() - ch->GetLastOfflinePMTime() > bDelay * 1000)
			{
				char buf[CHAT_MAX_LEN + 1];
				strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
				const size_t buflen = strlen(buf);
				CBanwordManager::instance().ConvertString(buf, buflen);
				int processReturn = ProcessTextTag(ch, buf, buflen);
				
				if (0 != processReturn)
				{
					TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);
					if (pTable) {
#ifdef ENABLE_MULTI_NAMES
						int Lang = ch && ch->GetDesc() ? ch->GetDesc()->GetLanguage() : 0;
#endif
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 823, "%s",
#ifdef ENABLE_MULTI_NAMES
						pTable->szLocaleName[Lang]
#else
						pTable->szLocaleName
#endif
						);
#endif
					}
					
					return (iExtraLen);
				}
				
				if (buflen > 0) 
				{
					ch->SendOfflineMessage(pinfo->szNameTo, buf);
					snprintf(msg, sizeof(msg), "An offline message has been sent.");
				}
				else
					return (iExtraLen);
			}
			else
			{
				snprintf(msg, sizeof(msg), "You have to wait %d seconds for send offline message.", bDelay);
			}
			
			TPacketGCWhisper pack;
			int len = MIN(CHAT_MAX_LEN, strlen(msg) + 1);
			pack.bHeader = HEADER_GC_WHISPER;
			pack.wSize = static_cast<WORD>(sizeof(TPacketGCWhisper) + len);
			pack.bType = WHISPER_TYPE_OFFLINE;
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			
			TEMP_BUFFER buf;
			buf.write(&pack, sizeof(TPacketGCWhisper));
			buf.write(msg, len);
			ch->GetDesc()->Packet(buf.read_peek(), buf.size());

#else
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
#endif
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire // 서로 제국이 다르면서
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER) // 둘다 일반 플레이어이면
							// 이름 밖에 모르니 gm_get_level 함수를 사용
						{
							if (!pkChr)
							{
								// 다른 서버에 있으니 제국 표시만 한다. bType의 상위 4비트를 Empire번호로 사용한다.
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1)/*변환확률*/);
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0!=processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
#ifdef ENABLE_MULTI_NAMES
						int Lang = ch && ch->GetDesc() ? ch->GetDesc()->GetLanguage() : 0;
#endif
						ch->ChatPacketNew(CHAT_TYPE_INFO, 823, "%s",
#ifdef ENABLE_MULTI_NAMES
						pTable->szLocaleName[Lang]
#else
						pTable->szLocaleName
#endif
						);
					}
				}

				// 릴래이 상태일 수 있으므로 릴래이를 풀어준다.
				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));
				// desc->BufferedPacket을 하지 않고 버퍼에 써야하는 이유는
				// P2P relay되어 패킷이 캡슐화 될 수 있기 때문이다.
				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
#ifdef ENABLE_CHAT_LOGGING
				if (ch->IsGM())
				{
					LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), buf, buflen);
					LogManager::instance().EscapeString(__escape_string2, sizeof(__escape_string2), pinfo->szNameTo, sizeof(pack.szNameFrom));
					LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, __escape_string2, "WHISPER", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
				}
#endif
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
			return;

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
			return;

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			// 사람마다 스킬레벨이 다르니 매번 해야합니다
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;

	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN+1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN+1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p),
		m_szChat(chat), m_lenChat(len_chat),
		m_szName(name),
		m_iMapIndex(iMapIndex), m_bEmpire(empire),
		m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1; // 널 문자 포함

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
			m_len_orig_msg = sizeof(m_orig_msg) - 1;

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1; // 널 문자 미포함

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
			m_len_conv_msg = sizeof(m_conv_msg) - 1;

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10); // 6은 "??? : "의 길이
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
			return;

		if (m_ring ||
			d->GetEmpire() == m_bEmpire ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

#ifdef __NEWPET_SYSTEM__
void CInputMain::BraveRequestPetName(LPCHARACTER ch, const char* c_pData)
{
	if (!ch->GetDesc()) { return; }
	int vid = ch->GetEggVid();
	if (vid == 0) { return; }

	TPacketCGRequestPetName* p = (TPacketCGRequestPetName*)c_pData;

	if (ch->GetGold() < 100000) 
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 768, "%d", 100000);
#endif
	}

	if (ch->CountSpecifyItem(vid) > 0 && check_name(p->petname) != 0) {
#ifdef ENABLE_NEW_PET_EDITS
		{
			char szQuery[256] = {0};
			snprintf(szQuery, sizeof(szQuery), "SELECT id FROM player.new_petsystem%s WHERE name='%s';", get_table_postfix(), p->petname);
			std::unique_ptr<SQLMsg> pRes(DBManager::instance().DirectQuery(szQuery));
			int iRes = pRes->Get()->uiNumRows;
			if (iRes > 0) {
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 50, "");
#endif
				return;
			}
		}
#endif
		
		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), -100000);
		ch->PointChange(POINT_GOLD, -100000, true);
		ch->RemoveSpecifyItem(vid, 1);
		LPITEM item = ch->AutoGiveItem(vid + 300, 1);
#ifdef ENABLE_NEW_PET_EDITS
		int tmpskill[4] = { -1, -1, -1, -1 };
#else
		int tmpskill[4] = { 0, 0, 0, 0 };
		int tmpslot = number(1, 3);
		for (int i = 0; i < 4; ++i)
		{
			if (i > tmpslot - 1)
				tmpskill[i] = -1;
		}
#endif
		int tmpdur = 3 * 24 * 60;
		char szQuery1[1024];
		int hp[] = {30, 35, 40};
		int mostri[] = {10, 15, 20};
		int medi[] = {10, 15, 20};
		snprintf(szQuery1, sizeof(szQuery1), "INSERT INTO new_petsystem VALUES(%u,'%s', 1, 0, 0, 0, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, 0"
#ifdef ENABLE_NEW_PET_EDITS
		", %d"
#endif
		")", item->GetID(), p->petname, hp[number(0, 2)], mostri[number(0, 2)], medi[number(0, 2)], tmpskill[0], 0, tmpskill[1], 0, tmpskill[2], 0, tmpskill[3], 0, tmpdur, tmpdur, get_global_time());
		std::unique_ptr<SQLMsg> pmsg2(DBManager::instance().DirectQuery(szQuery1));
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 769, "");
#endif
	}
#ifdef TEXTS_IMPROVEMENT
	else  {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 770, "");
	}
#endif
}
#endif

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}

	if (ch->GetGMLevel() == GM_PLAYER && ch->GetMapIndex() == 113)
	{
		return iExtraLen;
	}

	// 채팅 금지 Affect 처리
	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0!=processReturn)
	{
#ifdef TEXTS_IMPROVEMENT
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);
		if (NULL != pTable)
		{
#ifdef ENABLE_MULTI_NAMES
			int lang = 0;
			if (ch) {
				LPDESC desc = ch->GetDesc();
				lang = desc != NULL ? desc->GetLanguage() : 0;
			}
#endif
			ch->ChatPacketNew(CHAT_TYPE_INFO, 642, "%s", 
#ifdef ENABLE_MULTI_NAMES
			pTable->szLocaleName[lang]
#else
			pTable->szLocaleName
#endif
			);
		}
#endif
		return iExtraLen;
	}
	// @fixme133 end

	char chatbuf[CHAT_MAX_LEN + 1];
	//static const char* colorbuf[] = {"|cFFffa200|H|h[Staff]|h|r", "|cFFff0000|H|h[Shinsoo]|h|r", "|cFFffc700|H|h[Chunjo]|h|r", "|cFF000bff|H|h[Jinno]|h|r"};
#ifdef ENABLE_MULTI_LANGUAGE
	int len;
	std::string langName = ch->GetLang();
	if (pinfo->type == CHAT_TYPE_SHOUT) {
//		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l%s |Hmsg:%s|h%s [PM]|h|r : %s", langName.c_str(), (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), ch->GetName(), buf);
		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l%s |L%d|l |Hmsg:%s|h%s [PM]|h|r : %s", langName.c_str(), ch->GetEmpire(), ch->GetName(), ch->GetName(), buf);
	} else {
//		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l %s %s : %s", langName.c_str(), (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(), buf);
		len = snprintf(chatbuf, sizeof(chatbuf), "|L%s|l |L%d|l %s %s : %s", langName.c_str(), ch->GetEmpire(), ch->GetName(), buf);
	}
#else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s %s : %s", (ch->IsGM()?colorbuf[0]:colorbuf[MINMAX(0, ch->GetEmpire(), 3)]), ch->GetName(),buf);
#endif

	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		// const int SHOUT_LIMIT_LEVEL = 15;

		if (ch->GetLevel() < g_iShoutLimitLevel) {
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 411, "%d", g_iShoutLimitLevel);
#endif
			return (iExtraLen);
		}

		// if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * g_iShoutLimitTime)
		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
		strlcpy(p.szText, chatbuf, sizeof(p.szText));

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));

		SendShout(chatbuf, ch->GetEmpire());

#ifdef ENABLE_BATTLE_PASS
		BYTE bBattlePassId = ch->GetBattlePassId();
		if(bBattlePassId)
		{	
			DWORD dwCount, dwNotUsed;
			if(CBattlePass::instance().BattlePassMissionGetInfo(bBattlePassId, COUNTER_CHAT, &dwNotUsed, &dwCount))
			{
				if (!ch->IsCompletedMission(COUNTER_CHAT))
				{								
					if(ch->GetMissionProgress(COUNTER_CHAT, bBattlePassId) < dwCount)
						ch->UpdateMissionProgress(COUNTER_CHAT, bBattlePassId, 1, dwCount);
				}
			}
		}
		
#endif

		return (iExtraLen);
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();

	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				if (false)
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FYmirChatPacket(pack_chat,
								buf,
								strlen(buf),
								ch->GetName(),
								strlen(ch->GetName()),
								ch->GetMapIndex(),
								ch->GetEmpire(),
								ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
				}
				else
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(),
							FEmpireChatPacket(pack_chat,
								chatbuf,
								len,
								(ch->GetGMLevel() > GM_PLAYER ||
								 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
								ch->GetMapIndex(), strlen(ch->GetName())));
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, "", "NORMAL", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				if (!ch->GetParty())
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 485, "");
#endif
				if (ch->GetParty())
				{
					TEMP_BUFFER tbuf;

					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
					ch->GetParty()->ForEachOnlineMember(f);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetParty()->GetLeaderPID(), "", "PARTY", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
#ifdef TEXTS_IMPROVEMENT
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 486, "");
				}
#endif
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (ch->GetGuild()) {
					ch->GetGuild()->Chat(chatbuf);
#ifdef ENABLE_CHAT_LOGGING
					if (ch->IsGM())
					{
						LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
						LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetGuild()->GetID(), ch->GetGuild()->GetName(), "GUILD", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
					}
#endif
				}
#ifdef TEXTS_IMPROVEMENT
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 271, "");
				}
#endif
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;
	if (!ch)
		return;

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		return;
	}
#endif

	// 엘크가 0보다 크면 엘크를 버리는 것 이다.
	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;
	if (!ch)
		return;

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		return;
	}
#endif

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell, pinfo->count);
}

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
}

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
void CInputMain::InventoryExpansion(LPCHARACTER ch, const char * data)
{
	if (ch) 
		ch->Update_Inven();
}
#endif

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch) {
#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
		if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
			return;
		}
#endif
	}

	ch->PickupItem(pinfo->vid);
}

void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
#ifdef ENABLE_BUG_FIXES
	if (pinfo->slot.type == QUICKSLOT_TYPE_ITEM
#ifdef ENABLE_EXTRA_INVENTORY
 || pinfo->slot.type == 12
#endif
	) {
		LPITEM item = NULL;

#ifdef ENABLE_EXTRA_INVENTORY
		BYTE type;
		if (pinfo->slot.type == 12) {
			pinfo->slot.type = QUICKSLOT_TYPE_ITEM_EXTRA;
			type = EXTRA_INVENTORY;
		} else {
			type = INVENTORY;
		}
#endif

		TItemPos srcCell(
#ifdef ENABLE_EXTRA_INVENTORY
		type
#else
		INVENTORY
#endif
		, pinfo->slot.pos);

		if (!(item = ch->GetItem(srcCell)))
			return;

		if (item->GetType() != ITEM_USE && item->GetType() != ITEM_QUEST)
			return;

#ifdef ENABLE_EXTRA_INVENTORY
		if (type == QUICKSLOT_TYPE_ITEM_EXTRA && item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION) {
			return;
		}
#endif
	}
#endif

	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
					return -1;

				TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 370, "%s", ch_companion->GetName());
#endif
					return sizeof(TPacketCGMessengerAddByVID);
				}

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 184, "");
#endif
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (ch->GetDesc() == d) // 자신은 추가할 수 없다.
					return sizeof(TPacketCGMessengerAddByVID);

				MessengerManager::instance().RequestToAdd(ch, ch_companion);
				//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 184, "");
#endif
					return CHARACTER_NAME_MAX_LEN;
				}

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);
				if (tch)
				{
					if (tch == ch) // 자신은 추가할 수 없다.
						return CHARACTER_NAME_MAX_LEN;

					if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 370, "%s", tch->GetName());
#endif
					}
					else
					{
						// 메신저가 캐릭터단위가 되면서 변경
						MessengerManager::instance().RequestToAdd(ch, tch);
						//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
					}
				}
#ifdef TEXTS_IMPROVEMENT
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 108, "%s", name);
				}
#endif
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
#ifdef ENABLE_BUG_FIXES
				MessengerManager::instance().RemoveFromList(char_name, ch->GetName());
#endif
			}
			return CHARACTER_NAME_MAX_LEN;

		default:
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

#ifdef ENABLE_BATTLE_PASS
int CInputMain::BattlePass(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGBattlePassAction * p = (TPacketCGBattlePassAction *) data;

	if (uiBytes < sizeof(TPacketCGBattlePassAction))
		return -1;

	//const char * c_pData = data + sizeof(TPacketCGBattlePassAction);
	uiBytes -= sizeof(TPacketCGBattlePassAction);

	switch(p->bAction)
	{
		case 1:
			CBattlePass::instance().BattlePassRequestOpen(ch);
			break;
			
		case 2:
			CBattlePass::instance().BattlePassRequestReward(ch);
			break;
			
		case 3:
		{
			DWORD dwPlayerId = ch->GetPlayerID();
			BYTE bIsGlobal = 0;
			
			db_clientdesc->DBPacketHeader(HEADER_GD_BATTLE_PASS_RANKING, ch->GetDesc()->GetHandle(), sizeof(DWORD) + sizeof(BYTE));
			db_clientdesc->Packet(&dwPlayerId, sizeof(DWORD));
			db_clientdesc->Packet(&bIsGlobal, sizeof(BYTE));
		}
		break;
			
		default:
			break;
	}
	
	return 0;
}
#endif

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}
#ifndef ENABLE_EXTRA_INVENTORY
		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}
#endif
		case SHOP_SUBHEADER_CG_SELL2:
			{
				if (uiBytes < sizeof(BYTE)
#ifdef ENABLE_EXTRA_INVENTORY
				+ sizeof(WORD) 
#else
				+ sizeof(BYTE) 
#endif
#ifdef ENABLE_NEW_STACK_LIMIT
				+ sizeof(WORD) 
#else
				+ sizeof(BYTE) 
#endif
				)
					return -1;
				
#ifdef ENABLE_EXTRA_INVENTORY
				BYTE window = *(c_pData);
				WORD cell = *(WORD*)(c_pData + 1);
#else
				BYTE pos = *(c_pData++);
#endif
#ifdef ENABLE_NEW_STACK_LIMIT
				WORD count = *(WORD*)(c_pData + sizeof(WORD));
#else
				BYTE count = *(c_pData);
#endif

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());
				CShopManager::instance().Sell(ch,
#ifdef ENABLE_EXTRA_INVENTORY
				TItemPos(window, cell), 
#else
				pos, 
#endif
				count);
				return sizeof(BYTE) 
#ifdef ENABLE_EXTRA_INVENTORY
				+ sizeof(WORD) 
#else
				+ sizeof(BYTE) 
#endif
#ifdef ENABLE_NEW_STACK_LIMIT
				+ sizeof(WORD) 
#else
				+ sizeof(BYTE) 
#endif
				;
			}
#ifdef ENABLE_BUY_STACK_FROM_SHOP
		case SHOP_SUBHEADER_CG_BUY2:
			{
				size_t size = sizeof(uint8_t) + sizeof(uint8_t);
				if (uiBytes < size) {
					return -1;
				}

				uint8_t p = *(c_pData++);
				uint8_t c = *(c_pData);
				sys_log(1, "INPUT: %s SHOP: MULTIPLE BUY %d COUNT %d", ch->GetName(), p, c);
				CShopManager::instance().MultipleBuy(ch, p, c);
				return size;
			}
#endif
		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
#ifdef TEXTS_IMPROVEMENT
			to_ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
		return;
	}


	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
#ifdef TEXTS_IMPROVEMENT
						to_ch->ChatPacketNew(CHAT_TYPE_INFO, 234, "%d", g_nPortalLimitTime);
#endif
						return;
					}

					if (ch->GetGold() >= GOLD_MAX) {
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 406,
#ifdef ENABLE_LONG_LONG
						"%lld"
#else
						"%d"
#endif
						, GOLD_MAX);
#endif
						return;
					}

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}


					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
					|| ch->GetWheelDestiny()
#endif
					)
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
#endif
						return;
					}
					
#ifdef __ATTR_TRANSFER_SYSTEM__
					if (ch->IsAttrTransferOpen())
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
#endif
						return;
					}
#endif
#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
					if ((ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) || (to_ch->GetGMLevel() > GM_PLAYER && to_ch->GetGMLevel() < GM_IMPLEMENTOR)) {
						return;
					}
#endif
					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
			if (ch->GetExchange())
			{
#ifdef ENABLE_LONG_LONG
				const long long nTotalGold = static_cast<long long>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<long long>(pinfo->arg1);
#else
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);
#endif
				if (GOLD_MAX <= nTotalGold)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 226,
#ifdef ENABLE_LONG_LONG
					"%lld"
#else
					"%d"
#endif
					, nTotalGold);
#endif
					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddGold(pinfo->arg1);
			}
			break;
		case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
			if (ch->GetExchange())
				ch->GetExchange()->Cancel();
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch)
		return;

	if (!ch->CanMove())
		return;

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};

	// 텔레포트 핵 체크

//	if (!test_server)
	{
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
		if (((false == ch->IsRiding() && fDist > 30) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex())
		{
			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#endif

		DWORD dwCurTime = get_dword_time();
		if (ch->GetDesc()) {
			bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);
			if (CheckSpeedHack)
			{
				int iDelta = (int)(dwCurTime - pinfo->dwTime);
				int iServerDelta = (int)(dwCurTime - ch->GetDesc()->GetClientTime());
				if (iDelta >= 30000) {
					sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
					ch->GetDesc()->DelayedDisconnect(3);
				} else if (iDelta < -(iServerDelta / 50)) {
					sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
					ch->GetDesc()->DelayedDisconnect(3);
				}
			}

			//if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
			//{
			//	CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack); // 콤보 체크
			//}
		}
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);	// 중복 코드
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				ch->GetDesc()->DelayedDisconnect(number(150, 500));
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);	// 중복 코드
		ch->ResetStopTime();				// ""

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);
/*
	if (pinfo->dwTime == 10653691) // 디버거 발견
	{
		if (ch->GetDesc()->DelayedDisconnect(number(15, 30)))
			LogManager::instance().HackLog("Debugger", ch);

	}
	else if (pinfo->dwTime == 10653971) // Softice 발견
	{
		if (ch->GetDesc()->DelayedDisconnect(number(15, 30)))
			LogManager::instance().HackLog("Softice", ch);
	}
*/
	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

#ifdef __SKILL_COLOR_SYSTEM__
void CInputMain::SetSkillColor(LPCHARACTER ch, const char* pcData)
{
	if (!ch)
		return;
	
	TPacketCGSkillColor * p = (TPacketCGSkillColor*)pcData;
	if (p->skill >= ESkillColorLength::MAX_SKILL_COUNT)
		return;
	
	if ((p->col1 != 0) || (p->col2 != 0) || (p->col3 != 0) || (p->col4 != 0) || (p->col5 != 0)) {
		if (ch->CountSpecifyItem(164406) < 1) {
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 16, "");
#endif
			return;
		} else {
			ch->RemoveSpecifyItem(164406, 1);
		}
	}
	
	DWORD data[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memcpy(data, ch->GetSkillColor(), sizeof(data));

	data[p->skill][0] = p->col1;
	data[p->skill][1] = p->col2;
	data[p->skill][2] = p->col3;
	data[p->skill][3] = p->col4;
	data[p->skill][4] = p->col5;

#ifdef TEXTS_IMPROVEMENT
	ch->ChatPacketNew(CHAT_TYPE_INFO, 15, "");
#endif

	ch->SetSkillColor(data[0]);

	TSkillColor db_pack;
	memcpy(db_pack.dwSkillColor, data, sizeof(data));
	db_pack.player_id = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_SKILL_COLOR_SAVE, 0, sizeof(TSkillColor));
	db_clientdesc->Packet(&db_pack, sizeof(TSkillColor));
}
#endif

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK:
			{
				if (NULL == ch->GetDesc())
					return;

				const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

				ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

				LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

				if (NULL == victim || ch == victim)
					return;

				switch (victim->GetCharType())
				{
					case CHAR_TYPE_NPC:
					case CHAR_TYPE_WARP:
					case CHAR_TYPE_GOTO:
						return;
				}

				if (packMelee->bType > 0)
				{
					if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
					{
						return;
					}
				}

				ch->Attack(victim, packMelee->bType);
			}
			break;

		case HEADER_CG_SHOOT:
			{
				const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

				ch->Shoot(packShoot->bType);
			}
			break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 60;

	if( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
				continue;
		}

		// 소유권 검사
		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;
		// victim과의 거리가 2500 + a 이상이면 핵으로 간주.
		//	거리 참조 : 클라이언트의 __GetSkillTargetRange, __GetBowRange 함수
		//	2500 : 스킬 proto에서 가장 사거리가 긴 스킬의 사거리, 또는 활의 사거리
		//	a = POINT_BOW_DISTANCE 값... 인데 실제로 사용하는 값인지는 잘 모르겠음. 아이템이나 포션, 스킬, 퀘스트에는 없는데...
		//		그래도 혹시나 하는 마음에 버퍼로 사용할 겸해서 1000.f 로 둠...
		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			// g_iSyncHackLimitCount번 까지는 봐줌.
			if (ch->GetSyncHackCount() < 60)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		// SyncPosition을 악용하여 타유저를 이상한 곳으로 보내는 핵 방어하기 위하여,
		// 같은 유저를 g_lValidSyncInterval ms 이내에 다시 SyncPosition하려고 하면 핵으로 간주.
		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			// g_iSyncHackLimitCount번 까지는 봐줌.
			if (ch->GetSyncHackCount() < 60)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
					tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}
		else if( fDist > 40.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
				   	fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
				  e->lX, e->lY );

			ch->GetDesc()->SetPhase(PHASE_CLOSE);

			return -1;
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		//퀘스트 창에서 클릭시(__SelectQuest) 여기로
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250) // 다음 버튼에 대한 응답으로 온 패킷인 경우
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else // 선택 버튼을 골라서 온 패킷인 경우
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}


// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		return;
	}
#endif

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;

#ifdef ENABLE_BUG_FIXES
	if (pkItem->IsEquipped())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1244, "");
#endif
		return;
	}
#endif

#if defined(ENABLE_EXTRA_INVENTORY) && !defined(ENABLE_SPECIAL_INV_TO_SAFEBOX)
	if (pkItem->IsExtraItem())
		return;
#endif

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	if (pkItem->GetCell() >= ch->Inventory_Size() && IS_SET(pkItem->GetFlag(),ITEM_FLAG_IRREMOVABLE))
#else
	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
#endif
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 640, "");
#endif
		return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 641, "");
#endif
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 187, "");
#endif
		return;
	}

	if( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 187, "");
#endif
		return;
	}

	if (true == pkItem->isLocked())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 187, "");
#endif
		return;
	}

	// @fixme140 BEGIN
	if (ITEM_BELT == pkItem->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(ch))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 385, "");
#endif
		return;
	}
	// @fixme140 END

	pkItem->RemoveFromCharacter();
#ifdef ENABLE_EXTRA_INVENTORY
	if (!pkItem->IsDragonSoul() && !pkItem->IsExtraItem())
#else
	if (!pkItem->IsDragonSoul())
#endif
	{
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 255);
	}

	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		return;
	}
#endif

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

	// 아이템 몰에서 인벤으로 옮기는 부분에서 용혼석 특수 처리
	// (몰에서 만드는 아이템은 item_proto에 정의된대로 속성이 붙기 때문에,
	//  용혼석의 경우, 이 처리를 하지 않으면 속성이 하나도 붙지 않게 된다.)
	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 643, "");
#endif
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 644, "");
#endif
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#ifdef ENABLE_EXTRA_INVENTORY
	else if(pkItem->IsExtraItem())
	{
		if (p->ItemPos.window_type != EXTRA_INVENTORY)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 1292, "");
#endif
			return;
		}

		BYTE category = pkItem->GetExtraCategory();
		if (p->ItemPos.cell < category * EXTRA_INVENTORY_CATEGORY_MAX_NUM || p->ItemPos.cell >= (category + 1) * EXTRA_INVENTORY_CATEGORY_MAX_NUM) {
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
#endif
	else
	{
#ifdef ENABLE_EXTRA_INVENTORY
		if (EXTRA_INVENTORY == p->ItemPos.window_type)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 645, "");
#endif
			return;
		}
#endif
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 646, "");
#endif
			return;
		}
		// @fixme119
		if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 509, "");
#endif
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		return;
	}
#endif

	if (!ch->GetSafebox())
		return;

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}
#ifdef ENABLE_MAP_TELEPORTER
void CInputMain::MapTeleporter(LPCHARACTER ch, TPacketCGMapTeleporter* pPack)
{
	if (ch->IsHack() || ch->GetExchange() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->GetShop() || ch->GetMyShop()
#ifdef ENABLE_ACCE_SYSTEM
		|| ch->IsAcceOpen()
#endif
		)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 647, "");
#endif
		return;
	}
	
#ifdef __ATTR_TRANSFER_SYSTEM__
	if (ch->IsAttrTransferOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 647, "");
#endif
		return;
	}
#endif
	
	//Check DungeonMap Genezis 
	//Check if current map is a dungeon!
//	if (ch->GetDungeon())
//	{
//#ifdef TEXTS_IMPROVEMENT
//		ch->ChatPacketNew(CHAT_TYPE_INFO, 48, "");
//#endif
//		return;
//	}

	unsigned int iMapCode = pPack->iMapCode;
	if(iMapCode <0 || iMapCode >= g_vecMapConf.size())
		return;

	TMapConfig& rConf = g_vecMapConf[iMapCode];
	
	if(ch->GetLevel() < rConf.iLevel)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 771, "%d", rConf.iLevel);
#endif
		return;
	}
	
	if(rConf.iLevelMax != 0 && ch->GetLevel() > rConf.iLevelMax)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 772, "%d", rConf.iLevelMax);
#endif
		return;
	}
		
	if(ch->GetGold() < rConf.price)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 773, "%d", rConf.price);
#endif
		return;
	}

	ch->PointChange(POINT_GOLD, -rConf.price);

	for (auto itemVnum : rConf.items)
		if (ch->CountSpecifyItem(itemVnum) == 0)
			return;

	for(auto itemVnum : rConf.items)
		ch->RemoveSpecifyItem(itemVnum);

	// int iMapIndex = 0;

	// iMapIndex = rConf.iMapIndex;

	// PIXEL_POSITION pos;
	// SECTREE_MANAGER::instance().GetRecallPositionByEmpire(iMapIndex, ch->GetEmpire(), pos);

	// ch->WarpSet(pos.x, pos.y);


	long coord_x = 0;
	long coord_y = 0;
	
	coord_x = rConf.coord_x;
	coord_y = rConf.coord_y;
	
	ch->WarpSet(coord_x, coord_y);

}
#endif

// PARTY_JOIN_BUG_FIX
void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);
	if (!pInviter || !pInviter->GetDesc()) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 217, "");
#endif
	}
	else if (!p->accept) {
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	} else {
		pInviter->PartyInviteAccept(ch);
	}
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 206, "");
#endif
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 207, "");
#endif
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}
			break;
		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 303, "");
#endif
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 208, "");
#endif
		return;
	}

	if (ch->GetDungeon())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 203, "");
#endif
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (!ch->GetDungeon()) {
			// 적룡성에서 파티장이 던젼 밖에서 파티 해산 못하게 막자
			if(pParty->IsPartyInDungeon(351))
			{
#ifdef TEXTS_IMPROVEMENT
				ch->ChatPacketNew(CHAT_TYPE_INFO, 648, "");
#endif
				return;
			}

			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
#ifdef TEXTS_IMPROVEMENT
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B) {
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacketNew(CHAT_TYPE_INFO, 216, "");
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
#endif
				pParty->Quit(p->pid);
			}
		}
#ifdef TEXTS_IMPROVEMENT
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 205, "");
		}
#endif
	}
	else
	{
		if (p->pid == ch->GetPlayerID())
		{
			if (!ch->GetDungeon()) {
				if (pParty->GetMemberCount() == 2) {
					CPartyManager::instance().DeleteParty(pParty);
				} else {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 215, "");
#endif
					//pParty->SendPartyRemoveOneToAll(ch);
					pParty->Quit(ch->GetPlayerID());
					//pParty->SendPartyRemoveAllToOne(ch);
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
#ifdef TEXTS_IMPROVEMENT
			else {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 204, "");
			}
#endif
		}
#ifdef TEXTS_IMPROVEMENT
		else {
			ch->ChatPacketNew(CHAT_TYPE_INFO, 197, "");
		}
#endif
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetGold() < 200000) {
		return;
	}
#ifdef ENABLE_BUG_FIXES
	else if (ch->GetLevel() < 40) {
		return;
	}
#endif

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") < CGuildManager::instance().GetDisbandDelay()) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 181, "%d", quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
#endif
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") < CGuildManager::instance().GetWithdrawDelay()) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 179, "%d", quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
#endif
		return;
	}

	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 455, "");
#endif
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 125, "%s", cp.name);
#endif
		int GuildCreateFee = 200000;

		ch->PointChange(POINT_GOLD, -GuildCreateFee);
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 132, "");
	}
#endif
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 211, "");
#endif
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
				if (pch) {
					ch->GetParty()->SummonToLeader(pch->GetPlayerID());
				}
#ifdef TEXTS_IMPROVEMENT
				else {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 209, "");
				}
#endif
			}
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

#ifdef __INGAME_WIKI__
void CInputMain::RecvWikiPacket(LPCHARACTER ch, const char * c_pData)
{
	if (!ch || (ch && !ch->GetDesc()))
		return;
	
	if (!c_pData)
		return;
	
	InGameWiki::TCGWikiPacket * p = nullptr;
	if (!(p = (InGameWiki::TCGWikiPacket *) c_pData))
		return;
	
	InGameWiki::TGCWikiPacket pack;
	pack.set_data_type(!p->is_mob ? InGameWiki::LOAD_WIKI_ITEM : InGameWiki::LOAD_WIKI_MOB);
	pack.increment_data_size(WORD(sizeof(InGameWiki::TGCWikiPacket)));
	
	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const std::vector<CommonWikiData::TWikiItemOriginInfo>& originVec = ITEM_MANAGER::Instance().GetItemOrigin(p->vnum);
		const std::vector<CSpecialItemGroup::CSpecialItemInfo> _gV = ITEM_MANAGER::instance().GetWikiChestInfo(p->vnum);
		const std::vector<CommonWikiData::TWikiRefineInfo> _rV = ITEM_MANAGER::instance().GetWikiRefineInfo(p->vnum);
		const CommonWikiData::TWikiInfoTable* _wif = ITEM_MANAGER::instance().GetItemWikiInfo(p->vnum);
		
		if (!_wif)
			return;
		
		const size_t origin_size = originVec.size();
		const size_t chest_info_count = _wif->chest_info_count;
		const size_t refine_infos_count = _wif->refine_infos_count;
		const size_t buf_data_dize = sizeof(InGameWiki::TGCItemWikiPacket) +
								(origin_size * sizeof(CommonWikiData::TWikiItemOriginInfo)) +
								(chest_info_count * sizeof(CommonWikiData::TWikiChestInfo)) +
								(refine_infos_count * sizeof(CommonWikiData::TWikiRefineInfo));
		
		if (chest_info_count != _gV.size()) {
			sys_err("Item Vnum : %d || ERROR TYPE -> 1", p->vnum);
			return;
		}
		
		if (refine_infos_count != _rV.size()) {
			sys_err("Item Vnum : %d || ERROR TYPE -> 2", p->vnum);
			return;
		}
		
		pack.increment_data_size(WORD(buf_data_dize));
		
		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));
		
		InGameWiki::TGCItemWikiPacket data_packet;
		data_packet.mutable_wiki_info(*_wif);
		data_packet.set_origin_infos_count(origin_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(data_packet));
		
		{
			if (origin_size)
				for (int idx = 0; idx < (int)origin_size; ++idx)
					buf.write(&(originVec[idx]), sizeof(CommonWikiData::TWikiItemOriginInfo));
			
			if (chest_info_count > 0) {
				for (int idx = 0; idx < (int)chest_info_count; ++idx) {
					CommonWikiData::TWikiChestInfo write_struct(_gV[idx].vnum, _gV[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiChestInfo));
				}
			}
			
			if (refine_infos_count > 0)
				for (int idx = 0; idx < (int)refine_infos_count; ++idx)
					buf.write(&(_rV[idx]), sizeof(CommonWikiData::TWikiRefineInfo));
		}
		
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		CMobManager::TMobWikiInfoVector& mobVec = CMobManager::instance().GetMobWikiInfo(p->vnum);
		const size_t _mobVec_size = mobVec.size();
		
		if (!_mobVec_size) {
			if (test_server)
				sys_log(0, "Mob Vnum: %d : || LOG TYPE -> 1", p->vnum);
			return;
		}
		
		const size_t buf_data_dize = (sizeof(InGameWiki::TGCMobWikiPacket) + (_mobVec_size * sizeof(CommonWikiData::TWikiMobDropInfo)));
		pack.increment_data_size(WORD(buf_data_dize));
		
		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));
		
		InGameWiki::TGCMobWikiPacket data_packet;
		data_packet.set_drop_info_count(_mobVec_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(InGameWiki::TGCMobWikiPacket));
		
		{
			if (_mobVec_size) {
				for (int idx = 0; idx < (int)_mobVec_size; ++idx) {
					CommonWikiData::TWikiMobDropInfo write_struct(mobVec[idx].vnum, mobVec[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiMobDropInfo));
				}
			}
		}
		
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}
#endif

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
#ifdef TEXTS_IMPROVEMENT
			ch->ChatPacketNew(CHAT_TYPE_INFO, 138, "");
#endif
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				// by mhh : 길드자금은 당분간 넣을 수 없다.
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

				if (gold < 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 170, "");
#endif
					return SubPacketLen;
				}

				if (ch->GetGold() < gold)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 126, "");
#endif
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				// by mhh : 길드자금은 당분간 뺄 수 없다.
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);

				if (gold < 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 170, "");
#endif
					return SubPacketLen;
				}

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
				LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

				if (!newmember)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 128, "");
#endif
					return SubPacketLen;
				}

				// @fixme145 BEGIN (+newmember ispc check)
				if (!ch->IsPC() || !newmember->IsPC())
					return SubPacketLen;
				// @fixme145 END

				pGuild->Invite(ch, newmember);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 649, "");
#endif
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 161, "");
#endif
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 139, "");
#endif
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (g_bGuildInviteLimit)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 139, "");
#endif
						return SubPacketLen;
					}
					
#ifdef TEXTS_IMPROVEMENT
					if (pGuild->RequestRemoveMember(pid)) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 129, "");
					} else {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 128, "");
					}
#endif
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 175, "");
#endif
				} else if (*c_pData == GUILD_LEADER_GRADE) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 143, "");
#endif
				}
				else if (!check_name(gradename)) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 171, "");
#endif
				}
				else {
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());
				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 174, "");
#endif
				} else if (*c_pData == GUILD_LEADER_GRADE) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 142, "");
#endif
				}
				else {
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;
		case GUILD_SUBHEADER_CG_OFFER:
			{
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 650, "%d", GUILD_MAX_LEVEL);
#endif
				}
				else
				{
					offer /= 100;
					offer *= 100;
#ifdef TEXTS_IMPROVEMENT
					if (pGuild->OfferExp(ch, offer)) {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 121, "%u", offer);
					} else {
						ch->ChatPacketNew(CHAT_TYPE_INFO, 122, "");
					}
#endif
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 151, "");
#endif
					return SubPacketLen;
				}

#ifdef TEXTS_IMPROVEMENT
				if (!pGuild->ChargeSP(ch, offer)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 164, "");
				}
#endif
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					// 잘못된 길이.. 끊어주자.
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 127, "");
#endif
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 176, "");
#endif
				} else if (ch->GetPlayerID() == pid) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 143, "");
#endif
				} else if (grade == 1) {
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 141, "");
#endif
				} else {
					pGuild->ChangeMemberGrade(pid, grade);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

#ifdef TEXTS_IMPROVEMENT
				if (m->grade != GUILD_LEADER_GRADE) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 150, "");
				} else if (!pGuild->ChangeMemberGeneral(pid, is_general)) {
					ch->ChatPacketNew(CHAT_TYPE_INFO, 149, "");
				}
#endif
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch) {
#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
		if ((to_ch->GetGMLevel() > GM_PLAYER && to_ch->GetGMLevel() < GM_IMPLEMENTOR) || (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR)) {
			return;
		}
#endif
		
		ch->GiveItem(to_ch, p->ItemPos);
	}
#ifdef TEXTS_IMPROVEMENT
	else {
		ch->ChatPacketNew(CHAT_TYPE_INFO, 403, "");
	}
#endif
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);

	// 현재 클라이언트에서 이 패킷을 보내는 경우가 없으므로 무조건 끊도록 한다
	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;

	if (ch->GetGold() >= GOLD_MAX)
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 226, 
#ifdef ENABLE_LONG_LONG
		"%lld"
#else
		"%d"
#endif
		, GOLD_MAX);
#endif
		return (iExtraLen);
	}

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
#endif
		return (iExtraLen);
	}
	
#ifdef __ATTR_TRANSFER_SYSTEM__
	if (ch->IsAttrTransferOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
#endif
		return (iExtraLen);
	}
#endif
	
	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount
#ifdef KASMIR_PAKET_SYSTEM
	, p->dwKasmirNpc, p->bKasmirBaslik
#endif
	);
	return (iExtraLen);
}

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);
#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
	if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
		ch->ClearRefineMode();
		return;
	}
#endif

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 502, "");
#endif
		ch->ClearRefineMode();
		return;
	}

#ifdef __ATTR_TRANSFER_SYSTEM__
	if (ch->IsAttrTransferOpen())
	{
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 292, "");
#endif
		ch->ClearRefineMode();
		return;
	}
#endif
	
	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	if (p->pos >= ch->Inventory_Size())
#else
	if (p->pos >= INVENTORY_MAX_NUM)
#endif
	{
		ch->ClearRefineMode();
		return;
	}

	LPITEM item = ch->GetInventoryItem(p->pos);


#ifdef ENABLE_FEATURES_REFINE_SYSTEM
	if (!CRefineManager::instance().GetPercentage(ch, p->lLow, p->lMedium, p->lExtra, p->lTotal, item))
	{
		ch->ClearRefineMode();
		return;
	}

	CRefineManager::instance().Increase(ch, p->lLow, p->lMedium, p->lExtra);
#endif

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_noraml");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}

#ifdef ENABLE_SOUL_SYSTEM
	else if (p->type == REFINE_TYPE_SOUL)
	{
		sys_log (0, "refine_type_soul, ...");
		ch->DoRefineItemSoul(item);
	}
#endif
	else if (p->type == REFINE_TYPE_MONEY_ONLY) {
		const LPITEM item = ch->GetInventoryItem(p->pos);
		if (item) {
			if (ch->GetQuestFlag("deviltower_zone.can_refine"))
			{
#ifdef ENABLE_BUG_FIXES
				if (ch->DoRefine(item, true)) {
					ch->SetQuestFlag("deviltower_zone.can_refine", 0);
				}
#else
				ch->DoRefine(item, true);
				ch->SetQuestFlag("deviltower_zone.can_refine", 0);
#endif
			}
#ifdef TEXTS_IMPROVEMENT
			else {
				ch->ChatPacketNew(CHAT_TYPE_INFO, 361, "");
			}
#endif
		}
	}

	ch->ClearRefineMode();
}



#ifdef ENABLE_ACCE_SYSTEM
void CInputMain::Acce(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;

	TPacketAcce * sPacket = (TPacketAcce*) c_pData;
	switch (sPacket->subheader)
	{
	case ACCE_SUBHEADER_CG_CLOSE:
	{
		pkChar->CloseAcce();
	}
	break;
	case ACCE_SUBHEADER_CG_ADD:
	{
		pkChar->AddAcceMaterial(sPacket->tPos, sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REMOVE:
	{
		pkChar->RemoveAcceMaterial(sPacket->bPos);
	}
	break;
	case ACCE_SUBHEADER_CG_REFINE:
	{
		pkChar->RefineAcceMaterials();
	}
	break;
	default:
		break;
	}
}
#endif

#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
void CInputMain::CubeRenewalSend(LPCHARACTER ch, const char* data)
{
	struct packet_send_cube_renewal * pinfo = (struct packet_send_cube_renewal *) data;
	switch (pinfo->subheader)
	{
		case CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM:
		{

			int index_item = pinfo->index_item;
			int count_item = pinfo->count_item;
			int index_item_improve = pinfo->index_item_improve;

			Cube_Make(ch,index_item,count_item,index_item_improve);
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_CLOSE:
		{
			Cube_close(ch);
		}
		break;
	}
}
#endif


#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "new_offlineshop.h"
#include "new_offlineshop_manager.h"
template <class T>
bool CanDecode(T* p, int buffleft) {
	return buffleft >= (int)sizeof(T);
}

template <class T>
const char* Decode(T*& pObj, const char* data, int* pbufferLeng = nullptr, int* piBufferLeft=nullptr)
{
	pObj = (T*) data;

	if(pbufferLeng)
		*pbufferLeng += sizeof(T);

	if(piBufferLeft)
		*piBufferLeft -= sizeof(T);

	return data + sizeof(T);
}

int OfflineshopPacketCreateNewShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopCreate* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	offlineshop::TShopInfo& rShopInfo = pack->shop;

	//fix flooding
	if (rShopInfo.dwCount > 500 || rShopInfo.dwCount == 0) {
		sys_err("tried to open a shop with 500+ items.");
		return -1;
	}

	std::vector<offlineshop::TShopItemInfo> vec;
	vec.reserve(rShopInfo.dwCount);

	offlineshop::TShopItemInfo* pItem=nullptr;


	for (DWORD i = 0; i < rShopInfo.dwCount; ++i)
	{
		if(!CanDecode(pItem, iBufferLeft))
			return -1;

		data = Decode(pItem, data, &iExtra, &iBufferLeft);
		vec.push_back(*pItem);
	}

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopCreateNewClientPacket(ch, rShopInfo, vec)) {
		if (ch) {
			offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CREATE_SHOP);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefreshOfflineShop");
		}
	}
	
	return iExtra;
}


int OfflineshopPacketChangeShopName(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopChangeName* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopChangeNameClientPacket(ch, pack->szName))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CHANGE_NAME);

	return iExtra;
}


int OfflineshopPacketForceCloseShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopForceCloseClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_FORCE_CLOSE);

	return 0;
}


int OfflineshopPacketRequestShopList(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopRequestListClientPacket(ch);
	return 0;
}


int OfflineshopPacketOpenShop(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopOpen* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopOpenClientPacket(ch,pack->dwOwnerID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_OPEN_SHOP);

	return iExtra;
}


int OfflineshopPacketOpenShowOwner(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopOpenMyShopClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_OPEN_SHOP_OWNER);

	return 0;
}


int OfflineshopPacketBuyItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopBuyItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopBuyItemClientPacket(ch , pack->dwOwnerID, pack->dwItemID, pack->bIsSearch, pack->TotalPriceSeen)) //patch seen price check
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_BUY_ITEM);

	return iExtra;
}


int OfflineshopPacketAddItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAddItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);
	
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopAddItemClientPacket(ch, pack->pos, pack->price))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_ADD_ITEM);

	return iExtra;
}


int OfflineshopPacketRemoveItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGRemoveItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopRemoveItemClientPacket(ch, pack->dwItemID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_REMOVE_ITEM);

	return iExtra;
}


int OfflineshopPacketEditItem(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGEditItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopEditItemClientPacket(ch, pack->dwItemID, pack->price))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_EDIT_ITEM);

	return iExtra;
}


int OfflineshopPacketFilterRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGFilterRequest* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopFilterRequestClientPacket(ch, pack->filter))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_FILTER);

	return iExtra;
}


int OfflineshopPacketCreateOffer(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGOfferCreate* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopCreateOfferClientPacket(ch, pack->offer))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_CREATE_OFFER);

	return iExtra;
}


int OfflineshopPacketAcceptOffer(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGOfferAccept* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopAcceptOfferClientPacket(ch, pack->dwOfferID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_ACCEPT_OFFER);

	return iExtra;
}



int OfflineshopPacketOfferCancel(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGOfferCancel* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopCancelOfferClientPacket(ch, pack->dwOfferID, pack->dwOwnerID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_ACCEPT_OFFER);

	return iExtra;
}


int OfflineshopPacketOfferListRequest(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvOfferListRequestPacket(ch);
	return 0;
}


int OfflineshopPacketOpenSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxOpenClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_OPEN_SAFEBOX);

	return 0;
}


int OfflineshopPacketCloseBoard(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvCloseBoardClientPacket(ch);
	return 0;
}

int OfflineshopPacketCloseMyAuction(LPCHARACTER ch)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvCloseMyAuction(ch);
	return 0;
}

int OfflineshopPacketGetItemSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopSafeboxGetItem* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxGetItemClientPacket(ch, pack->dwItemID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_SAFEBOX_GET_ITEM);

	return iExtra;

}


int OfflineshopPacketGetValutesSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopSafeboxGetValutes* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxGetValutesClientPacket(ch, pack->valutes))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_SAFEBOX_GET_VALUTES);

	return iExtra;
}


int OfflineshopPacketCloseSafebox(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvShopSafeboxCloseClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_CANNOT_SAFEBOX_CLOSE);

	return 0;
}


int OfflineshopPacketListRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionListRequestClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_SEND_LIST);

	return 0;
}



int OfflineshopPacketOpenAuctionRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionOpenRequest* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionOpenRequestClientPacket(ch, pack->dwOwnerID))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_OPEN_AUCTION);

	return iExtra;
}



int OfflineshopPacketOpenMyAuctionRequest(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvMyAuctionOpenRequestClientPacket(ch))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_SEND_LIST);

	return 0;
}



int OfflineshopPacketCreateAuction(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionCreate* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionCreateClientPacket(ch, pack->dwDuration, pack->init_price, pack->pos))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_CREATE_AUCTION);

	return iExtra;
}



int OfflineshopPacketAddOffer(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionAddOffer* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	if(!rManager.RecvAuctionAddOfferClientPacket(ch, pack->dwOwnerID, pack->price))
		offlineshop::SendChatPacket(ch, offlineshop::CHAT_PACKET_AUCTION_CANNOT_ADD_OFFER);

	return iExtra;
}



int OfflineshopPacketExitFromAuction(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGAuctionExitFrom* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack,data, &iExtra, &iBufferLeft);

	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvAuctionExitFromAuction(ch, pack->dwOwnerID);
	return iExtra;
}


#ifdef ENABLE_NEW_SHOP_IN_CITIES
int OfflineshopPacketClickEntity(LPCHARACTER ch, const char* data, int iBufferLeft)
{
	offlineshop::TSubPacketCGShopClickEntity* pack = nullptr;
	if(!CanDecode(pack, iBufferLeft))
		return -1;

	int iExtra=0;
	data = Decode(pack, data, &iExtra, &iBufferLeft);

	
	offlineshop::CShopManager& rManager = offlineshop::GetManager();
	rManager.RecvShopClickEntity(ch, pack->dwShopVID);
	return iExtra;
}

#endif



int OfflineshopPacket(const char* data , LPCHARACTER ch, long iBufferLeft)
{
	unsigned int iBufferLeftCompare = iBufferLeft;
	if(iBufferLeftCompare < sizeof(TPacketCGNewOfflineShop))
		return -1;

	TPacketCGNewOfflineShop* pPack=nullptr;
	iBufferLeft -= sizeof(TPacketCGNewOfflineShop);
	data = Decode(pPack, data);

	

	switch (pPack->bSubHeader)
	{
	
	case offlineshop::SUBHEADER_CG_SHOP_CREATE_NEW:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCreateNewShop(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_CHANGE_NAME:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketChangeShopName(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_FORCE_CLOSE:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketForceCloseShop(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REQUEST_SHOPLIST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketRequestShopList(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OPEN:					return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenShop(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OPEN_OWNER:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenShowOwner(ch,data,iBufferLeft);
	
	case offlineshop::SUBHEADER_CG_SHOP_BUY_ITEM:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketBuyItem(ch, data , iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_ADD_ITEM:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketAddItem(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REMOVE_ITEM:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketRemoveItem(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_EDIT_ITEM:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketEditItem(ch,data,iBufferLeft);
	
	case offlineshop::SUBHEADER_CG_SHOP_FILTER_REQUEST:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketFilterRequest(ch,data,iBufferLeft);
	
	case offlineshop::SUBHEADER_CG_SHOP_OFFER_CREATE:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCreateOffer(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_OFFER_ACCEPT:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketAcceptOffer(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_REQUEST_OFFER_LIST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOfferListRequest(ch);
	case offlineshop::SUBHEADER_CG_SHOP_OFFER_CANCEL:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOfferCancel(ch, data,iBufferLeft);

	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_OPEN:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenSafebox(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketGetItemSafebox(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES:	return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketGetValutesSafebox(ch,data,iBufferLeft);
	case offlineshop::SUBHEADER_CG_SHOP_SAFEBOX_CLOSE:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCloseSafebox(ch,data,iBufferLeft);

	case offlineshop::SUBHEADER_CG_AUCTION_LIST_REQUEST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketListRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_AUCTION_OPEN_REQUEST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenAuctionRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_MY_AUCTION_OPEN_REQUEST:		return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketOpenMyAuctionRequest(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_CREATE_AUCTION:				return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketCreateAuction(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_AUCTION_ADD_OFFER:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketAddOffer(ch, data, iBufferLeft);
	case offlineshop::SUBHEADER_CG_EXIT_FROM_AUCTION:			return /*sizeof(TPacketCGNewOfflineShop) +*/ OfflineshopPacketExitFromAuction(ch, data, iBufferLeft);

	case offlineshop::SUBHEADER_CG_CLOSE_BOARD:					return /*sizeof(TPacketCGNewOfflineshop) +*/ OfflineshopPacketCloseBoard(ch);
#ifdef ENABLE_NEW_SHOP_IN_CITIES
	case offlineshop::SUBHEADER_CG_CLICK_ENTITY:				return /*sizeof(TPacketCGNewOfflineshop) +*/ OfflineshopPacketClickEntity(ch, data, iBufferLeft);
#endif
	case offlineshop::SUBHEADER_CG_AUCTION_CLOSE:
		return /*sizeof(TPacketCGNewOfflineshop) +*/ OfflineshopPacketCloseMyAuction(ch);

	default:
		sys_err("UNKNOWN SUBHEADER %d ",pPack->bSubHeader);
		return -1;
	}

}
#endif

void CInputMain::ItemDestroy(LPCHARACTER ch, const char * data)
{
	struct command_item_destroy * pinfo = (struct command_item_destroy *) data;
	if (ch) {
#ifdef ENABLE_RESTRICT_GM_PERMISSIONS
		if (ch->GetGMLevel() > GM_PLAYER && ch->GetGMLevel() < GM_IMPLEMENTOR) {
			return;
		}
#endif
		ch->DestroyItem(pinfo->Cell);
	}
}

void CInputMain::ItemDivision(LPCHARACTER ch, const char * data)
{
	struct command_item_division * pinfo = (struct command_item_division *) data;
	if (ch)
		ch->ItemDivision(pinfo->pos);
}

#ifdef ENABLE_NEW_FISHING_SYSTEM
void CInputMain::FishingNew(LPCHARACTER ch, const char* c_pData)
{
	if (!ch) {
		return;
	}

	TPacketFishingNew* p = (TPacketFishingNew*)c_pData;
	switch (p->subheader) {
		case FISHING_SUBHEADER_NEW_START:
			{
				ch->SetRotation(p->dir * 5);
				ch->fishing_new_start();
			}
			break;
		case FISHING_SUBHEADER_NEW_STOP:
			{
				ch->SetRotation(p->dir * 5);
				ch->fishing_new_stop();
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH:
			{
				ch->fishing_new_catch();
			}
			break;
		case FISHING_SUBHEADER_NEW_CATCH_FAILED:
			{
				ch->fishing_new_catch_failed();
			}
			break;
		default:
			return;
	}
}
#endif

#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
void CInputMain::WheelDestiny(LPCHARACTER ch, const char* data)
{
	if (!ch)
	{
		return;
	}

	if (ch->IsObserverMode() || ch->GetExchange())
	{
		return;
	}

	const auto pinfo = reinterpret_cast<const TPacketCGWheelDestiny*>(data);
	enum { OPEN, CLOSE, TURN, GIVE };

	switch (pinfo->option)
	{
		case OPEN:
			{
				if (quest::CQuestManager::instance().GetEventFlag("christmas_event") == 0)
				{
#ifdef TEXTS_IMPROVEMENT
					ch->ChatPacketNew(CHAT_TYPE_INFO, 1306, "");
#endif
					return;
				}

				if (!ch->GetWheelDestiny())
				{
					ch->SetWheelDestiny(std::make_shared<CWheelDestiny>(ch));
				}
			}
			break;
		case CLOSE:
			{
				if (ch->GetWheelDestiny())
				{
					if (ch->GetWheelDestiny()->GetGiftVnum())
					{
#ifdef TEXTS_IMPROVEMENT
						ch->ChatPacketNew(CHAT_TYPE_INFO, 1307, "");
#endif
					}
					else
					{
						ch->SetWheelDestiny(nullptr);
						ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_WHEEL_CLOSE");
					}
				}
			}
			break;
		case TURN:
			{
				if (ch->GetWheelDestiny())
				{
					ch->GetWheelDestiny()->TurnWheel();
				}
			}
			break;
		case GIVE:
			{
				if (ch->GetWheelDestiny())
				{
					ch->GetWheelDestiny()->GiveMyGift();
				}
			}
			break;
		default:
			{
				sys_err("CInputMain::WheelDestiny : Unknown option %d : %s", pinfo->option, ch->GetName());
			}
			break;
	}
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}

			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);
			// @fixme103 (removed CheckClientVersion since useless in here)
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

#ifdef ENABLE_ACCE_SYSTEM
		case HEADER_CG_ACCE:
			Acce(ch, c_pData);
			break;
#endif

		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;
		case HEADER_CG_ITEM_DESTROY:
			if (!ch->IsObserverMode())
				ItemDestroy(ch, c_pData);
			break;
		case HEADER_CG_ITEM_DIVISION:
			{
				if (!ch->IsObserverMode())
					ItemDivision(ch, c_pData);
			}
			break;
		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;


#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
		case ENVANTER_BLACK:
			if (!ch->IsObserverMode())
				InventoryExpansion(ch, c_pData);
		break;
#endif

		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;

		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if (!ch->IsObserverMode())
				Exchange(ch, c_pData);
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL:
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

#ifdef __SKILL_COLOR_SYSTEM__
		case HEADER_CG_SKILL_COLOR:
			SetSkillColor(ch, c_pData);
			break;
#endif
#ifdef ENABLE_OPENSHOP_PACKET
		case HEADER_CG_OPENSHOP: {
				TPacketOpenShop* p = reinterpret_cast<TPacketOpenShop*>((void*)c_pData);
				if (p->shopid > 0) {
					if (!(ch->IsObserverMode() || ch->IsOpenSafebox() || ch->GetExchange() || ch->IsCubeOpen() || ch->IsStun() || ch->IsDead()
#ifdef __ATTR_TRANSFER_SYSTEM__
						 || ch->IsAttrTransferOpen()
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
						 || ch->GetOfflineShopGuest() || ch->GetAuctionGuest()
#endif
					)) {
						LPSHOP shop = CShopManager::instance().Get(p->shopid);
						if (shop) {
							shop->AddGuest(ch, 0, false);
							ch->SetShopOwner(NULL);
						}
					}
				}
			} break;
#endif
		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;

#ifdef ENABLE_BATTLE_PASS
		case HEADER_CG_BATTLE_PASS:
			if ((iExtraLen = BattlePass(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

			// SCRIPT_SELECT_ITEM
		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;
			// END_OF_SCRIPT_SELECT_ITEM

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;

		case HEADER_CG_PARTY_INVITE:
			PartyInvite(ch, c_pData);
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL:
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;
#ifdef __INGAME_WIKI__
		case InGameWiki::HEADER_CG_WIKI:
			RecvWikiPacket(ch, c_pData);
			break;
#endif
		case HEADER_CG_ANSWER_MAKE_GUILD:
#ifdef ENABLE_NEWGUILDMAKE
			ch->ChatPacket(CHAT_TYPE_INFO, "<%s> AnswerMakeGuild disabled", __FUNCTION__);
#else
			AnswerMakeGuild(ch, c_pData);
#endif
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;
		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;
			
#ifdef __NEWPET_SYSTEM__
		case HEADER_CG_PetSetName:
			BraveRequestPetName(ch, c_pData);
			break;
#endif
		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
		case HEADER_CG_WHISPER_ADMIN:
			CWhisperAdmin::instance().Manager(ch, c_pData);
			break;
#endif

		case HEADER_CG_CLIENT_VERSION:
			Version(ch, c_pData);
			break;


#ifdef ENABLE_MULTI_LANGUAGE
		case HEADER_CG_CHANGE_LANGUAGE:
			{
				TPacketChangeLanguage* p = reinterpret_cast <TPacketChangeLanguage*>((void*)c_pData);
				ChangeLanguage(ch, p->bLanguage);
			}
			break;
		case HEADER_CG_REQUEST_LANGUAGE:
			{
				TPacketRequestLang* p = reinterpret_cast <TPacketRequestLang*>((void*)c_pData);
				RequestLanguage(ch, p->targetName);
			}
			break;
#endif
#ifdef __SEND_TARGET_INFO__
		case HEADER_CG_TARGET_INFO_LOAD:
			{
				TargetInfoLoad(ch, c_pData);
			}
			break;
#endif
#ifdef ENABLE_SWITCHBOT
		case HEADER_CG_SWITCHBOT:
			if ((iExtraLen = Switchbot(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif
#ifdef ENABLE_MAP_TELEPORTER
		case HEADER_CG_MAP_TELEPORTER:
			MapTeleporter(ch, (TPacketCGMapTeleporter*) c_pData);
			break;
#endif
		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
				}
			}
			break;
#ifdef ENABLE_DS_REFINE_ALL
		case HEADER_CG_DRAGON_SOUL_REFINE_ALL: {
			TPacketDragonSoulRefineAll* p = reinterpret_cast <TPacketDragonSoulRefineAll*>((void*)c_pData);
			DSManager::instance().DoRefineAll(ch, p->subheader, p->type, p->grade);
		} break;
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		case HEADER_CG_NEW_OFFLINESHOP:
			if((iExtraLen = OfflineshopPacket(c_pData, ch, m_iBufferLeft))< 0)
				return -1;
			break;
#endif
#ifdef ENABLE_CUBE_RENEWAL_WORLDARD
		case HEADER_CG_CUBE_RENEWAL:
			CubeRenewalSend(ch, c_pData);
			break;
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
		case HEADER_CG_FISHING_NEW:
			{
				FishingNew(ch, c_pData);
			}
			break;
#endif
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
		case HEADER_CG_WHEEL_DESTINY:
			{
				WheelDestiny(ch, c_pData);
			}
			break;
#endif
	}
	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return 0;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d);
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		default:
			return (0);
	}

	return (iExtraLen);
}
#ifdef ENABLE_SWITCHBOT
int CInputMain::Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	const TPacketCGSwitchbot* p = reinterpret_cast<const TPacketCGSwitchbot*>(data);

	if (uiBytes < sizeof(TPacketCGSwitchbot))
	{
		return -1;
	}

	const char* c_pData = data + sizeof(TPacketCGSwitchbot);
	uiBytes -= sizeof(TPacketCGSwitchbot);

	switch (p->subheader)
	{
	case SUBHEADER_CG_SWITCHBOT_START:
	{
		size_t extraLen = sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
		if (uiBytes < extraLen)
		{
			return -1;
		}

		std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives;

		for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
		{
			const TSwitchbotAttributeAlternativeTable* pAttr = reinterpret_cast<const TSwitchbotAttributeAlternativeTable*>(c_pData);
			c_pData += sizeof(TSwitchbotAttributeAlternativeTable);

			vec_alternatives.emplace_back(*pAttr);
		}

		CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, vec_alternatives);
		return extraLen;
	}

	case SUBHEADER_CG_SWITCHBOT_STOP:
	{
		CSwitchbotManager::Instance().Stop(ch->GetPlayerID(), p->slot);
		return 0;
	}
	}

	return 0;
}
#endif



#ifdef ENABLE_MULTI_LANGUAGE
void CInputMain::ChangeLanguage(LPCHARACTER ch, BYTE bLanguage)
{
	if (!ch)
		return;
	
	if (!ch->GetDesc())
		return;

	BYTE bCurrentLanguage = ch->GetDesc()->GetLanguage();
	
	if(bCurrentLanguage == bLanguage)
		return;
	
	if(bLanguage > LANGUAGE_DEFAULT && bLanguage < LANGUAGE_MAX_NUM)
	{
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE account.account SET language = %d WHERE id = %d;", bLanguage, ch->GetAID()));
		ch->GetDesc()->SetLanguage(bLanguage);
	}
}

void CInputMain::RequestLanguage(LPCHARACTER ch, const char* targetName)
{
	if (!ch)
		return;
	
	LPDESC d = ch->GetDesc();
	if (!d)
		return;
	
	int id = 0;
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT account_id FROM player.player WHERE name='%s'", targetName));
	if (pMsg->Get()->uiNumRows != 0) {
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		id = atoi(row[0]);
	}
	
	if (id == 0)
		return;
	
	std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT language FROM account.account WHERE id=%d", id));
	if (pMsg2->Get()->uiNumRows != 0) {
		MYSQL_ROW row = mysql_fetch_row(pMsg2->Get()->pSQLResult);
		
		TPacketRecvLang p;
		p.bHeader = HEADER_GC_RECV_LANGUAGE;
		strncpy(p.targetName, targetName, sizeof(p.targetName));
		strncpy(p.targetLanguage, row[0], sizeof(p.targetLanguage));
		d->Packet(&p, sizeof(p));
	}
}
#endif

