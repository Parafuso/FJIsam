//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ソース
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		FJIsam	: ISAM本体 レコード削除処理関係ソース
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAMクラス
#include "FJIsamFunc.h"				//ISAM用関数
#include "FJIsamException.h"		//例外クラス

using namespace FJIsam;
//------------------------------------------------------------------------------
//	class	:FJIsam	:ISAM本体(書込関数:レコード削除)
//------------------------------------------------------------------------------
//***** レコード削除
void FJIsam::FIsam::DeleteRecord( DataRecord^ inRec)
{
	DataRecord^		oldRec;				//確認用レコード
	DataRecord^		tmpOldRec;			//削除前レコードリレーション保存用レコード

	DataRecordPos_T^ tmpDelPos;			//データレコードリレーション

	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getParentFile()->Equals( this ) == false ||
		inRec->getBuildID() != BuildID )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このレコードを削除する権限がありません。" );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	tmpDelPos = gcnew DataRecordPos_T;

	//-- 確認用レコード取得 --//
	try
	{
		oldRec = getRecord( inRec->getRecPos() );
		if( oldRec->getDelFlg() == IS_RECDEL )
		{
			delete oldRec;

			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"このレコードは別の処理によって削除済みです。" );
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;		
		}

		//確認用レコードのリレーション情報保存
		tmpOldRec = gcnew DataRecord();
		tmpOldRec->setRecPos( oldRec->getRecPos() );
		tmpOldRec->setPrevRecPos( oldRec->getPrevRecPos() );
		tmpOldRec->setNextRecPos( oldRec->getNextRecPos() );
		tmpOldRec->setParentRecPos( oldRec->getParentRecPos() );
		tmpOldRec->setLtChildRecPos( oldRec->getLtChildRecPos() );
		tmpOldRec->setGtChildRecPos( oldRec->getGtChildRecPos() );

	//レコード読込失敗
	}catch( Exception^ exp ){	
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"削除レコードの状況確認時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- 削除削除情報検索 --//	
	try
	{
		ChkDeleteRecordPos( oldRec, tmpDelPos );
	}catch( Exception^ exp )	//レコード読込失敗
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"削除レコードリレーション情報チェックでエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;		
	}

	//-- リレーションするレコードヘッダを書込 --//
	try
	{
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
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"リレーションレコードヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 7;
	}

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
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 8;
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
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;		
		}

		//削除レコード更新
		oldRec->setPrevRecPos( DelRecLastPos );			//削除レコードの前レコードに設定

		//削除リスト更新成功
		delete tmpDelRec;
	}

	//-- 削除レコードヘッダ更新 --//
	try
	{
		//-- レコードデータセット --//
		oldRec->DeleteRec();
		oldRec->setParentRecPos( 0L );
		oldRec->setNextRecPos( 0L );
		oldRec->setLtChildRecPos( 0L );
		oldRec->setGtChildRecPos( 0L );

		WriteRecHeader( oldRec );
	}catch( Exception^ exp )	//レコード読込失敗
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"削除レコード書込でエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;		
	}

	//-- ISAMヘッダを書込 --//
	if( oldRec->getEachKeyFlg() == IS_NOEACHKEY )	//キーカウントダウン	
	{
		KeyCnt--;
	}

	RecCnt--;										//有効レコードカウントダウン
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
		if( tmpOldRec->getEachKeyFlg() == IS_EACHKEYFST )
		{
			RecRootPos = tmpDelPos->NextPos->getRecPos();
		}else if( RecCnt == 0 ){
			RecRootPos = 0L;
		}
	}

	if( DelRecFirstPos == 0L )						//削除レコード先頭に設定(レコード無しの場合)
	{
		DelRecFirstPos = oldRec->getRecPos();			
	}
	DelRecLastPos = oldRec->getRecPos();			//削除レコード最後を設定

	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"ISAMヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 11;
		throw tmpFJIsamException;		
	}

	//-- サブインデックスの追加 --//
	for( Int32 i = 2; i <= IdxCnt; i++)
	{
		SubIndex^ chkSubIdx;
		KeyInfo^  chkKeyInfo;
		array<Byte>^ tmpKey;

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
			tmpKey = oldRec->getKeyData( nullptr, chkKeyInfo, false );
			if( inRec->getIdxNum() == i )
			{
				chkSubIdx->DeleteRecord( tmpKey, oldRec->getRecPos(), 0L );
			}else{
				chkSubIdx->DeleteRecord( tmpKey, oldRec->getRecPos(), inRec->getSubIdxRec()->getRecPos() );
			}
			//書込後のデータ削除
			delete tmpKey;

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード削除時にエラーが発生しました(インデックス({0:d2})。", i),  exp );
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;		
		}
	}

	//-- データ削除 --//
	delete oldRec;
	delete tmpOldRec;

	if( tmpDelPos->PrevPos != nullptr ) delete tmpDelPos->PrevPos; 
	if( tmpDelPos->NextPos != nullptr ) delete tmpDelPos->NextPos; 
	if( tmpDelPos->ParentPos != nullptr ) delete tmpDelPos->ParentPos; 
	if( tmpDelPos->LtChildPos != nullptr ) delete tmpDelPos->LtChildPos; 
	if( tmpDelPos->GtChildPos != nullptr ) delete tmpDelPos->GtChildPos; 
	if( tmpDelPos->ExtRecPos != nullptr ) delete tmpDelPos->ExtRecPos; 
	delete tmpDelPos;

	//-- 正常終了 --//
	return;
}

//------------------------------------------------------------------------------
// Private 関数
//------------------------------------------------------------------------------
//***** レコード削除位置チェック
void FJIsam::FIsam::ChkDeleteRecordPos( DataRecord^ inRec, DataRecordPos_T^ inDelPos )
{
	DataRecord^			tmpRec;				//作業レコード
	DataRecord^			tmpOldRec;			//作業旧レコード

	DataRecord^			tmpParentRec;		//作業二分木親レコード
	DataRecord^			tmpLtChildRec;		//作業二分木キー小レコード
	DataRecord^			tmpGtChildRec;		//作業二分木キー大レコード

	Int32				tmpLtChildDep;		//二分木小子枝候補階層
	Int32				tmpGtChildDep;		//二分木大子枝候補階層
	DataRecord^			tmpLtChildEndRec;	//二分木キー小側最下層レコード
	DataRecord^			tmpGtChildEndRec;	//二分木キー大側最下層レコード

	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未設定です。" );
		tmpFJIsamException->ErrorCode = 1502;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inDelPos == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードリレーション検索情報が未設定です。" );
		tmpFJIsamException->ErrorCode = 1502;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	inDelPos->PrevPos = nullptr;
	inDelPos->NextPos = nullptr;
	inDelPos->ParentPos = nullptr;
	inDelPos->LtChildPos = nullptr;
	inDelPos->GtChildPos = nullptr;
	inDelPos->ExtRecPos = nullptr;
	tmpOldRec = nullptr;
	tmpParentRec = nullptr;
	tmpLtChildRec = nullptr;
	tmpGtChildRec = nullptr;

	//-- リスト前レコード検索 --//
	if( inRec->getPrevRecPos() != 0L )
	{
		//レコード読出
		try
		{
			tmpRec = getRecHeader( inRec->getPrevRecPos() );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"削除前レコード検索時にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;		
		}
		//前レコードの設定
		inDelPos->PrevPos = tmpRec;
		inDelPos->PrevPos->setNextRecPos( inRec->getNextRecPos() );		//次レコード位置

		//同一キーを更新(削除レコードが同一キー最後の場合)
		if( inRec->getEachKeyFlg() == IS_EACHKEYLST )
		{
			//前レコードが同一キーの最初なら、同一キーなしに設定
			if( inDelPos->PrevPos->getEachKeyFlg() == IS_EACHKEYFST )
			{
				 inDelPos->PrevPos->setEachKeyFlg( IS_NOEACHKEY );
			//上記以外(同一キー途中しかありえない)なら、同一キー最後に設定
			}else{
				inDelPos->PrevPos->setEachKeyFlg( IS_EACHKEYLST );
			}
		}
	}

	//-- リスト次レコード処理 --//
	if( inRec->getNextRecPos() != 0L )
	{
		//レコード読出
		try
		{
			tmpRec = getRecHeader( inRec->getPrevRecPos() );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"削除次レコード検索時にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;		
		}
		//次レコードの設定
		inDelPos->NextPos = tmpRec;
		inDelPos->NextPos->setPrevRecPos( inRec->getPrevRecPos() );		//次レコード位置

		//削除レコードが同一キー先頭の場合、次レコードの同一キー、二分木のツリー更新
		if( inRec->getEachKeyFlg() == IS_EACHKEYFST )
		{
			//次レコードが同一キー最後の場合、同一キーなしに設定する
			if( inDelPos->NextPos->getEachKeyFlg() == IS_EACHKEYLST )
			{
				inDelPos->NextPos->setEachKeyFlg( IS_NOEACHKEY );
			}else{
				inDelPos->NextPos->setEachKeyFlg( IS_EACHKEYFST );
			}
			inDelPos->NextPos->setParentRecPos( inRec->getParentRecPos() );		//二分木親キーを設定
			inDelPos->NextPos->setLtChildRecPos( inRec->getLtChildRecPos() );	//二分木キー小を設定
			inDelPos->NextPos->setGtChildRecPos( inRec->getGtChildRecPos() );	//二分木キー大を設定
		}
	}
	//-- 二分木更新が必要ない場合（同一キーの途中、最後）なら終了 --//
	if( inRec->getEachKeyFlg() == IS_EACHKEYMID ||
		inRec->getEachKeyFlg() == IS_EACHKEYLST  )
	{
		return;
	}

	//-- 同一キーの先頭の場合 --//
	if( inRec->getEachKeyFlg() == IS_EACHKEYFST )
	{
		//二分木親レコードを設定
		if( inRec->getParentRecPos() != 0L )
		{
			//前レコードと二分木親レコードが同一の場合
			if( inRec->getPrevRecPos() == inRec->getParentRecPos() )
			{
				inDelPos->PrevPos->setGtChildRecPos( inDelPos->NextPos->getRecPos() );
			//前レコードと二分木親レコードが同一でない場合
			}else{
				tmpRec = getRecHeader( inRec->getParentRecPos() );
				if( tmpRec == nullptr )
				{
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"削除二分木親レコード検索時にエラーが発生しました。" );
					tmpFJIsamException->ErrorCode = 1502;
					tmpFJIsamException->ErrorCode2 = 4;
					throw tmpFJIsamException;		
				}
				inDelPos->LtChildPos = tmpRec;

				if( tmpRec->getLtChildRecPos() == inRec->getRecPos() )
				{
					tmpRec->setLtChildRecPos( inDelPos->NextPos->getRecPos() );
				}else{
					tmpRec->setGtChildRecPos( inDelPos->NextPos->getRecPos() );
				}
			}
		}

		//二分木キー小レコードを設定
		if( inRec->getLtChildRecPos() != 0L )
		{
			//前レコードと二分木キー小レコードが同一の場合
			if( inRec->getPrevRecPos() == inRec->getLtChildRecPos() )
			{
				inDelPos->PrevPos->setParentRecPos( inDelPos->NextPos->getRecPos() );
			//前レコードと二分木キー小レコードが同一でない場合
			}else{
				tmpRec = getRecHeader( inRec->getLtChildRecPos() );
				if( tmpRec == nullptr )
				{
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"削除二分木キー小レコード検索時にエラーが発生しました。" );
					tmpFJIsamException->ErrorCode = 1502;
					tmpFJIsamException->ErrorCode2 = 5;
					throw tmpFJIsamException;		
				}
				inDelPos->ParentPos = tmpRec;
				tmpRec->setParentRecPos( inDelPos->NextPos->getRecPos() );
			}
		}
	
		//二分木キー大レコードを設定
		if( inRec->getGtChildRecPos() != 0L )
		{
			tmpRec = getRecHeader( inRec->getGtChildRecPos() );
			if( tmpRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"削除二分大キー小レコード検索時にエラーが発生しました。" );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 6;
				throw tmpFJIsamException;		
			}
			inDelPos->GtChildPos = tmpRec;
			tmpRec->setParentRecPos( inDelPos->NextPos->getRecPos() );
		}

		//-- 正常終了 --//
		return;
	}

	//-- 二分木親レコード --//
	if( inRec->getParentRecPos() != 0L )
	{
		//親レコード設定
		if( inRec->getParentRecPos() == inRec->getPrevRecPos() )		//前レコードと同一
		{
			tmpParentRec = inDelPos->PrevPos;
		}else if( inRec->getParentRecPos() == inRec->getPrevRecPos() ){	//次レコードと同一
			tmpParentRec = inDelPos->PrevPos;
		}else{															//上記とは異なるレコード
			try
			{
				tmpRec = getRecHeader( inRec->getParentRecPos() );
				inDelPos->ParentPos = tmpRec;
				tmpParentRec = tmpRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"削除二分木親レコード検索時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 7;
				throw tmpFJIsamException;		
			}
		}
	}

	//-- 二分木にキー大、小レコードの両方ない場合、親レコードの子リレーションをなくして終了 --//
	if( inRec->getLtChildRecPos() == 0L && 
		inRec->getGtChildRecPos() == 0L )
	{
		//削除レコードがキー小にリレーション
		if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )	
		{
			tmpParentRec->setLtChildRecPos( 0L );
		//削除レコードがキー大にリレーション
		}else{
			inDelPos->ParentPos->setGtChildRecPos( 0L );
		}
		return;
	}

	//-- キー小のみがない場合(キー大を親レコードにリレーション) --//
	if( inRec->getLtChildRecPos() == 0 && 
		inRec->getGtChildRecPos() > 0 )
	{
		//二分木キー大レコード設定
		if( inRec->getGtChildRecPos() == inRec->getNextRecPos() )		//次レコードと同一
		{
			tmpGtChildRec = inDelPos->NextPos;
		}else{															//上記以外
			try
			{
				tmpRec = getRecHeader( inRec->getGtChildRecPos() );
				inDelPos->GtChildPos = tmpRec;
				tmpGtChildRec = tmpRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"削除二分木キー大レコード検索時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 8;
				throw tmpFJIsamException;		
			}
		}
		//親レコードがない場合、キー大レコードが親レコードになる
		if( tmpParentRec == nullptr )
		{
			tmpGtChildRec->setParentRecPos( 0L );
			//キー大レコードが次レコードと同じ場合、次レコードを親レコードに移動
			if( tmpGtChildRec->Equals( inDelPos->NextPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->NextPos;
				inDelPos->NextPos = nullptr;
			//キー大レコードが次レコードと異なる場合、キー大レコードを親レコードに移動
			}else{
				inDelPos->ParentPos = inDelPos->GtChildPos;
				inDelPos->GtChildPos = nullptr;
			}
		}else{
			tmpGtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//キー大レコードの親レコードを設定

			//親レコード
			//削除レコードがキー小にリレーション	
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpGtChildRec->getRecPos() ); 
			//削除レコードがキー大にリレーション
			}else{
				tmpParentRec->setGtChildRecPos( tmpGtChildRec->getRecPos() ); 
			}	
		}

		//-- 正常終了 --//
		return;
	}

	//-- キー大のみがない場合(キー小を親レコードにリレーション) --//
	if( inRec->getLtChildRecPos() > 0 && 
		inRec->getGtChildRecPos() == 0 )
	{
		//二分木キー小レコード設定
		if( inRec->getLtChildRecPos() == inRec->getPrevRecPos() )		//次レコードと同一
		{
			tmpLtChildRec = inDelPos->PrevPos;
		}else{															//上記以外
			try
			{
				tmpRec = getRecHeader( inRec->getLtChildRecPos() );
				inDelPos->LtChildPos = tmpRec;
				tmpLtChildRec = tmpRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"削除二分木キー小レコード検索時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 9;
				throw tmpFJIsamException;		
			}
		}
		//親レコードがない場合、キー小レコードが親レコードになる
		if( tmpParentRec == nullptr )
		{
			tmpLtChildRec->setParentRecPos( 0L );
			//キー小レコードが前レコードと同じ場合、前レコードを親レコードに移動
			if( tmpLtChildRec->Equals( inDelPos->PrevPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->PrevPos;
				inDelPos->PrevPos = nullptr;
			//キー小レコードが次レコードと異なる場合、キー小レコードを親レコードに移動
			}else{
				inDelPos->ParentPos = inDelPos->LtChildPos;
				inDelPos->LtChildPos = nullptr;
			}
		}else{
			tmpLtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//キー大レコードの親レコードを設定

			//親レコード
			//削除レコードがキー小にリレーション
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpLtChildRec->getRecPos() ); 
			//削除レコードがキー大にリレーション
			}else{
				tmpParentRec->setGtChildRecPos( tmpLtChildRec->getRecPos() ); 
			}
		}

		//-- 正常終了 --//
		return;
	}

	//--二分木キー小レコード検索 --//
	//二分木キー小レコード設定
	if( inRec->getLtChildRecPos() == inRec->getPrevRecPos() )		//前レコードと同一
	{
		tmpLtChildRec = inDelPos->PrevPos;
	}else{															//上記以外
		try
		{
			tmpRec = getRecHeader( inRec->getLtChildRecPos() );
			inDelPos->LtChildPos = tmpRec;
			tmpLtChildRec = tmpRec;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"削除二分木キー小レコード検索時にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;		
		}
	}

	//二分木キー側の最下層レコード(キー大側を検索)
	tmpLtChildDep = 0;
	if( tmpLtChildRec->getGtChildRecPos() == 0 )
	{
		tmpLtChildEndRec = tmpLtChildRec;
	}else{
		try
		{
			tmpRec = getRecHeader(  tmpLtChildRec->getGtChildRecPos() );
			tmpLtChildDep++;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"削除二分木キー小レコード深度検索時にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 11;
			throw tmpFJIsamException;		
		}
		while( true )
		{
			//二分木最下層なら終了
			if(  tmpRec->getGtChildRecPos() == 0 )
			{
				tmpLtChildEndRec = tmpRec;
				break;
			}
			//次のレコードを読む
			try
			{
				tmpOldRec = tmpRec;
				tmpRec = getRecHeader( tmpLtChildRec->getGtChildRecPos() );
				tmpLtChildDep++;
				delete tmpOldRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"削除二分木キー小レコード深度検索時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 12;
				throw tmpFJIsamException;
			}
		}
	}

	//--二分木キー大レコード検索 --//
	//二分木キー大レコード設定
	if( inRec->getGtChildRecPos() == inRec->getNextRecPos() )		//次レコードと同一
	{
		tmpGtChildRec = inDelPos->NextPos;
		tmpGtChildEndRec = inDelPos->NextPos;
	}else{															//上記以外
		try
		{
			tmpRec = getRecHeader( inRec->getGtChildRecPos() );
			inDelPos->GtChildPos = tmpRec;
			tmpGtChildRec = tmpRec;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"削除二分木キー大レコード検索時にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 13;
			throw tmpFJIsamException;		
		}
	}

	//二分木キー側の最下層レコード(キー小側を検索)
	tmpGtChildDep = 0;
	if( tmpGtChildRec->getLtChildRecPos() == 0 )
	{
		tmpGtChildEndRec = tmpGtChildRec;
	}else{
		try
		{
			tmpRec = getRecHeader(  tmpLtChildRec->getLtChildRecPos() );
			tmpGtChildDep++;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"削除二分木キー大レコード深度検索時にエラーが発生しました。", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 14;
			throw tmpFJIsamException;		
		}
		while( true )
		{
			//二分木最下層なら終了
			if(  tmpRec->getLtChildRecPos() == 0 )
			{
				tmpGtChildEndRec = tmpRec;
				break;
			}
			//次のレコードを読む
			try
			{
				tmpOldRec = tmpRec;
				tmpRec = getRecHeader( tmpLtChildRec->getLtChildRecPos() );
				tmpLtChildDep++;
				delete tmpOldRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"削除二分木キー大レコード深度検索時にエラーが発生しました。", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 15;
				throw tmpFJIsamException;
			}
		}
	}

	//-- 二分木深度チェック(どちらが親レコードとリレーションするか) --//
	//キー小の方が深度が浅い場合、キー大はキー小の下につく
	if( tmpLtChildDep < tmpGtChildDep )
	{
		//親レコードをセット
		//親レコードがない場合、キー小レコードが親レコードになる
		if( tmpParentRec == nullptr )
		{
			tmpLtChildRec->setParentRecPos( 0L );
			RecRootPos = tmpLtChildRec->getRecPos();				//ルートレコードに設定

			//キー小レコードが前レコードと同じ場合、前レコードを親レコードに移動
			if( tmpLtChildRec->Equals( inDelPos->PrevPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->PrevPos;
				inDelPos->PrevPos = nullptr;
			//キー小レコードが前レコードと異なる場合、キー小レコードを親レコードに移動
			}else{
				inDelPos->ParentPos = inDelPos->LtChildPos;
				inDelPos->LtChildPos = nullptr;
			}
		}else{
			tmpLtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//キー大レコードの親レコードを設定
			//親レコード
			//削除レコードがキー小にリレーション
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpLtChildRec->getRecPos() ); 
			//削除レコードがキー大にリレーション
			}else{
				tmpParentRec->setGtChildRecPos( tmpLtChildRec->getRecPos() ); 
			}
		}

		//検索レコードの下にキー大レコードを位置づける
		tmpLtChildEndRec->setGtChildRecPos( tmpGtChildRec->getRecPos() );		//キー小の最下層にキー大レコードを設定
		tmpGtChildRec->setParentRecPos( tmpLtChildEndRec->getRecPos() );		//キー大レコードの親レコードにキー小の最下層レコードを設定
		
		//拡張レコードがキー小レコードと同じでなければ、検索結果に設定
		if( tmpLtChildEndRec->Equals( tmpLtChildRec ) == false )
		{
			inDelPos->ExtRecPos = tmpLtChildEndRec;
		}

	//キー大の方が深度が浅いか、両者同じ場合、キー大はキー小の下につく
	}else{
		//親レコードをセット
		//親レコードがない場合、キー小レコードが親レコードになる
		if( tmpParentRec == nullptr )
		{
			tmpGtChildRec->setParentRecPos( 0L );
			RecRootPos = tmpGtChildRec->getRecPos();				//ルートレコードに設定

			//キー小レコードが次レコードと同じ場合、次レコードを親レコードに移動
			if( tmpGtChildRec->Equals( inDelPos->NextPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->NextPos;
				inDelPos->NextPos = nullptr;
			//キー大レコードが次レコードと異なる場合、キー大レコードを親レコードに移動
			}else{
				inDelPos->ParentPos = inDelPos->GtChildPos;
				inDelPos->GtChildPos = nullptr;
			}
		}else{
			tmpGtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//キー大レコードの親レコードを設定
			//親レコード
			//削除レコードがキー小にリレーション
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpGtChildRec->getRecPos() ); 
			//削除レコードがキー大にリレーション
			}else{
				tmpParentRec->setGtChildRecPos( tmpGtChildRec->getRecPos() ); 
			}
		}

		//検索レコードの下にキー大レコードを位置づける
		tmpGtChildEndRec->setGtChildRecPos( tmpLtChildRec->getRecPos() );		//キー大の最下層にキー小レコードを設定
		tmpLtChildRec->setParentRecPos( tmpGtChildEndRec->getRecPos() );		//キー小レコードの親レコードにキー大の最下層レコードを設定
		
		//拡張レコードがキー大レコードと同じでなければ、検索結果に設定
		if( tmpGtChildEndRec->Equals( tmpGtChildRec ) == false )
		{
			inDelPos->ExtRecPos = tmpGtChildEndRec;
		}
	}
}
