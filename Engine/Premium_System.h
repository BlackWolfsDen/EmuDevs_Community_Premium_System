#ifndef PREMIUM_SYSTEM_H
#define PREMIUM_SYSTEM_H

struct PremiumElements
{
	uint32 id;
	uint8 premium;
	uint32 hp;
	uint32 power_max;
	uint64 time;
	bool chat;
	uint64 chat_time;
	std::string last_message;
};

struct PremiumItemElements
{
	uint32 id;
	uint8 premium;
};

struct PremiumLocationElements
{
	uint8 id;
	uint32 map_id;
	float x;
	float y;
	float z;
	float o;
};

struct PremiumPlayerLocationElements
{
	uint32 guid;
	uint32 map_id;
	float x;
	float y;
	float z;
	float o;
};

struct PremiumTeamLocationElements
{
	uint8 team;
	uint32 map_id;
	float x;
	float y;
	float z;
	float o;
};

struct ClassSpells
{
	uint64 id;
	uint8 class_id;
	uint32 spell_id;
};

class PREM
{

public:
	PREM();
	~PREM();

	// Toolz
	static uint64 ConvertStringToNumber(std::string arg);
	static std::string ConvertNumberToString(uint64 numberX);

	// Global Getterz
	static uint8 GetPremiumType();
	static float PREM::GetPremiumModifier();
	static bool IsPremiumTimed();
	static bool CanWaterBreathe();
	static bool CanDecreaseSpellCost();
	static uint64 GetPremiumDurationInSeconds();
	static uint32 GetPremiumDurationInDays();
	static bool IsPremiumItemsEnabled();
	static bool IsPremiumTalentPointBonusEnabled();
	static uint32 GetPremiumTalentPointBonus();
	static bool IsPremiumHealthPointBonusEnabled();
	static bool IsPremiumManaPointBonusEnabled();
	static bool IsPremiumRagePointBonusEnabled();
	static std::string GetAmountInString(uint32 amount);

	// Player Getterz
	static uint32 GetPlayerPremiumId(Player* player);
	static bool IsPlayerPremium(Player* player);
	static void UpdatePlayerCustomHomeTeleport(uint32 guid, uint32 map_id, float x, float y, float z, float o);
	static uint64 GetPlayerPremiumStartTimeInSeconds(Player* player);
	static uint64 GetPlayerPremiumRemainingTimeInSeconds(Player* player);
	static std::string GetPlayerPremiumTimeLeftInString(Player* player);
	static uint32 GetPlayerPremiumTimeInDays(Player* player);
	static void DepositGoldToPlayerGuildBank(Player* player, uint32 amount);
	static uint32 IncreaseValueWithModifier(Player* player, uint32 value);
	static uint32 DecreaseValueWithModifier(Player* player, uint32 value);

	// Player Setterz
	static void AddPremiumToPlayer(Player* player);
	static void RemovePremiumFromPlayer(Player* player);
	static void UpdatePlayerPremiumValue(Player* player, uint8 value, uint64 time);

	// Item Getters
	static bool IsItemPremium(Item* item);
	// Item Setterz
	static void UpdateItemPremiumValue(uint32 item_id, uint8 value);

private:
	// Tools
	static bool CheckIfPlayerInCombatOrDead(Player* player);
	static void TeleportPlayer(Player* player, uint8 id);
	static void RemoveItem(uint32 id, Player* player);
};

#endif