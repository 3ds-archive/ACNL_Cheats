#pragma once

#include "types.h"

using ItemData = std::tuple<std::string, u16>;

struct TramplePkt
{
	u32 item;
	u8  roomID, wX, wY, u0;
};

struct dropInfo
{
	u32 itemToRemove;
	u32 itemToShow;
	u32 itemToPlace;
	u8  unknown0;
	u8  playerindex;
	u8  wX;
	u8  wY;
	u8  dropID;
	u8  unknown1;
	u8  unknown2;
	u8  unknown3;
	u8  unknown4;
	u8  roomID;
};

struct dropPkt
{
	u32 replaceID;
	u32 placeID;
	u32 showID;
	u8  combinedDropIDPlayerIndex;
	u8  processFlags;
	u8  paramFlags;
	u8  flags;
	u8  u0;
	u8  u1;
	u8  roomID;
	u8  wX;
	u8  wY;
	u8  u2;
};

struct ITEM_MAP
{
	const u16	id;
	const char* name;
	const char* yomi;
};

struct RoomData
{
	const char* name;
	const u8	id;
};