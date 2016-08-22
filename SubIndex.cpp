//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　サブインデックス　ソース
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:サブインデックス
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndex.h"			//サブインデックスヘッダ
#include "FJIsam.h"				//ISAMファイル
#include "FJIsamException.h"	//FJIsam例外クラス
#include "FJIsamEnum.h"

using namespace FJIsam;

//------------------------------------------------------------------------------
//  class	:SubIndex	:サブインデックス
//------------------------------------------------------------------------------
//***** コンストラクタ *****//
FJIsam::SubIndex::SubIndex( Object^ inParent, int inIdxNum, IsamOpenMode inMode )
	: FStream(nullptr)		//ファイルハンドルクリア
{
	FIsam^ tmpParent;
	String^ tmpFilePath;

	//-- 引数チェック --//
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	try{
		 tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( !tmpParent->isOpen() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルがオープンされていません。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	if(inIdxNum < 1 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	if( inMode != IsamOpenMode::READ_WRITE &&					//ファイルオープンモードチェック
		inMode != IsamOpenMode::READ_ONLY )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイルオープン方法が不正です。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- サブインデックスファイルオープン --//

	//ファイルパス取得
	try
	{
		tmpFilePath = CreateFileName( tmpParent->getFilePath(nullptr), inIdxNum );
	}catch( Exception^ exp){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイル名が不正です。",exp );
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}
	//ファイル存在チェック
	if( !File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"指定されたファイルが存在しません。");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}
	//オープン
	try{
		//読み書き可
		if( inMode == IsamOpenMode::READ_WRITE )
		{
			FStream = File::Open( tmpFilePath,		//ファイル名
						FileMode::Open,				//作成モード
						FileAccess::ReadWrite,		//アクセスモード
						FileShare::None );			//共有:無し
		
		//読込専用
		}else{
			FStream = File::Open( tmpFilePath,		//ファイル名
						FileMode::Open,				//作成モード
						FileAccess::Read,			//アクセスモード
						FileShare::Read );			//共有:読込のみ可
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ファイルのオープンに失敗しました。",exp );
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//ファイル読込バッファ定義
	FileBuff = gcnew array<Byte>(4096);

	//-- 項目セット --//
	ParentFile = inParent;							//ISAMファイル
	FilePath = tmpFilePath;							//ファイルパス
	IdxNum = inIdxNum;								//インデックス番号

	//-- ヘッダ情報読み込み --//
	try
	{
		ReadHeader();
	}catch( Exception^ exp ){
		Close();

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ヘッダ情報が読み込めません。", exp);
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}

	//--ヘッダチェック --//
	String^ tmpFID = tmpParent->getFID( nullptr);
	if( FID->Contains( tmpFID ) == 0 ||
		FID->Length != tmpFID->Length )
	{
		Close();

		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"FIDが一致しません。" );
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;
	}
	delete tmpFID;
}

//***** コンストラクタ(新規)
FJIsam::SubIndex::SubIndex(System::Object ^inParent, int inIdxNum,
						   FJIsam::KeyInfo ^inKeyInfo)
{
	FIsam^ tmpParent;
	String^ tmpFilePath;

	//-- 引数チェック --//
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	try{
		 tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルが不正です。");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( !tmpParent->isOpen() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルがオープンされていません。");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	if( inIdxNum <= 1 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//ファイルパス取得
	try
	{
		tmpFilePath = CreateFileName( tmpParent->getFilePath(nullptr), inIdxNum );
	}catch( Exception^ exp){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイル名が不正です。",exp );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//ファイル存在チェック
	if( File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"同じ名前のファイル名が存在します。");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;

	}

	//ファイル読込バッファ定義
	FileBuff = gcnew array<Byte>(4096);

	//-- ISAM新規作成 --//
	try
	{
		FStream = File::Create( tmpFilePath,
						FileBuff->Length,
						FileOptions::RandomAccess,
						gcnew Security::AccessControl::FileSecurity( tmpParent->getFilePath(nullptr),
								Security::AccessControl::AccessControlSections::Access ) );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイルの作成に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 7;

		throw tmpFJIsamException;
	}

	//-- サブインデックス情報設定 --//
	try
	{
		ParentFile = inParent;									//ISAMファイル
		FID = tmpParent->getFID( nullptr );						//ファイルID設定
		ISAMVer = tmpParent->getVersion( nullptr );				//ISAMバージョン設定
		SubKeyInfo = gcnew KeyInfo( inKeyInfo );				//キー情報設定

		//レコード情報初期化
		AllRecCnt = 0;
		KeyCnt = 0;
		RecCnt = 0;
		RecFirstPos = 0L;
		RecLastPos = 0L;
		RecRootPos = 0L;
	
		DelRecCnt = 0;
		DelRecFirstPos = 0L;
		DelRecLastPos = 0L;
		SubKeyInfo = gcnew KeyInfo( inKeyInfo );

		//-- サブインデックス制御部書込 --//
		WriteHeader();
	}catch(Exception^ exp)
	{
		Close();

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"サブインデックス情報の作成に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 8;

		throw tmpFJIsamException;
	}

	//ファイル名セット
	FilePath = tmpFilePath;
	IdxNum = inIdxNum;

}
//------------------------------------------------------------------------------
//***** デストラクタ
FJIsam::SubIndex::~SubIndex( void )
{
	//ISAMファイルクローズ
	Close();

	//ファイルパス
	if( FilePath != nullptr ) delete FilePath;

	//ファイルバッファ削除
	if( FileBuff != nullptr ) delete FileBuff;

	//ヘッダ情報
	if( FID != nullptr ) delete FID;
	if( ISAMVer != nullptr ) delete ISAMVer;
	if( SubKeyInfo != nullptr ) delete SubKeyInfo;
}

//------------------------------------------------------------------------------
// ファイルアクセス 基本
//------------------------------------------------------------------------------
//***** ファイルクローズ
Int32 FJIsam::SubIndex::Close(void)
{

	//既にファイルがクローズなら何もしない
	if( FStream == nullptr )
	{
		return 0;
	}

	//ファイル名を空にする
	delete FilePath;

	//ISAMファイルクローズ
	FStream->Close();
	delete FStream;
	FStream = nullptr;

	BuildID = 0;

	//情報クリア
	if( FID != nullptr ) delete FID;
	if( ISAMVer != nullptr ) delete ISAMVer;
	AllRecCnt = 0;
	KeyCnt = 0;

	RecCnt = 0;
	RecFirstPos = 0L;
	RecLastPos = 0L;
	RecRootPos = 0L;

	DelRecCnt = 0;
	DelRecFirstPos = 0L;
	DelRecLastPos = 0L;

	//キー情報クリア
	if( SubKeyInfo != nullptr ) delete SubKeyInfo;
	SubKeyInfo = nullptr;

	//-- 正常終了 --//
	return 0;
}

//------------------------------------------------------------------------------
//サブインデックス状況取得
//------------------------------------------------------------------------------
//***** ファイルパス取得
String^ FJIsam::SubIndex::getFilePath( String^ otPath )
{
	//ファイルをオープンしていなければnullptrを返す
	if( !isOpen()  )
	{
		return nullptr;
	}

	//otPath空
	if( otPath == nullptr )
	{
		return String::Copy( FilePath );
	//otPathあり
	}else{
		otPath->Remove( 0 );
		otPath->CopyTo( 0, FilePath->ToCharArray(), 0, FilePath->Length );
		return otPath;
	}
}

//***** BuildID取得
Int32 FJIsam::SubIndex::getBuildID( void )
{
	return BuildID;
}

//***** ファイルオープンチェック
Boolean FJIsam::SubIndex::isOpen( void )
{
	if( FStream == nullptr )
	{
		return false;
	}else{
		return true;
	}	
}
//------------------------------------------------------------------------------
//ISAM 情報取得用関数
//------------------------------------------------------------------------------
//***** ファイルID取得 *****
String^ FJIsam::SubIndex::getFID( String^ otFID )
{
	//-- 引数チェック --//
	if( otFID == nullptr )
	{
		return String::Copy(FID);
	}

	//-- データコピー --//
	otFID->CopyTo( 0, FID->ToCharArray(), 0, IS_FIDLEN );

	//-- 正常終了 --//
	return otFID;
}

//***** ファイル作成ISAMバージョン取得
String^ FJIsam::SubIndex::getVersion( String^ otVer )
{
	//-- 引数チェック --//
	if( otVer == nullptr )
	{
		return String::Copy(ISAMVer);
	}

	//-- データコピー --//
	otVer->CopyTo( 0, ISAMVer->ToCharArray(), 0, IS_VERLEN );

	//-- 正常終了 --//
	return otVer;
}

//***** レコード数取得 
UInt32 FJIsam::SubIndex::getRecCnt( void )
{
	return RecCnt;
}

//***** 削除レコード数取得
UInt32 FJIsam::SubIndex::getDelRecCnt( void )
{
	return DelRecCnt;
}

//***** 全レコード数取得
UInt32 FJIsam::SubIndex::getAllRecCnt( void )
{
	return AllRecCnt;
}

//***** キー数取得
UInt32 FJIsam::SubIndex::getKeyCnt( void )
{
	return KeyCnt;
}

//***** 制御部データ長取得
Int32 FJIsam::SubIndex::Length( void )
{
	return	sizeof(Char) * IS_FIDLEN +	//ファイルID
			sizeof(Char) * IS_VERLEN +	//ISAMバージョン

			sizeof(UInt32) +			//全レコード数
			sizeof(UInt32) +			//キー数

			sizeof(UInt32) +			//有効レコード数
			sizeof(Int64) +				//先頭レコード
			sizeof(Int64) +				//最終レコード
			sizeof(Int64) +				//二分木ルート

			sizeof(UInt32) +			//削除レコード数
			sizeof(Int64) +				//先頭レコード
			sizeof(Int64) +				//最終レコード

			FJIsam::KeyInfo::Length();	//キー情報
}
//------------------------------------------------------------------------------
//キー情報取得
//------------------------------------------------------------------------------
//***** キー情報取得
KeyInfo^ FJIsam::SubIndex::getKeyInfo( void )
{
	return gcnew KeyInfo( SubKeyInfo );	
}
//***** インデックス名取得
String^ FJIsam::SubIndex::getKeyName( String^ otStr )
{
	return SubKeyInfo->getKeyName( otStr );
}

//-- キー位置取得
Int32 FJIsam::SubIndex::getKeyPos( void )
{
	return SubKeyInfo->getKeyPos();
}

//-- キー長取得
Int32 FJIsam::SubIndex::getKeyLen( void )
{
	return SubKeyInfo->getKeyLen();
}

//***** キータイプ取得
keyType_EN	FJIsam::SubIndex::getKeyType( void )
{
	return SubKeyInfo->getKeyType();
}

//***** キー順序取得
Byte FJIsam::SubIndex::getKeyOrder( void )
{
	return SubKeyInfo->getKeyOrder();
}

//***** ユキークキーフラグ取得
Byte FJIsam::SubIndex::getKeyUniqueFlg( void )
{
	return SubKeyInfo->getKeyUniqueFlg();
}

//***** ソートキー位置取得
Int32 FJIsam::SubIndex::getSrtKeyPos( void )
{
	return SubKeyInfo->getSrtKeyPos();
}

//***** ソートキー長取得
Int32 FJIsam::SubIndex::getSrtKeyLen( void )
{
	return SubKeyInfo->getSrtKeyLen();
}

//***** ソートキータイプ取得
keyType_EN FJIsam::SubIndex::getSrtKeyType( void )
{
	return SubKeyInfo->getSrtKeyType();
}

//***** ソートキー順序取得
Byte FJIsam::SubIndex::getSrtKeyOrder( void )
{
	return SubKeyInfo->getSrtKeyOrder();
}

