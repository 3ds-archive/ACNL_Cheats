#include <string>
#include <vector>
#include "CTRPluginFramework.hpp"
#include "Helpers.hpp"

using namespace std;

namespace CTRPluginFramework
{
	// とび森通貨を複合化する
	u32 DecryptACNLMoney(u64 money)
	{
		// Unpack 64-bit value into (u32, u16, u8, u8) values.
		u32 enc = (money & 0xFFFFFFFF);
		u16 adjust = ((money >> 32) & 0xFFFF);
		u8  shift_val = ((money >> 48) & 0xFF);
		u8  chk = ((money >> 56) & 0xFF);

		// Validate 8-bit checksum
		if ((((enc >> 0) + (enc >> 8) + (enc >> 16) + (enc >> 24) + 0xBA) & 0xFF) != chk) return 0;
		
		u8  left_shift = ((0x1C - shift_val) & 0xFF);
		u8  right_shift = 0x20 - left_shift;

		// Handle error case: Invalid shift value.
		if (left_shift >= 0x20)
		{
			return 0 + (enc << right_shift) - (adjust + 0x8F187432);
		}

		// This case should occur for all game-generated values.
		return (enc << left_shift) + (enc >> right_shift) - (adjust + 0x8F187432);
	}
	
	// 数値をとび森通貨の16進数値に暗号化する
	u64 EncryptACNLMoney(int dec)
	{
		// Make a new RNG
		u16 adjust = Utils::Random(0, 0x10000);
		u8  shift_val = Utils::Random(0, 0x1A);

		// Encipher value
		u32 enc = dec + adjust + 0x8F187432;
		enc = (enc >> (0x1C - shift_val)) + (enc << (shift_val + 4));

		// Calculate Checksum
		u8  chk = (((enc >> 0) + (enc >> 8) + (enc >> 16) + (enc >> 24) + 0xBA) & 0xFF);

		// Pack result
		return ((u64)enc << 0) | ((u64)adjust << 32) | ((u64)shift_val << 48) | ((u64)chk << 56);
	}
	
	void ToggleEntry(MenuEntry *entry, void(*on)(), void (*off)())
	{
		if( entry->WasJustActivated() )
		{
			if( on != nullptr ) on();
		}
		else if( !entry->IsActivated() )
		{
			if( off != nullptr ) off();
		}
	}
	
	// 引数 = アイテムID、出力先(u16の配列。十分なスペースが確保されている必要がある)
	// 戻り = 文字数 ( 無効なアイテムの場合は -1 )
	int GetItemName(u16 itemID, u16 * const out)
	{
		if( itemID < 0x2000 || itemID > 0x372A )
			return -1;
		
		u16 *addr = (u16*)( 0x31724DC0 + *(u32*)( 0x31724DC0 + (itemID - 0x2000) * 4 + 4 ) );
		int len = 0;
		
		for( ; addr[len] != 0; len++ )
			out[len] = addr[len];
		
		return len;
	}
	
	int GetItemName(u16 itemID, std::string &out)
	{
		if( itemID < 0x2000 || itemID > 0x372A )
			return -1;
		
		u16  name[50];
		int  len = GetItemName(itemID, name);
		
		out.clear();
		Process::ReadString( (u32)name, out, len * 2, StringFormat::Utf16 );
		
		return len;
	}
	
	u16 _st16(string s)
	{
		u16 r = 0;
		u32 v = 0x10;
		for( int i = 3; i >= 0; i-- )
		{
			if( s[i] >= '0' && s[i] <= '9' )
				r += v * (s[i] - '0');
			else if( s[i] >= 'A' && s[i] <= 'F' )
				r += v * (s[i] - 'A') + 0xA;
			else if( s[i] >= 'a' && s[i] <= 'f' )
				r += v * (s[i] - 'a') + 0xA;
			
			v *= 0x10;
		}
		return r;
	}
	
	//---------------------------------------------------
	// メモリからデータを探す
	// start          = 開始アドレス
	// end            = 終了アドレス
	// data           = 探すデータ
	// data_length    = 探すデータのバイト数
	//
	// 戻り			  = 一致したアドレス
	//---------------------------------------------------
	void * find_mem(void *start, void *end, void *data, size_t data_length)
	{
		for( u32 s = (u32)start; s < (u32)end - data_length; s++ )
		{
			if( memcmp((void*)s, data, data_length) == 0 )
				return (void*)s;
		}
		
		return nullptr;
	}
	
	//---------------------------------------------------
	// 文字列操作(UTF-16)
	//	mode
	//	 0 = ひらがな -> カタカナ
	//	 1 = カタカナ -> ひらがな
	//	 2 = 英語大文字 -> 小文字
	//	 3 = 英語小文字 -> 大文字
	//---------------------------------------------------
	void mojiChange(u16 *str, int mode, int length)
	{
		for( int i = 0; i < length; i++ )
		{
			u16 &c = str[i];
			
			switch( mode )
			{
			case 0:
				if( c >= 0x3042 && c <= 0x3093 ) c += 0x60;
				break;
			case 1:
				if( c >= 0x30A2 && c <= 0x30F3 ) c -= 0x60;
				break;
			case 2:
				if( c >= 0x41 && c <= 0x5A ) c += 0x20;
				break;
			case 3:
				if( c >= 0x61 && c <= 0x7A ) c -= 0x20;
				break;
			}
		}
	}
	
	
	void RAMtoTextDumper(u32 start, u32 end, string fileName, bool addr, const u32 cut)// cut = 1MB
	{
		const u32 MIN_ADDR = 0x100000;
		const u32 MAX_ADDR = 0x331B7FFC;
		
		start -= start % 4;
		end -= end % 4;
		
		if( start >= end )
		{
			(MessageBox("'start'が無効です。\nこのアドレスは'end'より小さくする必要があります。"))();
			return;
		}
		if( start < MIN_ADDR )
		{
			(MessageBox( Utils::Format("'start'が小さすぎます。\n0x%08X以上のアドレスにしてください。", MIN_ADDR) ))();
			return;
		}
		if( end > MAX_ADDR )
		{
			(MessageBox( Utils::Format("'end'が大きすぎます。\n0x%08X以下のアドレスにしてください。", MAX_ADDR) ))();
			return;
		}
		
		bool pause = false;
		if( !Process::IsPaused() )
		{
			Process::Pause();
			pause = true;
		}
		
		const Screen &scr = OSD::GetTopScreen();
		
		u32   value;
		u32   part = 0;
		File  file;
		for( ; start < end; part++ )
		{
			scr.Draw("-RAM To Text Dumper-", 10, 0);
			scr.Draw(Utils::Format("%08X ~ %08X", start, start + cut / 18 * 18), 10, 10);
			scr.Draw(Utils::Format("%s_part%d.txt", fileName.c_str(), part), 10, 20);
			scr.Draw("Hold <B> to cancel.", 10, 30);
			OSD::SwapBuffers();
			
			File::Open(file, fileName + Utils::Format("_part%d.txt", part), File::RWC);
			LineWriter writer(file);
			for( u32 size = 0; start < end && size <= cut; size += (addr ? 18 : 9), start += 4 )
			{
				if( Process::CheckAddress(start) )
				{
					if( addr )
						writer << Utils::Format( "%08X %08X\n", start, *(u32*)start );
					else
						writer << Utils::Format( "%08X\n", *(u32*)start );
				}
				
				Controller::Update();
				if( Controller::IsKeyDown(B) )
				{
					if( (MessageBox("終了しますか?", DialogType::DialogYesNo, ClearScreen::Top))() )
					{
						file.Flush();
						file.Close();
						goto end_;
					}
				}
			}
			
			file.Flush();
			file.Close();
		}
		
	end_:;
		if( pause )
		{
			Process::Play();
		}
	}
	
	
	// Notify
	struct ntfy_data
	{
		bool	sys;
		string	str;
		u32		foreC;
		u32		backC;
		u32		cnt;
	};
	vector<ntfy_data> _notifys = {{1, "Plugin ready!", (u32)-1, Color::Black.ToU32(), 0}};
	void Notify( const string &str, const Color &fore, const Color &back, bool sysFont )
	{
		if( _notifys.size() >= 8 ) _notifys.pop_back();
		_notifys.insert(_notifys.begin(), {sysFont, str, fore.ToU32(), back.ToU32(), 0});
	}
	bool Notify_OSDcallback(const Screen &scr)
	{
		if( scr.IsTop && !_notifys.empty() )
		{
			u32 dx, dy = 220, width;
			u32 i = 0;
			for( ntfy_data &n : _notifys )
			{
				width = OSD::GetTextWidth(n.sys, n.str);
				dx = 398 - width;
				if( n.sys )
				{
					scr.DrawRect(dx, dy, width, 16, Color::Black);
					scr.DrawSysfont(n.str, dx, dy);
					dy -= 18;
				}
				else
				{
					scr.Draw(n.str, dx, dy);
					dy -= 12;
				}
				
				n.cnt++;
				if( n.cnt >= 300 ) _notifys.erase(_notifys.begin() + i);
				
				i++;
			}
			return 1;
		}
		
		return 0;
	}
	
	bool GetItem(u8 wx, u8 wy, u32 &item)
	{
		u32 *pItem = Game::GetItemAtWorldCoords(wx, wy);
		if( pItem == nullptr ) return false;
		item = *pItem;
		return true;
	}
	
	Color GetItemColor(u16 item)
	{
		if( item >= 0 && item <= 0xFF )
			return Color::Green;
		else if( item != 0x7FFE )
			return Color(0xFC, 0x9D, 0xB8);
		
		return Color::Gray;
	}
	
	std::string	  Hex(u8 x)
	{
		char  buffer[3];

		sprintf(buffer, "%02X", x);
		return (std::string(buffer));
	}

	std::string	  Hex(u16 x)
	{
		char  buffer[5];

		sprintf(buffer, "%04X", x);
		return (std::string(buffer));
	}

	std::string	  Hex(u32 x)
	{
		char  buffer[9];

		sprintf(buffer, "%08X", x);
		return (std::string(buffer));
	}

	std::string	  Hex(u64 x)
	{
		char  buffer[17];

		sprintf(buffer, "%016llX", x);
		return (std::string(buffer));
	}

	std::string	  Hex(float x)
	{
		char  buffer[9];

		sprintf(buffer, "%08X", (u32)x);
		return (std::string(buffer));
	}

	std::string	  Hex(double x)
	{
		char  buffer[17];

		sprintf(buffer, "%016llX", (u64)x);
		return (std::string(buffer));
	}
}