#include "cheats.hpp"
using namespace CTRPluginFramework;

// ダンプ形式
enum class DumpType
{
	Text,		// テキスト
	Binary		// バイナリ
};

// オプション
namespace CF_Options
{
	// 基本
	u32			addr			= 0x207AD0;				// アドレス
	u32			pCount			= 4;					// パラメータ個数
	u32			params[11]		= {0x20C, 0x33099E50, 0x96FC06, 0xADF870, 0, 0, 0, 0, 0, 0, 0};// パラメータ
	u32			returnedValue	= 0;					// 戻り値
	bool		draw_retValue	= false;				// 戻り値の描画
	
	// メモリダンプ
	bool		mem_dump		= false;				// コール時のメモリダンプ
	DumpType	dump_type		= DumpType::Binary;		// ダンプ形式
	string		dump_fileName	= "mem";				// ファイル名
	u32			dump_start		= 0x900000;				// 開始アドレス
	u32			dump_end		= 0xA00000;				// 終了アドレス
	u32			dump_cut		= 0x7D000;				// 分割サイズ(テキスト形式のみ)  デフォルトは500KB
	
	CallFuncWrapper  funct;
};

struct CF_GameFunc
{
	string	name;
	string	note;
	u32		addr;
	u32		pCount;
};

const vector<CF_GameFunc>
	GameFuncTemplates =
{
	{"Drop Item",
		"u8 DropType\n"
		"u32 *ItemToReplace\n"
		"u32 *ItemToPlace\n"
		"u32 *ItemToShow\n"
		"u32 wx\n"
		"u32 wy\n"
		"unk1\n"
		"unk2\n"
		"unk3\n"
		"unk4\n"
		"unk5"
		, 0x59E5B4, 11
	},
	
	{"Drop Check",
		"u32 *wx_out\n"
		"u32 *wy_out\n"
		"unk1\n"
		"bool disallowConcrete\n"
		"unk2"
		, 0x5979E4, 5
	},
	
	{"Particles",
		"u16 ParticleID\n"
		"float *coords\n"
		"unk1\n"
		"unk2"
		, 0x207AD0, 4
	},
	
	{"Warp",
		"u8 RoomID\n"
		"unk1\n"
		"unk2\n"
		"unk3"
		, 0x304A94, 4
	},
	
	{"PlaceBuildingUpdateCollisions",
		"u32 wx\n"
		"u32 wy\n"
		"u16 BuildingID"
		, 0x2424F4, 3
	},
	
	{"Get Current Map",
		"[params nothing]"
		, 0x6A53DC, 0
	},
	
	{"Get Item on Map",
		"u32 Map\n"
		"u32 wx\n"
		"u32 wy\n"
		"?"
		, 0x2FEE38, 4
	},
	
	{"Get Player Coords",
		"u32 *wx\n"
		"u32 *wy\n"
		"u8 PlayerIndex\n"
		"bool noChecks"
		, 0x5BFCE4, 4
	},
	
	{"Player Instance",
		"u8 playerIndex\n"
		"bool noChecks"
		, 0x5C27D8, 2
	},
	
	{"Player Animation Insntace",
		"u32 playerInstance\n"
		"u8 someVal1\n"
		"u8 someVal2\n"
		"u32 encVal"
		, 0x6561F0, 4
	},
	
	{"Execute Animation",
		"u32 pInstance\n"
		"u8 AnimID\n"
		"u32 pAnimationInstance\n"
		"unk1"
		, 0x64C688, 4
	},
};

// 関数コール
void CallFunction(MenuEntry *entry)
{
	using namespace CF_Options;
	
	if( Process::IsPaused() || Controller::IsKeyPressed(L) || Controller::IsKeyDown(ZL) )
	{
		funct.SetAddress(addr);
		
		bool paused = false;
		if( mem_dump && !Process::IsPaused )
		{
			paused = true;
			Process::Pause();
		}
		
		returnedValue = funct.WithParamArray(pCount, params);
		
		if( mem_dump )
		{
			if( dump_type == DumpType::Text )
			{
				RAMtoTextDumper(dump_start, dump_end, dump_fileName, true, dump_cut);
			}
			else
			{
				File file(dump_fileName, File::RWC);
				file.Dump(dump_start, dump_end - dump_start);
				file.Flush();
				file.Close();
			}
		}
		
		if( paused )
			Process::Play();
	}
}

// テンプレート選択
void ChooseTemplates(MenuEntry *entry)
{
	vector<string>	names;
	Keyboard		kbd1("", {"適用", "説明を見る"});
	
	for( auto &i : GameFuncTemplates )
	{
		names.push_back(i.name);
	}
	
	for(;;)
	{
		int i = Keyboard("テンプレート", names).Open();
		
		if( i >= 0 )
		{
			auto g = GameFuncTemplates[i];
			switch( kbd1.Open() )
			{
			case 0:
				CF_Options::pCount = g.pCount;
				CF_Options::addr = g.addr;
				break;
			case 1:
				(MessageBox(g.name, g.note))();
				break;
			}
		}
		else
		{
			break;
		}
	}
}

void InputFunc(u32 &param)
{
	OSDMenu::Set(
	{
		"Player Wx",
		"Player Wy",
		"Player Coordinates",
		"Player Instatnce",
		"Player Anim Instance",
		"Current Map",
		"Online Player Index",
		"Actual Player Index",
		"Online Player Count"
	});
	
	u32 wx, wy;
	
	switch( OSDMenu::Open() )
	{
	case 0:// wx
	{
		Player::GetWorldCoords(&wx, &wy, 4, 1);
		param = wx;
		break;
	}
	case 1:// wy
	{
		Player::GetWorldCoords(&wx, &wy, 4, 1);
		param = wy;
		break;
	}
	case 2:// coords
	{
		param = (u32)Player::GetCoordinates();
		break;
	}
	case 3:// instance
	{
		param = Player::GetInstance(4, 1);
		break;
	}
	case 4:// anim instance
	{
		param = Player::GetAnimationInstance(Player::GetInstance(4, 1), 0, 0, 0);
		break;
	}
	case 5:// current map
	{
		param = Game::GetCurrentMap();
		break;
	}
	case 6:// online p index
	{
		param = Game::GetOnlinePlayerIndex();
		break;
	}
	case 7:// actual p index
	{
		param = Game::GetActualPlayerIndex();
		break;
	}
	case 8:// online p count
	{
		param = Game::GetOnlinePlayerCount();
		break;
	}
	}
}

// オプション
void ChangeOption(MenuEntry *entry)
{
	using namespace CF_Options;
	
	vector<string>	opt;
	Keyboard		kbd;
	kbd.IsHexadecimal(false);
	
	for(;;)
	{
		opt.clear();
		opt.push_back("アドレス: " << Hex(addr));
		opt.push_back("パラメータ個数: " << to_string(pCount));
		
		for( int i = 0; i < pCount; i++ )
			opt.push_back(Utils::Format("%d: %08X", i + 1, params[i]));
		
		int ch = Keyboard("オプション", opt).Open();
		if( ch == -1 ) break;
		
		switch( ch )
		{
		case 0:
			kbd.Open(addr, addr);
			break;
		case 1:
			kbd.Open(pCount, pCount);
			if( pCount > 11 ) pCount = 11;
			break;
		default:
			ch -= 2;
			Keyboard("0xFFFFFFFF を指定すると、ゲーム内から数値を引用できます。").Open(params[ch], params[ch]);
			if( params[ch] == 0xFFFFFFFF )
			{
				OSD::SwapBuffers();
				InputFunc(params[ch]);
			}
			break;
		}
	}
}

// ダンプ形式変更
void ChangeDumpType(MenuEntry *entry)
{
	switch( Keyboard("", {"テキスト", "バイナリ"}).Open() )
	{
		case 0: CF_Options::dump_type = DumpType::Text; break;
		case 1: CF_Options::dump_type = DumpType::Binary; break;
	}
}

// 分割サイズ変更
void ChangeDumpCutSize(MenuEntry *entry)
{
	if( CF_Options::dump_type == DumpType::Binary )
	{
		(MessageBox("バイナリ形式の場合、このオプションは無効です"))();
		return;
	}
	
	vector<string> opt = {"byte", "KB", "MB"};
	Keyboard  kbd("サイズ単位", opt);
	u32       value;
	
	int size = kbd.Open();
	if( size == -1 ) return;
	
	OSD::SwapBuffers();
	
	Keyboard input(opt[size]);
	input.IsHexadecimal(false);
	
	if( input.Open(value) == 0 )
	{
		if( value == 0 )
		{
			OSD::SwapBuffers();
			(MessageBox("無効な値です"))();
			return;
		}
		
		if( size == 1 )
			CF_Options::dump_cut = value * 0x400;
		else if( size == 2 )
			CF_Options::dump_cut = value * 0x100000;
	}
}

// ファイル名
void ChangeDumpFileName(MenuEntry *entry)
{
	Keyboard().Open(CF_Options::dump_fileName, CF_Options::dump_fileName);
}

// 戻り値表示 OSD
bool draw_ret_value(const Screen &scr)
{
	if( scr.IsTop )
	{
		scr.Draw(Utils::Format("%08X", CF_Options::returnedValue), 330, 2);
		return true;
	}
	
	return false;
}

MenuEntry * separator(MenuEntry *e, bool IsTop = true)
{
	IsTop ? e->UseTopSeparator() : e->UseBottomSeparator();
	return e;
}

// フォルダ作成
MenuFolder * CTRPluginFramework::InitCallFuncFolder(Color c)
{
	MenuFolder *f = new MenuFolder(c << "関数コール");
	{
		*f += new MenuEntry(c << "関数コール " FONT_L " / " FONT_ZL, CallFunction, CallFunction);
		*f += new MenuEntry(c << "テンプレート", nullptr, ChooseTemplates);
		*f += new MenuEntry(c << "戻り値表示",
			[](MenuEntry*e)// GameFunc
			{
				if( e->WasJustActivated() )
					OSD::Run(draw_ret_value);
				else if( !e->IsActivated() )
					OSD::Stop(draw_ret_value);
			});
		*f += new MenuEntry(c << "オプション", nullptr, ChangeOption);
		
	/*---------------*/
		*f += separator(new MenuEntry(c << "コール時にメモリダンプ",
			[](MenuEntry*e)// GameFunc
			{
				if( e->WasJustActivated() )
					CF_Options::mem_dump = true;
				else if( !e->IsActivated() )
					CF_Options::mem_dump = false;
			},
			[](MenuEntry*e)// MenuFunc
			{
				using namespace CF_Options;
				if( Keyboard("開始アドレス").Open(dump_start, dump_start) == 0 )
				{
					OSD::SwapBuffers();
					Keyboard("終了アドレス").Open(dump_end, dump_end);
				}
			},
			"オンになっている場合、コールする直前にプロセスを一時停止します。\n"
			"そして、コールした直後にメモリダンプが開始されます。(終わるとプロセスは再開されます)"
		));
		
		*f += new MenuEntry(c << "ダンプ形式", nullptr, ChangeDumpType);
		*f += new MenuEntry(c << "分割サイズ", nullptr, ChangeDumpCutSize);
		*f += new MenuEntry(c << "ファイル名", nullptr, ChangeDumpFileName);
		
	}
	
	return f;
}