#ifndef __INC_SERVICE_H__
#define __INC_SERVICE_H__

#define ENABLE_NEW_CHANGES
#define _IMPROVED_PACKET_ENCRYPTION_
#define ENABLE_SEND_TARGET_INFO
#define ENABLE_ITEMSHOP
#define __PLAYER_PIN_SYSTEM__
#define __ATTENDANCE_SYSTEM__
#define DROP_WIKI
#ifdef DROP_WIKI
#define ITEM_DROP_SIZE 70
#endif
#define __ATTR_TRANSFER_SYSTEM__
#define __PET_SYSTEM__
#define __UDP_BLOCK__
#define ENABLE_NEW_SECONDARY_SKILLS
#define ENABLE_NEW_PASSIVE_SKILLS
#define ENABLE_ATTR_COSTUMES
#define __ENABLE_BERAN_ADDONS_
#define GROUP_BUFF
#define __ENABLE_SPIDER_ADDONS_
#define ADVANCED_GUILD_INFO
#define ENABLE_ANNOUNCEMENT_LEVELUP
#define ENABLE_MAX_ADD_ATTRIBUTE
#define ENABLE_BUY_WITH_ITEM
#define ENABLE_SOUL_SYSTEM
#define ATTR_LOCK
#define FLY_SYSTEM
#define ENABLE_SKILL_COLOR_SYSTEM
#define ENABLE_BATTLE_PASS
#ifdef ENABLE_BATTLE_PASS
#define ENABLE_BATTLE_PASS_STAY_ONLINE
#define ENABLE_BATTLE_PASS_CHAT_CNT
#define ENABLE_BATTLE_PASS_SECURITY_KILL
#endif
#define __NEW_EXCHANGE_WINDOW__
#define ENABLE_PVP_ADVANCED
#define EQUIP_ENABLE_VIEW_SASH
#define __NEWPET_SYSTEM__
#ifdef ENABLE_PVP_ADVANCED
	#define BLOCK_CHANGEITEM	"pvp.BlockChangeItem"
	#define BLOCK_BUFF			"pvp.BlockBuff"
	#define BLOCK_POTION		"pvp.BlockPotion"
	#define BLOCK_RIDE			"pvp.BlockRide"
	#define BLOCK_PET			"pvp.BlockPet"
	#define BLOCK_POLY			"pvp.BlockPoly"
	#define BLOCK_PARTY			"pvp.BlockParty"
	#define BLOCK_EXCHANGE_		"pvp.BlockExchange"
	#define BLOCK_EQUIPMENT_	"pvp.BLOCK_VIEW_EQUIPMENT"
	#define BET_WINNER			"pvp.BetMoney"
	#define CHECK_IS_FIGHT		"pvp.IsFight"
#endif
#define ENABLE_RANKING
#define ENABLE_LOCKED_EXTRA_INVENTORY
#define ENABLE_DS_SET
#define ENABLE_DS_EDITS
#define ENABLE_DS_ENCHANT
#define ENABLE_MOUNT_COSTUME_SYSTEM
#define __HIGHLIGHT_SYSTEM__
#define ENABLE_NEW_PET_EDITS
#define ENABLE_REMOTE_ATTR_SASH_REMOVE
#define ENABLE_ATLAS_BOSS
#define ENABLE_STOLE_REAL
#define ENABLE_STOLE_COSTUME
#define ENABLE_COSTUME_PET
#define ENABLE_COSTUME_MOUNT
#define ENABLE_COSTUME_EFFECT
#define ENABLE_FIX_LEVELUP_EFFECT
#define KASMIR_PAKET_SYSTEM
#define ENABLE_WHISPER_ADMIN_SYSTEM
#define ENABLE_BUG_FIXES
#define ENABLE_DS_RUNE
#define ENABLE_BLOCK_MULTIFARM
#define ENABLE_ANCIENT_PYRAMID
#ifdef ENABLE_ANCIENT_PYRAMID
	#define PYRAMID_BOSSVNUM 4158
#endif
#define BL_OFFLINE_MESSAGE
#define ENABLE_DUNGEON_MANAGER
#define ENABLE_RUNE_SYSTEM
#ifdef ENABLE_RUNE_SYSTEM
	#define RUNE_SHOP 54
	#define RUNE_EFFECT_FROM 60
#endif
#define ENABLE_NEW_USE_POTION
#define ENABLE_ENCHANT_CHANGES
#define ENABLE_STATUS_MAX_344_POINTS
#define __EFFETTO_MANTELLO__
#define ENABLE_RECALL
#define ENABLE_SKILLS_BUFF_ALTERNATIVE
#define ENABLE_NEW_STACK_LIMIT
#define ENABLE_NEW_COMMON_BONUSES
#define ENABLE_CHANGE_ATTRIBUTE_RULES
#define ENABLE_NEW_CHAT
#define ENABLE_DS_GRADE_MYTH
#define TEXTS_IMPROVEMENT
#define BLOCK_RIDING_INSIDE_WAR
#define ENABLE_INFINITE_RAFINES
#define ENABLE_BIOLOGIST_UI
#define ENABLE_DS_POTION_DIFFRENT
#define ENABLE_NEW_FISHING_SYSTEM
#if defined(ENABLE_NEW_FISHING_SYSTEM) && !defined(FISHING_NEED_CATCH)
#define FISHING_NEED_CATCH 3
#endif
#define ENABLE_NEW_UNIQUE_WEAR_LIMITED
#define ENABLE_EXTRA_INVENTORY
#define ENABLE_NO_MALUS_JEONGWIHON
#define __INGAME_WIKI__
#define WJ_ENABLE_TRADABLE_ICON
#define ENABLE_NEW_GYEONGGONG_SKILL
#define __DISABLE_SEND_SEQUENCE__
#define ENABLE_REWARD_AT_START
//#define ENABLE_NO_ATTBONUS_MONSTER_FOR_STONES
#define ENABLE_25082021
#ifdef ENABLE_25082021
//#define ENABLE_LIMIT_BUY_SPEED
//#define ENABLE_EFFECT_PENETRATE
//#define ENABLE_CHAT_LOGGING
//#define ENABLE_CHAT_SPAMLIMIT
//#define ENABLE_WHISPER_CHAT_SPAMLIMIT
#define ENABLE_EXTEND_ITEM_AWARD
#endif
#define ENABLE_MULTI_NAMES
//#define ENABLE_NEW_MOVEMENT_SPEED
#define ENABLE_01092021
#ifdef ENABLE_01092021
#define ENABLE_SPECIAL_INV_TO_SAFEBOX
#endif
#define ENABLE_REVIVE_WITH_HALF_HP_IF_MONSTER_KILLED_YOU
#define ENABLE_CHOOSE_DOCTRINE_GUI
#define ENABLE_GENERAL_CH
#define ENABLE_ITEMSHOP_ITEM
//#define __NEW_EVENT_HANDLER__
#define ENABLE_RESTRICT_GM_PERMISSIONS
#define STATIC_NUMBER_GUILD
#define ENABLE_SPAM_CHECK
#define ENABLE_DS_REFINE_ALL
//#define ENABLE_ANTICHEAT
#define ENABLE_BUY_STACK_FROM_SHOP
#ifdef ENABLE_BUY_STACK_FROM_SHOP
#define MULTIPLE_BUY_LIMIT 100
#else
#define MULTIPLE_BUY_LIMIT 0
#endif
#define ENABLE_OPENSHOP_PACKET
#define ENABLE_HWID
#ifdef ENABLE_HWID
#define EANBLE_HWID_BAN
#endif
#define ENABLE_VOTE_FOR_BONUS
#define ENABLE_MELEY_LAIR
#define ENABLE_MESSENGER_TEAM
#define ENABLE_MESSENGER_HELPER
//#define EANBLE_PATCH_YOHARA
//#define __IMPROVED_HANDSHAKE_PROCESS__
//#define ENABLE_DUNGEON_BUGFIXES
/* finire
#define ENABLE_ORDER_BY_LASTPLAY
*/
#define ENABLE_USEITEM_COOLDOWN
// #define ENABLE_CHECK_BATTLE
#endif