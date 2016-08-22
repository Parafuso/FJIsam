//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　共通関数　ヘッダ
//	Version 1.2		Create Date 2006/03/01 新規作成
//  Version 1.2.1	Change Date 2011/07/08 KeyStringConv 修正（'や空白を無視する)
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		KeyCompare	:キー比較
//------------------------------------------------------------------------------
#pragma once

#include "FJIsam.h"					//ISAMクラス

namespace FJIsam
{

	//キー検索オプション
	public enum class KeyCmpOpt_EN
	{
		Key = 0,					//キーのみ比較
		Srt,						//ソートキーのみ比較
		All							//キー→ソートキーの順で比較
	};

	public ref class FJIFuncs
	{
	public :
	//-- 関数 --//
	static Int32 KeyCompare( array<Byte>^ inKey, array<Byte>^ inData,						//キー比較
						KeyInfo^ inKeyInfo, Int32 inIdxNum, KeyCmpOpt_EN Opt );

	static String^ ArrayToString( array<Byte>^ inData, Int32 inOffset, Int32 inLen );		//String(Chars)に変換
	static SByte ArrayToSByte( array<Byte>^ inData, Int32 inOffset );						//SByteに変換
	static Int16 ArrayToInt16( array<Byte>^ inData, Int32 inOffset );						//Int16に変換
	static UInt16 ArrayToUInt16( array<Byte>^ inData, Int32 inOffset );					//UInt16に変換
	static Int32 ArrayToInt32( array<Byte>^ inData, Int32 inOffset );						//Int32に変換
	static UInt32 ArrayToUInt32( array<Byte>^ inData, Int32 inOffset );					//UInt32に変換
	static Int64 ArrayToInt64( array<Byte>^ inData, Int32 inOffset );						//Int64に変換
	static UInt64 ArrayToUInt64( array<Byte>^ inData, Int32 inOffset );					//UInt64に変換

	static array<Byte>^ StringToArray( String^ inSrc, array<Byte>^ inDest,					//String(Chars)から変換
								Int32 inOffset, Int32 inLen );						
	static array<Byte>^ SByteToArray( SByte inSrc, array<Byte>^ inDest,					//SByteから変換
								Int32 inOffset );
	static array<Byte>^ Int16ToArray( Int16 inSrc, array<Byte>^ inDest,					//Int16から変換
								Int32 inOffset );
	static array<Byte>^ UInt16ToArray( UInt16 inSrc, array<Byte>^ inDest,					//UInt16から変換
								Int32 inOffset );
	static array<Byte>^ Int32ToArray( Int32 inSrc, array<Byte>^ inDest,						//Int32から変換
								Int32 inOffset );
	static array<Byte>^ UInt32ToArray( UInt32 inSrc, array<Byte>^ inDest,					//UInt32から変換
								Int32 inOffset );
	static array<Byte>^ Int64ToArray( Int64 inSrc, array<Byte>^ inDest,						//Int64から変換
								Int32 inOffset );
	static array<Byte>^ UInt64ToArray( UInt64 inSrc, array<Byte>^ inDest,					//UInt64から変換
								Int32 inOffset );

	static String^ KeyStringConv( String^ inStr );											//比較用の文字列に返還

	};
}