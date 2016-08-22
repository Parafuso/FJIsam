//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ヘッダ
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:サブインデックス
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"				//ISAMクラス
#include "FJIsamFunc.h"			//ISAM用関数
#include "FJIsamException.h"	//例外クラス

using namespace FJIsam;
//------------------------------------------------------------------------------
// ヘッダ情報入出力(Private)
//------------------------------------------------------------------------------
//***** ヘッダ情報読込
void FJIsam::SubIndex::ReadHeader( void )
{
	Int32 readLen;
	Int32 tmpPos;	//

	//-- ファイル読込 --//
	try
	{
		FStream->Seek( 0, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff , 0, SubIndex::Length() );
		if( readLen != SubIndex::Length() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ファイルにヘッダ情報がありません。" );
			tmpFJIsamException->ErrorCode = 3601;
			tmpFJIsamException->ErrorCode2 = 1;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ヘッダ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 3601;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	tmpPos = 0;

	//-- バッファからヘッダ構造体にデータをコピー --//
	try
	{
		//ファイルID
		delete FID;														
		FID = FJIFuncs::ArrayToString( FileBuff, 0, IS_FIDLEN * sizeof( Char ) );
		tmpPos = IS_FIDLEN * sizeof( Char );
	
		//ファイルバージョン
		delete ISAMVer;												
		ISAMVer = FJIFuncs::ArrayToString( FileBuff, tmpPos, IS_VERLEN * sizeof( Char ) );
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
	
		//キー情報
		SubKeyInfo = gcnew KeyInfo();
		tmpPos = SubKeyInfo->Read( FileBuff, tmpPos );
		tmpPos += KeyInfo::Length();

	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスヘッダ情報取得に失敗しました。。", exp );
		tmpFJIsamException->ErrorCode = 3601;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;
}

//***** ヘッダ情報書込
void FJIsam::SubIndex::WriteHeader( void )
{
	Int32 tmpPos;

	//オフセット初期化
	tmpPos = 0;

	try
	{
		//ファイルID
		FJIFuncs::StringToArray( FID, FileBuff, tmpPos, IS_FIDLEN * sizeof( Char ) );
		tmpPos += IS_FIDLEN * sizeof( Char );

		//ファイルバージョン
		FJIFuncs::StringToArray( ISAMVer, FileBuff, tmpPos, IS_FIDLEN * sizeof( Char ) );
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

		//キー情報
		tmpPos = SubKeyInfo->Write( FileBuff, tmpPos );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException(L"サブインデックスヘッダ情報書込に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 1017;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//--ファイルへ書込 --//
	try
	{
		Int32 tst = SubIndex::Length();
		FStream->Seek( 0, SeekOrigin::Begin );
		FStream->Write( FileBuff, 0, SubIndex::Length() );
	
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスヘッダ情報のファイルへの書込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 1602;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- 正常終了 --//
	return;

}

//------------------------------------------------------------------------------
//サブインデックスファイル名生成(Private)
//------------------------------------------------------------------------------
//***** サブインデックスファイル名生成
String^	FJIsam::SubIndex::CreateFileName( String^ inFname, int inIdxNum )
{
	//-- 引数チェック --//
	if( inFname == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイル名が未設定です。" );
		tmpFJIsamException->ErrorCode = 3603;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//サブインデックス名作成
	return  String::Concat( Path::GetDirectoryName( inFname ), L"\\", 
					Path::GetFileNameWithoutExtension( inFname ),
					String::Format( L".fs{0:d2}", inIdxNum ) );

}

