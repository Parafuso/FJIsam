//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　サブインデックス　ソース
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:サブインデックス書き込み関連ソース
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAMクラス
#include "FJIsamFunc.h"				//ISAM用関数
#include "FJIsamException.h"		//例外クラス

using namespace FJIsam;
//------------------------------------------------------------------------------
//	class	:SubIndex	:サブインデックス(書込関数:レコード更新)
//------------------------------------------------------------------------------
//***** レコード更新
void FJIsam::SubIndex::UpdateRecord(  array<Byte>^ inKey, array<Byte>^ inNewKey,				
							Int64 inRecPos, Int64 inNewRecPos, Int64 inSubIdxPos )
{
	SubIndexRecord^		oldRec;			//確認用旧レコード
	SubIndexRecord^		tmpOldRec;		//削除前レコードリレーション保存用レコード
	
	array<Byte>^		tmpData;		//キーコピー用データ
	Int32			chkKeyChgFlg;		//キー変更フラグ

	SubIndexRecordPos_T^ tmpDelPos;			//データレコードリレーション(レコード移動時)
	SubIndexRecordPos_T^ tmpMvPos;			//データレコードリレーション(キー変換時)

	//-- 引数チェック --//
	if( inKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"検索キーが未指定です。" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inNewKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"更新キーが未指定です。" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inNewRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"新しいデータレコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- 確認用レコード読込 --/
	try
	{
		//レコード位置がわかっている場合(検索時にサブインデックスを使用)
		if( inSubIdxPos != 0 )
		{
			oldRec = getRecord( inSubIdxPos );
		//レコード位置がわからない場合(検索時にメインまたは別のサブインデックスを使用)
		}else{
			SubIndexRecord^ tmpSchRec;
			SubIndexRecord^ tmpOldSchRec;
			tmpSchRec = ReadRecord( inKey );
			tmpOldSchRec = nullptr;
			while( true )
			{
				if( tmpSchRec == nullptr )
				{
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"削除対象のレコードがありません。" );
					tmpFJIsamException->ErrorCode = 3401;
					tmpFJIsamException->ErrorCode2 = 5;
					throw tmpFJIsamException;		
				}
				if( tmpSchRec->getDataRecPos() == inRecPos )
				{
					oldRec = tmpSchRec;
					if( tmpOldSchRec != nullptr ) delete tmpOldSchRec;
					break;
				}
				//もう同じキーのレコードがない
				if( tmpSchRec->getEachKeyFlg() == IS_NOEACHKEY || 
					tmpSchRec->getEachKeyFlg() == IS_EACHKEYLST ||
					tmpSchRec->getNextRecPos() == 0L )
				{
					delete tmpSchRec;
					if( tmpOldSchRec != nullptr ) delete tmpOldSchRec;
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"削除対象のレコードがありません。" );
					tmpFJIsamException->ErrorCode = 3401;
					tmpFJIsamException->ErrorCode2 = 6;
					throw tmpFJIsamException;		
				}
				//--次のレコードを読み込む
				tmpOldSchRec = tmpSchRec;
				tmpSchRec = NextRecord( tmpOldRec );
			}
		}
		if( oldRec->getDelFlg() == IS_RECDEL )
		{
			delete oldRec;

			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"このレコードは別の処理によって削除済みです。" );
			tmpFJIsamException->ErrorCode = 3401;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;		
		}

		if( oldRec->getDataRecPos() != inRecPos )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"削除対象のレコードがありません。" );
			tmpFJIsamException->ErrorCode = 3401;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;		
		}

		//確認用レコードのリレーション情報保存
		tmpOldRec = gcnew SubIndexRecord( this );
		tmpOldRec->setRecStat( oldRec->getRecStat() );
		tmpOldRec->setRecPos( oldRec->getRecPos() );
		tmpOldRec->setPrevRecPos( oldRec->getPrevRecPos() );
		tmpOldRec->setNextRecPos( oldRec->getNextRecPos() );
		tmpOldRec->setParentRecPos( oldRec->getParentRecPos() );
		tmpOldRec->setLtChildRecPos( oldRec->getLtChildRecPos() );
		tmpOldRec->setGtChildRecPos( oldRec->getGtChildRecPos() );
		tmpOldRec->setDataRecPos( oldRec->getDataRecPos() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコード状況検索中にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;		
	}

	//-- キーの変更があったかチェック --//
	chkKeyChgFlg = FJIFuncs::KeyCompare( inKey, inNewKey, SubKeyInfo, IdxNum, KeyCmpOpt_EN::All );

	//-- キーの変更がない場合 --//
	//何もせず終了
	if( chkKeyChgFlg != 0 )
	{
		delete oldRec;
		delete tmpOldRec;
		return;

	//-- キーの変更あり --//
	//レコードのリレーション削除し、新たに挿入位置を検索する
	}else{
		tmpDelPos = gcnew SubIndexRecordPos_T;
		tmpDelPos->PrevPos = nullptr;
		tmpDelPos->NextPos = nullptr;
		tmpDelPos->ParentPos = nullptr;
		tmpDelPos->LtChildPos = nullptr;
		tmpDelPos->GtChildPos = nullptr;
		tmpDelPos->ExtRecPos = nullptr;

		tmpMvPos = gcnew SubIndexRecordPos_T;
		tmpMvPos->PrevPos = nullptr;
		tmpMvPos->NextPos = nullptr;
		tmpMvPos->ParentPos = nullptr;
		tmpMvPos->LtChildPos = nullptr;
		tmpMvPos->GtChildPos = nullptr;
		tmpMvPos->ExtRecPos = nullptr;
		try
		{
			//レコードを削除する
			ChkDeleteRecordPos( oldRec, tmpDelPos );
			//移動前のキー削除を書込
			//前レコード
			if( tmpDelPos->PrevPos != nullptr )				
			{
				WriteRecHeader( tmpDelPos->PrevPos );
			}
			//次レコード
			if( tmpDelPos->NextPos != nullptr )				
			{
				WriteRecHeader( tmpDelPos->NextPos );
			}
			//二分木親レコード
			if( tmpDelPos->ParentPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->ParentPos );
			}
			//二分木キー小レコード
			if( tmpDelPos->LtChildPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->LtChildPos );
			}
			//二分木キー大レコード
			if( tmpDelPos->GtChildPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->GtChildPos );
			}
			//関連レコード
			if( tmpDelPos->ExtRecPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->ExtRecPos );
			}

			//レコードを変更
			oldRec->setEachKeyFlg( IS_NOEACHKEY );		//同一キーフラグ
			oldRec->setPrevRecPos( 0L );					//前レコード位置
			oldRec->setNextRecPos( 0L );					//次レコード位置
			oldRec->setParentRecPos( 0L );				//二分木親レコード位置
			oldRec->setLtChildRecPos( 0L );				//二分木キー小レコード位置
			oldRec->setGtChildRecPos( 0L );				//二分木キー大レコード位置
			tmpData = gcnew array<Byte>( SubKeyInfo->getKeyLen() );
			Buffer::BlockCopy( inNewKey, 0, tmpData, 0, SubKeyInfo->getKeyLen() );
			oldRec->setKeyData( tmpData );
			delete tmpData;
			tmpData = gcnew array<Byte>( SubKeyInfo->getSrtKeyLen() );
			Buffer::BlockCopy( inNewKey, SubKeyInfo->getKeyLen(), 
									tmpData, 0, SubKeyInfo->getSrtKeyLen() );
			oldRec->setKeyData( tmpData );
			delete tmpData;

			//レコード挿入位置を検索
			FindInsertRecordPos( oldRec, tmpMvPos );
			//レコード挿入位置を書込
			//前レコード
			if( tmpMvPos->PrevPos != nullptr )				
			{
				WriteRecHeader( tmpMvPos->PrevPos );
			}
			//次レコード
			if( tmpMvPos->NextPos != nullptr )				
			{
				WriteRecHeader( tmpMvPos->NextPos );
			}
			//二分木親レコード
			if( tmpMvPos->ParentPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->ParentPos );
			}
			//二分木キー小レコード
			if( tmpMvPos->LtChildPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->LtChildPos );
			}
			//二分木キー大レコード
			if( tmpMvPos->GtChildPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->GtChildPos );
			}
			//関連レコード
			if( tmpMvPos->ExtRecPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->ExtRecPos );
			}

			WriteRecord( oldRec );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"レコードのキー変更検索中にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 3401;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;
		}
	}

	//-- サブインデックスヘッダを書込 --//

	//キーが変更になった場合、キー数、レコード数を更新
	if( chkKeyChgFlg != 0 )
	{
		if( tmpOldRec->getEachKeyFlg() == IS_NOEACHKEY && 
			oldRec->getEachKeyFlg() != IS_NOEACHKEY )	
		{
			KeyCnt--;									//単一キーが同一キー変更したらキーを減らす
		}else if( tmpOldRec->getEachKeyFlg() != IS_NOEACHKEY && 
					oldRec->getEachKeyFlg() == IS_NOEACHKEY )	
		{
			KeyCnt++;									//同一キーが単一キー変更したらキーを増やす
		}
	}
	//Isamヘッダ更新
	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"サブインデックスヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 11;
		throw tmpFJIsamException;		
	}


	//-- データ削除 --//
	delete oldRec;
	delete tmpOldRec;

	if( tmpDelPos != nullptr )
	{
		if( tmpDelPos->PrevPos != nullptr ) delete tmpDelPos->PrevPos; 
		if( tmpDelPos->NextPos != nullptr ) delete tmpDelPos->NextPos; 
		if( tmpDelPos->ParentPos != nullptr ) delete tmpDelPos->ParentPos; 
		if( tmpDelPos->LtChildPos != nullptr ) delete tmpDelPos->LtChildPos; 
		if( tmpDelPos->GtChildPos != nullptr ) delete tmpDelPos->GtChildPos; 
		if( tmpDelPos->ExtRecPos != nullptr ) delete tmpDelPos->ExtRecPos; 

		delete tmpDelPos;
	}
	if( tmpMvPos != nullptr )
	{
		if( tmpMvPos->PrevPos != nullptr ) delete tmpDelPos->PrevPos; 
		if( tmpMvPos->NextPos != nullptr ) delete tmpDelPos->NextPos; 
		if( tmpMvPos->ParentPos != nullptr ) delete tmpDelPos->ParentPos; 
		if( tmpMvPos->LtChildPos != nullptr ) delete tmpDelPos->LtChildPos; 
		if( tmpMvPos->GtChildPos != nullptr ) delete tmpDelPos->GtChildPos; 
		if( tmpMvPos->ExtRecPos != nullptr ) delete tmpDelPos->ExtRecPos; 

		delete tmpDelPos;
	}

	//-- 正常終了 --//
	return;
}

