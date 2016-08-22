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
#include "FJIsamFunc.h"			//Isam用関数
#include "FJIsamException.h"	//例外クラス


using namespace FJIsam;

//------------------------------------------------------------------------------
//	class	:SubIndex	:サブインデックス(読込関数)
//------------------------------------------------------------------------------
//***** レコード読込
SubIndexRecord^ FJIsam::SubIndex::ReadRecord( array<Byte>^ inKey )
{
	SubIndexRecord^	rtRec;			//読込サブインデックスレコード
	Int64			chkRecPos;		//チェック用レコード位置

	Int32			chkKeyFlg;		//キーチェック結果

	//-- 引数チェック --//
	if( inKey == nullptr )		//キーなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが未指定です。");
		tmpFJIsamException->ErrorCode = 3101;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKey->Length != getKeyLen() && 
		getKeyType() != keyType_EN::ISKEY_UNICODE )			//キー長チェック
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー長が不正です。");
		tmpFJIsamException->ErrorCode = 3101;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- レコードが0件なら nullptr を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- サブインデックス検索 --//
	chkRecPos = RecRootPos;

	//キーが一致するまで検索
	while( true )
	{
		//レコードを取得してキーを比較
		try
		{
			rtRec = getRecord( chkRecPos );
			if(rtRec == nullptr )
			{
				return nullptr;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"レコードが読み込みに失敗しました。", exp );
			tmpFJIsamException->ErrorCode = 3101;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		//キー比較
		try
		{
			chkKeyFlg = FJIFuncs::KeyCompare( inKey , rtRec->getKeyData(nullptr), 
				SubKeyInfo, IdxNum, KeyCmpOpt_EN::Key );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				L"レコードの内容が不正です。", exp );
			tmpFJIsamException->ErrorCode = 3101;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//キーが一致(正常終了)
		if( chkKeyFlg == 0 )
		{
			return rtRec;

		//検索するキーの方が小さい(次は小の子枝を検索）
		}else if( chkKeyFlg < 0 )
		{
			chkRecPos = rtRec->getLtChildRecPos();
		//検索するキーの方が大きい(次は大の子枝を検索）
		}else {
			chkRecPos = rtRec->getGtChildRecPos();
		}
		delete rtRec;		//不要レコード削除

		//-- レコードがなくなれば終了 --//
		if( chkRecPos == 0 )
		{
			return nullptr;
		}
	}
}

//------------------------------------------------------------------------------
//***** 最初のレコード読込
SubIndexRecord^ FJIsam::SubIndex::FirstRecord( void )
{
	SubIndexRecord^	rtRec;		//返却用レコード

	//-- レコードがなければ空を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- メインインデックス検索 --//
	try
	{
		rtRec = getRecord( RecFirstPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードの読込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3102;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	//-- 正常終了 --//
	return rtRec;

}

//------------------------------------------------------------------------------
//***** レコード検索(最後のレコード)
SubIndexRecord^ FJIsam::SubIndex::LastRecord( void )
{
	SubIndexRecord^		rtRec;	//返却用レコード

	//-- レコードがなければ空を返す --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- サブインデックス検索 --//
	try
	{
		rtRec = getRecord( RecLastPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードの読込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3103;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	
	//-- 正常終了 --//
	return rtRec;
}

//------------------------------------------------------------------------------
//***** 次レコード検索
SubIndexRecord^ FJIsam::SubIndex::NextRecord( SubIndexRecord^ inRec )
{
	SubIndexRecord^	rtRec;			//読込レコード
	SubIndexRecord^	chkRec;			//チェック用レコード
	Int64			chkRecPos;		//チェック用次レコード位置

	//-- 引数チェック --//
	if( inRec == nullptr )		//レコードなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"入力レコードがありません。" );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//別のISAMまたは、再編成や再オープン等されていればエラー
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"次レコード検索が許可されません。" );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- サブインデックスから検索 --//
	//最新状態のレコード取得
	try{
		chkRec = getRecHeader( inRec->getRecPos() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード状態取得に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//最新レコードチェック＆次のレコード位置取得
	chkRecPos = chkRec->getNextRecPos();
	delete chkRec;
	if( chkRecPos == 0L )	
	{
		return nullptr;
	}

	//次のレコード取得
	try
	{
		rtRec = getRecord( chkRecPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"次レコード読込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return rtRec;
}

//------------------------------------------------------------------------------
//***** 前レコード検索
SubIndexRecord^ FJIsam::SubIndex::PrevRecord( SubIndexRecord^ inRec )
{
	SubIndexRecord^	rtRec;			//読込レコード
	SubIndexRecord^	chkRec;			//チェック用レコード
	Int64			chkRecPos;		//チェック用前レコード位置

	//-- 引数チェック --//
	if( inRec == nullptr )		//レコードなし
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"入力レコードがありません。" );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//別のISAMまたは、再編成や再オープン等されていればエラー
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"前レコード検索が許可されません。" );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- サブインデックスから検索 --//
	//最新状態のレコード取得
	try{
		chkRec = getRecHeader( inRec->getRecPos() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード状態取得に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//最新レコードチェック＆次のレコード位置取得
	chkRecPos = chkRec->getPrevRecPos();
	delete chkRec;
	if( chkRecPos == 0L )	
	{
		return nullptr;
	}

	//次のレコード取得
	try
	{
		rtRec = getRecord( chkRecPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"前レコード読込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return rtRec;
}

//------------------------------------------------------------------------------
// Private関数
//------------------------------------------------------------------------------
//***** レコード読込
SubIndexRecord^ FJIsam::SubIndex::getRecord( Int64 inRecPos )
{
	SubIndexRecord^	rtRec;				//読込レコード
	Int32			chkDataLen;			//チェック用レコードデータ長
	Int32			readLen;			//データ読込長

	//-- 状態チェック --//
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルがオープンされていません。" );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック --//
	if( inRecPos == 0L )					//レコード位置が0なら空を返す。			
	{
		return nullptr;
	}
	if( inRecPos < SubIndex::Length() )		//レコード位置が制御部にあるならエラーを返す。
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- レコード読込 --//
	//ヘッダ部読込
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, SubIndexRecord::getHeaderLength() );
		if( readLen != SubIndexRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報がありません。" );
			tmpFJIsamException->ErrorCode = 3106;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//データ部読込
	try
	{
		chkDataLen = getKeyLen() + getSrtKeyLen();
		readLen = FStream->Read( FileBuff, SubIndexRecord::getHeaderLength(), chkDataLen );
		if( readLen != chkDataLen )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコードキー情報がありません。" );
			tmpFJIsamException->ErrorCode = 3106;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコードキー情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	//-- データレコード定義 --//
	try
	{
		rtRec = gcnew SubIndexRecord( FileBuff, this, inRecPos, false );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"サブインデックスレコード定義に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//正常終了
	return rtRec;
}

//------------------------------------------------------------------------------
//***** レコードヘッダ読出
SubIndexRecord^ FJIsam::SubIndex::getRecHeader( Int64 inRecPos )
{
	SubIndexRecord^	rtRec;				//読込レコード
	Int32			readLen;			//データ読込長

	//-- 状態チェック --//
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルがオープンされていません。" );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- 引数チェック --//
	if( inRecPos == 0L )					//レコード位置が0なら空を返す。			
	{
		return nullptr;
	}
	if( inRecPos < SubIndex::Length() )		//レコード位置が制御部にあるならエラーを返す。
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- レコード読込 --//
	//ヘッダ部読込
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, SubIndexRecord::getHeaderLength() );
		if( readLen != SubIndexRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報がありません。" );
			tmpFJIsamException->ErrorCode = 3108;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコードヘッダ情報が読み込めません。", exp );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- データレコード定義 --//
	try
	{
		rtRec = gcnew SubIndexRecord( FileBuff, this, inRecPos, true );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード定義に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return rtRec;
}