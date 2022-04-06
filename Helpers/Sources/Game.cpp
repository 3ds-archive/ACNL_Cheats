#include "CTRPluginFramework.hpp"
#include "Helpers.hpp"

namespace CTRPluginFramework
{
	u8 DropType = 0xA, waitAnim = 0x56;
	u32 UseItem = 0x2117;
	
	volatile u32(*fp0)();
	volatile u32(*fp1)(u32);
	volatile u32(*fp2)(u32,u32);
	volatile u32(*fp3)(u32,u32,u32);
	volatile u32(*fp4)(u32,u32,u32,u32);
	volatile u32(*fp5)(u32,u32,u32,u32,u32);
	volatile u32(*fp6)(u32,u32,u32,u32,u32,u32);
	volatile u32(*fp7)(u32,u32,u32,u32,u32,u32,u32);
	volatile u32(*fp8)(u32,u32,u32,u32,u32,u32,u32,u32);
	volatile u32(*fp9)(u32,u32,u32,u32,u32,u32,u32,u32,u32);
	volatile u32(*fp10)(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32);
	volatile u32(*fp11)(u32,u32,u32,u32,u32,u32,u32,u32,u32,u32,u32);
	
	void Game::Warp(u8 roomID)
	{
		Process::Write32((u32)&fp4, 0x304A94);
		fp4(roomID, 1, 1, 0);
	}
	
	u8 Game::GetOnlinePlayerIndex()
	{
		Process::Write32((u32)&fp0, 0x305F6C);
		return fp0();
	}
	
	u8 Game::GetActualPlayerIndex()
	{
		u8 index = *(u8 *)((*(u32 *)0x94D644) + 0x13268);
		if (index >= 4) return 0;
		else return index;
	}
	
	u8 Game::GetOnlinePlayerCount()
	{
		Process::Write32((u32)&fp1, 0x75D798);
		return fp1( *(u32*)0x94D644 );
	}
	
	u32 * Game::GetItemAtWorldCoords(u32 wx, u32 wy)
	{
		Process::Write32((u32)&fp4, 0x2FEE38);
		return (u32*)fp4(Game::GetCurrentMap(), wx, wy, 0);
	}
	
	u32 Game::GetCurrentMap()
	{
		Process::Write32((u32)&fp0, 0x6A53DC);
		return fp0();
	}
	
	void Game::PlaceBuildingUpdateCollisions(u32 wx, u32 wy, u16 BuildingID)
	{
		Process::Write32((u32)&fp3, 0x2424F4);
		fp3(wx, wy, BuildingID);
	}
	
	bool Game::DropCheck(u32 &wx, u32 &wy)
	{
		u32 u0;
		Process::Write32((u32)&fp5, 0x5979E4);
		return fp5((u32)&wx, (u32)&wy, (u32)&u0, 0, 0);
	}
	
	void pseudoType0x21(dropPkt *pkt)
	{
		dropPkt *drop = new dropPkt();
		u32 x, y;
		s8 diffX, diffY;
		u32 *item;
		
		drop->replaceID = 0x71;
		drop->placeID = pkt->placeID;
		drop->showID = 0x71;
		drop->paramFlags = 0;
		drop->processFlags = 0;
		drop->u0 = 0;
		drop->u1 = 0;
		drop->u2 = 0;
		drop->roomID = pkt->roomID;
		drop->wX = pkt->wX;
		drop->wY = pkt->wY;
		drop->combinedDropIDPlayerIndex = Game::GetOnlinePlayerIndex() & 3 | 0x84;
		drop->processFlags = drop->processFlags & 0xC3 | (Game::GetOnlinePlayerCount() > 1 ? 0x1C : 0xC);
		drop->flags = 0;
		Player::GetWorldCoords(&x, &y, 4, 1);
		pkt->wX = x;
		pkt->wY = y;
		pkt->replaceID = 0x71;
		item = Game::GetItemAtWorldCoords(x, y);
		if (item == nullptr) return;
		pkt->placeID = *item;
		pkt->showID = drop->placeID == 0x7FFE ? 0x2001 : drop->placeID;
		pkt->paramFlags = 0x8;
		diffX = drop->wX - x;
		diffY = drop->wY - y;
		pkt->flags = (((diffX + 4) & 0xF) | (((diffY + 4) & 0xF) << 4));
		pkt->u0 = 0;
		pkt->u1 = 0;
		pkt->u2 = 0;
		
		Process::Write32((u32)&fp1, 0x59E8F8);
		fp1((u32)drop);
	}
	
	bool Game::DropItem(u32 item, u32 wx, u32 wy)
	{
        static Hook hook;
        u32  player = Player::GetInstance(4, 1);
		u32  animInstance;
		u8   autoWaitAnim = waitAnim;
		u8   currentIndex = Game::GetOnlinePlayerIndex();
		bool forced = (currentIndex != Game::GetActualPlayerIndex()) && (Game::GetActualPlayerIndex() <= 3);
		
		u32 *pItem = Game::GetItemAtWorldCoords(wx, wy);
		if( pItem == nullptr ) return false;
		if( *pItem != 0x7FFE ) return false;
		
		if( GetLockedSpotIndex(wx, wy, 0xA5) != 0xFFFFFFFF ) return false;
		
		if( (u16)item == 0x7FFE || (u16)item > 0x372A ) return false;
		if( *(u8*)0x33099f7c == 0xFF && ((u16)item == 0x207A || (u16)item == 0x2079 || ((u16)item > 0xFD && (u16)item < 0x2000)) ) return false;
		if( *(u8*)0x33099f7c != 0xFF && ((u16)item < 0x2000 || (item & 0x80000000) != 0 || DropType == 8 || DropType == 9 || DropType == 0xE || DropType == 0xF) )
			return false;
		
		u32 itemrp = 0x7FFE;
		if( DropType == 9 || DropType == 0xF ) itemrp = 0x26;
		if( DropType == 8 || DropType == 0xE ) itemrp = 0x3E;
		if( DropType >= 3 && DropType <= 5 ) itemrp = 0x2001;
		
        if( DropType == 0x21 )
		{
			if( player == 0 ) return 0;
			if( !hook.IsEnabled() )
			{
				Process::Write32(0x59EDD8, 0xE1A00000);
				Sleep(Milliseconds(5));
				hook.Initialize(0x59EDD8, (u32)&pseudoType0x21);
				hook.Enable();
			}
			Sleep(Milliseconds(5));
		}
		else
		{
			hook.Disable();
			//Sleep(Milliseconds(5));
			Process::Write32(0x59EDD8, 0xEB000019);
		}
        
		Process::Write32((u32)&fp11, 0x59E5B4);
		fp11(DropType == 0x21 ? 0xF : DropType, (u32)&itemrp, (u32)&item, (u32)&item, wx, wy, 0, 0, DropType == 8 || DropType == 0xE, 0, 0);
		
		if( !(DropType >= 0xA && DropType <= 0xD) && DropType != 0x22 )
		{
			if( player == 0 ) return false;
			animInstance = Player::GetAnimationInstance(player, 0, 0, 0);
			*(u32*)(player + 0x844) = 0;
			*(u8*)(player + 0x8CC) = DropType;
			
			Process::Write32((u32)&fp2, 0x5D3504);
			
			if( DropType >= 0x1 && DropType <= 0x6 )
			{
				if( forced )
				{
					if( DropType == 1 || DropType == 2 ) autoWaitAnim = 0x3D;
					if( DropType >= 3 && DropType <= 5 ) autoWaitAnim = 0x41;
					if( DropType == 6 ) autoWaitAnim = 0x40;
				}
				else
				{
					autoWaitAnim = 0x3C;
				}
				
				Process::Write32(animInstance + 0xE, 0x7FFE);
				Process::Write8(animInstance + 0x12, DropType == 0x21 ? *(u8*)(player + 0x47C) : wx);
				Process::Write8(animInstance + 0x13, DropType == 0x21 ? *(u8*)(player + 0x480) : wy);
				Process::Write8(animInstance + 0x15, DropType);
				
				fp2( animInstance + 0x17, (u32)Player::GetCoordinates(currentIndex) );
			}
			else
			{
				*(u8*)(animInstance + 0xE) = DropType == 0x21 ? *(u8*)(player + 0x47C) : wx;
				*(u8*)(animInstance + 0xF) = DropType == 0x21 ? *(u8*)(player + 0x480) : wy;
				*(u16*)(animInstance + 0x10) = 0x2001;
				
				if( DropType == 0x7 && waitAnim != 0x5C && waitAnim != 0x6A )
					autoWaitAnim = forced ? 0x5D : 0x5C;
				else if( DropType >= 0x8 && DropType <= 0xF && waitAnim != 0x6A )
					autoWaitAnim = forced ? 0x6B : 0x6A;
				else if( forced )
					autoWaitAnim = 0x4C;
				
				if( ((DropType >= 0x13 && DropType <= 0x1C) || autoWaitAnim == 0x4A) && !forced )
				{
					autoWaitAnim = 0x4A;
					*(u32*)0x680F28 = 0xE3A00000;
					Process::Write16(animInstance + 0x10, DropType);
					Sleep(Milliseconds(5));
				}
				else if( forced )
					autoWaitAnim = 0x4F;
			}
			
			if( currentIndex == Game::GetActualPlayerIndex() )
				Player::ExecuteAnimation(player, autoWaitAnim, animInstance, 0);
			else
				Player::SendAnimPacket(Game::GetActualPlayerIndex(), animInstance, autoWaitAnim, Player::GetRoom(currentIndex), currentIndex);	
			
			Sleep(Milliseconds(40));
			*(u32 *)0x680F28 = 0xEBFC52D7;
		}
		
		return true;
	}
	
	void Game::Particles(u32 particleID, float *coords)
	{
		Process::Write32((u32)&fp4, 0x207AD0);
		fp4(particleID, (u32)coords, 0x96fc06, 0xadf870);
	}
	
	void Game::clearLockedSpot(u8 wx, u8 wy, u8 roomID, u32 p4)
	{
		Process::Write32((u32)&fp4, 0x59FBB0);
		fp4(wx, wy, roomID, p4);
	}
	
	u32 Game::createLockedSpot(u8 DropID, u8 wX, u8 wY, u8 roomID, bool sendPkt)
	{
		u32 index;
		
		Process::Write32((u32)&fp5, 0x59FCFC);
		if (*(u32 *)0x59FD00 == 0xE3A00000)
		{
			*(u32 *)0x59FD00 = 0xE24DD01C;
			*(u32 *)0x59FD04 = 0xE1A07001;
			
			index = fp5(DropID, wX, wY, roomID, sendPkt);
			
			*(u32 *)0x59FD00 = 0xE3A00000;
			*(u32 *)0x59FD04 = 0xE8BD83F0;
			
			return index;
		}
		
		return fp5(DropID, wX, wY, roomID, sendPkt);
	}

	u32 Game::GetLockedSpotIndex(u8 wX, u8 wY, u8 roomID)
	{
		Process::Write32((u32)&fp3, 0x59FAF4);
		return fp3(wX, wY, roomID);
	}
	
	void Game::TrampleAt(u8 wx, u8 wy)
	{
		u32 *pItem = Game::GetItemAtWorldCoords(wx, wy);
		if( pItem != nullptr )
		{
			u8 room = Player::GetRoom(Game::GetActualPlayerIndex()); 
			if( Game::GetOnlinePlayerCount() != 0 )
			{	
				TramplePkt pkt{ *pItem, room, wx, wy, 0 };
				Process::Write32((u32)&fp4, 0x623F80);
				fp4(0x47, 4, (u32)&pkt, 8);
			}
			
			u32 w = 0x7FFE;
			Process::Write32((u32)&fp5, 0x168E08);
			Process::Write32((u32)&fp3, 0x59DA7C);
			fp5(wx, wy, 0, room, (u32)&w);
			fp3(wx, wy, 1);
		}
	}
	
	float * Game::WorldCoordsToCoords(u8 wx, u8 wy, float *coords)
	{
		float *c = Player::GetCoordinates();
		if( c != nullptr ) coords[1] = c[1];
		coords[0] = wx * 0x20 + 0x10;
		coords[2] = wy * 0x20 + 0x10;
		return coords;
	}
	
	
	bool Camera::Lock()
	{
		return Process::Write32(0x1A5128, 0xE8BD81F0);
	}
	
	bool Camera::Unlock()
	{
		return Process::Write32(0x1A5128, 0xE2805C01);
	}
	
	bool Camera::IsLocked()
	{
		return *(u32*)0x1A5128 == 0xE8BD81F0;
	}
	
	void Camera::AllowRotation(bool allow)
	{
		Process::Write32(0x1A3190, allow ? 0xE1A00000 : 0xE18020B4);
		Process::Write32(0x1A319C, allow ? 0xE1A00000 : 0xE18020B4);
	}
	
	float * Camera::GetCoords()
	{
		return (float*)( *(u32*)0x94a880 + 4 );
	}
	
	u16 * Camera::RotationX()
	{
		return (u16*)( *(u32*)0x94a880 + 0x1c );
	}
	
	u16 * Camera::RotationY()
	{
		return (u16*)( *(u32*)0x94a880 + 0x1e );
	}
	
	
	
	
}

