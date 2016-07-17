#ifndef PREMIUM_SYSTEM_H
#define PREMIUM_SYSTEM_H
#define sPremiumSystemMgr PremiumSystemMgr::instance()

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

	// variables
	uint8 PREMIUM_TYPE;
	bool PREMIUM_TIMER_ENABLE;
	uint64 PREMIUM_TIMER_DURATION = (((1 * 60) * 60) * 24); // Defining day in seconds. 1 = 1 second. 1 second * 60 = 1 minute. 1 min * 60 = 1 hour. 1 hour * 24 = 1 day. we then will call from the conf and get x for days.
	uint8 PREMIUM_GM_MINIMUM_RANK;
	uint32 PREMIUM_UPGRADE_ITEM;
	bool PREMIUM_ITEMS_ENABLE;
	float PREMIUM_MODIFIER;
	uint8 PREMIUM_CHAT_DELAY;
	uint8 PREMIUM_CHAT_TEAM;
	bool PREMIUM_TP_ENABLE;
	uint8 PREMIUM_TP_BONUS;
	bool PREMIUM_HP_ENABLE;
	bool PREMIUM_MANA_ENABLE;
	bool PREMIUM_RAGE_ENABLE;
	uint32 PREMIUM_TITLE_ID;
	uint32 PREMIUM_TITLE_MASK_ID;
	bool PREMIUM_WATER_BREATHE;
	bool PREMIUM_SPELL_COST_DECREASE_ENABLED;
	uint8 PREMIUM_SPELL_COST_DECREASE;

public:
	static PremiumSystemMgr* instance();

	std::unordered_map<uint32, PremiumElements>Premium;
	std::unordered_map<uint32, PremiumItemElements>PremiumItem;
	std::unordered_map<uint32, PremiumLocationElements>PremiumLocations;
	std::unordered_map<uint32, PremiumLocationElements>PremiumMallLocations;
	std::unordered_map<uint32, PremiumLocationElements>PremiumPublicMallLocations;
	std::unordered_map<uint32, PremiumPlayerLocationElements>PremiumPlayerLocations;
	std::unordered_map<uint32, PremiumTeamLocationElements>PremiumTeamLocations;

	// Toolz
	uint64 ConvertStringToNumber(std::string arg);
	std::string ConvertNumberToString(uint64 numberX);
	void RemovePremiumItem(uint32 id, Player* player)	{player->DestroyItemCount(uint32(id), 1, true);}

	// Global Getterz -n- Setterz
	uint8 GetPremiumType() { return PREMIUM_TYPE; };
	void SetPremiumType(uint8 v) { PREMIUM_TYPE = v; };
	bool IsPremiumTimed() { return PREMIUM_TIMER_ENABLE != 0; }
	void SetPremiumTimerEnabled(bool v) { PREMIUM_TIMER_ENABLE = v; }
	uint64 GetPremiumTimerDuration() { return PREMIUM_TIMER_DURATION; }
	void SetPremiumTimerDuration(uint64 v) { PREMIUM_TIMER_DURATION *= v; }
	uint64 GetPremiumDurationInSeconds() { return PREMIUM_TIMER_DURATION; }
	uint32 GetPremiumDurationInDays() { return ((((PREMIUM_TIMER_DURATION) / 60) / 60) / 24); }
	float GetPremiumModifier() { return PREMIUM_MODIFIER; }
	void SetPremiumModifier(float v) { PREMIUM_MODIFIER = v; }
	bool IsPremiumItemsEnabled() { return PREMIUM_ITEMS_ENABLE; };
	void SetPremiumItemEnabled(bool v) { PREMIUM_ITEMS_ENABLE = v; };
	bool IsPremiumTalentPointsBonusEnabled() { return PREMIUM_TP_ENABLE != 0; }
	void SetPremiumTalentPointsBonusEnabled(bool v) { PREMIUM_TP_ENABLE = v; }
	uint8 GetPremiumTalentPointsBonus() { return PREMIUM_TP_BONUS; }
	void SetPremiumTalentPointsBonus(uint8 v) { PREMIUM_TP_BONUS = v; }
	bool IsPremiumHealthPointsBonusEnabled() { return PREMIUM_HP_ENABLE != 0; }
	void SetPremiumHealthPointsBonusEnabled(bool v) { PREMIUM_HP_ENABLE = v; }
	bool IsPremiumManaPointsBonusEnabled() { return PREMIUM_MANA_ENABLE != 0; }
	void SetPremiumManaPointsBonusEnabled(bool v) { PREMIUM_MANA_ENABLE = v; }
	bool IsPremiumRagePointsBonusEnabled() { return PREMIUM_RAGE_ENABLE != 0; }
	void SetPremiumRagePointsBonusEnabled(bool v) { PREMIUM_RAGE_ENABLE = v; }
	bool CanPremiumWaterBreathe() { return PREMIUM_WATER_BREATHE; }
	void SetPremiumWaterBreathe(bool v) { PREMIUM_WATER_BREATHE = v; }
	uint8 GetPremiumChatDelay() { return PREMIUM_CHAT_DELAY; }
	void SetPremiumChatDelay(uint8 v) { PREMIUM_CHAT_DELAY = v; }
	uint32 GetPremiumTitleId() { return PREMIUM_TITLE_ID; }
	void SetPremiumTitleId(uint32 v) { PREMIUM_TITLE_ID = v; }
	uint32 GetPremiumTitleMaskId() { return PREMIUM_TITLE_MASK_ID; }
	void SetPremiumTitleMaskId(uint32 v) { PREMIUM_TITLE_MASK_ID = v; }
	bool CanDecreaseSpellCost() {return PREMIUM_SPELL_COST_DECREASE_ENABLED;};
	uint8 GetPremiumSpellCostDecrease() { return PREMIUM_SPELL_COST_DECREASE; }
	void SetPremiumSpellCostDecrease(uint8 v) { PREMIUM_SPELL_COST_DECREASE = v; }
	uint8 GetPremiumGMMinRank() { return PREMIUM_GM_MINIMUM_RANK; }
	void SetPremiumGMMinRank(uint8 v) { PREMIUM_GM_MINIMUM_RANK = v; }
	uint32 GetPremiumUpgradeItem() { return PREMIUM_UPGRADE_ITEM; }
	void SetPremiumUpgradeItem(uint32 v) { PREMIUM_UPGRADE_ITEM = v; }
	std::string GetAmountInString(uint32 amount);


	// Player Getterz
	uint32 GetPlayerPremiumId(Player* player);
	bool IsPlayerPremium(uint32 v) {return Premium[v].premium != 0;	};
	uint8 GetPremiumChatTeam() { return PREMIUM_CHAT_TEAM; }
	void SetPremiumChatTeam(uint8 v) { PREMIUM_CHAT_TEAM = v; }
	void UpdatePlayerCustomHomeTeleport(uint32 guid, uint32 map_id, float x, float y, float z, float o);
	uint64 GetPlayerPremiumStartTime(Player* player) { return Premium[GetPlayerPremiumId(player)].time; };
	uint64 GetPlayerPremiumRemainingTimeInSeconds(Player* player);
	std::string GetPlayerPremiumTimeLeftInString(Player* player);
	void DepositGoldToPremiumPlayerGuildBank(Player* player, uint32 amount);
	uint32 IncreaseValueWithPremiumModifier(Player* player, uint32 value);
	uint32 DecreaseValueWithPremiumModifier(Player* player, uint32 value);

	// Player Setterz
	void SetPlayerPremiumTime(Player* player, uint64 v) { Premium[GetPlayerPremiumId(player)].time = v; };

	void AddPremiumToPlayer(Player* player);
	void RemovePremiumFromPlayer(Player* player);
	void UpdatePlayerPremiumValue(Player* player, uint8 value, uint64 time);

	// Item Getters -n- Setterz
	bool IsItemPremium(uint32 v) { return PremiumItem[v].premium != 0; };
	void UpdateItemPremiumValue(uint32 v1, uint8 v2) { PremiumItem[v1].premium = v2; };

private:
	
	// Tools
	bool CheckIfPlayerInCombatOrDead(Player* player);
	void TeleportPlayer(Player* player, uint8 id);
};
#endif