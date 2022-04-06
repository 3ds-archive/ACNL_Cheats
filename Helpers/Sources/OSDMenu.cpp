#include <vector>
#include <string>

#include "3ds.h"
#include "CTRPluginFramework.hpp"
#include "Helpers/OSDMenu.hpp"

namespace CTRPluginFramework
{
	using namespace std;

	vector<string> _OSDMenu_entries;
	int            _OSDMenu_selector;
	bool           _OSDMenu_isOpened;

	void OSDMenu::Set(const vector<string> &entries)
	{
		_OSDMenu_entries = entries;
	}

	void OSDMenu::Append(const string &entry)
	{
		_OSDMenu_entries.push_back(entry);
	}

	void OSDMenu::Append(const vector<string> &entries)
	{
		for( const string &str : entries )
			_OSDMenu_entries.push_back(str);
	}

	void OSDMenu::Clear()
	{
		_OSDMenu_entries.clear();
	}

	void OSDMenu_Update()
	{
		static Clock clock;
		int &selector = _OSDMenu_selector;
		u32 key = Controller::GetKeysDown();

		if( !clock.HasTimePassed(Milliseconds(100)) )
			return;

		clock.Restart();

		if( key & DU )
			selector = max((int)0, selector - 1);
		else if( key & DD )
			selector = min((int)_OSDMenu_entries.size() - 1, selector + 1);

		if( key & A )
			_OSDMenu_isOpened = false;
	}

	void OSDMenu_Draw(const Screen &scr, const string &title)
	{
		vector<string> &entries = _OSDMenu_entries;
		int &selector = _OSDMenu_selector;

		scr.DrawRect(20, 20, 280, 200, Color::Black);
		scr.DrawSysfont(title, 22, 22);
		scr.DrawRect(22, 38, 250, 1, Color::White);

		u32 dy = 42;
		u32 start = max(0, selector - 6);
		u32 end = min((int)entries.size(), (int)start + 11);

		for( ; start < end; start++, dy += 16 )
			scr.DrawSysfont(entries[start], 22, dy, start == selector ? Color::SkyBlue : Color::White);
	}

	int OSDMenu::Open(const string &title)
	{
		if( _OSDMenu_entries.empty() ) return -1;

		Process::Pause();

		const Screen &scr = OSD::GetBottomScreen();
		_OSDMenu_isOpened = true;

		_OSDMenu_selector = 0;
		while( _OSDMenu_isOpened )
		{
			Controller::Update();
			if( Controller::IsKeyPressed(B) )
			{
				_OSDMenu_selector = -1;
				break;
			}
			OSDMenu_Update();
			OSDMenu_Draw(scr, title);
			OSD::SwapBuffers();
		}

		Process::Play();

		return _OSDMenu_selector;
	}
}