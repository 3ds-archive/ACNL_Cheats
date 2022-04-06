#pragma once

namespace CTRPluginFramework
{
	u32 DecryptACNLMoney(u64 money);
	u64 EncryptACNLMoney(int dec);
	
	void Notify( const std::string &str, const Color &fore = Color::White, const Color &back = Color::Black, bool sysFont = true );
	void ToggleEntry(MenuEntry *entry, void(*on)(), void (*off)());
	bool GetItem(u8 wx, u8 wy, u32 &item);
	Color GetItemColor(u16 item);
	
	std::string	  Hex(u8 x);
	std::string	  Hex(u16 x);
	std::string	  Hex(u32 x);
	std::string	  Hex(u64 x);
	std::string	  Hex(float x);
	
	// 引数 = アイテムID、出力先
	// 戻り = 文字数 ( 無効なアイテムの場合は -1 )
	int GetItemName(u16 itemID, u16 *out);
	int GetItemName(u16 itemID, std::string &out);
	
	//---------------------------------------------------
	// メモリからデータを探す
	// start          = 開始アドレス
	// end            = 終了アドレス
	// data           = 探すデータ
	// data_length    = 探すデータのバイト数
	//
	// 戻り			  = 一致したアドレス
	//---------------------------------------------------
	void * find_mem(void *start, void *end, void *data, size_t data_length);
	
	//---------------------------------------------------
	// 文字列操作(UTF-16)
	//	mode
	//	 0 = ひらがな -> カタカナ
	//	 1 = カタカナ -> ひらがな
	//	 2 = 英語大文字 -> 小文字
	//	 3 = 英語小文字 -> 大文字
	//---------------------------------------------------
	void mojiChange(u16 *str, int mode, int length);
	
	//---------------------------------------------------
	// メモリの内容をテキストに保存する
	// start      = 開始アドレス
	// end        = 終了アドレス
	// fileName   = ファイル名
	// addr       = アドレスを出力するかどうか
	// cut        = 分割するバイト数（範囲が大きいとテキストが読み込めないので、設定する必要がある。デフォルトでは1MB）
	//---------------------------------------------------
	void RAMtoTextDumper(u32 start, u32 end, std::string fileName, bool addr, const u32 cut = 0x100000);
	
	//---------------------------------------------------
	// チャットで選ぶ
	// items = 選ぶ配列( 要素 = tuple<string, T> )
	// 戻り  = 選んだ位置（チャットが閉じられると -1）
	// 選択中はチャットが打てなくなります。
	//---------------------------------------------------
	template <typename T>
	int ChatSelector( const std::vector<std::tuple<std::string, T>> &items )
	{
		if( !Chat::IsOpened() || items.empty() ) return -1;
		
		u32 tick = 1;
		for( int selector = 0; Chat::IsOpened(); )
		{
			Controller::Update();
			Chat::Write( std::get<0>(items[selector]) );
			u32 keys = Controller::GetKeysDown();
			
			if( (keys & B) && ((keys & R) || (keys & L)) )
			{
				if( tick == 0 || tick == 20 )
				{
					if( keys & R ) selector++;
					if( keys & L ) selector--;
					if( selector < 0 ) selector = items.size() - 1;
					if( selector >= items.size() ) selector = 0;
					if( tick == 20 ) tick -= 4;
				}
				tick++;
			}
			else
			{
				tick = 0;
			}
			
			if( Controller::IsKeysPressed(X + DR) )
				return selector;
			
			Sleep(Milliseconds(16));
		}
		
		return -1;
	}
}