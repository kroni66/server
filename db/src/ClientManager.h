// vim:ts=8 sw=4
#ifndef __INC_CLIENTMANAGER_H__
#define __INC_CLIENTMANAGER_H__

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include "../../common/stl.h"
#include "../../common/building.h"

#include "Peer.h"
#include "DBManager.h"
#include "LoginData.h"
#ifdef __ENABLE_NEW_OFFLINESHOP__
#include "OfflineshopCache.h"
#endif
#include <unordered_map>
#include <chrono>

#define ENABLE_PROTO_FROM_DB

class CPlayerTableCache;
class CItemCache;
class CItemPriceListTableCache;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	class CSKillColorCache;
#endif

class CPacketInfo
{
    public:
	void Add(int header);
	void Reset();

	std::map<int, int> m_map_info;
};

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab);

class CClientManager : public CNetBase, public singleton<CClientManager>
{
    public:
	typedef std::list<CPeer *>			TPeerList;
	typedef boost::unordered_map<DWORD, CPlayerTableCache *> TPlayerTableCacheMap;
	typedef boost::unordered_map<DWORD, CItemCache *> TItemCacheMap;
	typedef boost::unordered_set<CItemCache *, boost::hash<CItemCache*> > TItemCacheSet;
	typedef boost::unordered_map<DWORD, TItemCacheSet *> TItemCacheSetPtrMap;
	typedef boost::unordered_map<DWORD, CItemPriceListTableCache*> TItemPriceListCacheMap;
	typedef boost::unordered_map<short, BYTE> TChannelStatusMap;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	typedef boost::unordered_map<DWORD, CSKillColorCache *> TSkillColorCacheMap;
#endif

	// MYSHOP_PRICE_LIST
	/// ������ �������� ����Ʈ ��û ����
	/**
	 * first: Peer handle
	 * second: ��û�� �÷��̾��� ID
	 */
	typedef std::pair< DWORD, DWORD >		TItemPricelistReqInfo;
	// END_OF_MYSHOP_PRICE_LIST

	class ClientHandleInfo
	{
	    public:
		DWORD	dwHandle;
		DWORD	account_id;
		DWORD	player_id;
		BYTE	account_index;
		char	login[LOGIN_MAX_LEN + 1];
		char	safebox_password[SAFEBOX_PASSWORD_MAX_LEN + 1];
		char	ip[MAX_HOST_LENGTH + 1];

		TAccountTable * pAccountTable;
		TSafeboxTable * pSafebox;

		ClientHandleInfo(DWORD argHandle, DWORD dwPID = 0)
		{
		    dwHandle = argHandle;
		    pSafebox = NULL;
		    pAccountTable = NULL;
		    player_id = dwPID;
		};
		//���ϼ�����ɿ� ������
		ClientHandleInfo(DWORD argHandle, DWORD dwPID, DWORD accountId)
		{
		    dwHandle = argHandle;
		    pSafebox = NULL;
		    pAccountTable = NULL;
		    player_id = dwPID;
			account_id = accountId;
		};

		~ClientHandleInfo()
		{
		    if (pSafebox)
			{
				delete pSafebox;
				pSafebox = NULL;
			}
		}
	};

	public:
	CClientManager();
	~CClientManager();

	bool	Initialize();
	time_t	GetCurrentTime();

	void	MainLoop();
	void	Quit();

	void	GetPeerP2PHostNames(std::string& peerHostNames);
	void	SetTablePostfix(const char* c_pszTablePostfix);
	void	SetPlayerIDStart(int iIDStart);
	int	GetPlayerIDStart() { return m_iPlayerIDStart; }

	int	GetPlayerDeleteLevelLimit() { return m_iPlayerDeleteLevelLimit; }

	DWORD	GetUserCount();	// ���ӵ� ����� ���� ���� �Ѵ�.

	void	SendAllGuildSkillRechargePacket();
	void	SendTime();

	CPlayerTableCache *	GetPlayerCache(DWORD id);
	void			PutPlayerCache(TPlayerTable * pNew);

	void			CreateItemCacheSet(DWORD dwID);
	TItemCacheSet *		GetItemCacheSet(DWORD dwID);
	void			FlushItemCacheSet(DWORD dwID);

	CItemCache *		GetItemCache(DWORD id);
	void			PutItemCache(TPlayerItem * pNew, bool bSkipQuery = false);
	bool			DeleteItemCache(DWORD id);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	CSKillColorCache*	GetSkillColorCache(DWORD id);
	void				PutSkillColorCache(const TSkillColor* pNew);
	void				UpdateSkillColorCache();
#endif

	void			UpdatePlayerCache();
	void			UpdateItemCache();

	// MYSHOP_PRICE_LIST
	/// �������� ����Ʈ ĳ�ø� �����´�.
	/**
	 * @param [in]	dwID �������� ����Ʈ�� ������.(�÷��̾� ID)
	 * @return	�������� ����Ʈ ĳ���� ������
	 */
	CItemPriceListTableCache*	GetItemPriceListCache(DWORD dwID);

	/// �������� ����Ʈ ĳ�ø� �ִ´�.
	/**
	 * @param [in]	pItemPriceList ĳ�ÿ� ���� ������ �������� ����Ʈ
	 *
	 * ĳ�ð� �̹� ������ Update �� �ƴ� replace �Ѵ�.
	 */
	void			PutItemPriceListCache(const TItemPriceListTable* pItemPriceList);


	/// Flush �ð��� ����� ������ �������� ����Ʈ ĳ�ø� Flush ���ְ� ĳ�ÿ��� �����Ѵ�.
	void			UpdateItemPriceListCache(void);
	// END_OF_MYSHOP_PRICE_LIST


	void			SendGuildSkillUsable(DWORD guild_id, DWORD dwSkillVnum, bool bUsable);

	void			SetCacheFlushCountLimit(int iLimit);

	template <class Func>
	Func		for_each_peer(Func f);

	CPeer *		GetAnyPeer();

	void			ForwardPacket(BYTE header, const void* data, int size, BYTE bChannel = 0, CPeer * except = NULL);

	void			SendNotice(const char * c_pszFormat, ...);

	// @fixme203 directly GetCommand instead of strcpy
	char*			GetCommand(char* str, char* command);		//���ϼ�����ɿ��� ���ɾ� ��� �Լ�
	void			ItemAward(CPeer * peer, char* login);	//���� ���� ���

    protected:
	void	Destroy();

    private:
	bool		InitializeTables();
	bool		InitializeShopTable();
	bool		InitializeMobTable();
	bool		InitializeItemTable();
	bool		InitializeQuestItemTable();
	bool		InitializeSkillTable();
	bool		InitializeRefineTable();
	bool		InitializeBanwordTable();
	bool		InitializeItemAttrTable();
	bool		InitializeItemRareTable();
	bool		InitializeLandTable();
	bool		InitializeObjectProto();
	bool		InitializeObjectTable();
#ifdef ENABLE_ITEM_EXTRA_PROTO
	bool		InitializeItemExtraProtoTable();
	void		SendItemExtraProtoTable(CPeer* peer);
#endif

	// mob_proto.txt, item_proto.txt���� ���� mob_proto, item_proto�� real db�� �ݿ�.
	//	item_proto, mob_proto�� db�� �ݿ����� �ʾƵ�, ���� ���ư��µ��� ������ ������,
	//	��� ��� db�� item_proto, mob_proto�� �о� ���� ������ ������ �߻��Ѵ�.
	bool		MirrorMobTableIntoDB();
	bool		MirrorItemTableIntoDB();

	void		AddPeer(socket_t fd);
	void		RemovePeer(CPeer * pPeer);
	CPeer *		GetPeer(IDENT ident);

	int		AnalyzeQueryResult(SQLMsg * msg);
	int		AnalyzeErrorMsg(CPeer * peer, SQLMsg * msg);

	int		Process();

        void            ProcessPackets(CPeer * peer);

	CLoginData *	GetLoginData(DWORD dwKey);
	CLoginData *	GetLoginDataByLogin(const char * c_pszLogin);
	CLoginData *	GetLoginDataByAID(DWORD dwAID);

	void		InsertLoginData(CLoginData * pkLD);
	void		DeleteLoginData(CLoginData * pkLD);

	bool		InsertLogonAccount(const char * c_pszLogin, DWORD dwHandle, const char * c_pszIP);
	bool		DeleteLogonAccount(const char * c_pszLogin, DWORD dwHandle);
	bool		FindLogonAccount(const char * c_pszLogin);

	void		GuildCreate(CPeer * peer, DWORD dwGuildID);
	void		GuildSkillUpdate(CPeer * peer, TPacketGuildSkillUpdate* p);
	void		GuildExpUpdate(CPeer * peer, TPacketGuildExpUpdate* p);
	void		GuildAddMember(CPeer * peer, TPacketGDGuildAddMember* p);
	void		GuildChangeGrade(CPeer * peer, TPacketGuild* p);
	void		GuildRemoveMember(CPeer * peer, TPacketGuild* p);
	void		GuildChangeMemberData(CPeer * peer, TPacketGuildChangeMemberData* p);
	void		GuildDisband(CPeer * peer, TPacketGuild * p);
	void		GuildWar(CPeer * peer, TPacketGuildWar * p);
	void		GuildWarScore(CPeer * peer, TPacketGuildWarScore * p);
	
#ifdef ADVANCED_GUILD_INFO
	void		GuildResetInfo();
#endif
	
	void		GuildChangeLadderPoint(TPacketGuildLadderPoint* p);
	void		GuildUseSkill(TPacketGuildUseSkill* p);
	void		GuildDepositMoney(TPacketGDGuildMoney* p);
	void		GuildWithdrawMoney(CPeer* peer, TPacketGDGuildMoney* p);
	void		GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply* p);
	void		GuildWarBet(TPacketGDGuildWarBet * p);
	void		GuildChangeMaster(TPacketChangeGuildMaster* p);

	void		SetGuildWarEndTime(DWORD guild_id1, DWORD guild_id2, time_t tEndTime);

	void		QUERY_BOOT(CPeer * peer, TPacketGDBoot * p);

	void		QUERY_LOGIN(CPeer * peer, DWORD dwHandle, SLoginPacket* data);
	void		QUERY_LOGOUT(CPeer * peer, DWORD dwHandle, const char *);

	void		RESULT_LOGIN(CPeer * peer, SQLMsg *msg);

	void		QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket*);
	void		RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID);
	void		RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo);
	void		RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	void		RESULT_QUEST_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID);
	// @fixme402 (RESULT_AFFECT_LOAD +dwRealPID)
	void		RESULT_AFFECT_LOAD(CPeer * pkPeer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID);

#ifdef ENABLE_BATTLE_PASS
	void RESULT_BATTLE_PASS_LOAD(CPeer* peer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwRealPID);
	void QUERY_SAVE_BATTLE_PASS(CPeer* peer, DWORD dwHandle, TPlayerBattlePassMission* battlePass);
	void QUERY_REGISTER_RANKING(CPeer* peer, DWORD dwHandle, TBattlePassRegisterRanking* pRanking);

	bool LoadBattlePassRanking();
	void RequestLoadBattlePassRanking(CPeer* peer, DWORD dwHandle, const char* data);
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	void		QUERY_SKILL_COLOR_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket * packet);
	void		RESULT_SKILL_COLOR_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle);
#endif

	// PLAYER_INDEX_CREATE_BUG_FIX
	void		RESULT_PLAYER_INDEX_CREATE(CPeer *pkPeer, SQLMsg *msg);
	// END_PLAYER_INDEX_CREATE_BUG_FIX

	// MYSHOP_PRICE_LIST
	/// �������� �ε� ������ ���� Result ó��
	/**
	 * @param	peer ���������� ��û�� Game server �� peer ��ü ������
	 * @param	pMsg ������ Result �� ���� ��ü�� ������
	 *
	 * �ε�� �������� ����Ʈ�� ĳ�ÿ� �����ϰ� peer ���� ����Ʈ�� �����ش�.
	 */
	void		RESULT_PRICELIST_LOAD(CPeer* peer, SQLMsg* pMsg);

	/// �������� ������Ʈ�� ���� �ε� ������ ���� Result ó��
	/**
	 * @param	pMsg ������ Result �� ���� ��ü�� ������
	 *
	 * �ε�� ������ �������� ����Ʈ ĳ�ø� ����� ������Ʈ ���� ���������� ������Ʈ �Ѵ�.
	 */
	void		RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg* pMsg);
	// END_OF_MYSHOP_PRICE_LIST

	void		QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable*);

	void		__QUERY_PLAYER_CREATE(CPeer * peer, DWORD dwHandle, TPlayerCreatePacket *);
	void		__QUERY_PLAYER_DELETE(CPeer * peer, DWORD dwHandle, TPlayerDeletePacket *);
	void		__RESULT_PLAYER_DELETE(CPeer * peer, SQLMsg* msg);

	void		QUERY_PLAYER_COUNT(CPeer * pkPeer, TPlayerCountPacket *);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	void		QUERY_SKILL_COLOR_SAVE(const char * c_pData);
#endif

	void		QUERY_ITEM_SAVE(CPeer * pkPeer, const char * c_pData);
	void		QUERY_ITEM_DESTROY(CPeer * pkPeer, const char * c_pData);
	void		QUERY_ITEM_FLUSH(CPeer * pkPeer, const char * c_pData);


	void		QUERY_QUEST_SAVE(CPeer * pkPeer, TQuestTable *, DWORD dwLen);
	void		QUERY_ADD_AFFECT(CPeer * pkPeer, TPacketGDAddAffect * p);
	void		QUERY_REMOVE_AFFECT(CPeer * pkPeer, TPacketGDRemoveAffect * p);

	void		QUERY_SAFEBOX_LOAD(CPeer * pkPeer, DWORD dwHandle, TSafeboxLoadPacket *, bool bMall);
	void		QUERY_SAFEBOX_SAVE(CPeer * pkPeer, TSafeboxTable * pTable);
	void		QUERY_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangeSizePacket * p);
	void		QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangePasswordPacket * p);

	void		RESULT_SAFEBOX_LOAD(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, SQLMsg * msg);
	void		RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer * pkPeer, SQLMsg * msg);

	void		QUERY_EMPIRE_SELECT(CPeer * pkPeer, DWORD dwHandle, TEmpireSelectPacket * p);
	void		QUERY_SETUP(CPeer * pkPeer, DWORD dwHandle, const char * c_pData);

	void		SendPartyOnSetup(CPeer * peer);

	void		QUERY_HIGHSCORE_REGISTER(CPeer * peer, TPacketGDHighscore* data);
	void		RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg);

	void		QUERY_FLUSH_CACHE(CPeer * pkPeer, const char * c_pData);

	void		QUERY_PARTY_CREATE(CPeer * peer, TPacketPartyCreate* p);
	void		QUERY_PARTY_DELETE(CPeer * peer, TPacketPartyDelete* p);
	void		QUERY_PARTY_ADD(CPeer * peer, TPacketPartyAdd* p);
	void		QUERY_PARTY_REMOVE(CPeer * peer, TPacketPartyRemove* p);
	void		QUERY_PARTY_STATE_CHANGE(CPeer * peer, TPacketPartyStateChange* p);
	void		QUERY_PARTY_SET_MEMBER_LEVEL(CPeer * peer, TPacketPartySetMemberLevel* p);

	void		QUERY_RELOAD_PROTO();

	void		QUERY_CHANGE_NAME(CPeer * peer, DWORD dwHandle, TPacketGDChangeName * p);
	void		GetPlayerFromRes(TPlayerTable * player_table, MYSQL_RES* res);

	void		QUERY_SMS(CPeer * pkPeer, TPacketGDSMS * p);
	void		QUERY_LOGIN_KEY(CPeer * pkPeer, TPacketGDLoginKey * p);

	void		AddGuildPriv(TPacketGiveGuildPriv* p);
	void		AddEmpirePriv(TPacketGiveEmpirePriv* p);
	void		AddCharacterPriv(TPacketGiveCharacterPriv* p);

	void		MoneyLog(TPacketMoneyLog* p);

	void		QUERY_AUTH_LOGIN(CPeer * pkPeer, DWORD dwHandle, TPacketGDAuthLogin * p);

	void		QUERY_LOGIN_BY_KEY(CPeer * pkPeer, DWORD dwHandle, TPacketGDLoginByKey * p);
	void		RESULT_LOGIN_BY_KEY(CPeer * peer, SQLMsg * msg);

	void		ChargeCash(const TRequestChargeCash * p);

	void		LoadEventFlag();
	void		SetEventFlag(TPacketSetEventFlag* p);
	void		SendEventFlagsOnSetup(CPeer* peer);

	void		BillingExpire(TPacketBillingExpire * p);
	void		BillingCheck(const char * data);

	void		SendAllLoginToBilling();
	void		SendLoginToBilling(CLoginData * pkLD, bool bLogin);

	// ��ȥ
	void		MarriageAdd(TPacketMarriageAdd * p);
	void		MarriageUpdate(TPacketMarriageUpdate * p);
	void		MarriageRemove(TPacketMarriageRemove * p);

	void		WeddingRequest(TPacketWeddingRequest * p);
	void		WeddingReady(TPacketWeddingReady * p);
	void		WeddingEnd(TPacketWeddingEnd * p);
#if defined(BL_OFFLINE_MESSAGE)
	void		RequestReadOfflineMessages(CPeer* pkPeer, DWORD dwHandle, TPacketGDReadOfflineMessage* p);
	void		SendOfflineMessage(TPacketGDSendOfflineMessage* p);
	void		OfflineMessageGarbage();
#endif
#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
	void		FindChannel(CPeer* pkPeer, DWORD dwHandle, TPacketChangeChannel* p);
#endif

	// MYSHOP_PRICE_LIST
	// ���λ��� ��������

	/// ������ �������� ����Ʈ ������Ʈ ��Ŷ(HEADER_GD_MYSHOP_PRICELIST_UPDATE) ó���Լ�
	/**
	 * @param [in]	pPacket ��Ŷ �������� ������
	 */
	void		MyshopPricelistUpdate(const TItemPriceListTable* pPacket); // @fixme403 (TPacketMyshopPricelistHeader to TItemPriceListTable)

	/// ������ �������� ����Ʈ ��û ��Ŷ(HEADER_GD_MYSHOP_PRICELIST_REQ) ó���Լ�
	/**
	 * @param	peer ��Ŷ�� ���� Game server �� peer ��ü�� ������
	 * @param [in]	dwHandle ���������� ��û�� peer �� �ڵ�
	 * @param [in]	dwPlayerID �������� ����Ʈ�� ��û�� �÷��̾��� ID
	 */
	void		MyshopPricelistRequest(CPeer* peer, DWORD dwHandle, DWORD dwPlayerID);
	// END_OF_MYSHOP_PRICE_LIST

	// Building
	void		CreateObject(TPacketGDCreateObject * p);
	void		DeleteObject(DWORD dwID);
	void		UpdateLand(DWORD * pdw);

	// VCard
	void 		VCard(TPacketGDVCard * p);
	void		VCardProcess();

	// BLOCK_CHAT
	void		BlockChat(TPacketBlockChat * p);
	// END_OF_BLOCK_CHAT

#ifdef ENABLE_HWID
	void BlockHwid(THwidRequest * pack);
	void UnblockHwid(THwidRequest * pack);
#endif

    private:
	int					m_looping;
	socket_t				m_fdAccept;	// ���� �޴� ����
	TPeerList				m_peerList;

	CPeer *					m_pkAuthPeer;

	// LoginKey, LoginData pair
	typedef boost::unordered_map<DWORD, CLoginData *> TLoginDataByLoginKey;
	TLoginDataByLoginKey			m_map_pkLoginData;

	// Login LoginData pair
	typedef boost::unordered_map<std::string, CLoginData *> TLoginDataByLogin;
	TLoginDataByLogin			m_map_pkLoginDataByLogin;

	// AccountID LoginData pair
	typedef boost::unordered_map<DWORD, CLoginData *> TLoginDataByAID;
	TLoginDataByAID				m_map_pkLoginDataByAID;

	// Login LoginData pair (���� �α��� �Ǿ��ִ� ����)
	typedef boost::unordered_map<std::string, CLoginData *> TLogonAccountMap;
	TLogonAccountMap			m_map_kLogonAccount;

	int					m_iPlayerIDStart;
	int					m_iPlayerDeleteLevelLimit;
	int					m_iPlayerDeleteLevelLimitLower;

	std::vector<TMobTable>			m_vec_mobTable;
	std::vector<TItemTable>			m_vec_itemTable;
	std::map<DWORD, TItemTable *>		m_map_itemTableByVnum;


	int					m_iShopTableSize;
	TShopTable *				m_pShopTable;

	int					m_iRefineTableSize;
	TRefineTable*				m_pRefineTable;

	std::vector<TSkillTable>		m_vec_skillTable;
	std::vector<TBanwordTable>		m_vec_banwordTable;
	std::vector<TItemAttrTable>		m_vec_itemAttrTable;
	std::vector<TItemAttrTable>		m_vec_itemRareTable;

#ifdef ENABLE_ITEM_EXTRA_PROTO
	std::vector<TItemExtraProto> m_vec_itemExtraProto;
#endif

	std::vector<building::TLand>		m_vec_kLandTable;
	std::vector<building::TObjectProto>	m_vec_kObjectProto;
	std::map<DWORD, building::TObject *>	m_map_pkObjectTable;

	std::queue<TPacketGDVCard>		m_queue_vcard;

	bool					m_bShutdowned;

	TPlayerTableCacheMap			m_map_playerCache;  // �÷��̾� id�� key

	TItemCacheMap				m_map_itemCache;  // ������ id�� key
	TItemCacheSetPtrMap			m_map_pkItemCacheSetPtr;  // �÷��̾� id�� key, �� �÷��̾ � ������ ĳ���� ������ �ֳ�?

#ifdef ENABLE_BATTLE_PASS
	std::vector<TBattlePassRanking *> m_vec_battlePassRanking;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	TSkillColorCacheMap			m_map_SkillColorCache;
#endif

	// MYSHOP_PRICE_LIST
	/// �÷��̾ ������ �������� ����Ʈ map. key: �÷��̾� ID, value: �������� ����Ʈ ĳ��
	TItemPriceListCacheMap m_mapItemPriceListCache;  ///< �÷��̾ ������ �������� ����Ʈ
	// END_OF_MYSHOP_PRICE_LIST

	TChannelStatusMap m_mChannelStatus;

	struct TPartyInfo
	{
	    BYTE bRole;
	    BYTE bLevel;

		TPartyInfo() :bRole(0), bLevel(0)
		{
		}
	};

	typedef std::map<DWORD, TPartyInfo>	TPartyMember;
	typedef std::map<DWORD, TPartyMember>	TPartyMap;
	typedef std::map<BYTE, TPartyMap>	TPartyChannelMap;
	TPartyChannelMap m_map_pkChannelParty;

	typedef std::map<std::string, long>	TEventFlagMap;
	TEventFlagMap m_map_lEventFlag;
#if defined(BL_OFFLINE_MESSAGE)
	struct SOfflineMessage
	{
		std::string From;
		std::string Message;
		std::chrono::system_clock::time_point t;
		SOfflineMessage(const char* szFrom, const char* szMessage)
			: From(szFrom), Message(szMessage), t(std::chrono::system_clock::now()) {}
	};
	std::unordered_map<std::string, std::vector<std::shared_ptr<SOfflineMessage>>> m_OfflineMessage;
#endif
	
	BYTE					m_bLastHeader;
	int					m_iCacheFlushCount;
	int					m_iCacheFlushCountLimit;

    private :
	TItemIDRangeTable m_itemRange;

    public :
	bool InitializeNowItemID();
	DWORD GetItemID();
	DWORD GainItemID();
	TItemIDRangeTable GetItemRange() { return m_itemRange; }
#ifdef __ENABLE_NEW_OFFLINESHOP__
public:
	//booting
	bool	InitializeOfflineshopTable();
	void	SendOfflineshopTable(CPeer* peer);


	//packets exchanging
	void	RecvOfflineShopPacket(CPeer* peer, const char* data);
	bool	RecvOfflineShopBuyItemPacket(const char* data);
	bool	RecvOfflineShopLockBuyItem(CPeer* peer, const char* data);
	bool	RecvOfflineShopCannotBuyLockItem(const char* data); //topatch
	bool	RecvOfflineShopEditItemPacket(const char* data);
	bool	RecvOfflineShopRemoveItemPacket(const char* data);
	bool	RecvOfflineShopAddItemPacket(const char* data);

	bool	RecvOfflineShopForceClose(const char* data);
	bool	RecvOfflineShopCreateNew(const char* data);
	bool	RecvOfflineShopChangeName(const char* data);

	bool	RecvOfflineShopOfferCreate(const char* data);
	bool	RecvOfflineShopOfferNotified(const char* data);
	bool	RecvOfflineShopOfferAccepted(const char* data);
	bool	RecvOfflineshopOfferCancel(const char* data);

	bool	RecvOfflineShopSafeboxGetItem(const char* data);
	bool	RecvOfflineShopSafeboxGetValutes(const char* data);
	bool	RecvOfflineShopSafeboxAddItem(const char* data);


	//AUCTION
	bool	RecvOfflineShopAuctionCreate(const char* data);
	bool	RecvOfflineShopAuctionAddOffer(const char* data);
	bool	RecvOfflineShopAuctionClose(const char* data);

	bool	SendOfflineShopBuyItemPacket(DWORD dwOwner, DWORD dwGuest, DWORD dwItem);
	bool	SendOfflineShopBuyLockedItemPacket(CPeer* peer,DWORD dwOwner, DWORD dwGuest, DWORD dwItem);
	bool	SendOfflineShopEditItemPacket(DWORD dwOwner, DWORD dwItem, const offlineshop::TPriceInfo& price);
	bool	SendOfflineShopRemoveItemPacket(DWORD dwOwner, DWORD dwItem);
	bool	SendOfflineShopAddItemPacket(DWORD dwOwner, DWORD dwItemID, const offlineshop::TItemInfo& rInfo);

	bool	SendOfflineShopForceClose(DWORD dwOwnerID);
	bool	SendOfflineShopCreateNew(const offlineshop::TShopInfo& shop, const std::vector<offlineshop::TItemInfo>& vec);
	bool	SendOfflineShopChangeName(DWORD dwOwnerID, const char* szName);
	bool	SendOfflineshopShopExpired(DWORD dwOwnerID);

	bool	SendOfflineShopOfferCreate(const offlineshop::TOfferInfo& offer);
	bool	SendOfflineShopOfferNotified(DWORD dwOwnerID, DWORD dwOfferID);
	bool	SendOfflineShopOfferAccepted(DWORD dwOwnerID, DWORD dwOfferID);

	//offlineshop-updated 05/08/19
	void	SendOfflineShopOfferCancel(DWORD dwOwnerID, DWORD dwOfferID, bool isRemovingItem);

	void	SendOfflineshopSafeboxAddItem(DWORD dwOwnerID, DWORD dwItem, const offlineshop::TItemInfoEx& item);
	void	SendOfflineshopSafeboxAddValutes(DWORD dwOwnerID, const offlineshop::TValutesInfo& valute);
	void	SendOfflineshopSafeboxLoad(CPeer* peer ,DWORD dwOwnerID, const offlineshop::TValutesInfo& valute, const std::vector<offlineshop::TItemInfoEx>& items , const std::vector<DWORD>& ids);
	//patch 08-03-2020
	void	SendOfflineshopSafeboxExpiredItem(DWORD dwOwnerID, DWORD itemID);

	//AUCTION
	void	SendOfflineshopAuctionCreate(const offlineshop::TAuctionInfo& auction);
	void	SendOfflineshopAuctionAddOffer(const offlineshop::TAuctionOfferInfo& offer);
	void	SendOfflineshopAuctionExpired(DWORD dwOwnerID);

	void	OfflineShopResultQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void	OfflineShopResultAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void	OfflineShopResultCreateShopQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void	OfflineShopResultCreateShopAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void	OfflineShopResultOfferAddQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);
	void	OfflineShopResultSafeboxAddItemQuery(CPeer* peer, SQLMsg* msg, CQueryInfo* pQueryInfo);


	void	OfflineshopDurationProcess();
	void	OfflineshopExpiredShop(DWORD dwID);
	void	OfflineshopExpiredAuction(DWORD dwID);
	void	OfflineshopLoadShopSafebox(CPeer* peer ,DWORD dwID);
	//patch 08-03-2020
	void	OfflineshopExpiredAuctionItem(DWORD dwOwnerID);

	//updated 15-01-2020
	bool	IsUsingOfflineshopSystem(DWORD dwID);

#ifdef ENABLE_NEW_OFFLINESHOP_LOGS
	void	OfflineshopLog(const DWORD dwOwnerID, const DWORD dwItemID, const char* fmt, ...);
#endif

	//patch offerlist loading fix
	void	RemoveOfferOnShopItem(DWORD dwItemID);

private:
	offlineshop::CShopCache			m_offlineshopShopCache;
	offlineshop::CSafeboxCache		m_offlineshopSafeboxCache;
	offlineshop::COfferCache		m_offlineshopOfferCache;
	offlineshop::CAuctionCache		m_offlineshopAuctionCache;
#endif

	//BOOT_LOCALIZATION
    public:
	/* ���� ���� �ʱ�ȭ
	 **/
	bool InitializeLocalization();

    private:
	std::vector<tLocale> m_vec_Locale;
	//END_BOOT_LOCALIZATION
	//ADMIN_MANAGER

	bool __GetAdminInfo(const char *szIP, std::vector<tAdminInfo> & rAdminVec);
	bool __GetHostInfo(std::vector<std::string> & rIPVec);
	//END_ADMIN_MANAGER


	//RELOAD_ADMIN
	void ReloadAdmin(CPeer * peer, TPacketReloadAdmin * p);
	//END_RELOAD_ADMIN
	void BreakMarriage(CPeer * peer, const char * data);

	struct TLogoutPlayer
	{
	    DWORD	pid;
	    time_t	time;

	    bool operator < (const TLogoutPlayer & r)
	    {
		return (pid < r.pid);
	    }
	};

	typedef boost::unordered_map<DWORD, TLogoutPlayer*> TLogoutPlayerMap;
	TLogoutPlayerMap m_map_logout;

	void InsertLogoutPlayer(DWORD pid);
	void DeleteLogoutPlayer(DWORD pid);
	void UpdateLogoutPlayer();
	void UpdateItemCacheSet(DWORD pid);

	void FlushPlayerCacheSet(DWORD pid);

	void SendSpareItemIDRange(CPeer* peer);

	void UpdateHorseName(TPacketUpdateHorseName* data, CPeer* peer);
	void AckHorseName(DWORD dwPID, CPeer* peer);
	void DeleteLoginKey(TPacketDC *data);
	void ResetLastPlayerID(const TPacketNeedLoginLogInfo* data);
	//delete gift notify icon
	void DeleteAwardId(TPacketDeleteAwardID* data);
	void UpdateChannelStatus(TChannelStatus* pData);
	void RequestChannelStatus(CPeer* peer, DWORD dwHandle);
#if defined(__ATTENDANCE_SYSTEM__)
private:
	using AttendanceCacheMap = std::map<DWORD, TAttendanceTable>;

	bool InitializeAttendanceTable();

	void LoadAttendance(CPeer* pPeer, DWORD dwHandle, const TAttendanceTable* pkTable);
	void SaveAttendance(CPeer* pPeer, DWORD dwHandle, const TAttendanceTable* pkTable);
	void FlushAttendance(bool bFlush = false);

	AttendanceCacheMap m_map_AttendanceCacheMap;
	int m_iAttendanceFlushDelaySec;
#endif
#ifdef ENABLE_PROTO_FROM_DB
	public:
	bool		InitializeMobTableFromDB();
	bool		InitializeItemTableFromDB();
	protected:
	bool		bIsProtoReadFromDB;
#endif
#ifdef ENABLE_ITEMSHOP
		protected:
			bool		InitializeItemshopCategoryTable();
			bool		InitializeItemshopItemTable();
			bool		InitializeItemshopSpecialOffers();
			bool		InitializePromotionCodes();
			bool		InitializeRedeemedPromotionCodes();

			void		ItemshopCheckBuy(CPeer* pkPeer, DWORD dwHandle, TItemshopCheckBuy* p);
			void		RemoveSingleItemshopItem(long long llItemIndex);
			void		AddSingleItemshopItem(TItemshopItemTable* item, long long llItemIndex);
			void		RedeemPromotionCode(CPeer* pkPeer, DWORD dwHandle, TPromotionRedeem* p);
			void		UpdatePromotionTables();
			void		UpdateItemshopTable();
		private:
			void		QUERY_RELOAD_ITEMSHOP();
			int						m_iItemshopTableCategorySize;
			TItemshopCategoryTable* m_pItemshopTableCategories;
			int						m_iItemshopTableItemSize;
			TItemshopItemTable* m_pItemshopTableItems;

			std::vector<unsigned long> m_vec_usedSeeds;

			std::unordered_map <std::string, std::pair<long long, TItemshopItemTable>> m_ItemshopItems;
			std::unordered_map <std::string, std::pair<DWORD, long long>> m_map_ItemshopLimitCountFlush;

			std::vector<TSpecialOfferItems>	m_vec_itemshopSpecialOfferItems;
			std::unordered_map<std::string, std::pair<long long, std::vector<TPromotionItemTable>>> m_map_promotionCodeInfo;
			std::unordered_map<DWORD, std::vector<std::string>> m_vec_redeemedPromotionCodes;
			std::unordered_map<DWORD, std::string> m_map_redeemedPromotionFlush;
#endif
};

template<class Func>
Func CClientManager::for_each_peer(Func f)
{
    TPeerList::iterator it;
    for (it = m_peerList.begin(); it!=m_peerList.end();++it)
    {
	f(*it);
    }
    return f;
}
#endif
