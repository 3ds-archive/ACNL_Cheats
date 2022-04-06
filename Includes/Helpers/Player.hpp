#pragma once

namespace CTRPluginFramework
{
	class Player
	{
	public:
		static u32		GetInstance(u8 PlayerIndex, bool noChecks);
		static bool		Exists(u8 PlayerIndex);
		static bool		GetWorldCoords(u32 *worldx, u32 *worldy, u8 PlayerIndex, bool noChecks);
		static u32		GetAnimationInstance(u32 playerInstance, u8 u0, u8 u1, u32 u2);
		static bool		ExecuteAnimation(u32 pPlayerObj, u8 animID, u32 pAnimObj, bool u0);
		static bool		ExecuteAnimationWrapper(u8 playerIndex, u8 animID, u32 animItem, u8 emotion, u16 snake, u16 sound, bool u0, u8 wX, u8 wY, bool directSend = false, u8 appearance[] = nullptr);
		static bool		Pickup(u32 PlayerObj, u8 worldx, u8 worldy, bool u0, bool u1, bool u2);
		static float	*GetCoordinates(u8 PlayerIndex = 4);
		static bool		GetCoordinatesRef(u8 PlayerIndex, float &X, float &Z);
		static float	*GetCollisionSize(u8 PlayerIndex = 4);
		static u8		*GetAnimation(u8 PlayerIndex = 4);
		static u8		GetLocation();
		static u32		*GetHeldItem();
		static float	*GetVelocity(u8 PlayerIndex = 4);
		static u8		GetRoom(u8 PlayerIndex);
		static u32		*GetInvSlotAddr(int slot);
		static void		SendAnimPacket(u8 senderIndex, u32 animObj, u8 animID, u8 roomID, u8 targetPlayerIndex);
	};
}

