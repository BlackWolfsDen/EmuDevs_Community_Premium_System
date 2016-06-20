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

#include "Premium_System.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "Config.h"
#include "Guild.h"
#include "Unit.h"
#include "World.h"

int Buffs[24] = { 24752, 48074, 43223, 36880, 467, 48469, 48162, 48170, 16877, 10220, 13033, 11735, 10952, 23948, 26662, 47440, 53307, 132, 23737, 48470, 43002, 26393, 24705, 69994 };
int Debuffs[4] = { 57724, 57723, 80354, 95809 };

PremiumSystemMgr::PremiumSystemMgr()
{
	premiumTimerDuration = (((1 * 60) * 60) * 24); // Defining day in ms.
}

PremiumSystemMgr::~PremiumSystemMgr()
{
	for (std::unordered_map<uint32, PremiumElements>::iterator itr = Premium.begin(); itr != Premium.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, PremiumItemElements>::iterator itr = PremiumItem.begin(); itr != PremiumItem.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, PremiumLocationElements>::iterator itr = PremiumLocations.begin(); itr != PremiumLocations.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, PremiumLocationElements>::iterator itr = PremiumMallLocations.begin(); itr != PremiumMallLocations.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, PremiumLocationElements>::iterator itr = PremiumPublicMallLocations.begin(); itr != PremiumPublicMallLocations.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, PremiumPlayerLocationElements>::iterator itr = PremiumPlayerLocations.begin(); itr != PremiumPlayerLocations.end(); ++itr)
		delete &itr->second;
	for (std::unordered_map<uint32, PremiumTeamLocationElements>::iterator itr = PremiumTeamLocations.begin(); itr != PremiumTeamLocations.end(); ++itr)
		delete &itr->second;

	Premium.clear();
	PremiumItem.clear();
	PremiumLocations.clear();
	PremiumMallLocations.clear();
	PremiumPublicMallLocations.clear();
	PremiumPlayerLocations.clear();
	PremiumTeamLocations.clear();
}

PremiumSystemMgr* PremiumSystemMgr::instance()
{
	static PremiumSystemMgr instance;
	return &instance;
}

void SendPremiumMessage(std::string msg, uint8 team_id)
{
	SessionMap sessions = sWorld->GetAllSessions();
	for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
	{
		if (!itr->second)
			continue;

		Player* player = itr->second->GetPlayer();
		if (!player)
			continue;

		if (player->IsGameMaster() || (sPremiumSystemMgr->IsPlayerPremium(player) && ((player->GetTeamId() == team_id) || ((player->GetTeamId() != team_id) && ((sPremiumSystemMgr->GetChatTeam()) || team_id == 2)))))
			ChatHandler(player->GetSession()).SendSysMessage(msg.c_str());
	}
}

uint64 PremiumSystemMgr::ConvertStringToNumber(std::string arg)
{
	uint64 Value64;

	std::istringstream(arg) >> Value64;

	return Value64;
}

std::string PremiumSystemMgr::ConvertNumberToString(uint64 numberX)
{
	auto number = numberX;
	std::stringstream convert;
	std::string number32_to_string;
	convert << number;
	number32_to_string = convert.str();

	return number32_to_string;
}

std::string PremiumSystemMgr::GetAmountInString(uint32 amount)
{
	uint64 tmp;
	std::string output = "";
	if (amount > 9999)
	{
		tmp = floor(amount / 10000);
		amount = amount - (tmp * 10000);
		output = output + ConvertNumberToString(tmp) + " Gold, ";
	}

	if (amount > 99)
	{
		tmp = floor(amount / 100);
		amount = amount - (tmp * 100);
		output = output + ConvertNumberToString(tmp) + " Silver, ";
	}

	if (amount <= 99)
	{
		tmp = (amount);
		amount = amount - (tmp);
		output = output + ConvertNumberToString(tmp) + " Copper";
	}

	return output;
}

uint32 PremiumSystemMgr::GetPlayerPremiumId(Player* player)
{
	if (GetPremiumType() == 0)
		return player->GetSession()->GetAccountId();
	else
		return player->GetSession()->GetGUIDLow();

	return 0;
}

void PremiumSystemMgr::AddPremiumToPlayer(Player* player)
{
	uint32 id = GetPlayerPremiumId(player);
	uint32 maxPower = IncreaseValueWithModifier(player, Premium[id].power_max);

	if (GetTitleId() > 0)
	{
		if (!player->HasTitle(GetTitleId()))
		{
			CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(GetTitleId());
			if (titleEntry)
				player->SetTitle(titleEntry, false);
		}

		player->SetUInt32Value(PLAYER_CHOSEN_TITLE, GetTitleMaskId());
	}

	if (IsHealthPointBonusEnabled())
	{
		uint32 MaxHP = IncreaseValueWithModifier(player, Premium[id].hp);

		player->SetMaxHealth(MaxHP);
	}

	if (player->getPowerType() == POWER_MANA && IsManaPointBonusEnabled())
		player->SetMaxPower(POWER_MANA, maxPower);

	if (player->getPowerType() == POWER_RAGE && IsRagePointBonusEnabled())
		player->SetMaxPower(POWER_RAGE, maxPower);

	ChatHandler(player->GetSession()).SendSysMessage("player:PremiumRankAdded.");
}

void PremiumSystemMgr::RemovePremiumFromPlayer(Player* player)
{
	uint32 id = GetPlayerPremiumId(player);

	player->SetMaxHealth(Premium[id].hp);
	player->ResetTalents(false);

	if (GetTitleId() > 0)
	{
		if (player->GetUInt32Value(PLAYER_CHOSEN_TITLE) == GetTitleMaskId())
			player->SetUInt32Value(PLAYER_CHOSEN_TITLE, -1);
	}

	if (player->HasTitle(GetTitleId()))
	{
		CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(GetTitleId());
		if (titleEntry)
			player->SetTitle(titleEntry, true);
	}

	if (player->getPowerType() == POWER_MANA && IsManaPointBonusEnabled())
		player->SetMaxPower(POWER_MANA, Premium[id].power_max);

	if (player->getPowerType() == POWER_RAGE && IsRagePointBonusEnabled())
		player->SetMaxPower(POWER_RAGE, Premium[id].power_max);

	DeletePremium(player);

	ChatHandler(player->GetSession()).SendSysMessage("player:PremiumRankRemoved.");
}

bool PremiumSystemMgr::IsPlayerPremium(Player* player)
{
	uint32 id = GetPlayerPremiumId(player);

	return Premium[id].premium != 0;
}

void PremiumSystemMgr::UpdatePlayerCustomHomeTeleport(uint32 guid, uint32 map_id, float x, float y, float z, float o)
{
	WorldDatabase.PExecute("REPLACE INTO `premium_player_teleports` SET guid=%u, `map_id`='%u', `x`='%f', `y`='%f', `z`='%f', `o`='%f';", guid, map_id, x, y, z, o);

	PremiumPlayerLocationElements& data = PremiumPlayerLocations[guid];
	data.guid = guid;
	data.map_id = map_id;
	data.x = x;
	data.y = y;
	data.z = z;
	data.o = o;
}

void PremiumSystemMgr::UpdatePlayerPremiumValue(Player* player, uint8 value, uint64 time)
{
	uint32 id = GetPlayerPremiumId(player);

	if (GetPremiumType() == 0)
		LoginDatabase.PExecute("UPDATE `account` SET `premium`='%u', `premium_time`='%u' WHERE id=%u;", value, time, id);
	else if (GetPremiumType() > 0)
		CharacterDatabase.PExecute("UPDATE `characters` SET `premium`='%u', `premium_time`='%u' WHERE guid=%u;", value, time, id);

	if (value == 0)
	{
		Premium[id].premium = 0;
		Premium[id].time = 0;
		RemovePremiumFromPlayer(player);
	}
	else if (value > 0)
	{
		Premium[id].premium = 1;
		Premium[id].time = time;
		AddPremiumToPlayer(player);
	}
}

uint64 PremiumSystemMgr::GetPlayerPremiumStartTimeInSeconds(Player* player)
{
	return Premium[GetPlayerPremiumId(player)].time;
}

uint64 PremiumSystemMgr::GetPlayerPremiumRemainingTimeInSeconds(Player* player)
{
	uint32 id = GetPlayerPremiumId(player);
	uint64 duration = GetDurationInSeconds();
	uint64 current_time = sWorld->GetGameTime();
	uint64 player_time = Premium[id].time;

	return ((player_time + duration) - current_time);
}

std::string PremiumSystemMgr::GetPlayerPremiumTimeLeftInString(Player* player)
{
	uint64 remaining = GetPlayerPremiumRemainingTimeInSeconds(player);
	uint64 tmp = 0;
	std::string output = "";

	if (remaining >= 2592000)
	{
		tmp = floor(remaining / 2592000); //  (((((remaining / 1000) / 60) / 60) / 24) / 30);
		remaining = remaining - (tmp * 2592000);
		output = output + ConvertNumberToString(tmp) + " Months, ";
	}

	if (remaining >= 86400)
	{
		tmp = floor(remaining / 86400); //  ((((remaining / 1000) / 60) / 60) / 24);
		remaining = remaining - (tmp * 86400);
		output = output + ConvertNumberToString(tmp) + " Days, ";
	}

	if (remaining >= 3600)
	{
		tmp = floor(remaining / 3600); //  (((remaining / 1000) / 60) / 60);
		remaining = remaining - (tmp * 3600);
		output = output + ConvertNumberToString(tmp) + " Hours, ";
	}

	if (remaining >= 60)
	{
		tmp = floor(remaining / 60); //  ((remaining / 1000) / 60);
		remaining = remaining - (tmp * 60);
		output = output + ConvertNumberToString(tmp) + " Minutes, ";
	}

	if (remaining < 60)
	{
		tmp = (remaining / 1);
		remaining = remaining - (tmp * 1);
		output = output + ConvertNumberToString(tmp) + " Seconds";
	}
	return output;
}

uint32 PremiumSystemMgr::GetPlayerPremiumTimeInDays(Player* player)
{
	uint32 id = GetPlayerPremiumId(player);

	return (((Premium[id].time / 60) / 60) / 24);
}

void PremiumSystemMgr::DepositGoldToPlayerGuildBank(Player* player, uint32 amount)
{
	Guild* guild = player->GetGuild();
	if (!guild)
		return;

	uint32 Deposit_Amount = ceil(amount * GetModifier());

	std::string money = GetAmountInString(Deposit_Amount);

	guild->HandleMemberDepositMoney(player->GetSession(), Deposit_Amount);

	ChatHandler(player->GetSession()).PSendSysMessage("%s of your loot get's deposited into your Guild's bank.", money.c_str());

	player->ModifyMoney((0 - Deposit_Amount));
}

uint32 PremiumSystemMgr::IncreaseValueWithModifier(Player* player, uint32 value)
{
	if (IsPlayerPremium(player))
		value += ceil(value * GetModifier());

	return value;
}

uint32 PremiumSystemMgr::DecreaseValueWithModifier(Player* player, uint32 value)
{
	if (IsPlayerPremium(player))
		value -= ceil(value * GetModifier());

	return value;
}

void PremiumSystemMgr::RemoveItem(uint32 id, Player* player)
{
	player->DestroyItemCount(uint32(id), 1, true);
}

bool PremiumSystemMgr::IsItemPremium(Item* item)
{
	if (!item)
		return false;

	uint32 id = item->GetEntry();

	if (!PremiumItem[id].premium)
		PremiumItem[id].premium = 0;

	return PremiumItem[id].premium != 0;
}

void PremiumSystemMgr::UpdateItemPremiumValue(uint32 item_id, uint8 value)
{
	WorldDatabase.PExecute("UPDATE `item_template` SET `premium`='%u' WHERE `entry`=%u;", value, item_id);
	PremiumItem[item_id].premium = value;
}

bool PremiumSystemMgr::CheckIfPlayerInCombatOrDead(Player* player)
{
	if (player->isFrozen())
	{
		ChatHandler(player->GetSession()).SendSysMessage("Erm, you're frozen!");
		return false;
	}

	if (player->isDying())
	{
		ChatHandler(player->GetSession()).SendSysMessage("Erm, you're dying!");
		return false;
	}

	if (player->isDead())
	{
		ChatHandler(player->GetSession()).SendSysMessage("Erm, you're dead!");
		return false;
	}

	if (player->InArena())
	{
		ChatHandler(player->GetSession()).SendSysMessage("You are in an arena.");
		return false;
	}

	if (player->IsInCombat())
	{
		ChatHandler(player->GetSession()).SendSysMessage("You are in combat.");

		return false;
	}

	if (player->InBattleground())
	{
		ChatHandler(player->GetSession()).SendSysMessage("You are in a battleground.");
		return false;
	}

	return true;
}

void PremiumSystemMgr::TeleportPlayer(Player* player, uint8 id)
{
	auto team_id = player->GetTeamId();
	uint32 guid = player->GetGUID();

	if (!IsPlayerPremium(player))
		ChatHandler(player->GetSession()).SendSysMessage("You don't have a Premium rank. You must have a Premium rank to use this command.");
	else
	{
		if (CheckIfPlayerInCombatOrDead(player))
		{
			switch (id)
			{
			case 1:
				if (PremiumLocations.size() != 2)
					ChatHandler(player->GetSession()).SendSysMessage("Under Construction.");
				else
					player->TeleportTo(PremiumLocations[team_id].map_id, PremiumLocations[team_id].x, PremiumLocations[team_id].y, PremiumLocations[team_id].z, PremiumLocations[team_id].o);
				break;
			case 2:
				if (PremiumMallLocations.size() != 2)
					ChatHandler(player->GetSession()).SendSysMessage("Under Construction.");
				else
					player->TeleportTo(PremiumMallLocations[team_id].map_id, PremiumMallLocations[team_id].x, PremiumMallLocations[team_id].y, PremiumMallLocations[team_id].z, PremiumMallLocations[team_id].o);
				break;
			case 3:
				if (PremiumPlayerLocations[guid].guid != 0)
					player->TeleportTo(PremiumPlayerLocations[guid].map_id, PremiumPlayerLocations[guid].x, PremiumPlayerLocations[guid].y, PremiumPlayerLocations[guid].z, PremiumPlayerLocations[guid].o);
				else
					ChatHandler(player->GetSession()).SendSysMessage("You must set a location as home first.");
				break;
			case 4:
				if (PremiumTeamLocations.size() != 2)
					ChatHandler(player->GetSession()).SendSysMessage("Under Construction.");
				else
					player->TeleportTo(PremiumTeamLocations[team_id].map_id, PremiumTeamLocations[team_id].x, PremiumTeamLocations[team_id].y, PremiumTeamLocations[team_id].z, PremiumTeamLocations[team_id].o);
				break;
			case 5:
				if (PremiumPublicMallLocations.size() != 2)
					ChatHandler(player->GetSession()).SendSysMessage("Under Construction.");
				else
					player->TeleportTo(PremiumPublicMallLocations[team_id].map_id, PremiumPublicMallLocations[team_id].x, PremiumPublicMallLocations[team_id].y, PremiumPublicMallLocations[team_id].z, PremiumPublicMallLocations[team_id].o);
				break;
			}
		}
	}
}

void PremiumSystemMgr::DeletePremium(Player* player)
{
	if (!player)
		return;

	Premium.erase(GetPlayerPremiumId(player));
}

class PremiumSystemConf : public WorldScript
{
public:
	PremiumSystemConf() : WorldScript("PremiumSystemConf") { }

	void OnConfigLoad(bool /*reload*/) override
	{
		TC_LOG_INFO("server.loading", "Loading EmuDevs.com Premium System");

		sPremiumSystemMgr->SetPremiumType(sConfigMgr->GetBoolDefault("PREM.TYPE", false));          // 0 = acct, 1 = character.
		sPremiumSystemMgr->SetTimerEnabled(sConfigMgr->GetBoolDefault("PREM.TIMED", false));        // 0 = no, 1 = yes duration - Premium will reset after x days..
		sPremiumSystemMgr->SetTimerDuration(sConfigMgr->GetIntDefault("PREM.DURATION", 30));        // x in days.
		sPremiumSystemMgr->SetGMMinRank(sConfigMgr->GetIntDefault("PREM.GM_MINIMUM_RANK", 3));
		sPremiumSystemMgr->SetUpgradeItem(sConfigMgr->GetIntDefault("PREM.UPGRADE_ITEM", 64000));
		sPremiumSystemMgr->SetItemEnabled(sConfigMgr->GetBoolDefault("PREM.ITEMS", false));
		sPremiumSystemMgr->SetModifier(sConfigMgr->GetFloatDefault("PREM.MODIFIER", 0.2f));
		sPremiumSystemMgr->SetChatDelay(sConfigMgr->GetIntDefault("PREM.CHAT_TIMER", 5));           // in seconds, 5 = 5 seconds.
		sPremiumSystemMgr->SetChatTeam(sConfigMgr->GetIntDefault("PREM.CHAT_TEAM", 0));             // 0 = team chat only, 1 world chat
		sPremiumSystemMgr->SetTPEnabled(sConfigMgr->GetBoolDefault("PREM.TP_ENABLE", false));
		sPremiumSystemMgr->SetTPBonus(sConfigMgr->GetIntDefault("PREM.TP_BONUS", 14));
		sPremiumSystemMgr->SetHPEnabled(sConfigMgr->GetBoolDefault("PREM.HP_ENABLE", false));
		sPremiumSystemMgr->SetManaEnabled(sConfigMgr->GetBoolDefault("PREM.MANA_ENABLE", false));
		sPremiumSystemMgr->SetRageEnabled(sConfigMgr->GetBoolDefault("PREM.RAGE_ENABLE", false));
		sPremiumSystemMgr->SetTitleId(sConfigMgr->GetIntDefault("PREM.TITLE_ID", 500));
		sPremiumSystemMgr->SetTitleMaskId(sConfigMgr->GetIntDefault("PREM.TITLE_MASK_ID", 156));
		sPremiumSystemMgr->SetWaterBreathe(sConfigMgr->GetIntDefault("PREM.WATER_BREATHE", 0));
		sPremiumSystemMgr->SetSpellCostDecrease(sConfigMgr->GetIntDefault("PREM.SPELL_COST_DECREASE", 0));

		if (sPremiumSystemMgr->IsItemsEnabled())
		{
			QueryResult ItemQery = WorldDatabase.PQuery("SELECT `entry`, `premium` FROM `item_template` WHERE premium='1'");
			if (ItemQery)
			{
				do
				{
					Field* fields = ItemQery->Fetch();
					uint32 item_id = fields[0].GetUInt32();
					uint32 premium = fields[1].GetUInt8();

					PremiumItemElements& data1 = sPremiumSystemMgr->PremiumItem[item_id];
					data1.id = item_id;
					data1.premium = premium;

				} while (ItemQery->NextRow());
			}
		}

		QueryResult PremLocQry = WorldDatabase.Query("SELECT * FROM premium_locations");
		if (PremLocQry)
		{
			do
			{
				Field *fields = PremLocQry->Fetch();
				uint8 id = fields[0].GetUInt8();
				uint32 map_id = fields[1].GetUInt32();
				float x = fields[2].GetFloat();
				float y = fields[3].GetFloat();
				float z = fields[4].GetFloat();
				float o = fields[5].GetFloat();

				PremiumLocationElements& data2 = sPremiumSystemMgr->PremiumLocations[id];
				data2.id = id;
				data2.map_id = map_id;
				data2.x = x;
				data2.y = y;
				data2.z = z;
				data2.o = o;

			} while (PremLocQry->NextRow());
		}

		QueryResult PremMallLocQry = WorldDatabase.Query("SELECT * FROM premium_Mall_locations");
		if (PremMallLocQry)
		{
			do
			{
				Field *fields = PremMallLocQry->Fetch();
				uint8 id = fields[0].GetUInt8();
				uint32 map_id = fields[1].GetUInt32();
				float x = fields[2].GetFloat();
				float y = fields[3].GetFloat();
				float z = fields[4].GetFloat();
				float o = fields[5].GetFloat();

				PremiumLocationElements& data3 = sPremiumSystemMgr->PremiumMallLocations[id];
				data3.id = id;
				data3.map_id = map_id;
				data3.x = x;
				data3.y = y;
				data3.z = z;
				data3.o = o;

			} while (PremMallLocQry->NextRow());
		}

		QueryResult PremPlayerLocQry = WorldDatabase.Query("SELECT * FROM premium_player_teleports");
		if (PremPlayerLocQry)
		{
			do
			{
				Field *fields = PremPlayerLocQry->Fetch();
				uint32 guid = fields[0].GetUInt32();
				uint32 map_id = fields[1].GetUInt32();
				float x = fields[2].GetFloat();
				float y = fields[3].GetFloat();
				float z = fields[4].GetFloat();
				float o = fields[5].GetFloat();

				PremiumPlayerLocationElements& data4 = sPremiumSystemMgr->PremiumPlayerLocations[guid];
				data4.guid = guid;
				data4.map_id = map_id;
				data4.x = x;
				data4.y = y;
				data4.z = z;
				data4.o = o;

			} while (PremPlayerLocQry->NextRow());
		}

		QueryResult PremTeamLocQry = WorldDatabase.Query("SELECT * FROM premium_team_teleports");
		if (PremTeamLocQry)
		{
			do
			{
				Field *fields = PremTeamLocQry->Fetch();
				uint32 team = fields[0].GetUInt32();
				uint32 map_id = fields[1].GetUInt32();
				float x = fields[2].GetFloat();
				float y = fields[3].GetFloat();
				float z = fields[4].GetFloat();
				float o = fields[5].GetFloat();

				PremiumTeamLocationElements& data5 = sPremiumSystemMgr->PremiumTeamLocations[team];
				data5.team = team;
				data5.map_id = map_id;
				data5.x = x;
				data5.y = y;
				data5.z = z;
				data5.o = o;

			} while (PremTeamLocQry->NextRow());
		}

		QueryResult PremPublicMallLocQry = WorldDatabase.Query("SELECT * FROM premium_Mall_locations");
		if (PremPublicMallLocQry)
		{
			do
			{
				Field *fields = PremPublicMallLocQry->Fetch();
				uint8 id = fields[0].GetUInt8();
				uint32 map_id = fields[1].GetUInt32();
				float x = fields[2].GetFloat();
				float y = fields[3].GetFloat();
				float z = fields[4].GetFloat();
				float o = fields[5].GetFloat();

				PremiumLocationElements& data6 = sPremiumSystemMgr->PremiumPublicMallLocations[id];
				data6.id = id;
				data6.map_id = map_id;
				data6.x = x;
				data6.y = y;
				data6.z = z;
				data6.o = o;

			} while (PremPublicMallLocQry->NextRow());
		}

		TC_LOG_INFO("server.loading", "Loading Premium System Completed. Premium rank upgrade item id: %u", sPremiumSystemMgr->GetUpgradeItem());
	}
};

class Premium_Reset_Timer : public BasicEvent
{
public:
	Premium_Reset_Timer(Player* player) : player(player)
	{
		uint32 id = sPremiumSystemMgr->GetPlayerPremiumId(player);
		uint64 current_time = sWorld->GetGameTime();
		uint64 player_premium_time = sPremiumSystemMgr->Premium[id].time;
		uint64 duration = sPremiumSystemMgr->GetDurationInSeconds();
		uint64 cycle_duration = ((player_premium_time + duration) - current_time);

		player->m_Events.AddEvent(this, player->m_Events.CalculateTime(cycle_duration * 1000));
	}

	bool Execute(uint64, uint32) override
	{
		if (player->IsInWorld())
		{
			ChatHandler(player->GetSession()).SendSysMessage("Your Premium rank has expired.");
			sPremiumSystemMgr->UpdatePlayerPremiumValue(player, 0, 0);
		}

		return true;
	}

private:
	Player* player;
};

class Premium_Coin_Script : public ItemScript
{
public:
	Premium_Coin_Script() : ItemScript("Premium_Coin_Script") { }

	bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
	{
		uint32 id = sPremiumSystemMgr->GetPlayerPremiumId(player);

		if (sPremiumSystemMgr->IsPlayerPremium(player))  // @TODO: I may just remove this and let players update while they still are Premium ranked.
		{
			std::string output = sPremiumSystemMgr->GetPlayerPremiumTimeLeftInString(player);
			ChatHandler(player->GetSession()).SendSysMessage("You allready have the Premium rank.");
			ChatHandler(player->GetSession()).PSendSysMessage("Your Premium Rank will expire in %s.", output.c_str());
		}
		else
		{
			sPremiumSystemMgr->RemoveItem(item->GetEntry(), player);
			sPremiumSystemMgr->UpdatePlayerPremiumValue(player, 1, sWorld->GetGameTime());

			ChatHandler(player->GetSession()).SendSysMessage("Congratulations! You have been awarded the Premium Rank.");

			if (sPremiumSystemMgr->IsTimed())
			{
				std::string output = sPremiumSystemMgr->GetPlayerPremiumTimeLeftInString(player);
				ChatHandler(player->GetSession()).PSendSysMessage("Your Premium Rank will expire in %u days.", output.c_str());
			}
		}

		return true;
	}
};

class Player_Premium_Engine : public PlayerScript
{
public:
	Player_Premium_Engine() : PlayerScript("Player_Premium_Engine") { }

	void OnLogout(Player* player) override
	{
		sPremiumSystemMgr->RemovePremiumFromPlayer(player);
	}

	void OnLogin(Player* player, bool /*firstLogin*/) override
	{
		uint32 id = sPremiumSystemMgr->GetPlayerPremiumId(player);

		QueryResult premiumQuery = nullptr;

		if (sPremiumSystemMgr->GetPremiumType() == 0)
			premiumQuery = LoginDatabase.PQuery("SELECT premium, premium_time FROM account WHERE id=%u", id);
		else
			premiumQuery = CharacterDatabase.PQuery("SELECT premium , premium_time FROM characters WHERE guid=%u", id);

		if (premiumQuery)
		{
			Field* fields = premiumQuery->Fetch();
			uint8 premium = fields[0].GetUInt8();
			uint32 premium_time = fields[1].GetUInt64();

			PremiumElements& data = sPremiumSystemMgr->Premium[id];
			data.id = id;
			data.premium = premium;
			data.time = premium_time;
			data.hp = player->GetMaxHealth();
			data.power_max = player->GetMaxPower(player->getPowerType());
			data.chat = 0;
			data.chat_time = 0;
		}

		if (sPremiumSystemMgr->IsPlayerPremium(player))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Greetings %s. You are ranked Premium.", player->GetName());
			sPremiumSystemMgr->AddPremiumToPlayer(player);
		}
		else
		{
			ChatHandler(player->GetSession()).PSendSysMessage("Greetings %s.You can donate to recieve the Premium rank.", player->GetName());
			sPremiumSystemMgr->RemovePremiumFromPlayer(player);
		}
	}

	void OnDuelEnd(Player* killer, Player* victim, DuelCompleteType /*type*/) override
	{
		if (sPremiumSystemMgr->IsPlayerPremium(killer))
		{
			killer->SetHealth(killer->GetMaxHealth());

			if (killer->getPowerType() == POWER_MANA)
				killer->SetPower(POWER_MANA, killer->GetMaxPower(POWER_MANA));

			for (uint32 i = 0; i < sizeof(Debuffs) / sizeof(Debuffs); i++)
			{
				if (killer->HasAura(Debuffs[i]))
					killer->RemoveAura(Debuffs[i]);
			}
		}

		if (sPremiumSystemMgr->IsPlayerPremium(victim))
		{
			victim->SetHealth(victim->GetMaxHealth());

			if (victim->getPowerType() == POWER_MANA)
				victim->SetPower(POWER_MANA, victim->GetMaxPower(POWER_MANA));

			for (uint32 i = 0; i < sizeof(Debuffs) / sizeof(Debuffs); i++)
			{
				if (victim->HasAura(Debuffs[i]))
					victim->RemoveAura(Debuffs[i]);
			}
		}
	}

	void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg) override
	{
		uint64 current_time = sWorld->GetGameTime();
		PremiumElements premium = sPremiumSystemMgr->Premium[sPremiumSystemMgr->GetPlayerPremiumId(player)];
		std::string PCMSG = "";

		std::string ChannelColor = "|cff808080";
		std::string TeamColor[2] = { "|cff0080FF", "|cffCC0000" };

		if (player->IsGameMaster())
		{
			premium.time = current_time - sPremiumSystemMgr->GetChatDelay();
			premium.last_message = "";
		}

		if ((msg != "" || msg != "Away") && lang != LANG_ADDON && player->CanSpeak() && premium.chat == 1)
		{
			if (current_time < (premium.time + sPremiumSystemMgr->GetChatDelay()) || premium.last_message == msg)
				ChatHandler(player->GetSession()).SendSysMessage("Spam detect triggered");
			else
			{
				premium.last_message = msg;
				premium.time = current_time;
				uint8 team_id = player->GetTeamId();

				PCMSG += "[" + ChannelColor + "Premium|r][" + TeamColor[team_id] + player->GetName() + "|r]";

				if (player->IsGameMaster())
				{
					PCMSG += "[GM]";
					team_id = 2;
				}

				PCMSG += ":" + msg;

				SendPremiumMessage(PCMSG, team_id);

				msg = "";
			}
		}
	}

	void OnGiveXP(Player* player, uint32& amount, Unit* /*victim*/) override
	{
		amount = sPremiumSystemMgr->IncreaseValueWithModifier(player, amount);
	}

	void OnReputationChange(Player* player, uint32 /*factionId*/, int32& standing, bool /*incremental*/) override
	{
		standing = sPremiumSystemMgr->IncreaseValueWithModifier(player, standing);
	}
};

class Unit_Premium_Engine : public UnitScript
{
public:
	Unit_Premium_Engine() : UnitScript("Unit_Premium_Engine") { }

	void OnHeal(Unit* healer, Unit* reciever, uint32& gain) override
	{
		Player* healerPlayer = healer->ToPlayer();
		if (healerPlayer)
			sPremiumSystemMgr->IncreaseValueWithModifier(healerPlayer, gain);

		Player* target = reciever->ToPlayer();
		if (target)
			gain = sPremiumSystemMgr->IncreaseValueWithModifier(target, gain);
	}

	void OnDamage(Unit* attacker, Unit* victim, uint32& damage) override
	{
		Player* attackerPlayer = attacker->ToPlayer();
		if (attackerPlayer)
			damage = sPremiumSystemMgr->IncreaseValueWithModifier(attackerPlayer, damage);

		Player* target = victim->ToPlayer();
		if (target)
			damage = sPremiumSystemMgr->DecreaseValueWithModifier(target, damage);
	}

	void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override
	{
		Player* attackerPlayer = attacker->ToPlayer();
		if (attackerPlayer)
			damage = sPremiumSystemMgr->IncreaseValueWithModifier(attackerPlayer, damage);

		Player* targetPlayer = target->ToPlayer();
		if (targetPlayer)
			damage = sPremiumSystemMgr->DecreaseValueWithModifier(targetPlayer, damage);
	}

	void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage) override
	{
		Player* attackerPlayer = attacker->ToPlayer();
		if (attackerPlayer)
			damage = sPremiumSystemMgr->IncreaseValueWithModifier(attackerPlayer, damage);

		Player* targetPlayer = target->ToPlayer();
		if (targetPlayer)
			damage = sPremiumSystemMgr->DecreaseValueWithModifier(targetPlayer, damage);
	}
};

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

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->DurabilityRepairAll(0, 0, false);
		handler->SendSysMessage("Done.");

		return true;
	}

	static bool HandlePremiumBuffCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		for (uint8 i = 0; i < sizeof(Buffs) / sizeof(*Buffs); i++)
			player->AddAura(Buffs[i], player);

		return true;
	}

	static bool HandlePremiumResetTPCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->ResetTalents(true);

		uint32 tp = player->GetFreeTalentPoints();
		uint32 extratp = sPremiumSystemMgr->GetTalentPointBonus();

		player->SetFreeTalentPoints(tp + extratp);

		player->SendTalentsInfoData(false);

		ChatHandler(player->GetSession()).SendSysMessage("Your Talent Points have been reset.");
		ChatHandler(player->GetSession()).PSendSysMessage("+ %utp.", extratp);

		return true;
	}

	static bool HandlePremiumDrinkCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		uint8 drink = player->GetDrunkValue() + 5;
		player->SetDrunkValue(drink);

		handler->SendSysMessage("Down the hatch!");

		return true;
	}

	static bool HandlePremiumDrunkCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->SetDrunkValue(100);
		handler->SendSysMessage("You're drunk!");

		return true;
	}

	static bool HandlePremiumSoborCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->SetDrunkValue(1);
		handler->SendSysMessage("You're now sobor..");

		return true;
	}

	static bool HandlePremiumChangeRaceCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
		handler->SendSysMessage("Relog to change race of your character.");

		return true;
	}

	static bool HandlePremiumChangeFactionCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
		handler->SendSysMessage("Relog to change faction of your character.");

		return true;
	}

	static bool HandlePremiumCustomizeCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
		handler->SendSysMessage("Relog to customize your character.");

		return true;
	}

	static bool HandlePremiumChatOnCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player) || !player->IsGameMaster())
		{
			handler->PSendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		PremiumElements premium = sPremiumSystemMgr->Premium[sPremiumSystemMgr->GetPlayerPremiumId(player)];
		premium.chat = true;
		premium.chat_time = sWorld->GetGameTime() - sPremiumSystemMgr->GetChatDelay();

		handler->SendSysMessage("Premium Chat on.");
		handler->SendSysMessage("now switch to `/s` and chat away.");

		return true;
	}

	static bool HandlePremiumChatOffCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player) || !player->IsGameMaster())
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		uint32 id = sPremiumSystemMgr->GetPlayerPremiumId(player);
		sPremiumSystemMgr->Premium[id].chat = false;

		ChatHandler(player->GetSession()).SendSysMessage("Premium Chat off.");

		return false;
	}

	static bool HandlePremiumMorphPlayerCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->SetDisplayId((uint32)atol((char*)args));

		return true;
	}

	static bool HandlePremiumDeMorphPlayerCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->RestoreDisplayId();

		return true;
	}

	static bool HandlePremiumMaxSkillsCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		player->UpdateSkillsToMaxSkillsForLevel();

		handler->SendSysMessage("Your skills have been maxed.");

		return true;
	}

	static bool HandlePremiumOnCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		if (sPremiumSystemMgr->GetTitleId() == 0)
		{
			handler->SendSysMessage("The Admin hasn't added a custom title yet.");

			return false;
		}

		if (!player->HasTitle(sPremiumSystemMgr->GetTitleId()))
			player->SetTitle(sCharTitlesStore.LookupEntry(sPremiumSystemMgr->GetTitleId()), false);

		player->SetUInt32Value(PLAYER_CHOSEN_TITLE, sPremiumSystemMgr->GetTitleMaskId());

		return true;
	}

	static bool HandlePremiumOffCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		if (sPremiumSystemMgr->GetTitleId() == 0)
		{
			handler->PSendSysMessage("The Admin hasn't added a custom title yet.");
			return false;
		}

		player->SetUInt32Value(PLAYER_CHOSEN_TITLE, -1);

		return true;
	}

	static bool HandlePremiumSetPlayerRankTrueCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		Player* target = player->GetSelectedPlayer();
		uint64 game_time = sWorld->GetGameTime();

		if (player->GetSession()->GetSecurity() >= sPremiumSystemMgr->GetGMMinRank())
		{
			if (target)
			{
				sPremiumSystemMgr->UpdatePlayerPremiumValue(target, 1, game_time);
				ChatHandler(player->GetSession()).PSendSysMessage("You have added the Premium rank to %s.", target->GetName().c_str());
			}
			else
				ChatHandler(player->GetSession()).SendSysMessage("Please target a player.");
		}

		return true;
	}

	static bool HandlePremiumSetPlayerRankFalseCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		Player* target = player->GetSelectedPlayer();
		uint64 game_time = sWorld->GetGameTime();
		std::string arg = args;

		if (player->GetSession()->GetSecurity() >= sPremiumSystemMgr->GetGMMinRank())
		{
			if (target)
			{
				sPremiumSystemMgr->UpdatePlayerPremiumValue(target, 0, 0);
				ChatHandler(player->GetSession()).PSendSysMessage("You have removed %s`s Premium rank.", target->GetName().c_str());
			}
			else
				ChatHandler(player->GetSession()).SendSysMessage("Please target a player.");
		}

		return true;
	}

	static bool HandlePremiumSetItemRankTrueCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		std::string arg = args;

		if (player->GetSession()->GetSecurity() >= sPremiumSystemMgr->GetGMMinRank())
		{
			if (arg != "")
			{
				sPremiumSystemMgr->UpdateItemPremiumValue(sPremiumSystemMgr->ConvertStringToNumber(arg), 1);
				ChatHandler(player->GetSession()).PSendSysMessage("Item %u set to %u", sPremiumSystemMgr->ConvertStringToNumber(arg), 1);
			}
			else
				ChatHandler(player->GetSession()).SendSysMessage("enter an item id.");
		}

		return true;
	}

	static bool HandlePremiumSetItemRankFalseCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();
		std::string arg = args;

		if (player->GetSession()->GetSecurity() >= sPremiumSystemMgr->GetGMMinRank())
		{
			if (arg != "")
			{
				sPremiumSystemMgr->UpdateItemPremiumValue(sPremiumSystemMgr->ConvertStringToNumber(arg), 0);
				ChatHandler(player->GetSession()).PSendSysMessage("Item %u set to %u", sPremiumSystemMgr->ConvertStringToNumber(arg), 0);
			}
			else
				ChatHandler(player->GetSession()).SendSysMessage("Enter an item id!");
		}

		return true;
	}

	static bool HandlePremiumLocationTeleport(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		sPremiumSystemMgr->TeleportPlayer(player, 1);

		return true;
	}

	static bool HandlePremiumMallTeleport(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		sPremiumSystemMgr->TeleportPlayer(player, 2);

		return true;
	}

	static bool HandlePremiumTeleportHome(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		sPremiumSystemMgr->TeleportPlayer(player, 3);

		return true;
	}

	static bool HandlePremiumTeleportBase(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		sPremiumSystemMgr->TeleportPlayer(player, 4);

		return true;
	}

	static bool HandlePremiumTeleportMall(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		sPremiumSystemMgr->TeleportPlayer(player, 5);

		return true;
	}

	static bool HandlePremiumSetHomeTeleport(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!sPremiumSystemMgr->IsPlayerPremium(player))
		{
			handler->SendSysMessage("You dont have the Premium rank. You must have the Premium rank to use this command.");
			return false;
		}

		sPremiumSystemMgr->UpdatePlayerCustomHomeTeleport(player->GetGUID(), player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
		handler->SendSysMessage("Location set.");

		return true;
	}
};

void AddSC_Premium_System()
{
	new PremiumSystemConf;
	new Unit_Premium_Engine;
	new Player_Premium_Engine;
	new Premium_Coin_Script;
	new Premium_Commands;
}