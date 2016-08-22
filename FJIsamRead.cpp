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
DataRecord^ FJIsam::FIsam::ReadRecord( array<Byte>^ inKey, Int32 inIdxNum )
{
	DataRecord^		rtRec;			//取得レコード
	Int64			chkRecPos;		//チェック用レコード位置
	SubIndex^		chkSubIdx;		//チェック用サブインデックス
	SubIndexRecord^	chkIdxRec;		//チェック用サブインデックスレコード

	Int32			chkKeyFlg;		//キーチェック結果

	//-- 引数チェック --//
	if( inKey == nullptr )		//キーなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが未指定です。");
		tmpFJIsamException->ErrorCode = 1101;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inIdxNum < 1 ||			//インデックス異常
		inIdxNum >IdxCnt )			
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスの指定が不正です。");
		tmpFJIsamException->ErrorCode = 1101;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inKey->Length != getKeyLen( inIdxNum ) && 
		getKeyType( inIdxNum ) != keyType_EN::ISKEY_UNICODE )			//キー長チェック
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー長が不正です。");
		tmpFJIsamException->ErrorCode = 1101;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
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

		//キーが一致するまで検索
		while( true )
		{
			//レコードを取得してキーを比較
			try
			{
				rtRec = getRecord( chkRecPos );
				if(rtRec == nullptr )
				{
					return nullptr;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"レコードが読み込みに失敗しました。", exp );
				tmpFJIsamException->ErrorCode = 1101;
				tmpFJIsamException->ErrorCode2 = 4;
				throw tmpFJIsamException;
			}
			//キー比較
			try
			{
				chkKeyFlg = FJIFuncs::KeyCompare( inKey , rtRec->getData(nullptr), 
								ISKeyInfo, 1, KeyCmpOpt_EN::Key );
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"レコードの内容が不正です。", exp );
				tmpFJIsamException->ErrorCode = 1101;
				tmpFJIsamException->ErrorCode2 = 5;
				throw tmpFJIsamException;
			}

			//キーが一致(正常終了)
			if( chkKeyFlg == 0 )
			{
				return rtRec;

			//検索するキーの方が小さい(次は小の子枝を検索）
			}else if( chkKeyFlg < 0 )
			{
				chkRecPos = rtRec->getLtChildRecPos();

			//検索するキーの方が大きい(次は大の子枝を検索）
			}else {
				chkRecPos = rtRec->getGtChildRecPos();
			}

			delete rtRec;		//不要レコード削除

			//-- レコードがなくなれば終了 --//
			if( chkRecPos == 0 )
			{
				return nullptr;
			}
		}

	//-- サブインデックス検索 --//
	}else{
		//サブインデックス取得
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
		//サブインデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}

		//対象サブインデックスレコード取得
		try{
			chkIdxRec = chkSubIdx->ReadRecord( inKey );
			if( chkIdxRec == nullptr )	//該当レコードなし
			{
				return nullptr;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}

		//データレコード
		try
		{
			rtRec = getRecord( inIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスキーに対するレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}

		//-- 正常終了 --//
		return rtRec;

	}
}

//------------------------------------------------------------------------------
//***** 先頭レコード読込
DataRecord^ FJIsam::FIsam::FirstRecord( Int32 inIdxNum )
{
	DataRecord^		rtRec;		//返却用レコード
	SubIndex^		chkSubIdx;	//チェック用サブインデックス
	SubIndexRecord^	chkIdxRec;	//チェック用サブインデックスレコード

	//-- 引数チェック --//
	if( inIdxNum < 1 ||
		inIdxNum > IdxCnt )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックス番号が不正です。" );
		tmpFJIsamException->ErrorCode = 1102;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- レコードがなければ空を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- メインインデックス検索 --//
	if( inIdxNum == 1 )
	{
		try
		{
			rtRec = getRecord( RecFirstPos );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードの読込に失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		//-- 正常終了 --//
		return rtRec;

	//-- サブインデックス検索 --//
	}else{
		//サブインデックス取得
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		//サブインデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
		//対象サブインデックスレコード取得
		try
		{
			chkIdxRec = chkSubIdx->FirstRecord();
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}

		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//データレコード取得
		try
		{
			rtRec = getRecord( inIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスキーに対するレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//-- 正常終了 --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
//***** 最後レコード検索
DataRecord^ FJIsam::FIsam::LastRecord( Int32 inIdxNum )
{
	DataRecord^		rtRec;		//返却用レコード
	SubIndex^		chkSubIdx;	//チェック用サブインデックス
	SubIndexRecord^	chkIdxRec;	//チェック用サブインデックスレコード

	//-- 引数チェック --//
	if( inIdxNum < 1 ||
		inIdxNum > IdxCnt )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックス番号が不正です。" );
		tmpFJIsamException->ErrorCode = 1103;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- レコードがなければ空を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- メインインデックス検索 --//
	if( inIdxNum == 1 )
	{
		try
		{
			rtRec = getRecord( RecLastPos );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードの読込に失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		//-- 正常終了 --//
		return rtRec;

	//-- サブインデックス検索 --//
	}else{
		//サブインデックス取得
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		//サブインデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
		//対象サブインデックスレコード取得
		try
		{
			chkIdxRec = chkSubIdx->LastRecord();
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}

		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//データレコード取得
		try
		{
			rtRec = getRecord( inIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスキーに対するレコード読込に失敗しました。インデックス({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//-- 正常終了 --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
//***** 次レコード検索
DataRecord^ FJIsam::FIsam::NextRecord( DataRecord^ inRec )
{
	Int32			chkIdxNum;		//チェック用サブインデックスレコード
	DataRecord^		chkRec;			//チェック用レコード
	Int64			chkRecPos;		//チェック用次レコード位置
	SubIndex^		chkSubIdx;		//チェック用サブインデックス
	SubIndexRecord^ chkIdxRec;		//チェック用サブインデックスレコード
	DataRecord^		rtRec;			//読込レコード

	//-- 引数チェック --//
	if( inRec == nullptr )		//レコードなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"入力レコードがありません。" );
		tmpFJIsamException->ErrorCode = 1104;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//別のISAMまたは、再編成や再オープン等されていればエラー
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"次レコード検索が許可されません。" );
		tmpFJIsamException->ErrorCode = 1104;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	chkIdxNum = inRec->getIdxNum();

	if( chkIdxNum < 1 ||			//インデックス番号が不正
		chkIdxNum > IdxCnt )	
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"検索インデックス番号が不正です。" );
		tmpFJIsamException->ErrorCode = 1104;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- メインインデックスから検索 --//
	if( chkIdxNum == 1 )
	{
		//最新状態のレコード取得
		try{

			chkRec = getRecHeader( inRec->getRecPos() );

		}catch( Exception^ exp )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード状態取得に失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//最新レコードチェック＆次のレコード位置取得
		chkRecPos = chkRec->getNextRecPos();
		delete chkRec;
		if( chkRecPos == 0L )	
		{
			return nullptr;
		}

		//次のレコード取得
		try
		{
			rtRec = getRecord( chkRecPos );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"次レコード読込に失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		return rtRec;

	//-- サブインデックスから検索 --//
	}else{
		chkIdxRec = inRec->getSubIdxRec();
		//サブインデックスレコード取得
		if( chkIdxRec == nullptr )		//サブインデックスレコードなし
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"検索サブインデックスレコードがありませんです。" );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		try
		{
			chkSubIdx = SubIdx[chkIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
		//サブインデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", chkIdxNum) );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}
		//対象サブインデックスレコード取得
		try
		{
			chkIdxRec = chkSubIdx->NextRecord( chkIdxRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード読込に失敗しました。インデックス({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//データレコード取得
		try
		{
			rtRec = getRecord( chkIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスキーに対するレコード読込に失敗しました。インデックス({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

		//-- 正常終了 --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
//***** 前レコード検索
DataRecord^ FJIsam::FIsam::PrevRecord( DataRecord^ inRec )
{
	Int32			chkIdxNum;		//チェック用サブインデックスレコード
	DataRecord^		chkRec;			//チェック用レコード
	Int64			chkRecPos;		//チェック用前レコード位置
	SubIndex^		chkSubIdx;		//チェック用サブインデックス
	SubIndexRecord^ chkIdxRec;		//チェック用サブインデックスレコード
	DataRecord^		rtRec;			//読込レコード

	//-- 引数チェック --//
	if( inRec == nullptr )		//レコードなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"入力レコードがありません。" );
		tmpFJIsamException->ErrorCode = 1105;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//別のISAMまたは、再編成や再オープン等されていればエラー
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"前レコード検索が許可されません。" );
		tmpFJIsamException->ErrorCode = 1105;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	chkIdxNum = inRec->getIdxNum();

	if( chkIdxNum < 1 ||			//インデックス番号が不正
		chkIdxNum > IdxCnt )	
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"検索インデックス番号が不正です。" );
		tmpFJIsamException->ErrorCode = 1105;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- メインインデックスから検索 --//
	if( chkIdxNum == 1 )
	{
		//最新状態のレコード取得
		try{

			chkRec = getRecHeader( inRec->getRecPos() );

		}catch( Exception^ exp )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード状態取得に失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//最新レコードチェック＆次のレコード位置取得
		chkRecPos = chkRec->getPrevRecPos();
		delete chkRec;
		if( chkRecPos == 0L )	
		{
			return nullptr;
		}

		//次のレコード取得
		try
		{
			rtRec = getRecord( chkRecPos );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"前レコード読込に失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		return rtRec;

	//-- サブインデックスから検索 --//
	}else{
		chkIdxRec = inRec->getSubIdxRec();
		//サブインデックスレコード取得
		if( chkIdxRec == nullptr )		//サブインデックスレコードなし
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"検索サブインデックスレコードがありませんです。" );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		try
		{
			chkSubIdx = SubIdx[chkIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
		//サブインデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスが存在しません。インデックス({0:d2})", chkIdxNum) );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}
		//対象サブインデックスレコード取得
		try
		{
			chkIdxRec = chkSubIdx->NextRecord( chkIdxRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード読込に失敗しました。インデックス({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//データレコード取得
		try
		{
			rtRec = getRecord( chkIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"サブインデックスキーに対するレコード読込に失敗しました。インデックス({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

		//-- 正常終了 --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
// Private関数
//------------------------------------------------------------------------------
//***** レコード読込
DataRecord^ FJIsam::FIsam::getRecord( Int64 inRecPos )
{
	DataRecord^	rtRec;				//読込レコード
	Int32		chkDatLen;			//チェック用レコードデータ長
	Int32		readLen;			//データ読込長

	//-- 状態チェック
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルがオープンされていません。" );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック
	if( inRecPos == 0L )					//レコード位置が0なら空を返す。			
	{
		return nullptr;
	}
	if( inRecPos < FIsam::Length() )		//レコード位置が制御部にあるならエラーを返す。
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- レコード読込 --//
	//ヘッダ部読込
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff , 0, DataRecord::getHeaderLength() );
		if( readLen != DataRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報がありません。" );
			tmpFJIsamException->ErrorCode = 1106;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//レコードのデータ長チェック
	chkDatLen = FJIFuncs::ArrayToInt32( FileBuff, 0 );
	if( chkDatLen <= 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータが空です。" );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//データ部読込
	try
	{
		readLen = FStream->Read( FileBuff, DataRecord::getHeaderLength(), chkDatLen );
		if( readLen != chkDatLen )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータ情報がありません。" );
			tmpFJIsamException->ErrorCode = 1106;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//-- データレコード定義 --//
	try
	{
		rtRec = gcnew DataRecord( FileBuff, this, inRecPos, false, 1, nullptr );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード定義に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//正常終了
	return rtRec;
}

//***** レコード読込(サブインデックスより)
DataRecord^ FJIsam::FIsam::getRecord( Int32 inIdxNum, SubIndexRecord^ inIdxRec )
{
	DataRecord^	rtRec;				//読込レコード
	Int64		chkRecPos;			//チェック用レコード位置
	Int32		chkDatLen;			//チェック用レコードデータ長
	Int32		readLen;			//データ読込長

	//-- 状態チェック
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルがオープンされていません。" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック
	if( inIdxNum < 1 &&
		inIdxNum > IdxCnt )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックス番号が不正です。" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inIdxRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスレコードが指定されていません。" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- レコード位置チェック
	chkRecPos = inIdxRec->getDataRecPos();

	if( chkRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- レコード読込 --//
	//ヘッダ部読込
	try
	{
		FStream->Seek( chkRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, DataRecord::getHeaderLength() );
		if( readLen != DataRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報がありません。" );
			tmpFJIsamException->ErrorCode = 1107;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	chkDatLen = FJIFuncs::ArrayToInt32( FileBuff, 0 );
	if( chkDatLen < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータが空です。" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//データ部読込
	try
	{
		readLen = FStream->Read( FileBuff, DataRecord::getHeaderLength(), chkDatLen );
		if( readLen != chkDatLen )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータ情報がありません。" );
			tmpFJIsamException->ErrorCode = 1107;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}

	//-- データレコード定義 --//
	try
	{
		rtRec = gcnew DataRecord( FileBuff, this, inIdxRec->getDataRecPos(), false, inIdxNum, inIdxRec );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード定義に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;
	}

	//正常終了
	return rtRec;
}

//***** レコードヘッダ読出
DataRecord^ FJIsam::FIsam::getRecHeader( Int64 inRecPos )
{
	DataRecord^	rtRec;				//読込レコード
	Int32		chkDatLen;			//チェック用レコードデータ長
	Int32		readLen;			//データ読込長

	//-- 状態チェック
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルがオープンされていません。" );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック
	if( inRecPos == 0L )					//レコード位置が0なら空を返す。			
	{
		return nullptr;
	}
	if( inRecPos < FIsam::Length() )		//レコード位置が制御部にあるならエラーを返す。
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- レコード読込 --//
	//ヘッダ部読込
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, DataRecord::getHeaderLength() );
		if( readLen != DataRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報がありません。" );
			tmpFJIsamException->ErrorCode = 1108;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	chkDatLen = FJIFuncs::ArrayToInt32( FileBuff, 0 );
	if( chkDatLen < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードデータが空です。" );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- データレコード定義 --//
	try
	{
		rtRec = gcnew DataRecord( FileBuff, this, inRecPos, true, 1, nullptr );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード定義に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//正常終了
	return rtRec;
}

