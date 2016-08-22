//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ヘッダ
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		FJIsam : FJIsam 読込処理関係ソース
//------------------------------------------------------------------------------
#pragma once 

#include "stdafx.h"
#include "FJIsam.h"
#include "FJIsamFunc.h"
#include "FJIsamException.h"

using namespace FJIsam;
using namespace System;
using namespace System::IO;
//------------------------------------------------------------------------------
//	class	:FJIsam	:FJIsam(読込関数)
//------------------------------------------------------------------------------
//***** レコード読込
List<DataRecord^>^ FJIsam::FIsam::ReadRecordWords( String^ inKey, Int32 inIdxNum )
{
	List<DataRecord^>^	rtList;				//取得レコードリスト
	DataRecord^			baseRec;			//取得レコードベース
	String^				tmpKeyStr;			//検索キー
	Int32				tmpKeyLen;			//検索キー長
	Int32				tmpKeyLenT;			//検索キー長

	DataRecord^			tmpRec;				//取得レコード

	Int64				chkRecPos;			//チェック用レコード位置
	SubIndex^			chkSubIdx;			//チェック用サブインデックス

	Int32				chkKeyFlg;			//キーチェック結果


	//-- 引数チェック --//
	if( inKey == nullptr )		//キーなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが未指定です。");
		tmpFJIsamException->ErrorCode = 1701;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inIdxNum < 1 ||			//インデックス異常
		inIdxNum > IdxCnt )			
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスの指定が不正です。");
		tmpFJIsamException->ErrorCode = 1701;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( getKeyType( inIdxNum ) != keyType_EN::ISKEY_UNICODE )		//キータイプチェック
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キータイプがUNICODEではありません。");
		tmpFJIsamException->ErrorCode = 1701;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;

	}

	//-- 検索文字がキー長より長ければ nullptr を返す --//
	tmpKeyLen = inKey->Length;										//キー長取得
	if( tmpKeyLen > getKeyLen( inIdxNum ) )							//キー長チェック
	{
		return nullptr;
	}

	//-- レコードが0件なら nullptr を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}


	//-- メインインデックス検索 --//
	if( inIdxNum == 1 )
	{

		chkRecPos = RecRootPos;
		tmpKeyStr = FJIFuncs::KeyStringConv( inKey );
		tmpKeyLenT = tmpKeyStr->Length;

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
					tmpFJIsamException->ErrorCode = 1701;
					tmpFJIsamException->ErrorCode2 = 5;
					throw tmpFJIsamException;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコード読み込みに失敗しました。", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 6;
				throw tmpFJIsamException;
			}
			//キー比較
			try
			{
				chkKeyFlg = String::Compare( tmpKeyStr, 0, 
					FJIFuncs::KeyStringConv( 
						FJIFuncs::ArrayToString( baseRec->getData(nullptr), getKeyPos(1), getKeyLen(1) ) ), 0,
						tmpKeyLenT, StringComparison::Ordinal );

			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"レコードの内容が不正です。", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 7;
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
		rtList = gcnew List<DataRecord^>();
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
					tmpFJIsamException->ErrorCode = 1701;
					tmpFJIsamException->ErrorCode2 = 8;
					throw tmpFJIsamException;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコード読み込みに失敗しました。", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 9;
				throw tmpFJIsamException;
			}

			//キー比較
			try
			{
				chkKeyFlg = String::Compare( tmpKeyStr, 0, 
					FJIFuncs::KeyStringConv( 
						FJIFuncs::ArrayToString( tmpRec->getData(nullptr), getKeyPos(1), getKeyLen(1) ) ), 0, 
						tmpKeyLenT, StringComparison::Ordinal );

			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"レコードの内容が不正です。", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 10;
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
					tmpFJIsamException->ErrorCode = 1701;
					tmpFJIsamException->ErrorCode2 = 11;
					throw tmpFJIsamException;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコード読み込みに失敗しました。", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 12;
				throw tmpFJIsamException;
			}

			//キー比較
			try
			{
				chkKeyFlg = String::Compare( tmpKeyStr, 0, 
					FJIFuncs::KeyStringConv( 
						FJIFuncs::ArrayToString( tmpRec->getData(nullptr), getKeyPos(1), getKeyLen(1) ) ), 0,
						tmpKeyLenT, StringComparison::Ordinal );

			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"レコードの内容が不正です。", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 13;
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

	//-- サブインデックス検索 --//
	}else{

		List<SubIndexRecord^>^ tmpSubList;

		//サブインデックス取得
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1701;
			tmpFJIsamException->ErrorCode2 = 14;
			throw tmpFJIsamException;
		}
		//サブインデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1701;
			tmpFJIsamException->ErrorCode2 = 15;
			throw tmpFJIsamException;
		}

		//対象サブインデックスレコード取得
		try{
			tmpSubList = chkSubIdx->ReadRecordWords( inKey );
			if( tmpSubList == nullptr )	//該当レコードなし
			{
				return nullptr;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1701;
			tmpFJIsamException->ErrorCode2 = 16;
			throw tmpFJIsamException;
		}

		//データレコード
		rtList = gcnew List<DataRecord^>(tmpSubList->Count);

		for( Int32 i = 0; i < tmpSubList->Count; i++ )
		{

			try
			{
				tmpRec = getRecord( inIdxNum, tmpSubList[i] );
				rtList->Add( tmpRec );

			}catch( Exception^ exp ){
				for( Int32 j = 0; j < tmpSubList->Count; j++ )
				{
					if( i > j )
					{
						delete rtList[j];
					}else{
						delete tmpSubList[j];
					}
				}
				delete tmpSubList;
				delete rtList;

				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					String::Format( L"サブインデックスキーに対するレコード読込に失敗しました。インデックス({0:d2}) リスト番号{1:d2}", inIdxNum, i), exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 17;
				throw tmpFJIsamException;
			}
		}
		delete tmpSubList;
	}

	//-- 正常終了 --//
	return rtList;

}
