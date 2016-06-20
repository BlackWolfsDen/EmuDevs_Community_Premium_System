#ifndef PREMIUM_SYSTEM_H
#define PREMIUM_SYSTEM_H

#include <unordered_map>

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

class PremiumSystemMgr
{
private:
	PremiumSystemMgr();
	~PremiumSystemMgr();

public:
	static PremiumSystemMgr* instance();

	uint8 GetPremiumType() { return premiumType; }
	uint8 GetChatTeam() { return premiumChatTeam; }
	uint8 GetGMMinRank() { return premiumGMMinRank; }
	float GetModifier() { return premiumMod; }
	bool IsTimed() { return premiumTimerEnabled != 0; }
	bool CanWaterBreathe() { return premiumWaterBreathe != 0; }
	bool CanDecreaseSpellCost() { return premiumSpellCostDecrease != 0; }
	bool IsItemsEnabled() { return premiumItemsEnabled != 0; }
	bool IsTalentPointBonusEnabled() { return premiumTPEnabled != 0; }
	bool IsHealthPointBonusEnabled() { return premiumHPEnabled != 0; }
	bool IsManaPointBonusEnabled() { return premiumManaEnabled != 0; }
	bool IsRagePointBonusEnabled() { return premiumRageEnabled != 0; }
	uint64 GetDurationInSeconds() { return premiumTimerDuration; }
	uint32 GetDurationInDays() { return ((((premiumTimerDuration) / 60) / 60) / 24); }
	uint32 GetTalentPointBonus() { return premiumTPBonus; }
	uint32 GetUpgradeItem() { return premiumUpgradeItem; }
	uint32 GetTitleId() { return premiumTitleId; }
	uint32 GetTitleMaskId() { return premiumTitleMaskId; }
	uint32 GetChatDelay() { return premiumChatDelay; }

	void SetPremiumType(uint8 v) { premiumType = v; }
	void SetTimerEnabled(bool v) { premiumTimerEnabled = v; }
	void SetTimerDuration(uint64 v) { premiumTimerDuration *= v; }
	void SetGMMinRank(uint8 v) { premiumGMMinRank = v; }
	void SetItemEnabled(bool v) { premiumItemsEnabled = v; }
	void SetModifier(float v) { premiumMod = v; }
	void SetChatDelay(uint32 v) { premiumChatDelay = v; }
	void SetChatTeam(uint8 v) { premiumChatTeam = v; }
	void SetTPEnabled(bool v) { premiumTPEnabled = v; }
	void SetTPBonus(uint32 v) { premiumTPBonus = v; }
	void SetHPEnabled(bool v) { premiumHPEnabled = v; }
	void SetManaEnabled(bool v) { premiumManaEnabled = v; }
	void SetRageEnabled(bool v) { premiumRageEnabled = v; }
	void SetTitleId(uint32 v) { premiumTitleId = v; }
	void SetTitleMaskId(uint32 v) { premiumTitleMaskId = v; }
	void SetWaterBreathe(uint8 v) { premiumWaterBreathe = v; }
	void SetSpellCostDecrease(uint8 v) { premiumSpellCostDecrease = v; }
	void SetUpgradeItem(uint32 v) { premiumUpgradeItem = v; }

	// Player
	uint32 GetPlayerPremiumId(Player* player);
	uint32 GetPlayerPremiumTimeInDays(Player* player);
	uint32 IncreaseValueWithModifier(Player* player, uint32 value);
	uint32 DecreaseValueWithModifier(Player* player, uint32 value);
	uint64 GetPlayerPremiumStartTimeInSeconds(Player* player);
	uint64 GetPlayerPremiumRemainingTimeInSeconds(Player* player);
	std::string GetPlayerPremiumTimeLeftInString(Player* player);
	bool IsPlayerPremium(Player* player);
	void UpdatePlayerCustomHomeTeleport(uint32 guid, uint32 map_id, float x, float y, float z, float o);
	void DepositGoldToPlayerGuildBank(Player* player, uint32 amount);
	void AddPremiumToPlayer(Player* player);
	void RemovePremiumFromPlayer(Player* player);
	void UpdatePlayerPremiumValue(Player* player, uint8 value, uint64 time);

	// Item
	bool IsItemPremium(Item* item);
	void UpdateItemPremiumValue(uint32 item_id, uint8 value);
	void RemoveItem(uint32 id, Player* player);

	// Misc
	void DeletePremium(Player* player);
	uint64 ConvertStringToNumber(std::string arg);
	std::string ConvertNumberToString(uint64 numberX);
	std::string GetAmountInString(uint32 amount);
	bool CheckIfPlayerInCombatOrDead(Player* player);
	void TeleportPlayer(Player* player, uint8 id);

	std::unordered_map<uint32, PremiumElements> Premium;
	std::unordered_map<uint32, PremiumItemElements> PremiumItem;
	std::unordered_map<uint32, PremiumLocationElements> PremiumLocations;
	std::unordered_map<uint32, PremiumLocationElements> PremiumMallLocations;
	std::unordered_map<uint32, PremiumLocationElements> PremiumPublicMallLocations;
	std::unordered_map<uint32, PremiumPlayerLocationElements> PremiumPlayerLocations;
	std::unordered_map<uint32, PremiumTeamLocationElements> PremiumTeamLocations;

private:
	uint8 premiumType;
	uint8 premiumTimerEnabled;
	uint8 premiumItemsEnabled;
	uint8 premiumTPEnabled;
	uint8 premiumHPEnabled;
	uint8 premiumManaEnabled;
	uint8 premiumRageEnabled;
	uint8 premiumGMMinRank;
	uint8 premiumChatTeam;
	uint8 premiumWaterBreathe;
	uint8 premiumSpellCostDecrease;
	uint32 premiumTitleId;
	uint32 premiumTitleMaskId;
	uint32 premiumTPBonus;
	uint32 premiumUpgradeItem;
	uint32 premiumChatDelay;
	uint64 premiumTimerDuration;

	float premiumMod;
};

#define sPremiumSystemMgr PremiumSystemMgr::instance()
#endif