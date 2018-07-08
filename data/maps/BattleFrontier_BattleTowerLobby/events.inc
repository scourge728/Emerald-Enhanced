BattleFrontier_BattleTowerLobby_EventObjects: @ 853B638
	object_event 1, EVENT_OBJ_GFX_TEALA, 0, 6, 5, 3, 8, 0, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23E936, 0
	object_event 2, EVENT_OBJ_GFX_MAN_5, 0, 23, 5, 3, 8, 0, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23EEE7, 0
	object_event 3, EVENT_OBJ_GFX_WOMAN_7, 0, 17, 8, 3, 2, 1, 1, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23EFB5, 0
	object_event 4, EVENT_OBJ_GFX_BOY_1, 0, 15, 9, 3, 2, 1, 1, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23EFBE, 0
	object_event 5, EVENT_OBJ_GFX_REPORTER_F, 0, 11, 9, 3, 10, 0, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_28CB96, 918
	object_event 6, EVENT_OBJ_GFX_HIPSTER, 0, 1, 5, 3, 8, 1, 1, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_2B688D, 701
	object_event 7, EVENT_OBJ_GFX_TEALA, 0, 10, 5, 3, 8, 0, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23EA9F, 0
	object_event 8, EVENT_OBJ_GFX_TEALA, 0, 14, 5, 3, 8, 0, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23EC08, 0
	object_event 9, EVENT_OBJ_GFX_TEALA, 0, 18, 5, 3, 8, 0, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23ED74, 0

BattleFrontier_BattleTowerLobby_MapWarps: @ 853B710
	warp_def 12, 9, 3, 0, MAP_BATTLE_FRONTIER_OUTSIDE_EAST
	warp_def 13, 9, 3, 0, MAP_BATTLE_FRONTIER_OUTSIDE_EAST
	warp_def 6, 1, 0, 0, MAP_BATTLE_FRONTIER_BATTLE_TOWER_BATTLE_ROOM

BattleFrontier_BattleTowerLobby_MapBGEvents: @ 853B728
	bg_event 4, 5, 0, 1, 0, BattleFrontier_BattleTowerLobby_EventScript_23EFC7
	bg_event 8, 5, 0, 1, 0, BattleFrontier_BattleTowerLobby_EventScript_23EFE0
	bg_event 12, 5, 0, 1, 0, BattleFrontier_BattleTowerLobby_EventScript_23EFF9
	bg_event 16, 5, 0, 1, 0, BattleFrontier_BattleTowerLobby_EventScript_23F012
	bg_event 2, 1, 0, 0, 0, BattleFrontier_BattleTowerLobby_EventScript_23F4BE

BattleFrontier_BattleTowerLobby_MapEvents:: @ 853B764
	map_events BattleFrontier_BattleTowerLobby_EventObjects, BattleFrontier_BattleTowerLobby_MapWarps, 0x0, BattleFrontier_BattleTowerLobby_MapBGEvents
