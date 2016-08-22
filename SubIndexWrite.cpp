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
//	class	:SubIndex	:サブインデックス(書込関数)
//------------------------------------------------------------------------------
void FJIsam::SubIndex::WriteRecord( SubIndexRecord^ inRec )
{
	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->isHeaderOnly() == true )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコードのデータ部がありません。" );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このサブインデックスレコードは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 3201;
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
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコード書込中にエラーが発生しました。",exp );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;
}

//------------------------------------------------------------------------------
//***** レコードヘッダ部書込(Private)
void FJIsam::SubIndex::WriteRecHeader( SubIndexRecord^ inRec )
{
	//-- 引数チェック --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコードが未指定です。" );
		tmpFJIsamException->ErrorCode = 3202;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このサブインデックスレコードは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 3202;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"このファイルは書込みが許可されません。" );
		tmpFJIsamException->ErrorCode = 3202;
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
		FStream->Write( FileBuff, 0, SubIndexRecord::getHeaderLength() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコード書込中にエラーが発生しました。",exp );
		tmpFJIsamException->ErrorCode = 3202;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;
}