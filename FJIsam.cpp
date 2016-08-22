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
#include "FJIsam.h"				//ISAMファイル
#include "FJIsamFunc.h"			//ISAM用関数
#include "FJIsamException.h"	//FJIsam例外クラス

using namespace FJIsam;
using namespace System;
using namespace System::IO;

//------------------------------------------------------------------------------
//  class	:FJIsam	:ISAM　メインクラス
//------------------------------------------------------------------------------

//***** コンストラクタ１ *****
FJIsam::FIsam::FIsam()
{
}

//***** コンストラクタ３ *****
FJIsam::FIsam::FIsam( String^ inFname, Int32 inBufsz, IsamOpenMode inMode )
	:	BuildID(0)						//ファイル編成カウンタ
{
	//ファイルオープン
	if( inFname != nullptr &&
		inFname->Length > 0 )
	{
		this->Open( inFname, inBufsz, inMode );
	}
}

//------------------------------------------------------------------------------
//***** デストラクタ *****
FJIsam::FIsam::~FIsam()
{
	//ISAMファイルクローズ
	Close();
}


//------------------------------------------------------------------------------
// ファイルアクセス 基本
//------------------------------------------------------------------------------
//***** ISAMファイルオープン
Int32 FJIsam::FIsam::Open( String^ inFname, Int32 inBufsz, IsamOpenMode inMode )
{
	SubIndex^		tmpSubIdx;			//サブインデックスワーク
	String^			tmpFilePath;		//ファイルパス
	String^			tmpExt;				//拡張子

	//-- すでにファイルがオープンしていればエラー
	if( FStream != nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"既にファイルを開いています。");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック
	if( inFname == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイル名が未指定です。");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( inFname->Length == 0 )			//ファイル名が未指定
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイル名が空です。");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inMode != IsamOpenMode::READ_WRITE &&		//ファイルオープンモードチェック
		inMode != IsamOpenMode::READ_ONLY )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイルオープン方法が不正です。");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//ファイルバッファ作成
	if( inBufsz < 4096 )
	{
		inBufsz = 4096;
	}

	//ファイルの文字チェック(利用できない文字があるか)
	if( inFname->IndexOfAny( Path::GetInvalidPathChars() ) != -1 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイル名に不正な文字が利用されています。");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}	
	//拡張子チェック( .fisか？)
	tmpExt = gcnew String(IS_EXTSTR);
	if( String::Compare( Path::GetExtension( inFname ), tmpExt , true ) == 0 )
	{
		// .fis ならそのまま
		tmpFilePath = String::Copy( inFname );
	}else {
		// .fis 以外なら .fis を付加
		tmpFilePath = String::Copy( inFname );
		tmpFilePath->Concat( tmpExt  );
	}

	//ファイル存在チェック
	if( !File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"指定されたファイルが存在しません。");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	//ファイルバッファ定義
	FileBuff = gcnew array<Byte>(inBufsz);

	//-- ヘッダ情報領域定義 --//
	FID = gcnew String( L'\x0000', IS_FIDLEN );
	ISAMVer = gcnew String( L'\x0000', IS_VERLEN );
	ISKeyInfo = nullptr;
	UserInfo = gcnew String( L'\x0000', IS_UINFLEN );

	//サブインデックス情報
	SubIdx = gcnew List<SubIndex^>();

	//ファイル名セット
	FilePath = tmpFilePath;

	//-- ファイルオープン(既存のみ)
	try{
		//読み書き可能
		if( inMode == IsamOpenMode::READ_WRITE )
		{
			FStream = File::Open( tmpFilePath,		//ファイル名
						FileMode::Open,				//作成モード
						FileAccess::ReadWrite,		//アクセスモード
						FileShare::None );			//共有：無し

		//読込専用
		}else{
			FStream = File::Open( tmpFilePath,		//ファイル名
						FileMode::Open,				//作成モード
						FileAccess::Read,			//アクセスモード
						FileShare::Read );			//共有：読込可
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ファイルのオープンに失敗しました。",exp );
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//-- ヘッダ情報読み込み
	try
	{
		ReadHeader();
	}catch( Exception^ exp ){
		FStream->Close();
		delete FStream;
		FStream = nullptr;

		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ヘッダ読込に失敗しました。",exp );
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//-- サブインデックスファイルオープン
	for( Int32 i = 2; i <= IdxCnt ; i++)
	{
		try
		{
			tmpSubIdx = gcnew SubIndex( this, i, inMode );
			SubIdx->Add( tmpSubIdx );

		//サブインデックスのオープンに失敗したら全体をクローズ
		}catch(FJIsamException^ tmpFJIsamException2 )
		{
			delete tmpSubIdx;

			for( Int32 j = 0; j < i-2; j++ )
			{
				tmpSubIdx->Close();
				delete tmpSubIdx;
				SubIdx[i-2] = nullptr;
			}

			FStream->Close();
			delete FStream;
			FStream = nullptr;

			//例外を送信
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( String::Format( L"サブインデックス{0:d2}のオープンに失敗しました。", i ), 
														tmpFJIsamException2 );
			tmpFJIsamException->ErrorCode = 1001;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

	}

	//-- ISAM情報収集 --//
	CreateBuildID();
	try
	{
		if( ChkKeyPositions() != 0 )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスキー情報取得に失敗しました。" );														
			tmpFJIsamException->ErrorCode = 1001;
			tmpFJIsamException->ErrorCode2 = 10;
		}
	}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスのキー情報取得に失敗しました。", exp );														
			tmpFJIsamException->ErrorCode = 1001;
			tmpFJIsamException->ErrorCode2 = 11;
	}

	//-- 正常終了--//
	delete tmpExt;

	return 0;
}

//------------------------------------------------------------------------------
//***** ISAMファイルクローズ
Int32 FJIsam::FIsam::Close(void)
{
	SubIndex^	tmpSubIdx;						//サブインデックスワーク

	//既にファイルがクローズなら何もしない
	if( FStream == nullptr )
	{
		return 0;
	}

	//ファイルバッファ
	if( FileBuff != nullptr ) delete FileBuff;
	FileBuff = nullptr;

	//ヘッダ情報領域定義
	if( FID != nullptr ) delete FID;
	FID = nullptr;
	if( ISAMVer != nullptr ) delete ISAMVer;
	ISAMVer = nullptr;
	if( ISKeyInfo != nullptr ) delete ISKeyInfo;
	ISKeyInfo = nullptr;
	if( UserInfo != nullptr ) delete UserInfo;
	UserInfo = nullptr;
	//ファイル名を空にする
	if( FilePath != nullptr ) delete FilePath;
	FilePath = nullptr;

	//ISAMファイルクローズ
	FStream->Close();
	delete FStream;
	FStream = nullptr;

	BuildID = 0;

	//サブインデックスファイルクローズ
	for( Int32 i = 2; i <= IdxCnt ; i++)
	{
		tmpSubIdx = SubIdx[i-2];
		if(tmpSubIdx != nullptr )
		{
			tmpSubIdx->Close();
			delete tmpSubIdx;
		}
	}
	SubIdx->Clear();
	if( SubIdx != nullptr ) delete SubIdx;

	//情報クリア
	AllRecCnt = 0;
	KeyCnt = 0;

	RecCnt = 0;
	RecFirstPos = 0L;
	RecLastPos = 0L;
	RecRootPos = 0L;

	DelRecCnt = 0;
	DelRecFirstPos = 0L;
	DelRecLastPos = 0L;

	RecMaxLen = 0;
	RecMinLen = 0;

	IdxCnt = 0;

	//-- 正常終了 --//
	return 0;
}

//------------------------------------------------------------------------------
//***** ISAMファイル作成
Int32 FJIsam::FIsam::Create( String^ inFname, String^ inFID,
						  KeyInfo^ inKeyInfo, String^ inUserInfo,
						  Int32 inBufsz,
						  System::Security::AccessControl::FileSecurity^ inFileSecurity  )
{
	String^			tmpFilePath;		//ファイルパス
	String^			tmpExt;				//拡張子

	//-- ISAMの状態チェック（ファイルを開いているとエラーにする)
	if( FStream != nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"既にファイルが開いています。ファイルを閉じた後、新規作成してください。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック
	//ファイル名
	if( inFname == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ファイル名が未指定です。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inFname->Length == 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ファイル名が未指定です。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inFID == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ファイルIDが未指定です。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}
	if( inFID->Length == 0 || inFID->Length > IS_FIDLEN )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ファイルIDの長さが不正です(1～16文字)。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}
	//キー情報
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー情報を指定してください。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}
	//ユーザー情報
	if( inUserInfo != nullptr
		&& inUserInfo->Length > IS_UINFLEN)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ユーザー情報の長さが不正です(0～256文字)。" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//ファイルバッファチェック
	if( inBufsz < 4096 )
	{
		inBufsz = 4096;
	}

	//-- ファイル名チェック
	//ファイルの文字チェック
	if( inFname->IndexOfAny( Path::GetInvalidPathChars() ) != -1 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイル名に不正な文字が利用されています。");
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}	
	//拡張子チェック( .fisか？)
	tmpExt = gcnew String( IS_EXTSTR );
	if( String::Compare( Path::GetExtension( inFname ), tmpExt, true ) == 0 )
	{
		// .fis ならそのまま
		tmpFilePath = String::Copy( inFname );
	}else {
		// .fis 以外なら .fis を付加
		tmpFilePath = String::Copy( inFname );
		tmpFilePath->Concat( tmpExt );
	}

	//ファイル存在チェック
	if( File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"同じ名前のファイル名が存在します。");
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;

	}

	//-- ISAM新規作成 --//
	//ファイルバッファ定義
	FileBuff = gcnew array<Byte>(inBufsz);

	//サブインデックス情報
	SubIdx = gcnew List<SubIndex^>();

	try
	{
		FStream = nullptr;

		if( inFileSecurity == nullptr )
		{
			FStream  = File::Create( tmpFilePath,
						FileBuff->Length,
						FileOptions::RandomAccess );
		}else{
			FStream = File::Create( tmpFilePath,
						FileBuff->Length,
						FileOptions::RandomAccess,
						inFileSecurity);
		}
	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ファイルの作成に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 10;

		throw tmpFJIsamException;
	}

	//-- ISAM制御情報設定
	//ファイルID設定
	FID = String::Copy( inFID );

	//ISAMバージョン設定
	try
	{
		ISAMVer = L" 1. 2. 0. 0";
	}catch(Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAMのバージョン取得に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 11;

		throw tmpFJIsamException;
	}
	//キー情報設定
	ISKeyInfo = gcnew KeyInfo( inKeyInfo );

	//ユーザー情報設定(任意)
	if( inUserInfo != nullptr )
	{
		UserInfo = String::Copy( inUserInfo );
	}else{
		UserInfo = L"";
	}

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
	
	RecMaxLen = 0;
	RecMinLen = 0;

	IdxCnt = 1;

	//-- ISAM制御部書込
	try
	{
		WriteHeader();
	}catch(Exception^ exp)
	{
		FStream->Close();
		delete FStream;
		FStream = nullptr;

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAM情報の作成に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 11;

		throw tmpFJIsamException;
	}

	//ファイル名セット
	FilePath = String::Copy( tmpFilePath );

	//-- ISAM情報収集 --//
	CreateBuildID();
	try
	{
		if( ChkKeyPositions() != 0 )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスキー情報取得に失敗しました。" );														
			tmpFJIsamException->ErrorCode = 1002;
			tmpFJIsamException->ErrorCode2 = 12;
		}
	}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスのキー情報取得に失敗しました。", exp );														
			tmpFJIsamException->ErrorCode = 1002;
			tmpFJIsamException->ErrorCode2 = 13;
	}

	delete tmpExt;
	//-- 正常終了 --//
	return 0;
}

//------------------------------------------------------------------------------
//***** サブインデックス作成 *****
Int32	FJIsam::FIsam::CreateSubIndex( KeyInfo^ inKeyInfo )
{
	SubIndex^		tmpSubIdx;		//サブインデックス

	//-- 引数作成 --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"キーが設定されていません。");
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//作成可能チェック(今のところレコードがない場合だけ許可)
	if( AllRecCnt > 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"サブインデックスの作成が出来ません。");
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- キー位置、ソートキー位置の整合性チェック(将来的なもの) --//
	if( RecCnt > 0 )
	{
		if( RecMinLen > inKeyInfo->getKeyPos() + inKeyInfo->getKeyLen() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"キーが最小レコード長より後ろにあります。キーを設定できません。");
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		if( RecMinLen > inKeyInfo->getSrtKeyPos() + inKeyInfo->getSrtKeyLen() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"ソートキーが最小レコード長より後ろにあります。キーを設定できません。");
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
	}
	//-- サブインデックス作成 --//
	
	try
	{
		tmpSubIdx = gcnew SubIndex( this, IdxCnt+1, inKeyInfo );

	}catch( Exception^ ext )
	{
		delete tmpSubIdx;

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"サブインデックスの作成に失敗しました。",
													ext );
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- サブインデックスをListに追加 --//
	SubIdx->Add( tmpSubIdx );
	IdxCnt++;

	//-- 制御部書き込み --//
	try
	{
		WriteHeader();
	}catch( Exception^ ext )
	{
		SubIndex^ chkSubIdx = SubIdx[IdxCnt-2];
		chkSubIdx->Close();
		delete chkSubIdx;

		SubIdx->RemoveAt(IdxCnt-2);

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAMヘッダの更新に失敗しました。", ext );
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode = 6;
		throw tmpFJIsamException;
	}

	//-- ISAM情報収集 --//
	CreateBuildID();
	try
	{
		if( ChkKeyPositions() != 0 )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスキー情報取得に失敗しました。" );														
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 7;
		}
	}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"インデックスのキー情報取得に失敗しました。", exp );														
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 8;
	}

	//-- 正常終了 --//
	return 0;
}

//------------------------------------------------------------------------------
//ISAM状況取得
//------------------------------------------------------------------------------
//***** ファイルパス取得
String^ FJIsam::FIsam::getFilePath( String^ otPath )
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
Int32 FJIsam::FIsam::getBuildID( void )
{
	return BuildID;
}

//***** ファイルオープンチェック
Boolean FJIsam::FIsam::isOpen( void )
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
String^ FJIsam::FIsam::getFID( String^ otFID )
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

//***** ファイル作成ISAMバージョン取得 *****
String^ FJIsam::FIsam::getVersion( String^ otVer )
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


//***** ユーザー定義情報取得 *****
String^ FJIsam::FIsam::getUserInfo( String^ otUInfo )
{
	//-- 引数チェック --//
	if( otUInfo == nullptr )
	{
		return String::Copy( UserInfo );
	}

	otUInfo->CopyTo( 0, UserInfo->ToCharArray(), 0, UserInfo->Length );

	//-- 正常終了 --//
	return otUInfo;
}

//***** レコード数取得 ***** 
UInt32 FJIsam::FIsam::getRecCnt( void )
{
	return RecCnt;
}

//***** 削除レコード数取得 *****
UInt32 FJIsam::FIsam::getDelRecCnt( void )
{
	return DelRecCnt;
}

//***** 全レコード数取得 *****
UInt32 FJIsam::FIsam::getAllRecCnt( void )
{
	return AllRecCnt;
}

//***** キー数取得 *****
UInt32 FJIsam::FIsam::getKeyCnt( void )
{
	return KeyCnt;
}

//***** レコード最大長 *****
Int32 FJIsam::FIsam::getRecMaxLen( void )
{
	return RecMaxLen;
}

//***** レコード最小長 *****
Int32 FJIsam::FIsam::getRecMinLen()
{
	return RecMinLen;

}

//***** インデックス数取得
Int32 FJIsam::FIsam::getIdxCnt( void )
{
	return IdxCnt;
}

//***** キーの最初の位置取得
Int32 FJIsam::FIsam::getKeyStartPos( void )
{
	return KeyStartPos;
}

											
//***** キーの最後の位置取得
Int32 FJIsam::FIsam::getKeyLastPos( void )
{
	return KeyLastPos;
}											

//***** 制御部データ長取得
Int32 FJIsam::FIsam::Length( void )
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

			sizeof(Int32) +				//最大レコード長
			sizeof(Int32) +				//最小レコード長

			sizeof(Int32) +				//インデックス数

			FJIsam::KeyInfo::Length() +		//キー情報
			
			sizeof(Char) * IS_UINFLEN;	//ユーザー情報

}

//------------------------------------------------------------------------------
//キー情報取得
//------------------------------------------------------------------------------
//***** インデックス番号取得
Int32 FJIsam::FIsam::getIdxNum(String^ inKeyName )
{
	//-- 引数チェック
	if( inKeyName == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException("キー名が未設定です。");
		tmpFJIsamException->ErrorCode = 1004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- キー名判定(主キー)
	if( String::Compare( ISKeyInfo->getKeyName( nullptr ), inKeyName ) == 0 )
	{
		return 1;
	}

	//-- キー名判定(サブインデックスキー) --//
	for( int i = 0; i < IdxCnt - 2; i++ )
	{
		SubIndex^ tmpSubIdx = SubIdx[i];
		if(tmpSubIdx == nullptr )
		{
			continue;
		}

		if( String::Compare( tmpSubIdx->getKeyName( nullptr ), inKeyName ) == 0 )
		{
			return i+2;
		}
	}

	//-- 該当なし　--//
	FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"該当するキーがありません。");
	tmpFJIsamException->ErrorCode2 = 1004;
	tmpFJIsamException->ErrorCode2 = 2;
	throw tmpFJIsamException;
}

//***** キー情報取得
KeyInfo^ FJIsam::FIsam::getKeyInfo( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return gcnew KeyInfo( ISKeyInfo );

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum-2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"該当するサブインデックスがありません。");
			tmpFJIsamException->ErrorCode = 1005;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyInfo();
	}
	
}

//***** インデックスキー名取得
String^ FJIsam::FIsam::getKeyName( Int32 inIdxNum, String^ otStr )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyName( otStr );

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum-2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"該当するサブインデックスがありません。");
			tmpFJIsamException->ErrorCode = 1006;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyName( otStr );
	}

}

//-- キー位置取得
Int32 FJIsam::FIsam::getKeyPos( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック --//
	if( inIdxNum > IdxCnt )	//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- メインインデックス --//
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyPos();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1007;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyPos();
	}
}

//-- キー長取得
Int32 FJIsam::FIsam::getKeyLen( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyLen();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1008;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyLen();
	}
}

//***** キータイプ取得
keyType_EN	FJIsam::FIsam::getKeyType( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyType();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1009;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyType();
	}
}

//***** キー順序取得
Byte		FJIsam::FIsam::getKeyOrder( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyOrder();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1010;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyOrder();
	}
}

//***** ユキークキーフラグ取得
Byte		FJIsam::FIsam::getKeyUniqueFlg( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1011;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyUniqueFlg();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1011;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyUniqueFlg();
	}
}

//***** ソートキー位置取得
Int32		FJIsam::FIsam::getSrtKeyPos( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1012;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyPos();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1012;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyPos();
	}
}

//***** ソートキー長取得
Int32		FJIsam::FIsam::getSrtKeyLen( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1013;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyLen();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1013;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyLen();
	}
}

//***** ソートキータイプ取得
keyType_EN	FJIsam::FIsam::getSrtKeyType( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1014;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyType();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1014;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyType();
	}
}

//***** ソートキー順序取得
Byte		FJIsam::FIsam::getSrtKeyOrder( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//チェック用SubIndex

	//-- 引数チェック
	if( inIdxNum > IdxCnt )			//インデックス番号
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"インデックス番号が不正です。");
		tmpFJIsamException->ErrorCode = 1015;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- メインインデックス
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyOrder();

	//-- サブインデックス --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//該当インデックスなし
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"該当するサブインデックスがありません(インデックス{0:d2})。", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1015;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyOrder();
	}
}

//------------------------------------------------------------------------------
//ユーザー定義情報設定
//------------------------------------------------------------------------------
Int32 FJIsam::FIsam::setUserInfo( String^ inUInfo )
{
	array<Byte>^ otUserInfo;

	//-- 引数チェック --//
	if( inUInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ユーザー情報が空です。");
		tmpFJIsamException->ErrorCode = 1016;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inUInfo->Length > IS_UINFLEN )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ユーザー情報が長すぎます(最大256文字)。");
		tmpFJIsamException->ErrorCode = 1016;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//ユーザー情報を更新
	UserInfo->Remove(0);
	UserInfo->Concat(inUInfo );

	//バッファ作成	
	otUserInfo = FJIFuncs::StringToArray( UserInfo, nullptr, 0, IS_UINFLEN );	

	//ファイルに書込
	FStream->Seek( FIsam::Length() - sizeof(Char) * IS_UINFLEN , SeekOrigin::Begin );
	FStream->Write( otUserInfo, 0, sizeof(Char) * IS_UINFLEN );

	//-- 正常終了
	return 0;
}
