#include "types.h"
#include "Helpers/String.hpp"

namespace CTRPluginFramework
{
	int lenUtf8(std::string &str);
	
	String::String()
	{
		if( _data.empty() == false ) _data.clear();
	}
	
	String::String(u32 address, u32 length)
	{
		u16 *p = (u16*)address;
		_data.clear();
		for( u32 i = 0; i < length; i++, p++ )
			_data.push_back(*p);
	}
	
	String::String(const std::vector<u16> &data)
		:_data(data) {}
	
	String:: ~String()
	{
		_data.clear();
	}
	
	u16 & String::operator [] (u32 pos)
	{
		return _data[pos];
	}
	
	bool String::Equal(const String &str)
	{
		if( _data.size() != str._data.size() )
			return false;
		
		for( u32 i = 0; i < str._data.size(); i++ )
			if( _data[i] != str._data[i] ) return false;
		
		return true;
	}
	
	String String::SubString(u32 pos)
	{
		std::vector<u16> data(_data.begin() + pos, _data.end());
		return String(data);
	}
	
	String String::SubString(u32 pos, u32 length)
	{
		std::vector<u16> data(_data.begin() + pos, _data.begin() + pos + length);
		return String(data);
	}
	
   void String::Insert(u32 pos, u16 ch)
   {
      _data.insert( _data.begin() + pos, ch );
   }
   
	void String::PushBack(const String &str)
	{
		for( const u16 &i : str._data )
			_data.push_back(i);
	}
}

