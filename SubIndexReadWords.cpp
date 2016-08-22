//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　サブインデックス　ソース
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:サブインデックス
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndex.h"			//サブインデックスヘッダ
#include "FJIsamFunc.h"			//Isam用関数
#include "FJIsamException.h"	//例外クラス

using namespace FJIsam;

//------------------------------------------------------------------------------
//	class	:SubIndex	:サブインデックス(読込関数)
//------------------------------------------------------------------------------
//***** レコード読込
List<SubIndexRecord^>^ FJIsam::SubIndex::ReadRecordWords( String^ inKey )
{
	List<SubIndexRecord^>^	rtList;			//取得レコードリスト
	SubIndexRecord^		baseRec;			//取得レコードベース
	String^				tmpKeyStr;			//検索キー

	SubIndexRecord^		tmpRec;				//読込サブインデックスレコード
	Int64				chkRecPos;			//チェック用レコード位置

	Int32				chkKeyFlg;			//キーチェック結果

	//-- 引数チェック --//
	if( inKey == nullptr )		//キーなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが未指定です。");
		tmpFJIsamException->ErrorCode = 3701;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	if( getKeyType() != keyType_EN::ISKEY_UNICODE )		//キータイプチェック
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キータイプがUNICODEではありません。");
		tmpFJIsamException->ErrorCode = 3701;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( inKey->Length > getKeyLen() )					//キー長チェック
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー長が不正です。");
		tmpFJIsamException->ErrorCode = 3701;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- レコードが0件なら nullptr を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- サブインデックス検索 --//
	chkRecPos = RecRootPos;
	tmpKeyStr = FJIFuncs::KeyStringConv( inKey );

	//キーが一致するまで検索
	while( true )
	{
		//レコードを取得してキーを比較
		try
		{
			baseRec = getRecord( chkRecPos );
			if(baseRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコードが存在しません。" );
				tmpFJIsamException->ErrorCode = 3701;
				tmpFJIsamException->ErrorCode2 = 4;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"レコード読み込みに失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
		//キー比較
		try
		{
			chkKeyFlg = String::Compare( tmpKeyStr , 0,
					 FJIFuncs::KeyStringConv(
						FJIFuncs::ArrayToString( baseRec->getKeyData(nullptr), 0, getKeyLen() ) ), 0, 
					 tmpKeyStr->Length, StringComparison::Ordinal );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				L"レコードの内容が不正です。", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}

		//キーが一致(正常終了)
		if( chkKeyFlg == 0 )
		{
			break;

		//検索するキーの方が小さい(次は小の子枝を検索）
		}else if( chkKeyFlg < 0 )
		{
			chkRecPos = baseRec->getLtChildRecPos();
		//検索するキーの方が大きい(次は大の子枝を検索）
		}else {
			chkRecPos = baseRec->getGtChildRecPos();
		}
		delete baseRec;		//不要レコード削除

		//-- レコードがなくなれば終了 --//
		if( chkRecPos == 0 )
		{
			return nullptr;
		}
	}

	//-- 前後のレコード取得 --//
	rtList = gcnew List<SubIndexRecord^>();
	rtList->Add( baseRec );
	tmpRec = baseRec;

	//前レコード
	while( true )
	{
		if( tmpRec->getPrevRecPos() == 0L )
		{
			break;
		}

		//レコードを取得してキーを比較
		try
		{
			tmpRec = PrevRecord( tmpRec );
			if(tmpRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコードが存在しません。" );
				tmpFJIsamException->ErrorCode = 3701;
				tmpFJIsamException->ErrorCode2 = 7;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"レコード読み込みに失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}

		//キー比較
		try
		{
			chkKeyFlg = String::Compare( tmpKeyStr, 0, 
				FJIFuncs::KeyStringConv( 
					FJIFuncs::ArrayToString( tmpRec->getKeyData(nullptr), 0, getKeyLen() ) ), 0,
					tmpKeyStr->Length, StringComparison::Ordinal );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
			L"レコードの内容が不正です。", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

		//レコード追加
		if( chkKeyFlg == 0 )
		{
			rtList->Insert( 0, tmpRec );
		}else{
			break;
		}
	}

	//後レコード
	tmpRec = baseRec;
	while( true )
	{
		if( tmpRec->getNextRecPos() == 0L )
		{
			break;
		}

		//レコードを取得してキーを比較
		try
		{
			tmpRec = NextRecord( tmpRec );
			if(tmpRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコードが存在しません。" );
				tmpFJIsamException->ErrorCode = 3701;
				tmpFJIsamException->ErrorCode2 = 10;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"レコード読み込みに失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 11;
			throw tmpFJIsamException;
		}

		//キー比較
		try
		{
			chkKeyFlg = String::Compare( tmpKeyStr, 0, 
				FJIFuncs::KeyStringConv( 
					FJIFuncs::ArrayToString( tmpRec->getKeyData(nullptr), 0, getKeyLen() ) ), 0, 
					tmpKeyStr->Length, StringComparison::Ordinal );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				L"レコードの内容が不正です。", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;
		}

		//レコード追加
		if( chkKeyFlg == 0 )
		{
			rtList->Add( tmpRec );
		}else{
			break;
		}
	}

	//--正常終了 --//

	return rtList;

}