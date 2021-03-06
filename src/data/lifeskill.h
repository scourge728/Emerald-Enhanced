//This file has all the rewards tables for the different life gkills.
#include "constants/region_map_sections.h"
//gem tier list. These will be used when refining their respective ores.
const u16 gGemTier3[] = {//rare gem ore
    ITEM_FLYING_GEM,
    ITEM_DRAGON_GEM,
    ITEM_FIGHTING_GEM,
    ITEM_STEEL_GEM,
    ITEM_ICE_GEM,
    ITEM_NORMAL_GEM,
};

const u16 gGemTier2[] = {//uncommon gem ore
    ITEM_ROCK_GEM,
    ITEM_DARK_GEM,
    ITEM_FAIRY_GEM,
    ITEM_ELECTRIC_GEM,
    ITEM_FIRE_GEM,
    ITEM_GROUND_GEM,
};
const u16 gGemTier1[] = {//common gem ore
    ITEM_POISON_GEM,
    ITEM_WATER_GEM,
    ITEM_GHOST_GEM,
    ITEM_BUG_GEM,
    ITEM_PSYCHIC_GEM,
    ITEM_GRASS_GEM,
};

const u16 gFossilTable[] = {//used when refining fossil ore
    ITEM_ARMOR_FOSSIL,
    ITEM_ARMOR_FOSSIL,
    ITEM_SKULL_FOSSIL,
    ITEM_SKULL_FOSSIL,
    ITEM_HELIX_FOSSIL,
    ITEM_HELIX_FOSSIL,
    ITEM_DOME_FOSSIL,
    ITEM_DOME_FOSSIL,
    ITEM_PLUME_FOSSIL,
    ITEM_PLUME_FOSSIL,
    ITEM_JAW_FOSSIL,
    ITEM_JAW_FOSSIL,
    ITEM_SAIL_FOSSIL,
    ITEM_SAIL_FOSSIL,
    ITEM_ROOT_FOSSIL,
    ITEM_ROOT_FOSSIL,
    ITEM_CLAW_FOSSIL,
    ITEM_CLAW_FOSSIL,
    ITEM_OLD_AMBER,
    ITEM_OLD_AMBER,
    ITEM_THICK_CLUB,
    ITEM_RARE_BONE,
    ITEM_EVERSTONE,
    ITEM_HARD_STONE,
    ITEM_FLOAT_STONE,
    ITEM_DAMP_ROCK,
    ITEM_ICY_ROCK,
    ITEM_SMOOTH_ROCK,
    ITEM_HEAT_ROCK,
    ITEM_SOFT_SAND,
    ITEM_OVAL_STONE,
    ITEM_MASTER_BALL,//no item
    ITEM_MASTER_BALL,//no item
    ITEM_MASTER_BALL,//no item
    ITEM_MASTER_BALL,//no item
};

const u16 gShardOreTable[] = {//the table rolled on when refining shard ores.
    ITEM_DARK_SHARD,
    ITEM_DARK_SHARD,
    ITEM_LIGHT_SHARD,
    ITEM_LIGHT_SHARD,
    ITEM_CYAN_SHARD,
    ITEM_CYAN_SHARD,
    ITEM_GLOSSY_SHARD,
    ITEM_GLOSSY_SHARD,
    ITEM_GOLD_SHARD,
    ITEM_GOLD_SHARD,
    ITEM_GRAY_SHARD,
    ITEM_GRAY_SHARD,
    ITEM_RED_SHARD,
    ITEM_RED_SHARD,
    ITEM_BLUE_SHARD,
    ITEM_BLUE_SHARD,
    ITEM_YELLOW_SHARD,
    ITEM_YELLOW_SHARD,
    ITEM_GREEN_SHARD,
    ITEM_GREEN_SHARD,
    ITEM_LIGHT_CLAY,
    ITEM_IRON_BALL,
    ITEM_METAL_COAT,
    ITEM_STAR_PIECE,
    ITEM_STARDUST,
    ITEM_HEART_SCALE,
    ITEM_REVIVE,
    ITEM_MAX_REVIVE,
    ITEM_RARE_CANDY,
};

//Mining tables for interior maps

const u16 gInsideMiningTier1[] = {//level 1
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_COMMON_GEM_ORE,
    ITEM_COMMON_GEM_ORE,
    ITEM_COPPER_ORE,
    ITEM_COPPER_ORE,
    ITEM_COPPER_ORE,
    ITEM_SILVER_ORE,
};

const u16 gInsideMiningTier2[] = {//level 2
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_COMMON_GEM_ORE,
    ITEM_FOSSIL_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_COPPER_ORE,
    ITEM_COPPER_ORE,
    ITEM_SILVER_ORE,
    ITEM_SILVER_ORE,
    ITEM_GOLD_ORE
};
const u16 gInsideMiningTier3[] = {//level 3
    ITEM_GOLD_ORE,
    ITEM_RARE_GEM_ORE,
    ITEM_RARE_GEM_ORE,
    ITEM_RARE_GEM_ORE,
    ITEM_COMMON_GEM_ORE,
    ITEM_COMMON_GEM_ORE,
    ITEM_FOSSIL_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_SILVER_ORE,
    ITEM_GOLD_ORE,
    ITEM_GOLD_ORE
};

//Mining table for list of 'outside' maps. 
//You need to add new outside locations here 
//when you add new rocks if you want the miningcheck to differentiate them.

const u16 gOutsideMapSecs[] = {
    MAPSEC_ROUTE_66,
    MAPSEC_ROUTE_114,
    MAPSEC_ROUTE_115,
    MAPSEC_ROUTE_116,
    MAPSEC_MT_PYRE,
    MAPSEC_JAGGED_PASS,
    MAPSEC_JAGGED_PASS2,
};

//mining tables for exterior maps
const u16 gOutsideMiningTier1[] = {//level 1
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_SHARD_ORE,
    ITEM_SHARD_ORE,
    ITEM_COMMON_GEM_ORE,
    ITEM_COPPER_ORE,
    ITEM_COPPER_ORE,
    ITEM_COPPER_ORE,
    ITEM_SILVER_ORE,
};

const u16 gOutsideMiningTier2[] = {//level 2
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_FOSSIL_ORE,
    ITEM_COMMON_GEM_ORE,
    ITEM_SHARD_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_SHARD_ORE,
    ITEM_COPPER_ORE,
    ITEM_SILVER_ORE,
    ITEM_SILVER_ORE,
    ITEM_GOLD_ORE
};
const u16 gOutsideMiningTier3[] = {//level 3
    ITEM_COPPER_ORE,
    ITEM_RARE_GEM_ORE,
    ITEM_RARE_GEM_ORE,
    ITEM_FOSSIL_ORE,
    ITEM_SHARD_ORE,
    ITEM_COMMON_GEM_ORE,
    ITEM_NONE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_UNCOMMON_GEM_ORE,
    ITEM_SHARD_ORE,
    ITEM_NONE,
    ITEM_NONE,
    ITEM_SILVER_ORE,
    ITEM_COPPER_ORE,
    ITEM_FOSSIL_ORE
};