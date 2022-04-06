#pragma once

namespace CTRPluginFramework
{
	class Chat
	{
	public:
		static bool IsOpened();
		static void Write(std::string str);
		static std::string Read();
		static void Send();
		static void Clear();
		static bool Empty();
		
		static u16 *GetAddress();
		static u8 GetLength();
	};
}