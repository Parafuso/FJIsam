//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ソース
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		FJIsam : FJIsam レコード追加処理関係ソース
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAMクラス
#include "FJIsamFunc.h"				//ISAM用関数
#include "FJIsamException.h"		//例外クラス

using namespace FJIsam;
//------------------------------------------------------------------------------
//	class	:FJIsam	:ISAM本体(書込関数:レコード追加)
//------------------------------------------------------------------------------
//***** レコード追加(新規レコード追加)
void FJIsam::FIsam::AddRecord( DataRecord^ inRec)
{
	DataRecordPos_T^ tmpAddPos;			//データレコードリレーション

	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->isHeaderOnly() == true )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードのデータ部がありません。" );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecLength() < getKeyLastPos() + 1 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データ部が短すぎます。" );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このサブインデックスレコードは追加できません。" );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}
	
	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- データ初期化 --//
	tmpAddPos = gcnew DataRecordPos_T;

	//-- レコード基本情報設定 --//
	try
	{
		inRec->setParentFile( this );			//Isamファイル
		inRec->setRecPos( FStream->Length );	//レコード追加位置
	}catch(Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"レコード基本情報設定時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;		
	}

	//-- レコード追加位置検索(リスト、二分木) --//
	try
	{
		FindInsertRecordPos( inRec, tmpAddPos );

	}catch( FJIsamException^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"レコード追加位置検索にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;		
	}

	//-- レコード書込 --//
	try
	{
		WriteRecord( inRec );									//レコードをファイルに書込
		inRec->setRecStat( DataRecStat_EN::REC_OLD );			//レコード状態を設定

	}catch( FJIsamException^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"データレコード書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;		
	}

	//-- リレーションするレコードヘッダを書込 --//
	try
	{
		//前レコード
		if( tmpAddPos->PrevPos != nullptr )				
		{
			WriteRecHeader( tmpAddPos->PrevPos );
		}
		//次レコード
		if( tmpAddPos->NextPos != nullptr )				
		{
			WriteRecHeader( tmpAddPos->NextPos );
		}
		//二分木親レコード
		if( tmpAddPos->ParentPos != nullptr )
		{
			WriteRecHeader( tmpAddPos->ParentPos );
		}
		//二分木キー小レコード
		if( tmpAddPos->LtChildPos != nullptr )
		{
			WriteRecHeader( tmpAddPos->LtChildPos );
		}
		//二分木キー大レコード
		if( tmpAddPos->GtChildPos != nullptr )
		{
			WriteRecHeader( tmpAddPos->GtChildPos );
		}
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"リレーションレコードヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;		
	}

	//-- ISAMヘッダを書込 --//
	//レコード最大、最小の長さを設定
	if( RecCnt == 0 )
	{
		RecMaxLen = inRec->getDataLength();
		RecMinLen = inRec->getDataLength(); 
	}else{
		if( RecMaxLen < inRec->getDataLength() )
		{
			RecMaxLen = inRec->getDataLength();
		}
		if( RecMinLen > inRec->getDataLength() )
		{
			RecMinLen = inRec->getDataLength();
		}
	}
	AllRecCnt++;									//全レコードカウントアップ
	if( inRec->getEachKeyFlg() == IS_NOEACHKEY )	//キーカウントアップ	
	{
		KeyCnt++;
	}
	RecCnt++;										//有効レコードカウントアップ
	if( RecCnt == 1 )
	{
		RecRootPos = inRec->getRecPos();			//ルートレコード
		RecFirstPos = inRec->getRecPos();			//先頭レコード
		RecLastPos = inRec->getRecPos();			//最後レコード
	}else{
		if( inRec->getEachKeyFlg() == IS_EACHKEYFST &&
			inRec->getNextRecPos() == RecRootPos )	//ルートレコード
		{
			RecRootPos = inRec->getRecPos();
		}

		if( RecFirstPos == inRec->getNextRecPos() )	//先頭レコード
		{
			RecFirstPos = inRec->getRecPos();
		}
		if( RecLastPos == inRec->getPrevRecPos() )	//最後レコード
		{
			RecLastPos = inRec->getRecPos();
		}
	}

	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"ISAMヘッダ書込時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1301;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;		
	}

	//-- サブインデックスの追加 --//
	for( Int32 i = 2; i <= IdxCnt; i++)
	{
		SubIndex^ chkSubIdx;
		KeyInfo^  chkKeyInfo;
		array<Byte>^ tmpKey;
		array<Byte>^ tmpSrtKey;
		SubIndexRecord^ tmpSubIdxRec;

		//サブインデックスファイル取得(nullptrならスキップ)
		chkSubIdx = SubIdx[i-2];
		if( chkSubIdx == nullptr )
		{
			continue;
		}

		//サブインデックスレコード追加
		try
		{
			chkKeyInfo = chkSubIdx->getKeyInfo();
			tmpKey = inRec->getKeyData( nullptr, chkKeyInfo, true );
			tmpSrtKey = inRec->getSrtKeyData( nullptr, chkKeyInfo );

			tmpSubIdxRec = gcnew SubIndexRecord( chkSubIdx );
			tmpSubIdxRec->setDataRecPos( inRec->getRecPos() );
			tmpSubIdxRec->setKeyData( tmpKey );
			tmpSubIdxRec->setSrtKeyData( tmpSrtKey );

			chkSubIdx->AddRecord( tmpSubIdxRec );

			//書込後のデータ削除
			delete tmpKey;
			delete tmpSrtKey;
			delete tmpSubIdxRec;

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"サブインデックスレコード追加時にエラーが発生しました(インデックス({0:d2})。", i),  exp );
			tmpFJIsamException->ErrorCode = 1301;
			tmpFJIsamException->ErrorCode2 = 11;
			throw tmpFJIsamException;		
		}
	}

	//-- データ削除 --//
	if( tmpAddPos->PrevPos != nullptr ) delete tmpAddPos->PrevPos; 
	if( tmpAddPos->NextPos != nullptr ) delete tmpAddPos->NextPos; 
	if( tmpAddPos->ParentPos != nullptr ) delete tmpAddPos->ParentPos; 
	if( tmpAddPos->LtChildPos != nullptr ) delete tmpAddPos->LtChildPos; 
	if( tmpAddPos->GtChildPos != nullptr ) delete tmpAddPos->GtChildPos; 
	delete tmpAddPos;

	//-- 正常終了 --//
	return;
}

//------------------------------------------------------------------------------
//***** Private 関数 *****//
//--レコード挿入位置検索(Private)
void FJIsam::FIsam::FindInsertRecordPos( DataRecord^ inRec, DataRecordPos_T^ inAddPos )
{
	DataRecord^		tmpRec;				//検索レコード
	DataRecord^		tmpOldRec;			//検索旧レコード
	array<Byte>^	chkKey;				//検索キー情報
	array<Byte>^	chkSrtKey;			//検索キー情報
	array<Byte>^	tmpData;			//検索用データ情報

	Int64			chkRecPos;			//チェック用レコード位置

	Int32			chkVal;				//キーチェック結果
	
	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未設定です。" );
		tmpFJIsamException->ErrorCode = 1302;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inAddPos == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードリレーション検索情報が未設定です。" );
		tmpFJIsamException->ErrorCode = 1302;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}


	//-- データ初期化 --//
	inAddPos->PrevPos = nullptr;
	inAddPos->NextPos = nullptr;
	inAddPos->ParentPos = nullptr;
	inAddPos->LtChildPos = nullptr;
	inAddPos->GtChildPos = nullptr;
	tmpOldRec = nullptr;

	//-- ISAMで1件目のレコードならISAMでのレコード位置をセットして終了 --//
	if( RecCnt == 0 )
	{
		return;
	}

	//-- チェック用変数初期化 --//
	try
	{
		chkKey = inRec->getKeyData( nullptr, ISKeyInfo, true );
		if( ISKeyInfo->getSrtKeyLen() > 0 )
		{
			chkSrtKey = inRec->getSrtKeyData( nullptr, ISKeyInfo );
		}
		chkRecPos = RecRootPos;
		tmpOldRec = nullptr;
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード追加位置検索情報初期化時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1302;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}


	//-- 二分木挿入位置検索 --//
	try
	{
		tmpRec = getRecord( RecRootPos );
	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"二分木ルートレコードの取得に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1302;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	try
	{
		while( true )
		{
			tmpData = tmpRec->getData( nullptr );
			chkVal = FJIFuncs::KeyCompare( chkKey, tmpData, ISKeyInfo, 1, KeyCmpOpt_EN::Key );

			//挿入レコードのキーと検索レコードのキーが同じ(二分木検索終了)
			if( chkVal == 0 )
			{
				break;

			//挿入レコードのキーが小さい(二分木キー小レコードを読込)
			}else if( chkVal < 0 )
			{
				chkRecPos = tmpRec->getLtChildRecPos(); 
			//挿入レコードのキーが大きい(二分木キー大レコードを読込)
			}else{
				chkRecPos = tmpRec->getGtChildRecPos(); 
			}
			//二分木の子レコードが無くなれば終了
			if( chkRecPos == 0L )
			{
				inAddPos->ParentPos = tmpRec;
				break;
			}
	
			//次のレコードを検索
			delete tmpOldRec;								//前回検索のレコードを削除
			tmpOldRec = tmpRec;								//検索レコードをバックアップ
			tmpRec = getRecord( chkRecPos );				//次のレコードを読込
		}

		//-- 挿入レコードのキーが同一キーの場合の処理 --//
		//ソートキーを調べる
		
		if( chkVal == 0 ) 
		{
			chkVal = FJIFuncs::KeyCompare( chkSrtKey, tmpData, ISKeyInfo, 1, KeyCmpOpt_EN::Srt );

			//ソートキーが小さい場合、レコードは同一キーの先頭になる。
			if( chkVal < 0 )
			{
				//レコードの状況をセット
				inRec->setEachKeyFlg( IS_EACHKEYFST );							//同一キーフラグ:先頭
				inRec->setPrevRecPos( tmpRec->getPrevRecPos() );				//前レコード位置：次レコードの前レコード
				inRec->setNextRecPos( tmpRec->getRecPos() );					//次レコード位置：次レコード
				inRec->setParentRecPos( tmpRec->getParentRecPos() );			//二分木親レコード位置：次レコードの親レコード
				inRec->setLtChildRecPos( tmpRec->getLtChildRecPos() );			//二分木キー小レコード位置：次レコードのキー小
				inRec->setGtChildRecPos( tmpRec->getGtChildRecPos() );			//二分木キー大レコード位置：次レコードのキー大
				inAddPos->NextPos = tmpRec;

				//親レコードを設定
				if( tmpOldRec != nullptr )
				{
					inAddPos->ParentPos = tmpOldRec;
					//親レコードの二分木の子レコードを設定
					if( inAddPos->ParentPos->getLtChildRecPos() == tmpRec->getRecPos() )
					{
						inAddPos->ParentPos->setLtChildRecPos( inRec->getRecPos() );
					}else{
						inAddPos->ParentPos->setGtChildRecPos( inRec->getRecPos() );
					}

					//親レコードの次レコードが次レコードなら、検索レコードをセット
					if( inAddPos->ParentPos->getNextRecPos() == inAddPos->NextPos->getRecPos() )
					{
						inAddPos->ParentPos->setNextRecPos( inRec->getRecPos() );
					}
				}

				//前レコードを設定
				//先頭のレコードかチェック
				if( inRec->getPrevRecPos() != 0L )	
				{
					//前レコードが親レコード以外なら前レコードを読込、前レコードの次レコードに挿入レコードをセット
					if( inRec->getParentRecPos() != inRec->getPrevRecPos() )
					{
						inAddPos->PrevPos = getRecHeader( inRec->getPrevRecPos() );	
						inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );
					}
				}
				
				//次レコード(同一キーレコード)を設定
				if( tmpRec->getEachKeyFlg() == IS_EACHKEYFST )			//同一キーフラグ：先頭→真中
				{
					tmpRec->setEachKeyFlg( IS_EACHKEYMID );
				}else if( tmpRec->getEachKeyFlg() == IS_NOEACHKEY ){	//同一キーフラグ：単一→最後尾
					tmpRec->setEachKeyFlg( IS_EACHKEYLST );
				}
				tmpRec->setPrevRecPos( inRec->getRecPos() );			//前レコード：挿入レコード
				tmpRec->setParentRecPos( 0L );							//二分木親レコード無し
				tmpRec->setLtChildRecPos( 0L );							//二分木キー小レコード無し
				tmpRec->setGtChildRecPos( 0L );							//二分木キー大レコード無し

				//二分木キー小レコードを設定
				if( inRec->getLtChildRecPos() != 0L )
				{
					if( inRec->getLtChildRecPos() != inRec->getPrevRecPos() || 
						inAddPos->PrevPos == nullptr )
					{
						inAddPos->LtChildPos = getRecHeader( inRec->getLtChildRecPos() );	//二分木キー小レコード読込
						inAddPos->LtChildPos->setParentRecPos( inRec->getRecPos() );		//親レコードに設定
					}else{
						inAddPos->PrevPos->setParentRecPos( inRec->getRecPos() );
					}
				}

				//二分木キー大レコードを設定
				if( inRec->getGtChildRecPos() != 0L )
				{
					inAddPos->GtChildPos = getRecHeader( inRec->getGtChildRecPos() );	//二分木キー大レコード読込
					inAddPos->GtChildPos->setParentRecPos( inRec->getRecPos() );		//親レコードに設定
				}

			//同一キーの先頭ではない場合
			}else{
				//挿入レコードを設定
				inAddPos->ParentPos = nullptr;
				inRec->setParentRecPos( 0L );
				inRec->setLtChildRecPos( 0L );
				inRec->setGtChildRecPos( 0L );
				
				while( true )
				{
					//同一キーフラグが同一キー最後、または同一キー無しなら検索終了
					if( tmpRec->getEachKeyFlg() == IS_EACHKEYLST ||
						tmpRec->getEachKeyFlg() == IS_NOEACHKEY )
					{
						//検索レコードを設定
						inRec->setEachKeyFlg( IS_EACHKEYLST );				//同一キーフラグを最後尾に設定
						inRec->setPrevRecPos( tmpRec->getRecPos() );		//前レコードに設定
						inRec->setNextRecPos( tmpRec->getNextRecPos() );	//次レコードを設定

						//前レコードを設定
						inAddPos->PrevPos = tmpRec;
						if( tmpRec->getEachKeyFlg() == IS_EACHKEYLST )
						{
							inAddPos->PrevPos->setEachKeyFlg( IS_EACHKEYMID );		//同一キーフラグ：真中
						}else{
							inAddPos->PrevPos->setEachKeyFlg( IS_EACHKEYFST );		//同一キーフラグ：最後
						}
						inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );		//検索レコードを次レコードに設定

						//次レコードを設定
						if( inRec->getNextRecPos() != 0L )
						{
							inAddPos->NextPos = getRecHeader( inRec->getNextRecPos() );
							inAddPos->NextPos->setPrevRecPos( inRec->getRecPos() );
						}
						break;
					}
					//次のレコードを比較
					delete tmpOldRec;
					tmpOldRec = tmpRec;
					tmpRec = getRecord( tmpOldRec->getNextRecPos() );
					array<Byte>^ tmpDat2 = tmpRec->getData( nullptr );
					//読込レコードのキーより小さくなれば終了
					if( FJIFuncs::KeyCompare( chkSrtKey, tmpDat2, ISKeyInfo, 1, KeyCmpOpt_EN::Srt ) < 0 )
					{
						//挿入レコードを設定
						inRec->setEachKeyFlg( IS_EACHKEYMID );
						inRec->setPrevRecPos( tmpOldRec->getRecPos() );
						inRec->setNextRecPos( tmpRec->getRecPos() );

						//前レコードを設定
						inAddPos->PrevPos = tmpOldRec;
						inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );

						//次レコードを設定
						inAddPos->NextPos = tmpRec;
						inAddPos->NextPos->setPrevRecPos( inRec->getRecPos() );
		
						delete tmpDat2;

						break;
					}
					delete tmpDat2;
				}
			}
		//-- 挿入レコードのキーが小さい場合、親レコードの大レコード、次レコードに設定 --//
		}else if( chkVal < 0 ){

			//親レコードを設定
			tmpRec->setLtChildRecPos( inRec->getRecPos() );						//二分木キー小レコードに設定

			//挿入レコードを設定
			inRec->setParentRecPos( tmpRec->getRecPos() );						//親レコードを設定
			inRec->setPrevRecPos( tmpRec->getPrevRecPos() );					//前レコードを設定
			inRec->setNextRecPos( tmpRec->getRecPos() );						//次レコードを設定
			
			//前レコードを設定
			if( inRec->getPrevRecPos() != 0L )
			{
				inAddPos->PrevPos = getRecHeader( inRec->getPrevRecPos() );		//前レコード取得
				inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );			//次レコードに設定
			}

			//次レコードを設定
			tmpRec->setPrevRecPos( inRec->getRecPos() );						//前レコードに設定

		//挿入レコードのキーが大きい場合、親レコードの大レコード、次レコードに設定 --//
		}else{
			//親レコードを設定
			tmpRec->setGtChildRecPos( inRec->getRecPos() );						//二分木キー大レコードに設定

			//挿入レコードを設定
			inRec->setParentRecPos( tmpRec->getRecPos() );						//親レコードを設定
			while( true )
			{
				if( tmpRec->getEachKeyFlg() == IS_NOEACHKEY ||
					tmpRec->getEachKeyFlg() == IS_EACHKEYLST )
				{
					break;
				}
				delete tmpOldRec;
				tmpOldRec = tmpRec;
				tmpRec = getRecord( tmpOldRec->getNextRecPos() );
			}
			inRec->setPrevRecPos( tmpRec->getRecPos() );						//前レコードを設定
			inRec->setNextRecPos( tmpRec->getNextRecPos() );					//次レコードを設定
			//前レコードを設定
			tmpRec->setNextRecPos( inRec->getRecPos() );						//次レコードに設定

			if( inAddPos->ParentPos != tmpRec )									//おやレコードと前レコードが異なれば前レコードを設定
			{
				inAddPos->PrevPos = tmpRec;
			}

			//次レコードを設定
			if( inRec->getNextRecPos() != 0L )
			{
				inAddPos->NextPos = getRecHeader( inRec->getNextRecPos() );		//次レコード取得
				inAddPos->NextPos->setPrevRecPos( inRec->getRecPos() );			//前レコードに設定
			}
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコード挿入位置検索時にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1302;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	if( chkKey != nullptr ) delete chkKey;
	if( chkSrtKey != nullptr ) delete chkSrtKey;
	if( tmpData != nullptr ) delete tmpData;

	//-- 正常終了 --//
	return;
}
