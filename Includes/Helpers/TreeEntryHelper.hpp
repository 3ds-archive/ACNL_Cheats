#pragma once

#include <memory>

namespace CTRPluginFramework
{
	using namespace std;
	using FuncPointer = void(*)(MenuEntry *);
	
	class TreeItemImpl;
	class TreeItem
	{
	public:
		// entry
		TreeItem(MenuEntry *entry);
		TreeItem(const string &name, FuncPointer GameFunc, const string &note = "");
		TreeItem(const string &name, FuncPointer GameFunc, FuncPointer MenuFunc, const string &note = "");
		
		// folder
		TreeItem(MenuFolder *folder);
		TreeItem(const string &name, const string &note, const vector<TreeItem*> &entries);
		TreeItem(const string &name, const vector<TreeItem*> &entries);
		
		~TreeItem();
		
		MenuEntry   *ToEntry();
		MenuFolder  *ToFolder();
		
	private:
		unique_ptr<TreeItemImpl> _item;
		friend class TreeItemImpl;
	};
}