#include "cheats.hpp"

namespace CTRPluginFramework
{
    const char *action_names[] =
    {
        "0E スライド",
        "11 寝る",
        "12 立ち泳ぎ",
        "15 ゆっくり右を向く",
        "16 ウィンドウ開く動き",
        "18 消える",
        "1B 道具をしまえない",
        "1C ジャンプした後落ち続ける",
        "1D 水しぶき",
        "1E 泳ごうとする",
        "22 黒く地面から出る",
        "24 海の幸をとる",
        "26 クラゲにさされる",
        "29 テキストボックス",
        "2C 黒くなって消える",
        "36 流れ星",
        "39 道具を切り替える",
        "3D アイテムを拾う",
        "40 雑草を抜く",
        "49 スコップ",
        "4B スコップ(空振り)",
        "4C スコップ(石にあたる)",
        "4D スコップ(物にあたる)",
        "51 アイテムが出る(?)",
        "55 オノ(空振り)",
        "58 オノ(物にあたる)",
        "5A オノ(木を切る)",
        "5F 落とし穴に落ちる",
        "60 穴にはまる",
        "61 穴から出る",
        "6A 半回転",
        "6B 木をゆする",
        "6C 水をあげる",
        "70 アミをふる",
        "72 アイテムでばかになる",
        "73 アイテムをドロップする",
        "74 アイテムを受け取る",
        "7C 座る",
        "88 傘笑い",
        "8A 傘を回す",
        "8F シャボン玉を吹く",
        "90 タンポポのわたげを飛ばす",
        "93 アイテムを食べる",
        "9D 気絶",
        "9E 起き上がる",
        "9F 倒れる",
        "A5 ハチにさされる",
        "A6 蚊にさされる",
        "DD わー",
        "E0 セーブできるようになった",
        "E7 手を振る"
    };
    
    u32  act_AnimItem = 0;
    
    void act(MenuEntry *entry)
    {
        u32 wx, wy;
        if( Controller::IsKeysDown(R + A) && Player::GetWorldCoords(&wx, &wy, 4, 1) )
        {
            Player::ExecuteAnimationWrapper(Game::GetOnlinePlayerIndex(), *(u8*)entry->GetArg(), act_AnimItem, 1, 1, 1, 0, wx, wy, true);
        }
    }
    
    MenuFolder * InitActionFolder(Color c)
    {
		MenuFolder *folder = new MenuFolder( c << "アクション", FONT_R PLUS FONT_A ": 実行" );
        *folder += new MenuEntry(c << "アイテムID", nullptr, [](MenuEntry*e){ Keyboard().Open(act_AnimItem, act_AnimItem); });
        
        int index = 0;
        for( const char *name : action_names )
        {
            MenuEntry *e = new MenuEntry(5, c << std::string(name).substr(3), act);
            e->SetArg( new int(std::stoi(std::string(name).substr(0, 2), nullptr, 16)) );
            *folder += e;
            index++;
        }
        
        return folder;
    }
    
    
    
}







