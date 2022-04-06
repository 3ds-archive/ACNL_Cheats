#include <string>
#include <vector>
#include "CTRPluginFramework.hpp"
#include "Helpers/TreeEntryHelper.hpp"

namespace CTRPluginFramework
{
	class TreeItemImpl
	{
		bool                   is_folder;
		string                 name, note;
		FuncPointer            game, menu;
		MenuEntry              *entry;
		MenuFolder             *folder;
		vector<TreeItemImpl*>  entries;
		
		void clear_entries()
		{
			for( TreeItemImpl *i : entries )
				if( i != nullptr ) delete i;
			entries.clear();
		}
		
		void make_entries(const vector<TreeItem*> items)
		{
			clear_entries();
			
			for( TreeItem *i : items )
			{
				entries.push_back(i->_item.get());
			}
		}
		
		MenuEntry *to_entry()
		{
			if( is_folder )
				return nullptr;
			
			if( entry != nullptr )
				return entry;
			
			return new MenuEntry(name, game, menu, note);
		}
		
		MenuFolder *to_folder()
		{
			if( !is_folder )
				return nullptr;
			
			if( folder != nullptr )
				return folder;
			
			MenuFolder *f = new MenuFolder(name, note);
			
			for( TreeItemImpl *i : entries )
			{
				if( i->is_folder )
					f->Append(i->to_folder());
				else
					f->Append(i->to_entry());
			}
			
			return f;
		}
		
		TreeItemImpl(const string &name, const string &note, FuncPointer game, FuncPointer menu, bool is_folder)
			:name(name), note(note), game(game), menu(menu), is_folder(is_folder), entries({})
		{
			
		}
		
		TreeItemImpl(MenuEntry *entry, MenuFolder *folder, bool is_folder)
			:entry(entry), folder(folder), is_folder(is_folder)
		{
			
		}
		
	public:
		~TreeItemImpl()
		{
			clear_entries();
		}
		
		friend class TreeItem;
	};
	
	// entry
	TreeItem::TreeItem(MenuEntry *entry)
		:_item(new TreeItemImpl(entry, nullptr, false))
	{
		
	}
	
	TreeItem::TreeItem(const string &name, FuncPointer GameFunc, const string &note)
		:_item(new TreeItemImpl(name, note, GameFunc, nullptr, false))
	{
		
	}
	
	TreeItem::TreeItem(const string &name, FuncPointer GameFunc, FuncPointer MenuFunc, const string &note)
		:_item(new TreeItemImpl(name, note, GameFunc, MenuFunc, false))
	{
		
	}
	
	// folder
	TreeItem::TreeItem(MenuFolder *folder)
		:_item(new TreeItemImpl(nullptr, folder, true))
	{
		
	}
	
	TreeItem::TreeItem(const string &name, const string &note, const vector<TreeItem*>& entries)
	{
		TreeItemImpl* impl = new TreeItemImpl(name, note, nullptr, nullptr, true);
		impl->make_entries(entries);
		_item.reset(impl);
	}
	
	TreeItem::TreeItem(const string &name, const vector<TreeItem*>& entries)
	{
		TreeItemImpl* impl = new TreeItemImpl(name, "", nullptr, nullptr, true);
		impl->make_entries(entries);
		_item.reset(impl);
	}
	
	TreeItem:: ~TreeItem()
	{
		_item.reset();
	}
	
	MenuEntry * TreeItem::ToEntry()
	{
		return _item->to_entry();
	}
	
	MenuFolder * TreeItem::ToFolder()
	{
		return _item->to_folder();
	}
}




