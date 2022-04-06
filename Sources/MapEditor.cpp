#include "cheats.hpp"

#define		TILE_SIZE			18
#define		UI_WIDTH			11
#define		UI_HEIGHT			9
#define		UI_DRAW_SX			( 160 - (UI_WIDTH * TILE_SIZE) / 2 )
#define		UI_DRAW_SY			( 112 - (UI_HEIGHT * TILE_SIZE) / 2 )

#define		B_KIDOU				ZR + DU
#define		B_SET_CAM_PLAYER	ZR + B
#define		B_SYNC_CAMERA		ZR + DR
#define		B_REMOVAL			ZR + DL
#define		B_PEN_SIZE			ZR + DD
#define		B_CHECK_ITEM		L
#define		B_NO_TOUCH			ZR + L

#define		B_NO_MOVE			ZR

namespace CTRPluginFramework
{
	extern u8 waitAnim;
	extern u8 DropType;
	extern u32 itemIDToReplace;
	u32 PlayerWorldX, PlayerWorldY;
	int TouchX, TouchY;
	int MapX, MapY;
	int MapWidth, MapHeight;
	
	u8 pen_size = 0;
	
	bool MapEditor_enabled;
	bool NoTouch = true;
	bool SyncCamera = false;
	bool deleteMode = false;
	bool drawItemInfo = false;
	bool drawTouchCursor;
	string info_str;
	
	float particle_Coords[3];
	
	extern u32 itemRnd_start, itemRnd_end;
	extern bool itemRnd_enabled;
	void tchD(u8 wx, u8 wy)
	{
		bool random = pen_size > 0 && itemRnd_enabled && PluginMenu::GetRunningInstance()->GetFolderList()[1]->GetEntryList()[4]->IsActivated();
		for( u8 y = wy - pen_size; y <= wy + pen_size; y++ )
			for( u8 x = wx - pen_size; x <= wx + pen_size; x++ )
			{
				if( NoTouch || (x >= 0x10 && y >= 0x10 && x <= MapWidth && y <= MapHeight) )
				{
					if( random )
						UseItem = Utils::Random(itemRnd_start, itemRnd_end);
					
					if( deleteMode )
						Game::TrampleAt(x, y);
					else
						Game::DropItem(UseItem, x, y);
					
					if( NoTouch ) continue;
					
					particle_Coords[0] = x * 0x20 + 0x10;
					particle_Coords[2] = y * 0x20 + 0x10;
					Game::Particles(0x20C, particle_Coords);
				}
			}
	}
	
	bool MapEditor_osd(const Screen &);
	void MapEditor_control();
	void MapEditor(MenuEntry *e)
	{
		Player::GetWorldCoords(&PlayerWorldX, &PlayerWorldY, 4, 1);
		particle_Coords[1] = *(float*)( 0x33099E54 + get12c() );
		
		// モード切替
		if( !MapEditor_enabled && Controller::IsKeysPressed(B_NO_TOUCH) )
		{
			NoTouch = !NoTouch;
			Notify(NoTouch ? Color::Red << "タッチモード: オフ" : Color::Green << "タッチモード: オン");
		}
		
		if( !NoTouch )
		{
			if( *(u32*)0x33099E50 == 0 ) return;
			if( *(u8*)0x95133A != 0 && *(u8*)0x95133A != 0x68 ) return;
		}
		
		// エディタ 起動
		if( Controller::IsKeysPressed(B_KIDOU) )
		{
			if( !MapEditor_enabled )
			{
				switch( *(u8*)0x95133A )
				{
				case 0:
					MapWidth = 0x5F;
					MapHeight = 0x4F;
					break;
				case 0x68:
					MapWidth = 0x2F;
					MapHeight = 0x2F;
					break;
				}
				
				if( NoTouch || (PlayerWorldX >= 0x10 && PlayerWorldY >= 0x10 && PlayerWorldX <= MapWidth && PlayerWorldY <= MapHeight) )
				{
					MapEditor_enabled = true;
					MapX = PlayerWorldX - (UI_WIDTH - 1) / 2;
					MapY = PlayerWorldY - (UI_HEIGHT - 1) / 2;
					
					Process::Write32(0x47D170, 0x3FE80000);
					
					if( !NoTouch )
					{
						u32 wx, wy; Player::GetWorldCoords(&wx, &wy, 4, 1);
						Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex(), 0x16, 0, 1, 1, 1, 0, wx, wy, true);
					}
					else
					{
						SyncCamera = true;
					}
					
					if( SyncCamera )
					{
						Process::Patch(0x1A5128, 0xE8BD81F0);
						Camera::GetCoords()[1] += 90.0f;
						*Camera::RotationX() += 0x700;
					}
					OSD::Run(MapEditor_osd);
				}
			}
			else
			{
				MapEditor_enabled = false;
				Process::Write32(0x1A5128, 0xE2805C01);
				Process::Write32(0x47D170, 0x40000000);
				
				if( !NoTouch )
				{
					u32 wx, wy; Player::GetWorldCoords(&wx, &wy, 4, 1);
					Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex(), 6, 0, 1, 1, 1, 0, wx, wy, true);
				}
				
				OSD::Stop(MapEditor_osd);
			}
		}
		
		if( MapEditor_enabled ) MapEditor_control();
	}
	
	void MapEditor_control()
	{
		static u32 tick;
		drawTouchCursor = false;
		drawItemInfo = false;
		
		// 十字キー 移動
		if( ((NoTouch || !Controller::IsKeyDown(A)) && !Controller::IsKeyDown(B_NO_MOVE)) && Controller::IsKeyDown(DPad) )
		{
			if( tick == 0 || tick == 14 )
			{
				u32 key = Controller::GetKeysDown();
				if( key & DU ) MapY--;
				if( key & DD ) MapY++;
				if( key & DL ) MapX--;
				if( key & DR ) MapX++;
				if( tick >= 14 ) tick -= 2;
			}
			tick++;
		}
		else tick = 0;
		
		// 範囲制限
		if( !NoTouch )
		{
			if( MapX < 14 ) MapX = 14;
			if( MapY < 12 ) MapY = 12;
			if( MapX > MapWidth - UI_WIDTH + 3 ) MapX = MapWidth - UI_WIDTH + 3;
			if( MapY > MapHeight - UI_HEIGHT + 6 ) MapY = MapHeight - UI_HEIGHT + 6;
		}
		
		// エディタの座標をプレイヤー位置に合わせる
		if( Controller::IsKeysPressed(B_SET_CAM_PLAYER) )
		{
			MapX = PlayerWorldX - (UI_WIDTH - 1) / 2;
			MapY = PlayerWorldY - (UI_HEIGHT - 1) / 2;
		}
		
		// カメラ同期　切り替え
		if( !NoTouch && Controller::IsKeysPressed(B_SYNC_CAMERA) )
		{
			// OFF
			if( SyncCamera )
			{
				Process::Write32(0x47D170, 0x40000000);
				Process::Patch(0x1A5128, 0xE2805C01);
				SyncCamera = false;
			}
			// ON
			else
			{
				Process::Write32(0x47D170, 0x3FE80000);
				Process::Patch(0x1A5128, 0xE8BD81F0);
				Camera::GetCoords()[1] += 90.0f;
				*Camera::RotationX() += 0x700;
				SyncCamera = true;
			}
		}
		
		// カメラ同期==Trueだったらカメラ座標をMapX,MapYに合わせる
		if( SyncCamera )
		{
			Camera::GetCoords()[0] = (float)((MapX + (UI_WIDTH - 1) / 2) * 0x20 + 0x10);
			Camera::GetCoords()[2] = (float)((MapY + (UI_HEIGHT - 1) / 2) * 0x20 + 0x70);
		}
		
		// 削除モード切替
		if( Controller::IsKeysPressed(B_REMOVAL) )
		{
			TOGGLE_BOOL( deleteMode );
		}
		
		// ペンサイズ変更
		if( Controller::IsKeysPressed(B_PEN_SIZE) )
		{
			pen_size += 1;
			if( pen_size > 4 ) pen_size = 0;
		}
		
		// パーティクル (ボタンモードのみ)
		if( NoTouch )
		{
			u8 wx = MapX + (UI_WIDTH - 1) / 2;
			u8 wy = MapY + (UI_HEIGHT - 1) / 2;
			u8 i = 0;
			for( int y = wy - pen_size; y <= wy + pen_size; y++ )
				for( int x = wx - pen_size; x <= wx + pen_size; x++ )
				{
					particle_Coords[0] = x * 0x20 + 0x10;
					particle_Coords[2] = y * 0x20 + 0x10;
					Game::Particles((i % 2) ? 0x20C : 0x214, particle_Coords);
					i++;
				}
		}
		
		// Aで置く/消す (ボタンモードのみ)
		if( NoTouch && Controller::IsKeyDown(A) )
		{
			tchD( MapX + (UI_WIDTH - 1) / 2, MapY + (UI_HEIGHT - 1) / 2 );
		}
		
		if( NoTouch ) return;
		
		// タッチ
		UIntVector tpos = Touch::GetPosition();
		TouchX = (tpos.x - UI_DRAW_SX) / TILE_SIZE;
		TouchY = (tpos.y - UI_DRAW_SY) / TILE_SIZE;
		if( Touch::IsDown() && TouchX >= 0 && TouchY >= 0 && TouchX < UI_WIDTH && TouchY < UI_HEIGHT && Player::GetInstance(4, 1) != 0 )
		{
			// アイテム情報表示
			if( Controller::IsKeyDown(B_CHECK_ITEM) &&
				MapX + TouchX >= 0x10 && MapY + TouchY >= 0x10 && MapX + TouchX <= MapWidth && MapY + TouchY <= MapHeight )
			{
				drawItemInfo = true;
				u32 item;
				GetItem(MapX + TouchX, MapY + TouchY, item);
                std::string s;
                GetItemName((u16)item, s);
				info_str = Utils::Format("%04X", (u16)item) << s;
			}
			else
			{
				drawTouchCursor = true;
				tchD(MapX + TouchX, MapY + TouchY);
			}
		}
	}
	
	Color GetItemColor(u16 item);
	bool MapEditor_osd(const Screen &screen)
	{
		if( NoTouch ? !screen.IsTop : screen.IsTop ) return false;
		
		// ペンサイズ
		screen.Draw(Utils::Format("PenSize = %d", pen_size + 1), 0, NoTouch ? 220 : 0);
		
		// 削除モード表示
		if( deleteMode == true )
		{
			screen.Draw(Color::Red << "Removal", 0, NoTouch ? 210 : 10);
		}
		
		if( NoTouch ) return false;
		
		u32 item;
		for( u8 y = 0; y < UI_HEIGHT; y++ )
			for( u8 x = 0; x < UI_WIDTH; x++ )
			{
				// 背景
				if( MapX + x >= 0x10 && MapY + y >= 0x10 && MapX + x <= MapWidth && MapY + y <= MapHeight )
				{
					GetItem(MapX + x, MapY + y, item);
					screen.DrawRect( UI_DRAW_SX + x * TILE_SIZE, UI_DRAW_SY + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GetItemColor((u16)item) );
				}
				
				// プレイヤー 白色
				if( MapX + x == PlayerWorldX && MapY + y == PlayerWorldY )
					screen.DrawRect( UI_DRAW_SX + x * TILE_SIZE, UI_DRAW_SY + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, Color::White);
				
				// マス目 黒色
				screen.DrawRect( UI_DRAW_SX + x * TILE_SIZE, UI_DRAW_SY + y * TILE_SIZE, TILE_SIZE + 1, TILE_SIZE + 1, Color::Black, false);
			}
		
		// X座標表示
		for( int x = 0; x < UI_WIDTH; x++ )
		{
			if( MapX + x >= 0x10 && MapX + x <= MapWidth )
				screen.Draw( Utils::Format("%02D", MapX + x - 0x10), UI_DRAW_SX + x * TILE_SIZE, UI_DRAW_SY - 10 );
		}
		
		// Y座標表示
		for( int y = 0; y < UI_HEIGHT; y++ )
		{
			if( MapY + y >= 0x10 && MapY + y <= MapHeight )
				screen.Draw( Utils::Format("%02D", MapY + y - 0x10), UI_DRAW_SX - 14, UI_DRAW_SY + y * TILE_SIZE );
		}
		
		// ペン描画
		if( drawTouchCursor )
		{
			for( int y = TouchY - pen_size; y <= TouchY + pen_size; y++ )
			{
				for( int x = TouchX - pen_size; x <= TouchX + pen_size; x++ )
				{
					if( x >= 0 && y >= 0 && x < UI_WIDTH && y < UI_HEIGHT )
						screen.DrawSysfont("+", UI_DRAW_SX + x * TILE_SIZE + 4, UI_DRAW_SY + y * TILE_SIZE);
				}
			}
		}
		
		// アイテム情報
		if( drawItemInfo )
		{
			screen.DrawRect(160 - OSD::GetTextWidth(true, info_str.substr(4)) / 2, 208, OSD::GetTextWidth(true, info_str.substr(4)), 16, Color::Black);
			screen.Draw(info_str.substr(0, 4), UI_DRAW_SX, 198);
			
			if( info_str.length() > 4 )
				screen.DrawSysfont(info_str.substr(4), 160 - OSD::GetTextWidth(true, info_str.substr(4)) / 2, 208);
		}
		
		return true;
	}
}