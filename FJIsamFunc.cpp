//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　共通関数　ソース
//	Version 1.2		Create Date 2006/03/01 新規作成
//  Version 1.2.1	Change Date 2011/07/08 FJIFunc::KeyStringConv 修正（'や空白を無視する)
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Functions
//		KeyCompare	:キー比較
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsamFunc.h"			//共通関数
#include "FJIsamException.h"	//例外クラス

using namespace FJIsam;

//------------------------------------------------------------------------------
//***** キー比較
Int32 FJIsam::FJIFuncs::KeyCompare( array<Byte>^ inKey, array<Byte>^ inData,
						  KeyInfo^ inKeyInfo, Int32 inIdxNum, KeyCmpOpt_EN Opt )
{
	Int32 chkKeyLen;			//キー長
	Int32 chkDataPos;			//データ側のキー位置
	Int32 chkKeyPos;			//キー位置
	Int32 chkVal;				//キー比較結果

	//-- 引数チェック --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー情報が未設定です。");
		tmpFJIsamException->ErrorCode = 5001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( Opt != KeyCmpOpt_EN::Srt && inKeyInfo->getSrtKeyLen() != 0  && inKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーデータが未設定です。");
		tmpFJIsamException->ErrorCode = 5001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( Opt != KeyCmpOpt_EN::Srt && inKeyInfo->getSrtKeyLen() != 0  && inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}
	if( inIdxNum < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 5001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- キーをチェック --//
	if( Opt == KeyCmpOpt_EN::Key || Opt == KeyCmpOpt_EN::All )
	{
		chkKeyLen = inKeyInfo->getKeyLen();				//キー長
		//キー位置
		switch( inIdxNum ) 
		{
		case 0 :									//データレコード同士の比較
			chkKeyPos = inKeyInfo->getKeyPos();
			chkDataPos = inKeyInfo->getKeyPos();
			break;
		case 1 :									//キーとデータレコードの比較
			chkKeyPos = 0;
			chkDataPos = inKeyInfo->getKeyPos();
			break;
		default :									//キーとサブインデックスの比較
			chkKeyPos = 0;
			chkDataPos = 0;
		}

		if( inKey->Length < chkKeyLen + chkKeyPos  )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー長が不足しています。");
			tmpFJIsamException->ErrorCode = 5001;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;		
		}
		if( inData->Length < chkKeyLen + chkDataPos )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データ長が不足しています。");
			tmpFJIsamException->ErrorCode = 5001;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;		
		}

		//-- データ比較 --//
		try
		{
			switch( inKeyInfo->getKeyType() )
			{
			//UNICODE：String^
			case keyType_EN::ISKEY_UNICODE :
				String^ tmpStrKey;
				String^ tmpStrDat;
		
				tmpStrKey =	KeyStringConv( ArrayToString( inKey, chkKeyPos, chkKeyLen ) );
				tmpStrDat = KeyStringConv( ArrayToString( inData, chkDataPos, chkKeyLen ) );
				chkVal = String::Compare( tmpStrKey, tmpStrDat, StringComparison::Ordinal );
				break;	

			//Wide Charactor：array<Byte>^(SJIS等)
			case keyType_EN::ISKEY_CHAR :			
				chkVal = 0;
				for( Int32 i = 0; i < chkKeyLen; i++ )
				{
					Byte tmpChrKey;		//比較キー文字
					Byte tmpChrDat;		//比較データ文字

					//比較キー文字の小文字アルファベットを大文字にする
					tmpChrKey = inKey[chkKeyPos + i];
					if( tmpChrKey >= 'a' && tmpChrKey <= 'z' )
					{
						tmpChrKey -= ( 'a' - 'A' ); 
					}
	
					//比較データ文字の小文字アルファベットを大文字にする
					tmpChrDat = inData[chkDataPos + i];
					if( tmpChrDat >= 'a' && tmpChrDat <= 'z' )
					{
						tmpChrDat -= ( 'a' - 'A' ); 
					}

					chkVal = tmpChrKey.CompareTo( tmpChrDat );
					//データが異なれば完了
					if( chkVal != 0 )		break;
				}
				break;


			//バイトデータ：array<Byte>^
			case keyType_EN::ISKEY_BYTE :
				chkVal = 0;
				for( Int32 i = 0; i < chkKeyLen; i++ )
				{
					Byte tmpBKey;
					Byte tmpBDat;
					tmpBKey = inKey[chkKeyPos + i];
					tmpBDat = inData[chkDataPos + i];
					chkVal = tmpBKey.CompareTo( tmpBDat );
					//データが異なれば完了
					if( chkVal != 0 )	break;
				}
				break;

			//符号つき数値バイト
			case keyType_EN::ISKEY_SNUM :				//符号付数値：Int8～Int64
				switch( chkKeyLen )
				{
				//Byte
				case 1:
					SByte tmpSBKey;
					SByte tmpSBDat;
	
					tmpSBKey= ArrayToSByte( inKey, chkKeyPos );
					tmpSBDat = ArrayToSByte( inData, chkDataPos );
					if( tmpSBKey == tmpSBDat) chkVal = 0;
					if( tmpSBKey > tmpSBDat) chkVal =  1;
					if( tmpSBKey < tmpSBDat) chkVal = -1;
					break;

				//Int16
				case 2:
					Int16 tmpS16Key;
					Int16 tmpS16Dat;
	
					tmpS16Key= ArrayToInt16( inKey, chkKeyPos );
					tmpS16Dat = ArrayToInt16( inData, chkDataPos );
					if( tmpS16Key == tmpS16Dat) chkVal = 0;
					if( tmpS16Key > tmpS16Dat) chkVal = 1;
					if( tmpS16Key < tmpS16Dat) chkVal = -1;
					break;

				//Int32
				case 4:
					Int32 tmpS32Key;
					Int32 tmpS32Dat;
	
					tmpS32Key = ArrayToInt32( inKey, chkKeyPos );
					tmpS32Dat = ArrayToInt32( inData, chkDataPos );
					chkVal = tmpS32Key - tmpS32Dat;
					break;

				//Int64
				case 8:
					Int64 tmpS64Key;
					Int64 tmpS64Dat;

					tmpS64Key = ArrayToInt64( inKey, chkKeyPos );
					tmpS64Dat = ArrayToInt64( inData, chkDataPos );
					if( tmpS64Key == tmpS64Dat) chkVal = 0;
					if( tmpS64Key > tmpS64Dat) chkVal = 1;
					if( tmpS64Key < tmpS64Dat) chkVal = -1;
					break;

				//キー長が 1,2,4,8以外
				default :
					FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"符号つき数値のキー長が不正です。");
					tmpFJIsamException->ErrorCode = 5001;
					tmpFJIsamException->ErrorCode2 = 7;
					throw tmpFJIsamException;		
				}
				break;

			case keyType_EN::ISKEY_USNUM :				//符号なし数値：UInt8～UInt64
				switch( chkKeyLen )
				{
				//Byte
				case 1:
					if( inKey[chkKeyPos] == inData[chkDataPos] ) chkVal = 0;
					if( inKey[chkKeyPos] > inData[chkDataPos] ) chkVal = 1;
					if( inKey[chkKeyPos] < inData[chkDataPos] ) chkVal = -1;
					break;

				//UInt16
				case 2:
					UInt16 tmpU16Key;
					UInt16 tmpU16Dat;
	
					tmpU16Key = ArrayToUInt16( inKey, chkKeyPos );
					tmpU16Dat = ArrayToUInt16( inData, chkDataPos );
					if( tmpU16Key == tmpU16Dat) chkVal = 0;
					if( tmpU16Key > tmpU16Dat) chkVal = 1;	
					if( tmpU16Key < tmpU16Dat) chkVal = -1;
					break;
	
				//UInt32
				case 4:
					UInt32 tmpU32Key;
					UInt32 tmpU32Dat;
					
					tmpU32Key = ArrayToUInt32( inKey, chkKeyPos );
					tmpU32Dat = ArrayToUInt32( inData, chkDataPos );
					if( tmpU32Key == tmpU32Dat) chkVal = 0;
					if( tmpU32Key > tmpU32Dat) chkVal = 1;
					if( tmpU32Key < tmpU32Dat) chkVal = -1;
					break;

				//UInt64
				case 8:
					UInt64 tmpU64Key;
					UInt64 tmpU64Dat;
	
					tmpU64Key = ArrayToUInt64( inKey, chkKeyPos );
					tmpU64Dat = ArrayToUInt64( inData, chkDataPos );
					if( tmpU64Key == tmpU64Dat) chkVal = 0;
					if( tmpU64Key > tmpU64Dat) chkVal = 1;
					if( tmpU64Key < tmpU64Dat) chkVal = -1;
					break;
				//キー長が 1,2,4,8以外
				default :
					FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"符号なし数値のキー長が不正です。");
					tmpFJIsamException->ErrorCode = 5001;
					tmpFJIsamException->ErrorCode2 = 8;
					throw tmpFJIsamException;					
				}
			//データタイプがいずれにも該当しない
			default :
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データタイプが不正です。");
				tmpFJIsamException->ErrorCode = 5001;
				tmpFJIsamException->ErrorCode2 = 9;
				throw tmpFJIsamException;		
			}
		//キー値比較中にエラー発生
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが不正のため比較できません。", exp);
			tmpFJIsamException->ErrorCode = 5001;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;		
		}
		//キーが0以外(順序が出ている)なら終了
		if( chkVal != 0 ) 
		{
			if( inKeyInfo->getKeyOrder() == 'A' )
			{
				return chkVal;
			}else{
				return -chkVal;
			}
		}
		//比較対象がキーのみなら終了
		if( Opt == KeyCmpOpt_EN::Key ) return chkVal;
	}


	//-- ソートキーをチェック --//
	//ソートキー長
	chkKeyLen = inKeyInfo->getSrtKeyLen();				

	//ソートキー長が0 (ソートキーがない)なら終了
	if( chkKeyLen == 0 ) return 1;

	//キー位置
	switch( inIdxNum )
	{
	case 0 :										//データレコード同士の比較
		chkKeyPos = inKeyInfo->getSrtKeyPos();
		chkDataPos = inKeyInfo->getSrtKeyPos();
		break;
	case 1 :										//キーとデータレコードの比較
		if( Opt == KeyCmpOpt_EN::Srt )
		{
			chkKeyPos = 0;								//ソートキーのみの比較時
		}else{
			chkKeyPos = inKeyInfo->getKeyLen();			//キー＆ソートキーを比較
		}
		chkDataPos = inKeyInfo->getSrtKeyPos();
		break;
	default :										//キーとサブインデックスの比較
		if( Opt == KeyCmpOpt_EN::Srt )
		{
			chkKeyPos = 0;							//ソートキーのみの比較時
			chkDataPos = 0;
		}else{
			chkKeyPos = inKeyInfo->getKeyLen();		//キー＆ソートキーを比較
			chkDataPos = inKeyInfo->getKeyLen();		
		}
	}

	if( inKey->Length <	 chkKeyLen + chkKeyPos )
	{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー長が不足しています。");
			tmpFJIsamException->ErrorCode = 5001;
			tmpFJIsamException->ErrorCode2 = 11;
			throw tmpFJIsamException;		
		}
		if( inData->Length < chkKeyLen + chkDataPos )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データ長が不足しています。");
			tmpFJIsamException->ErrorCode = 5001;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;		
		}

		//-- データ比較 --//
		try
		{
			switch( inKeyInfo->getSrtKeyType() )
			{
			//UNICODE：String^
			case keyType_EN::ISKEY_UNICODE :
				String^ tmpStrKey;
				String^ tmpStrDat;
		
				tmpStrKey =	KeyStringConv( ArrayToString( inKey, chkKeyPos, chkKeyLen ) );
				tmpStrDat = KeyStringConv( ArrayToString( inData, chkDataPos, chkKeyLen ) );
				chkVal = String::Compare( tmpStrKey, tmpStrDat, true );
				break;	

			//Wide Charactor：array<Byte>^(SJIS等)
			case keyType_EN::ISKEY_CHAR :			
				chkVal = 0;
				for( Int32 i = 0; i < chkKeyLen; i++ )
				{
					Byte tmpChrKey;		//比較キー文字
					Byte tmpChrDat;		//比較データ文字

					//比較キー文字の小文字アルファベットを大文字にする
					tmpChrKey = inKey[chkKeyPos + i];
						if( tmpChrKey >= 'a' && tmpChrKey <= 'z' )
					{
						tmpChrKey -= ( 'a' - 'A' ); 
					}
	
					//比較データ文字の小文字アルファベットを大文字にする
					tmpChrDat = inData[chkDataPos + i];
					if( tmpChrDat >= 'a' && tmpChrDat <= 'z' )
					{
						tmpChrDat -= ( 'a' - 'A' ); 
					}

					chkVal = tmpChrKey.CompareTo( tmpChrDat );
					//データが異なれば完了
					if( chkVal != 0 )		break;
				}
				break;


			//バイトデータ：array<Byte>^
			case keyType_EN::ISKEY_BYTE :
				chkVal = 0;
				for( Int32 i = 0; i < chkKeyLen; i++ )
				{
					Byte tmpBKey;
					Byte tmpBDat;
					tmpBKey = inKey[chkKeyPos + i];
					tmpBDat = inData[chkDataPos + i];
					chkVal = tmpBKey.CompareTo( tmpBDat );
					break;
				}
				break;

			//符号つき数値バイト
			case keyType_EN::ISKEY_SNUM :				//符号付数値：Int8～Int64
				switch( chkKeyLen )
				{
				//Byte
				case 1:
					SByte tmpSBKey;
					SByte tmpSBDat;
	
					tmpSBKey= ArrayToSByte( inKey, chkKeyPos );
					tmpSBDat = ArrayToSByte( inData, chkDataPos );
					if( tmpSBKey == tmpSBDat) chkVal = 0;
					if( tmpSBKey > tmpSBDat) chkVal =  1;
					if( tmpSBKey < tmpSBDat) chkVal = -1;
					break;

				//Int16
				case 2:
					Int16 tmpS16Key;
					Int16 tmpS16Dat;
	
					tmpS16Key= ArrayToInt16( inKey, chkKeyPos );
					tmpS16Dat = ArrayToInt16( inData, chkDataPos );
					if( tmpS16Key == tmpS16Dat) chkVal = 0;
					if( tmpS16Key > tmpS16Dat) chkVal = 1;
					if( tmpS16Key < tmpS16Dat) chkVal = -1;
					break;

				//Int32
				case 4:
					Int32 tmpS32Key;
					Int32 tmpS32Dat;
	
					tmpS32Key = ArrayToInt32( inKey, chkKeyPos );
					tmpS32Dat = ArrayToInt32( inData, chkDataPos );
					chkVal = tmpS32Key - tmpS32Dat;
					break;

				//Int64
				case 8:
					Int64 tmpS64Key;
					Int64 tmpS64Dat;

					tmpS64Key = ArrayToInt64( inKey, chkKeyPos );
					tmpS64Dat = ArrayToInt64( inData, chkDataPos );
					if( tmpS64Key == tmpS64Dat) chkVal = 0;
					if( tmpS64Key > tmpS64Dat) chkVal = 1;
					if( tmpS64Key < tmpS64Dat) chkVal = -1;
					break;

				//キー長が 1,2,4,8以外
				default :
					FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"符号つき数値のキー長が不正です。");
					tmpFJIsamException->ErrorCode = 5001;
					tmpFJIsamException->ErrorCode2 = 13;
					throw tmpFJIsamException;		
				}
				break;

			case keyType_EN::ISKEY_USNUM :				//符号なし数値：UInt8～UInt64
				switch( chkKeyLen )
				{
				//Byte
				case 1:
					if( inKey[chkKeyPos] == inData[chkDataPos] ) chkVal = 0;
					if( inKey[chkKeyPos] > inData[chkDataPos] ) chkVal = 1;
					if( inKey[chkKeyPos] < inData[chkDataPos] ) chkVal = -1;
					break;

				//UInt16
				case 2:
					UInt16 tmpU16Key;
					UInt16 tmpU16Dat;
	
					tmpU16Key = ArrayToUInt16( inKey, chkKeyPos );
					tmpU16Dat = ArrayToUInt16( inData, chkDataPos );
					if( tmpU16Key == tmpU16Dat) chkVal = 0;
					if( tmpU16Key > tmpU16Dat) chkVal = 1;	
					if( tmpU16Key < tmpU16Dat) chkVal = -1;
					break;
	
				//UInt32
				case 4:
					UInt32 tmpU32Key;
					UInt32 tmpU32Dat;
					
					tmpU32Key = ArrayToUInt32( inKey, chkKeyPos );
					tmpU32Dat = ArrayToUInt32( inData, chkDataPos );
					if( tmpU32Key == tmpU32Dat) chkVal = 0;
					if( tmpU32Key > tmpU32Dat) chkVal = 1;
					if( tmpU32Key < tmpU32Dat) chkVal = -1;
					break;

				//UInt64
				case 8:
					UInt64 tmpU64Key;
					UInt64 tmpU64Dat;
	
					tmpU64Key = ArrayToUInt64( inKey, chkKeyPos );
					tmpU64Dat = ArrayToUInt64( inData, chkDataPos );
					if( tmpU64Key == tmpU64Dat) chkVal = 0;
					if( tmpU64Key > tmpU64Dat) chkVal = 1;
					if( tmpU64Key < tmpU64Dat) chkVal = -1;
					break;
				//キー長が 1,2,4,8以外
				default :
					FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"符号なし数値のキー長が不正です。");
					tmpFJIsamException->ErrorCode = 5001;
					tmpFJIsamException->ErrorCode2 = 14;
					throw tmpFJIsamException;					
				}
			//データタイプがいずれにも該当しない
			default :
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データタイプが不正です。");
				tmpFJIsamException->ErrorCode = 5001;
				tmpFJIsamException->ErrorCode2 = 15;
				throw tmpFJIsamException;		
		}
	//キー値比較中にエラー発生
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが不正のため比較できません。", exp);
		tmpFJIsamException->ErrorCode = 5001;
		tmpFJIsamException->ErrorCode2 = 16;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	if( inKeyInfo->getSrtKeyOrder() == 'A' )
	{
		return chkVal;
	}else{
		return -chkVal;
	}


}

//------------------------------------------------------------------------------
//***** String(Chars)に変換
String^ FJIsam::FJIFuncs::ArrayToString( array<Byte>^ inData, Int32 inOffset, Int32 inLen )
{
	array<Char>^ tmpDat;
	String^ rtDat;

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inLen <= 1 || (inLen %2 != 0) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データ長が不正です。");
		tmpFJIsamException->ErrorCode = 5002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}
	if( inData->Length < inOffset + inLen )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5002;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	tmpDat = gcnew array<Char>(inLen /2 +1);
	Buffer::BlockCopy( inData, inOffset, tmpDat , 0, inLen);
	tmpDat[inLen /2] = L'\x0000';
	rtDat = gcnew String( tmpDat );
	return rtDat;
}

//***** SByteに変換
SByte FJIsam::FJIFuncs::ArrayToSByte( array<Byte>^ inData, Int32 inOffset )
{
	array<SByte>^ rtDat = { 0 };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 1)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5003;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 1 );

	return rtDat[0];
}
						
//***** Int16に変換
Int16 FJIsam::FJIFuncs::ArrayToInt16( array<Byte>^ inData, Int32 inOffset )
{
	array<Int16>^ rtDat = { 0 };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5004;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 2)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5004;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 2 );

	return rtDat[0];
}
						
//***** UInt16に変換
UInt16 FJIsam::FJIFuncs::ArrayToUInt16( array<Byte>^ inData, Int32 inOffset )
{
	array<UInt16>^ rtDat = { 0 };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5005;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 2)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5005;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 2 );

	return rtDat[0];
}
					
//***** Int32に変換
Int32 FJIsam::FJIFuncs::ArrayToInt32( array<Byte>^ inData, Int32 inOffset )
{
	array<Int32>^ rtDat = { 0 };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5006;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 4)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5006;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 4 );

	return rtDat[0];
}
						
//***** UInt32に変換
UInt32 FJIsam::FJIFuncs::ArrayToUInt32( array<Byte>^ inData, Int32 inOffset )
{
	array<UInt32>^ rtDat = { 0 };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5007;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 4)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5007;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 4 );

	return rtDat[0];
}
					
//***** Int64に変換
Int64 FJIsam::FJIFuncs::ArrayToInt64( array<Byte>^ inData, Int32 inOffset )
{
	array<Int64>^ rtDat = { 0L };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5008;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 8)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5008;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 8 );

	return rtDat[0];
}
						
//***** UInt64に変換
UInt64 FJIsam::FJIFuncs::ArrayToUInt64( array<Byte>^ inData, Int32 inOffset )
{
	array<UInt64>^ rtDat = { 0L };

	//-- 引数チェック --//
	if( inData == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5009;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inData->Length < inOffset + 8)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
		tmpFJIsamException->ErrorCode = 5009;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- データコピー --//
	Buffer::BlockCopy( inData, inOffset, rtDat, 0, 8 );

	return rtDat[0];
}

//------------------------------------------------------------------------------
//***** String(Chars)から変換
array<Byte>^ FJIsam::FJIFuncs::StringToArray( String^ inSrc, array<Byte>^ inDest,			
								   	Int32 inOffset, Int32 inLen )
{
	array<Byte>^	rtDat;			//返却データ
	Int32			tmpLen;			//一時データ長

	//-- 引数チェック --//
	if( inSrc == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"コピー元データが未設定です。");
		tmpFJIsamException->ErrorCode = 5010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5010;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inLen <= 1 || (inLen % 2 != 0) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データ長が不正です。");
		tmpFJIsamException->ErrorCode = 5010;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}


	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + inLen );
	}else{
		if( inDest->Length < inOffset + inLen )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5010;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}

	
	//-- データコピー --//
	try
	{
		if( inSrc->Length *2  >= inLen )
		{
			Buffer::BlockCopy( inSrc->ToCharArray(), 0, rtDat, inOffset, inLen );
		}else{
			Buffer::BlockCopy( inSrc->ToCharArray(), 0, rtDat, inOffset, inSrc->Length * 2 );
			tmpLen = inLen - ( inSrc->Length * 2 );
	
			for( Int32 i = 0; i < tmpLen; i++ )
			{
				Buffer::SetByte( rtDat, inOffset + inSrc->Length * 2 + i, 0 ); 
			}
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 5010;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//***** SByteから変換
array<Byte>^ FJIsam::FJIFuncs::SByteToArray( SByte inSrc, array<Byte>^ inDest,				
								Int32 inOffset )
{
	array<SByte>^	tmpDat;			//返却データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5011;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(SByte) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(SByte) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5011;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}
	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<SByte>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(SByte) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5011;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//***** Int16から変換								
array<Byte>^ FJIsam::FJIFuncs::Int16ToArray( Int16 inSrc, array<Byte>^ inDest,						
								  Int32 inOffset )
{
	array<Int16>^ tmpDat;			//コピー用データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5012;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(Int16) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(Int16) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5012;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}	
		rtDat = inDest;
	}
	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<Int16>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(Int16) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5012;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}
//***** UInt16から変換
array<Byte>^ FJIsam::FJIFuncs::UInt16ToArray( UInt16 inSrc, array<Byte>^ inDest,					
								Int32 inOffset )
{
	array<UInt16>^ tmpDat;			//コピー用データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5013;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(UInt16) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(UInt16) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5013;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}
	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<UInt16>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(UInt16) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5013;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//***** Int32から変換
array<Byte>^ FJIsam::FJIFuncs::Int32ToArray( Int32 inSrc, array<Byte>^ inDest,
								Int32 inOffset )
{
	array<Int32>^ tmpDat;			//コピー用データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5014;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(Int32) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(Int32) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5014;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}
	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<Int32>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(Int32) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5014;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//***** UInt32から変換
array<Byte>^ FJIsam::FJIFuncs::UInt32ToArray( UInt32 inSrc, array<Byte>^ inDest,					
								Int32 inOffset )
{
	array<UInt32>^ tmpDat;			//コピー用データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5015;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(UInt32) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(UInt32) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5015;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}

	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<UInt32>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(UInt32) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5015;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//***** Int64から変換
array<Byte>^ FJIsam::FJIFuncs::Int64ToArray( Int64 inSrc, array<Byte>^ inDest,						
								  Int32 inOffset )
{
	array<Int64>^ tmpDat;			//コピー用データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5016;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(Int64) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(Int64) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5016;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}

	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<Int64>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(Int64) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5016;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//***** UInt64から変換
array<Byte>^ FJIsam::FJIFuncs::UInt64ToArray( UInt64 inSrc, array<Byte>^ inDest,					
								Int32 inOffset )
{
	array<UInt64>^ tmpDat;			//コピー用データ
	array<Byte>^	rtDat;			//返却データ

	//-- 引数チェック --//
	if( inOffset < 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"オフセット値が不正です。");
		tmpFJIsamException->ErrorCode = 5017;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	if( inDest == nullptr )
	{
		rtDat = gcnew array<Byte>( inOffset + sizeof(UInt64) );
	}else{
		if( inDest->Length < inOffset + (Int32)sizeof(UInt64) )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データが短すぎます。");
			tmpFJIsamException->ErrorCode = 5017;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;		
		}
		rtDat = inDest;
	}

	
	//-- データコピー --//
	try
	{
		tmpDat = gcnew array<UInt64>(1);
		tmpDat[0] = inSrc;
		Buffer::BlockCopy( tmpDat, 0, rtDat, inOffset, sizeof(UInt64) );
		delete tmpDat;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" データコピーに失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 5017;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return rtDat;
}

//------------------------------------------------------------------------------
//*****比較用の文字列に変換
String^ FJIsam::FJIFuncs::KeyStringConv( String^ inStr )
{
	array<Char>^	tmpDat;				//入力文字データ
	array<Char>^	rtDat;				//返却文字データ
	Int32			rtDatCnt;			//返却文字位置
	String^			rtStr;				//返却文字列

	//-- 引数チェック --//
	if( inStr == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。");
		tmpFJIsamException->ErrorCode = 5018;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	//-- 初期化 --//
	//-- 初期化 --//
	tmpDat = inStr->ToUpper()->ToCharArray();		//入力文字を配列か

	rtDatCnt = 0;
	rtDat = gcnew array<Char>( tmpDat->Length );	//返却文字の領域確保
	
	tmpDat = inStr->ToUpper()->ToCharArray();
	//-- アクセントつき文字を変換 --//
	for( Int32 i = 0; i < tmpDat->Length; i++ )
	{
		switch( tmpDat[i] )
		{
		case L'\u00c0' :		//'À' :
		case L'\u00c1' :		//'Á' :
		case L'\u00c2' :		//'Â' :
		case L'\u00c3' :		//'Ã' :
		case L'\u00c4' :		//'Ä' :
		case L'\u00c5' :		//'Å' :
		case L'\u00c6' :		//'Æ' :
			rtDat[rtDatCnt] = L'A';		
			break;
		case L'\u00c7' :		//'Ç' :
			rtDat[rtDatCnt] = L'C';		
			break;
		case L'\u00c9' :		//'É' :
		case L'\u00c8' :		//'È' :
		case L'\u00ca' :		//'Ê' :
		case L'\u00cb' :		//'Ë' :
			rtDat[rtDatCnt] = L'E';		
			break;
		case L'\u00cc' :		//'Ì' :
		case L'\u00cd' :		//'Í' :
		case L'\u00ce' :		//'Î' :
		case L'\u00cf' :		//'Ï' :
			rtDat[rtDatCnt] = L'I';		
			break;
		case L'\u00d0' :		//'Ð' :
		case L'\u00d1' :		//'Ñ' :
			rtDat[i] = L'N';		
			break;
		case L'\u00d3' :		//'Ó' :
		case L'\u00d2' :		//'Ò' :
		case L'\u00d5' :		//'Õ' :
		case L'\u00d4' :		//'Ô' :
		case L'\u00d6' :		//'Ö' :
		case L'\u00d8' :		//'Ö' :
			rtDat[rtDatCnt] = L'O';		
			break;
		case L'\u00d9' :		//'Ù' :
		case L'\u00da' :		//'Ú' :
		case L'\u00db' :		//'Û' :
		case L'\u00dc' :		//'Ü' :
			rtDat[rtDatCnt] = L'U';		
			break;
		case L'\u00dd' :		//'Ý' :
			tmpDat[rtDatCnt] = L'Y';		
			break;
		case L'\u00de' :		//'Þ' :
			rtDat[rtDatCnt] = L'T';		
			break;
		case L'\u00df' :		//'ß' :
			rtDat[rtDatCnt] = L'S';		
			break;
		case L'\'' :			//''' : シングルクォーテーション
		case L' ' :				//' ' : 半角スペース
		case L'\u3000' :		//'　': 全角スペース
			rtDatCnt--;					//返還文字からは削除
			break;
		default :				//該当なしならそのままコピー
			rtDat[rtDatCnt] = tmpDat[i];
		}
		//返却文字加算
		rtDatCnt++;
	}

	if( rtDatCnt < tmpDat->Length )
	{
		rtDat[rtDatCnt] = L'\0';
	}

	//-- 正常終了 --//
	rtStr = gcnew String( rtDat );
	delete tmpDat;
	delete rtDat;

	return rtStr;
}	
