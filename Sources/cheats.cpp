#include "cheats.hpp"
#include "OSDManager.hpp"

namespace CTRPluginFramework
{
	extern u8 waitAnim;
	extern u8 DropType;
	extern bool MapEditor_enabled;
	
	bool DrawUseItem(const Screen &scr)
	{
		if( global_entries.size() == 0 ) return 0;
		if( scr.IsTop && (*(u8*)0x33099e53 != 0 || *(u8*)0x33099f7f != 0) && *(u16*)0x948450 == 0 && global_entries[0]->IsActivated() )
        {
            scr.Draw(Utils::Format("Drop:%08X", UseItem), 0, 230, Color(40, 250, 40));
            return 1;
        }
		
        return 0;
	}
	
	// アイテム名変更
	struct sItemMAP { u32 id; string name; };
	vector<sItemMAP>  InvItemName_data;
	
	// セーブデータ保存
	void SaveDataBackups(MenuEntry* entry)
	{
		Directory dir("dumps", true);
		Keyboard k1("", { "バックアップ", "リストア" });
		int r1 = k1.Open();

		if (r1 == 0)
		{
			Keyboard k2("ファイル名を入力");
			string fileName;
			OSD::SwapBuffers();
			if (k2.Open(fileName) == 0)
			{
				File  file;
				if (dir.OpenFile(file, fileName, File::RWC) == 0)
				{
					file.Dump(0x31f49a00, 0x89B00);
					file.Flush();
					file.Close();
				}
			}
		}
		else if (r1 == 1)
		{
			vector<string> files;
			dir.ListFiles(files);
			if (!files.empty())
			{
				Keyboard k2("", files);
				OSD::SwapBuffers();
				int r2 = k2.Open();
				if (r2 >= 0)
				{
					File file;
					if (dir.OpenFile(file, files[r2]) == 0)
					{
						file.Inject(0x31f49a00, 0x89A80);
						OSD::SwapBuffers();
						(MessageBox("リストアしました。"))();
					}
					else
					{
						OSD::SwapBuffers();
						(MessageBox("リストアできませんでした。"))();
					}
				}
			}
			else
			{
				OSD::SwapBuffers();
				(MessageBox("ファイルがありません。"))();
			}
		}
	}

	// 村名変更
	void ChangeTownName(MenuEntry* entry)
	{
		if (!Chat::IsOpened())
		{
			(MessageBox("チャットが開かれていません。"))();
			return;
		}

		string name = Chat::Read();

		if ((MessageBox("村名を '" << name << "' に設定します。\nよろしいですか?", DialogType::DialogYesNo))() == true)
		{
			u32 addr = 0x31F49A00, size = 0x89B00;
			for (u16* p = (u16*)addr; p < (u16*)(addr + size); p++)
			{
				if (*p == 0x88FC)
				{
					p++;
					memset(p, 0, 16);
					Process::WriteString((u32)p, name, StringFormat::Utf16);
				}
			}
		}
	}

	// プレイヤー名変更
	void ChangePlayerName(MenuEntry* entry)
	{
		if (!Chat::IsOpened())
		{
			(MessageBox("チャットが開かれていません。"))();
			return;
		}

		string name;
		int len = Chat::GetLength();
		if (len > 8) len = 8;
		Process::ReadString((u32)Chat::GetAddress(), name, len * 2, StringFormat::Utf16);

		vector<string> names;
		const u32 addr = 0x31f4f048;

		string str;
		for (int i = 0; i < 4; i++)
		{
			if (*(u16*)(addr + i * 0xa480) != 0)
			{
				Process::ReadString(addr + i * 0xa480, str, 16, StringFormat::Utf16);
				names.push_back(str);
			}
		}

		for (;;)
		{
			Keyboard  k1("", names);
			int r1 = k1.Open();

			if (r1 >= 0)
			{
				OSD::SwapBuffers();
				if ((MessageBox(Utils::Format("プレイヤー%dの名前を '%s' に設定します。\nよろしいですか?", r1 + 1, name.c_str()), DialogType::DialogYesNo))())
				{
					memset((void*)(addr + r1 * 0xa480), 0, 16);
					Process::WriteString(addr + r1 * 0xa480, name, StringFormat::Utf16);
				}
				OSD::SwapBuffers();
			}
			else
				break;
		}
	}

	// 国籍変更
	struct kuni_s { const char* name; u32 data; };
	const std::vector< kuni_s > kuni =
	{
	   { "日本",						0x190c0001 },
	   { "アルバニア",					 0x40 },
	   { "アメリカ",					 0x31 },
	   { "アンギラ",					 0x8	},
	   { "アンティグア・バーブーダ",		 0x9	},
	   { "アルゼンチン",				  0xa	},
	   { "アルバ",						0xb	},
	   { "ウルグアイ",					 0x32 },
	   { "エクアドル",					 0x19 },
	   { "エルサルバドル",				  0x1a },
	   { "英領ヴァージン諸島",			   0x11 },
	   { "オランダ領アンティル",			0x26 },
	   { "カナダ",						0x12 },
	   { "ガイアナ",					 0x1f },
	   { "グレナダ",					 0x1c },
	   { "グアドループ",				  0x1d },
	   { "グアテマラ",					 0x1e },
	   { "ケイマン諸島",				  0x13 },
	   { "コスタリカ",					 0x16 },
	   { "コロンビア",					 0x15 },
	   { "ジャマイカ",					 0x22 },
	   { "スリナム",					 0x2e },
	   { "セントキッツ・ネイビス",			0x2b },
	   { "セントルシア",				  0x2c },
	   { "セントビンセント・グレナディーン",   0x2d },
	   { "タークス・カイコス諸島",			0x30 },
	   { "チリ",						0x14 },
	   { "トリニダード・トバゴ",			0x2f },
	   { "ドミニカ国",					 0x17 },
	   { "ドミニカ共和国",				  0x18 },
	   { "ニカラグア",					 0x27 },
	   { "ハイチ",						0x20 },
	   { "パナマ",						0x28 },
	   { "パラグアイ",					 0x29 },
	   { "バハマ",						0xc	},
	   { "バルバドス",					 0xd	},
	   { "フランス領ギアナ",			   0x1b },
	   { "ブラジル",					 0x10 },
	   { "ベリーズ",					 0xe	},
	   { "ペルー",						0x2a },
	   { "ベネズエラ",					 0x34 },
	   { "ホンジュラス",				  0x21 },
	   { "ボリビア",					 0xf	},
	   { "マルティニーク",				  0x23 },
	   { "メキシコ",					 0x24 },
	   { "モントセラト",				  0x25 },
	   { "米領バージン諸島",			   0x33 },
	};
	void ChangeKokuseki(MenuEntry* entry)
	{
		vector<string> vstr;
		for (const kuni_s& i : kuni)
			vstr.push_back(i.name);

		Keyboard key("国籍変更", vstr);
		int r = key.Open();

		if (r >= 0) Process::Write32(0x31f4f072, kuni[r].data);
	}

	// 性別変更
	void ChangeSeibetu(MenuEntry* entry)
	{
		Keyboard key("プレイヤー", { "村長", "サブ1", "サブ2", "サブ3" });
		int r0 = key.Open();

		OSD::SwapBuffers();

		if (r0 >= 0)
		{
			Keyboard denger("性別", { "男", "女" });
			int r1 = denger.Open();

			if (r1 >= 0)
				Process::Write8(0x31F4F05A + r0 * 0xA480, r1);
		}
	}

	// 遊び用
	void DropMod(MenuEntry* entry)
	{
		static u32 pIndex = 0;
		
		if (Game::GetActualPlayerIndex() == 0)
			*(u32*)0x30B844 = 0x13A00001;
		else
			*(u32*)0x30B844 = 0x13A00000;
		
		if (Controller::IsKeysPressed(B + DU))
		{
			Process::Write32(0x19CF5C, 0xE1A00000);
			Process::Write8(0x65EF28, 0x06); //set anim after bury

			int r = (Keyboard("", { "通常", "埋める", "花", "マイデザイン", "値を入力" })).Open();
			if( r == 4 )
			{
				//OSD::SwapBuffers();
				Sleep(Milliseconds(10));
				if( (Keyboard("値を入力")).Open(DropType, DropType) == 0 )
					Process::Write8(0x596054, DropType);
			}
			else if( r >= 0 )
			{
				DropType = 0xA + r;
				Process::Write8(0x596054, DropType);
			}
		}
		
		if( *(u8*)0x75D7B3 == 0xE1 )
			*(u8*)0x75d7c0 = pIndex;
		
		if( Controller::IsKeysPressed(B + DR) )
		{
			if (*(u8*)0x75d7b3 == 0xE3)
			{
				*(u32*)0x75d7b0 = 0xE1500000;
				Notify(Color::Green << "乗っ取り: オン");
			}
			else
			{
				*(u32*)0x75d7b0 = 0xE3520000;
				*(u8*)0x75d7c0 = 0;
				Notify(Color::Red << "乗っ取り: オフ");
			}
		}
		
		if( Controller::IsKeysPressed(B + DD) )
		{
			pIndex++;
			if (pIndex > 3) pIndex = 0;
			Notify(Utils::Format("プレイヤー: %d", pIndex));
		}
	}
	
	u32	  itemRnd_start = 0x9F;
	u32	  itemRnd_end   = 0xC7;
	bool  itemRnd_enabled;
	bool  from_inv = false;
	void ChangeUseItem(MenuEntry* entry)
	{
		if (!from_inv)
		{
			if (Controller::IsKeysPressed(Y + DR) && !itemRnd_enabled)
				(Keyboard("アイテム変更")).Open(UseItem, UseItem);
		}
		else
		{
			UseItem = *(u32*)(*(u32*)0xAA914C + 0x6BD0);
		}
	}
	void ChangeUseItem_opt(MenuEntry* entry)
	{
		Keyboard k("持ち物欄1番目のアイテムを使う", { "オン", "オフ" });
		int r = k.Open();
		if (r >= 0) from_inv = (r == 0);
	}

	void ItemRandomizer(MenuEntry* entry)
	{
		if (Controller::IsKeysPressed(R + X))
		{
			Keyboard k;

			if (k.Open(itemRnd_start, itemRnd_start) == 0)
			{
				Sleep(Milliseconds(100));
				k.Open(itemRnd_end, itemRnd_end);
			}
		}

		if (Controller::IsKeysPressed(L + DL))
		{
			itemRnd_enabled = !itemRnd_enabled;
		}

		if (itemRnd_enabled)
			UseItem = Utils::Random(itemRnd_start, itemRnd_end);
	}
	
	bool AutoDrop_enabled = false;
	void AutoDrop(MenuEntry* entry)
	{
		if (Controller::IsKeysPressed(B + DL))
		{
			if (AutoDrop_enabled)
			{
				AutoDrop_enabled = false;
				Notify(Color::Red << "自動ドロップ: オフ");
			}
			else
			{
				AutoDrop_enabled = true;
				Notify(Color::Green << "自動ドロップ: オン");
			}
		}

		u32 wx, wy;
		if( AutoDrop_enabled && Game::DropCheck(wx, wy) )
		{
			Game::DropItem(UseItem, wx, wy);
		}
	}
	
	void TouchDrop(MenuEntry* entry)
	{
		if (Touch::IsDown() && *(u8*)0x949C30 == 1)
		{
			UIntVector pos = Touch::GetPosition();
			u32 wx, wy;

			switch (*(u8*)0x95133A)
			{
			case 0:// 村
				if (pos.x >= 68 && pos.y >= 50 && pos.x <= 245 && pos.y <= 192)
				{
					wx = ((pos.x - 68) / 2.2125) + 0x10;
					wy = ((pos.y - 50) / 2.2125) + 0x10;
				}
				else return;
				break;
			case 0x68:// 島
				if (pos.x >= 116 && pos.y >= 70 && pos.x <= 201 && pos.y <= 157)
				{
					wx = ((pos.x - 116) / 2.65) + 0x10;
					wy = ((pos.y - 70) / 2.65) + 0x10;
				}
				else return;
				break;
			default:
				return;
			}

			Game::DropItem(UseItem, wx, wy);
		}
	}

	void TouchRemove(MenuEntry* entry)
	{
		if (Touch::IsDown() && *(u8*)0x949C30 == 1)
		{
			UIntVector pos = Touch::GetPosition();
			u32 wx, wy;

			switch (*(u8*)0x95133A)
			{
			case 0:// 村
				if (pos.x >= 68 && pos.y >= 50 && pos.x <= 245 && pos.y <= 192)
				{
					wx = ((pos.x - 68) / 2.2125) + 0x10;
					wy = ((pos.y - 50) / 2.2125) + 0x10;
				}
				else return;
				break;
			case 0x68:// 島
				if (pos.x >= 116 && pos.y >= 70 && pos.x <= 201 && pos.y <= 157)
				{
					wx = ((pos.x - 116) / 2.65) + 0x10;
					wy = ((pos.y - 70) / 2.65) + 0x10;
				}
				else return;
				break;
			default:
				return;
			}

			Game::TrampleAt(wx, wy);
		}
	}

	void InventoryItemNames(MenuEntry* entry)
	{
		for (;;)
		{
			vector<string>  opt{ "[新規作成]", "[すべて削除]" };
			for (const sItemMAP& i : InvItemName_data)
				opt.push_back(Utils::Format("%X", i.id) << ": " << i.name);
			int r = (Keyboard("", opt)).Open();

			if (r == -1)
				break;
			else if (r == 0)
			{
				OSD::SwapBuffers();
				u16 id;
				if ((Keyboard("アイテムID")).Open(id) == 0)
				{
					bool redefined = false;
					for (const sItemMAP& i : InvItemName_data)
						if (id == i.id) { redefined = true; break; }

					if (!redefined)
					{
						OSD::SwapBuffers();
						string name = "***";
						if ((Keyboard("アイテム名\n(***を指定するとチャットの文字列にします)")).Open(name, name) == 0)
						{
							if (name == "***" && Chat::IsOpened()) name = Chat::Read();
							InvItemName_data.push_back({ id, name });
						}
					}
					else
					{
						OSD::SwapBuffers();
						(MessageBox("既に同じIDが存在します。"))();
					}
				}
			}
			else if (r == 1)
			{
				OSD::SwapBuffers();
				if ((MessageBox("本当にすべて削除しますか？", DialogType::DialogYesNo))() == true)
					InvItemName_data.clear();
			}
			else
			{
				sItemMAP& item = InvItemName_data[r - 2];
				int r2 = (Keyboard("", { "IDを変更", "名前を変更", "削除" })).Open();
				switch (r2)
				{
				case 0:
				{
					OSD::SwapBuffers();
					Keyboard().Open(item.id, item.id);
					break;
				}
				case 1:
				{
					OSD::SwapBuffers();
					Keyboard().Open(item.name, item.name);
					break;
				}
				case 2:
				{
					InvItemName_data.erase(InvItemName_data.begin() + (r - 2));
					break;
				}
				}
			}
		}
	}

	void TrampleAllItem(MenuEntry* entry)
	{
		if (Controller::IsKeysDown(R + Y))
			for (u8 wx = 0; wx < 0xA0; wx++)
			{
				for (u8 wy = 0; wy < 0xA0; wy++)
					if (Game::GetItemAtWorldCoords(wx, wy) != nullptr) Game::TrampleAt(wx, wy);
				Sleep(Milliseconds(1));
			}
	}
	
	// インベントリ
	void ChangeItemOption(MenuEntry* entry)
	{
		const u32 addr[]{ 0x19B9D8, 0x19BFC8, 0x19C150, 0x19BA38 };
		const u32 orig[]{ 0x1A000012, 0xA00001A, 0xA000009, 0x1A000012 };
		vector<string> names
		{
		   "見せびらかす",
		   "壁掛け & 床に飾る",
		   "食べる",
		   "手紙を書く"
		};

		for(;;)
		{
			vector<string> opt;
			for (int i = 0; i < 4; i++)
				opt.push_back(string(*(u8*)addr[i] == 0 ? "on " : "off ") + names[i]);
			int r = (Keyboard("", opt)).Open();

			if (r == -1)
				break;
			else if (*(u8*)addr[r] == 0)
				*(u32*)addr[r] = orig[r];
			else
				*(u32*)addr[r] = NOP;

			OSD::SwapBuffers();
		}
	}

	struct InventoryData
	{
		string   name;
		u32	  items[16];
	};
	vector<InventoryData> InvBackups;
	void InventoryBackup(MenuEntry* entry)
	{
		u32* pInv = (u32*)(*(u32*)0xAA914C + 0x6BD0);

		for (;;)
		{
			vector<string> opt{ "[新規作成]", "[ファイルから]", "[すべて削除]" };
			for (const InventoryData& i : InvBackups)
				opt.push_back(i.name);
			int r1 = (Keyboard("", opt)).Open();

			if (r1 == -1)
			{
				break;
			}
			else if (r1 == 0)
			{
				string name;
				if ((Keyboard("名前を入力")).Open(name) != 0) break;
				InventoryData data;
				data.name = name;
				memcpy(data.items, pInv, 0x40);
				InvBackups.push_back(data);
			}
			else if (r1 == 1)
			{
				Directory dir("inventory", true);
				vector<string> files;
				dir.ListFiles(files);
				if (files.empty())
				{
					(MessageBox("ファイルがありません。"))();
				}
				else
				{
					int r = (Keyboard("", files)).Open();
					if (r >= 0)
					{
						File file(files[r]);
						InventoryData data;
						data.name = files[r];
						file.Inject((u32)data.items, 0x40);
						InvBackups.push_back(data);
					}
				}
			}
			else if (r1 == 2)
			{
				if ((MessageBox("本当にすべて消しますか？", DialogType::DialogYesNo))() == true)
					InvBackups.clear();
			}
			else
			{
				r1 -= 3;
				InventoryData& data = InvBackups[r1];
				int r2 = (Keyboard(data.name, { "名前を変える", "アイテム編集", "リストア", "ファイルに保存", "削除" })).Open();
				switch (r2)
				{
				case 0: // 名前を変える
				{
					(Keyboard()).Open(data.name, data.name);
					break;
				}
				case 1: // アイテム編集
				{
					for (;;)
					{
						vector<string> items;
						for (u32 i : data.items)
						{
							string s;
							GetItemName((u16)i, s);
							string name = (i != 0x7FFE) ? s : "[アイテム無し]";
							items.push_back(name.empty() ? "???" : name);
						}
						int r3 = (Keyboard("", items)).Open();
						if (r3 == -1)
							break;
						else
							(Keyboard()).Open(data.items[r3], data.items[r3]);
					}
					break;
				}
				case 2: // リストア
				{
					if ((MessageBox("このデータをインベントリにリストアしますか？"))() == true)
						memcpy(pInv, data.items, 0x40);
					break;
				}
				case 3: // ファイルに保存
				{
					Directory dir("inventory", true);
					File file;
					if (dir.OpenFile(file, data.name, File::RWC) == 0)
					{
						file.Dump((u32)data.items, 0x40);
						Sleep(Milliseconds(100));
						file.Flush();
						(MessageBox("保存しました。"))();
					}
					break;
				}
				case 4: // 削除
				{
					if ((MessageBox("本当に削除しますか？"))() == true)
						InvBackups.erase(InvBackups.begin() + r1);
					break;
				}
				}
			}
		}
	}

	void AllDelete(MenuEntry* entry)
	{
		bool b = (MessageBox("確認", "持ち物欄のアイテムを全て削除しますか?", DialogType::DialogYesNo))();
		if (b == true)
		{
			u32* p = (u32*)(*(u32*)0xAA914C + 0x6BD0);
			for (int i = 0; i < 16; i++)
				p[i] = 0x7FFE;
		}
	}

	void ViewTokushuItem(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x7238c0, 0xe1a00000);
			Process::Write32(0xad0250, 0x01000000);
		},
		[]()
		{
			Process::Write32(0x7238c0, 0x0a000001);
			Process::Write32(0xad0250, 0x00000000);
		});

		u32* invPtr = (u32*)0x9499EC;
		if (*invPtr == 0) return;

		u8 slot = *(u8*)(*invPtr + 0xCC);
		u32 name = *invPtr + 0xCFC;

		if (*(u32*)(*invPtr - 4) == 0xA338 && slot != 0xFF)
		{
			u32 item = *Player::GetInvSlotAddr(slot);
			bool find = false; string f_name;
			for (const sItemMAP& i : InvItemName_data)
				if (i.id == item) { find = true; f_name = i.name; break; }
			if (find)
			{
				memset((u32*)name, 0, 0x20);
				Process::WriteString(name, f_name, StringFormat::Utf16);
			}
			else if (item >= 5 && item <= 0xFD)
			{
				//const ITEM_MAP &i = ACNL_ALL_ITEMS[item - 5];
				//memset( (u32*)name, 0, 0x20 );
				//Process::WriteString(name, i.name, StringFormat::Utf16);
			}
		}
	}

	void InfinityItemDrop(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x19C4D0, NOP);
			Process::Write32(0x19C42C, NOP);
		},
		[]()
		{
			Process::Write32(0x19C4D0, 0xEB057D0A);
			Process::Write32(0x19C42C, 0xEB057D33);
		});
	}

	void DragDropSpam(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x19C4D0, NOP);
			Process::Write32(0x19C42C, NOP);
			Process::Write32(0x19C548, NOP);
			Process::Write32(0x19DDE4, NOP);
			Process::Write32(0x26F000, NOP);
		},
		[]()
		{
			Process::Write32(0x19C548, 0xEB03FB85);
			Process::Write32(0x19DDE4, 0xEB03F55E);
			Process::Write32(0x26F000, 0xEB00B0D7);
		});
	}
	
	void FillInv(MenuEntry *entry)
	{
		static u32 item = 0x2117;
		
		if( Controller::IsKeyPressed(ZL) )
		{
			Keyboard("埋めつくすアイテム").Open(item, item);
			for( int i = 0; i < 16; i++ )
				*Player::GetInvSlotAddr(i) = item;
		}
	}
	
	void TextToItem(MenuEntry* entry)
	{
		u32 item;
		
		if( Controller::IsKeysPressed(X + DR) && Keyboard().Open(item) == 0 )
		{
			u32* p = (u32*)(*(u32*)0xAA914C + 0x6BD0);
			for (int i = 0; i < 16; i++, p++)
			{
				if (*p == 0x7FFE)
				{
					*p = item;
					break;
				}
			}
		}
	}

	void GetAshimotoItem(MenuEntry* entry)
	{
		if (*(u8*)0x33099e53 == 0 && *(u8*)0x33099f7f == 0)
			return;

		u32 item, wx, wy;
		Player::GetWorldCoords(&wx, &wy, 4, 1);
		u32 *pItem = Game::GetItemAtWorldCoords(wx, wy);

		if (Controller::IsKeysPressed(X + DD) && pItem != nullptr)
		{
			u32* p = (u32*)(*(u32*)0xAA914C + 0x6BD0);
			for (int i = 0; i < 16; i++)
			{
				if (p[i] == 0x7FFE)
				{
					p[i] = item;
					Notify(Utils::Format("スロット%d = %08X", i, item));
					break;
				}
			}
		}
	}

	// アニメーション
	void Idle(MenuEntry* entry)
	{
		u32 wx, wy;
		if (Controller::IsKeysPressed(R + DD) && Player::GetWorldCoords(&wx, &wy, 4, 1))
		{
			Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex(), 6, 0, 1, 1, 1, 0, wx, wy, true);
		}
	}

	void ExecAnim(MenuEntry* entry)
	{
		static u8  animID = 6;
		static u32 animItem = 0;
		static u32 wx, wy;

		if (Controller::IsKeysDown(L + A) && Player::GetWorldCoords(&wx, &wy, 4, 1))
			Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex(), animID, animItem, 1, 1, 1, 0, wx, wy, true);

		if (Controller::IsKeysPressed(L + DR))
			Keyboard("アイテム").Open(animItem, animItem);

		if (Controller::IsKeysPressed(L + DD))
			Keyboard("アニメID").Open(animID, animID);
	}

	// 動き、アクション
	void NoBreakFlower(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x596890, 0xE3A0001D);
		},
		[]()
		{
			Process::Write32(0x596890, 0xEBF5990F);
		});
	}

	int use_bt = 0;
	float c_speed = 8;
	void CoordinatesModifier(MenuEntry* entry)
	{
		if ((*(u8*)0x33099e53 == 0 && *(u8*)0x33099f7f == 0) || MapEditor_enabled)
			return;
		
		volatile float* coord = (float*)(0x33099e50 + get12c());
		
		switch (use_bt)
		{
		case 0: // A + 十字キー
			if (Controller::IsKeyDown(A))
			{
				if (Controller::IsKeyDown(DU)) coord[2] -= c_speed;
				if (Controller::IsKeyDown(DD)) coord[2] += c_speed;
				if (Controller::IsKeyDown(DL)) coord[0] -= c_speed;
				if (Controller::IsKeyDown(DR)) coord[0] += c_speed;
			}
			break;
		case 1: // A + スライドパッド
			circlePosition cpos;
			hidCircleRead(&cpos);

			if (Controller::IsKeyDown(A))
			{
				coord[0] += abs(cpos.dx) > 20 ? ((float)cpos.dx / 160.0f) * c_speed : 0;
				coord[2] -= abs(cpos.dy) > 20 ? ((float)cpos.dy / 160.0f) * c_speed : 0;
			}

			break;
		case 2: // Cスティック
			if (Controller::IsKeyDown(CStickUp)) coord[2] -= c_speed;
			if (Controller::IsKeyDown(CStickDown)) coord[2] += c_speed;
			if (Controller::IsKeyDown(CStickLeft)) coord[0] -= c_speed;
			if (Controller::IsKeyDown(CStickRight)) coord[0] += c_speed;
			break;
		}
	}
	void Coord_opt(MenuEntry* entry)
	{
		Keyboard key("", { "ボタン変更", "スピード変更" });

		for (;;)
		{
			OSD::SwapBuffers();
			int ret = key.Open();

			if (ret == -1)
			{
				break;
			}
			else if (ret == 0)
			{
				OSD::SwapBuffers();
				Keyboard k("", { FONT_A " + " FONT_DPAD, FONT_A " + " FONT_CP, "C-Stick" });
				int r = k.Open();
				if (r >= 0) use_bt = r;
			}
			else if (ret == 1)
			{
				OSD::SwapBuffers();
				Keyboard k;
				k.Open(c_speed, c_speed);
			}
		}
	}

	void WalkOverObjects(MenuEntry* entry)
	{
		if (Controller::IsKeysPressed(L + DPadUp))
		{
			if (*(u8*)0x64eef7 == 0xA)
			{
				Process::Write8(0x64eef7, 0xea);
				Process::Write8(0x64ef0f, 0xea);
				Process::Write8(0x64f073, 0xea);
				Process::Write8(0x64f0eb, 0xea);
				Process::Write8(0x64f1bb, 0xea);
				Process::Write8(0x64f1e7, 0xea);
				Process::Write32(0x64f19c, NOP);
				Process::Write32(0x64f1b4, NOP);
				Notify(Color::Green << "壁抜け: オン");
			}
			else
			{
				Process::Write8(0x64eef7, 0xa);
				Process::Write8(0x64ef0f, 0xa);
				Process::Write8(0x64f073, 0xa);
				Process::Write8(0x64f0eb, 0xda);
				Process::Write8(0x64f1bb, 0xa);
				Process::Write8(0x64f1e7, 0xa);
				Process::Write32(0x64f19c, 0xed841a05);
				Process::Write32(0x64f1b4, 0xed840a07);
				Notify(Color::Red << "壁抜け: オフ");
			}
		}
	}

	const vector<RoomData> rooms
	{
	   { "村",							 0x00 },
	   { "商店街",						 0x01 },
	   { "駅",							 0x02 },
	   { "P1 家 メイン",				   0x03 },
	   { "P1 家 二階",					 0x04 },
	   { "P1 家 地下",					 0x05 },
	   { "P1 家 右",					   0x06 },
	   { "P1 家 左",					   0x07 },
	   { "P1 家 奥",					   0x08 },
	   { "P2 家 メイン",				   0x09 },
	   { "P2 家 二階",					 0x0A },
	   { "P2 家 地下",					 0x0B },
	   { "P2 家 右",					   0x0C },
	   { "P2 家 左",					   0x0D },
	   { "P2 家 奥",					   0x0E },
	   { "P3 家 メイン",				   0x0F },
	   { "P3 家 二階",					 0x10 },
	   { "P3 家 地下",					 0x11 },
	   { "P3 家 右",					   0x12 },
	   { "P3 家 左",					   0x13 },
	   { "P3 家 奥",					   0x14 },
	   { "P4 家 メイン",				   0x15 },
	   { "P4 家 二階",					 0x16 },
	   { "P4 家 地下",					 0x17 },
	   { "P4 家 右",					   0x18 },
	   { "P4 家 左",					   0x19 },
	   { "P4 家 奥",					   0x1A },
	   { "住民1 家",					   0x1B },
	   { "住民2 家",					   0x1C },
	   { "住民3 家",					   0x1D },
	   { "住民4 家",					   0x1E },
	   { "住民5 家",					   0x1F },
	   { "住民6 家",					   0x20 },
	   { "住民7 家",					   0x21 },
	   { "住民8 家",					   0x22 },
	   { "住民9 家",					   0x23 },
	   { "住民10 家",					  0x24 },
	   { "役場",						   0x26 },
	   { "R パーカーズ",				   0x2B },
	   { "モダンな交番",				   0x30 },
	   { "クラシックな交番",			   0x31 },
	   { "カフェ",						 0x32 },
	   { "カフェ バイトイベント",		  0x33 },
	   { "夢見の館",					   0x37 },
	   { "デパート1階",					0x3C },
	   { "デパート2階",					0x3D },
	   { "デパート3階",					0x3E },
	   { "エイブルシスターズ 服屋",		0x3F },
	   { "エイブルシスターズ アクセ屋",	0x40 },
	   { "Shoe Shank 靴屋",				0x40 },
	   { "不動産屋",					   0x41 },
	   { "Club444",						0x47 },
	   { "ビューティーサロン スピーディ",  0x48 },
	   { "博物館 中央",					0x49 },
	   { "博物館 魚",					  0x4A },
	   { "博物館 虫",					  0x4D },
	   { "博物館 化石",					0x50 },
	   { "博物館 絵画",					0x53 },
	   { "博物館 2階",					 0x55 },
	   { "博物館 2階 左上",				0x56 },
	   { "博物館 2階 左下",				0x57 },
	   { "博物館 2階 右上",				0x58 },
	   { "博物館 2階 右下",				0x59 },
	   { "郵便局",						 0x5A },
	   { "夢見の館",					   0x5B },
	   { "証明写真機",					 0x5C },
	   { "Dream Bed Error",				0x64 },
	   { "島へ行くイベント",			   0x65 },
	   { "村へ戻るイベント",			   0x66 },
	   { "島 室内",						0x67 },
	   { "島 屋外",						0x68 },
	   { "ハッピーホームアカデミー 中央",  0x8E },
	   { "オートキャンプ場",			   0xA0 },
	   { "オートキャンプ場 車の中",		0xA1 },
	   { "しずえがいるエリア",			 0x63 },
	   { "タイトル",					   0x5E },
	   { "カメラ",						 0xA4 },
	   { "住民登録",					   0x5D },
	   { "オレンジテント",				 0x25 },
	   { "模様替えイベント",			   0xA3 },
	   { "釣り結果",					   0x2F },
	};
	void RoomWarping(MenuEntry* entry)
	{
		if( Controller::IsKeysPressed(L + DR) )
		{
			vector<string> opt;
			for( const RoomData &i : rooms )
				opt.push_back(i.name);
			
			int r = (Keyboard("", opt)).Open();
			opt.clear();
			
			if( r >= 0 )
				Game::Warp(rooms[r].id);
		}
	}

	void Stalker(MenuEntry* entry)
	{
		if (Controller::IsKeyDown(A))
		{
			float* coord = Player::GetCoordinates();
			float* coord2;

			if (Controller::IsKeyDown(DU) && (coord2 = Player::GetCoordinates(0)) != nullptr)
				memcpy(coord, coord2, 0xC);
			else if (Controller::IsKeyDown(DR) && (coord2 = Player::GetCoordinates(1)) != nullptr)
				memcpy(coord, coord2, 0xC);
			else if (Controller::IsKeyDown(DD) && (coord2 = Player::GetCoordinates(2)) != nullptr)
				memcpy(coord, coord2, 0xC);
			else if (Controller::IsKeyDown(DL) && (coord2 = Player::GetCoordinates(3)) != nullptr)
				memcpy(coord, coord2, 0xC);
		}
	}

	void DPadCoord(MenuEntry* entry)
	{
		volatile u16* rot = (u16*)(0x33099E6A + get12c());
		volatile float* coord = (float*)(0x33099e50 + get12c());
		const float spd = 4.f;

		u32 keys = Controller::GetKeysDown();
		if (keys & R)
		{
			if (keys & DU)
			{
				*rot = 0x8000;
				coord[2] -= spd;
			}
			if (keys & DD)
			{
				*rot = 0;
				coord[2] += spd;
			}
			if (keys & DL)
			{
				*rot = 0xC000;
				coord[0] -= spd;
			}
			if (keys & DR)
			{
				*rot = 0x4000;
				coord[0] += spd;
			}
		}
	}

	void BlockMove(MenuEntry* entry)
	{
		static u32 wx, wy, tick;

		if (Controller::IsKeyDown(DPad))
		{
			if (tick == 0 || tick == 14)
			{
				u32 keys = Controller::GetKeysDown();
				if (keys & DU) wy--;
				if (keys & DD) wy++;
				if (keys & DL) wx--;
				if (keys & DR) wx++;
				if (tick == 14) tick -= 2;
			}
			tick++;

			float* coords = (float*)(0x33099e50 + get12c());
			coords[0] = wx * 0x20 + 0x10;
			coords[2] = wy * 0x20 + 0x10;
		}
		else
		{
			tick = 0;
			Player::GetWorldCoords(&wx, &wy, 4, 1);
		}
	}

	void TouchWarping(MenuEntry* entry)
	{
		u8 room = *(u8*)0x95133A;
		float* coords = (float*)(0x33099E50 + get12c());

		if (Touch::IsDown() && *(u8*)0x949C30 == 1)
		{
			u16 tx = (u16)Touch::GetPosition().x;
			u16 ty = (u16)Touch::GetPosition().y;

			// 村
			if ((room == 0 || room == 0x69) && tx >= 70 && ty >= 32 && tx <= 249 && ty <= 214)
			{
				coords[0] = (tx - 69) * 14.2f + 526.f;
				coords[2] = (ty - 54) * 14.2f + 526.f;
			}
			// 商店街
			else if (room == 1 && ty >= 43 && ty <= 201)
			{
				coords[0] = (tx - 11) * 6.26f + 150.f;
				coords[2] = (ty - 48) * 6.26f - 117.f;
			}
			// 島
			else if (room == 0x68 && tx >= 76 && ty >= 36 && tx <= 243 && ty <= 203)
			{
				coords[0] = (tx - 74) * 12.2f;
				coords[2] = (ty - 35) * 12.2f;
			}
		}
	}

	void AntiCheat(MenuEntry* entry)
	{
		if (Controller::IsKeyPressed(L))
		{
			if (*(u8*)0x67724c == 0)
			{
				*(u32*)0x67724c = 0xEBFF550D;
				Notify(Color::Red << "アンチチート: オフ");
			}
			else
			{
				*(u32*)0x67724c = NOP;
				Notify(Color::Green << "アンチチート: オン");
			}
		}
	}

	u32 wRemover_size = 0;
	vector<u32> wThroughItems;
	void WalkRemover(MenuEntry* entry)
	{
		if (*(u8*)0x33099e53 == 0 && *(u8*)0x33099f7f == 0)
			return;

		u32 wx, wy;
		Player::GetWorldCoords(&wx, &wy, 4, 1);

		for (int x = wx - wRemover_size; x <= wx + wRemover_size; x++)
			for (int y = wy - wRemover_size; y <= wy + wRemover_size; y++)
			{
				u32* pItem = Game::GetItemAtWorldCoords(x, y);
				if (pItem == nullptr) continue;

				for (u32 item : wThroughItems)
					if (*pItem == item) goto skip;

				Game::TrampleAt(x, y);
			skip:;
			}
	}

	void WalkRemover_opt(MenuEntry* entry)
	{
		Keyboard k1("", { "サイズ変更", "消さないアイテム" });

		for (bool loop = true; loop;)
		{
			switch (k1.Open())
			{
			case -1: loop = false; break;
			case 0:// サイズ変更
			{
				Keyboard k2;
				OSD::SwapBuffers();
				k2.Open(wRemover_size, wRemover_size);
				break;
			}
			case 1:// 消さないアイテム
			{
				for (;;)
				{
					OSD::SwapBuffers();
					vector<string> vstr{ "[追加]" };
					for (u32 i : wThroughItems)
						vstr.push_back(Utils::Format("%08X", i));
					Keyboard k3("", vstr);
					Keyboard k4("", { "削除", "変更" });
					Keyboard k5;
					int r3 = k3.Open();
					if (r3 == 0)// [追加]
					{
						OSD::SwapBuffers();
						u32 item;
						if (k5.Open(item) == 0)
							wThroughItems.push_back(item);
					}
					else if (r3 >= 1)// アイテム
					{
						OSD::SwapBuffers();
						int r4 = k4.Open();
						if (r4 == 0)// 削除
							wThroughItems.erase(wThroughItems.begin() + r3 - 1);
						else if (r4 == 1)// 変更
						{
							u32& item = wThroughItems[r3 - 1];
							OSD::SwapBuffers();
							k5.Open(item, item);
						}
					}
					else break;
				}
				break;
			}
			}

			OSD::SwapBuffers();
		}
	}
	
	void FuckingWater(MenuEntry *entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x64D31C, 0xE3A00000);
		},
		[]()
		{
			Process::Write32(0x64D31C, NOP);
		});
	}
	
	
	// チャット
	void ChatRepeater(MenuEntry* entry)
	{
		static string str;

		if (Controller::IsKeyDown(X) && Chat::IsOpened())
		{
			if (Controller::IsKeyDown(DU))
			{
				Chat::Write(str);
				Chat::Send();
			}
			else if (Controller::IsKeyPressed(DD))
			{
				str = Chat::Read();
			}
		}
	}

	void ChatEnter(MenuEntry* entry)
	{
		if (Chat::IsOpened()) *(u8*)0xAD0253 = 1;
	}

	void ChatUnlockNumber(MenuEntry* entry)
	{
		if (Chat::IsOpened()) *(u8*)0xAD0158 = 1;
	}


	// その他
	void HideSaveMenu(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x1A08C8, 0xE1A00000);
			Process::Write32(0x1A08CC, 0xE3A00000);
			Process::Write32(0x1A08D0, 0xEB0E011D);
		},
		[]()
		{
			Process::Write32(0x1A08C8, 0xE3A01040);
			Process::Write32(0x1A08CC, 0xE5900000);
			Process::Write32(0x1A08D0, 0xEB14CAC6);
		});
	}

	void NoHitOtherPlayer(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write8(0x650D83, 0xEA);
		},
		[]()
		{
			Process::Write8(0x650D83, 0x2A);
		});
	}

	void DisableScreenShot(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write8(0x5B2AE7, 0xA);
		},
		[]()
		{
			Process::Write8(0x5B2AE7, 0xBA);
		});
	}

	void FastGameSpeed(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x54C6E8, 0xE3E004FF);
		},
		[]()
		{
			Process::Write32(0x54C6E8, 0xE59400A0);
		});
	}

	void FastMessage(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x5FAF68, 0xE3A00001);
			Process::Write32(0x5FB298, 0x60000000);
		},
		[]()
		{
			Process::Write32(0x5FAF68, 0xEBFFF4B2);
		});
	}
	
	void TimeMachine(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write8(0x6563A8, 0x1A);
			Process::Write8(0x670A48, 6);
			Process::Write8(0x68A074, 6);
			Process::Write8(0x6D2498, 0x2E); // bottom UI ?
		},
		[]()
		{
			Process::Write8(0x6563A8, 0x17);
			Process::Write8(0x670A48, 0x42);
			Process::Write8(0x6D2498, 0x4A);
		});
		
		u32 wx, wy;
		if (Controller::IsKeysPressed(L + R + DR) && Player::GetWorldCoords(&wx, &wy, 4, 1))
		{
			Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex() & 3, 0x3F, 0, 1, 1, 1, 0, wx, wy, true);
		}
	}
	
	void PrivateStorage(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write8(0x6563A8, 0x1A);
			Process::Write8(0x670A48, 6);
			Process::Write8(0x68A074, 6);
			Process::Write8(0x6D2498, 0x89); // bottom UI ?
		},
		[]()
		{
			Process::Write8(0x6563A8, 0x17);
			Process::Write8(0x670A48, 0x42);
			Process::Write8(0x6D2498, 0x4A);
		});
		
		u32 wx, wy;
		if (Controller::IsKeysPressed(L + R + DR) && Player::GetWorldCoords(&wx, &wy, 4, 1))
		{
			Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex() & 3, 0x3F, 0, 1, 1, 1, 0, wx, wy, true);
		}
	}
	
	void CameraControl(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Camera::AllowRotation(true);
		},
		[]()
		{
			Camera::AllowRotation(false);
		});
		
		if (*(u8*)0x33099e53 == 0 && *(u8*)0x33099f7f == 0)
			return;
		
		u32 keys = Controller::GetKeysDown();
		const u16 rot = 0x100;
		const float spd = 12.f;
		float* coords = Camera::GetCoords();
		
		if (keys & R)
		{
			if (keys & CU) *Camera::RotationX() += rot;
			if (keys & CD) *Camera::RotationX() -= rot;
			if (keys & CL) *Camera::RotationY() -= rot;
			if (keys & CR) *Camera::RotationY() += rot;
		}
		
		if (Controller::IsKeysPressed(Y + B))
			Camera::IsLocked() ? Camera::Unlock() : Camera::Lock();
		
		if (Camera::IsLocked() && (keys & B))
		{
			if (keys & DU) coords[2] -= spd;
			if (keys & DD) coords[2] += spd;
			if (keys & DL) coords[0] -= spd;
			if (keys & DR) coords[0] += spd;
			if (keys & R) coords[1] -= spd;
			if (keys & L) coords[1] += spd;
		}
	}

	bool ViewAshimotoItem_osd(const Screen& scr)
	{
		if (*(u8*)0x33099e53 == 0 && *(u8*)0x33099f7f == 0)
			return 0;

		u32 wx, wy;
		Player::GetWorldCoords(&wx, &wy, 4, 1);
		u32* pItem = Game::GetItemAtWorldCoords(wx, wy);

		if (scr.IsTop && pItem != nullptr)
		{
			scr.Draw(Utils::Format("%08X", *pItem), 0, 180);
			string name;
			GetItemName(*pItem, name);
			if (!name.empty())
			{
				scr.DrawRect(0, 190, OSD::GetTextWidth(true, name), 16, Color::Black);
				scr.DrawSysfont(name, 0, 190);
				return 1;
			}
		}

		return 0;
	}
	void ViewAshimotoItem(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			OSD::Run(ViewAshimotoItem_osd);
		},
		[]()
		{
			OSD::Stop(ViewAshimotoItem_osd);
		});
	}
	
	void CheaterIsland(MenuEntry* entry)
	{
		ToggleEntry(entry,
		[]()
		{
			Process::Write32(0x34f8f8, 0xe3a00002);
		},
		[]()
		{
			Process::Write32(0x34f8f8, 0xe1a00c20);
		});
	}
	
	void ViewAddress(MenuEntry* entry)
	{
		static vector<u32> addrList;
		
		if (Controller::IsKeysPressed(R + X))
		{
			Process::Pause();

			for (;;)
			{
				vector<string> vstr{ "[追加]", "[すべて削除]" };
				for (u32 i : addrList)
					vstr.push_back(Utils::Format("%08X", i));
				Keyboard k1("", vstr);
				int r1 = k1.Open();

				if (r1 == 0)
				{
					OSD::SwapBuffers();
					Keyboard k2;
					u32 addr;
					if (k2.Open(addr) == 0)
						addrList.push_back(addr);
				}
				else if (r1 == 1)
				{
					addrList.clear();
				}
				else if (r1 >= 2)
				{
					r1 -= 2;
					Keyboard k3("", { "削除", "変更" });
					OSD::SwapBuffers();
					switch (k3.Open())
					{
					case 0:
						addrList.erase(addrList.begin() + r1);
						break;
					case 1:
						Keyboard kk;
						OSD::SwapBuffers();
						kk.Open(addrList[r1], addrList[r1]);
						break;
					}
				}
				else break;
				OSD::SwapBuffers();
			}

			Process::Play();
		}

		if (addrList.empty()) return;

		OSDManager.Lock();
		const Screen &scr = OSD::GetTopScreen();
		u32 dy = 0, val;
		for (u32 i : addrList)
		{
			if (Process::Read32(i, val))
				scr.Draw(Utils::Format("%08X: %08X", i, val), 0, dy);
			else
				scr.Draw(Utils::Format("%08X: ???", i), 0, dy);

			dy += 10;
		}
		OSDManager.Unlock();
	}
	
	void HexEditor(MenuEntry* entry)
	{
		static u32 address = 0x33099e50;
		u32 edit_value, jump_addr;
		const int view_length = 10;
		static int sx, sy;
		u32 tmp;

		u32 now_addr = address + sy * 0x10 + sx * 0x4;

		// Xをおしながら十字キーで編集の場所変更
		if (Controller::IsKeyDown(X))
		{
			if (Controller::IsKeyPressed(DU)) sy--;
			if (Controller::IsKeyPressed(DD)) sy++;
			if (Controller::IsKeyPressed(DL)) sx--;
			if (Controller::IsKeyPressed(DR)) sx++;

			if (sx < 0) sx = 0;
			if (sx > 3) sx = 3;

			if (sy < 0) sy = 0;
			if (sy > 9) sy = 9;
		}

		// Yを押しながら十字キーの上/下でスクロール
		if (Controller::IsKeyDown(Y))
		{
			if (Controller::IsKeyPressed(DU)) address -= 0x10;
			if (Controller::IsKeyPressed(DD)) address += 0x10;
		}

		// ジャンプ
		if (Controller::IsKeysDown(X + Y))
		{
			Keyboard key("ジャンプ");
			if (key.Open(jump_addr, address) == 0)
			{
				address = jump_addr;
			}
		}

		// 編集
		if (Controller::IsKeyPressed(A))
		{
			Keyboard key("値を編集");
			if (key.Open(edit_value, *(u32*)(now_addr)) == 0)
			{
				Process::Write32(now_addr, edit_value);
			}
		}

		OSDManager.Lock();
		const Screen& scr = OSD::GetTopScreen();
		for (int i = 0; i < view_length; i++)
		{
			scr.Draw(Utils::Format("%08X:", address + i * 0x10), 0, i * 10);
			for (int x = 0; x < 4; x++)
			{
				Color c = Color::Black;
				if (x == sx && i == sy) c = Color::Blue;

				if (Process::Read32((address + i * 0x10 + x * 0x4), tmp))
					scr.Draw(Utils::Format("%08X", tmp), (6 * 10 + 1) + x * ((6 * 9) + 1), i * 10, Color::White, c);
				else
					scr.Draw("????????", (6 * 10 + 1) + x * ((6 * 9) + 1), i * 10, Color::White, c);
			}
		}
		OSDManager.Unlock();
	}

	int lenUtf8(string&);
	void StringSearcher(MenuEntry* entry)
	{
		static u32		start = 0x32C00000;
		static u32		end = 0x33000000;
		static string	str = "pizza";
		static vector<u32> result;
		
		// Change option
		if( Controller::IsKeyPressed(ZR) )
		{
			Process::Pause();
			for (bool loop = true; loop; )
			{
				int r = (Keyboard("", { "開始: " << Hex(start), "終了: " << Hex(end), "文字列: " << str })).Open();
				switch (r)
				{
				case -1:
					loop = false;
					break;
				case 0:
					OSD::SwapBuffers();
					Keyboard().Open(start, start);
					break;
				case 1:
					OSD::SwapBuffers();
					Keyboard().Open(end, end);
					break;
				case 2:
					OSD::SwapBuffers();
					(Keyboard("サーチする文字列\n'***' を指定するとチャットの文字列でサーチします")).Open(str, str);
					if( str == "***" ) str = Chat::Read();
					break;
				}
				OSD::SwapBuffers();
			}
			Process::Play();
		}
		
		// Search
		if( Controller::IsKeyPressed(L) )
		{
			Process::Pause();
			
			u32 length = lenUtf8(str);
			u16* data = new u16[length];
			Process::WriteString((u32)data, str, StringFormat::Utf16);
			
			result.clear();
			for( u32 i = start; i < end; i += 2 )
			{
				if( memcmp((u16*)i, data, length * 2) == 0 )
				{
					result.push_back(i);
					i += length * 2;
				}
			}
			
			Process::Play();
		}
		
		// Show result
		if( Controller::IsKeyPressed(ZL) )
		{
			vector<string> res;
			for( u32 i : result )
				res.push_back(Utils::Format("%08X", i));
			
			OSDMenu::Set(res);
			OSDMenu::Open("サーチ結果");
		}
	}
	
	struct fake_cpu
	{
		u32 r[13]; ///< r0-r12.
		u32 sp;    ///< sp.
		u32 lr;    ///< lr.
		u32 pc;    ///< pc. May need to be adjusted.
		u32 cpsr;  ///< cpsr.
	};
	CpuRegisters	cpu_;
	fake_cpu		fake_c;
	
	void test_hook()
	{
	//  CpuRegisters c;
		memcpy( &fake_c, &cpu_, sizeof(cpu_) );
	}
	
	void cpu_rec(MenuEntry* entry)
	{
		static Hook hook;
		static u32  addr;
		
		// アドレス変更
		if( Controller::IsKeyPressed(ZR) )
		{
			Keyboard("address").Open(addr, addr);
			
			if( hook.IsEnabled() ) hook.Disable();
			hook.Initialize( addr, (u32)&test_hook );
			hook.Enable();
		}
		
		// オンオフ切り替え
		if( Controller::IsKeyPressed(ZL) )
		{
			if( hook.IsEnabled() )
			{
				hook.Disable();
			}
			else
			{
				//hook.Initialize( addr, (u32)&test_hook );
				hook.Enable();
			}
		}
		
		OSDManager.Lock();
		const Screen &scr = OSD::GetTopScreen();
		
		u32 dy = 0, i = 0;
		for( u32 r : fake_c.r )
			dy = scr.Draw(Utils::Format("r%d = %08X", i++, r), 0, dy);
		
		dy = scr.Draw(Utils::Format("sp = %08X", fake_c.sp), 0, dy);
		dy = scr.Draw(Utils::Format("lr = %08X", fake_c.lr), 0, dy);
		dy = scr.Draw(Utils::Format("pc = %08X", fake_c.pc), 0, dy);
		dy = scr.Draw(Utils::Format("cpsr = %08X", fake_c.cpsr), 0, dy);
		if( hook.IsEnabled() )
			scr.Draw(Color(0, 250, 0) << "enabled", 0, dy);
		
		OSDManager.Unlock();
	}

	void Test_opt(MenuEntry* entry)
	{
		u16  item;
		u16  name[50];
		int  len;
		std::string  str;

		if (Keyboard("Item ID").Open(item) != 0) return;
		OSD::SwapBuffers();

		len = GetItemName(item, name);
		if (len == -1) (MessageBox("error: Invalid ItemID"))();

		Process::ReadString((u32)name, str, len * 2, StringFormat::Utf16);

		std::string msg = Utils::Format("ItemID = %04X\nLength = %d\nName = %s\nArray = ", item, len, str.c_str());
		for (int i = 0; i < len; i++)
			msg += Utils::Format("%04X ", name[i]);

		(MessageBox(msg))();
	}
	
	void PatchTest(MenuEntry *entry)
	{
		
	}
	
	
}