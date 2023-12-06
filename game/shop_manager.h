#ifndef __INC_METIN_II_GAME_SHOP_MANAGER_H__
#define __INC_METIN_II_GAME_SHOP_MANAGER_H__

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	void	Destroy();

	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);

	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);

	void	Buy(LPCHARACTER ch, BYTE pos);
#ifdef ENABLE_BUY_STACK_FROM_SHOP
	void MultipleBuy(LPCHARACTER ch, uint8_t p, uint8_t c);
#endif
#ifdef ENABLE_EXTRA_INVENTORY
	void	Sell(LPCHARACTER ch, TItemPos Cell,
#ifdef ENABLE_NEW_STACK_LIMIT
	WORD bCount = 0
#else
	BYTE bCount = 0
#endif
	);
#else
	void	Sell(LPCHARACTER ch, BYTE bCell,
#ifdef ENABLE_NEW_STACK_LIMIT
	WORD bCount = 0
#else
	BYTE bCount = 0
#endif
	);
#endif


	LPSHOP	CreatePCShop(LPCHARACTER ch, TShopItemTable * pTable, BYTE bItemCount);
	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;

	bool	ReadShopTableEx(const char* stFileName);
};

#endif
