//
//  toHiragana.cpp
//  MkXTBWikiplexus
//
//  Created by 河田 智明 on 7/28/11.
//  Copyright 2011 Nexhawks. All rights reserved.
//

#include "stdafx.h"

std::string toHiragana(std::string s);

static std::string replaceString(const std::string& str, const std::string& from, const std::string& to){
	std::string::size_type pos=0;
	std::string newStr;
	if(str.find(from)==std::string::npos)
		return str;
	while(pos<str.size()){
		std::string::size_type nextPos;
		nextPos=str.find(from, pos);
		if(nextPos==std::string::npos)
			break;
		newStr+=str.substr(pos, nextPos-pos);
		newStr+=to;
		pos=nextPos+from.size();
	}
	newStr+=str.substr(pos);
	return newStr;
}

std::string toHiragana(std::string s){
	s=replaceString(s, "ア", "あ");
	s=replaceString(s, "イ", "い");
	s=replaceString(s, "ウ", "う");
	s=replaceString(s, "エ", "え");
	s=replaceString(s, "オ", "お");
	s=replaceString(s, "カ", "か");
	s=replaceString(s, "キ", "き");
	s=replaceString(s, "ク", "く");
	s=replaceString(s, "ケ", "け");
	s=replaceString(s, "コ", "こ");
	s=replaceString(s, "サ", "さ");
	s=replaceString(s, "シ", "し");
	s=replaceString(s, "ス", "す");
	s=replaceString(s, "セ", "せ");
	s=replaceString(s, "ソ", "そ");
	s=replaceString(s, "タ", "た");
	s=replaceString(s, "チ", "ち");
	s=replaceString(s, "ツ", "つ");
	s=replaceString(s, "テ", "て");
	s=replaceString(s, "ト", "と");
	s=replaceString(s, "ナ", "な");
	s=replaceString(s, "ニ", "に");
	s=replaceString(s, "ヌ", "ぬ");
	s=replaceString(s, "ネ", "ね");
	s=replaceString(s, "ノ", "の");
	s=replaceString(s, "ハ", "は");
	s=replaceString(s, "ヒ", "ひ");
	s=replaceString(s, "フ", "ふ");
	s=replaceString(s, "ヘ", "へ");
	s=replaceString(s, "ホ", "ほ");
	s=replaceString(s, "マ", "ま");
	s=replaceString(s, "ミ", "み");
	s=replaceString(s, "ム", "む");
	s=replaceString(s, "メ", "め");
	s=replaceString(s, "モ", "も");
	s=replaceString(s, "ヤ", "や");
	s=replaceString(s, "ユ", "ゆ");
	s=replaceString(s, "ヨ", "よ");
	s=replaceString(s, "ラ", "ら");
	s=replaceString(s, "リ", "り");
	s=replaceString(s, "ル", "る");
	s=replaceString(s, "レ", "れ");
	s=replaceString(s, "ロ", "ろ");
	s=replaceString(s, "ワ", "わ");
	s=replaceString(s, "ヰ", "ゐ");
	s=replaceString(s, "ヱ", "ゑ");
	s=replaceString(s, "ヲ", "を");
	s=replaceString(s, "ン", "ん");
	s=replaceString(s, "ガ", "が");
	s=replaceString(s, "ギ", "ぎ");
	s=replaceString(s, "グ", "ぐ");
	s=replaceString(s, "ゲ", "げ");
	s=replaceString(s, "ゴ", "ご");
	s=replaceString(s, "ザ", "ざ");
	s=replaceString(s, "ジ", "じ");
	s=replaceString(s, "ズ", "ず");
	s=replaceString(s, "ゼ", "ぜ");
	s=replaceString(s, "ゾ", "ぞ");
	s=replaceString(s, "ダ", "だ");
	s=replaceString(s, "ヂ", "ぢ");
	s=replaceString(s, "ヅ", "づ");
	s=replaceString(s, "デ", "で");
	s=replaceString(s, "ド", "ど");
	s=replaceString(s, "バ", "ば");
	s=replaceString(s, "ビ", "び");
	s=replaceString(s, "ブ", "ぶ");
	s=replaceString(s, "ベ", "べ");
	s=replaceString(s, "ボ", "ぼ");
	s=replaceString(s, "パ", "ぱ");
	s=replaceString(s, "ピ", "ぴ");
	s=replaceString(s, "プ", "ぷ");
	s=replaceString(s, "ペ", "ぺ");
	s=replaceString(s, "ポ", "ぽ");
	s=replaceString(s, "ァ", "ぁ");
	s=replaceString(s, "ィ", "ぃ");
	s=replaceString(s, "ゥ", "ぅ");
	s=replaceString(s, "ェ", "ぇ");
	s=replaceString(s, "ォ", "ぉ");
	s=replaceString(s, "ヵ", "ゕ");
	s=replaceString(s, "ヶ", "ゖ");
	s=replaceString(s, "ッ", "っ");
	s=replaceString(s, "ャ", "ゃ");
	s=replaceString(s, "ュ", "ゅ");
	s=replaceString(s, "ョ", "ょ");
	s=replaceString(s, "ヮ", "ゎ");
	s=replaceString(s, "ヴ", "ゔ");
	s=replaceString(s, "カ゚", "か゚");
	s=replaceString(s, "キ゚", "き゚");
	s=replaceString(s, "ク゚", "く゚");
	s=replaceString(s, "ケ゚", "け゚");
	s=replaceString(s, "コ゚", "こ゚");
	s=replaceString(s, "ヽ", "ゝ");
	s=replaceString(s, "ヾ", "ゞ");
	s=replaceString(s, "ヿ", "ゟ");
	
	
	return s;
}

std::string avoidDangerousChars(std::string s){
	s=replaceString(s, "〜", "ー");
	s=replaceString(s, "～", "ー");
	s=replaceString(s, "—", "-");
	
	s=replaceString(s, "Ⅰ", "1");
	s=replaceString(s, "Ⅱ", "2");
	s=replaceString(s, "Ⅲ", "3");
	s=replaceString(s, "Ⅳ", "4");
	s=replaceString(s, "Ⅴ", "5");
	s=replaceString(s, "Ⅵ", "6");
	s=replaceString(s, "Ⅶ", "7");
	s=replaceString(s, "Ⅷ", "8");
	s=replaceString(s, "Ⅸ", "9");
	s=replaceString(s, "Ⅹ", "10");
	s=replaceString(s, "Ⅺ", "11");
	s=replaceString(s, "Ⅻ", "12");
	
	s=replaceString(s, "ⅰ", "1");
	s=replaceString(s, "ⅱ", "2");
	s=replaceString(s, "ⅲ", "3");
	s=replaceString(s, "ⅳ", "4");
	s=replaceString(s, "ⅴ", "5");
	s=replaceString(s, "ⅵ", "6");
	s=replaceString(s, "ⅶ", "7");
	s=replaceString(s, "ⅷ", "8");
	s=replaceString(s, "ⅸ", "9");
	s=replaceString(s, "ⅹ", "10");
	s=replaceString(s, "ⅺ", "11");
	s=replaceString(s, "ⅻ", "12");
	
	s=replaceString(s, "①", "1");
	s=replaceString(s, "②", "2");
	s=replaceString(s, "③", "3");
	s=replaceString(s, "④", "4");
	s=replaceString(s, "⑤", "5");
	s=replaceString(s, "⑥", "6");
	s=replaceString(s, "⑦", "7");
	s=replaceString(s, "⑧", "8");
	s=replaceString(s, "⑨", "9");
	s=replaceString(s, "⑩", "10");
	s=replaceString(s, "⑪", "11");
	s=replaceString(s, "⑫", "12");
	s=replaceString(s, "⑬", "13");
	s=replaceString(s, "⑭", "14");
	s=replaceString(s, "⑮", "15");
	s=replaceString(s, "⑯", "16");
	s=replaceString(s, "⑰", "17");
	s=replaceString(s, "⑱", "18");
	s=replaceString(s, "⑲", "19");
	s=replaceString(s, "⑳", "20");
	
	s=replaceString(s, "㍉", "ミリ");
	s=replaceString(s, "㌔", "キロ");
	s=replaceString(s, "㌢", "センチ");
	s=replaceString(s, "㍍", "メートル");
	s=replaceString(s, "㌘", "グラム");
	s=replaceString(s, "㌧", "トン");
	s=replaceString(s, "㌃", "アール");
	s=replaceString(s, "㌶", "ヘクタール");
	s=replaceString(s, "㍑", "リットル");
	s=replaceString(s, "㍗", "ワット");
	s=replaceString(s, "㌍", "カロリー");
	s=replaceString(s, "㌦", "ドル");
	s=replaceString(s, "㌣", "セント");
	s=replaceString(s, "㌫", "パーセント");
	s=replaceString(s, "㍊", "ミリバール");
	s=replaceString(s, "㌻", "ページ");
	s=replaceString(s, "㎜", "mm");
	s=replaceString(s, "㎝", "cm");
	s=replaceString(s, "㎞", "km");
	s=replaceString(s, "㎎", "mg");
	s=replaceString(s, "㎏", "kg");
	s=replaceString(s, "㏄", "cc");
	s=replaceString(s, "㎡", "m2");
	s=replaceString(s, "㍻", "[平成]");
	s=replaceString(s, "〝", "\"");
	s=replaceString(s, "〟", "\"");
	s=replaceString(s, "№", "No");
	s=replaceString(s, "㏍", "KK");
	s=replaceString(s, "℡", "TEL");
	s=replaceString(s, "㊤", "上");
	s=replaceString(s, "㊥", "中");
	s=replaceString(s, "㊦", "下");
	
	s=replaceString(s, "㊧", "左");
	s=replaceString(s, "㊨", "右");
	s=replaceString(s, "㈱", "[株]");
	s=replaceString(s, "㈲", "[有]");
	s=replaceString(s, "㈹", "[代]");
	s=replaceString(s, "㍾", "[明治]");
	s=replaceString(s, "㍽", "[大正]");
	s=replaceString(s, "㍼", "[昭和]");
	s=replaceString(s, "≒", "=");
	s=replaceString(s, "≡", "[合同]");
	s=replaceString(s, "∫", "[積分]");
	s=replaceString(s, "∮", "[積分]");
	s=replaceString(s, "∑", "シグマ");
	s=replaceString(s, "√", "ルート");
	
	s=replaceString(s, "®", "[R]");
	s=replaceString(s, "©", "[C]");
	s=replaceString(s, "℗", "[P]");
	s=replaceString(s, "™", "[TM]");
	s=replaceString(s, "℠", "[SM]");
	s=replaceString(s, "№", "No");
	s=replaceString(s, "ª", "a");
	s=replaceString(s, "º", "o");
	s=replaceString(s, "℔", "lb");
	s=replaceString(s, "℥", "オンス");
	s=replaceString(s, "ℨ", "Z");
	s=replaceString(s, "ℬ", "B");
	s=replaceString(s, "ℊ", "g");
	s=replaceString(s, "µ", "マイクロ");
	s=replaceString(s, "Ω", "オーム");
	s=replaceString(s, "ℹ", "i");
	s=replaceString(s, "ℌ", "H");
	s=replaceString(s, "ℑ", "I");
	s=replaceString(s, "℞", "Px");
	s=replaceString(s, "ℳ", "M");
	s=replaceString(s, "℃", "C");
	s=replaceString(s, "℉", "F");
	s=replaceString(s, "℀", "account of");
	s=replaceString(s, "℁", "addressed to the subject");
	s=replaceString(s, "℅", "care of");
	s=replaceString(s, "℆", "cada una");
	
	s=replaceString(s, "º", "");
	s=replaceString(s, "ª", "");
	
	s=replaceString(s, "¿", "");
	s=replaceString(s, "¡", "");
	
	
	

	
	
	return s;
}
