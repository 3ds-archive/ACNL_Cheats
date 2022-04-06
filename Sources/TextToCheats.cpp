#include "cheats.hpp"

namespace CTRPluginFramework
{
	u32 get_len(u16 *s)
	{
		u32 len = 0;
		for( ; *s; s++, len++ );
		return len;
	}
	
	string make_str(u16 *p, u32 len)
	{
		string  s;
		Process::ReadString( (u32)p, s, len * 2, StringFormat::Utf16 );
		return s;
	}
	
	//---------------------------------------------------
	// アイテム検索
	// out   = 出力先
	//---------------------------------------------------
	void FindItems( std::vector<ItemData> &out )
	{
		u16 buf[50];					// チャット文字列(ひらがなにする為のコピー)
		u32 len  = Chat::GetLength();	// buf長さ
		
		u16 *name;		// アイテム名(ポインタ)
		u32 name_len;	// アイテム名文字数
		u16 tmp[50];	// アイテム名(ひらがな変換用)
		
		// チャットの文字列のカタカナをひらがなにする
		memcpy(buf, Chat::GetAddress(), len * 2); // bufにコピー
		mojiChange(buf, 1, len); // ひらがなにする
		
		out.clear();
		for( u16 item = 0x2000; item <= 0x372A; item++ )
		{
			name     = (u16*)ITEM_NAME_ADDR(item);
			name_len = get_len(name);
			
			memcpy(tmp, name, name_len * 2);
			mojiChange(tmp, 1, name_len);
			
			if( find_mem( tmp, tmp + name_len, buf, len * 2 ) != nullptr )
			{
				out.push_back( make_tuple(make_str(name, name_len), item) );
			}
		}
	}
	
	//---------------------------------------------------
	// コマンド
	//---------------------------------------------------
	void TextToCheats(MenuEntry* entry)
	{
		// チャットを開いていなかったら帰る
		if( Chat::IsOpened() == false )
			return;
		
		if( Controller::IsKeysPressed(B + R) && !Chat::Empty() )
		{
			std::string com = Chat::Read();
			
			std::vector<ItemData> items;
			FindItems(items);
			
			if( !items.empty() )
			{
				int r = ChatSelector<u16>(items);
				if (r >= 0)
				{
					u16 item = std::get<1>(items[r]);
					*Player::GetInvSlotAddr(0) = item;
					UseItem = item;
					Chat::Write(Utils::Format("set: %04X", item));
				}
			}
		}
		
		
	}
	
	
	
}







