#include "CTRPluginFramework.hpp"
#include "Helpers/Chat.hpp"

namespace CTRPluginFramework
{
	int lenByte(unsigned char c)
	{
		if ((c >= 0x00) && (c <= 0x7f))
			return 1;
		else if ((c >= 0xc2) && (c <= 0xdf))
			return 2;
		else if ((c >= 0xe0) && (c <= 0xef))
			return 3;
		
		return 0;
	}
	
	int lenUtf8(std::string &str)
	{
		auto _pstr = str.c_str();

		int i = 0;
		int len = 0;

		while (_pstr[i] != '\0')
		{
			len++;
			i += lenByte(_pstr[i]);
		}

		return len;
	}
   
	// チャットを開いているか(false = 開いてない, true = 開いている)
	bool Chat::IsOpened()
	{
		if (*(u32*)0x958114 == 0) return false;
		return true;
	}

	// 書き込み
	void Chat::Write(std::string str)
	{
		if (!IsOpened()) return;
		
		Clear();
		*(u8*)(( *(u32*)0x958114 ) + 0x8) = lenUtf8(str);
		Process::WriteString( *(u32*)(0x958108), str, StringFormat::Utf16);
	}

	// 読み取り
	std::string Chat::Read()
	{
		if (!IsOpened()) return std::string("");

		std::string str;
		Process::ReadString( *(u32*)0x958108, str, 33, StringFormat::Utf16);

		return str;
	}

	// 送信
	void Chat::Send()
	{
		if (IsOpened()) Process::Write8(0x9580E1, 1);
	}

	// 全部消去する
	void Chat::Clear()
	{
		if (!IsOpened()) return;

		u32 *addr = (u32*)( *(u32*)0x958108 );
		memset(addr, 0, 0x20);
	}
	
	bool Chat::Empty()
	{
		return GetLength() == 0;
	}
	
	u16 * Chat::GetAddress()
	{
		return (u16*)( *(u32*)0x958108 );
	}
	
	u8 Chat::GetLength()
	{
		return *(u8*)(( *(u32*)0x958114 ) + 0x8);
	}
}