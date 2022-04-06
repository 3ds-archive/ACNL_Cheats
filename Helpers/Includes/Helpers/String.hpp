#pragma once

#include <string>
#include <vector>

namespace CTRPluginFramework
{
	// UTF16 string
	class String
	{
	public:
		String();
		String(u32 address, u32 length);
		String(const std::vector<u16> &data);
		~String();
		
		// 取得
		u16 & operator [] (u32 pos);
		
		// 一致確認
		bool Equal(const String &str);
		
		// 切り取り
		String SubString(u32 pos);
		String SubString(u32 pos, u32 length);
		
		// 追加
      void Insert(u32 pos, u16 ch);
		void PushBack(const String &str);
		
	private:
		std::vector<u16> _data;
	};
}