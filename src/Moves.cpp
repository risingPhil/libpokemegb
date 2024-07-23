#include "Moves.h"

const char *getMoveString(Move move)
{
    switch (move)
    {
    case Move::POUND:
        return "Pound";
    case Move::KARATE_CHOP:
        return "Karate Chop";
    case Move::DOUBLE_SLAP:
        return "Double Slap";
    case Move::COMET_PUNCH:
        return "Comet Punch";
    case Move::MEGA_PUNCH:
        return "Mega Punch";
    case Move::PAY_DAY:
        return "Pay Day";
    case Move::FIRE_PUNCH:
        return "Fire Punch";
    case Move::ICE_PUNCH:
        return "Ice Punch";
    case Move::THUNDER_PUNCH:
        return "Thunder Punch";
    case Move::SCRATCH:
        return "Scratch";
    case Move::VICE_GRIP:
        return "Vice Grip";
    case Move::GUILLOTINE:
        return "Guillotine";
    case Move::RAZOR_WIND:
        return "Razor Wind";
    case Move::SWORDS_DANCE:
        return "Swords Dance";
    case Move::CUT:
        return "Cut";
    case Move::GUST:
        return "Gust";
    case Move::WING_ATTACK:
        return "Wing Attack";
    case Move::WHIRLWIND:
        return "Whirlwind";
    case Move::FLY:
        return "Fly";
    case Move::BIND:
        return "Bind";
    case Move::SLAM:
        return "Slam";
    case Move::VINE_WHIP:
        return "Vine Whip";
    case Move::STOMP:
        return "Stomp";
    case Move::DOUBLE_KICK:
        return "Double Kick";
    case Move::MEGA_KICK:
        return "Mega Kick";
    case Move::JUMP_KICK:
        return "Jump Kick";
    case Move::ROLLING_KICK:
        return "Rolling Kick";
    case Move::SAND_ATTACK:
        return "Sand Attack";
    case Move::HEADBUTT:
        return "Headbutt";
    case Move::HORN_ATTACK:
        return "Horn Attack";
    case Move::FURY_ATTACK:
        return "Fury Attack";
    case Move::HORN_DRILL:
        return "Horn Drill";
    case Move::TACKLE:
        return "Tackle";
    case Move::BODY_SLAM:
        return "Body Slam";
    case Move::WRAP:
        return "Wrap";
    case Move::TAKE_DOWN:
        return "Take Down";
    case Move::THRASH:
        return "Thrash";
    case Move::DOUBLE_EDGE:
        return "Double Edge";
    case Move::TAIL_WHIP:
        return "Tail Whip";
    case Move::POISON_STING:
        return "Poison Sting";
    case Move::TWINEEDLE:
        return "Twineedle";
    case Move::PIN_MISSILE:
        return "Pin Missile";
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
        return "Sonic Boom";
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
        return "Water Gun";
    case Move::HYDRO_PUMP:
        return "Hydro Pump";
    case Move::SURF:
        return "Surf";
    case Move::ICE_BEAM:
        return "Ice Beam";
    case Move::BLIZZARD:
        return "Blizzard";
    case Move::PSYBEAM:
        return "Psybeam";
    case Move::BUBBLE_BEAM:
        return "Bubble Beam";
    case Move::AURORA_BEAM:
        return "Aurora Beam";
    case Move::HYPER_BEAM:
        return "Hyper Beam";
    case Move::PECK:
        return "Peck";
    case Move::DRILL_PECK:
        return "Drill Peck";
    case Move::SUBMISSION:
        return "Submission";
    case Move::LOW_KICK:
        return "Low Kick";
    case Move::COUNTER:
        return "Counter";
    case Move::SEISMIC_TOSS:
        return "Seismic Toss";
    case Move::STRENGTH:
        return "Strength";
    case Move::ABSORB:
        return "Absorb";
    case Move::MEGA_DRAIN:
        return "Mega Drain";
    case Move::LEECH_SEED:
        return "Leech Seed";
    case Move::GROWTH:
        return "Growth";
    case Move::RAZOR_LEAF:
        return "Razor Leaf";
    case Move::SOLAR_BEAM:
        return "Solar Beam";
    case Move::POISON_POWDER:
        return "Poison Powder";
    case Move::STUN_SPORE:
        return "Stun Spore";
    case Move::SLEEP_POWDER:
        return "Sleep Powder";
    case Move::PETAL_DANCE:
        return "Petal Dance";
    case Move::STRING_SHOT:
        return "String Shot";
    case Move::DRAGON_RAGE:
        return "Dragon Rage";
    case Move::FIRE_SPIN:
        return "Fire Spin";
    case Move::THUNDER_SHOCK:
        return "Thunder Shock";
    case Move::THUNDERBOLT:
        return "Thunderbolt";
    case Move::THUNDER_WAVE:
        return "Thunder Wave";
    case Move::THUNDER:
        return "Thunder";
    case Move::ROCK_THROW:
        return "Rock Throw";
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
        return "Quick Attack";
    case Move::RAGE:
        return "Rage";
    case Move::TELEPORT:
        return "Teleport";
    case Move::NIGHT_SHADE:
        return "Night Shade";
    case Move::MIMIC:
        return "Mimic";
    case Move::SCREECH:
        return "Screech";
    case Move::DOUBLE_TEAM:
        return "Double Team";
    case Move::RECOVER:
        return "Recover";
    case Move::HARDEN:
        return "Harden";
    case Move::MINIMIZE:
        return "Minimize";
    case Move::SMOKESCREEN:
        return "Smokescreen";
    case Move::CONFUSE_RAY:
        return "Confuse Ray";
    case Move::WITHDRAW:
        return "Withdraw";
    case Move::DEFENSE_CURL:
        return "Defense Curl";
    case Move::BARRIER:
        return "Barrier";
    case Move::LIGHT_SCREEN:
        return "Light Screen";
    case Move::HAZE:
        return "Haze";
    case Move::REFLECT:
        return "Reflect";
    case Move::FOCUS_ENERGY:
        return "Focus Energy";
    case Move::BIDE:
        return "Bide";
    case Move::METRONOME:
        return "Metronome";
    case Move::MIRROR_MOVE:
        return "Mirror Move";
    case Move::SELF_DESTRUCT:
        return "Self Destruct";
    case Move::EGG_BOMB:
        return "Egg Bomb";
    case Move::LICK:
        return "Lick";
    case Move::SMOG:
        return "Smog";
    case Move::SLUDGE:
        return "Sludge";
    case Move::BONE_CLUB:
        return "Bone Club";
    case Move::FIRE_BLAST:
        return "Fire Blast";
    case Move::WATERFALL:
        return "Waterfall";
    case Move::CLAMP:
        return "Clamp";
    case Move::SWIFT:
        return "Swift";
    case Move::SKULL_BASH:
        return "Skull Bash";
    case Move::SPIKE_CANNON:
        return "Spike Cannon";
    case Move::CONSTRICT:
        return "Constrict";
    case Move::AMNESIA:
        return "Amnesia";
    case Move::KINESIS:
        return "Kinesis";
    case Move::SOFT_BOILED:
        return "Soft Boiled";
    case Move::HI_JUMP_KICK:
        return "Hi Jump Kick";
    case Move::GLARE:
        return "Glare";
    case Move::DREAM_EATER:
        return "Dream Eater";
    case Move::POISON_GASS:
        return "Poison Gass";
    case Move::BARRAGE:
        return "Barrage";
    case Move::LEECH_LIFE:
        return "Leech Life";
    case Move::LOVELY_KISS:
        return "Lovely Kiss";
    case Move::SKY_ATTACK:
        return "Sky Attack";
    case Move::TRANSFORM:
        return "Transform";
    case Move::BUBBLE:
        return "Bubble";
    case Move::DIZZY_PUNCH:
        return "Dizzy Punch";
    case Move::SPORE:
        return "Spore";
    case Move::FLASH:
        return "Flash";
    case Move::PSYWAVE:
        return "Psywave";
    case Move::SPLASH:
        return "Splash";
    case Move::ACID_ARMOR:
        return "Acid Armor";
    case Move::CRABHAMMER:
        return "Crabhammer";
    case Move::EXPLOSION:
        return "Explosion";
    case Move::FURY_SWIPES:
        return "Fury Swipes";
    case Move::BONEMERANG:
        return "Bonemerang";
    case Move::REST:
        return "Rest";
    case Move::ROCK_SLIDE:
        return "Rock Slide";
    case Move::HYPER_FANG:
        return "Hyper Fang";
    case Move::SHARPEN:
        return "Sharpen";
    case Move::CONVERSION:
        return "Conversion";
    case Move::TRI_ATTACK:
        return "Tri Attack";
    case Move::SUPER_FANG:
        return "Super Fang";
    case Move::SLASH:
        return "Slash";
    case Move::SUBSTITUTE:
        return "Substitute";
    case Move::STRUGGLE:
        return "Struggle";
    case Move::SKETCH:
        return "Sketch";
    case Move::TRIPLE_KICK:
        return "Triple Kick";
    case Move::THIEF:
        return "Thief";
    case Move::SPIDER_WEB:
        return "Spider Web";
    case Move::MIND_READER:
        return "Mind Reader";
    case Move::NIGHTMARE:
        return "Nightmare";
    case Move::FLAME_WHEEL:
        return "Flame Wheel";
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
        return "Cotton Spore";
    case Move::REVERSAL:
        return "Reversal";
    case Move::SPITE:
        return "Spite";
    case Move::POWDER_SNOW:
        return "Powder Snow";
    case Move::PROTECT:
        return "Protect";
    case Move::MACH_PUNCH:
        return "Mach Punch";
    case Move::SCARY_FACE:
        return "Scary Face";
    case Move::FEINT_ATTACK:
        return "Feint Attack";
    case Move::SWEET_KISS:
        return "Sweet Kiss";
    case Move::BELLY_DRUM:
        return "Belly Drum";
    case Move::SLUDGE_BOMB:
        return "Sludge Bomb";
    case Move::MUD_SLAP:
        return "Mud Slap";
    case Move::OCTAZOOKA:
        return "Octazooka";
    case Move::SPIKES:
        return "Spikes";
    case Move::ZAP_CANNON:
        return "Zap Cannon";
    case Move::FORESIGHT:
        return "Foresight";
    case Move::DESTINY_BOND:
        return "Destiny Bond";
    case Move::PERISH_SONG:
        return "Perish Song";
    case Move::ICY_WIND:
        return "Icy Wind";
    case Move::DETECT:
        return "Detect";
    case Move::BONE_RUSH:
        return "Bone Rush";
    case Move::LOCK_ON:
        return "Lock On";
    case Move::OUTRAGE:
        return "Outrage";
    case Move::SANDSTORM:
        return "Sandstorm";
    case Move::GIGA_DRAIN:
        return "Giga Drain";
    case Move::ENDURE:
        return "Endure";
    case Move::CHARM:
        return "Charm";
    case Move::ROLLOUT:
        return "Rollout";
    case Move::FALSE_SWIPE:
        return "False Swipe";
    case Move::SWAGGER:
        return "Swagger";
    case Move::MILK_DRINK:
        return "Milk Drink";
    case Move::SPARK:
        return "Spark";
    case Move::FURY_CUTTER:
        return "Fury Cutter";
    case Move::STEEL_WING:
        return "Steel Wing";
    case Move::MEAN_LOOK:
        return "Mean Look";
    case Move::ATTRACT:
        return "Attract";
    case Move::SLEEP_TALK:
        return "Sleep Talk";
    case Move::HEAL_BELL:
        return "Heal Bell";
    case Move::RETURN:
        return "Return";
    case Move::PRESENT:
        return "Present";
    case Move::FRUSTRATION:
        return "Frustration";
    case Move::SAFEGUARD:
        return "Safeguard";
    case Move::PAIN_SPLIT:
        return "Pain Split";
    case Move::SACRED_FIRE:
        return "Sacred Fire";
    case Move::MAGNITUTE:
        return "Magnitute";
    case Move::DYNAMIC_PUNCH:
        return "Dynamic Punch";
    case Move::MEGAHORN:
        return "Megahorn";
    case Move::DRAGON_BREATH:
        return "Dragon Breath";
    case Move::BATON_PASS:
        return "Baton Pass";
    case Move::ENCORE:
        return "Encore";
    case Move::PURSUIT:
        return "Pursuit";
    case Move::RAPID_SPIN:
        return "Rapid Spin";
    case Move::SWEET_SCENT:
        return "Sweet Scent";
    case Move::IRON_TAIL:
        return "Iron Tail";
    case Move::METAL_CLAW:
        return "Metal Claw";
    case Move::VITAL_THROW:
        return "Vital Throw";
    case Move::MORNING_SUN:
        return "Morning Sun";
    case Move::SYNTHESIS:
        return "Synthesis";
    case Move::MOONLIGHT:
        return "Moonlight";
    case Move::HIDDEN_POWER:
        return "Hidden Power";
    case Move::CROSS_CHOP:
        return "Cross Chop";
    case Move::TWISTER:
        return "Twister";
    case Move::RAIN_DANCE:
        return "Rain Dance";
    case Move::SUNNY_DAY:
        return "Sunny Day";
    case Move::CRUNCH:
        return "Crunch";
    case Move::MIRROR_COAT:
        return "Mirror Coat";
    case Move::PSYCH_UP:
        return "Psych Up";
    case Move::EXTREME_SPEED:
        return "Extreme Speed";
    case Move::ANCIENT_POWER:
        return "Ancient Power";
    case Move::SHADOW_BALL:
        return "Shadow Ball";
    case Move::FUTURE_SIGHT:
        return "Future Sight";
    case Move::ROCK_SMASH:
        return "Rock Smash";
    case Move::WHIRLPOOL:
        return "Whirlpool";
    case Move::BEAT_UP:
        return "Beat Up";
    default:
        return "Unknown move";
    }
}