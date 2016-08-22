//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　書込処理ソース
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
//***** レコード書き込み(Private)
void FJIsam::FIsam::WriteRecord( DataRecord^ inRec )
{
	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->isHeaderOnly() == true )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードのデータ部がありません。" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このレコードは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	try
	{
		//書込バッファ作成
		inRec->WriteRecData( FileBuff, false );

		//レコード位置付け
		FStream->Seek( inRec->getRecPos(), SeekOrigin::Begin );
		//レコード書込

		FStream->Write( FileBuff, 0, inRec->getRecLength() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード書込中にエラーが発生しました。",exp );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;
}

//------------------------------------------------------------------------------
//***** レコードヘッダ部書込(Private)
void FJIsam::FIsam::WriteRecHeader( DataRecord^ inRec )
{
	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"データレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このレコードは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	try
	{
		//書込バッファ作成
		inRec->WriteRecData( FileBuff, true );

		//レコード位置付け
		FStream->Seek( inRec->getRecPos(), SeekOrigin::Begin );
		//レコード書込
		FStream->Write( FileBuff, 0, DataRecord::getHeaderLength() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード書込中にエラーが発生しました。",exp );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;
}
