CREATE TABLE `premium_locations` (
	`team_id` TINYINT(1) UNSIGNED NOT NULL,
	`map_id` MEDIUMINT(8) UNSIGNED NOT NULL,
	`x` FLOAT UNSIGNED NOT NULL,
	`y` FLOAT UNSIGNED NOT NULL,
	`z` FLOAT UNSIGNED NOT NULL,
	`o` FLOAT UNSIGNED NOT NULL
)
UNIQUE INDEX `team_id` (`team_id`)
COMMENT='other premium only teleport locations.\r\ntype 0 = premium only location\r\ntype 1 = public location'
ENGINE=InnoDB
;

CREATE TABLE `premium_mall_locations` (
	`team_id` TINYINT(1) UNSIGNED NOT NULL,
	`map_id` MEDIUMINT(8) UNSIGNED NOT NULL,
	`x` FLOAT NOT NULL DEFAULT '0',
	`y` FLOAT NOT NULL DEFAULT '0',
	`z` FLOAT NOT NULL DEFAULT '0',
	`o` FLOAT NOT NULL DEFAULT '0'
)
UNIQUE INDEX `team_id` (`team_id`)
COMMENT='team teleport locations for premium only mall location.\r\nrequires both teams gps added even if same gps.'
COLLATE='utf8_general_ci'
ENGINE=InnoDB
;

CREATE TABLE IF NOT EXISTS `premium_public_mall_locations` (
  `team_id` TINYINT(1) unsigned NOT NULL,
  `map_id` mediumint(8) unsigned NOT NULL,
  `x` float NOT NULL DEFAULT '0',
  `y` float NOT NULL DEFAULT '0',
  `z` float NOT NULL DEFAULT '0',
  `o` float NOT NULL DEFAULT '0'
) 
UNIQUE INDEX `team_id` (`team_id`)
COMMENT='team teleport locations for public mall location.\r\nrequires both teams gps added even if same gps.'
DEFAULT CHARSET=utf8 
ENGINE=InnoDB 
;

CREATE TABLE `premium_player_teleports` (
	`guid` MEDIUMINT(8) UNSIGNED NOT NULL,
	`map_id` MEDIUMINT(8) UNSIGNED NOT NULL,
	`x` FLOAT UNSIGNED NOT NULL,
	`y` FLOAT UNSIGNED NOT NULL,
	`z` FLOAT UNSIGNED NOT NULL,
	`o` FLOAT UNSIGNED NOT NULL
)
	UNIQUE INDEX `guid` (`team_id`)
COLLATE='utf8_general_ci'
ENGINE=InnoDB
;

CREATE TABLE `premium_team_teleports` (
	`team_id` TINYINT(3) UNSIGNED NOT NULL,
	`map_id` MEDIUMINT(8) UNSIGNED NOT NULL,
	`x` FLOAT UNSIGNED NOT NULL,
	`y` FLOAT UNSIGNED NOT NULL,
	`z` FLOAT UNSIGNED NOT NULL,
	`o` FLOAT UNSIGNED NOT NULL
)
UNIQUE INDEX `team_id` (`team_id`)
COLLATE='utf8_general_ci'
ENGINE=InnoDB
;

REPLACE INTO `premium_team_teleports` (`team_id`, `map_id`, `x`, `y`, `z`, `o`) VALUES 
(0, 0, -4902, -960.816, 501.459, 2.20724), 
(1, 1, 1600.98, -4378.82, 9.99832, 5.24819);