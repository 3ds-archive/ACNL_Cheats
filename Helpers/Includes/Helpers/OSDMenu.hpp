#pragma once

namespace CTRPluginFramework
{
	using namespace std;
	
	class OSDMenu
	{
	public:
		static void Set(const vector<string> &entries);
		static void Append(const string &entry);
		static void Append(const vector<string> &entries);
		static void Clear();
		static int  Open(const string &title = "");
	};
}