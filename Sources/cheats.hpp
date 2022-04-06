#pragma once

#include <vector>
#include <string>

#include "3ds.h"
#include "CTRPluginFramework.hpp"
#include "Helpers.hpp"
#include "Unicode.h"

#define		ITEM_NAME_ADDR(item)	( 0x31724DC0 + *(u32*)( 0x31724DC0 + (item - 0x2000) * 4 + 4 ) )
#define		TOGGLE_BOOL(b)			(b = !b)
#define		PLUS					" + "
#define		NOP						0xE1A00000

using namespace std;

namespace CTRPluginFramework
{
	using FuncPointer = void(*)(MenuEntry*);
	
	extern u32 UseItem;
	extern vector<MenuEntry*> global_entries;
	
	void TextToCheats(MenuEntry*);
	
	// セーブデータ
	void SaveDataBackups(MenuEntry*);      // セーブデータ保存
	void ChangeTownName(MenuEntry*);       // 村名変更
	void ChangePlayerName(MenuEntry*);     // プレイヤー名変更
	void ChangeKokuseki(MenuEntry*);       // 国籍変更
	void ChangeSeibetu(MenuEntry*);        // 性別変更

	// 遊び用
	void DropMod(MenuEntry*);              // オプション
	void ChangeUseItem(MenuEntry*);        // アイテム変更
	void ChangeUseItem_opt(MenuEntry*);    // アイテム変更(オプション)
	void MapEditor(MenuEntry*);            // マップエディタ
	void ItemRandomizer(MenuEntry*);       // アイテムランダマイザ
	void AutoDrop(MenuEntry*);             // 自動ドロップ
	void TouchDrop(MenuEntry*);            // タッチドロップ
	void TouchRemove(MenuEntry*);          // タッチアイテム削除
	void InventoryItemNames(MenuEntry*);   // アイテム名変更
	void TrampleAllItem(MenuEntry*);       // アイテム全削除

	// インベントリ
	void ChangeItemOption(MenuEntry*);     // アイテム選択肢変更
	void InventoryBackup(MenuEntry*);      // アイテム保存
	void AllDelete(MenuEntry*);            // すべて削除
	void ViewTokushuItem(MenuEntry*);      // 特殊アイテム表示
	void InfinityItemDrop(MenuEntry*);     // 置いてもなくならない
	void DragDropSpam(MenuEntry*);         // 連続ドラッグドロップ
	void TextToItem(MenuEntry*);           // アイテム取得
	void GetAshimotoItem(MenuEntry*);      // 足元のアイテム取得
	void FillInv(MenuEntry*);		       // 埋める
    
	// 動き、アクション
	void Idle(MenuEntry*);                 // 動作解除
	void CoordinatesModifier(MenuEntry*);  // 座標移動
	void Coord_opt(MenuEntry*);            // 座標移動(オプション)
	void WalkOverObjects(MenuEntry*);      // 壁抜け
	void RoomWarping(MenuEntry*);          // ワープ
	void Stalker(MenuEntry*);              // ストーカー
	void DPadCoord(MenuEntry*);            // 十字キー移動
	void BlockMove(MenuEntry*);            // ブロック移動
	void TouchWarping(MenuEntry*);         // タッチワープ
	void AntiCheat(MenuEntry*);            // アンチチート
	void WalkRemover(MenuEntry*);          // 足元のアイテム削除
	void WalkRemover_opt(MenuEntry*);      // 足元のアイテム削除(オプション)
    void ExecAnim(MenuEntry*);             // アニメーション実行
    void FuckingWater(MenuEntry*);         // 海の上歩ける
    void NoBreakFlower(MenuEntry*);        // 走っても花散らない
	
	// チャット
	void ChatRepeater(MenuEntry*);         // コピペ/送信
	void ChatEnter(MenuEntry*);            // 改行
	void ChatUnlockNumber(MenuEntry*);     // 数字無制限

	// その他
	void HideSaveMenu(MenuEntry*);         // セーブメニュー非表示
	void NoHitOtherPlayer(MenuEntry*);     // 他人と衝突しない
	void DisableScreenShot(MenuEntry*);    // スクショ無効化
	void FastGameSpeed(MenuEntry*);        // ゲーム速度高速
	void FastMessage(MenuEntry*);          // 高速メッセージ表示
	void TimeMachine(MenuEntry*);          // タイムマシン
	void PrivateStorage(MenuEntry*);          // 隠し倉庫
	void CameraControl(MenuEntry*);        // カメラ操作
	void ViewAshimotoItem(MenuEntry*);     // 足元のアイテム表示
	void CheaterIsland(MenuEntry*);        // チート島

	// デバッグ
	void ViewAddress(MenuEntry*);          // アドレス監視
	void HexEditor(MenuEntry*);            // HEXエディタ
	void HexEditor_opt(MenuEntry*);        // HEXエディタ(オプション)
	void StringSearcher(MenuEntry*);       // 文字列サーチ
	void cpu_rec(MenuEntry*);              // cpu記録
	void PatchTest(MenuEntry*);			   // パッチテスト
	MenuFolder *InitCallFuncFolder(Color);
	
	
}


