#include "Moves.h"

const char *moveToString(Move move)
{
    switch (move)
    {
    case Move::POUND:
        return "Pound";
    case Move::KARATE_CHOP:
        return "Karate chop";
    case Move::DOUBLE_SLAP:
        return "Double slap";
    case Move::COMET_PUNCH:
        return "Comet punch";
    case Move::MEGA_PUNCH:
        return "Mega punch";
    case Move::PAY_DAY:
        return "Pay day";
    case Move::FIRE_PUNCH:
        return "Fire punch";
    case Move::ICE_PUNCH:
        return "Ice punch";
    case Move::THUNDER_PUNCH:
        return "Thunder punch";
    case Move::SCRATCH:
        return "Scratch";
    case Move::VICE_GRIP:
        return "Vice grip";
    case Move::GUILLOTINE:
        return "Guillotine";
    case Move::RAZOR_WIND:
        return "Razor wind";
    case Move::SWORDS_DANCE:
        return "Swords dance";
    case Move::CUT:
        return "Cut";
    case Move::GUST:
        return "Gust";
    case Move::WING_ATTACK:
        return "Wing attack";
    case Move::WHIRLWIND:
        return "Whirlwind";
    case Move::FLY:
        return "Fly";
    case Move::BIND:
        return "Bind";
    case Move::SLAM:
        return "Slam";
    case Move::VINE_WHIP:
        return "Vine whip";
    case Move::STOMP:
        return "Stomp";
    case Move::DOUBLE_KICK:
        return "Double kick";
    case Move::MEGA_KICK:
        return "Mega kick";
    case Move::JUMP_KICK:
        return "Jump kick";
    case Move::ROLLING_KICK:
        return "Rolling kick";
    case Move::SAND_ATTACK:
        return "Sand attack";
    case Move::HEADBUTT:
        return "Headbutt";
    case Move::HORN_ATTACK:
        return "Horn attack";
    case Move::FURY_ATTACK:
        return "Fury attack";
    case Move::HORN_DRILL:
        return "Horn drill";
    case Move::TACKLE:
        return "Tackle";
    case Move::BODY_SLAM:
        return "Body slam";
    case Move::WRAP:
        return "Wrap";
    case Move::TAKE_DOWN:
        return "Take down";
    case Move::THRASH:
        return "Thrash";
    case Move::DOUBLE_EDGE:
        return "Double edge";
    case Move::TAIL_WHIP:
        return "Tail whip";
    case Move::POISON_STING:
        return "Poison sting";
    case Move::TWINEEDLE:
        return "Twineedle";
    case Move::PIN_MISSILE:
        return "Pin missile";
    case Move::LEER:
        return "Leer";
    case Move::BITE:
        return "Bite";
    case Move::GROWL:
        return "Growl";
    case Move::ROAR:
        return "Roar";
    case Move::SING:
        return "Sing";
    case Move::SUPERSONIC:
        return "Supersonic";
    case Move::SONIC_BOOM:
        return "Sonic boom";
    case Move::DISABLE:
        return "Disable";
    case Move::ACID:
        return "Acid";
    case Move::EMBER:
        return "Ember";
    case Move::FLAMETHROWER:
        return "Flamethrower";
    case Move::MIST:
        return "Mist";
    case Move::WATER_GUN:
        return "Water gun";
    case Move::HYDRO_PUMP:
        return "Hydro pump";
    case Move::SURF:
        return "Surf";
    case Move::ICE_BEAM:
        return "Ice beam";
    case Move::BLIZZARD:
        return "Blizzard";
    case Move::PSYBEAM:
        return "Psybeam";
    case Move::BUBBLE_BEAM:
        return "Bubble beam";
    case Move::AURORA_BEAM:
        return "Aurora beam";
    case Move::HYPER_BEAM:
        return "Hyper beam";
    case Move::PECK:
        return "Peck";
    case Move::DRILL_PECK:
        return "Drill peck";
    case Move::SUBMISSION:
        return "Submission";
    case Move::LOW_KICK:
        return "Low kick";
    case Move::COUNTER:
        return "Counter";
    case Move::SEISMIC_TOSS:
        return "Seismic toss";
    case Move::STRENGTH:
        return "Strength";
    case Move::ABSORB:
        return "Absorb";
    case Move::MEGA_DRAIN:
        return "Mega drain";
    case Move::LEECH_SEED:
        return "Leech seed";
    case Move::GROWTH:
        return "Growth";
    case Move::RAZOR_LEAF:
        return "Razor leaf";
    case Move::SOLAR_BEAM:
        return "Solar beam";
    case Move::POISON_POWDER:
        return "Poison powder";
    case Move::STUN_SPORE:
        return "Stun spore";
    case Move::SLEEP_POWDER:
        return "Sleep powder";
    case Move::PETAL_DANCE:
        return "Petal dance";
    case Move::STRING_SHOT:
        return "String shot";
    case Move::DRAGON_RAGE:
        return "Dragon rage";
    case Move::FIRE_SPIN:
        return "Fire spin";
    case Move::THUNDER_SHOCK:
        return "Thunder shock";
    case Move::THUNDERBOLT:
        return "Thunderbolt";
    case Move::THUNDER_WAVE:
        return "Thunder wave";
    case Move::THUNDER:
        return "Thunder";
    case Move::ROCK_THROW:
        return "Rock throw";
    case Move::EARTHQUAKE:
        return "Earthquake";
    case Move::FISSURE:
        return "Fissure";
    case Move::DIG:
        return "Dig";
    case Move::TOXIC:
        return "Toxic";
    case Move::CONFUSION:
        return "Confusion";
    case Move::PSYCHIC:
        return "Psychic";
    case Move::HYPNOSIS:
        return "Hypnosis";
    case Move::MEDITATE:
        return "Meditate";
    case Move::AGILITY:
        return "Agility";
    case Move::QUICK_ATTACK:
        return "Quick attack";
    case Move::RAGE:
        return "Rage";
    case Move::TELEPORT:
        return "Teleport";
    case Move::NIGHT_SHADE:
        return "Night shade";
    case Move::MIMIC:
        return "Mimic";
    case Move::SCREECH:
        return "Screech";
    case Move::DOUBLE_TEAM:
        return "Double team";
    case Move::RECOVER:
        return "Recover";
    case Move::HARDEN:
        return "Harden";
    case Move::MINIMIZE:
        return "Minimize";
    case Move::SMOKESCREEN:
        return "Smokescreen";
    case Move::CONFUSE_RAY:
        return "Confuse ray";
    case Move::WITHDRAW:
        return "Withdraw";
    case Move::DEFENSE_CURL:
        return "Defense curl";
    case Move::BARRIER:
        return "Barrier";
    case Move::LIGHT_SCREEN:
        return "Light screen";
    case Move::HAZE:
        return "Haze";
    case Move::REFLECT:
        return "Reflect";
    case Move::FOCUS_ENERGY:
        return "Focus energy";
    case Move::BIDE:
        return "Bide";
    case Move::METRONOME:
        return "Metronome";
    case Move::MIRROR_MOVE:
        return "Mirror move";
    case Move::SELF_DESTRUCT:
        return "Self destruct";
    case Move::EGG_BOMB:
        return "Egg bomb";
    case Move::LICK:
        return "Lick";
    case Move::SMOG:
        return "Smog";
    case Move::SLUDGE:
        return "Sludge";
    case Move::BONE_CLUB:
        return "Bone club";
    case Move::FIRE_BLAST:
        return "Fire blast";
    case Move::WATERFALL:
        return "Waterfall";
    case Move::CLAMP:
        return "Clamp";
    case Move::SWIFT:
        return "Swift";
    case Move::SKULL_BASH:
        return "Skull bash";
    case Move::SPIKE_CANNON:
        return "Spike cannon";
    case Move::CONSTRICT:
        return "Constrict";
    case Move::AMNESIA:
        return "Amnesia";
    case Move::KINESIS:
        return "Kinesis";
    case Move::SOFT_BOILED:
        return "Soft boiled";
    case Move::HI_JUMP_KICK:
        return "Hi jump kick";
    case Move::GLARE:
        return "Glare";
    case Move::DREAM_EATER:
        return "Dream eater";
    case Move::POISON_GASS:
        return "Poison gass";
    case Move::BARRAGE:
        return "Barrage";
    case Move::LEECH_LIFE:
        return "Leech life";
    case Move::LOVELY_KISS:
        return "Lovely kiss";
    case Move::SKY_ATTACK:
        return "Sky attack";
    case Move::TRANSFORM:
        return "Transform";
    case Move::BUBBLE:
        return "Bubble";
    case Move::DIZZY_PUNCH:
        return "Dizzy punch";
    case Move::SPORE:
        return "Spore";
    case Move::FLASH:
        return "Flash";
    case Move::PSYWAVE:
        return "Psywave";
    case Move::SPLASH:
        return "Splash";
    case Move::ACID_ARMOR:
        return "Acid armor";
    case Move::CRABHAMMER:
        return "Crabhammer";
    case Move::EXPLOSION:
        return "Explosion";
    case Move::FURY_SWIPES:
        return "Fury swipes";
    case Move::BONEMERANG:
        return "Bonemerang";
    case Move::REST:
        return "Rest";
    case Move::ROCK_SLIDE:
        return "Rock slide";
    case Move::HYPER_FANG:
        return "Hyper fang";
    case Move::SHARPEN:
        return "Sharpen";
    case Move::CONVERSION:
        return "Conversion";
    case Move::TRI_ATTACK:
        return "Tri attack";
    case Move::SUPER_FANG:
        return "Super fang";
    case Move::SLASH:
        return "Slash";
    case Move::SUBSTITUTE:
        return "Substitute";
    case Move::STRUGGLE:
        return "Struggle";
    case Move::SKETCH:
        return "Sketch";
    case Move::TRIPLE_KICK:
        return "Triple kick";
    case Move::THIEF:
        return "Thief";
    case Move::SPIDER_WEB:
        return "Spider web";
    case Move::MIND_READER:
        return "Mind reader";
    case Move::NIGHTMARE:
        return "Nightmare";
    case Move::FLAME_WHEEL:
        return "Flame wheel";
    case Move::SNORE:
        return "Snore";
    case Move::CURSE:
        return "Curse";
    case Move::FLAIL:
        return "Flail";
    case Move::CONVERSION_2:
        return "Conversion 2";
    case Move::AEROBLAST:
        return "Aeroblast";
    case Move::COTTON_SPORE:
        return "Cotton spore";
    case Move::REVERSAL:
        return "Reversal";
    case Move::SPITE:
        return "Spite";
    case Move::POWDER_SNOW:
        return "Powder snow";
    case Move::PROTECT:
        return "Protect";
    case Move::MACH_PUNCH:
        return "Mach punch";
    case Move::SCARY_FACE:
        return "Scary face";
    case Move::FEINT_ATTACK:
        return "Feint attack";
    case Move::SWEET_KISS:
        return "Sweet kiss";
    case Move::BELLY_DRUM:
        return "Belly drum";
    case Move::SLUDGE_BOMB:
        return "Sludge bomb";
    case Move::MUD_SLAP:
        return "Mud slap";
    case Move::OCTAZOOKA:
        return "Octazooka";
    case Move::SPIKES:
        return "Spikes";
    case Move::ZAP_CANNON:
        return "Zap cannon";
    case Move::FORESIGHT:
        return "Foresight";
    case Move::DESTINY_BOND:
        return "Destiny bond";
    case Move::PERISH_SONG:
        return "Perish song";
    case Move::ICY_WIND:
        return "Icy wind";
    case Move::DETECT:
        return "Detect";
    case Move::BONE_RUSH:
        return "Bone rush";
    case Move::LOCK_ON:
        return "Lock on";
    case Move::OUTRAGE:
        return "Outrage";
    case Move::SANDSTORM:
        return "Sandstorm";
    case Move::GIGA_DRAIN:
        return "Giga drain";
    case Move::ENDURE:
        return "Endure";
    case Move::CHARM:
        return "Charm";
    case Move::ROLLOUT:
        return "Rollout";
    case Move::FALSE_SWIPE:
        return "False swipe";
    case Move::SWAGGER:
        return "Swagger";
    case Move::MILK_DRINK:
        return "Milk drink";
    case Move::SPARK:
        return "Spark";
    case Move::FURY_CUTTER:
        return "Fury cutter";
    case Move::STEEL_WING:
        return "Steel wing";
    case Move::MEAN_LOOK:
        return "Mean look";
    case Move::ATTRACT:
        return "Attract";
    case Move::SLEEP_TALK:
        return "Sleep talk";
    case Move::HEAL_BELL:
        return "Heal bell";
    case Move::RETURN:
        return "Return";
    case Move::PRESENT:
        return "Present";
    case Move::FRUSTRATION:
        return "Frustration";
    case Move::SAFEGUARD:
        return "Safeguard";
    case Move::PAIN_SPLIT:
        return "Pain split";
    case Move::SACRED_FIRE:
        return "Sacred fire";
    case Move::MAGNITUTE:
        return "Magnitute";
    case Move::DYNAMIC_PUNCH:
        return "Dynamic punch";
    case Move::MEGAHORN:
        return "Megahorn";
    case Move::DRAGON_BREATH:
        return "Dragon breath";
    case Move::BATON_PASS:
        return "Baton pass";
    case Move::ENCORE:
        return "Encore";
    case Move::PURSUIT:
        return "Pursuit";
    case Move::RAPID_SPIN:
        return "Rapid spin";
    case Move::SWEET_SCENT:
        return "Sweet scent";
    case Move::IRON_TAIL:
        return "Iron tail";
    case Move::METAL_CLAW:
        return "Metal claw";
    case Move::VITAL_THROW:
        return "Vital throw";
    case Move::MORNING_SUN:
        return "Morning sun";
    case Move::SYNTHESIS:
        return "Synthesis";
    case Move::MOONLIGHT:
        return "Moonlight";
    case Move::HIDDEN_POWER:
        return "Hidden power";
    case Move::CROSS_CHOP:
        return "Cross chop";
    case Move::TWISTER:
        return "Twister";
    case Move::RAIN_DANCE:
        return "Rain dance";
    case Move::SUNNY_DAY:
        return "Sunny day";
    case Move::CRUNCH:
        return "Crunch";
    case Move::MIRROR_COAT:
        return "Mirror coat";
    case Move::PSYCH_UP:
        return "Psych up";
    case Move::EXTREME_SPEED:
        return "Extreme speed";
    case Move::ANCIENT_POWER:
        return "Ancient power";
    case Move::SHADOW_BALL:
        return "Shadow ball";
    case Move::FUTURE_SIGHT:
        return "Future sight";
    case Move::ROCK_SMASH:
        return "Rock smash";
    case Move::WHIRLPOOL:
        return "Whirlpool";
    case Move::BEAT_UP:
        return "Beat up";
    default:
        return "Unknown move";
    }
}