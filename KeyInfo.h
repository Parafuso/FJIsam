//KeyInfo.h
//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ISAMキー情報 クラス
//	Version	1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	classes
//		Keyinfo		:ISAMキー情報
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM定数定義

using namespace System;

//------------------------------------------------------------------------------
//	class	:keyInfo	:キー情報
//------------------------------------------------------------------------------
namespace FJIsam
{
	//キー種別
	public enum class keyType_EN	
	{
			ISKEY_UNICODE = 0,		//ユニコード：Chars
			ISKEY_CHAR,				//ワイドキャラクター：Char(SJIS等)
			ISKEY_BYTE,				//バイトデータ：Byte
			ISKEY_SNUM,				//符号付数値：Int8～Int64
			ISKEY_USNUM				//符号なし数値：UInt8～UInt64
	};

	public ref class KeyInfo sealed 
	{
	public :
		KeyInfo();												//コンストラクタ
		KeyInfo(		String^		inKeyName,
						Int32		inKeyPos,
						Int32		inKeyLen,
						keyType_EN	inKeyType,
						Byte		inKeyOrder,
						Byte		inKeyUniqueFlg,
						Int32		inSrtKeyPos,
						Int32		inSrtKeyLen,
						keyType_EN	inSrtKeyType,
						Byte		inSrtKeyOrder
				);
		KeyInfo( KeyInfo^ source );								//コピーコンストラクタ
		~KeyInfo();												//デストラクタ

		//--キー情報取得--//
		String^		getKeyName(String^ otStr );					//キー名取得
		Int32		getKeyLen( void );							//キー長取得
		Int32		getKeyPos( void );							//キー位置取得
		keyType_EN	getKeyType( void );							//キータイプ取得
		Byte		getKeyOrder( void );						//キー順序
		Byte		getKeyUniqueFlg( void );					//ユニークキーフラグ取得
		Int32		getSrtKeyLen( void );						//ソートキー長取得
		Int32		getSrtKeyPos( void );						//ソートキー位置取得
		keyType_EN	getSrtKeyType( void );						//ソートキータイプ取得
		Byte		getSrtKeyOrder( void );						//ソートキー順序
	
		//--キー情報設定--//
		Int32		Set(										//キー情報設定
						String^		inKeyName,
						Int32		inKeyPos,
						Int32		inKeyLen,
						keyType_EN	inKeyType,
						Byte		inKeyOrder,
						Byte		inKeyUniqueFlg,
						Int32		inSrtKeyPos,
						Int32		inSrtKeyLen,
						keyType_EN	inSrtKeyType,
						Byte		inSrtKeyOrder
						);

		//-- ファイル入出力用データアクセス --//
		static Int32 Length( void );								//ファイル入出力用データ長取得
		Int32		Read( array<Byte>^ inBuff, Int32 offset );		//キーデータ出力
		Int32		Write( array<Byte>^ otBuff, Int32 offset );		//キーデータ書込

	private :
		//-- メンバ --//
		String^			KeyName;								//キー名
		Int32			KeyPos;									//キー位置
		Int32			KeyLen;									//キー長
		keyType_EN		KeyType;								//キー形式
		Byte			KeyOrder;								//キー順序
		Byte			KeyUniqueFlg;							//ユニークキーフラグ
		Int32			SrtKeyPos;								//ソートキー位置
		Int32			SrtKeyLen;								//ソートキー長
		keyType_EN		SrtKeyType;								//ソートキー形式
		Byte			SrtKeyOrder;							//キー順序
	};
}