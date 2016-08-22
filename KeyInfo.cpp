//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ISAMキー情報 クラス　ソース
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	classes
//		keyinfo	:ISAMキー情報
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "KeyInfo.h"			//キー情報
#include "FJIsamFunc.h"			//ISAM用関数
#include "FJIsamException.h"	//FJIsam例外クラス

using namespace FJIsam;

//------------------------------------------------------------------------------
//***** コンストラクタ１ *****
FJIsam::KeyInfo::KeyInfo()
{
}

//***** コンストラクタ２ *****
FJIsam::KeyInfo::KeyInfo(
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
						)
{
	//初期化
	Set( inKeyName, inKeyPos, inKeyLen, inKeyType, inKeyOrder,inKeyUniqueFlg, 
		inSrtKeyPos, inSrtKeyLen, inSrtKeyType, inSrtKeyOrder );

}

//***** コンストラクタ３ *****
FJIsam::KeyInfo::KeyInfo( KeyInfo^ source )
{
	//初期化
	Set( source->KeyName, source->KeyPos, source->KeyLen, 
		source->KeyType, source->KeyOrder,source->KeyUniqueFlg, 
		source->SrtKeyPos, source->SrtKeyLen, source->SrtKeyType, source->SrtKeyOrder );

}

//------------------------------------------------------------------------------
//***** デストラクタ *****
FJIsam::KeyInfo::~KeyInfo()
{
	delete KeyName;	
}

//------------------------------------------------------------------------------
//キー情報取得
//------------------------------------------------------------------------------
//***** キー名取得 *****
String^ FJIsam::KeyInfo::getKeyName(String^ otStr )
{
	//-- 引数チェック --//
	if( otStr == nullptr )
	{
		return String::Copy( KeyName );
	}

	//-- キー名セット --//
	otStr->CopyTo(0, KeyName->ToCharArray(), 0, IS_KEYNAMELEN );

	//正常終了
	return otStr;

}

//***** キー長取得 *****
Int32 FJIsam::KeyInfo::getKeyLen( void )
{
	return KeyLen;
}

//***** キー位置取得
Int32 FJIsam::KeyInfo::getKeyPos( void )
{
	return KeyPos;
}

//***** キータイプ取得 *****
keyType_EN FJIsam::KeyInfo::getKeyType( void )
{
	return KeyType;
}

//***** キー順序取得 *****
Byte FJIsam::KeyInfo::getKeyOrder( void )
{
	return KeyOrder;
}

//***** ユニークキーフラグ取得 *****
Byte FJIsam::KeyInfo::getKeyUniqueFlg( void )
{
	return KeyUniqueFlg;
}

//***** ソートキー長取得 *****
Int32 FJIsam::KeyInfo::getSrtKeyLen( void )
{
	return SrtKeyLen;
}

//***** ソートキー位置取得 *****
Int32 FJIsam::KeyInfo::getSrtKeyPos( void )
{
	return SrtKeyPos;
}

//***** ソートキータイプ取得 *****
keyType_EN FJIsam::KeyInfo::getSrtKeyType( void )
{
	return SrtKeyType;
}

//***** ソートキー順序取得 *****
Byte FJIsam::KeyInfo::getSrtKeyOrder( void )
{
	return SrtKeyOrder;
}


//------------------------------------------------------------------------------
//キー情報設定
//------------------------------------------------------------------------------
Int32 FJIsam::KeyInfo::Set(
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
				)
{
	//-- 単体条件チェック --//
	//キー名
	if( inKeyName == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー名が空です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKeyName->Length > IS_KEYNAMELEN )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー名が長すぎます。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( KeyName != nullptr ) delete KeyName;
	KeyName = String::Copy( inKeyName );

	//キー位置
	KeyPos = inKeyPos;

	//キー長
	if( inKeyLen == 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー長が0です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	KeyLen = inKeyLen;

	//キータイプ
	KeyType = inKeyType;

	//キー順序
	if( inKeyOrder != 'A' && inKeyOrder != 'D' )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー順序が不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}
	KeyOrder = inKeyOrder;

	//ユニークキーフラグ
	if( inKeyUniqueFlg != IS_UNIQUEKEY &&
		inKeyUniqueFlg != IS_DUPLICATEKEY )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ユニークキーフラグが不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}
	KeyUniqueFlg = inKeyUniqueFlg;

	//ソートキー位置
	SrtKeyPos = inSrtKeyPos;

	//ソートキー長
	SrtKeyLen = inSrtKeyLen;

	//ソートキータイプ
	SrtKeyType = inSrtKeyType;

	//ソートキー順序
	if( inSrtKeyLen != 0 &&	( inSrtKeyOrder != 'A' && inSrtKeyOrder != 'D') )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ソートキー順序が不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}
	SrtKeyOrder = inSrtKeyOrder;

	//-- 複合条件チェック --//
	//キー最後尾位置チェック
	if( (Int64)(KeyPos + KeyLen) > Int32::MaxValue )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー位置が不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//ソート最後尾位置チェック
	if( (Int64)(SrtKeyPos + SrtKeyLen) > Int32::MaxValue )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ソートキー位置が不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//キー形式＆長さチェック
	switch( KeyType )
	{
	case keyType_EN::ISKEY_UNICODE :
		if( KeyLen % 2 == 1 )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー長が不正です(UNICODE char)。");
			tmpFJIsamException->ErrorCode = 3001;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}
		break;

	case keyType_EN::ISKEY_SNUM :
	case keyType_EN::ISKEY_USNUM :
		if( KeyLen != 1 &&
			KeyLen != 2 &&
			KeyLen != 4 &&
			KeyLen != 8 )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー長が不正です(numeric)。");
			tmpFJIsamException->ErrorCode = 3001;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;
		}
	}

	//-- 正常終了 --//
	return 0;
}

//------------------------------------------------------------------------------
//ファイル入出力用データアクセス
//------------------------------------------------------------------------------
//***** ファイル入出力用データ長取得 *****//
Int32 FJIsam::KeyInfo::Length( void )
{
	return sizeof(Char) *20 +
			sizeof(Int32) +
			sizeof(Int32) +
			sizeof(keyType_EN) +
			sizeof(Byte) +
			sizeof(Byte) +
			sizeof(Int32) +
			sizeof(Int32) +
			sizeof(keyType_EN) +
			sizeof(Byte);
}

//------------------------------------------------------------------------------
//ファイル読み出しデータ取得
//------------------------------------------------------------------------------
Int32 FJIsam::KeyInfo::Read( array<Byte>^ inBuff, Int32 offset  )
{
	int tmpKeyType;

	//-- 引数チェック --//
	if(  inBuff  == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"読み取り領域が空です。" );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	if( offset < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"オフセット値が不正です。" );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( inBuff->Length - offset < KeyInfo::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"読み取り領域が小さすぎます。" );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- メモリセット --//
	try
	{
		//キー名
		KeyName = FJIFuncs::ArrayToString( inBuff, offset, IS_KEYNAMELEN );
		offset += IS_KEYNAMELEN * sizeof( Char );

		//キー位置
		KeyPos = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( KeyPos );
		//キー長
		KeyLen = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( KeyLen );

		//キータイプ
		tmpKeyType = FJIFuncs::ArrayToInt32( inBuff, offset );
		if( (keyType_EN)tmpKeyType < keyType_EN::ISKEY_UNICODE &&
			(keyType_EN)tmpKeyType > keyType_EN::ISKEY_USNUM )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キータイプが不正です。" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
		KeyType = (keyType_EN)tmpKeyType;
		offset += sizeof( tmpKeyType );

		//キー順序
		KeyOrder = inBuff[offset];
		if( KeyOrder != 'A' && KeyOrder != 'D' && KeyOrder != ' ' )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー順序が不正です。" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
		offset++;

		//ユニークキーフラグ
		KeyUniqueFlg = inBuff[offset];
		if( KeyUniqueFlg != IS_UNIQUEKEY &&
			KeyUniqueFlg != IS_DUPLICATEKEY )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ユニークキーフラグが不正です。" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
		offset++;

		//ソートキー位置
		SrtKeyPos = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( SrtKeyPos );

		//ソートキー長
		SrtKeyLen = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( SrtKeyLen );

		//ソートキータイプ
		tmpKeyType = FJIFuncs::ArrayToInt32( inBuff, offset );
		if( (keyType_EN)tmpKeyType < keyType_EN::ISKEY_UNICODE &&
			(keyType_EN)tmpKeyType > keyType_EN::ISKEY_USNUM )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ソートキータイプが不正です。" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}
		SrtKeyType = (keyType_EN)tmpKeyType;
		offset += sizeof( tmpKeyType );

		//キー順序
		SrtKeyOrder = inBuff[offset];
		if( SrtKeyOrder != 'A' && SrtKeyOrder != 'D' && SrtKeyOrder != ' ' )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ソートキー順序が不正です。" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
		offset++;

	}catch(Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー情報設定に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return offset;
}

//------------------------------------------------------------------------------
//キー情報書込
//------------------------------------------------------------------------------
int FJIsam::KeyInfo::Write( array<Byte>^ otBuff, Int32 offset )
{
	//-- 引数チェック --//
	//バッファ有無
	if( otBuff == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"書込領域が空です。");
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//オフセットチェック
	if( offset < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//バッファサイズ
	if(otBuff->Length - offset < KeyInfo::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"書込領域が小さすぎます。");
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- メモリセット --//
	try
	{
		//キー名
		FJIFuncs::StringToArray( KeyName, otBuff, offset, IS_KEYNAMELEN );
		offset += IS_KEYNAMELEN * sizeof( Char );
	
		//キー位置
		FJIFuncs::Int32ToArray( KeyPos, otBuff, offset );
		offset += sizeof( KeyPos );

		//キー長
		FJIFuncs::Int32ToArray( KeyLen, otBuff, offset );
		offset += sizeof( KeyLen );

		//キー種別
		FJIFuncs::Int32ToArray( (Int32)KeyType, otBuff, offset );
		offset += sizeof( Int32 );

		//キー順序
		otBuff[offset] = KeyOrder;
		offset++;

		//ユニークキーフラグ
		otBuff[offset] = KeyUniqueFlg;
		offset++;

		//ソートキー位置
		FJIFuncs::Int32ToArray( SrtKeyPos, otBuff, offset );
		offset += sizeof( SrtKeyPos );

		//ソートキー長
		FJIFuncs::Int32ToArray( SrtKeyLen, otBuff, offset );
		offset += sizeof( SrtKeyLen );

		//ソートキー種別
		FJIFuncs::Int32ToArray( (Int32)SrtKeyType, otBuff, offset );
		offset += sizeof( Int32 );

		//ソートキー順序
		otBuff[offset] = SrtKeyOrder;
		offset++;

	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー情報出力に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//--正常終了 --//
	return offset;
}
