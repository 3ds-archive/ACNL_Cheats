#pragma once

namespace CTRPluginFramework
{
   class Game
	{
	public:
		static void		Warp(u8 roomID);
		static u8		GetOnlinePlayerIndex();
		static u8		GetActualPlayerIndex();
		static u8		GetOnlinePlayerCount();
		static u32		*GetItemAtWorldCoords(u32 wx, u32 wy);
		static u32		GetCurrentMap();
		static void		PlaceBuildingUpdateCollisions(u32 x, u32 y, u16 buildingID);
		static bool		DropCheck(u32 &wx, u32 &wy);
		static bool		DropItem(u32 item, u32 wx, u32 wy);
		static void		Particles(u32 particleID, float *coords);
		static void		clearLockedSpot(u8 wX, u8 wY, u8 roomID, u32 p4 = 4);
		static u32		createLockedSpot(u8 DropID, u8 wX, u8 wY, u8 roomID, bool sendPkt = 1);
		static u32		GetLockedSpotIndex(u8 wX, u8 wY, u8 roomID = 0xA5);
		static void		TrampleAt(u8 wx, u8 wy);
		static float	*WorldCoordsToCoords(u8 wx, u8 wy, float *coords);
	};
	
	class Camera
	{
	public:
		static bool		Lock();
		static bool		Unlock();
		static bool		IsLocked();
		static void		AllowRotation(bool allow);
		static float	*GetCoords();
		static u16		*RotationX();
		static u16		*RotationY();
	};
}