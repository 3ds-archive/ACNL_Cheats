#include "cheats.hpp"
#include "TextColors.h"

namespace CTRPluginFramework
{
    void PatchProcess(FwkSettings& fs)
    {
        if (Process::GetTitleID() != 0x0004000000086200) return;
        fs.WaitTimeToBoot = Seconds(5.f);
        
        // 「ここにはもう置けないヨ」を非表示にする
        Process::Write32(0x19DF08, NOP); // EB10FA8C
        
        // ハチに刺されない
        Process::Write32(0x64C680, 0xE3A01006);
        
        // inventory base
        Process::Write32(0x19B604, NOP);
        Process::Write32(0x19BE8C, NOP);
        Process::Write32(0x19B9D8, NOP);
        Process::Write32(0x19B8F0, NOP);
        Process::Write32(0x19BFB0, NOP);
        Process::Write32(0x7238C0, NOP);
        Process::Write32(0x19B52C, 0xE3A06001); // items2money
        
        // pitfall
        Process::Write32(0x6774DC, 0xEA00002D); // 1A00002D
        Process::Write32(0x659160, 0xEA000014); // 1A000014
        
        // validate
        Process::Write32(0x769034, 0xE3A00001); // E3A00000
        
        // items2money
        Process::Write32(0x19B52C, 0xE3A06001); // 03A06001
        
        // prevent particle crash
        Process::Write32(0x54F008, 0xE3A0C000); // E59DC070
        Process::Write32(0x54F300, 0xE3A0C000); // E59DC070
        Process::Write32(0x71FBB4, 0xE3A0C000); // E592C0F0
        
        // prevent rockbreak crash
        Process::Write32(0x5A1658, 0xF3); // 000001F6
        
        // idle after tree shake/cut
        Process::Write8(0x55F0F8, 6); // E5990000
        Process::Write8(0x660E20, 6); // E3A01001
        
        // shovel animations
        Process::Write32(0x663294, 0xEA000028); // 0A000028
        
        // drop
        Process::Write32(0x59E5DC, 0xEA000004); // 0A000004
        
        // host drop
        Process::Write32(0x59FD8C, 0xE1A00000); // 0A00002C // create locked spot pkt send
        Process::Write32(0x59FC2C, 0xE1A00000); // 0A00002C // clear locked spot pkt send
        Process::Write32(0x59E894, 0xEA000010); // 0A000010 // pkt is from host
        Process::Write32(0x59E6D0, 0xEA000003); // 0A000003 // bypass check in 0x59FC7C
        Process::Write32(0x596D2C, 0xE1A00000); // 0A00001D // unnecessary?
        Process::Write32(0x596E14, 0xE1A00000); // 0A000025 // unnecessary?
        Process::Write32(0x59E1D8, 0xE1A00000); // 0A00003B
        Process::Write32(0x59F2E8, 0xEA000022); // 0A000022 // item tree drop
        Process::Write32(0x59F72C, 0xEA00002D); // 0A00002D // fruit tree drop
        
        // drop everywhere
        Process::Write32(0x1655D4, 0xE3A00001);
        Process::Write32(0x1655E0, 0xEA000006);
        Process::Write32(0x1654D4, 0xEA000005);
        Process::Write32(0x165568, 0xEA000010); // on other players
        
        // some drop things
        for( int i = 0; i <= 0x21; i++ )
            Process::Write32(0x59F88C + i * 4, 0x59FA58);
        
        // Prevent others from crashing you
        Process::Write32(0x598A30, 0xE3A00001);    // Disable Sprite Spawn for Anim 0x5f and 0x60
        Process::Write32(0x5A0FEC, NOP);           // Makes Burying spawn toss sprite on the ground
        Process::Write32(0x5A102C, 0xE3A00001);    // Disable Eat Item Sprite Spawn part 1
        Process::Write32(0x6502C4, 0x03500001);    // Disable Eat Item Sprite Spawn part 2
        Process::Write32(0x6502DC, NOP);           // Another fix for buring items
        Process::Write32(0x659594, 0x01000540);    // Fixes annoying as fuck sound
        Process::Write32(0x664940, 0xE3A01006);    // Replaces Animation for falling into hole after sit, lay down. Prevents you from accidentally crashing others.
        Process::Write32(0x68A074, 0xE3A01016);    // Changes "putinpocket" anim after eat
        
        // FIX ROOM EDIT MENU CRASH ONLINE
        Process::Write32(0x6D1328, 0xE3A0008A); // Menu Change to error
        Process::Write32(0x66F398, 0xE3A01016); // In-Pocket Animation
        
        // SANTA BAG FIX
        Process::Write16(0x68BBE4, 0x7FFE);
        Process::Write16(0x68806C, 0x7FFE);
        
        // BYPASS CHECKS
        Process::Write32(0x1D4300, 0xE3A00001);
        Process::Write32(0x1D431C, 0xE3A00001);
        Process::Write32(0x1D432C, 0xE3A00001);
        Process::Write32(0x7570C4, 0xE1A00005);
        
        // ラグなし
        Process::Write32(0x59E6D0, 0xEA000003);
        Process::Write32(0x59E894, 0xEA000010);
        Process::Write32(0x59FD8C, 0xE1A00000);
        
        // 壁抜け
        Process::Write8(0x64eef7, 0xea);
        Process::Write8(0x64ef0f, 0xea);
        Process::Write8(0x64f073, 0xea);
        Process::Write8(0x64f0eb, 0xea);
        Process::Write8(0x64f1bb, 0xea);
        Process::Write8(0x64f1e7, 0xea);
        Process::Write32(0x64f19c, NOP);
        Process::Write32(0x64f1b4, NOP);
    }
	
    MenuEntry* EnableEntry(MenuEntry* e)
    {
        if (e != nullptr) e->Enable();
        return e;
    }
	
    MenuEntry* EntryWithHotkey(MenuEntry* entry, const Hotkey& hotkey)
    {
        if (entry != nullptr)
        {
            entry->Hotkeys += hotkey;
            entry->SetArg(new std::string(entry->Name()));
            entry->Name() += " " + hotkey.ToString();
            entry->Hotkeys.OnHotkeyChangeCallback([](MenuEntry* entry, int index)
                {
                    std::string* name = reinterpret_cast<std::string*>(entry->GetArg());
                    entry->Name() = *name + " " + entry->Hotkeys[0].ToString();
                });
        }
        return (entry);
    }
	
    MenuEntry* EntryWithHotkey(MenuEntry* entry, const std::vector<Hotkey>& hotkeys)
    {
        if (entry != nullptr)
        {
            for (const Hotkey& hotkey : hotkeys)
                entry->Hotkeys += hotkey;
        }
        return (entry);
    }
	
    // 修飾
    MenuFolder * ModFolder(const Color& color, const string& mod, const string& name, const vector<MenuEntry*>& entries)
    {
        MenuFolder *folder = new MenuFolder(color << mod << name << mod);

        for( const MenuEntry *e : entries )
        {
            e->Name() = color << e->Name();
            *folder += e;
        }

        return folder;
    }
	
    vector<MenuEntry*> global_entries;
    
    MenuFolder * InitActionFolder(Color c);
    void CreateMenu(PluginMenu &menu)
    {
		using tree = TreeItem;
		global_entries.push_back( EnableEntry(new MenuEntry("アイテムID表示", [](MenuEntry*e){})) );
		
		/*
		 *	セーブデータ
		 *****************/
        menu += ModFolder(Color(40, 150, 255), "\uE015", "セーブデータ",
        {
            new MenuEntry("セーブデータ保存", nullptr, SaveDataBackups),
            new MenuEntry("村名変更", nullptr, ChangeTownName),
            new MenuEntry("プレイヤー名変更", nullptr, ChangePlayerName),
            new MenuEntry("性別変更", nullptr, ChangeSeibetu),
        });
		
		/*
		 *	遊び人
		 *****************/
        MenuFolder *asobi = ModFolder(Color(40, 230, 106), "\uE016", "遊び人",
        {
			EnableEntry(new MenuEntry("アイテム変更 " FONT_Y PLUS FONT_DR, ChangeUseItem, ChangeUseItem_opt)),
			global_entries[0],// アイテムID表示
			EnableEntry(new MenuEntry("動作解除 " FONT_R PLUS FONT_DD, Idle, "プレイヤーのアニメ状態をデフォルトに戻す")),
			new MenuEntry("オプション", DropMod),
			new MenuEntry("マップエディタ " FONT_ZR PLUS FONT_DU, MapEditor),
			new MenuEntry("アイテムランダマイザ " FONT_L PLUS FONT_DL, ItemRandomizer),
			new MenuEntry("自動ドロップ " FONT_B PLUS FONT_DL, AutoDrop),
			new MenuEntry("タッチドロップ", TouchDrop),
			new MenuEntry("タッチアイテム削除", TouchRemove),
			new MenuEntry("座標移動", CoordinatesModifier, Coord_opt),
			new MenuEntry("壁抜け " FONT_L PLUS FONT_DU, WalkOverObjects),
			new MenuEntry("ワープ " FONT_L PLUS FONT_DR, RoomWarping),
			new MenuEntry("ストーカー " FONT_A PLUS FONT_DPAD, Stalker),
			new MenuEntry("十字キー移動 " FONT_R PLUS FONT_DPAD, DPadCoord),
			new MenuEntry("ブロック移動", BlockMove),
			new MenuEntry("タッチワープ", TouchWarping),
			new MenuEntry("アイテム名変更", nullptr, InventoryItemNames),
			new MenuEntry("アイテム全削除 " FONT_R PLUS FONT_Y, TrampleAllItem),
			new MenuEntry("足元のアイテム削除", WalkRemover, WalkRemover_opt),
			new MenuEntry("アニメーション実行 " FONT_L PLUS FONT_A, ExecAnim, FONT_L PLUS FONT_DD ": 設定変更"),
		});
		*asobi += InitActionFolder(Color::Orange);
		menu += asobi;
		
		/*
		 *	インベントリ
		 *****************/
        menu += ModFolder(Color(255, 255, 0), "\uE017", "インベントリ",
        {
            new MenuEntry("アイテム選択肢変更", nullptr, ChangeItemOption),
            new MenuEntry("インベントリ保存", nullptr, InventoryBackup),
            new MenuEntry("すべて削除", nullptr, AllDelete),
            EnableEntry(new MenuEntry("特殊アイテム表示", ViewTokushuItem)),
            new MenuEntry("置いてもなくならない", InfinityItemDrop),
            new MenuEntry("連続ドラッグドロップ", DragDropSpam),
            new MenuEntry("アイテム取得 " FONT_X PLUS FONT_DR, TextToItem),
            new MenuEntry("足元のアイテム取得 " FONT_X PLUS FONT_DD, GetAshimotoItem),
			new MenuEntry("埋める " FONT_ZL, FillInv),
        });
        
		/*
		*	チャット
		*****************/
		menu += ModFolder(Color(255, 180, 200), "\uE013", "チャット",
		{
			EnableEntry(new MenuEntry("コマンド", TextToCheats)),
			EnableEntry(new MenuEntry("改行", ChatEnter)),
			new MenuEntry("コピペ/送信", ChatRepeater),
			new MenuEntry("数字無制限", ChatUnlockNumber),
		});
		
		/*
		 *	その他
		 *****************/
		MenuFolder *misc_f = ModFolder(Color(245, 70, 70), "\uE008", "その他",
		{
			EnableEntry(new MenuEntry("セーブメニュー出さない", HideSaveMenu)),
			EnableEntry(new MenuEntry("他人と衝突しない", NoHitOtherPlayer)),
			EnableEntry(new MenuEntry("スクショ無効化", DisableScreenShot)),
			EnableEntry(new MenuEntry("海の上歩ける", FuckingWater)),
			EnableEntry(new MenuEntry("走っても花散らない", NoBreakFlower)),
			new MenuEntry("ゲーム速度高速", FastGameSpeed),
			new MenuEntry("高速メッセージ表示", FastMessage),
			new MenuEntry("カメラ操作", CameraControl),
			new MenuEntry("足元のアイテム表示", ViewAshimotoItem),
			new MenuEntry("アンチチート " FONT_L, AntiCheat),
			new MenuEntry("チート島", CheaterIsland),
        });
		*misc_f += ModFolder(Color(245, 70, 70), "", "メニュー " FONT_R PLUS FONT_L PLUS FONT_DR,
		{
			new MenuEntry(5, "タイムマシン", TimeMachine),
			new MenuEntry(5, "隠し倉庫", PrivateStorage),
		});
		MenuFolder *dbg = ModFolder(Color(170, 130, 100), "", "デバッグ",
		{
			new MenuEntry("アドレス記録/監視", ViewAddress),
			new MenuEntry("HEX エディタ", HexEditor),
			new MenuEntry("文字列サーチ", StringSearcher),
			new MenuEntry("cpu記録", cpu_rec),
			new MenuEntry("パッチテスト", PatchTest),
		});
		*dbg += InitCallFuncFolder(Color(170, 130, 100));
		*misc_f += dbg;
		menu += misc_f;
		
		
    }
	
	
    bool Notify_OSDcallback(const Screen&);
	bool DrawUseItem(const Screen&);
    int main()
    {
        if( Process::GetTitleID() != 0x0004000000086200 )
        {
            (MessageBox("ここでは実行できません"))();
            return -1;
        }
        
        OSD::Run(Notify_OSDcallback);
        OSD::Run(DrawUseItem);
        
        PluginMenu menu("Coffee", 1, 1, 0);
        
        menu.SynchronizeWithFrame(true);
        menu.ShowWelcomeMessage(false);
        
        CreateMenu(menu);
        menu.Run();
		
		return 0;
    }
}




