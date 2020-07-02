#include "global.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "bg.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_object_lock.h"
#include "event_scripts.h"
#include "fieldmap.h"
#include "field_effect.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "field_weather.h"
#include "field_screen_effect.h"
#include "frontier_pass.h"
#include "frontier_util.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "item_menu.h"
#include "link.h"
#include "load_save.h"
#include "main.h"
#include "menu.h"
#include "new_game.h"
#include "option_menu.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokenav.h"
#include "safari_zone.h"
#include "save.h"
#include "scanline_effect.h"
#include "script.h"
#include "sound.h"
#include "start_menu.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "trainer_card.h"
#include "window.h"
#include "constants/songs.h"
#include "rom_8011DC0.h"
#include "union_room.h"
#include "constants/rgb.h"
#include "field_message_box.h"
#include "constants/event_objects.h"
#include "rtc.h"
#include "gba/m4a_internal.h"

static EWRAM_DATA u8 MenuSpriteId1 = 0;
static EWRAM_DATA u8 MenuSpriteId2 = 0;

extern u8 RyuDebugMenuScript[];
extern u8 RyuStartMenuConfigInfoScript[];
extern const u8 gText_RyuVersion[];

// Menu actions
enum
{
    MENU_ACTION_POKEDEX,
    MENU_ACTION_POKEMON,
    MENU_ACTION_BAG,
    MENU_ACTION_POKENAV,
    MENU_ACTION_PLAYER,
    MENU_ACTION_SAVE,
    MENU_ACTION_OPTION,
    MENU_ACTION_EXIT,
    MENU_ACTION_RETIRE_SAFARI,
    MENU_ACTION_PLAYER_LINK,
    MENU_ACTION_REST_FRONTIER,
    MENU_ACTION_RETIRE_FRONTIER,
    MENU_ACTION_PYRAMID_BAG
};

// Save status
enum
{
    SAVE_IN_PROGRESS,
    SAVE_SUCCESS,
    SAVE_CANCELED,
    SAVE_ERROR
};

// IWRAM common
bool8 (*gMenuCallback)(void);

// EWRAM
EWRAM_DATA static u8 sSafariBallsWindowId = 0;
EWRAM_DATA static u8 sBattlePyramidFloorWindowId = 0;
EWRAM_DATA static u8 sStartMenuCursorPos = 0;
EWRAM_DATA static u8 sNumStartMenuActions = 0;
EWRAM_DATA static u8 sCurrentStartMenuActions[9] = {0};
EWRAM_DATA static u8 sInitStartMenuData[2] = {0};

EWRAM_DATA static u8 (*sSaveDialogCallback)(void) = NULL;
EWRAM_DATA static u8 sSaveDialogTimer = 0;
EWRAM_DATA static bool8 sSavingComplete = FALSE;
EWRAM_DATA static u8 sSaveInfoWindowId = 0;

// Menu action callbacks
static bool8 StartMenuPokedexCallback(void);
static bool8 StartMenuPokemonCallback(void);
static bool8 StartMenuBagCallback(void);
static bool8 StartMenuPokeNavCallback(void);
static bool8 StartMenuPlayerNameCallback(void);
static bool8 StartMenuSaveCallback(void);
static bool8 StartMenuOptionCallback(void);
static bool8 StartMenuExitCallback(void);
static bool8 StartMenuSafariZoneRetireCallback(void);
static bool8 StartMenuLinkModePlayerNameCallback(void);
static bool8 StartMenuBattlePyramidRetireCallback(void);
static bool8 StartMenuBattlePyramidBagCallback(void);

// Menu callbacks
static bool8 SaveStartCallback(void);
static bool8 SaveCallback(void);
static bool8 BattlePyramidRetireStartCallback(void);
static bool8 BattlePyramidRetireReturnCallback(void);
static bool8 BattlePyramidRetireCallback(void);
static bool8 HandleStartMenuInput(void);

// Save dialog callbacks
static u8 SaveConfirmSaveCallback(void);
static u8 SaveYesNoCallback(void);
static u8 SaveConfirmInputCallback(void);
static u8 SaveFileExistsCallback(void);
static u8 SaveConfirmOverwriteDefaultNoCallback(void);
static u8 SaveConfirmOverwriteCallback(void);
static u8 SaveOverwriteInputCallback(void);
static u8 SaveSavingMessageCallback(void);
static u8 SaveDoSaveCallback(void);
static u8 SaveSuccessCallback(void);
static u8 SaveReturnSuccessCallback(void);
static u8 SaveErrorCallback(void);
static u8 SaveReturnErrorCallback(void);
static u8 BattlePyramidConfirmRetireCallback(void);
static u8 BattlePyramidRetireYesNoCallback(void);
static u8 BattlePyramidRetireInputCallback(void);

// Task callbacks
static void StartMenuTask(u8 taskId);
static void SaveGameTask(u8 taskId);
static void sub_80A0550(u8 taskId);
static void sub_80A08A4(u8 taskId);
static bool8 FieldCB_ReturnToFieldStartMenu(void);

extern int CountBadges();

static const struct WindowTemplate sSafariBallsWindowTemplate = {0, 1, 1, 9, 4, 0xF, 8};

static const u8* const sPyramindFloorNames[] =
{
    gText_Floor1,
    gText_Floor2,
    gText_Floor3,
    gText_Floor4,
    gText_Floor5,
    gText_Floor6,
    gText_Floor7,
    gText_Peak
};

static const struct WindowTemplate sPyramidFloorWindowTemplate_2 = {0, 1, 1, 0xA, 4, 0xF, 8};
static const struct WindowTemplate sPyramidFloorWindowTemplate_1 = {0, 1, 1, 0xC, 4, 0xF, 8};

static const struct MenuAction sStartMenuItems[] =
{
    {gText_MenuPokedex, {.u8_void = StartMenuPokedexCallback}},
    {gText_MenuPokemon, {.u8_void = StartMenuPokemonCallback}},
    {gText_MenuBag, {.u8_void = StartMenuBagCallback}},
    {gText_MenuPokenav, {.u8_void = StartMenuPokeNavCallback}},
    {gText_MenuPlayer, {.u8_void = StartMenuPlayerNameCallback}},
    {gText_MenuSave, {.u8_void = StartMenuSaveCallback}},
    {gText_MenuOption, {.u8_void = StartMenuOptionCallback}},
    {gText_MenuExit, {.u8_void = StartMenuExitCallback}},
    {gText_MenuRetire, {.u8_void = StartMenuSafariZoneRetireCallback}},
    {gText_MenuPlayer, {.u8_void = StartMenuLinkModePlayerNameCallback}},
    {gText_MenuRest, {.u8_void = StartMenuSaveCallback}},
    {gText_MenuRetire, {.u8_void = StartMenuBattlePyramidRetireCallback}},
    {gText_MenuBag, {.u8_void = StartMenuBattlePyramidBagCallback}}
};

static const struct BgTemplate sUnknown_085105A8[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }
};

static const struct WindowTemplate sUnknown_085105AC[] =
{
    {0, 2, 0xF, 0x1A, 4, 0xF, 0x194},
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sSaveInfoWindowTemplate = {0, 1, 1, 0xE, 0xA, 0xF, 8};

// Local functions
static void BuildStartMenuActions(void);
static void AddStartMenuAction(u8 action);
static void BuildNormalStartMenu(void);
static void BuildSafariZoneStartMenu(void);
static void BuildLinkModeStartMenu(void);
static void BuildUnionRoomStartMenu(void);
static void BuildBattlePikeStartMenu(void);
static void BuildBattlePyramidStartMenu(void);
static void BuildMultiPartnerRoomStartMenu(void);
static void ShowSafariBallsWindow(void);
static void ShowPyramidFloorWindow(void);
static void RemoveExtraStartMenuWindows(void);
static bool32 PrintStartMenuActions(s8 *pIndex, u32 count);
static bool32 InitStartMenuStep(void);
static void InitStartMenu(void);
static void CreateStartMenuTask(TaskFunc followupFunc);
static void InitSave(void);
static u8 RunSaveCallback(void);
static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void));
static void sub_80A0014(void);
static void HideSaveInfoWindow(void);
static void SaveStartTimer(void);
static bool8 SaveSuccesTimer(void);
static bool8 SaveErrorTimer(void);
static void InitBattlePyramidRetire(void);
static void sub_80A03D8(void);
static bool32 sub_80A03E4(u8 *par1);
static void sub_80A0540(void);
static void ShowSaveInfoWindow(void);
static void RemoveSaveInfoWindow(void);
static void HideStartMenuWindow(void);

void SetDexPokemonPokenavFlags(void) // unused
{
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKENAV_GET);
}

static void BuildStartMenuActions(void)
{
    sNumStartMenuActions = 0;

    if (IsUpdateLinkStateCBActive() == TRUE)
    {
        BuildLinkModeStartMenu();
    }
    else if (InUnionRoom() == TRUE)
    {
        BuildUnionRoomStartMenu();
    }
    else if (GetSafariZoneFlag() == TRUE)
    {
        BuildSafariZoneStartMenu();
    }
    else if (InBattlePike())
    {
        BuildBattlePikeStartMenu();
    }
    else if (InBattlePyramid())
    {
        BuildBattlePyramidStartMenu();
    }
    else if (InMultiPartnerRoom())
    {
        BuildMultiPartnerRoomStartMenu();
    }
    else
    {
        BuildNormalStartMenu();
    }
}

static void AddStartMenuAction(u8 action)
{
    AppendToList(sCurrentStartMenuActions, &sNumStartMenuActions, action);
}

static void BuildNormalStartMenu(void)
{
    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKEDEX);
    }
    if (FlagGet(FLAG_SYS_POKEMON_GET) == TRUE)
    {
        if (!(CalculatePlayerPartyCount() == 0))
            AddStartMenuAction(MENU_ACTION_POKEMON);
    }

    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        if (!(CalculatePlayerPartyCount() == 0))
        {
            AddStartMenuAction(MENU_ACTION_POKENAV);
        }
    }

    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_SAVE);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildSafariZoneStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_RETIRE_SAFARI);
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildLinkModeStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER_LINK);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildUnionRoomStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_BAG);

    if (FlagGet(FLAG_SYS_POKENAV_GET) == TRUE)
    {
        AddStartMenuAction(MENU_ACTION_POKENAV);
    }

    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildBattlePikeStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEDEX);
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildBattlePyramidStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PYRAMID_BAG);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_REST_FRONTIER);
    AddStartMenuAction(MENU_ACTION_RETIRE_FRONTIER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void BuildMultiPartnerRoomStartMenu(void)
{
    AddStartMenuAction(MENU_ACTION_POKEMON);
    AddStartMenuAction(MENU_ACTION_PLAYER);
    AddStartMenuAction(MENU_ACTION_OPTION);
    AddStartMenuAction(MENU_ACTION_EXIT);
}

static void ShowSafariBallsWindow(void)
{
    sSafariBallsWindowId = AddWindow(&sSafariBallsWindowTemplate);
    PutWindowTilemap(sSafariBallsWindowId);
    DrawStdWindowFrame(sSafariBallsWindowId, FALSE);
    ConvertIntToDecimalStringN(gStringVar1, gNumSafariBalls, STR_CONV_MODE_RIGHT_ALIGN, 2);
    StringExpandPlaceholders(gStringVar4, gText_SafariBallStock);
    AddTextPrinterParameterized(sSafariBallsWindowId, 1, gStringVar4, 0, 1, 0xFF, NULL);
    CopyWindowToVram(sSafariBallsWindowId, 2);
}

static void ShowPyramidFloorWindow(void)
{
    if (gSaveBlock2Ptr->frontier.curChallengeBattleNum == 7)
        sBattlePyramidFloorWindowId = AddWindow(&sPyramidFloorWindowTemplate_1);
    else
        sBattlePyramidFloorWindowId = AddWindow(&sPyramidFloorWindowTemplate_2);

    PutWindowTilemap(sBattlePyramidFloorWindowId);
    DrawStdWindowFrame(sBattlePyramidFloorWindowId, FALSE);
    StringCopy(gStringVar1, sPyramindFloorNames[gSaveBlock2Ptr->frontier.curChallengeBattleNum]);
    StringExpandPlaceholders(gStringVar4, gText_BattlePyramidFloor);
    AddTextPrinterParameterized(sBattlePyramidFloorWindowId, 1, gStringVar4, 0, 1, 0xFF, NULL);
    CopyWindowToVram(sBattlePyramidFloorWindowId, 2);
}

static void RemoveExtraStartMenuWindows(void)
{
    if (GetSafariZoneFlag())
    {
        ClearStdWindowAndFrameToTransparent(sSafariBallsWindowId, FALSE);
        CopyWindowToVram(sSafariBallsWindowId, 2);
        RemoveWindow(sSafariBallsWindowId);
    }
    if (InBattlePyramid())
    {
        ClearStdWindowAndFrameToTransparent(sBattlePyramidFloorWindowId, FALSE);
        RemoveWindow(sBattlePyramidFloorWindowId);
    }
}

EWRAM_DATA static u8 sPrintNumberWindowId = 1;
void PrintNumberToScreen(s32 num)
{
    struct WindowTemplate template;

    //prepare window
    SetWindowTemplateFields(&template, 0, 1, 1, 13, 5, 15, 8);
    sPrintNumberWindowId = AddWindow(&template);
    FillWindowPixelBuffer(sPrintNumberWindowId, 0);
    PutWindowTilemap(sPrintNumberWindowId);
    CopyWindowToVram(sPrintNumberWindowId, 1);
    DrawStdFrameWithCustomTileAndPalette(sPrintNumberWindowId, FALSE, 0x214, 14);

    //song readout
    StringCopy(gStringVar1, gText_HighlightTransparent);
    StringAppend(gStringVar1, gText_ryuJukeboxLabel);
    ConvertIntToDecimalStringN(gStringVar2, num, 0, 3);
    StringAppend(gStringVar1, gStringVar2);

    //time readout
    RtcCalcLocalTime();
    StringCopy(gStringVar3, gText_HighlightTransparent);
    StringAppend(gStringVar3, gText_SpaceTime);
    ConvertIntToDecimalStringN(gStringVar2, gLocalTime.hours, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar3, gStringVar2);
    StringAppend(gStringVar3, gText_colon);
    ConvertIntToDecimalStringN(gStringVar2, gLocalTime.minutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar3, gStringVar2);
    StringAppend(gStringVar3, gText_colon);
    ConvertIntToDecimalStringN(gStringVar2, gLocalTime.seconds, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringAppend(gStringVar3, gStringVar2);
    //print 'day', 'dusk', 'night' or 'dawn' in reference to evolution
    if (gLocalTime.hours >= 18 || gLocalTime.hours < 6)
    {
        if (gLocalTime.hours == 18)
        {
            StringAppend(gStringVar3, gText_ColorLightBlueShadowDarkGrey);
            StringAppend(gStringVar3, gText_Dusk);   
        }
        else
        {
            StringAppend(gStringVar3, gText_ColorLightBlueShadowBlue);
            StringAppend(gStringVar3, gText_Night);
        }
    }
    else
    {   
        if (gLocalTime.hours == 6)
        {
            StringAppend(gStringVar3, gText_ColorLightRedShadowDarkGrey);
            StringAppend(gStringVar3, gText_RyuDawn);
        }        
        else
        {
            StringAppend(gStringVar3, gText_ColorLightRedShadowRed);
            StringAppend(gStringVar3, gText_RyuDay);
        }
    }
    
    //print version number
    StringCopy(gStringVar2, gText_RyuVersion);
    ConvertIntToDecimalStringN(gStringVar4, VarGet(VAR_LAST_KNOWN_GAME_VERSION), STR_CONV_MODE_LEFT_ALIGN, 4);
    StringAppend(gStringVar2, gStringVar4);


    //print all text
    AddTextPrinterParameterized(sPrintNumberWindowId, 0, gStringVar1, 0, 0, 0, NULL);
    AddTextPrinterParameterized(sPrintNumberWindowId, 0, gStringVar2, 56, 0, 0, NULL);
    AddTextPrinterParameterized(sPrintNumberWindowId, 0, gStringVar3, 0, 10, 0, NULL);
    //print KO's
    StringCopy(gStringVar1, gText_RyuKills);
    ConvertIntToDecimalStringN(gStringVar2, VarGet(VAR_RYU_TOTAL_FAINTS), STR_CONV_MODE_LEFT_ALIGN, 5);
    StringAppend(gStringVar1, gStringVar2);
    AddTextPrinterParameterized(sPrintNumberWindowId, 0, gStringVar1, 0, 22, 0, NULL);

    StringCopy(gStringVar1, gText_RyuBadges);
    ConvertIntToDecimalStringN(gStringVar2, (CountBadges()), STR_CONV_MODE_LEFT_ALIGN, 1);
    StringAppend(gStringVar1, gStringVar2);
    AddTextPrinterParameterized(sPrintNumberWindowId, 0, gStringVar1, 56, 22, 0, NULL);
}

void RemovePrintedNumber(void)
{
    ClearStdWindowAndFrameToTransparent(sPrintNumberWindowId, FALSE);
    CopyWindowToVram(sPrintNumberWindowId, 2);
    RemoveWindow(sPrintNumberWindowId);
}

void PrintSongNumber(u16 song)
{
    PrintNumberToScreen(song);
}

static bool32 PrintStartMenuActions(s8 *pIndex, u32 count)
{
    s8 index = *pIndex;

    do
    {
        if (sStartMenuItems[sCurrentStartMenuActions[index]].func.u8_void == StartMenuPlayerNameCallback)
        {
            PrintPlayerNameOnWindow(GetStartMenuWindowId(), sStartMenuItems[sCurrentStartMenuActions[index]].text, 8, (index << 4) + 9);
        }
        else
        {
            StringExpandPlaceholders(gStringVar4, sStartMenuItems[sCurrentStartMenuActions[index]].text);
            AddTextPrinterParameterized(GetStartMenuWindowId(), 1, gStringVar4, 8, (index << 4) + 9, 0xFF, NULL);
        }

        index++;
        if (index >= sNumStartMenuActions)
        {
            *pIndex = index;
            return TRUE;
        }

        count--;
    }
    while (count != 0);

    *pIndex = index;
    return FALSE;
}



//Start menu logo display defines


static const u32 DevonLogoGfx[] = INCBIN_U32("graphics/cutscene/devonLogo.4bpp");
static const u16 DevonLogoPal[] = INCBIN_U16("graphics/cutscene/devonLogo.gbapal");
static const u32 DevonScientistLogoGfx[] = INCBIN_U32("graphics/cutscene/devonScientistLogo.4bpp");
static const u32 AquaLogoGfx[] = INCBIN_U32("graphics/cutscene/aquaLogo.4bpp");
static const u16 AquaLogoPal[] = INCBIN_U16("graphics/cutscene/aquaLogo.gbapal");
static const u32 AquaShellyLogoGfx[] = INCBIN_U32("graphics/cutscene/aquaShellyLogo.4bpp");
static const u32 MagmaLogoGfx[] = INCBIN_U32("graphics/cutscene/magmaLogo.4bpp");
static const u16 MagmaLogoPal[] = INCBIN_U16("graphics/cutscene/magmaLogo.gbapal");

const struct SpriteSheet DevonLogoSheet =
{
    .data = DevonLogoGfx,
    .size = sizeof(DevonLogoGfx),
    .tag = 2652
};

const struct SpritePalette DevonLogoPalette =
{
    .data = DevonLogoPal, 
	.tag = 2652
};

static const struct OamData DevonLogoOamData =
{
    .y = 0,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 0
};

const struct SpriteTemplate DevonLogoSpriteTemplate =
{
    .tileTag = 2652,
    .paletteTag = 2652,
    .oam = &DevonLogoOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

const struct SpriteSheet DevonScientistLogoSheet =
{
    .data = DevonScientistLogoGfx,
    .size = sizeof(DevonScientistLogoGfx),
    .tag = 2652
};

static const struct OamData DevonScientistLogoOamData =
{
    .y = 0,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 0
};

const struct SpriteTemplate DevonScientistLogoSpriteTemplate =
{
    .tileTag = 2652,
    .paletteTag = 2652,
    .oam = &DevonScientistLogoOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

const struct SpriteSheet AquaLogoSheet =
{
    .data = AquaLogoGfx,
    .size = sizeof(AquaLogoGfx),
    .tag = 1254
};

const struct SpritePalette AquaLogoPalette =
{
    .data = AquaLogoPal, 
	.tag = 1254
};

static const struct OamData AquaLogoOamData =
{
    .y = 0,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 0
};

const struct SpriteTemplate AquaLogoSpriteTemplate =
{
    .tileTag = 1254,
    .paletteTag = 1254,
    .oam = &AquaLogoOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

const struct SpriteSheet AquaShellyLogoSheet =
{
    .data = AquaLogoGfx,
    .size = sizeof(AquaLogoGfx),
    .tag = 1254
};

static const struct OamData AquaShellyLogoOamData =
{
    .y = 0,
    .shape = SPRITE_SHAPE(32x32),
    .size = SPRITE_SIZE(32x32),
    .priority = 0
};

const struct SpriteTemplate AquaShellyLogoSpriteTemplate =
{
    .tileTag = 1254,
    .paletteTag = 1254,
    .oam = &AquaShellyLogoOamData,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

void DrawDevonLogo(void)
{
    if (FlagGet(FLAG_RYU_DEVON_SCIENTIST) == 1)
    {
        LoadSpriteSheet(&DevonScientistLogoSheet);
        LoadSpritePalette(&DevonLogoPalette);
        MenuSpriteId1 = (CreateSprite(&DevonLogoSpriteTemplate, 16, 62, 0));
    }
    else
    {
        LoadSpriteSheet(&DevonLogoSheet);
        LoadSpritePalette(&DevonLogoPalette);
        MenuSpriteId1 = (CreateSprite(&DevonScientistLogoSpriteTemplate, 16, 62, 0));
    }
}

void DrawAquaLogo(void)
{
    if (FlagGet(FLAG_RYU_PLAYER_ARCHIE_ACQ) == 1)
    {
        LoadSpriteSheet(&AquaShellyLogoSheet);
        LoadSpritePalette(&AquaLogoPalette);
        MenuSpriteId1 = (CreateSprite(&AquaShellyLogoSpriteTemplate, 16, 62, 0));
    }
    else
    {
        LoadSpriteSheet(&AquaLogoSheet);
        LoadSpritePalette(&AquaLogoPalette);
        MenuSpriteId1 = (CreateSprite(&AquaLogoSpriteTemplate, 16, 62, 0));
    }
}
void DrawMagmaLogo(void)
{}

static bool32 InitStartMenuStep(void)
{
    s8 state = sInitStartMenuData[0];

    switch (state)
    {
    case 0:
        sInitStartMenuData[0]++;
        break;
    case 1:
        BuildStartMenuActions();
        sInitStartMenuData[0]++;
        break;
    case 2:
        LoadMessageBoxAndBorderGfx();
        DrawStdWindowFrame(sub_81979C4(sNumStartMenuActions), FALSE);
        sInitStartMenuData[1] = 0;
        sInitStartMenuData[0]++;
        break;
    case 3:
        if (GetSafariZoneFlag())
            ShowSafariBallsWindow();
        if (InBattlePyramid())
            ShowPyramidFloorWindow();
        sInitStartMenuData[0]++;
        break;
    case 4:
        if (PrintStartMenuActions(&sInitStartMenuData[1], 2))
            sInitStartMenuData[0]++;
        break;
    case 5:
        sStartMenuCursorPos = sub_81983AC(GetStartMenuWindowId(), 1, 0, 9, 16, sNumStartMenuActions, sStartMenuCursorPos);
        CopyWindowToVram(GetStartMenuWindowId(), TRUE);
		if (FlagGet(FLAG_RYU_JUKEBOX_ENABLED) == 1)
		{
			PrintSongNumber(VarGet(VAR_RYU_JUKEBOX));
		}
		else
		{
			PrintSongNumber(GetCurrentMapMusic());
		}

        if (FlagGet(FLAG_RYU_PLAYER_HELPING_DEVON) == 1)
        {
            DrawDevonLogo();
        }
        else if (FlagGet(FLAG_RYU_PLAYER_HELPING_AQUA) == 1)
        {
            DrawAquaLogo();
        }
        else{}
        
        return TRUE;
    }

    return FALSE;
}

static void InitStartMenu(void)
{
    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    while (!InitStartMenuStep())
        ;
}

static void StartMenuTask(u8 taskId)
{
    if (InitStartMenuStep() == TRUE)
        SwitchTaskToFollowupFunc(taskId);
}

static void CreateStartMenuTask(TaskFunc followupFunc)
{
    u8 taskId;

    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    taskId = CreateTask(StartMenuTask, 0x50);
    SetTaskFuncWithFollowupFunc(taskId, StartMenuTask, followupFunc);
}

static bool8 FieldCB_ReturnToFieldStartMenu(void)
{
    if (InitStartMenuStep() == FALSE)
    {
        return FALSE;
    }

    ReturnToFieldOpenStartMenu();
    return TRUE;
}

void ShowReturnToFieldStartMenu(void)
{
    sInitStartMenuData[0] = 0;
    sInitStartMenuData[1] = 0;
    gFieldCallback2 = FieldCB_ReturnToFieldStartMenu;
}

void Task_ShowStartMenu(u8 taskId)
{
    struct Task* task = &gTasks[taskId];

    switch(task->data[0])
    {
    case 0:
        if (InUnionRoom() == TRUE)
            var_800D_set_xB();

        gMenuCallback = HandleStartMenuInput;
        task->data[0]++;
        break;
    case 1:
        if (gMenuCallback() == TRUE)
            DestroyTask(taskId);
        break;
    }
}

void ShowStartMenu(void)
{
    if (!IsUpdateLinkStateCBActive())
    {
        FreezeObjectEvents();
        sub_808B864();
        sub_808BCF4();
    }

    CreateStartMenuTask(Task_ShowStartMenu);
    ScriptContext2_Enable();
}

void PlayNextTrack(void)
{
    u16 song = (VarGet(VAR_RYU_JUKEBOX));
    PlaySE(SE_PIN);
    PlayBGM(song);
    RemovePrintedNumber();
    PrintSongNumber(song);
}

static bool8 HandleStartMenuInput(void)
{
    u16 song = VarGet(VAR_RYU_JUKEBOX);
    song = song + 1;
    switch(song)
    {
        case 1:
        case 999:
        case 557:
            song = 350;
            break;
        default:
            break;
    }
    if(song > 1000)
    {
        song = 350;
    }
    if ((FlagGet(FLAG_RYU_JUKEBOX_ENABLED) == 1) && gMain.newKeys & R_BUTTON)
    {
        PlaySE(SE_PIN);
        if(gSongTable[song].me != 0) // if second number in this songtable entry is not zero
        {
            for(;gSongTable[song].me != 0; song = song + 1) // skip songs that are like that until we find one that isn't 
            {

            }
        }
        VarSet(VAR_RYU_JUKEBOX, song);
        PlayNextTrack();
    }

    if (gMain.newKeys & SELECT_BUTTON && (FlagGet(FLAG_RYU_DEV_MODE) == 1))
    {
            RemovePrintedNumber();
            RemoveExtraStartMenuWindows();
            if (!(MenuSpriteId1 == 0))
            {
                DestroySpriteAndFreeResources(&gSprites[MenuSpriteId1]);
                MenuSpriteId1 = 0;
            }

            HideStartMenu();
            HideFieldMessageBox();
            RemovePrintedNumber();
            ScriptContext2_Enable();
            ScriptContext1_SetupScript(RyuDebugMenuScript);
            return TRUE;
    }

    if (gMain.newKeys & DPAD_UP)
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(-1);
    }

    if (gMain.newKeys & DPAD_DOWN)
    {
        PlaySE(SE_SELECT);
        sStartMenuCursorPos = Menu_MoveCursor(1);
    }

    if (gMain.newKeys & DPAD_LEFT)
    {
        if (FlagGet(FLAG_RYU_HAS_FOLLOWER) == 1)
            TryMoveObjectEventToMapCoords(OBJ_EVENT_ID_FOLLOWER, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->pos.x, ((gSaveBlock1Ptr->pos.y) + 1));
    }

    if ((gMain.newKeys & DPAD_RIGHT) && (FlagGet(FLAG_RYU_JUKEBOX_ENABLED) == 0))
    {
        if (FlagGet(FLAG_RYU_RANDOMIZE_MUSIC) == 1)
        {
            FlagClear(FLAG_RYU_RANDOMIZE_MUSIC);
            PlaySE(SE_PIN);
        }
        else
        {
            FlagSet(FLAG_RYU_RANDOMIZE_MUSIC);
            PlaySE(SE_EXPMAX);
        }
    }


    if (gMain.newKeys & A_BUTTON)
    {
        PlaySE(SE_SELECT);
        if (sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func.u8_void == StartMenuPokedexCallback)
        {
            if (GetNationalPokedexCount(FLAG_GET_SEEN) == 0)
                return FALSE;
        }

        gMenuCallback = sStartMenuItems[sCurrentStartMenuActions[sStartMenuCursorPos]].func.u8_void;

        if (gMenuCallback != StartMenuSaveCallback
            && gMenuCallback != StartMenuExitCallback
            && gMenuCallback != StartMenuSafariZoneRetireCallback
            && gMenuCallback != StartMenuBattlePyramidRetireCallback)
        {
           FadeScreen(FADE_TO_BLACK, 0);
        }

        return FALSE;
    }

    if (gMain.newKeys & (START_BUTTON | B_BUTTON))
    {
        RemoveExtraStartMenuWindows();
        HideStartMenu();
        RemovePrintedNumber();
        if (!(MenuSpriteId1 == 0))
        {
            DestroySpriteAndFreeResources(&gSprites[MenuSpriteId1]);
            MenuSpriteId1 = 0;
        }
        return TRUE;
    }

        if (gMain.heldKeys & L_BUTTON && gMain.newKeys & R_BUTTON)
        {   
            u16 curmusic = 0;
            u16 song = 350;
            switch (FlagGet(FLAG_RYU_JUKEBOX_ENABLED))//0 for unset, 1 for set
            {
                case 0:
                    FlagSet(FLAG_RYU_JUKEBOX_ENABLED);
                    PlaySE(SE_PC_ON);
                    
                    if (VarGet(VAR_RYU_SAVED_BGM) > 350 && (VarGet(VAR_RYU_SAVED_BGM) < 558))
                    {
                        song = VarGet(VAR_RYU_SAVED_BGM);
                        song --;
                    }

                    VarSet(VAR_RYU_JUKEBOX, song);
                    PlayBGM(song);
                    PrintSongNumber(song);
                    break;
                case 1:
                    FlagClear(FLAG_RYU_JUKEBOX_ENABLED);
                    VarSet(VAR_RYU_SAVED_BGM, VarGet(VAR_RYU_JUKEBOX));
                    ResetMapMusic();
                    Overworld_ChangeMusicToDefault();
                    VarSet(VAR_RYU_JUKEBOX, 998);
                    PlaySE(SE_PC_OFF);
                    break;
                default:
                    break;
            }

        }

    return FALSE;
}

static bool8 StartMenuPokedexCallback(void)
{
    if (!gPaletteFade.active)
    {
        IncrementGameStat(GAME_STAT_CHECKED_POKEDEX);
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_OpenPokedex);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokemonCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PartyMenuFromStartMenu); // Display party menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_BagMenuFromStartMenu); // Display bag menu

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPokeNavCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitPokeNav);  // Display PokeNav

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuPlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();

        if (IsUpdateLinkStateCBActive() || InUnionRoom())
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card
        else if (FlagGet(FLAG_SYS_FRONTIER_PASS))
            ShowFrontierPass(CB2_ReturnToFieldWithOpenMenu); // Display frontier pass
        else
            ShowPlayerTrainerCard(CB2_ReturnToFieldWithOpenMenu); // Display trainer card

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuSaveCallback(void)
{
    if (InBattlePyramid())
        RemoveExtraStartMenuWindows();

    if (!(MenuSpriteId1 == 0))
    {
        DestroySpriteAndFreeResources(&gSprites[MenuSpriteId1]);
        MenuSpriteId1 = 0;
    }
    gMenuCallback = SaveStartCallback; // Display save menu

    return FALSE;
}

static bool8 StartMenuOptionCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_InitOptionMenu); // Display option menu
        gMain.savedCallback = CB2_ReturnToFieldWithOpenMenu;

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuExitCallback(void)
{
    if (gMain.newKeys & A_BUTTON || gMain.heldKeys & A_BUTTON)
    {
        HideStartMenu();
        RemovePrintedNumber();
        HideFieldMessageBox();
        ScriptContext2_Enable();
        ScriptContext1_SetupScript(RyuStartMenuConfigInfoScript);
        return TRUE;
    }
    else
    RemoveExtraStartMenuWindows();
    HideStartMenu(); // Hide start menu
    RemoveExtraStartMenuWindows();
    HideFieldMessageBox();
    RemovePrintedNumber();
    return TRUE;
}

static bool8 StartMenuSafariZoneRetireCallback(void)
{
    RemoveExtraStartMenuWindows();
    HideStartMenu();
    SafariZoneRetirePrompt();

    return TRUE;
}

static bool8 StartMenuLinkModePlayerNameCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        CleanupOverworldWindowsAndTilemaps();
        ShowTrainerCardInLink(gLocalLinkPlayerId, CB2_ReturnToFieldWithOpenMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 StartMenuBattlePyramidRetireCallback(void)
{
    gMenuCallback = BattlePyramidRetireStartCallback; // Confirm retire

    return FALSE;
}

// Functionally unused
void ShowBattlePyramidStartMenu(void)
{
    ClearDialogWindowAndFrameToTransparent(0, FALSE);
    ScriptUnfreezeObjectEvents();
    CreateStartMenuTask(Task_ShowStartMenu);
    ScriptContext2_Enable();
}

static bool8 StartMenuBattlePyramidBagCallback(void)
{
    if (!gPaletteFade.active)
    {
        PlayRainStoppingSoundEffect();
        RemoveExtraStartMenuWindows();
        CleanupOverworldWindowsAndTilemaps();
        SetMainCallback2(CB2_PyramidBagMenuFromStartMenu);

        return TRUE;
    }

    return FALSE;
}

static bool8 SaveStartCallback(void)
{
    InitSave();
    gMenuCallback = SaveCallback;

    return FALSE;
}

static bool8 SaveCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED: // Back to start menu
        ClearDialogWindowAndFrameToTransparent(0, FALSE);
        InitStartMenu();
        gMenuCallback = HandleStartMenuInput;
        return FALSE;
    case SAVE_SUCCESS:
    case SAVE_ERROR:    // Close start menu
        ClearDialogWindowAndFrameToTransparent(0, TRUE);
        ScriptUnfreezeObjectEvents();
        ScriptContext2_Disable();
        SoftResetInBattlePyramid();
        return TRUE;
    }

    return FALSE;
}

static bool8 BattlePyramidRetireStartCallback(void)
{
    InitBattlePyramidRetire();
    gMenuCallback = BattlePyramidRetireCallback;

    return FALSE;
}

static bool8 BattlePyramidRetireReturnCallback(void)
{
    InitStartMenu();
    gMenuCallback = HandleStartMenuInput;

    return FALSE;
}

static bool8 BattlePyramidRetireCallback(void)
{
    switch (RunSaveCallback())
    {
    case SAVE_SUCCESS: // No (Stay in Battle Pyramid)
        RemoveExtraStartMenuWindows();
        gMenuCallback = BattlePyramidRetireReturnCallback;
        return FALSE;
    case SAVE_IN_PROGRESS:
        return FALSE;
    case SAVE_CANCELED: // Yes (Retire from Battle Pyramid)
        ClearDialogWindowAndFrameToTransparent(0, TRUE);
        ScriptUnfreezeObjectEvents();
        ScriptContext2_Disable();
        ScriptContext1_SetupScript(BattlePyramid_Retire);
        return TRUE;
    }

    return FALSE;
}

static void InitSave(void)
{
    save_serialize_map();
    sSaveDialogCallback = SaveConfirmSaveCallback;
    sSavingComplete = FALSE;
}

static u8 RunSaveCallback(void)
{
    // True if text is still printing
    if (RunTextPrintersAndIsPrinter0Active() == TRUE)
    {
        return SAVE_IN_PROGRESS;
    }

    sSavingComplete = FALSE;
    return sSaveDialogCallback();
}

void SaveGame(void)
{
    InitSave();
    CreateTask(SaveGameTask, 0x50);
}

static void ShowSaveMessage(const u8 *message, u8 (*saveCallback)(void))
{
    StringExpandPlaceholders(gStringVar4, message);
    sub_819786C(0, TRUE);
    AddTextPrinterForMessage_2(TRUE);
    sSavingComplete = TRUE;
    sSaveDialogCallback = saveCallback;
}

static void SaveGameTask(u8 taskId)
{
    u8 status = RunSaveCallback();

    switch (status)
    {
    case SAVE_CANCELED:
    case SAVE_ERROR:
        gSpecialVar_Result = 0;
        break;
    case SAVE_SUCCESS:
        gSpecialVar_Result = status;
        break;
    case SAVE_IN_PROGRESS:
        return;
    }

    DestroyTask(taskId);
    EnableBothScriptContexts();
}

static void sub_80A0014(void)
{
    ClearDialogWindowAndFrame(0, TRUE);
}

static void HideSaveInfoWindow(void)
{
    RemoveSaveInfoWindow();
}

static void SaveStartTimer(void)
{
    sSaveDialogTimer = 60;
}

static bool8 SaveSuccesTimer(void)
{
    sSaveDialogTimer--;

    if (gMain.heldKeys & A_BUTTON)
    {
        PlaySE(SE_SELECT);
        return TRUE;
    }
    else if (sSaveDialogTimer == 0)
    {
        return TRUE;
    }

    return FALSE;
}

static bool8 SaveErrorTimer(void)
{
    if (sSaveDialogTimer != 0)
    {
        sSaveDialogTimer--;
    }
    else if (gMain.heldKeys & A_BUTTON)
    {
        return TRUE;
    }

    return FALSE;
}

static u8 SaveConfirmSaveCallback(void)
{
    ClearStdWindowAndFrame(GetStartMenuWindowId(), FALSE);
    RemoveStartMenuWindow();
    ShowSaveInfoWindow();

    if (InBattlePyramid())
    {
        ShowSaveMessage(gText_BattlePyramidConfirmRest, SaveYesNoCallback);
    }
    else
    {
        ShowSaveMessage(gText_ConfirmSave, SaveYesNoCallback);
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveYesNoCallback(void)
{
    DisplayYesNoMenuDefaultYes(); // Show Yes/No menu
    sSaveDialogCallback = SaveConfirmInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        switch (gSaveFileStatus)
        {
        case SAVE_STATUS_EMPTY:
        case SAVE_STATUS_CORRUPT:
            if (gDifferentSaveFile == FALSE)
            {
                sSaveDialogCallback = SaveFileExistsCallback;
                return SAVE_IN_PROGRESS;
            }

            sSaveDialogCallback = SaveSavingMessageCallback;
            return SAVE_IN_PROGRESS;
        default:
            sSaveDialogCallback = SaveFileExistsCallback;
            return SAVE_IN_PROGRESS;
        }
    case -1: // B Button
    case 1: // No
        HideSaveInfoWindow();
        sub_80A0014();
        return SAVE_CANCELED;
    }

    return SAVE_IN_PROGRESS;
}

// A different save file exists
static u8 SaveFileExistsCallback(void)
{
    if (gDifferentSaveFile == TRUE)
    {
        ShowSaveMessage(gText_DifferentSaveFile, SaveConfirmOverwriteDefaultNoCallback);
    }
    else
    {
        ShowSaveMessage(gText_AlreadySavedFile, SaveConfirmOverwriteCallback);
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteDefaultNoCallback(void)
{
    DisplayYesNoMenuWithDefault(1); // Show Yes/No menu (No selected as default)
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveConfirmOverwriteCallback(void)
{
    DisplayYesNoMenuDefaultYes(); // Show Yes/No menu
    sSaveDialogCallback = SaveOverwriteInputCallback;
    return SAVE_IN_PROGRESS;
}

static u8 SaveOverwriteInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        sSaveDialogCallback = SaveSavingMessageCallback;
        return SAVE_IN_PROGRESS;
    case -1: // B Button
    case 1: // No
        HideSaveInfoWindow();
        sub_80A0014();
        return SAVE_CANCELED;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveSavingMessageCallback(void)
{
    ShowSaveMessage(gText_SavingDontTurnOff, SaveDoSaveCallback);
    return SAVE_IN_PROGRESS;
}

static u8 SaveDoSaveCallback(void)
{
    u8 saveStatus;

    IncrementGameStat(GAME_STAT_SAVED_GAME);
    PausePyramidChallenge();

    if (gDifferentSaveFile == TRUE)
    {
        saveStatus = TrySavingData(SAVE_OVERWRITE_DIFFERENT_FILE);
        gDifferentSaveFile = FALSE;
    }
    else
    {
        saveStatus = TrySavingData(SAVE_NORMAL);
    }

    if (saveStatus == SAVE_STATUS_OK)
        ShowSaveMessage(gText_PlayerSavedGame, SaveSuccessCallback);
    else
        ShowSaveMessage(gText_SaveError, SaveErrorCallback);

    SaveStartTimer();
    return SAVE_IN_PROGRESS;
}

static u8 SaveSuccessCallback(void)
{
    if (!IsTextPrinterActive(0))
    {
        PlaySE(SE_SAVE);
        sSaveDialogCallback = SaveReturnSuccessCallback;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnSuccessCallback(void)
{
    if (!IsSEPlaying() && SaveSuccesTimer())
    {
        HideSaveInfoWindow();
        return SAVE_SUCCESS;
    }
    else
    {
        return SAVE_IN_PROGRESS;
    }
}

static u8 SaveErrorCallback(void)
{
    if (!IsTextPrinterActive(0))
    {
        PlaySE(SE_BOO);
        sSaveDialogCallback = SaveReturnErrorCallback;
    }

    return SAVE_IN_PROGRESS;
}

static u8 SaveReturnErrorCallback(void)
{
    if (!SaveErrorTimer())
    {
        return SAVE_IN_PROGRESS;
    }
    else
    {
        HideSaveInfoWindow();
        return SAVE_ERROR;
    }
}

static void InitBattlePyramidRetire(void)
{
    sSaveDialogCallback = BattlePyramidConfirmRetireCallback;
    sSavingComplete = FALSE;
}

static u8 BattlePyramidConfirmRetireCallback(void)
{
    ClearStdWindowAndFrame(GetStartMenuWindowId(), FALSE);
    RemoveStartMenuWindow();
    ShowSaveMessage(gText_BattlePyramidConfirmRetire, BattlePyramidRetireYesNoCallback);

    return SAVE_IN_PROGRESS;
}

static u8 BattlePyramidRetireYesNoCallback(void)
{
    DisplayYesNoMenuWithDefault(1); // Show Yes/No menu (No selected as default)
    sSaveDialogCallback = BattlePyramidRetireInputCallback;

    return SAVE_IN_PROGRESS;
}

static u8 BattlePyramidRetireInputCallback(void)
{
    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case 0: // Yes
        return SAVE_CANCELED;
    case -1: // B Button
    case 1: // No
        sub_80A0014();
        return SAVE_SUCCESS;
    }

    return SAVE_IN_PROGRESS;
}

static void sub_80A03D8(void)
{
    TransferPlttBuffer();
}

static bool32 sub_80A03E4(u8 *par1)
{
    switch (*par1)
    {
    case 0:
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0);
        SetVBlankCallback(NULL);
        ScanlineEffect_Stop();
        DmaClear16(3, PLTT, PLTT_SIZE);
        DmaFillLarge16(3, 0, (void *)(VRAM + 0x0), 0x18000, 0x1000);
        break;
    case 1:
        ResetSpriteData();
        ResetTasks();
        ResetPaletteFade();
        ScanlineEffect_Clear();
        break;
    case 2:
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sUnknown_085105A8, ARRAY_COUNT(sUnknown_085105A8));
        InitWindows(sUnknown_085105AC);
        LoadUserWindowBorderGfx_(0, 8, 224);
        Menu_LoadStdPalAt(240);
        break;
    case 3:
        ShowBg(0);
        BlendPalettes(-1, 16, 0);
        SetVBlankCallback(sub_80A03D8);
        EnableInterrupts(1);
        break;
    case 4:
        return TRUE;
    }

    (*par1)++;
    return FALSE;
}

void sub_80A0514(void)
{
    if (sub_80A03E4(&gMain.state))
    {
        CreateTask(sub_80A0550, 0x50);
        SetMainCallback2(sub_80A0540);
    }
}

static void sub_80A0540(void)
{
    RunTasks();
    UpdatePaletteFade();
}

static void sub_80A0550(u8 taskId)
{
    s16 *step = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        switch (*step)
        {
        case 0:
            FillWindowPixelBuffer(0, PIXEL_FILL(1));
            AddTextPrinterParameterized2(0,
                                        1,
                                        gText_SavingDontTurnOffPower,
                                        255,
                                        NULL,
                                        2,
                                        1,
                                        3);
            DrawTextBorderOuter(0, 8, 14);
            PutWindowTilemap(0);
            CopyWindowToVram(0, 3);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);

            if (gWirelessCommType != 0 && InUnionRoom())
            {
                if (sub_800A07C())
                {
                    *step = 1;
                }
                else
                {
                    *step = 5;
                }
            }
            else
            {
                gSoftResetDisabled = 1;
                *step = 1;
            }
            break;
        case 1:
            SetContinueGameWarpStatusToDynamicWarp();
            FullSaveGame();
            *step = 2;
            break;
        case 2:
            if (CheckSaveFile())
            {
                ClearContinueGameWarpStatus2();
                *step = 3;
                gSoftResetDisabled = 0;
            }
            break;
        case 3:
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
            *step = 4;
            break;
        case 4:
            FreeAllWindowBuffers();
            SetMainCallback2(gMain.savedCallback);
            DestroyTask(taskId);
            break;
        case 5:
            CreateTask(sub_8153688, 0x5);
            *step = 6;
            break;
        case 6:
            if (!FuncIsActiveTask(sub_8153688))
            {
                *step = 3;
            }
            break;
        }
    }
}

static void ShowSaveInfoWindow(void)
{
    struct WindowTemplate saveInfoWindow = sSaveInfoWindowTemplate;
    u8 gender;
    u8 color;
    u32 xOffset;
    u32 yOffset;

    if (!FlagGet(FLAG_SYS_POKEDEX_GET))
    {
        saveInfoWindow.height -= 2;
    }

    sSaveInfoWindowId = AddWindow(&saveInfoWindow);
    DrawStdWindowFrame(sSaveInfoWindowId, FALSE);

    gender = gSaveBlock2Ptr->playerGender;
    color = TEXT_COLOR_RED;  // Red when female, blue when male.

    if (gender == MALE)
    {
        color = TEXT_COLOR_BLUE;
    }

    // Print region name
    yOffset = 1;
    BufferSaveMenuText(SAVE_MENU_LOCATION, gStringVar4, TEXT_COLOR_GREEN);
    AddTextPrinterParameterized(sSaveInfoWindowId, 1, gStringVar4, 0, yOffset, 0xFF, NULL);

    // Print player name
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, 1, gText_SavingPlayer, 0, yOffset, 0xFF, NULL);
    BufferSaveMenuText(SAVE_MENU_NAME, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(1, gStringVar4, 0x70);
    PrintPlayerNameOnWindow(sSaveInfoWindowId, gStringVar4, xOffset, yOffset);

    // Print badge count
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, 1, gText_SavingBadges, 0, yOffset, 0xFF, NULL);
    BufferSaveMenuText(SAVE_MENU_BADGES, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(1, gStringVar4, 0x70);
    AddTextPrinterParameterized(sSaveInfoWindowId, 1, gStringVar4, xOffset, yOffset, 0xFF, NULL);

    if (FlagGet(FLAG_SYS_POKEDEX_GET) == TRUE)
    {
        // Print pokedex count
        yOffset += 16;
        AddTextPrinterParameterized(sSaveInfoWindowId, 1, gText_SavingPokedex, 0, yOffset, 0xFF, NULL);
        BufferSaveMenuText(SAVE_MENU_CAUGHT, gStringVar4, color);
        xOffset = GetStringRightAlignXOffset(1, gStringVar4, 0x70);
        AddTextPrinterParameterized(sSaveInfoWindowId, 1, gStringVar4, xOffset, yOffset, 0xFF, NULL);
    }

    // Print play time
    yOffset += 16;
    AddTextPrinterParameterized(sSaveInfoWindowId, 1, gText_SavingTime, 0, yOffset, 0xFF, NULL);
    BufferSaveMenuText(SAVE_MENU_PLAY_TIME, gStringVar4, color);
    xOffset = GetStringRightAlignXOffset(1, gStringVar4, 0x70);
    AddTextPrinterParameterized(sSaveInfoWindowId, 1, gStringVar4, xOffset, yOffset, 0xFF, NULL);

    CopyWindowToVram(sSaveInfoWindowId, 2);
}

static void RemoveSaveInfoWindow(void)
{
    ClearStdWindowAndFrame(sSaveInfoWindowId, FALSE);
    RemoveWindow(sSaveInfoWindowId);
}

static void sub_80A08A4(u8 taskId)
{
    if (!FuncIsActiveTask(sub_8153688))
    {
        DestroyTask(taskId);
        EnableBothScriptContexts();
    }
}

void sub_80A08CC(void)
{
    u8 taskId = CreateTask(sub_8153688, 0x5);
    gTasks[taskId].data[2] = 1;
    gTasks[CreateTask(sub_80A08A4, 0x6)].data[1] = taskId;
}

static void HideStartMenuWindow(void)
{
    ClearStdWindowAndFrame(GetStartMenuWindowId(), TRUE);
    RemoveStartMenuWindow();
    RemovePrintedNumber();
    if (!(MenuSpriteId1 == 0))
        {
            DestroySpriteAndFreeResources(&gSprites[MenuSpriteId1]);
            MenuSpriteId1 = 0;
        }
    ScriptUnfreezeObjectEvents();
    ScriptContext2_Disable();
}

void HideStartMenu(void)
{
    PlaySE(SE_SELECT);
    HideStartMenuWindow();
}

void AppendToList(u8 *list, u8 *pos, u8 newEntry)
{
    list[*pos] = newEntry;
    (*pos)++;
}
