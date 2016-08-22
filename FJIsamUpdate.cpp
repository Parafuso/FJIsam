//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ソース
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		FJIsam	: FJIsam 書込処理関係ソース
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAMクラス
#include "FJIsamFunc.h"				//ISAM用関数
#include "FJIsamException.h"		//例外クラス

using namespace FJIsam;
//------------------------------------------------------------------------------
//	class	:FJIsam	:ISAM本体(書込関数:レコード上書)
//------------------------------------------------------------------------------
//***** レコード上書
void FJIsam::FIsam::UpdateRecord( DataRecord^ inRec )
{
	DataRecord^		oldRec;			//確認用旧レコード
	DataRecord^		tmpOldRec;			//削除前レコードリレーション保存用レコード

	array<Byte>^	tmpData;		//レコードデータ
	array<Byte>^	oldData;		//レコードデータ

	Boolean			chkMoveFlg;			//レコード置き換えフラグ
	Int32			chkKeyChgFlg;		//キー変更フラグ

	DataRecordPos_T^ tmpDelPos;		//データレコードリレーション(レコード移動時)
	DataRecordPos_T^ tmpMvPos;			//データレコードリレーション(キー変換時)

	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getParentFile()->Equals( this ) == false ||
		inRec->getBuildID() != BuildID )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このレコードを更新する権限がありません。" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_OLD )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このレコードは更新できません。" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- 確認用レコード読込 --/
	try
	{
		oldRec = getRecord( inRec->getRecPos() );

		//確認用レコードのリレーション情報保存
		tmpOldRec = gcnew DataRecord();
		tmpOldRec->setRecPos( oldRec->getRecPos() );
		tmpOldRec->setPrevRecPos( oldRec->getPrevRecPos() );
		tmpOldRec->setNextRecPos( oldRec->getNextRecPos() );
		tmpOldRec->setParentRecPos( oldRec->getParentRecPos() );
		tmpOldRec->setLtChildRecPos( oldRec->getLtChildRecPos() );
		tmpOldRec->setGtChildRecPos( oldRec->getGtChildRecPos() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード状況検索中にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}
	if( oldRec->getRecStat() == DataRecStat_EN::REC_DEL )		//レコードが削除済みならエラー
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このレコードは削除されています。" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;		
	}

	//-- レコードの移動必要かチェック(レコード長が大きくなればファイル位置を移動) --//
	if( inRec->getDataLength() > oldRec->getDataLength() )
	{
		chkMoveFlg = true;
	}
	//-- キーの変更があったかチェック --//
	tmpData = inRec->getData( nullptr );
	oldData = oldRec->getData( nullptr );
	chkKeyChgFlg = FJIFuncs::KeyCompare( tmpData, oldData, ISKeyInfo, -1, KeyCmpOpt_EN::All );

	//-- レコード移動不要で、キーの変更もなし --//
	//該当レコードを書込だけ
	if( chkMoveFlg == false && chkKeyChgFlg == 0 )
	{
		tmpDelPos = nullptr;
		tmpMvPos = nullptr;

		inRec->setEachKeyFlg( oldRec->getEachKeyFlg() );		//同一キーフラグチェック
		inRec->setPrevRecPos( oldRec->getPrevRecPos() );		//前レコード位置
		inRec->setNextRecPos( oldRec->getNextRecPos() );		//次レコード位置
		inRec->setParentRecPos( oldRec->getParentRecPos() );	//二分木親レコード位置
		inRec->setLtChildRecPos( oldRec->getLtChildRecPos() );	//二分木キー小レコード位置
		inRec->setGtChildRecPos( oldRec->getGtChildRecPos() );	//二分木キー大レコード位置
		try
		{
			WriteRecord( inRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"レコード更新中にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;		
		}
			
	//-- レコード移動必要で、キーの変更もなし --//
	//レコードを最後尾に移動し、リレーションするレコードを更新する
	}else if( chkMoveFlg == true && chkKeyChgFlg == 0 ){

		tmpDelPos = nullptr;
		tmpMvPos = nullptr;

		//レコードを変更
		inRec->setRecPos( FStream->Length );						//レコード位置を変更
		inRec->setEachKeyFlg( oldRec->getEachKeyFlg() );		//同一キーフラグチェック
		inRec->setPrevRecPos( oldRec->getPrevRecPos() );		//前レコード位置
		inRec->setNextRecPos( oldRec->getNextRecPos() );		//次レコード位置
		inRec->setParentRecPos( oldRec->getParentRecPos() );	//二分木親レコード位置
		inRec->setLtChildRecPos( oldRec->getLtChildRecPos() );	//二分木キー小レコード位置
		inRec->setGtChildRecPos( oldRec->getGtChildRecPos() );	//二分木キー大レコード位置
		try
		{
			//レコード書込
			WriteRecord( inRec );
			//リレーションするレコードを書換
			WriteMoveRecordPos( inRec, inRec->getRecPos() );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"レコード移動＆更新中にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;		
		}

	//-- レコード移動不要で、キーの変更あり --//
	//レコードのリレーション削除し、新たに挿入位置を検索する
	}else if(  chkKeyChgFlg != 0 ){
		tmpDelPos = gcnew DataRecordPos_T;
		tmpDelPos->PrevPos = nullptr;
		tmpDelPos->NextPos = nullptr;
		tmpDelPos->ParentPos = nullptr;
		tmpDelPos->LtChildPos = nullptr;
		tmpDelPos->GtChildPos = nullptr;
		tmpDelPos->ExtRecPos = nullptr;

		tmpMvPos = gcnew DataRecordPos_T;
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
			if( chkMoveFlg == true )					//レコード位置移動があれば変更
			{
				inRec->setRecPos( FStream->Length );
			}

			inRec->setEachKeyFlg( IS_NOEACHKEY );		//同一キーフラグ
			inRec->setPrevRecPos( 0L );					//前レコード位置
			inRec->setNextRecPos( 0L );					//次レコード位置
			inRec->setParentRecPos( 0L );				//二分木親レコード位置
			inRec->setLtChildRecPos( 0L );				//二分木キー小レコード位置
			inRec->setGtChildRecPos( 0L );				//二分木キー大レコード位置

			//レコード挿入位置を検索
			FindInsertRecordPos( inRec, tmpMvPos );
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

			//レコード書込
			WriteRecord( inRec );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"レコードのキー変更検索中にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}
	}


	//-- レコードを移動したなら、旧レコードを削除レコードにする --//
	if( chkMoveFlg == true )
	{
		//-- 削除リスト更新 --//
		if( DelRecLastPos != 0 )
		{
			DataRecord^ tmpDelRec;				//削除レコード
	
			//削除リスト最後尾レコード取得
			try
			{
				tmpDelRec = getRecord( DelRecLastPos );
			}catch(  Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
					L"削除済みレコードリスト検索時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1401;
				tmpFJIsamException->ErrorCode2 = 10;
				throw tmpFJIsamException;		
			}
			//削除したレコードを次レコードに設定
			tmpDelRec->setNextRecPos( oldRec->getRecPos() );
	
			//ヘッダ部書込み
			try
			{
				WriteRecHeader( tmpDelRec );
	
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
					L"削除済みレコードリスト書込時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1401;
				tmpFJIsamException->ErrorCode2 = 11;
				throw tmpFJIsamException;		
			}

			//削除レコード更新
			oldRec->setPrevRecPos( DelRecLastPos );			//削除レコードの前レコードに設定

			//削除リスト更新成功
			delete tmpDelRec;
		}
		//削除レコードヘッダ更新
		try
		{
			//レコードデータセット//
			oldRec->DeleteRec();
			oldRec->setParentRecPos( 0L );
			oldRec->setNextRecPos( 0L );
			oldRec->setLtChildRecPos( 0L );
			oldRec->setGtChildRecPos( 0L );

			WriteRecHeader( oldRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"削除レコード書込でエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;		
		}
	}

	//-- ISAMヘッダを書込 --//
	//レコード最大長、レコード最小長を更新
	if( RecMaxLen < inRec->getDataLength() )
	{
		RecMaxLen = inRec->getDataLength();
	}
	if( RecMinLen > inRec->getDataLength() )
	{
		RecMinLen = inRec->getDataLength();
	}

	//キーが変更になった場合、キー数を更新
	if( chkKeyChgFlg != 0 )
	{
		if( oldRec->getEachKeyFlg() == IS_NOEACHKEY && 
			inRec->getEachKeyFlg() != IS_NOEACHKEY )	
		{
			KeyCnt--;									//単一キーが同一キー変更したらキーを減らす
		}else if( oldRec->getEachKeyFlg() != IS_NOEACHKEY && 
			inRec->getEachKeyFlg() == IS_NOEACHKEY )	
		{
			KeyCnt++;									//同一キーが単一キー変更したらキーを増やす
		}
	}
	//レコードが移動した場合、レコード数
	if( chkMoveFlg == true )
	{
		AllRecCnt++;									//全レコード数をカウントアップ
		DelRecCnt++;									//削除レコードカウントアップ
		//Isamレコード情報更新
		if( RecFirstPos == tmpOldRec->getRecPos() )		//先頭レコードを設定
		{
			RecFirstPos = tmpOldRec->getNextRecPos();
		}
		if( RecLastPos == tmpOldRec->getRecPos() )			//最後レコードを設定
		{
			RecLastPos = tmpOldRec->getPrevRecPos();
		}
		if( RecRootPos == tmpOldRec->getRecPos() )			//二分木ルートレコードを設定
		{
			if( tmpDelPos->ParentPos == nullptr )
			{
				RecRootPos = 0L;
			}else{
				RecRootPos = tmpDelPos->ParentPos->getRecPos();
			}
		}
		if( DelRecFirstPos == 0L )						//削除レコード先頭を設定
		{
			DelRecFirstPos = oldRec->getRecPos();
		}
		DelRecLastPos = oldRec->getRecPos();			//削除レコード最後を設定
	}
	//Isamヘッダ更新
	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"ISAMヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 13;
		throw tmpFJIsamException;		
	}


	//-- サブインデックスの追加 --//
	for( Int32 i = 2; i <= IdxCnt; i++)
	{
		SubIndex^ chkSubIdx;
		KeyInfo^  chkKeyInfo;

		array<Byte>^ tmpKey;
		array<Byte>^ tmpOldKey;

		//サブインデックスファイル取得(nullptrならスキップ)
		chkSubIdx = SubIdx[i-2];
		if( chkSubIdx == nullptr )
		{
			continue;
		}

		//サブインデックスレコード削除
		try
		{
			chkKeyInfo = chkSubIdx->getKeyInfo();
			//キーの変更がなく、レコードの移動もなければ何もしない
			if( FJIFuncs::KeyCompare( tmpData, oldData, chkKeyInfo, -1, KeyCmpOpt_EN::All ) ==0 &&
				chkMoveFlg == true )
			{
				continue;
			}

			//サブインデックスレコード更新//
			tmpKey = inRec->getKeyData( nullptr, chkKeyInfo, false );
			tmpOldKey = oldRec->getKeyData( nullptr, chkKeyInfo, false );
			if( inRec->getIdxNum() == i )
			{
				chkSubIdx->UpdateRecord( tmpOldKey, tmpKey, oldRec->getRecPos(),
											inRec->getRecPos(), 0L );
			}else{
				chkSubIdx->UpdateRecord( tmpOldKey, tmpKey, oldRec->getRecPos(),
											inRec->getRecPos(), inRec->getSubIdxRec()->getRecPos() );
			}
			//書込後のデータ削除
			delete tmpKey;
			delete tmpOldKey;

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード削除時にエラーが発生しました(インデックス({0:d2})。", i),  exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 14;
			throw tmpFJIsamException;		
		}
	}


	//-- データ削除 --//
	delete oldRec;
	delete tmpOldRec;
	delete tmpData;
	delete oldData;

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

//------------------------------------------------------------------------------
//レコード更新＆場所移動チェック	
void FJIsam::FIsam::WriteMoveRecordPos( DataRecord^ inRec, Int64 inNewRecPos )
{
	DataRecordPos_T^ tmpPos;	//更新リレーション

	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inNewRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"新しいレコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//-- 初期化 --//
	tmpPos = gcnew DataRecordPos_T;
	tmpPos->PrevPos = nullptr;
	tmpPos->NextPos = nullptr;
	tmpPos->ParentPos = nullptr;
	tmpPos->LtChildPos = nullptr;
	tmpPos->GtChildPos = nullptr;

	//-- リレーションするレコードを検索 --//
	try
	{
		//-- 前レコード検索 --//
		if( inRec->getPrevRecPos() != 0L)
		{
			tmpPos->PrevPos = getRecord( inRec->getPrevRecPos() );
			tmpPos->PrevPos->setNextRecPos( inNewRecPos );
		}
		//-- 次レコード検索 --//
		if( inRec->getNextRecPos() != 0L)
		{
			tmpPos->NextPos = getRecHeader( inRec->getNextRecPos() );
			tmpPos->NextPos->setPrevRecPos( inNewRecPos );
		}
		//-- 二分木親レコード検索 --//
		if( inRec->getParentRecPos() != 0L )
		{
			//前レコードと親レコードが同じなら、前レコードの二分木キー大レコードに設定
			if( inRec->getParentRecPos() == inRec->getPrevRecPos() )
			{
				tmpPos->PrevPos->setGtChildRecPos( inNewRecPos );

			//次レコードと親レコードが同じなら、前レコードの二分木キー小レコードに設定
			}else if( inRec->getParentRecPos() == inRec->getNextRecPos()){
				tmpPos->NextPos->setLtChildRecPos( inNewRecPos );
			//違うレコードなら、レコードを読込、更新
			}else{
				tmpPos->ParentPos = getRecHeader( inRec->getParentRecPos() );
				if( tmpPos->ParentPos->getLtChildRecPos() == inRec->getRecPos() )
				{
					tmpPos->ParentPos->setLtChildRecPos( inNewRecPos );
				}else{
					tmpPos->ParentPos->setGtChildRecPos( inNewRecPos );
				}
			}
		}
		//-- 二分木キー小レコード検索 --//
		if( inRec->getLtChildRecPos() != 0L )
		{
			//二分木キー小レコードと前レコードが同じなら、前レコードの二分木親レコードに設定
			if( inRec->getLtChildRecPos() == inRec->getPrevRecPos() )
			{
				tmpPos->PrevPos->setParentRecPos( inNewRecPos );
			
			//違うレコードなら、レコードを読込、更新
			}else{
				tmpPos->LtChildPos = getRecHeader( inRec->getLtChildRecPos() );
				tmpPos->LtChildPos->setParentRecPos( inNewRecPos );
			}
		}
		//-- 二分木キー大レコード検索 --//
		if( inRec->getGtChildRecPos() != 0L )
		{
			//二分木キー小レコードと次レコードが同じなら、次レコードの二分木親レコードに設定
			if( inRec->getGtChildRecPos() == inRec->getPrevRecPos() )
			{
				tmpPos->NextPos->setParentRecPos( inNewRecPos );
			
			//違うレコードなら、レコードを読込、更新
			}else{
				tmpPos->GtChildPos = getRecHeader( inRec->getGtChildRecPos() );
				tmpPos->GtChildPos->setParentRecPos( inNewRecPos );
			}
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
						L"リレーションレコード検索中にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- リレーションするレコードヘッダを書込 --//
	try
	{
		//前レコード
		if( tmpPos->PrevPos != nullptr )				
		{
			WriteRecHeader( tmpPos->PrevPos );
		}
		//次レコード
		if( tmpPos->NextPos != nullptr )				
		{
			WriteRecHeader( tmpPos->NextPos );
		}
		//二分木親レコード
		if( tmpPos->ParentPos != nullptr )
		{
			WriteRecHeader( tmpPos->ParentPos );
		}
		//二分木キー小レコード
		if( tmpPos->LtChildPos != nullptr )
		{
			WriteRecHeader( tmpPos->LtChildPos );
		}
		//二分木キー大レコード
		if( tmpPos->GtChildPos != nullptr )
		{
			WriteRecHeader( tmpPos->GtChildPos );
		}
		//関連レコード
		if( tmpPos->ExtRecPos != nullptr )
		{
			WriteRecHeader( tmpPos->ExtRecPos );
		}
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"リレーションレコードヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 4;
	}
}

