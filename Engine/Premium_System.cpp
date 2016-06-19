/*
			© EmuDev's Premium System ©

	By slp13at420 `The Mad Scientist` of EmuDevs.com ©
	with the community of EmuDevs.com,
	for the community of EmuDevs.com.

	project info:
		Type : CPP
		started : 04/20/2016
		finished : unfinished Public Release
		public released : 04/20/2016
		lead programmer : slp13at420
		ideas provided by : Portals, Kaev, Vitrex, jonmii, slp13at420.
		scripting provided by : slp13at420.
		scripting guideance provided by : GrandElf, Rochet2.

	This is a CPP Premium System.
	This allows for either account Premium
	rank or character Premium rank.
	allows for permenant Premium rank 
	or timed based duration Premium rank.

	© This is an EmuDevs.com only release. ©
	© do not remove or change credits above ©
	© dont share this System without prior approval ©
	© Dont re-release as yours or anothers work ©
*/

#include "chat.h"
#include "Config.h"
#include "Guild.h"
#include "player.h"
#include "RBAC.h"
#include "Premium_System.h"
#include "ScriptMgr.h"
#include "Unit.h"
#include "World.h"

//#include "AccountMgr.h"
//#include <cstring>
//#include "Group.h"
//#include "Language.h"
//#include "math.h"
//#include "ObjectMgr.h"
//#include "ScriptedGossip.h"
//#include <sstream>
//#include <string>
//#include <unordered_map>

uint8 PREMIUM_TYPE;
uint8 PREMIUM_TIMER_ENABLE;
uint64 PREMIUM_TIMER_DURATION = (((1 * 60) * 60) * 24); // Defining day in ms. 1000 = 1 second. 1 second * 60 = 1 minute. 1 min * 60 = 1 hour. 1 hour * 24 = 1 day. we then will call from the conf and get x for days.
uint8 PREMIUM_GM_MINIMUM_RANK;
uint32 PREMIUM_UPGRADE_ITEM;
uint8 PREMIUM_ITEMS_ENABLE;
float PREMIUM_MODIFIER;
uint32 PREMIUM_CHAT_DELAY;
uint8 PREMIUM_CHAT_TEAM;
uint8 PREMIUM_TP_ENABLE;
uint32 PREMIUM_TP_BONUS;
uint8 PREMIUM_HP_ENABLE;
uint8 PREMIUM_MANA_ENABLE;
uint8 PREMIUM_RAGE_ENABLE;
uint32 PREMIUM_TITLE_ID;
uint32 PREMIUM_TITLE_MASK_ID;
uint8 PREMIUM_WATER_BREATHE;
uint8 PREMIUM_SPELL_COST_DECREASE;

std::unordered_map<uint32, PremiumElements>Premium;
std::unordered_map<uint32, PremiumItemElements>PremiumItem;
std::unordered_map<uint32, PremiumLocationElements>PremiumLocations;
std::unordered_map<uint32, PremiumLocationElements>PremiumMallLocations;
std::unordered_map<uint32, PremiumLocationElements>PremiumPublicMallLocations;
std::unordered_map<uint32, PremiumPlayerLocationElements>PremiumPlayerLocations;
std::unordered_map<uint32, PremiumTeamLocationElements>PremiumTeamLocations;


int BUFFS[24] = { 24752, 48074, 43223, 36880, 467, 48469, 48162, 48170, 16877, 10220, 13033, 11735, 10952, 23948, 26662, 47440, 53307, 132, 23737, 48470, 43002, 26393, 24705, 69994 };
int DEBUFFS[4] = { 57724, 57723, 80354, 95809 };

PREM::PREM() { }

PREM::~PREM()
{
}

// Global Functions

uint64 PREM::ConvertStringToNumber(std::string arg)
{
	uint64 Value64;

	std::istringstream(arg) >> Value64;

	return Value64;
}

std::string PREM::ConvertNumberToString(uint64 numberX)
{
	auto number = numberX;
	std::stringstream convert;
	std::string number32_to_string;
	convert << number;
	number32_to_string = convert.str();

	return number32_to_string;
};

std::string PREM::GetAmountInString(uint32 amount)
{
	uint64 tmp;
	std::string output = "";

	if (amount > 9999)
	{
		tmp = floor(amount / 10000); 
		amount = amount - (tmp * 10000);
		output = output + PREM::ConvertNumberToString(tmp) + " Gold, ";
	}

	if (amount > 99)
	{
		tmp = floor(amount / 100);
		amount = amount - (tmp * 100);
		output = output + PREM::ConvertNumberToString(tmp) + " Silver, ";
	}

	if (amount <= 99)
	{
		tmp = (amount);
		amount = amount - (tmp);
		output = output + PREM::ConvertNumberToString(tmp) + " Copper";
	}

	return output;
}

class PREMIUM_Load_Conf : public WorldScript
{
public: 
	PREMIUM_Load_Conf() : WorldScript("PREMIUM_Load_Conf"){ };

		virtual void OnConfigLoad(bool /*reload*/)
		{
			TC_LOG_INFO("server.loading", "______________________________________");
			TC_LOG_INFO("server.loading", "-     EmuDevs.com Premium System     -");

			PREMIUM_TYPE				= sConfigMgr->GetIntDefault("PREM.TYPE", 0); // 0 = acct :: 1 = character.
			PREMIUM_TIMER_ENABLE		= sConfigMgr->GetIntDefault("PREM.TIMED", 0); // 0 = no :: 1 = yes duration -> Premium will reset after x days..
			PREMIUM_TIMER_DURATION		*= sConfigMgr->GetIntDefault("PREM.DURATION", 30); // x in days.
			PREMIUM_GM_MINIMUM_RANK		= sConfigMgr->GetIntDefault("PREM.GM_MINIMUM_RANK", 3);
			PREMIUM_UPGRADE_ITEM		= sConfigMgr->GetIntDefault("PREM.UPGRADE_ITEM", 64000);
			PREMIUM_ITEMS_ENABLE		= sConfigMgr->GetIntDefault("PREM.ITEMS", 0);
			PREMIUM_MODIFIER			= sConfigMgr->GetFloatDefault("PREM.MODIFIER", 0.2f);
			PREMIUM_CHAT_DELAY			= sConfigMgr->GetIntDefault("PREM.CHAT_TIMER", 5); // in seconds // 5 = 5 seconds.
			PREMIUM_CHAT_TEAM			= sConfigMgr->GetIntDefault("PREM.CHAT_TEAM", 0); // 0 = team chat only // 1 world chat.
			PREMIUM_TP_ENABLE			= sConfigMgr->GetIntDefault("PREM.TP_ENABLE", 0);
			PREMIUM_TP_BONUS			= sConfigMgr->GetIntDefault("PREM.TP_BONUS", 14);
			PREMIUM_HP_ENABLE			= sConfigMgr->GetIntDefault("PREM.HP_ENABLE", 0);
			PREMIUM_MANA_ENABLE			= sConfigMgr->GetIntDefault("PREM.MANA_ENABLE", 0);
			PREMIUM_RAGE_ENABLE			= sConfigMgr->GetIntDefault("PREM.RAGE_ENABLE", 0);
			PREMIUM_TITLE_ID			= sConfigMgr->GetIntDefault("PREM.TITLE_ID", 500);
			PREMIUM_TITLE_MASK_ID		= sConfigMgr->GetIntDefault("PREM.TITLE_MASK_ID", 156);
			PREMIUM_WATER_BREATHE		= sConfigMgr->GetIntDefault("PREM.WATER_BREATHE", 0);
			PREMIUM_SPELL_COST_DECREASE = sConfigMgr->GetIntDefault("PREM.SPELL_COST_DECREASE", 0);

			uint32 PREMIUM_ITEM_COUNT = 0;

				if (PREMIUM_ITEMS_ENABLE > 0)
				{
					QueryResult ItemQery = WorldDatabase.PQuery("SELECT `entry`, `premium` FROM `item_template` WHERE `premium`='1';");

					if (ItemQery)
					{
						do
						{
							Field* fields = ItemQery->Fetch();
							uint32 item_id = fields[0].GetUInt32();
							uint32 premium = fields[1].GetUInt8();

							PremiumItemElements& data1 = PremiumItem[item_id];
							// Save the DB values to the MyData object
							data1.id = item_id;
							data1.premium = premium;

							 PREMIUM_ITEM_COUNT += 1;

						} while (ItemQery->NextRow());
					}
				}

			if (PREMIUM_TYPE == 0){ TC_LOG_INFO("server.loading", "- Premium type:Account"); };
			if (PREMIUM_TYPE > 0){ TC_LOG_INFO("server.loading", "- Premium type:Character"); };

			TC_LOG_INFO("server.loading", "- Premium GM Minimum rank:%u", PREMIUM_GM_MINIMUM_RANK);

			if (PREM::IsPremiumTimed()){ TC_LOG_INFO("server.loading", "- Premium rank duration activated"); };
			if (PREM::IsPremiumTimed()){ TC_LOG_INFO("server.loading", "- Premium rank duration:%u days", ((((PREMIUM_TIMER_DURATION) / 60) / 60) / 24)); };
			if (PREM::IsPremiumItemsEnabled()){ TC_LOG_INFO("server.loading", "- Premium Items loaded:%u", PREMIUM_ITEM_COUNT); };

			TC_LOG_INFO("server.loading", "- Premium modifier value:%.2f", PREMIUM_MODIFIER);

			if (PREM::IsPremiumTalentPointBonusEnabled()){ TC_LOG_INFO("server.loading", "- Extra Talent Points Enabled +%u", PREMIUM_TP_BONUS); };
			if (PREM::IsPremiumHealthPointBonusEnabled()){ TC_LOG_INFO("server.loading", "- Extra Health Points Enabled x%.2f", PREMIUM_MODIFIER); };
			if (PREM::IsPremiumManaPointBonusEnabled()){ TC_LOG_INFO("server.loading", "- Extra Mana Points Enabled x%.2f", PREMIUM_MODIFIER); };
			if (PREM::IsPremiumRagePointBonusEnabled()){ TC_LOG_INFO("server.loading", "- Extra Rage Points Enabled x%.2f", PREMIUM_MODIFIER); };
			if (PREM::CanWaterBreathe()){ TC_LOG_INFO("server.loading", "- Premium Water Breathing enabled"); };
			if (PREM::CanDecreaseSpellCost()){ TC_LOG_INFO("server.loading", "- Premium Reduced Spell Costs enabled"); };
			if (PREMIUM_TITLE_ID > 0){ TC_LOG_INFO("server.loading", "- Premium Title ID:%u enabled", PREMIUM_TITLE_ID); };

			QueryResult PremLocQry = WorldDatabase.Query("SELECT * FROM premium_locations");

			if (PremLocQry)
			{
				do
				{
					Field *fields = PremLocQry->Fetch();
					// Save the DB values to the LocData object
					uint8 id = fields[0].GetUInt8();
					uint32 map_id = fields[1].GetUInt32();
					float x = fields[2].GetFloat();
					float y = fields[3].GetFloat();
					float z = fields[4].GetFloat();
					float o = fields[5].GetFloat();

					PremiumLocationElements& data2 = PremiumLocations[id]; // like Lua table GWARZ[guild_id].entry
					data2.id = id;
					data2.map_id = map_id;
					data2.x = x;
					data2.y = y;
					data2.z = z;
					data2.o = o;

				} while (PremLocQry->NextRow());

			}

			if (PremLocQry){ TC_LOG_INFO("server.loading", "- Premium Locations Loaded:%u", PremiumLocations.size()); };

			QueryResult PremMallLocQry = WorldDatabase.Query("SELECT * FROM premium_Mall_locations");

			if (PremMallLocQry)
			{
				do
				{
					Field *fields = PremMallLocQry->Fetch();
					// Save the DB values to the LocData object
					uint8 id = fields[0].GetUInt8();
					uint32 map_id = fields[1].GetUInt32();
					float x = fields[2].GetFloat();
					float y = fields[3].GetFloat();
					float z = fields[4].GetFloat();
					float o = fields[5].GetFloat();

					PremiumLocationElements& data3 = PremiumMallLocations[id]; // like Lua table GWARZ[guild_id].entry
					data3.id = id;
					data3.map_id = map_id;
					data3.x = x;
					data3.y = y;
					data3.z = z;
					data3.o = o;

				} while (PremMallLocQry->NextRow());

			}

			if (PremMallLocQry){ TC_LOG_INFO("server.loading", "- Premium Mall Locations Loaded:%u", PremiumMallLocations.size()); };

			QueryResult PremPlayerLocQry = WorldDatabase.Query("SELECT * FROM premium_player_teleports");

			if (PremPlayerLocQry)
			{
				do
				{
					Field *fields = PremPlayerLocQry->Fetch();
					// Save the DB values to the LocData object
					uint32 guid = fields[0].GetUInt32();
					uint32 map_id = fields[1].GetUInt32();
					float x = fields[2].GetFloat();
					float y = fields[3].GetFloat();
					float z = fields[4].GetFloat();
					float o = fields[5].GetFloat();

					PremiumPlayerLocationElements& data4 = PremiumPlayerLocations[guid]; // like Lua table GWARZ[guild_id].entry
					data4.guid = guid;
					data4.map_id = map_id;
					data4.x = x;
					data4.y = y;
					data4.z = z;
					data4.o = o;

				} while (PremPlayerLocQry->NextRow());

			}

			if (PremPlayerLocQry){ TC_LOG_INFO("server.loading", "- Premium Player Locations Loaded:%u", PremiumPlayerLocations.size()); };

			QueryResult PremTeamLocQry = WorldDatabase.Query("SELECT * FROM premium_team_teleports");

			if (PremTeamLocQry)
			{
				do
				{
					Field *fields = PremTeamLocQry->Fetch();
					// Save the DB values to the LocData object
					uint32 team = fields[0].GetUInt32();
					uint32 map_id = fields[1].GetUInt32();
					float x = fields[2].GetFloat();
					float y = fields[3].GetFloat();
					float z = fields[4].GetFloat();
					float o = fields[5].GetFloat();

					PremiumTeamLocationElements& data5 = PremiumTeamLocations[team]; // like Lua table GWARZ[guild_id].entry
					data5.team = team;
					data5.map_id = map_id;
					data5.x = x;
					data5.y = y;
					data5.z = z;
					data5.o = o;

				} while (PremTeamLocQry->NextRow());

			}

			if (PremTeamLocQry){ TC_LOG_INFO("server.loading", "- Premium Team Locations Loaded"); };

			QueryResult PremPublicMallLocQry = WorldDatabase.Query("SELECT * FROM premium_Mall_locations");

			if (PremPublicMallLocQry)
			{
				do
				{
					Field *fields = PremPublicMallLocQry->Fetch();
					// Save the DB values to the LocData object
					uint8 id = fields[0].GetUInt8();
					uint32 map_id = fields[1].GetUInt32();
					float x = fields[2].GetFloat();
					float y = fields[3].GetFloat();
					float z = fields[4].GetFloat();
					float o = fields[5].GetFloat();

					PremiumLocationElements& data6 = PremiumPublicMallLocations[id]; // like Lua table GWARZ[guild_id].entry
					data6.id = id;
					data6.map_id = map_id;
					data6.x = x;
					data6.y = y;
					data6.z = z;
					data6.o = o;

				} while (PremPublicMallLocQry->NextRow());

			}


			TC_LOG_INFO("server.loading", "- Premium rank upgrade item id:%u", PREMIUM_UPGRADE_ITEM);
			TC_LOG_INFO("server.loading", "______________________________________");
		}

};

uint8 PREM::GetPremiumType()
{
	return PREMIUM_TYPE;
}

float PREM::GetPremiumModifier()
{
	return PREMIUM_MODIFIER;
}

bool PREM::IsPremiumTimed()
{
	return PREMIUM_TIMER_ENABLE != 0;
}

bool PREM::CanWaterBreathe()
{
	return PREMIUM_WATER_BREATHE != 0;
}

bool PREM::CanDecreaseSpellCost()
{
	return PREMIUM_SPELL_COST_DECREASE != 0;
}

uint64 PREM::GetPremiumDurationInSeconds()
{
	return PREMIUM_TIMER_DURATION;
}

uint32 PREM::GetPremiumDurationInDays()
{
	return ((((PREMIUM_TIMER_DURATION) / 60) / 60) / 24);
}

bool PREM::IsPremiumItemsEnabled()
{
	return PREMIUM_ITEMS_ENABLE != 0;
}

bool PREM::IsPremiumTalentPointBonusEnabled()
{
	return PREMIUM_TP_ENABLE != 0;
}

uint32 PREM::GetPremiumTalentPointBonus()
{
	return PREMIUM_TP_BONUS;
}

bool PREM::IsPremiumHealthPointBonusEnabled()
{
	return PREMIUM_HP_ENABLE != 0;
}

bool PREM::IsPremiumManaPointBonusEnabled()
{
	return PREMIUM_MANA_ENABLE != 0;
}

bool PREM::IsPremiumRagePointBonusEnabled()
{
	return PREMIUM_RAGE_ENABLE != 0;
}

// Player Functions

uint32 PREM::GetPlayerPremiumId(Player* player)
{
	uint32 id;

		if (PREMIUM_TYPE == 0)
		{
			id = player->GetSession()->GetAccountId();
		}
		else
		{
			id = player->GetSession()->GetGUIDLow();
		}

	return id;
}

bool GetPremiumChatTeam()
{
		return PREMIUM_CHAT_TEAM != 0;
}

class Premium_Reset_Timer : public BasicEvent
{
public:
	Premium_Reset_Timer(Player* player) : player(player)
	{
		uint32 id = PREM::GetPlayerPremiumId(player);

		uint64 current_time = sWorld->GetGameTime();
		uint64 player_premium_time = Premium[id].time;
		uint64 duration = PREM::GetPremiumDurationInSeconds();

		uint64 cycle_duration = ((player_premium_time + duration) - current_time);
		
		player->m_Events.AddEvent(this, player->m_Events.CalculateTime(cycle_duration * 1000)); // timed events are in ms while everything else is stored in seconds...
	}

	bool Execute(uint64, uint32) override
	{
		if (player->IsInWorld())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Your Premium rank has expired.");

			PREM::UpdatePlayerPremiumValue(player, 0, 0);
		}
		return true;
	}

	Player* player;
};

void PREM::AddPremiumToPlayer(Player* player)
{
	uint32 id = PREM::GetPlayerPremiumId(player);
	uint32 maxPower = PREM::IncreaseValueWithModifier(player, Premium[id].power_max);

	if (PREMIUM_TITLE_ID > 0)
	{
		if (!player->HasTitle(PREMIUM_TITLE_ID)){player->SetTitle(sCharTitlesStore.LookupEntry(PREMIUM_TITLE_ID), false); };

		player->SetUInt32Value(PLAYER_CHOSEN_TITLE, PREMIUM_TITLE_MASK_ID);
	}

	if (PREM::IsPremiumHealthPointBonusEnabled())
	{
		uint32 MaxHP = PREM::IncreaseValueWithModifier(player, Premium[id].hp);

		player->SetMaxHealth(MaxHP);
	}

	if ((player->getPowerType() == POWER_MANA) && (PREM::IsPremiumManaPointBonusEnabled()))	{ player->SetMaxPower(POWER_MANA, maxPower); };

	if ((player->getPowerType() == POWER_RAGE) && (PREM::IsPremiumRagePointBonusEnabled()))	{ player->SetMaxPower(POWER_RAGE, maxPower); };

	ChatHandler(player->GetSession()).PSendSysMessage("player:PremiumRankAdded.");
}

void PREM::RemovePremiumFromPlayer(Player* player)
{
	uint32 id = PREM::GetPlayerPremiumId(player);

	player->SetMaxHealth(Premium[id].hp);
	player->ResetTalents(false);

	if (PREMIUM_TITLE_ID > 0)
	{
		if (player->GetUInt32Value(PLAYER_CHOSEN_TITLE) == PREMIUM_TITLE_MASK_ID){ player->SetUInt32Value(PLAYER_CHOSEN_TITLE, -1); };
	}

	if (player->HasTitle(PREMIUM_TITLE_ID)){ player->SetTitle(sCharTitlesStore.LookupEntry(PREMIUM_TITLE_ID), true); };

	if ((player->getPowerType() == POWER_MANA) && (PREM::IsPremiumManaPointBonusEnabled()))	{ player->SetMaxPower(POWER_MANA, Premium[id].power_max); };

	if ((player->getPowerType() == POWER_RAGE) && (PREM::IsPremiumRagePointBonusEnabled()))	{ player->SetMaxPower(POWER_RAGE, Premium[id].power_max); };

	ChatHandler(player->GetSession()).PSendSysMessage("player:PremiumRankRemoved.");
}

bool PREM::IsPlayerPremium(Player* player)
{
	uint32 id = GetPlayerPremiumId(player);

	return Premium[id].premium != 0;
}

void PREM::UpdatePlayerCustomHomeTeleport(uint32 guid, uint32 map_id, float x, float y, float z, float o)
{
	WorldDatabase.PExecute("REPLACE INTO `premium_player_teleports` SET guid=%u, `map_id`='%u', `x`='%f', `y`='%f', `z`='%f', `o`='%f';", guid, map_id, x, y, z, o);

	PremiumPlayerLocationElements& data = PremiumPlayerLocations[guid];
	// Save the DB values to the MyData object
	data.guid = guid;
	data.map_id = map_id;
	data.x = x;
	data.y = y;
	data.z = z;
	data.o = o;

}

void PREM::UpdatePlayerPremiumValue(Player* player, uint8 value, uint64 time)
{
	uint32 id = GetPlayerPremiumId(player);

	if (PREMIUM_TYPE == 0)
	{
		LoginDatabase.PExecute("UPDATE `account` SET `premium`='%u', `premium_time`='%u' WHERE id=%u;", value, time, id);
	}
	if (PREMIUM_TYPE > 0)
	{
		CharacterDatabase.PExecute("UPDATE `characters` SET `premium`='%u', `premium_time`='%u' WHERE guid=%u;", value, time, id);
	}

	if (value == 0)
	{
		Premium[id].premium = 0;
		Premium[id].time = 0;
		PREM::RemovePremiumFromPlayer(player);
	}

	if (value > 0)
	{
		Premium[id].premium = 1;
		Premium[id].time = time;
		PREM::AddPremiumToPlayer(player);
	}
}

uint64 PREM::GetPlayerPremiumStartTimeInSeconds(Player* player)
{
	uint32 id = PREM::GetPlayerPremiumId(player);

	return Premium[id].time;
}

uint64 PREM::GetPlayerPremiumRemainingTimeInSeconds(Player* player)
{
	uint32 id = PREM::GetPlayerPremiumId(player);
	uint64 duration = PREM::GetPremiumDurationInSeconds();
	uint64 current_time = sWorld->GetGameTime();
	uint64 player_time = Premium[id].time;
	uint64 remaining_time = 0;

	remaining_time = ((player_time + duration) - current_time);

	return (remaining_time);
}

std::string PREM::GetPlayerPremiumTimeLeftInString(Player* player)
{
	uint64 remaining = PREM::GetPlayerPremiumRemainingTimeInSeconds(player);
	uint64 tmp;
	std::string output = "";

	if (remaining >= 2592000)
	{
		tmp = floor(remaining / 2592000); //  (((((remaining / 1000) / 60) / 60) / 24) / 30);
		remaining = remaining - (tmp * 2592000);
		output = output + PREM::ConvertNumberToString(tmp) + " Months, ";
	}

	if (remaining >= 86400)
	{
		tmp = floor(remaining / 86400); //  ((((remaining / 1000) / 60) / 60) / 24);
		remaining = remaining - (tmp * 86400);
		output = output + PREM::ConvertNumberToString(tmp) + " Days, ";
	}

	if (remaining >= 3600)
	{
		tmp = floor(remaining / 3600); //  (((remaining / 1000) / 60) / 60);
		remaining = remaining - (tmp * 3600);
		output = output + PREM::ConvertNumberToString(tmp) + " Hours, ";
	}

	if (remaining >= 60)
	{
		tmp = floor(remaining / 60); //  ((remaining / 1000) / 60);
		remaining = remaining - (tmp * 60);
		output = output + PREM::ConvertNumberToString(tmp) + " Minutes, ";
	}

	if (remaining < 60)
	{
		tmp = (remaining / 1);
		remaining = remaining - (tmp * 1);
		output = output + PREM::ConvertNumberToString(tmp) + " Seconds";
	}
	return output;
}

uint32 PREM::GetPlayerPremiumTimeInDays(Player* player)
{
	uint32 id = PREM::GetPlayerPremiumId(player);

	return (((Premium[id].time / 60) / 60) / 24);
}

void PREM::DepositGoldToPlayerGuildBank(Player* player, uint32 amount)
{
		Guild* guild = player->GetGuild();

		uint32 Deposit_Amount = ceil(amount * PREM::GetPremiumModifier());

		std::string money = PREM::GetAmountInString(Deposit_Amount);

		guild->HandleMemberDepositMoney(player->GetSession(), Deposit_Amount);

		ChatHandler(player->GetSession()).PSendSysMessage("%s of your loot get's deposited into your Guild's bank.", money.c_str());

		player->ModifyMoney((0 - Deposit_Amount));
}

uint32 PREM::IncreaseValueWithModifier(Player* player, uint32 value)
{
	if (PREM::IsPlayerPremium(player))
	{
		value += ceil(value * PREM::GetPremiumModifier());
	}
	return value;
}

uint32 PREM::DecreaseValueWithModifier(Player* player, uint32 value)
{
	if (PREM::IsPlayerPremium(player))
	{
		value -= ceil(value * PREM::GetPremiumModifier());
	}
	return value;
}

void SendPremiumMessage(std::string msg, uint8 team_id)
{
	SessionMap sessions = sWorld->GetAllSessions();

	for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
	{
			if (!itr->second)
				continue;

		Player *player = itr->second->GetPlayer();
		bool IsPlayerPremium = PREM::IsPlayerPremium(player);

			if ((player->IsGameMaster()) || (IsPlayerPremium && ((player->GetTeamId() == team_id) || ((player->GetTeamId() != team_id) && ((GetPremiumChatTeam()) || team_id == 2)))))
			{
				ChatHandler(player->GetSession()).PSendSysMessage(msg.c_str());
			}
	}
};

class Unit_Premium_Engine : public UnitScript
{
public:
	Unit_Premium_Engine() : UnitScript("Unit_Premium_Engine"){ };

	virtual void OnHeal(Unit* healer, Unit* reciever, uint32& gain)
	{
		Player* healer_player = healer->ToPlayer();
		Player* target_player = reciever->ToPlayer();

		if (healer_player)
		{
			gain = PREM::IncreaseValueWithModifier(healer_player, gain);
		}

		if (target_player)
		{
			gain = PREM::IncreaseValueWithModifier(target_player, gain);
		}
	}

	virtual void OnDamage(Unit* attacker, Unit* victim, uint32& damage)
	{
		Player* attacking_player = attacker->ToPlayer();
		Player* target_player = victim->ToPlayer();

		if (attacking_player)
		{
			damage = PREM::IncreaseValueWithModifier(attacking_player, damage);
		}

		if (target_player)
		{
			damage = PREM::DecreaseValueWithModifier(target_player, damage);
		}
	}

	virtual void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage)
	{
		Player* attacking_player = attacker->ToPlayer();
		Player* target_player = target->ToPlayer();

		if (attacking_player)
		{
			damage = PREM::IncreaseValueWithModifier(attacking_player, damage);
		}

		if (target_player)
		{
			damage = PREM::DecreaseValueWithModifier(target_player, damage);
		}
	}

	virtual void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage)
	{
		Player* attacking_player = attacker->ToPlayer();
		Player* target_player = target->ToPlayer();

		if (attacking_player)
		{
			damage = PREM::IncreaseValueWithModifier(attacking_player, damage);
		}

		if (target_player)
		{
			damage = PREM::DecreaseValueWithModifier(target_player, damage);
		}
	}
};

class Player_Premium_Engine : public PlayerScript
{
public: 
	Player_Premium_Engine() : PlayerScript("Player_Premium_Engine"){ };

		virtual void OnLogout(Player* player)
		{
			PREM::RemovePremiumFromPlayer(player);

			uint32 id = PREM::GetPlayerPremiumId(player);

			Premium.erase(id);
		};

		virtual void OnLogin(Player* player, bool /*firstLogin*/)
		{
			uint32 id = PREM::GetPlayerPremiumId(player);

			QueryResult PremiumQery;
			
				if (PREM::GetPremiumType() == 0)
				{ 
					PremiumQery = LoginDatabase.PQuery("SELECT premium, premium_time FROM account WHERE id=%u;", id);
				}
				else
				{ 
					PremiumQery = CharacterDatabase.PQuery("SELECT premium , premium_time FROM characters WHERE guid=%u;", id);
				}

				if (PremiumQery)
				{
					Field* fields = PremiumQery->Fetch();
					uint8 premium = fields[0].GetUInt8();
					uint32 premium_time = fields[1].GetUInt64();

					PremiumElements& data = Premium[id];
					// Save the values to the Data object. Build the players unordered_map table.
					data.id = id;
					data.premium = premium;
					data.time = premium_time;
					data.hp = player->GetMaxHealth();
					data.power_max = player->GetMaxPower(player->getPowerType());
					data.chat = 0;
					data.chat_time = 0;
				}
				else
				{
					if (PREM::GetPremiumType() == 0)
					{ 
						TC_LOG_INFO("server.loading", "PREMIUM_LOAD_ERROR ID:%u", id); 
					}
					else
					{ 
						TC_LOG_INFO("server.loading", "PREMIUM_LOAD_ERROR GUID:%u", id); 
					}
				}

				if (PREM::IsPlayerPremium(player))
				{
					ChatHandler(player->GetSession()).PSendSysMessage("Greetings %s. You are ranked Premium.", player->GetName());

					PREM::AddPremiumToPlayer(player);
				}
				else
				{
					ChatHandler(player->GetSession()).PSendSysMessage("Greetings %s.You can donate to recieve the Premium rank.", player->GetName());
	
					PREM::RemovePremiumFromPlayer(player);
				}
		} // On Login

		virtual void OnDuelEnd(Player* killer, Player* victim, DuelCompleteType /*type*/)
		{ // idea from Kaev
			if (PREM::IsPlayerPremium(killer))
			{
				killer->SetHealth(killer->GetMaxHealth());

					if (killer->getPowerType() == POWER_MANA){ killer->SetPower(POWER_MANA, killer->GetMaxPower(POWER_MANA)); };

					for (uint32 i = 0; i < sizeof(DEBUFFS) / sizeof(DEBUFFS); i++)
					{
						if (killer->HasAura(DEBUFFS[i])){ killer->RemoveAura(DEBUFFS[i]); };
					}
			}

			if (PREM::IsPlayerPremium(victim))
			{
				victim->SetHealth(victim->GetMaxHealth());

					if (victim->getPowerType() == POWER_MANA){ victim->SetPower(POWER_MANA, victim->GetMaxPower(POWER_MANA)); };

					for (uint32 i = 0; i < sizeof(DEBUFFS) / sizeof(DEBUFFS); i++)
					{
						if (victim->HasAura(DEBUFFS[i])){ victim->RemoveAura(DEBUFFS[i]); };
					}
			}
		} // Duel end

		virtual void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg)
		{
			uint64 current_time = sWorld->GetGameTime();
			uint32 id = PREM::GetPlayerPremiumId(player);
			std::string PCMSG = "";

			std::string ChannelColor = "|cff808080";
			std::string TeamColor[2] = { "|cff0080FF", "|cffCC0000" };

			if (player->IsGameMaster()) // here we will set the gm's stored values so they clear the checks.
			{
				Premium[id].time = current_time - PREMIUM_CHAT_DELAY;
				Premium[id].last_message = "";
			}

			if ((msg != "") && (lang != LANG_ADDON) && (msg != "Away") && (player->CanSpeak() == true) && (Premium[id].chat == 1))
			{

				if ((current_time < (Premium[id].time + PREMIUM_CHAT_DELAY)) || (Premium[id].last_message == msg))
				{
					ChatHandler(player->GetSession()).PSendSysMessage("-Spam detect triggered-");
				}
				else
				{
					Premium[id].last_message = msg;
					Premium[id].time = current_time;
					uint8 team_id = player->GetTeamId();

					PCMSG += "[" + ChannelColor + "Premium|r][" + TeamColor[team_id] + player->GetName() + "|r]";

					if (player->IsGameMaster())
					{ 
						PCMSG += "[GM]"; 
						team_id = 2;
					};

					PCMSG += ":" + msg;

					SendPremiumMessage(PCMSG, team_id);

					msg = -1;
				}
			}
		}

		virtual void OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/) 
		{
			amount = PREM::IncreaseValueWithModifier(player, amount);
		}

		virtual void OnReputationChange(Player* player, uint32 /*factionId*/, int32& standing, bool /*incremental*/)
		{
			standing = PREM::IncreaseValueWithModifier(player, standing);
		}
};

// Item Functions

void RemoveItem(uint32 id, Player* player)
{
	player->DestroyItemCount(uint32(id), 1, true);
}

bool PREM::IsItemPremium(Item* item)
{
	uint32 id = item->GetEntry();

	if (!PremiumItem[id].premium){ PremiumItem[id].premium = 0; };

	return PremiumItem[id].premium != 0;
}

void PREM::UpdateItemPremiumValue(uint32 item_id, uint8 value)
{
	WorldDatabase.PExecute("UPDATE `item_template` SET `premium`='%u' WHERE `entry`=%u;", value, item_id);
	PremiumItem[item_id].premium = value;
}

class Premium_Coin_Script : public ItemScript
{
public: 
	Premium_Coin_Script() : ItemScript("Premium_Coin_Script"){ };

		virtual bool OnUse(Player* player, Item* item, SpellCastTargets const& targets)
		{
			uint32 id = PREM::GetPlayerPremiumId(player);

				if (PREM::IsPlayerPremium(player))  // i may just remove this and let players update while they still are Premium ranked.
				{
					std::string output = PREM::GetPlayerPremiumTimeLeftInString(player);

					ChatHandler(player->GetSession()).PSendSysMessage("You allready have the Premium rank.");

					ChatHandler(player->GetSession()).PSendSysMessage("Your Premium Rank will expire in %s.", output.c_str());

				}
				else
				{
					RemoveItem(item->GetEntry(), player);

					PREM::UpdatePlayerPremiumValue(player, 1, sWorld->GetGameTime());

					ChatHandler(player->GetSession()).PSendSysMessage("Congratulations. You have been awarded the Premium Rank.");

					if (PREM::IsPremiumTimed())	
					{ 
						std::string output = PREM::GetPlayerPremiumTimeLeftInString(player);

						ChatHandler(player->GetSession()).PSendSysMessage("Your Premium Rank will expire in %u days.", output.c_str());
					}
				}
			return true;
		}
};

// Commands

bool CheckIfPlayerInCombatOrDead(Player* player)
{

	bool return_type = true;

	if (player->isFrozen())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("ermm .. your frozen...");

		return_type = false;
	}
	
	if (player->isDying())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("ermm .. your dying...");

		return_type = false;
	}

	if (player->isDead())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("ermm .. your dead...");

		return_type = false;
	}

	if (player->InArena())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("You are in an arena.");

		return_type = false;
	}

	if (player->IsInCombat())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("You are in combat.");

		return_type = false;
	}

	if (player->InBattleground())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("You are in a battleground.");

		return_type = false;
	}
	return return_type;
}

void TeleportPlayer(Player* player, uint8 id)
{
	bool IsPrem = PREM::IsPlayerPremium(player);
	auto team_id = player->GetTeamId();
	uint32 guid = player->GetGUID();

	if (!IsPrem)
	{
		ChatHandler(player->GetSession()).PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
	}
	else
	{
		if (CheckIfPlayerInCombatOrDead(player))
		{
			switch (id)
			{
				case(1) :
					if (PremiumLocations.size() != 2)
					{
						ChatHandler(player->GetSession()).PSendSysMessage("Under Construction.");
					}
					else
					{
						player->TeleportTo(PremiumLocations[team_id].map_id, PremiumLocations[team_id].x, PremiumLocations[team_id].y, PremiumLocations[team_id].z, PremiumLocations[team_id].o);
					}
				break;

				case(2) :
					if (PremiumMallLocations.size() != 2)
					{
						ChatHandler(player->GetSession()).PSendSysMessage("Under Construction.");
					}
					else
					{
						player->TeleportTo(PremiumMallLocations[team_id].map_id, PremiumMallLocations[team_id].x, PremiumMallLocations[team_id].y, PremiumMallLocations[team_id].z, PremiumMallLocations[team_id].o);
					}
				break;

				case(3) :
					if (PremiumPlayerLocations[guid].guid != 0)  // player customizable home gps.
					{
						player->TeleportTo(PremiumPlayerLocations[guid].map_id, PremiumPlayerLocations[guid].x, PremiumPlayerLocations[guid].y, PremiumPlayerLocations[guid].z, PremiumPlayerLocations[guid].o);
					}
					else
					{
						ChatHandler(player->GetSession()).PSendSysMessage("You must set a location as home first.");
					}
				break;

				case(4) :
					if (PremiumTeamLocations.size() != 2)
					{
						ChatHandler(player->GetSession()).PSendSysMessage("Under Construction.");
					}
					else
					{
						player->TeleportTo(PremiumTeamLocations[team_id].map_id, PremiumTeamLocations[team_id].x, PremiumTeamLocations[team_id].y, PremiumTeamLocations[team_id].z, PremiumTeamLocations[team_id].o);
					}
				break;

				case(5) :
					if (PremiumPublicMallLocations.size() != 2)
					{
						ChatHandler(player->GetSession()).PSendSysMessage("Under Construction.");
					}
					else
					{
						player->TeleportTo(PremiumPublicMallLocations[team_id].map_id, PremiumPublicMallLocations[team_id].x, PremiumPublicMallLocations[team_id].y, PremiumPublicMallLocations[team_id].z, PremiumPublicMallLocations[team_id].o);
					}
				break;
			}
		}
	}
}

class Premium_Commands : public CommandScript
{
public:
	Premium_Commands() : CommandScript("Premium_Commands") { }

	std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> SetPlayerPremiumCommandTable =
		{
			{ "on", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumSetPlayerRankTrueCommand, "allows the Admin to set a players Premium rank to 1." },
			{ "off", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumSetPlayerRankFalseCommand, "allows the Admin to set a players Premium rank to 0." },
		};

		static std::vector<ChatCommand> SetItemPremiumCommandTable =
		{
			{ "on", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumSetItemRankTrueCommand, "allows the Admin to set an items Premium rank to 1." },
			{ "off", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumSetItemRankFalseCommand, "allows the Admin to set an items Premium rank to 0." },
		};

		static std::vector<ChatCommand> SetPremiumCommandSetTable =
		{
			{ "player", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Player Premium set sub command tree.", SetPlayerPremiumCommandTable },
			{ "item", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Item Premium set sub command tree.", SetItemPremiumCommandTable },
		};

		static std::vector<ChatCommand> ResetPremiumCommandTable =
		{
			{ "tp", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumResetTPCommand, "allows the player to reset there talent points without cost." },
		};

		static std::vector<ChatCommand> SetPremiumCommandChangeTable =
		{
			{ "home", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumSetHomeTeleport, "allows the player to set there custom teleport location." },
			{ "drink", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumDrinkCommand, "allows the player to increase +5 to Drunk." },
			{ "drunk", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumDrunkCommand, "allows the player become 100 drunk." },
			{ "sober", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumSoborCommand, "allows the player to become sobor." },
			{ "premium", rbac::RBAC_PERM_COMMAND_SERVER, true, NULL, "Premium set sub command tree.", SetPremiumCommandSetTable },
		};

		static std::vector<ChatCommand> CharacterPremiumCommandChangeTable =
		{
			{ "race", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumChangeRaceCommand, "allows the player to change there race during next login." },
			{ "faction", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumChangeFactionCommand, "allows the player to change there faction during next login." },
			{ "customize", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumCustomizeCommand, "allows the player to re-costumize there character during next login." },
		};

		static std::vector<ChatCommand> ChatPremiumCommandChangeTable =
		{
			{ "on", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumChatOnCommand, "allows the player to set there /s channel to a Premium only chat channel.." },
			{ "off", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumChatOffCommand, "allows the player to reset there /say to normal function." },
		};

		static std::vector<ChatCommand> PremiumTeleportLocations =
		{
			{ "location", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumLocationTeleport, "allows the player to teleport to custom Premium only location." },
			{ "mall", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumMallTeleport, "allows the player to teleport to custom Premium mall location." },
		};

		static std::vector<ChatCommand> PremiumTeleportTable =
		{
			{ "home", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumTeleportHome, "allows the player to teleport to there personal custom location." },
			{ "base", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumTeleportBase, "allows the player to teleport to there team base location." },
			{ "mall", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumTeleportMall, "allows the player to teleport to a public mall custom location." },
			{ "premium", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium locations sub location tree.", PremiumTeleportLocations },
		};

		static std::vector<ChatCommand> PremiumCommandTable =
		{
			{ "chat", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium Chat sub command tree.", ChatPremiumCommandChangeTable },
			{ "buff", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumBuffCommand, "Premium Command used to Buff your character." },
			{ "maxskills", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumMaxSkillsCommand, "Premium Command used to repair all items without cost." },
			{ "morph", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumMorphPlayerCommand, "Premium Command used to morph." },
			{ "demorph", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumDeMorphPlayerCommand, "Premium Command used to demorph." },
			{ "repair", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumRepairCommand, "Premium Command used to repair all items without cost." },
			{ "reset", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium reset sub command tree.", ResetPremiumCommandTable },
			{ "teleport", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium set sub command tree.", PremiumTeleportTable },
			{ "set", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium set sub command tree.", SetPremiumCommandChangeTable },
			{ "character", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium Character customizing commands.", CharacterPremiumCommandChangeTable },
			{ "on", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumOnCommand, "Fast activate Premium Title." },
			{ "off", rbac::RBAC_IN_GRANTED_LIST, true, &HandlePremiumOffCommand, "Fast un-activate Premium Title." },
		};

		static std::vector<ChatCommand> commandTable =
		{
			{ "premium", rbac::RBAC_IN_GRANTED_LIST, true, NULL, "Premium custom commands.", PremiumCommandTable },
		};
		return commandTable;
	}

	static bool HandlePremiumRepairCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->DurabilityRepairAll(0, 0, false);
			handler->PSendSysMessage("Done.");
			return_type = true;
		}
		return return_type;
	}

	static bool HandlePremiumBuffCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;

		if (PREM::IsPlayerPremium(player))
		{
			for (uint8 i = 0; i < sizeof(BUFFS) / sizeof(*BUFFS); i++)
			{
				player->AddAura(BUFFS[i], player);

				return_type = true;
			}
		}
		else
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		return return_type;
	}

	static bool HandlePremiumResetTPCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;

		if (!PREM::IsPlayerPremium(player))
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->ResetTalents(true);

			uint32 tp = player->GetFreeTalentPoints();
			uint32 extratp = PREM::GetPremiumTalentPointBonus();

			player->SetFreeTalentPoints(tp + extratp);

			player->SendTalentsInfoData(false);

			ChatHandler(player->GetSession()).PSendSysMessage("Your Talent Points have been reset.");
			ChatHandler(player->GetSession()).PSendSysMessage("+ %utp.", extratp);

			return_type = true;
		}

		return return_type;
	}

	static bool HandlePremiumDrinkCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");

			return_type = false;
		}
		else
		{
			uint8 drink = player->GetDrunkValue();

			player->SetDrunkValue(drink + 5);

			handler->PSendSysMessage("!Down the hatch!");

			return_type = true;
		}

		return return_type;
	}

	static bool HandlePremiumDrunkCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");

			return_type = false;
		}
		else
		{
			player->SetDrunkValue(100);

			handler->PSendSysMessage("!You drunk!");

			return_type = true;
		}

		return return_type;
	}

	static bool HandlePremiumSoborCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");

			return_type = false;
		}
		else
		{
			player->SetDrunkValue(1);

			handler->PSendSysMessage("You're now sobor..");

			return_type = true;
		}

		return return_type;
	}

	static bool HandlePremiumChangeRaceCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
			handler->PSendSysMessage("Relog to change race of your character.");

			return_type = true;
		}
		return return_type;
	}

	static bool HandlePremiumChangeFactionCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
			handler->PSendSysMessage("Relog to change faction of your character.");
			return_type = true;
		}
		return return_type;
	}

	static bool HandlePremiumCustomizeCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
			handler->PSendSysMessage("Relog to customize your character.");

			return_type = true;
		}
		return return_type;
	}

	// Player commands

	static bool HandlePremiumChatOnCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem || !player->IsGameMaster())
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");

			return_type = false;
		}
		else
		{
			uint32 id = PREM::GetPlayerPremiumId(player);

			Premium[id].chat = true;
			Premium[id].chat_time = sWorld->GetGameTime() - PREMIUM_CHAT_DELAY;

			ChatHandler(player->GetSession()).PSendSysMessage("Premium Chat on.");
			ChatHandler(player->GetSession()).PSendSysMessage("now switch to `/s` and chat away.");
			return_type = true;
		}

		return return_type;
	}

	static bool HandlePremiumChatOffCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem || !player->IsGameMaster())
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");

			return_type = false;
		}
		else
		{
			uint32 id = PREM::GetPlayerPremiumId(player);

			Premium[id].chat = false;

			ChatHandler(player->GetSession()).PSendSysMessage("Premium Chat off.");

			return_type = true;
		}

		return return_type;
	}

	static bool HandlePremiumMorphPlayerCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->SetDisplayId((uint32)atol((char*)args));
			return_type = true;
		}
		return return_type;
	}

	static bool HandlePremiumDeMorphPlayerCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->RestoreDisplayId();

			return_type = true;
		}
		return return_type;
	}

	static bool HandlePremiumMaxSkillsCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			player->UpdateSkillsToMaxSkillsForLevel();

			handler->PSendSysMessage("Your skills have been maxed.");
			return_type = true;
		}
		return return_type;
	}

	static bool HandlePremiumOnCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			if (PREMIUM_TITLE_ID == 0)
			{
				handler->PSendSysMessage("The Admin hasn't added a custom title yet.");
	
				return_type = false;
			}
			else
			{
				if (!player->HasTitle(PREMIUM_TITLE_ID)){ player->SetTitle(sCharTitlesStore.LookupEntry(PREMIUM_TITLE_ID), false); };

				player->SetUInt32Value(PLAYER_CHOSEN_TITLE, PREMIUM_TITLE_MASK_ID);

				return_type = true;
			}
		}
		return return_type;
	}

	static bool HandlePremiumOffCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		bool return_type;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");

			return_type = false;
		}
		else
		{
			if (PREMIUM_TITLE_ID == 0)
			{
				handler->PSendSysMessage("The Admin hasn't added a custom title yet.");

				return_type = false;
			}
			else
			{
				player->SetUInt32Value(PLAYER_CHOSEN_TITLE, -1);

				return_type = true;
			}
		}
		return return_type;
	}

	// GM Player Commands

	static bool HandlePremiumSetPlayerRankTrueCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		Player* target = player->GetSelectedPlayer();
		uint64 game_time = sWorld->GetGameTime();

		if (player->GetSession()->GetSecurity() >= PREMIUM_GM_MINIMUM_RANK)
		{
			if (target)
			{
				PREM::UpdatePlayerPremiumValue(target, 1, game_time);

				ChatHandler(player->GetSession()).PSendSysMessage("You have added the Premium rank to %s.", target->GetName().c_str());
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Please target a player.");
			}
		}
		return true;
	}

	static bool HandlePremiumSetPlayerRankFalseCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		Player* target = player->GetSelectedPlayer();
		uint64 game_time = sWorld->GetGameTime();
		std::string arg = args;

		if (player->GetSession()->GetSecurity() >= PREMIUM_GM_MINIMUM_RANK)
		{
			if (target)
			{
				PREM::UpdatePlayerPremiumValue(target, 0, 0);

				ChatHandler(player->GetSession()).PSendSysMessage("You have removed %s`s Premium rank.", target->GetName().c_str());
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("Please target a player.");
			}
		}
		return true;
	}

	// GM Item Commands

	static bool HandlePremiumSetItemRankTrueCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		std::string arg = args;

		if (player->GetSession()->GetSecurity() >= PREMIUM_GM_MINIMUM_RANK)
		{
			if (arg != "")
			{
				PREM::UpdateItemPremiumValue(PREM::ConvertStringToNumber(arg), 1);
				ChatHandler(player->GetSession()).PSendSysMessage("Item %u set to %u", PREM::ConvertStringToNumber(arg), 1);
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("enter an item id.");
			}
		}
		return true;
	}

	static bool HandlePremiumSetItemRankFalseCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		std::string arg = args;

		if (player->GetSession()->GetSecurity() >= PREMIUM_GM_MINIMUM_RANK)
		{
			if (arg != "")
			{
				PREM::UpdateItemPremiumValue(PREM::ConvertStringToNumber(arg), 0);
				ChatHandler(player->GetSession()).PSendSysMessage("Item %u set to %u", PREM::ConvertStringToNumber(arg), 0);
			}
			else
			{
				ChatHandler(player->GetSession()).PSendSysMessage("enter an item id.");
			}
		}
		return true;
	}

	// Player Teleports

	static bool HandlePremiumLocationTeleport(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		TeleportPlayer(player, 1);

		return true;
	}

	static bool HandlePremiumMallTeleport(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		TeleportPlayer(player, 2);

		return true;
	}

	static bool HandlePremiumTeleportHome(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		TeleportPlayer(player, 3);

		return true;
	}

	static bool HandlePremiumTeleportBase(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		TeleportPlayer(player, 4);

		return true;
	}

	static bool HandlePremiumTeleportMall(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		TeleportPlayer(player, 5);

		return true;
	}

	static bool HandlePremiumSetHomeTeleport(ChatHandler* handler, const char* args)
	{
		// teleport to player custom location

		Player* player = handler->GetSession()->GetPlayer();

		bool return_type = true;
		bool IsPrem = PREM::IsPlayerPremium(player);

		if (!IsPrem)
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return_type = false;
		}
		else
		{
			PREM::UpdatePlayerCustomHomeTeleport(player->GetGUID(), player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());

			handler->PSendSysMessage("Location set.");
		}

		return return_type;
	}
};

void AddSC_Premium_System()
{
	new PREMIUM_Load_Conf;
	new Unit_Premium_Engine;
	new Player_Premium_Engine;
	new Premium_Coin_Script;
	new Premium_Commands;
}