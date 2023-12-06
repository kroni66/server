#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <boost/unordered_map.hpp>
#include <unordered_map>
#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#if defined(__ATTENDANCE_SYSTEM__)
#include "attendance.h"
#endif
#ifndef ENABLE_CUBE_RENEWAL_WORLDARD
#include "cube.h"
#else
#include "cuberenewal.h"
#endif
#include "mining.h"

#include "utils.h"

#include "../../common/CommonDefines.h"

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
class CMountSystem;
#endif

#ifdef ENABLE_BATTLE_PASS
	#include "utils.h"
#endif

#ifdef __ATTR_TRANSFER_SYSTEM__
#include "attr_transfer.h"
#endif
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
#include "wheel_of_destiny.h"
#endif

#define ENABLE_ANTI_CMD_FLOOD
#define ENABLE_OPEN_SHOP_WITH_ARMOR
enum eMountType {MOUNT_TYPE_NONE=0, MOUNT_TYPE_NORMAL=1, MOUNT_TYPE_COMBAT=2, MOUNT_TYPE_MILITARY=3};
eMountType GetMountLevelByVnum(DWORD dwMountVnum, bool IsNew);
const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);


class CBuffOnAttributes;
class CPetSystem;
#ifdef __NEWPET_SYSTEM__
class CNewPetSystem;
#endif

#ifdef __ENABLE_NEW_OFFLINESHOP__
namespace offlineshop
{
	class CShop;
	class CShopSafebox;
	class CAuction;
}
#endif
#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)


#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
#ifdef ENABLE_WOLFMAN_CHARACTER
	MAIN_RACE_WOLFMAN_M,
#endif
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
#ifdef ENABLE_WOLFMAN_CHARACTER
	BLEEDING_LENGTH = 30,
#endif
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	//��ų
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
#ifdef ENABLE_WOLFMAN_CHARACTER
	DAMAGE_TYPE_BLEEDING,
#endif
};

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE= (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
#if defined(ENABLE_WOLFMAN_CHARACTER) && !defined(USE_MOB_BLEEDING_AS_POISON)
	DAMAGE_BLEEDING	= (1 << 6),
#endif
};

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8
	POINT_STAMINA,              // 9  ���׹̳�
	POINT_MAX_STAMINA,          // 10 �ִ� ���׹̳�

	POINT_GOLD,                 // 11
	POINT_ST,                   // 12 �ٷ�
	POINT_HT,                   // 13 ü��
	POINT_DX,                   // 14 ��ø��
	POINT_IQ,                   // 15 ���ŷ�
	POINT_DEF_GRADE,		// 16 ...
	POINT_ATT_SPEED,            // 17 ���ݼӵ�
	POINT_ATT_GRADE,		// 18 ���ݷ� MAX
	POINT_MOV_SPEED,            // 19 �̵��ӵ�
	POINT_CLIENT_DEF_GRADE,	// 20 �����
	POINT_CASTING_SPEED,        // 21 �ֹ��ӵ� (��ٿ�Ÿ��*100) / (100 + �̰�) = ���� ��ٿ� Ÿ��
	POINT_MAGIC_ATT_GRADE,      // 22 �������ݷ�
	POINT_MAGIC_DEF_GRADE,      // 23 ��������
	POINT_EMPIRE_POINT,         // 24 ��������
	POINT_LEVEL_STEP,           // 25 �� ���������� �ܰ�.. (1 2 3 �� �� ����, 4 �Ǹ� ���� ��)
	POINT_STAT,                 // 26 �ɷ�ġ �ø� �� �ִ� ����
	POINT_SUB_SKILL,		// 27 ���� ��ų ����Ʈ
	POINT_SKILL,		// 28 ��Ƽ�� ��ų ����Ʈ
	POINT_WEAPON_MIN,		// 29 ���� �ּ� ������
	POINT_WEAPON_MAX,		// 30 ���� �ִ� ������
	POINT_PLAYTIME,             // 31 �÷��̽ð�
	POINT_HP_REGEN,             // 32 HP ȸ����
	POINT_SP_REGEN,             // 33 SP ȸ����

	POINT_BOW_DISTANCE,         // 34 Ȱ �����Ÿ� ����ġ (meter)

	POINT_HP_RECOVERY,          // 35 ü�� ȸ�� ������
	POINT_SP_RECOVERY,          // 36 ���ŷ� ȸ�� ������

	POINT_POISON_PCT,           // 37 �� Ȯ��
	POINT_STUN_PCT,             // 38 ���� Ȯ��
	POINT_SLOW_PCT,             // 39 ���ο� Ȯ��
	POINT_CRITICAL_PCT,         // 40 ũ��Ƽ�� Ȯ��
	POINT_PENETRATE_PCT,        // 41 ����Ÿ�� Ȯ��
	POINT_CURSE_PCT,            // 42 ���� Ȯ��

	POINT_ATTBONUS_HUMAN,       // 43 �ΰ����� ����
	POINT_ATTBONUS_ANIMAL,      // 44 �������� ������ % ����
	POINT_ATTBONUS_ORC,         // 45 ���Ϳ��� ������ % ����
	POINT_ATTBONUS_MILGYO,      // 46 �б����� ������ % ����
	POINT_ATTBONUS_UNDEAD,      // 47 ��ü���� ������ % ����
	POINT_ATTBONUS_DEVIL,       // 48 ����(�Ǹ�)���� ������ % ����
	POINT_ATTBONUS_INSECT,      // 49 ������
	POINT_ATTBONUS_FIRE,        // 50 ȭ����
	POINT_ATTBONUS_ICE,         // 51 ������
	POINT_ATTBONUS_DESERT,      // 52 �縷��
	POINT_ATTBONUS_MONSTER,     // 53 ��� ���Ϳ��� ����
	POINT_ATTBONUS_WARRIOR,     // 54 ���翡�� ����
	POINT_ATTBONUS_ASSASSIN,	// 55 �ڰ����� ����
	POINT_ATTBONUS_SURA,		// 56 ���󿡰� ����
	POINT_ATTBONUS_SHAMAN,		// 57 ���翡�� ����
	POINT_ATTBONUS_TREE,     	// 58 �������� ���� 20050729.myevan UNUSED5

	POINT_RESIST_WARRIOR,		// 59 ���翡�� ����
	POINT_RESIST_ASSASSIN,		// 60 �ڰ����� ����
	POINT_RESIST_SURA,			// 61 ���󿡰� ����
	POINT_RESIST_SHAMAN,		// 62 ���翡�� ����

	POINT_STEAL_HP,             // 63 ������ ����
	POINT_STEAL_SP,             // 64 ���ŷ� ����

	POINT_MANA_BURN_PCT,        // 65 ���� ��

	/// ���ؽ� ���ʽ� ///

	POINT_DAMAGE_SP_RECOVER,    // 66 ���ݴ��� �� ���ŷ� ȸ�� Ȯ��

	POINT_BLOCK,                // 67 ������
	POINT_DODGE,                // 68 ȸ����

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74  ȭ��   ����   : ����� ����
	POINT_RESIST_FIRE,          // 75  ȭ��   ����   : ȭ�����ݿ� ���� ����� ����
	POINT_RESIST_ELEC,          // 76  ����   ����   : ������ݿ� ���� ����� ����
	POINT_RESIST_MAGIC,         // 77  ����   ����   : �������� ���� ����� ����
	POINT_RESIST_WIND,          // 78  �ٶ�   ����   : �ٶ����ݿ� ���� ����� ����

	POINT_REFLECT_MELEE,        // 79 ���� �ݻ�

	/// Ư�� ���ؽ� ///
	POINT_REFLECT_CURSE,		// 80 ���� �ݻ�
	POINT_POISON_REDUCE,		// 81 �������� ����

	/// �� �Ҹ�� ///
	POINT_KILL_SP_RECOVER,		// 82 �� �Ҹ�� MP ȸ��
	POINT_EXP_DOUBLE_BONUS,		// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,		// 85

	/// ȸ�� ���� ///
	POINT_POTION_BONUS,			// 86
	POINT_KILL_HP_RECOVERY,		// 87

	POINT_IMMUNE_STUN,			// 88
	POINT_IMMUNE_SLOW,			// 89
	POINT_IMMUNE_FALL,			// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,			// 93
	POINT_DEF_BONUS,			// 94

	POINT_ATT_GRADE_BONUS,		// 95
	POINT_DEF_GRADE_BONUS,		// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,		// 100
	POINT_HIT_SP_RECOVERY, 		// 101
	POINT_MANASHIELD,			// 102 ��ż�ȣ ��ų�� ���� �������� ȿ�� ����

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,			// 107
	POINT_POLYMORPH,			// 108 ������ ���� ��ȣ
	POINT_MOUNT,			// 109 Ÿ���ִ� ���� ��ȣ

	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,		// 112 ���� �ܾ� ����� ���� ���� ���� ����Ʈ (1�� 1����Ʈ ���°���)

	POINT_HORSE_SKILL,			// 113

	POINT_MALL_ATTBONUS,		// 114 ���ݷ� +x%
	POINT_MALL_DEFBONUS,		// 115 ���� +x%
	POINT_MALL_EXPBONUS,		// 116 ����ġ +x%
	POINT_MALL_ITEMBONUS,		// 117 ������ ����� x/10��
	POINT_MALL_GOLDBONUS,		// 118 �� ����� x/10��

	POINT_MAX_HP_PCT,			// 119 �ִ������ +x%
	POINT_MAX_SP_PCT,			// 120 �ִ����ŷ� +x%

	POINT_SKILL_DAMAGE_BONUS,		// 121 ��ų ������ *(100+x)%
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122 ��Ÿ ������ *(100+x)%

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123 ��ų ��� ������
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124 ��Ÿ ��� ������
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	// PC_BANG_ITEM_ADD
	POINT_PC_BANG_EXP_BONUS,		// 125 PC�� ���� ����ġ ���ʽ�
	POINT_PC_BANG_DROP_BONUS,		// 126 PC�� ���� ��ӷ� ���ʽ�
	// END_PC_BANG_ITEM_ADD
	POINT_RAMADAN_CANDY_BONUS_EXP,			// �󸶴� ���� ����ġ ������

	POINT_ENERGY = 128,					// 128 ���

	// ��� ui ��.
	// �������� ���� �ʱ⸸, Ŭ���̾�Ʈ���� ����� �� �ð��� POINT�� �����ϱ� ������ �̷��� �Ѵ�.
	// �� �β�����
	POINT_ENERGY_END_TIME = 129,					// 129 ��� ���� �ð�

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,

	// �߰� �Ӽ� ����
	POINT_RESIST_ICE = 133,          //   �ñ� ����   : �������ݿ� ���� ����� ����
	POINT_RESIST_EARTH = 134,        //   ���� ����   : �������ݿ� ���� ����� ����
	POINT_RESIST_DARK = 135,         //   ��� ����   : �������ݿ� ���� ����� ����

	POINT_RESIST_CRITICAL = 136,		// ũ��Ƽ�� ����	: ����� ũ��Ƽ�� Ȯ���� ����
	POINT_RESIST_PENETRATE = 137,		// ����Ÿ�� ����	: ����� ����Ÿ�� Ȯ���� ����

#ifdef ENABLE_WOLFMAN_CHARACTER
	POINT_BLEEDING_REDUCE = 138,
	POINT_BLEEDING_PCT = 139,

	POINT_ATTBONUS_WOLFMAN = 140,				// 140 ���������� ����
	POINT_RESIST_WOLFMAN = 141,				// 141 ���������� ����
	POINT_RESIST_CLAW = 142,					// 142 CLAW�� ����
#endif

#ifdef ENABLE_ACCE_SYSTEM
	POINT_ACCEDRAIN_RATE = 143,
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	POINT_RESIST_MAGIC_REDUCTION = 144,
#endif

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	POINT_INVEN = 145,
#endif


#ifdef ELEMENT_NEW_BONUSES
	POINT_ATTBONUS_ELEC = 146,
	POINT_ATTBONUS_WIND = 147,
	POINT_ATTBONUS_EARTH = 148,
	POINT_ATTBONUS_DARK = 149,
#ifdef ENABLE_NEW_BONUS_TALISMAN
	POINT_ATTBONUS_IRR_SPADA = 150,
	POINT_ATTBONUS_IRR_SPADONE = 151,
	POINT_ATTBONUS_IRR_PUGNALE = 152,
	POINT_ATTBONUS_IRR_FRECCIA = 153,
	POINT_ATTBONUS_IRR_VENTAGLIO = 154,
	POINT_ATTBONUS_IRR_CAMPANA = 155,
	POINT_RESIST_MEZZIUOMINI = 156,
	POINT_DEF_TALISMAN = 157,
	POINT_ATTBONUS_FORT_ZODIAC = 158,
#endif
#endif
#ifdef ENABLE_STRONG_METIN
	POINT_ATTBONUS_METIN = 159,
#endif
#ifdef ENABLE_STRONG_BOSS
	POINT_ATTBONUS_BOSS = 160,
#endif
#ifdef ENABLE_RESIST_MONSTER
	POINT_RESIST_MONSTER			= 161,
#endif
#ifdef ENABLE_MEDI_PVM
	POINT_ATTBONUS_MEDI_PVM				= 162,
#endif

#ifdef ENABLE_GAYA_SYSTEM
		POINT_GAYA = 163,
#endif

#ifdef ENABLE_BATTLE_PASS
	POINT_BATTLE_PASS_ID,
#endif
#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
	POINT_EXTRA_INVENTORY1 = 165,
	POINT_EXTRA_INVENTORY2,
	POINT_EXTRA_INVENTORY3,
	POINT_EXTRA_INVENTORY4,
	POINT_EXTRA_INVENTORY5,
#endif
	POINT_PVM_CRITICAL_PCT = 170,
#ifdef ENABLE_DS_RUNE
	POINT_RUNE_MONSTERS = 171,
#endif
#ifdef ENABLE_NEW_COMMON_BONUSES
	POINT_DOUBLE_DROP_ITEM = 172,
	POINT_IRR_WEAPON_DEFENSE = 173,
#endif
	POINT_FISHING_RARE = 174,
#ifdef ENABLE_NEW_USE_POTION
	POINT_PARTY_DROPEXP = 175,
#endif
#ifdef EANBLE_PATCH_YOHARA
	POINT_CHAMPION_LEVEL = 176,
	POINT_CHAMPION_ST,
	POINT_CHAMPION_HT,
	POINT_CHAMPION_DX,
	POINT_CHAMPION_IQ,
#endif
	//POINT_MAX_NUM = 129	common/length.h
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr {
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset() {
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs) {
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const {
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

/* �����ϴ� ������ */
typedef struct character_point
{
#ifdef ENABLE_LONG_LONG
	long long			points[POINT_MAX_NUM];
#else
	int			points[POINT_MAX_NUM];
#endif
	BYTE			job;
	BYTE			voice;

	BYTE			level;
#ifdef EANBLE_PATCH_YOHARA
	uint8_t championlevel;
#endif
	DWORD			exp;
#ifdef ENABLE_LONG_LONG
	long long		gold;
#else
	long			gold;
#endif
#ifdef ENABLE_GAYA_SYSTEM
	int				gaya;
#endif

#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
	int 			envanter;
#endif

	int				hp;
	int				sp;

	int				iRandomHP;
	int				iRandomSP;

	int				stamina;

	BYTE			skill_group;
} CHARACTER_POINT;

/* ������� �ʴ� ĳ���� ������ */
typedef struct character_point_instant
{
#ifdef ENABLE_LONG_LONG
	long long			points[POINT_MAX_NUM];
#else
	int			points[POINT_MAX_NUM];
#endif
	float			fRot;

	int				iMaxHP;
	int				iMaxSP;

	long			position;

	long			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;

	WORD			parts[PART_MAX_NUM];

// ��... ��¥ ���� �� �ҷ��� �� �� ���� ����.
// char�� �κ��� BYTE array�� grid�� �����ϰ�, exchange�� cube�� CGrid�� grid�� �����ϰ� ���� �̰�...
// grid�� ����� ������ grid�� ���� ���̾�!!!
// ���� ��ȥ�� �κ��� �Ȱ��� ���� ���� ���� �߸��ߴ� �Ф�
	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
	WORD			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];

	// ��ȥ�� �κ��丮.
	LPITEM			pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD			wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
#ifdef ENABLE_EXTRA_INVENTORY
	LPITEM			pExtraItems[EXTRA_INVENTORY_MAX_NUM];
	WORD			wExtraItemGrid[EXTRA_INVENTORY_MAX_NUM];
#endif
#ifdef ENABLE_SWITCHBOT
	LPITEM			pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif
	// by mhh
	LPITEM			pCubeItems[CUBE_MAX_NUM];
	LPCHARACTER		pCubeNpc;
#ifdef __ATTR_TRANSFER_SYSTEM__
	LPITEM			pAttrTransferItems[MAX_ATTR_TRANSFER_SLOT];
	LPCHARACTER		pAttrTransferNpc;
#endif
#ifdef ENABLE_ACCE_SYSTEM
	LPITEM				pAcceMaterials[ACCE_WINDOW_MAX_MATERIALS];
#endif
	LPCHARACTER			battle_victim;

	BYTE			gm_level;

	BYTE			bBasePart;	// ��� ��ȣ

	int				iMaxStamina;

	BYTE			bBlockMode;

	int				iDragonSoulActiveDeck;
	LPENTITY		m_pDragonSoulRefineWindowOpener;
} CHARACTER_POINT_INSTANT;

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	long	value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL)
		{
		}

		BYTE	bType;
		int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

typedef std::map<DWORD, size_t> target_map;
struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
	DWORD   dwVID;
	bool    isGrandMaster;

	target_map TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false),
		dwVID(0), isGrandMaster(false)
   	{}

	bool    HitOnce(DWORD dwVnum = 0);

	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;

class CShop;
typedef class CShop * LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

#ifdef DROP_WIKI
typedef std::vector<DWORD> ITEMDROP_MAP;
#endif

class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	protected:
		//////////////////////////////////////////////////////////////////////////////////
		// Entity ����
		virtual void	EncodeInsertPacket(LPENTITY entity);
		virtual void	EncodeRemovePacket(LPENTITY entity);
		//////////////////////////////////////////////////////////////////////////////////

	public:
		LPCHARACTER			FindCharacterInView(const char * name, bool bFindPCOnly);
		void				UpdatePacket();

		//////////////////////////////////////////////////////////////////////////////////
		// FSM (Finite State Machine) ����
	protected:
		CStateTemplate<CHARACTER>	m_stateMove;
		CStateTemplate<CHARACTER>	m_stateBattle;
		CStateTemplate<CHARACTER>	m_stateIdle;

	public:
		virtual void		StateMove();
		virtual void		StateBattle();
		virtual void		StateIdle();
		virtual void		StateFlag();
		virtual void		StateFlagBase();
		void				StateHorse();

	protected:
		// STATE_IDLE_REFACTORING
		void				__StateIdle_Monster();
		void				__StateIdle_Stone();
		void				__StateIdle_NPC();
		// END_OF_STATE_IDLE_REFACTORING

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }

		void				SetAggressive();
		bool				IsAggressive() const;

		void				SetCoward();
		bool				IsCoward() const;
		void				CowardEscape();

		void				SetNoAttackShinsu();
		bool				IsNoAttackShinsu() const;

		void				SetNoAttackChunjo();
		bool				IsNoAttackChunjo() const;

		void				SetNoAttackJinno();
		bool				IsNoAttackJinno() const;

		void				SetAttackMob();
		bool				IsAttackMob() const;

		virtual void			BeginStateEmpty();
		virtual void			EndStateEmpty() {}

		void				RestartAtSamePos();
#ifdef FLY_SYSTEM
	
		bool				isFlyingMount();
		void				SendFlyData(long target_fly, DWORD target_vid);
		void				SendFlyData(long fly);
#endif

	protected:
		DWORD				m_dwStateDuration;
		//////////////////////////////////////////////////////////////////////////////////

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void			Create(const char * c_pszName, DWORD vid, bool isPC);
		void			Destroy();

		void			Disconnect(const char * c_pszReason);
#ifdef FLY_SYSTEM
		float			FlyHeight;
		bool			FlyState;
#endif

	protected:
		void			Initialize();

		//////////////////////////////////////////////////////////////////////////////////
		// Basic Points
#ifdef ENABLE_SEND_TARGET_INFO
	private:
		DWORD			dwLastTargetInfoPulse;

	public:
		DWORD			GetLastTargetInfoPulse() const	{ return dwLastTargetInfoPulse; }
		void			SetLastTargetInfoPulse(DWORD pulse) { dwLastTargetInfoPulse = pulse; }
#endif
	public:
		DWORD			GetPlayerID() const	{ return m_dwPlayerID; }

		void			SetPlayerProto(const TPlayerTable * table);
		void			CreatePlayerProto(TPlayerTable & tab);	// ���� �� ���

		void			SetProto(const CMob * c_pkMob);
		WORD			GetRaceNum() const;

		void			Save();		// DelayedSave
		void			SaveReal();	// ���� ����
		void			FlushDelayedSaveItem();

#ifdef ENABLE_MULTI_NAMES
		const char *	GetName(BYTE lang = DEFAULT_LANGUAGE) const;
#else
		const char *	GetName() const;
#endif

		const VID &		GetVID() const		{ return m_vid;		}

		void			SetName(const std::string& name) { m_stName = name; }

		void			SetRace(BYTE race);
		bool			ChangeSex();

		DWORD			GetAID() const;
		int				GetChangeEmpireCount() const;
		void			SetChangeEmpireCount();
		int				ChangeEmpire(BYTE empire);

		BYTE			GetJob() const;
		BYTE			GetCharType() const;

		bool			IsPC() const		{ return GetDesc() ? true : false; }
		bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
		bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
		bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
		bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; }
		bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
		bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
//		bool			IsPet() const		{ return m_bCharType == CHAR_TYPE_PET; }

		DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
		void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int				GetLevel() const		{ return m_points.level;	}
		void			SetLevel(BYTE level);

		BYTE			GetGMLevel() const;
		BOOL 			IsGM() const;
		void			SetGMLevel();

		DWORD			GetExp() const		{ return m_points.exp;	}
		void			SetExp(DWORD exp)	{ m_points.exp = exp;	}
		
#ifdef __ENABLE_BLOCK_EXP__
		bool			Block_Exp;
#endif
		DWORD			GetNextExp() const;
#ifdef __NEWPET_SYSTEM__
		DWORD			PetGetNextExp() const;
#endif
		LPCHARACTER		DistributeExp();	// ���� ���� ���� ����� �����Ѵ�.
		void			DistributeHP(LPCHARACTER pkKiller);
		void			DistributeSP(LPCHARACTER pkKiller, int iMethod=0);

		void			SetPosition(int pos);
		bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
		int				GetPosition() const		{ return m_pointsInstant.position; }

		void			SetPart(BYTE bPartPos, WORD wVal);
		WORD			GetPart(BYTE bPartPos) const;
		WORD			GetOriginalPart(BYTE bPartPos) const;

		void			SetHP(int hp)		{ m_points.hp = hp; }
		int				GetHP() const		{ return m_points.hp; }

		void			SetSP(int sp)		{ m_points.sp = sp; }
		int				GetSP() const		{ return m_points.sp; }

		void			SetStamina(int stamina)	{ m_points.stamina = stamina; }
		int				GetStamina() const		{ return m_points.stamina; }

		void			SetMaxHP(int iVal)	{ m_pointsInstant.iMaxHP = iVal; }
		int				GetMaxHP() const	{ return m_pointsInstant.iMaxHP; }

		void			SetMaxSP(int iVal)	{ m_pointsInstant.iMaxSP = iVal; }
		int				GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }

		void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
		int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }

		void			SetRandomHP(int v)	{ m_points.iRandomHP = v; }
		void			SetRandomSP(int v)	{ m_points.iRandomSP = v; }

		int				GetRandomHP() const	{ return m_points.iRandomHP; }
		int				GetRandomSP() const	{ return m_points.iRandomSP; }

		int				GetHPPct() const;

		void			SetRealPoint(BYTE idx, int val);
		int				GetRealPoint(BYTE idx) const;

		void			SetPoint(BYTE idx, long long val);
#ifdef ENABLE_LONG_LONG
		long long		GetPoint(BYTE idx) const;
#else
		int				GetPoint(BYTE idx) const;
#endif
		int				GetLimitPoint(BYTE idx) const;
		int				GetPolymorphPoint(BYTE idx) const;

		const TMobTable &	GetMobTable() const;
		BYTE				GetMobRank() const;
		BYTE				GetMobBattleType() const;
		BYTE				GetMobSize() const;
		DWORD				GetMobDamageMin() const;
		DWORD				GetMobDamageMax() const;
		WORD				GetMobAttackRange() const;
		DWORD				GetMobDropItemVnum() const;
		float				GetMobDamageMultiply() const;

		// NEWAI
		bool			IsBerserker() const;
		bool			IsBerserk() const;
		void			SetBerserk(bool mode);

		bool			IsStoneSkinner() const;

		bool			IsGodSpeeder() const;
		bool			IsGodSpeed() const;
		void			SetGodSpeed(bool mode);

		bool			IsDeathBlower() const;
		bool			IsDeathBlow() const;

		bool			IsReviver() const;
		bool			HasReviverInParty() const;
		bool			IsRevive() const;
		void			SetRevive(bool mode);
		// NEWAI END

		bool			IsRaceFlag(DWORD dwBit) const;
		bool			IsSummonMonster() const;
		DWORD			GetSummonVnum() const;

		DWORD			GetPolymorphItemVnum() const;
		DWORD			GetMonsterDrainSPPoint() const;

		void			MainCharacterPacket();	// ���� ����ĳ���Ͷ�� �����ش�.

		void			ComputePoints();
		void			ComputeBattlePoints();
#ifdef ENABLE_LONG_LONG
		void			PointChange(BYTE type, long long amount, bool bAmount = false, bool bBroadcast = false
#ifdef __ENABLE_BLOCK_EXP__
, bool bForceExp = false
#endif
		);
#else
		void			PointChange(BYTE type, int amount, bool bAmount = false, bool bBroadcast = false
#ifdef __ENABLE_BLOCK_EXP__
, bool bForceExp = false
#endif
		);
#endif
		void			PointsPacket();
		void			ApplyPoint(BYTE bApplyType, int iVal);
#ifdef __NEWPET_SYSTEM__
		void			SendPetLevelUpEffect(int vid, int type, int value, int amount);
#endif		
		void			CheckMaximumPoints();	// HP, SP ���� ���� ���� �ִ밪 ���� ������ �˻��ϰ� ���ٸ� �����.

		bool			Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);

		void			Sitdown(int is_ground);
		void			Standup();

#ifdef ENABLE_ANCIENT_PYRAMID
		void			SetRotation(float fRot, bool bForce = false);
#else
		void			SetRotation(float fRot);
#endif
		void			SetRotationToXY(long x, long y);
		float			GetRotation() const	{ return m_pointsInstant.fRot; }

		void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void			Motion(BYTE motion, LPCHARACTER victim = NULL);

		void			ChatPacket(BYTE type, const char *format, ...);
		void			SendGreetMessage();

		void			ResetPoint(int iLv);

		void			SetBlockMode(BYTE bFlag);
		void			SetBlockModeForce(BYTE bFlag);
		bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag)?true:false; }

		bool			IsPolymorphed() const		{ return m_dwPolymorphRace>0; }
		bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; } // ���� ������ �����ϴ� ��������.
		void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
		int				GetPolymorphPower() const;

		// FISING
		void			fishing();
		void			fishing_take();
		// END_OF_FISHING

		// MINING
		void			mining(LPCHARACTER chLoad);
		void			mining_cancel();
		void			mining_take();
		// END_OF_MINING

		void			ResetPlayTime(DWORD dwTimeRemain = 0);

		void			CreateFly(BYTE bType, LPCHARACTER pkVictim);

		void			ResetChatCounter();
		BYTE			IncreaseChatCounter();
		BYTE			GetChatCounter() const;

		void			ResetMountCounter();
		BYTE			IncreaseMountCounter();
		BYTE			GetMountCounter() const;

	protected:
		DWORD			m_dwPolymorphRace;
		bool			m_bPolyMaintainStat;
		DWORD			m_dwLoginPlayTime;
		DWORD			m_dwPlayerID;
		VID				m_vid;
		std::string		m_stName;
#ifdef __NEWPET_SYSTEM__
		BYTE			m_stImmortalSt;
#endif
		BYTE			m_bCharType;
#ifdef __NEWPET_SYSTEM__
		DWORD			m_newpetskillcd[4];
#endif
		CHARACTER_POINT		m_points;
		CHARACTER_POINT_INSTANT	m_pointsInstant;

		int				m_iMoveCount;
		DWORD			m_dwPlayStartTime;
		BYTE			m_bAddChrState;
		bool			m_bSkipSave;
		std::string		m_stMobile;
		char			m_szMobileAuth[5];
		BYTE			m_bChatCounter;
#ifdef DROP_WIKI
		ITEMDROP_MAP	m_map_drop_item;
#endif

		BYTE			m_bMountCounter;

		// End of Basic Points

		//////////////////////////////////////////////////////////////////////////////////
		// Move & Synchronize Positions
		//////////////////////////////////////////////////////////////////////////////////
	public:
		long	SetInvincible(bool arg);
		bool	GetInvincible();
		long	IncreaseMobHP(long lArg);
		long	IncreaseMobRigHP(long lArg);

	private:
		bool	isInvincible;

	public:
		bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
		bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
		bool			IsWalking() const			{ return m_bNowWalking || GetStamina()<=0; }
		void			SetWalking(bool bWalkFlag)	{ m_bWalking=bWalkFlag; }
		void			SetNowWalking(bool bWalkFlag);
		void			ResetWalking()			{ SetNowWalking(m_bWalking); }

		bool			Goto(long x, long y);	// �ٷ� �̵� ��Ű�� �ʰ� ��ǥ ��ġ�� BLENDING ��Ų��.
		void			Stop();

		bool			CanMove() const;		// �̵��� �� �ִ°�?

		void			SyncPacket();
		bool			Sync(long x, long y);	// ���� �� �޼ҵ�� �̵� �Ѵ� (�� �� ���ǿ� ���� �̵� �Ұ��� ����)
		bool			Move(long x, long y);	// ������ �˻��ϰ� Sync �޼ҵ带 ���� �̵� �Ѵ�.
		void			OnMove(bool bIsAttack = false);	// �����϶� �Ҹ���. Move() �޼ҵ� �̿ܿ����� �Ҹ� �� �ִ�.
		DWORD			GetMotionMode() const;
		float			GetMoveMotionSpeed() const;
		float			GetMoveSpeed() const;
		void			CalculateMoveDuration();
		void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
		DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
		DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
		DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }

		void			SetLastAttacked(DWORD time);	// ���������� ���ݹ��� �ð� �� ��ġ�� ������

		bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool			IsSyncOwner(LPCHARACTER ch) const;

		bool			WarpSet(long x, long y, long lRealMapIndex = 0);
		
		
#ifdef ENABLE_CHANNEL_SWITCH_SYSTEM
		bool			SwitchChannel(long newAddr, WORD newPort);
		bool			StartChannelSwitch(long newAddr, WORD newPort);
#endif
		
		void			SetWarpLocation(long lMapIndex, long x, long y);
		void			WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool			WarpToPID(DWORD dwPID);

		void			SaveExitLocation();
		void			ExitToSavedLocation();

		void			StartStaminaConsume();
		void			StopStaminaConsume();
		bool			IsStaminaConsume() const;
		bool			IsStaminaHalfConsume() const;

		void			ResetStopTime();
		DWORD			GetStopTime() const;

	protected:
		void			ClearSync();

		float			m_fSyncTime;
		LPCHARACTER		m_pkChrSyncOwner;
		CHARACTER_LIST	m_kLst_pkChrSyncOwned;	// ���� SyncOwner�� �ڵ�

		PIXEL_POSITION	m_posDest;
		PIXEL_POSITION	m_posStart;
		PIXEL_POSITION	m_posWarp;
		long			m_lWarpMapIndex;

		PIXEL_POSITION	m_posExit;
		long			m_lExitMapIndex;

		DWORD			m_dwMoveStartTime;
		DWORD			m_dwMoveDuration;

		DWORD			m_dwLastMoveTime;
		DWORD			m_dwLastAttackTime;
		DWORD			m_dwWalkStartTime;
		DWORD			m_dwStopTime;

		bool			m_bWalking;
		bool			m_bNowWalking;
		bool			m_bStaminaConsume;
		// End

		// Quickslot ����
	public:
		void			SyncQuickslot(BYTE bType, BYTE bOldPos, BYTE bNewPos);
		bool			GetQuickslot(BYTE pos, TQuickslot ** ppSlot);
		bool			SetQuickslot(BYTE pos, TQuickslot & rSlot);
		bool			DelQuickslot(BYTE pos);
		bool			SwapQuickslot(BYTE a, BYTE b);
		void			ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos);
#ifdef __ENABLE_NEW_OFFLINESHOP__
	public:
		offlineshop::CShop*		GetOfflineShop() {return m_pkOfflineShop;}
		void					SetOfflineShop(offlineshop::CShop* pkShop) {m_pkOfflineShop = pkShop;}
		
		offlineshop::CShop*		GetOfflineShopGuest() const {return m_pkOfflineShopGuest;}
		void					SetOfflineShopGuest(offlineshop::CShop* pkShop) {m_pkOfflineShopGuest = pkShop;}
		
		offlineshop::CShopSafebox*
								GetShopSafebox() {return m_pkShopSafebox;}
		void					SetShopSafebox(offlineshop::CShopSafebox* pk);

		void					SetAuction(offlineshop::CAuction* pk) {m_pkAuction = pk;}
		void					SetAuctionGuest(offlineshop::CAuction* pk) {m_pkAuctionGuest = pk;}

		offlineshop::CAuction*	GetAuction() {return m_pkAuction;}
		offlineshop::CAuction*	GetAuctionGuest() const {return m_pkAuctionGuest;}


		//offlineshop-updated 05/08/19
		void					SetLookingOfflineshopOfferList(bool is) { m_bIsLookingOfflineshopOfferList = is; }
		bool					IsLookingOfflineshopOfferList() { return m_bIsLookingOfflineshopOfferList; }
		int						GetOfflineShopUseTime() const { return m_iOfflineShopUseTime; }
		void					SetOfflineShopUseTime() { m_iOfflineShopUseTime = thecore_pulse(); }

	private:
		offlineshop::CShop*			m_pkOfflineShop;
		offlineshop::CShop*			m_pkOfflineShopGuest;
		offlineshop::CShopSafebox*	m_pkShopSafebox;
		offlineshop::CAuction*		m_pkAuction;
		offlineshop::CAuction*		m_pkAuctionGuest;

		//offlineshop-updated 05/08/19
		bool	m_bIsLookingOfflineshopOfferList;
		// patch with warp check
		int		m_iOfflineShopUseTime = 0;
#endif

	protected:
		TQuickslot		m_quickslot[QUICKSLOT_MAX_NUM];

		////////////////////////////////////////////////////////////////////////////////////////
		// Affect
	public:
		void			StartAffectEvent();
		void			ClearAffect(bool bSave=false);
		void			ComputeAffect(CAffect * pkAff, bool bAdd);
		bool			AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false);
		void			RefreshAffect();
		bool			RemoveAffect(DWORD dwType);
		bool			IsAffectFlag(DWORD dwAff) const;

		bool			UpdateAffect();	// called from EVENT
		int				ProcessAffect();

		void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void			SaveAffect();

		// Affect loading�� ���� �����ΰ�?
		bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }

		bool			IsGoodAffect(BYTE bAffectType) const;

		void			RemoveGoodAffect();
		void			RemoveBadAffect();

		CAffect *		FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool			RemoveAffect(CAffect * pkAff);

	protected:
		bool			m_bIsLoadedAffect;
		TAffectFlag		m_afAffectFlag;
		std::list<CAffect *>	m_list_pkAffect;

#ifdef ENABLE_SKILLS_BUFF_ALTERNATIVE
	public:
		void						ClearAffectSkills();
		void						SaveAffectSkills(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost);
		void						LoadAffectSkills();

	protected:
		std::vector<TAffectSkills>	m_list_pkAffectSkills;
#endif

	public:
		// PARTY_JOIN_BUG_FIX
		void			SetParty(LPPARTY pkParty);
		LPPARTY			GetParty() const	{ return m_pkParty; }

		bool			RequestToParty(LPCHARACTER leader);
		void			DenyToParty(LPCHARACTER member);
		void			AcceptToParty(LPCHARACTER member);

		/// �ڽ��� ��Ƽ�� �ٸ� character �� �ʴ��Ѵ�.
		/**
		 * @param	pchInvitee �ʴ��� ��� character. ��Ƽ�� ���� ������ �����̾�� �Ѵ�.
		 *
		 * ���� character �� ���°� ��Ƽ�� �ʴ��ϰ� �ʴ���� �� �ִ� ���°� �ƴ϶�� �ʴ��ϴ� ĳ���Ϳ��� �ش��ϴ� ä�� �޼����� �����Ѵ�.
		 */
		void			PartyInvite(LPCHARACTER pchInvitee);

		/// �ʴ��ߴ� character �� ������ ó���Ѵ�.
		/**
		 * @param	pchInvitee ��Ƽ�� ������ character. ��Ƽ�� ���������� �����̾�� �Ѵ�.
		 *
		 * pchInvitee �� ��Ƽ�� ������ �� �ִ� ��Ȳ�� �ƴ϶�� �ش��ϴ� ä�� �޼����� �����Ѵ�.
		 */
		void			PartyInviteAccept(LPCHARACTER pchInvitee);

		/// �ʴ��ߴ� character �� �ʴ� �źθ� ó���Ѵ�.
		/**
		 * @param [in]	dwPID �ʴ� �ߴ� character �� PID
		 */
		void			PartyInviteDeny(DWORD dwPID);

		bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int				GetLeadershipSkillLevel() const;

		bool			CanSummon(int iLeaderShip);

		void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:

		/// ��Ƽ�� �����Ѵ�.
		/**
		 * @param	pkLeader ������ ��Ƽ�� ����
		 */
		void			PartyJoin(LPCHARACTER pkLeader);

		/**
		 * ��Ƽ ������ �� �� ���� ����� �����ڵ�.
		 * Error code �� �ð��� �������ΰ��� ���� ���氡����(mutable) type �� ����(static) type ���� ������.
		 * Error code �� ���� PERR_SEPARATOR ���� ������ ���氡���� type �̰� ������ ���� type �̴�.
		 */
		enum PartyJoinErrCode {
			PERR_NONE		= 0,	///< ó������
			PERR_SERVER,			///< ���������� ��Ƽ���� ó�� �Ұ�
			PERR_DUNGEON,			///< ĳ���Ͱ� ������ ����
			PERR_OBSERVER,			///< ���������
			PERR_LVBOUNDARY,		///< ��� ĳ���Ϳ� �������̰� ��
			PERR_LOWLEVEL,			///< �����Ƽ�� �ְ��������� 30���� ����
			PERR_HILEVEL,			///< �����Ƽ�� ������������ 30���� ����
			PERR_ALREADYJOIN,		///< ��Ƽ���� ��� ĳ���Ͱ� �̹� ��Ƽ��
			PERR_PARTYISFULL,		///< ��Ƽ�ο� ���� �ʰ�
			PERR_SEPARATOR,			///< Error type separator.
			PERR_DIFFEMPIRE,		///< ��� ĳ���Ϳ� �ٸ� ������
			PERR_MAX				///< Error code �ְ�ġ. �� �տ� Error code �� �߰��Ѵ�.
		};

		/// ��Ƽ �����̳� �Ἲ ������ ������ �˻��Ѵ�.
		/**
		 * @param 	pchLeader ��Ƽ�� leader �̰ų� �ʴ��� character
		 * @param	pchGuest �ʴ�޴� character
		 * @return	��� PartyJoinErrCode �� ��ȯ�� �� �ִ�.
		 */
		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		/// ��Ƽ �����̳� �Ἲ ������ ������ ������ �˻��Ѵ�.
		/**
		 * @param 	pchLeader ��Ƽ�� leader �̰ų� �ʴ��� character
		 * @param	pchGuest �ʴ�޴� character
		 * @return	mutable type �� code �� ��ȯ�Ѵ�.
		 */
		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		LPPARTY			m_pkParty;
		DWORD			m_dwLastDeadTime;
		LPEVENT			m_pkPartyRequestEvent;

		/**
		 * ��Ƽ��û Event map.
		 * key: �ʴ���� ĳ������ PID
		 * value: event�� pointer
		 *
		 * �ʴ��� ĳ���͵鿡 ���� event map.
		 */
		typedef std::map< DWORD, LPEVENT >	EventMap;
		EventMap		m_PartyInviteEventMap;

		// END_OF_PARTY_JOIN_BUG_FIX

		////////////////////////////////////////////////////////////////////////////////////////
		// Dungeon
	public:
		void			SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
		LPDUNGEON		GetDungeonForce() const;
	protected:
		LPDUNGEON	m_pkDungeon;
		int			m_iEventAttr;

		////////////////////////////////////////////////////////////////////////////////////////
		// Guild
	public:
		void			SetGuild(CGuild * pGuild);
		CGuild*			GetGuild() const	{ return m_pGuild; }

		void			SetWarMap(CWarMap* pWarMap);
		CWarMap*		GetWarMap() const	{ return m_pWarMap; }

	protected:
		CGuild *		m_pGuild;
		DWORD			m_dwUnderGuildWarInfoMessageTime;
		CWarMap *		m_pWarMap;

		////////////////////////////////////////////////////////////////////////////////////////
		// Item related
	public:
		bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false); // ������ ���� ������ �� �� �ִ°�?

		bool			IsItemLoaded() const	{ return m_bItemLoaded; }
		void			SetItemLoaded()	{ m_bItemLoaded = true; }

		void			ClearItem();

#ifdef ENABLE_SORT_INVEN	
		void			EditMyInven();
		void			EditMyExtraInven();
#endif
		
#ifdef __HIGHLIGHT_SYSTEM__
		void			SetItem(TItemPos Cell, LPITEM item, bool isHighLight = false);
#else
		void			SetItem(TItemPos Cell, LPITEM item);
#endif
		LPITEM			GetItem(TItemPos Cell) const;
		LPITEM			GetInventoryItem(WORD wCell) const;
#ifdef ENABLE_EXTRA_INVENTORY
		LPITEM			GetExtraInventoryItem(WORD wCell) const;
		void			SetNextSortExtraInventoryPulse(int pulse) { m_sortExtraInventoryPulse = pulse; }
		int				GetSortExtraInventoryPulse() { return m_sortExtraInventoryPulse; }
		int				m_sortExtraInventoryPulse;
#endif
#ifdef ENABLE_LOCKED_EXTRA_INVENTORY
		int		ExtraInventoryMaxSlots(int iArg1, bool bAuto = false) const;
		void	UnlockExtraInventory(BYTE category);
#endif
		bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;

		void			SetWear(BYTE bCell, LPITEM item);
		LPITEM			GetWear(BYTE bCell) const;

		// MYSHOP_PRICE_LIST
		void			UseSilkBotary(void); 		/// ��� ������ �������� ���

		/// DB ĳ�÷� ���� �޾ƿ� �������� ����Ʈ�� �������� �����ϰ� ������ ������ ����� ó���Ѵ�.
		/**
		 * @param [in] p	�������� ����Ʈ ��Ŷ
		 *
		 * ������ �� ó�� ��� ������ ������ ��� �� UseSilkBotary ���� DB ĳ�÷� �������� ����Ʈ�� ��û�ϰ�
		 * ������� ������ �� �Լ����� ���� ��ܺ����� ����� ó���Ѵ�.
		 */
		void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
		// END_OF_MYSHOP_PRICE_LIST

		bool			UseItemEx(LPITEM item, TItemPos DestCell);
		bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

		// ADD_REFINE_BUILDING
		bool			IsRefineThroughGuild() const;
		CGuild *		GetRefineGuild() const;
		int				ComputeRefineFee(int iCost, int iMultiply = 5) const;
		void			PayRefineFee(int iTotalMoney);
		void			SetRefineNPC(LPCHARACTER ch);
		// END_OF_ADD_REFINE_BUILDING

		bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);
		bool			DropItem(TItemPos Cell,
#ifdef ENABLE_NEW_STACK_LIMIT
		int 
#else
		BYTE 
#endif
		bCount = 0);
		bool			DestroyItem(TItemPos Cell);
		void			ItemDivision(TItemPos Cell);
		bool			GiveRecallItem(LPITEM item);
		void			ProcessRecallItem(LPITEM item);

		//	void			PotionPacket(int iPotionType);
		void			EffectPacket(int enumEffectType);
		void			SpecificEffectPacket(const char filename[128]);

		// ADD_MONSTER_REFINE
		bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
		// END_OF_ADD_MONSTER_REFINE

		bool			DoRefineWithScroll(LPITEM item);
		bool			RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell = -1);

		void			SetRefineMode(int iAdditionalCell = -1);
		void			ClearRefineMode();

		bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool			CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void			ReceiveItem(LPCHARACTER from, LPITEM item);
		bool			GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums,
							std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);

		bool			MoveItem(TItemPos pos, TItemPos change_pos,
#ifdef ENABLE_NEW_STACK_LIMIT
		int 
#else
		BYTE 
#endif
		count);
		bool			PickupItem(DWORD vid);
		bool			EquipItem(LPITEM item, int iCandidateCell = -1);
		bool			UnequipItem(LPITEM item);

		// ���� item�� ������ �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
		bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		// �������� item�� ���� �� �ִ� �� Ȯ���ϰ�, �Ұ��� �ϴٸ� ĳ���Ϳ��� ������ �˷��ִ� �Լ�
		bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool			SwapItem(BYTE bCell, BYTE bDestCell);
		
		LPITEM			AutoGiveItem(DWORD dwItemVnum,
#ifdef ENABLE_NEW_STACK_LIMIT
		int 
#else
		BYTE 
#endif
		bCount=1, int iRarePct = -1, bool bMsg = true
#ifdef __HIGHLIGHT_SYSTEM__
		, bool isHighLight = true
#endif
		);
		void			AutoGiveItem(LPITEM item, bool longOwnerShip = false
#ifdef __HIGHLIGHT_SYSTEM__
		, bool isHighLight = true
#endif
		);
#ifdef ENABLE_DS_REFINE_ALL
		bool	AutoGiveDS(LPITEM item, bool longOwnerShip = false);
#endif
		bool			CanTakeInventoryItem(LPITEM item, TItemPos* pos);
		
#ifdef ENABLE_EXTRA_INVENTORY
		int				GetEmptyExtraInventory(LPITEM pItem) const;
		int				GetEmptyExtraInventory(BYTE size, BYTE category) const; // needed for offline shop
#endif
		
		int				GetEmptyInventory(BYTE size) const;
		int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
		void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

		int				CountEmptyInventory() const;

		int 			CountSpecifyItemRenewal(DWORD vnum) const;
		
		int				CountSpecifyItem(DWORD vnum) const;
		void			RemoveSpecifyItem(DWORD vnum, int count = 1, bool cuberenewal = false);
		LPITEM			FindSpecifyItem(DWORD vnum
#ifdef ENABLE_EXTRA_INVENTORY
		, bool reinforce = false
#endif
		) const;
		LPITEM			FindItemByID(DWORD id) const;

		int				CountSpecifyTypeItem(BYTE type) const;
		void			RemoveSpecifyTypeItem(BYTE type, int count = 1);

		bool			IsEquipUniqueItem(DWORD dwItemVnum) const;

		// CHECK_UNIQUE_GROUP
		bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
		// END_OF_CHECK_UNIQUE_GROUP

		void			SendEquipment(LPCHARACTER ch);
		// End of Item

	protected:

		/// �� �����ۿ� ���� ���������� �����Ѵ�.
		/**
		 * @param [in]	dwItemVnum ������ vnum
		 * @param [in]	dwItemPrice ������ ����
		 */
#ifdef ENABLE_LONG_LONG
		void			SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice);
#else
		void			SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice);
#endif
		bool			m_bNoOpenedShop;	///< �̹� ���� �� ���λ����� �� ���� �ִ����� ����(������ ���� ���ٸ� true)

		bool			m_bItemLoaded;
		int				m_iRefineAdditionalCell;
		bool			m_bUnderRefine;
		DWORD			m_dwRefineNPCVID;

	public:
		////////////////////////////////////////////////////////////////////////////////////////
		// Money related
#ifdef ENABLE_LONG_LONG
		long long				GetGold() const { return m_points.gold; }
		void			SetGold(long long gold) { m_points.gold = gold; }
#else
		INT				GetGold() const		{ return m_points.gold;	}
		void			SetGold(INT gold)	{ m_points.gold = gold;	}
#endif
#ifdef __ENABLE_EXTEND_INVEN_SYSTEM__
		INT				Inven_Point() const			{ return m_points.envanter; }
		INT				Inventory_Size() const			{ return 90 + (5*Inven_Point()); }
		void			Set_Inventory_Point(int black)	{ m_points.envanter = black; }
		bool			Update_Inven();
#endif
		bool			DropGold(INT gold);
#ifdef ENABLE_LONG_LONG
		long long				GetAllowedGold() const;
		void			GiveGold(long long iAmount);	// ��Ƽ�� ������ ��Ƽ �й�, �α� ���� ó��
#else
		INT				GetAllowedGold() const;
		void			GiveGold(INT iAmount);	// ��Ƽ�� ������ ��Ƽ �й�, �α� ���� ó��
#endif
#ifdef ENABLE_PVP_ADVANCED
		int				GetDuel(const char* type) const;
		void			SetDuel(const char* type, int value);
#endif
#ifdef ENABLE_GAYA_SYSTEM
		INT				GetGaya() const { return m_points.gaya; }
		void			SetGaya(INT gaya) { m_points.gaya = gaya; }
#endif

		// End of Money

		////////////////////////////////////////////////////////////////////////////////////////
		// Shop related
	public:
		void			SetShop(LPSHOP pkShop);
		LPSHOP			GetShop() const { return m_pkShop; }
		void			ShopPacket(BYTE bSubHeader);

		void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}

		void			OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount
#ifdef KASMIR_PAKET_SYSTEM
		, DWORD KasmirNpc, BYTE KasmirBaslik
#endif
		);
#ifdef KASMIR_PAKET_SYSTEM
		void			UseSilkBotaryKasmir(void);
#endif
		LPSHOP			GetMyShop() const { return m_pkMyShop; }
		void			CloseMyShop();
#ifdef ENABLE_PVP_ADVANCED
		void			DestroyPvP();
#endif

	protected:

		LPSHOP			m_pkShop;
		LPSHOP			m_pkMyShop;
#ifdef KASMIR_PAKET_SYSTEM
		BYTE			m_bKasmirPaketBaslik;
		bool			m_bKasmirPaketDurum;
#endif
		std::string		m_stShopSign;
		LPCHARACTER		m_pkChrShopOwner;
		// End of shop

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	public:
		void			SetSkillColor(DWORD* dwSkillColor);
		DWORD* GetSkillColor() { return m_dwSkillColor[0]; }

	protected:
		DWORD			m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

		////////////////////////////////////////////////////////////////////////////////////////
		// Exchange related
	public:
		bool			ExchangeStart(LPCHARACTER victim);
		void			SetExchange(CExchange * pkExchange);
		CExchange *		GetExchange() const	{ return m_pkExchange;	}

	protected:
		CExchange *		m_pkExchange;
		// End of Exchange

#ifdef __DUNGEON_INFO_SYSTEM__
	public:
		void				SetQuestDamage(int race, int dmg);
		int					GetQuestDamage(int race);

	private:
		std::map<int, int>	dungeonDamage;
#endif

		////////////////////////////////////////////////////////////////////////////////////////
		// Battle
	public:
		struct TBattleInfo
		{
			int iTotalDamage;
			int iAggro;

			TBattleInfo(int iTot, int iAggr)
				: iTotalDamage(iTot), iAggro(iAggr)
				{}
		};
		typedef std::map<VID, TBattleInfo>	TDamageMap;

		typedef struct SAttackLog
		{
			DWORD	dwVID;
			DWORD	dwTime;
		} AttackLog;


#ifdef __ENABLE_BERAN_ADDONS__
		bool				IsBeranMap(int lMapIndex);
#endif


		bool				Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool				__Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void				DeathPenalty(BYTE bExpLossPercent);
		void				ReviveInvisible(int iDur);

		bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool				CanFight() const;

		bool				CanBeginFight() const;
		void				BeginFight(LPCHARACTER pkVictim); // pkVictimr�� �ο�� �����Ѵ�. (��������, ������ �� �ֳ� üũ�Ϸ��� CanBeginFight�� ���)

		bool				CounterAttack(LPCHARACTER pkChr); // �ݰ��ϱ� (���͸� ���)

		bool				IsStun() const;
		void				Stun();
		bool				IsDead() const;
		void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=false);
#ifdef __NEWPET_SYSTEM__
		void				SetImmortal(int st) { m_stImmortalSt = st; };
		bool				IsImmortal() { return 1 == m_stImmortalSt; };
		void				SetNewPetSkillCD(int s, DWORD time) { m_newpetskillcd[s] = time; };
		DWORD				GetNewPetSkillCD(int s) { return m_newpetskillcd[s]; };
#endif
		void				Reward(bool bItemDrop);
		void				RewardGold(LPCHARACTER pkAttacker);

		bool				Shoot(BYTE bType);
		void				FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

		void				ForgetMyAttacker();
		void				AggregateMonster();
		void				AttractRanger();
		void				PullMonster();

		int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void				UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

		void				AttackedByPoison(LPCHARACTER pkAttacker);
		void				RemovePoison();
#ifdef ENABLE_WOLFMAN_CHARACTER
		void				AttackedByBleeding(LPCHARACTER pkAttacker);
		void				RemoveBleeding();
#endif
		void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void				RemoveFire();

		void				UpdateAlignment(int iAmount);
		int					GetAlignment() const;

		//����ġ ���
		int					GetRealAlignment() const;
		void				ShowAlignment(bool bShow);

		void				SetKillerMode(bool bOn);
		bool				IsKillerMode() const;
		void				UpdateKillerMode();

		BYTE				GetPKMode() const;
		void				SetPKMode(BYTE bPKMode);

		void				ItemDropPenalty(LPCHARACTER pkKiller);

		void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

		//
		// HACK
		//
	public:
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;

		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;

		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);

		BYTE GetComboIndex() const;

		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
		void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

		DWORD				m_dwFlyTargetID;
		std::vector<DWORD>	m_vec_dwFlyTargets;
		TDamageMap			m_map_kDamage;	// � ĳ���Ͱ� ������ �󸶸�ŭ�� �������� �־��°�?
//		AttackLog			m_kAttackLog;
		DWORD				m_dwKillerPID;

		int					m_iAlignment;		// Lawful/Chaotic value -200000 ~ 200000
		int					m_iRealAlignment;
		int					m_iKillerModePulse;
		BYTE				m_bPKMode;

		// Aggro
		DWORD				m_dwLastVictimSetTime;
		int					m_iMaxAggro;
		// End of Battle

		// Stone
	public:
		void				SetStone(LPCHARACTER pkChrStone);
		void				ClearStone();
		void				DetermineDropMetinStone();
		DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }
		void				DetermineDropMetinStofa();
		DWORD				GetDropMetinStofaVnum() const { return m_dwDropMetinStofa; }
		BYTE				GetDropMetinStofaPct() const { return m_bDropMetinStofaPct; }
		void				DetermineDropMetinSacca();
		DWORD				GetDropMetinSaccaVnum() const { return m_dwDropMetinSacca; }
		BYTE				GetDropMetinSaccaPct() const { return m_bDropMetinSaccaPct; }

	protected:
		LPCHARACTER			m_pkChrStone;		// ���� ������ ��
		CHARACTER_SET		m_set_pkChrSpawnedBy;	// ���� ������ ���
		DWORD				m_dwDropMetinStone;
		BYTE				m_bDropMetinStonePct;
		DWORD				m_dwDropMetinStofa;
		BYTE				m_bDropMetinStofaPct;
		DWORD				m_dwDropMetinSacca;
		BYTE				m_bDropMetinSaccaPct;

#ifdef ENABLE_RANKING
	protected:
		long long	m_lRankPoints[RANKING_MAX_CATEGORIES];

	public:
		long long	GetRankPoints(int iArg);
		void		SetRankPoints(int iArg, long long lPoint);
		void		RankingSubcategory(int iArg);
#endif
#ifdef ENABLE_ATTR_COSTUMES
	public:
		void	SetAttrDialogRemove(int iArg)	{ attrdialog_remove = iArg; }
		int		GetAttrDialogRemove() const { return attrdialog_remove; }

	protected:
		int	attrdialog_remove;
#endif
#ifdef ENABLE_NEW_PET_EDITS
	public:
		void	SetPetEnchant(int iArg)	{ petenchant = iArg; }
		int		GetPetEnchant() const { return petenchant; }

	protected:
		int	petenchant;
#endif
	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,

			// ADD_GRANDMASTER_SKILL
			SKILL_UP_BY_QUEST,
			// END_OF_ADD_GRANDMASTER_SKILL
		};

#ifdef ENABLE_NEW_PASSIVE_SKILLS
		bool				SkillCanUp(DWORD dwVnum, bool book = false);
#endif

		void				SkillLevelPacket();
		void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool				SkillLevelDown(DWORD dwVnum);
		// ADD_GRANDMASTER_SKILL
		bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
		void				ResetSkill();
		void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
		int					GetUsedSkillMasterType(DWORD dwVnum);

		bool				IsLearnableSkill(DWORD dwSkillVnum) const;
		// END_OF_ADD_GRANDMASTER_SKILL

		bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool				CanUseSkill(DWORD dwSkillVnum) const;
		bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int					GetSkillLevel(DWORD dwVnum) const;
		int					GetSkillMasterType(DWORD dwVnum) const;
		int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

		time_t				GetSkillNextReadTime(DWORD dwVnum) const;
		void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

		void				ComputePassiveSkill(DWORD dwVnum);
#ifdef ENABLE_NEW_GYEONGGONG_SKILL
		int					ComputeGyeongGongSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
		int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#ifdef GROUP_BUFF
		int					ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
		int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
		void				ComputeSkillPoints();

		void				SetSkillGroup(BYTE bSkillGroup);
		BYTE				GetSkillGroup() const		{ return m_points.skill_group; }

		int					ComputeCooltime(int time);

		void				GiveRandomSkillBook();

		void				DisableCooltime();
		bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool				LearnGrandMasterSkill(DWORD dwSkillVnum);

	private:
		bool				m_bDisableCooltime;
		DWORD				m_dwLastSkillTime;	///< ���������� skill �� �� �ð�(millisecond).
		// End of Skill

		// MOB_SKILL
	public:
		bool				HasMobSkill() const;
		size_t				CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool				CanUseMobSkill(unsigned int idx) const;
		bool				UseMobSkill(unsigned int idx);
		void				ResetMobSkillCooltime();
	protected:
		DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
		// END_OF_MOB_SKILL

		// for SKILL_MUYEONG
	public:
		void				StartMuyeongEvent();
		void				StopMuyeongEvent();
#ifdef ENABLE_NEW_GYEONGGONG_SKILL
		void				StartGyeongGongEvent();
		void				StopGyeongGongEvent();
#endif

	private:
		LPEVENT				m_pkMuyeongEvent;
#ifdef ENABLE_NEW_GYEONGGONG_SKILL
		LPEVENT				m_pkGyeongGongEvent;
#endif

		// for SKILL_CHAIN lighting
	public:
		int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
		void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int					GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int					m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

		// for SKILL_EUNHYUNG
	public:
		void				SetAffectedEunhyung();
		void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD				m_dwAffectedEunhyungLevel;

		//
		// Skill levels
		//
	protected:
		TPlayerSkill*					m_pSkillLevels;
		boost::unordered_map<BYTE, int>		m_SkillDamageBonus;
		std::map<int, TSkillUseInfo>	m_SkillUseInfo;

		////////////////////////////////////////////////////////////////////////////////////////
		// AI related
	public:
		void			AssignTriggers(const TMobTable * table);
		LPCHARACTER		GetVictim() const;	// ������ ��� ����
		void			SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER		GetProtege() const;	// ��ȣ�ؾ� �� ��� ����

		bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool			Return();
		bool			IsGuardNPC() const;
		bool			IsChangeAttackPosition(LPCHARACTER target) const;
		void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}

		bool			OnIdle();

		void			OnAttack(LPCHARACTER pkChrAttacker);
		void			OnClick(LPCHARACTER pkChrCauser);

		VID				m_kVIDVictim;

	protected:
		DWORD			m_dwLastChangeAttackPositionTime;
		CTrigger		m_triggerOnClick;
		// End of AI

		////////////////////////////////////////////////////////////////////////////////////////
		// Target
	protected:
		LPCHARACTER				m_pkChrTarget;		// �� Ÿ��
		CHARACTER_SET	m_set_pkChrTargetedBy;	// ���� Ÿ������ ������ �ִ� �����

	public:
		void				SetTarget(LPCHARACTER pkChrTarget);
#ifdef DROP_WIKI
		void				SendTargetDrop();
#endif
		void				BroadcastTargetPacket();
		void				ClearTarget();
		void				CheckTarget();
		LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

		////////////////////////////////////////////////////////////////////////////////////////
		// Safebox
	public:
		int					GetSafeboxSize() const;
		void				QuerySafeboxSize();
		void				SetSafeboxSize(int size);

		CSafebox *			GetSafebox() const;
		void				LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
		void				ChangeSafeboxSize(BYTE bSize);
		void				CloseSafebox();

		/// â�� ���� ��û
		/**
		 * @param [in]	pszPassword 1�� �̻� 6�� ������ â�� ��й�ȣ
		 *
		 * DB �� â�����⸦ ��û�Ѵ�.
		 * â���� �ߺ����� ���� ���ϸ�, �ֱ� â���� ���� �ð����� ���� 10�� �̳����� �� �� ���Ѵ�.
		 */
		void				ReqSafeboxLoad(const char* pszPassword);

		/// â�� ���� ��û�� ���
		/**
		 * ReqSafeboxLoad �� ȣ���ϰ� CloseSafebox ���� �ʾ��� �� �� �Լ��� ȣ���ϸ� â���� �� �� �ִ�.
		 * â�������� ��û�� DB �������� ���������� �޾��� ��� �� �Լ��� ����ؼ� ��û�� �� �� �ְ� ���ش�.
		 */
		void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

		void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int					GetMallLoadTime() const { return m_iMallLoadTime; }
#ifdef DROP_WIKI
		void				SetItemDropRecvTime(int t) { m_iSendItemDropTime = t; }
		int					GetItemDropRecvTime() const { return m_iSendItemDropTime; }
#endif

		CSafebox *			GetMall() const;
		void				LoadMall(int iItemCount, TPlayerItem * pItems);
		void				CloseMall();

		void				SetSafeboxOpenPosition();
		float				GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox *			m_pkSafebox;
		int					m_iSafeboxSize;
		int					m_iSafeboxLoadTime;
		bool				m_bOpeningSafebox;	///< â���� ���� ��û ���̰ų� �����ִ°� ����, true �� ��� �����û�̰ų� ��������.

		CSafebox *			m_pkMall;
		int					m_iMallLoadTime;
#ifdef DROP_WIKI
		int					m_iSendItemDropTime;
#endif

		PIXEL_POSITION		m_posSafeboxOpen;

		////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////
		// Mounting
	public:
		void				MountVnum(DWORD vnum);
		DWORD				GetMountVnum() const { return m_dwMountVnum; }
		DWORD				GetLastMountTime() const { return m_dwMountTime; }

		bool				CanUseHorseSkill();

		// Horse
		virtual	void		SetHorseLevel(int iLevel);

		virtual	bool		StartRiding();
		virtual	bool		StopRiding();

		virtual	DWORD		GetMyHorseVnum() const;

		virtual	void		HorseDie();
		virtual bool		ReviveHorse();

		virtual void		SendHorseInfo();
		virtual	void		ClearHorseInfo();

		void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

		LPCHARACTER			GetHorse() const			{ return m_chHorse; }	 // ���� ��ȯ���� ��
		LPCHARACTER			GetRider() const; // rider on horse
		void				SetRider(LPCHARACTER ch);

		bool				IsRiding() const;
#ifdef __ATTR_TRANSFER_SYSTEM__
	public:
		LPITEM*	GetAttrTransferItem() {return m_pointsInstant.pAttrTransferItems;}
		bool IsAttrTransferOpen () const	{return (m_pointsInstant.pAttrTransferNpc?true:false);}
		void SetAttrTransferNpc(LPCHARACTER npc)	{m_pointsInstant.pAttrTransferNpc = npc;}
		bool CanDoAttrTransfer() const;
#endif
#ifdef __PET_SYSTEM__
	public:
		CPetSystem*			GetPetSystem()				{ return m_petSystem; }

	protected:
		CPetSystem*			m_petSystem;

	public:
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	public:
		CMountSystem*		GetMountSystem() { return m_mountSystem; }

		void 				MountSummon(LPITEM mountItem);
		void 				MountUnsummon(LPITEM mountItem);
		void 				CheckMount();
		bool 				IsRidingMount();

	protected:
		CMountSystem*		m_mountSystem;
#endif

#ifdef __NEWPET_SYSTEM__
	public:
		CNewPetSystem*			GetNewPetSystem() { return m_newpetSystem; }

	protected:
		CNewPetSystem*			m_newpetSystem;

	public:
#endif
#ifdef ENABLE_COSTUME_PET
	public:
		void	UpdatePetSkin();
		DWORD	GetPetSkinVnum();
#endif
#ifdef ENABLE_COSTUME_MOUNT
	public:
		void	UpdateMountSkin();
		DWORD	GetMountSkinVnum();
#endif
	protected:
		LPCHARACTER			m_chHorse;
		LPCHARACTER			m_chRider;

		DWORD				m_dwMountVnum;
		DWORD				m_dwMountTime;

		BYTE				m_bSendHorseLevel;
		BYTE				m_bSendHorseHealthGrade;
		BYTE				m_bSendHorseStaminaGrade;

		////////////////////////////////////////////////////////////////////////////////////////
		// Detailed Log
	public:
		void				DetailLog() { m_bDetailLog = !m_bDetailLog; }
		void				ToggleMonsterLog();
		void				MonsterLog(const char* format, ...);
	private:
		bool				m_bDetailLog;
		bool				m_bMonsterLog;

		////////////////////////////////////////////////////////////////////////////////////////
		// Empire

	public:
		void 				SetEmpire(BYTE bEmpire);
		BYTE				GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE				m_bEmpire;

		////////////////////////////////////////////////////////////////////////////////////////
		// Regen
	public:
		void				SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION			m_posRegen;
		float				m_fRegenAngle;
		LPREGEN				m_pkRegen;
		size_t				regen_id_; // to help dungeon regen identification
		// End of Regen

		////////////////////////////////////////////////////////////////////////////////////////
		// Resists & Proofs
	public:
		bool				CannotMoveByAffect() const;	// Ư�� ȿ���� ���� ������ �� ���� �����ΰ�?
		bool				IsImmune(DWORD dwImmuneFlag);
		void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void				ApplyMobAttribute(const TMobTable* table);
		// End of Resists & Proofs

		////////////////////////////////////////////////////////////////////////////////////////
		// QUEST
		//
	public:
		void				SetQuestNPCID(DWORD vid);
		DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER			GetQuestNPC() const;

		void				SetQuestItemPtr(LPITEM item);
		void				ClearQuestItemPtr();
		LPITEM				GetQuestItemPtr() const;

		void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
		DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }

		int					GetQuestFlag(const std::string& flag) const;
		void				SetQuestFlag(const std::string& flag, int value);

		void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD				m_dwQuestNPCVID;
		DWORD				m_dwQuestByVnum;
		LPITEM				m_pQuestItem;

		// Events
	public:
		bool				StartStateMachine(int iPulse = 1);
		void				StopStateMachine();
		void				UpdateStateMachine(DWORD dwPulse);
		void				SetNextStatePulse(int iPulseNext);

		// ĳ���� �ν��Ͻ� ������Ʈ �Լ�. ������ �̻��� ��ӱ����� CFSM::Update �Լ��� ȣ���ϰų� UpdateStateMachine �Լ��� ����ߴµ�, ������ ������Ʈ �Լ� �߰���.
		void				UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD				m_dwNextStatePulse;

		// Marriage
	public:
		LPCHARACTER			GetMarryPartner() const;
		void				SetMarryPartner(LPCHARACTER ch);
		int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

		void				SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER			m_pkChrMarried;

		// Warp Character
	public:
		void				StartWarpNPCEvent();

	public:
		void				StartSaveEvent();
		void				StartRecoveryEvent();
		void				StartDestroyWhenIdleEvent();

		LPEVENT				m_pkDeadEvent;
		LPEVENT				m_pkStunEvent;
		LPEVENT				m_pkSaveEvent;
		LPEVENT				m_pkRecoveryEvent;
		LPEVENT				m_pkTimedEvent;
		LPEVENT				m_pkFishingEvent;
		LPEVENT				m_pkAffectEvent;
		LPEVENT				m_pkPoisonEvent;
#ifdef ENABLE_WOLFMAN_CHARACTER
		LPEVENT				m_pkBleedingEvent;
#endif
		LPEVENT				m_pkFireEvent;
		LPEVENT				m_pkWarpNPCEvent;
		//DELAYED_WARP
		//END_DELAYED_WARP

		// MINING
		LPEVENT				m_pkMiningEvent;
		// END_OF_MINING
		LPEVENT				m_pkWarpEvent;
		LPEVENT				m_pkDestroyWhenIdleEvent;
		LPEVENT				m_pkPetSystemUpdateEvent;
#ifdef __NEWPET_SYSTEM__
		LPEVENT				m_pkNewPetSystemUpdateEvent;
		LPEVENT				m_pkNewPetSystemExpireEvent;
#endif
		bool IsWarping() const { return m_pkWarpEvent ? true : false; }

		bool				m_bHasPoisoned;
#ifdef ENABLE_WOLFMAN_CHARACTER
		bool				m_bHasBled;
#endif

		const CMob *		m_pkMobData;
		CMobInstance *		m_pkMobInst;

		std::map<int, LPEVENT> m_mapMobSkillEvent;

		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

	public:
		int				GetPremiumRemainSeconds(BYTE bType) const;

	private:
		int				m_aiPremiumTimes[PREMIUM_MAX_NUM];

		// CHANGE_ITEM_ATTRIBUTES
		// static const DWORD		msc_dwDefaultChangeItemAttrCycle;	///< ����Ʈ ������ �Ӽ����� ���� �ֱ�
		static const char		msc_szLastChangeItemAttrFlag[];		///< �ֱ� ������ �Ӽ��� ������ �ð��� Quest Flag �̸�
		// static const char		msc_szChangeItemAttrCycleFlag[];		///< ������ �Ӽ����� ���� �ֱ��� Quest Flag �̸�
		// END_OF_CHANGE_ITEM_ATTRIBUTES

		// PC_BANG_ITEM_ADD
	private :
		bool m_isinPCBang;

	public :
		bool SetPCBang(bool flag) { m_isinPCBang = flag; return m_isinPCBang; }
		bool IsPCBang() const { return m_isinPCBang; }
		// END_PC_BANG_ITEM_ADD

		// NEW_HAIR_STYLE_ADD
	public :
		bool ItemProcess_Hair(LPITEM item, int iDestCell);
		// END_NEW_HAIR_STYLE_ADD

	public :
		void ClearSkill();
		void ClearSubSkill();

		// RESET_ONE_SKILL
		bool ResetOneSkill(DWORD dwVnum);
		// END_RESET_ONE_SKILL

	private :
		void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	// ARENA
	private :
		CArena *m_pArena;
		bool m_ArenaObserver;
		int m_nPotionLimit;

	public :
		void 	SetArena(CArena* pArena) { m_pArena = pArena; }
		void	SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

		CArena* GetArena() const { return m_pArena; }
		bool	GetArenaObserverMode() const { return m_ArenaObserver; }

		void	SetPotionLimit(int count) { m_nPotionLimit = count; }
		int		GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

		//PREVENT_TRADE_WINDOW
	public:
		bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
		void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

		int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }
		//END_PREVENT_TRADE_WINDOW
	private:
		bool	m_isOpenSafebox;

	public:
		int		GetSkillPowerByLevel(int level, bool bMob = false) const;

		//PREVENT_REFINE_HACK
		int		GetRefineTime() const { return m_iRefineTime; }
		void	SetRefineTime() { m_iRefineTime = thecore_pulse(); }
		int		m_iRefineTime;
		//END_PREVENT_REFINE_HACK

		//RESTRICT_USE_SEED_OR_MOONBOTTLE
		int 	GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
		void  	SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
		int 	m_iSeedTime;
		//END_RESTRICT_USE_SEED_OR_MOONBOTTLE

		//PREVENT_PORTAL_AFTER_EXCHANGE
		int		GetExchangeTime() const { return m_iExchangeTime; }
		void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int		m_iExchangeTime;
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

		int 	m_iMyShopTime;
		int		GetMyShopTime() const	{ return m_iMyShopTime; }
		void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

		// Hack ������ ���� üũ.
		bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

		void Say(const std::string & s);

	public:
		bool ItemProcess_Polymorph(LPITEM item);

		// by mhh
		LPITEM*	GetCubeItem() { return m_pointsInstant.pCubeItems; }
		bool IsCubeOpen () const	{ return (m_pointsInstant.pCubeNpc?true:false); }
		void SetCubeNpc(LPCHARACTER npc)	{ m_pointsInstant.pCubeNpc = npc; }
		bool CanDoCube() const;

	private:
		//�߱� ����
		//18�� �̸� ����
		//3�ð� : 50 % 5 �ð� 0%
		e_overtime m_eOverTime;

	public:
		bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
		void SetOverTime(e_overtime e) { m_eOverTime = e; }

	private:
		int		m_deposit_pulse;

	public:
		void	UpdateDepositPulse();
		bool	CanDeposit() const;

	private:
		void	__OpenPrivateShop(
#ifdef KASMIR_PAKET_SYSTEM
		bool bKasmir = false
#endif
		);

	public:
		struct AttackedLog
		{
			DWORD 	dwPID;
			DWORD	dwAttackedTime;

			AttackedLog() : dwPID(0), dwAttackedTime(0)
			{
			}
		};

		AttackLog	m_kAttackLog;
		AttackedLog m_AttackedLog;
		int			m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

	public:
		bool UnEquipSpecialRideUniqueItem ();

		bool CanWarp () const;

	private:
		DWORD m_dwLastGoldDropTime;
#ifdef ENABLE_NEWSTUFF
		DWORD m_dwLastBoxUseTime;
		DWORD m_dwLastBuySellTime;
	public:
		DWORD GetLastBuySellTime() const { return m_dwLastBuySellTime; }
		void SetLastBuySellTime(DWORD dwLastBuySellTime) { m_dwLastBuySellTime = dwLastBuySellTime; }
#endif
	public:
		void AutoRecoveryItemProcess (const EAffectTypes);
#ifdef ENABLE_RECALL
		void AutoRecallProcess();
#endif

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;
		// ���� : ��Ȱ�� �׽�Ʈ�� ���Ͽ�.
	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }
	private:
		bool cannot_dead;
#ifdef __PET_SYSTEM__
	private:
		bool m_bIsPet;
	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() { return m_bIsPet; }
#endif

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	private:
		bool m_bIsMount;
	public:
		void SetMount() { m_bIsMount = true; }
		bool IsMount() { return m_bIsMount; }
#endif

#ifdef __NEWPET_SYSTEM__
	private:
		bool m_bIsNewPet;
		int m_eggvid;
	public:
		void SetNewPet() { m_bIsNewPet = true; }
		bool IsNewPet() const { return m_bIsNewPet ? true : false; }
		void SetEggVid(int vid) { m_eggvid = vid; }
		int GetEggVid() { return m_eggvid; }

#endif

	//���� ������ ����.
	private:
		float m_fAttMul;
		float m_fDamMul;
	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	private:
		bool IsValidItemPosition(TItemPos Pos) const;

	public:
		//��ȥ��

		// ĳ������ affect, quest�� load �Ǳ� ���� DragonSoul_Initialize�� ȣ���ϸ� �ȵȴ�.
		// affect�� ���� �������� �ε�Ǿ� LoadAffect���� ȣ����.
		void	DragonSoul_Initialize();

		int		DragonSoul_GetActiveDeck() const;
		bool	DragonSoul_IsDeckActivated() const;
		bool	DragonSoul_ActivateDeck(int deck_idx);

		void	DragonSoul_DeactivateAll();
		// �ݵ�� ClearItem ���� �ҷ��� �Ѵ�.
		// �ֳ��ϸ�....
		// ��ȥ�� �ϳ� �ϳ��� deactivate�� ������ ���� active�� ��ȥ���� �ִ��� Ȯ���ϰ�,
		// active�� ��ȥ���� �ϳ��� ���ٸ�, ĳ������ ��ȥ�� affect��, Ȱ�� ���¸� �����Ѵ�.
		//
		// ������ ClearItem ��, ĳ���Ͱ� �����ϰ� �ִ� ��� �������� unequip�ϴ� �ٶ���,
		// ��ȥ�� Affect�� ���ŵǰ�, �ᱹ �α��� ��, ��ȥ���� Ȱ��ȭ���� �ʴ´�.
		// (Unequip�� ������ �α׾ƿ� ��������, �ƴ��� �� �� ����.)
		// ��ȥ���� deactivate��Ű�� ĳ������ ��ȥ�� �� Ȱ�� ���´� �ǵ帮�� �ʴ´�.
		void	DragonSoul_CleanUp();
		// ��ȥ�� ��ȭâ
	public:
		bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
		bool		DragonSoul_RefineWindow_Close();
		LPENTITY	DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
		bool		DragonSoul_RefineWindow_CanRefine();
#if defined(BL_OFFLINE_MESSAGE)
	protected:
		DWORD				dwLastOfflinePMTime;
	public:
		DWORD				GetLastOfflinePMTime() const { return dwLastOfflinePMTime; }
		void				SetLastOfflinePMTime() { dwLastOfflinePMTime = get_dword_time(); }
		void				SendOfflineMessage(const char* To, const char* Message);
		void				ReadOfflineMessages();
#endif
		//���� ���� ��� ��Ŷ �ӽ� ����
	private:
		unsigned int itemAward_vnum;
		char		 itemAward_cmd[20];
		//bool		 itemAward_flag;
	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
		//bool		 GetItemAward_flag() { return itemAward_flag; }
		void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }
		//void		 SetItemAward_flag(bool flag) { itemAward_flag = flag; }
#ifdef ENABLE_ANTI_CMD_FLOOD
	private:
		int m_dwCmdAntiFloodPulse;
		DWORD m_dwCmdAntiFloodCount;
	public:
		int GetCmdAntiFloodPulse(){return m_dwCmdAntiFloodPulse;}
		DWORD GetCmdAntiFloodCount(){return m_dwCmdAntiFloodCount;}
		DWORD IncreaseCmdAntiFloodCount(){return ++m_dwCmdAntiFloodCount;}
		void SetCmdAntiFloodPulse(int dwPulse){m_dwCmdAntiFloodPulse=dwPulse;}
		void SetCmdAntiFloodCount(DWORD dwCount){m_dwCmdAntiFloodCount=dwCount;}
#endif
	private:
		// SyncPosition�� �ǿ��Ͽ� Ÿ������ �̻��� ������ ������ �� ����ϱ� ���Ͽ�,
		// SyncPosition�� �Ͼ ���� ���.
		timeval		m_tvLastSyncTime;
		int			m_iSyncHackCount;
	public:
		void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
		void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int				GetSyncHackCount() { return m_iSyncHackCount; }

#ifdef ENABLE_ACCE_SYSTEM
	protected:
		bool	m_bAcceCombination, m_bAcceAbsorption;

	public:
		bool	isAcceOpened(bool bCombination) {return bCombination ? m_bAcceCombination : m_bAcceAbsorption;}
		void	OpenAcce(bool bCombination);
		void	CloseAcce();
		void	ClearAcceMaterials();
		bool	CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget);
		LPITEM*	GetAcceMaterials() {return m_pointsInstant.pAcceMaterials;}
		bool	AcceIsSameGrade(long lGrade);
		DWORD	GetAcceCombinePrice(long lGrade
#ifdef ENABLE_STOLE_COSTUME
		, bool isCostume
#endif
		);
		void	GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);
		BYTE	CheckEmptyMaterialSlot();
		void	AddAcceMaterial(TItemPos tPos, BYTE bPos);
		void	RemoveAcceMaterial(BYTE bPos);
		BYTE	CanRefineAcceMaterials();
		void	RefineAcceMaterials();
		bool	IsAcceOpen() const {return m_bAcceCombination || m_bAcceAbsorption;}
#endif



#ifdef __HIDE_COSTUME_SYSTEM__
public:
	void SetBodyCostumeHidden(bool hidden, bool pass = false);
	bool IsBodyCostumeHidden() const { return m_bHideBodyCostume; };

	void SetHairCostumeHidden(bool hidden, bool pass = false);
	bool IsHairCostumeHidden() const { return m_bHideHairCostume; };

#ifdef ENABLE_ACCE_SYSTEM
	void SetAcceCostumeHidden(bool hidden, bool pass = false);
	bool IsAcceCostumeHidden() const { return m_bHideAcceCostume; };
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	void SetWeaponCostumeHidden(bool hidden, bool pass = false);
	bool IsWeaponCostumeHidden() const { return m_bHideWeaponCostume; };
#endif

private:
	bool m_bHideBodyCostume;
	bool m_bHideHairCostume;
#ifdef ENABLE_ACCE_SYSTEM
	bool m_bHideAcceCostume;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	bool m_bHideWeaponCostume;
#endif
#endif



#ifdef ENABLE_GAYA_SYSTEM
	public:
		struct Gaya_Shop_Values
		{
			int		value_1;
			int		value_2;
			int 	value_3;
			int 	value_4;
			int 	value_5;
			int 	value_6;

			bool operator == (const Gaya_Shop_Values& b)
			{
				return (this->value_1 == b.value_1) && (this->value_2 == b.value_2) &&
					(this->value_3 == b.value_3) && (this->value_4 == b.value_4) &&
					(this->value_5 == b.value_5) && (this->value_6 == b.value_6);
			}
		};

		struct Gaya_Load_Values
		{
			DWORD	items;
			DWORD	gaya;
			DWORD	count;
			DWORD	glimmerstone;
			DWORD	gaya_expansion;
			DWORD	gaya_refresh;
			DWORD	glimmerstone_count;
			DWORD	gaya_expansion_count;
			DWORD	gaya_refresh_count;
			DWORD	grade_stone;
			DWORD	give_gaya;
			DWORD	prob_gaya;
			DWORD	cost_gaya_yang;
		};

		bool CheckItemsFull();
		void UpdateItemsGayaMarker0();
		void UpdateItemsGayaMarker();
		void InfoGayaMarker();
		void ClearGayaMarket();
		bool CheckSlotGayaMarket(int slot);
		void UpdateSlotGayaMarket(int slot);
		void BuyItemsGayaMarket(int slot);
		void RefreshItemsGayaMarket();
		void CraftGayaItems(int slot);
		void MarketGayaItems(int slot);
		void RefreshGayaItems();
		void LOAD_GAYA();
		int  GetGayaState(const std::string& state) const;
		void SetGayaState(const std::string& state, int szValue);
		void StartCheckTimeMarket();

	private:
		std::vector<Gaya_Shop_Values> info_items;
		std::vector<Gaya_Shop_Values> info_slots;
		std::vector<Gaya_Load_Values> load_gaya_items;
		Gaya_Load_Values	load_gaya_values;
		LPEVENT	GayaUpdateTime;
#endif
		
		

#ifdef ENABLE_SOUL_SYSTEM
	public:
		bool 		DoRefineItemSoul(LPITEM item);
		int 		GetSoulItemDamage(LPCHARACTER pkVictim, int iDamage, BYTE bSoulType);
#endif

#ifdef ENABLE_BATTLE_PASS
	public:
		void LoadBattlePass(DWORD dwCount, TPlayerBattlePassMission * data);
		DWORD GetMissionProgress(DWORD dwMissionID, DWORD dwBattlePassID);
		void UpdateMissionProgress(DWORD dwMissionID, DWORD dwBattlePassID, DWORD dwUpdateValue, DWORD dwTotalValue, bool isOverride = false);
		bool IsCompletedMission(BYTE bMissionType);
		bool IsLoadedBattlePass() const	{ return m_bIsLoadedBattlePass; }
		BYTE GetBattlePassId();
	
	private:
		bool m_bIsLoadedBattlePass;
		std::list<TPlayerBattlePassMission *> m_listBattlePass;

	public:
		int 			GetSecondsTillNextMonth();
		int 			GetBattlePassEndTime() { return (m_dwBattlePassEndTime - get_global_time()); };
	protected:
		DWORD			m_dwBattlePassEndTime;
		
#ifdef ENABLE_BATTLE_PASS_STAY_ONLINE		
	public:
		LPEVENT				m_pkStayOnlineEvent;
		void	CancelStayOnlineEvent();
		void 	LoadStayActiveBattlePass();
#endif
#endif

#ifdef ENABLE_WHISPER_ADMIN_SYSTEM
	std::string GetLang();
#endif
#ifdef ENABLE_RUNE_SYSTEM
	public:
		WORD	GetRuneEffect();
#endif
#ifdef TEXTS_IMPROVEMENT
	public:
		void ChatPacketNew(BYTE type, DWORD idx, const char * format, ...);
#endif
#ifdef ENABLE_NEW_FISHING_SYSTEM
	public:
		LPEVENT m_pkFishingNewEvent;

		void fishing_new_start();
		void fishing_new_stop();
		void fishing_new_catch();
		void fishing_new_catch_failed();
		void fishing_catch_decision(DWORD itemVnum);
		void SetFishCatch(int i) { m_bFishCatch = i; }
		BYTE GetFishCatch() { return m_bFishCatch; }
		void SetLastCatchTime(DWORD i) { m_dwLastCatch = i; }
		int GetLastCatchTime() { return m_dwLastCatch; }
		void SetFishCatchFailed(int i) { m_dwCatchFailed = i; }
		BYTE GetFishCatchFailed() { return m_dwCatchFailed; }

	private:
		BYTE m_bFishCatch;
		DWORD m_dwCatchFailed;
		int m_dwLastCatch;
#endif
	public:
		int		GetGoToXYTime() const { return m_iGoToXYTime; }
		void	SetGoToXYTime() { m_iGoToXYTime = thecore_pulse(); }

	protected:
		int		m_iGoToXYTime;

#ifdef ENABLE_SAVEPOINT_SYSTEM
	public:
		int		GetSavePointTime() const { return m_iSavePointTime; }
		void	SetSavePointTime() { m_iSavePointTime = thecore_pulse(); }

	protected:
		int		m_iSavePointTime;
#endif

#ifdef ENABLE_SORT_INVEN
	public:
		int		GetSortInv1Time() const { return m_iSortInv1Time; }
		void	SetSortInv1Time() { m_iSortInv1Time = thecore_pulse(); }
		int		GetSortInv2Time() const { return m_iSortInv2Time; }
		void	SetSortInv2Time() { m_iSortInv2Time = thecore_pulse(); }

	protected:
		int		m_iSortInv1Time;
		int		m_iSortInv2Time;
#endif

#ifdef ENABLE_LIMIT_BUY_SPEED
	public:
		int			GetLastBuyTime() const { return m_iLastBuyTime; }
		void		SetLastBuyTime() { m_iLastBuyTime = thecore_pulse(); }

	protected:
		int			m_iLastBuyTime;
#endif

#ifdef ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
	public:
		bool	GetDeadByMonster() const { return m_deadByMonster; }
		void	SetDeadByMonster(bool flag) { m_deadByMonster = flag; }

	protected:
		bool	m_deadByMonster;
#endif

#ifdef ENABLE_SPAM_CHECK
	public:
		int32_t	GetLastUnlock() const { return m_iLastUnlock; }
		void	SetLastUnlock() { m_iLastUnlock = get_global_time() + 3; }
		int32_t	GetLastDSREfine() const { return m_iLastDSRefine; }
		void	SetLastDSREfine() { m_iLastDSRefine = get_global_time() + 3; }

	protected:
		int32_t	m_iLastUnlock, m_iLastDSRefine;
#endif

	public:
#ifdef ENABLE_BIOLOGIST_UI
		void CheckBiologistReward();
#endif
#ifdef ENABLE_ANTICHEAT
		void ClearCheatChecks();
		void ProcessCheatCheck(int32_t time);
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
		void BlockProcessed();
		void BlockDrop();
		void UnblockDrop();
		void SetDropStatus();
#endif
#ifdef EANBLE_PATCH_YOHARA
		uint8_t GetChampionLevel() const { return m_points.championlevel; }
		void SetChampionLevel(uint8_t level) { m_points.championlevel = championlevel; };
#endif
#ifdef __DEFENSE_WAVE__
		bool IsDefanceWaweMastAttackMob(int32_t vnum) const { return (vnum >= 3401 && vnum <= 3405) || (vnum >= 3601 && vnum <= 3605) || (vnum >= 3950 && vnum <= 3964); }
#endif
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
		void SetWheelDestiny(std::shared_ptr<CWheelDestiny> pt) { pWheelDestiny = std::move(pt); };
		std::shared_ptr<CWheelDestiny> GetWheelDestiny() const { return pWheelDestiny; }
#endif

	private:
#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
		std::shared_ptr<CWheelDestiny> pWheelDestiny = nullptr;
#endif

	protected:
#ifdef ENABLE_ANTICHEAT
		int32_t m_firstReward, m_rewardCount, m_checkRepeated, m_dropitemcount, m_lastdropitem;
#endif
#ifdef ENABLE_BLOCK_MULTIFARM
		LPEVENT m_pkDropEvent;
#endif

#ifdef ENABLE_USEITEM_COOLDOWN
	private:
		int m_dwItemUseAntiFloodPulse;
		DWORD m_dwItemUseAntiFloodCount;

	public:
		int GetItemUseAntiFloodPulse(){return m_dwItemUseAntiFloodPulse;}
		DWORD GetItemUseAntiFloodCount(){return m_dwItemUseAntiFloodCount;}
		DWORD IncreaseItemUseAntiFloodCount(){return ++m_dwItemUseAntiFloodCount;}
		void SetItemUseAntiFloodPulse(int dwPulse){m_dwItemUseAntiFloodPulse=dwPulse;}
		void SetItemUseAntiFloodCount(DWORD dwCount){m_dwItemUseAntiFloodCount=dwCount;}
#endif

#ifdef ENABLE_ITEMSHOP
		public:
			void	SendItemshopItemsPacket(
				const std::unordered_map <BYTE, std::vector<TItemshopItemTable>> items,
				const std::unordered_map <BYTE, TItemshopCategoryInfo> categories
			);
			void	SendPromotionRewardPacket(BYTE byAnswer, std::vector<TPromotionItemTable>);
			void	SendItemshopSingleItemRefreshPacket(TItemshopItemTable* item);
			void	SendItemshopSingleItemRemovePacket(TItemshopItemTable* item);
			void	SendItemshopSingleItemAddPacket(TItemshopItemTable* item, TItemshopCategoryInfo category);
			void	SendItemshopCoinPacket();
			bool	CanUseItemshop() const { return  m_iItemshopCooldownTime < thecore_pulse(); }
			void	SetItemshopCooldown(int pulse) { m_iItemshopCooldownTime = pulse; }
			bool	CanUsePromotionCode() const { return  m_iPromotionCodeCooldownTime < thecore_pulse(); }
			void	SetPromotionCodedown(int pulse) { m_iPromotionCodeCooldownTime = pulse; }
		protected:
			int		m_iItemshopCooldownTime;
			int		m_iPromotionCodeCooldownTime;
#endif
#if defined(__ATTENDANCE_SYSTEM__)
public:
	void SetAttendance(CAttendance* pAttendance);
	CAttendance* GetAttendance() const { return m_pkAttendance; }

	bool IsOpenAttendance() const { return m_bIsOpenAttendance; }
	void SetOpenAttendance(bool bOpen) { m_bIsOpenAttendance = bOpen; }

private:
	CAttendance* m_pkAttendance;
	bool m_bIsOpenAttendance;
#endif
};

ESex GET_SEX(LPCHARACTER ch);

#ifdef ENABLE_BLOCK_MULTIFARM
EVENTINFO(drop_event_info) {
	DynamicCharacterPtr ch;
	time_t time;
	bool drop;
};
#endif

#ifdef ENABLE_NEW_FISHING_SYSTEM
EVENTINFO(fishingnew_event_info)
{
	DWORD pid, vnum, chance, sec;
	fishingnew_event_info() : pid(0), vnum(0), chance(0), sec(0) {}
};
#endif
#endif
