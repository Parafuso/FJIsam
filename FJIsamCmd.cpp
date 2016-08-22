//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ヘッダ
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		FJIsam :	FJIsam 初期処理、作成、情報部ソース
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndex.h"			//サブインデックスヘッダ
#include "FJIsam.h"				//ISAMファイル
#include "FJIsamFunc.h"			//ISAM用関数
#include "FJIsamException.h"	//FJIsam例外クラス

using namespace FJIsam;
using namespace System;
using namespace System::IO;

//------------------------------------------------------------------------------
//  class	:FJIsam	:ISAM　メインクラス
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ヘッダ情報入出力(Private)
//------------------------------------------------------------------------------
//***** ヘッダ情報読込
void FJIsam::FIsam::ReadHeader( void )
{
	Int32 readLen;
	Int32 tmpPos;			//バッファ読込位置

	//-- ファイル読込 --//
	try
	{
		FStream->Seek( 0, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff , 0, FIsam::Length() );
		if( readLen != FIsam::Length() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイルにヘッダ情報がありません。");
			tmpFJIsamException->ErrorCode = 1601;
			tmpFJIsamException->ErrorCode2 = 1;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ヘッダ情報が読み込めません。", exp);
		tmpFJIsamException->ErrorCode = 1601;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- バッファからヘッダ構造体にデータをコピー --//
	tmpPos = 0;
	try
	{
		//ファイルID
		delete FID;														
		FID = FJIFuncs::ArrayToString( FileBuff, 0, IS_FIDLEN * sizeof(Char) );
		tmpPos = IS_FIDLEN * sizeof( Char );
	
		//ファイルバージョン
		delete ISAMVer;												
		ISAMVer = FJIFuncs::ArrayToString( FileBuff, tmpPos, IS_VERLEN * sizeof(Char) );
		tmpPos += IS_VERLEN * sizeof( Char );

		//全レコード数
		AllRecCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos);
		tmpPos += sizeof(AllRecCnt);

		//キー数
		KeyCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos );		
		tmpPos += sizeof(KeyCnt);

		//有効レコード数
		RecCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos );		
		tmpPos += sizeof(RecCnt);

		//リスト先頭
		RecFirstPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(RecFirstPos);

		//リスト最後
		RecLastPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(RecLastPos);

		//二分木木ルート
		RecRootPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(RecRootPos);					
			
		//削除レコード数
		DelRecCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos );
		tmpPos += sizeof(DelRecCnt);

		//リスト先頭
		DelRecFirstPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(DelRecFirstPos);

		//リスト最後
		DelRecLastPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(DelRecLastPos);

		//レコード最大長
		RecMaxLen = FJIFuncs::ArrayToInt32( FileBuff, tmpPos );
		tmpPos += sizeof(RecMaxLen);										

		//レコード最小長
		RecMinLen = FJIFuncs::ArrayToInt32( FileBuff, tmpPos );
		tmpPos += sizeof(RecMinLen);

		//インデックス数
		IdxCnt = FJIFuncs::ArrayToInt32( FileBuff, tmpPos );
		tmpPos += sizeof(IdxCnt);								
	
		//キー情報
		ISKeyInfo = gcnew KeyInfo();
		tmpPos = ISKeyInfo->Read( FileBuff, tmpPos );

		//ユーザー情報
		delete UserInfo;
		UserInfo = FJIFuncs::ArrayToString( FileBuff, tmpPos, IS_UINFLEN * sizeof(Char) );
		tmpPos += IS_UINFLEN * sizeof( Char );
		
	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAMヘッダ情報取得に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 1601;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;
}

//------------------------------------------------------------------------------
//***** ヘッダ情報書込
void FJIsam::FIsam::WriteHeader( void )
{
	Int32 tmpPos;	

	//オフセット初期化
	tmpPos = 0;

	try
	{
		//ファイルID
		FJIFuncs::StringToArray( FID, FileBuff, tmpPos, IS_FIDLEN * sizeof(Char)  );
		tmpPos += IS_FIDLEN * sizeof( Char );

		//ファイルバージョン
		FJIFuncs::StringToArray( ISAMVer, FileBuff, tmpPos, IS_FIDLEN * sizeof(Char) );
		tmpPos += IS_FIDLEN * sizeof( Char );

		//全レコード数
		FJIFuncs::UInt32ToArray( AllRecCnt, FileBuff, tmpPos );
		tmpPos += sizeof( AllRecCnt );

		//キー数
		FJIFuncs::UInt32ToArray( KeyCnt, FileBuff, tmpPos );
		tmpPos += sizeof( KeyCnt );

		//有効レコード数
		FJIFuncs::UInt32ToArray( RecCnt, FileBuff, tmpPos );
		tmpPos += sizeof( RecCnt );

		//リスト先頭
		FJIFuncs::Int64ToArray( RecFirstPos, FileBuff, tmpPos );
		tmpPos += sizeof( RecFirstPos );

		//リスト最後
		FJIFuncs::Int64ToArray( RecLastPos, FileBuff, tmpPos );
		tmpPos += sizeof( RecLastPos );

		//二分木木ルート
		FJIFuncs::Int64ToArray( RecRootPos, FileBuff, tmpPos );
		tmpPos += sizeof( RecRootPos );

		//削除レコード数
		FJIFuncs::UInt32ToArray( DelRecCnt, FileBuff, tmpPos );
		tmpPos += sizeof( DelRecCnt );

		//リスト先頭
		FJIFuncs::Int64ToArray( DelRecFirstPos, FileBuff, tmpPos );
		tmpPos += sizeof( DelRecFirstPos );

		//リスト最後
		FJIFuncs::Int64ToArray( DelRecLastPos, FileBuff, tmpPos );
		tmpPos += sizeof( DelRecLastPos );

		//レコード最大長
		FJIFuncs::Int32ToArray( RecMaxLen, FileBuff, tmpPos );
		tmpPos += sizeof( RecMaxLen );

		//レコード最小長
		FJIFuncs::Int32ToArray( RecMinLen, FileBuff, tmpPos );
		tmpPos += sizeof( RecMinLen );

		//インデックス数
		FJIFuncs::Int32ToArray( IdxCnt, FileBuff, tmpPos );
		tmpPos += sizeof( IdxCnt );

		//キー情報
		tmpPos = ISKeyInfo->Write( FileBuff, tmpPos );

		//ユーザー情報
		FJIFuncs::StringToArray( UserInfo, FileBuff, tmpPos, IS_UINFLEN * sizeof(Char) );
		tmpPos += IS_UINFLEN * sizeof( Char );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ISAMヘッダ情報書込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1602;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//--ファイルへ書込
	try
	{
		FStream->Seek( 0, SeekOrigin::Begin );
		FStream->Write( FileBuff, 0, FIsam::Length() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ISAMヘッダ情報のファイルへの書込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1602;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;

}

//------------------------------------------------------------------------------		
// ISAM情報確認
//------------------------------------------------------------------------------
//***** キーの開始、終了位置をチェック
Int32 FJIsam::FIsam::ChkKeyPositions( void )
{
	Int32		tmpStartPos;			//全インデックス中のキー先頭位置
	Int32		tmpLastPos;				//全インデックス中のキー最後尾位置

	//-- ファイルがオープンされていなければエラー --//
	if( !isOpen() ) return -1;

	//全インデックス中のキーの先頭位置と最後尾の位置を調査
	try
	{
		
		KeyStartPos = getKeyPos( 1 );
		KeyLastPos = KeyStartPos + getKeyLen( 1 ) - 1;
		for( Int32 i = 2; i < IdxCnt; i++ )
		{
			tmpStartPos = getKeyPos( i );
			tmpLastPos = tmpStartPos + getKeyLen( i ) - 1;
			
			if( KeyStartPos > tmpStartPos ) KeyStartPos = tmpStartPos;
			if( KeyLastPos > tmpLastPos ) KeyLastPos = tmpLastPos;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"キー位置調査中にエラーが発生しました。", exp );
		tmpFJIsamException->ErrorCode = 1603;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return 0;
}

//***** BuildID生成
Int32 FJIsam::FIsam::CreateBuildID( void )
{
	DateTime tmpDate;
	Int32	tmpBuildID;

	while( true )
	{
		tmpDate = DateTime::Now;
		
		tmpBuildID = tmpDate.Year + tmpDate.Month + tmpDate.Day +
			tmpDate.Hour + tmpDate.Minute + tmpDate.Millisecond;

		if( tmpBuildID != BuildID )
		{
			BuildID = tmpBuildID;
			return 0;
		}
	}
}									
