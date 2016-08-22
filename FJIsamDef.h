// FJIsamDef.h
///------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　基本クラス　定義ファイル
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
#pragma once

namespace FJIsam
{

	//レコード削除フラグ(ISAM、サブインデックス共通)
	static const Byte IS_RECNOR = '\0';				//レコード有効
	static const Byte IS_RECDEL = '\xff';			//レコード削除済み

	//レコード同一キーフラグ
	static const Byte IS_NOEACHKEY = 0;				//同一キーなし
	static const Byte IS_EACHKEYFST = 1;			//同一キー先頭
	static const Byte IS_EACHKEYMID = 2;			//同一キー前後あり
	static const Byte IS_EACHKEYLST = 3;			//同一キー最後

	//ユニークキーフラグ
	static const Byte IS_UNIQUEKEY = '\0';			//ユニークキーのみ許可
	static const Byte IS_DUPLICATEKEY = '\xff';		//重複キー許可

	//データ長
	static const Int32 IS_FIDLEN = 16;				//ファイルID
	static const Int32 IS_VERLEN = 16;				//ファイル作成ISAMライブラリバージョン
	static const Int32 IS_UINFLEN = 256;			//ユーザー情報

	static const Int32 IS_KEYNAMELEN = 20;			//キー名長

	//拡張子
	#define IS_EXTSTR  L".fis"						//ISAM拡張子

}