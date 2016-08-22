//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　DataRecord クラス　ソース
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.1.1	create date 2004/06/03
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	classes
//		DataRecordA	: データレコード
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "DataRecord.h"
#include "FJIsam.h"				//ISAMヘッダ
#include "FJIsamFunc.h"			//ISAM用関数
#include "FJIsamException.h"	//ISAM用例外クラス

using namespace FJIsam;

//------------------------------------------------------------------------------
//	class : DataRecord	:データレコード部
//------------------------------------------------------------------------------
//***** コンストラクタ１
FJIsam::DataRecord::DataRecord()
	:	ParentFile(nullptr),					//ISAMファイル
		BuildID(0),								//ISAMビルドカウンタ
		RecStat(DataRecStat_EN::REC_NEW),		//レコード状態
		IsHeaderOnly(false),					//レコード読込状態
		IdxNum(1),								//検索サブインデックス番号
		IdxRec(nullptr),						//検索サブインデックスレコード
		RecPos(0L),								//レコード位置
		DataLen(0),								//データ長
		DelFLG(IS_RECNOR),						//削除フラグ
		EachKeyFLG(IS_NOEACHKEY),				//同一キーフラグ
		PrevRecPos(0L),							//前レコード位置
		NextRecPos(0L),							//次レコード位置
		ParentRecPos(0L),						//親レコード位置
		LtChildRecPos(0L),						//子レコード位置(キー小)
		GtChildRecPos(0L),						//子レコード位置(キー大)
		Data(nullptr)							//データ
{
}

//***** コンストラクタ２
FJIsam::DataRecord::DataRecord( 
					array<Byte>^ inBuff,
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec )
{
	//-- データレコード初期化 --//
	Set( inBuff, inParent, inRecPos, inHeaderOnly, inIdxNum, inIdxRec );
}

//***** コンストラクタ３
FJIsam::DataRecord::DataRecord(DataRecord^ inRec )
	:	ParentFile(nullptr),					//ISAMファイル
		BuildID(0),								//ISAMビルドカウンタ
		RecStat(DataRecStat_EN::REC_NEW),		//レコード状態
		IdxNum(1),								//検索サブインデックス番号
		IdxRec(nullptr),						//検索サブインデックスレコード
		RecPos(0L),								//レコード位置
		DelFLG(IS_RECNOR),						//削除フラグ
		EachKeyFLG(IS_NOEACHKEY),				//同一キーフラグ
		PrevRecPos(0L),							//前レコード位置
		NextRecPos(0L),							//次レコード位置
		ParentRecPos(0L),						//親レコード位置
		LtChildRecPos(0L),						//子レコード位置(キー小)
		GtChildRecPos(0L)						//子レコード位置(キー大)
{
	//--各情報をコピーする
	IsHeaderOnly = inRec->IsHeaderOnly;		//レコード読込状態

	//レコード制御情報
	if( IsHeaderOnly == true )
	{
		Data = nullptr;
	}else{
		//データ
		DataLen = inRec->DataLen;				
		Data = gcnew array<Byte>(inRec->DataLen);
		Buffer::BlockCopy( inRec->Data, 0, Data, 0, inRec->DataLen );
	}
		
}

//------------------------------------------------------------------------------
//***** デストラクタ
FJIsam::DataRecord::~DataRecord()
{
	if( Data != nullptr ) delete Data;
	if( IdxRec != nullptr ) delete IdxRec;
}

//------------------------------------------------------------------------------
// レコード情報
//------------------------------------------------------------------------------
//***** レコード位置取得
Int64 FJIsam::DataRecord::getRecPos( void )
{
	return RecPos;
}

//***** Isamファイル取得
Object^ FJIsam::DataRecord::getParentFile( void )
{
	return ParentFile;
}

//***** BuildID取得
Int32 FJIsam::DataRecord::getBuildID( void )
{
	return BuildID;
}

//***** レコード長取得(全体)
Int32 FJIsam::DataRecord::getRecLength( void )
{
	return getHeaderLength() + getDataLength();
}


//***** データ長取得
Int32 FJIsam::DataRecord::getDataLength( void )
{
	if( IsHeaderOnly ) return 0;

	return DataLen;
}						
//***** レコード位置設定
void FJIsam::DataRecord::setRecPos( Int64 inRecPos )
{
	//Isamファイルなし
	if( ParentFile == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isamファイルが設定されていません。" );
		tmpFJIsamException->ErrorCode = 2009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//ファイル位置が不正
	if( inRecPos < (Int64)FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"レコード位置が不正です。" );
		tmpFJIsamException->ErrorCode = 2009;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	RecPos = inRecPos;

	return;
}

//***** Isamファイル設定
void FJIsam::DataRecord::setParentFile( Object^ inParent )
{
	FIsam^ tmpParent;

	//引数チェック
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isamファイルが設定されていません。" );
		tmpFJIsamException->ErrorCode = 2010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	try
	{
		tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isamファイルが不正です。" );
		tmpFJIsamException->ErrorCode = 2010;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( tmpParent->isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isamファイルがオープンされていません。" );
		tmpFJIsamException->ErrorCode = 2010;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//データセット
	ParentFile = inParent;
	BuildID = tmpParent->getBuildID();
	
	//正常終了
	return;
}

//***** レコード読込チェック
Boolean FJIsam::DataRecord::isHeaderOnly( void )
{
	return IsHeaderOnly;
}

//***** レコードヘッダ長取得
Int32 FJIsam::DataRecord::getHeaderLength( void )
{
	return	sizeof( Int32 ) +		//データ長
			sizeof( Byte ) +		//削除フラグ
			sizeof( Byte ) +		//同一キーフラグ
			sizeof( Int64 ) +		//前レコード位置
			sizeof( Int64 ) +		//次レコード位置
			sizeof( Int64 ) +		//二分木親レコード位置
			sizeof( Int64 ) +		//二分木キー小レコード位置
			sizeof( Int64 );		//二分木キー大レコード位置
}

//------------------------------------------------------------------------------
// インデックス情報
//------------------------------------------------------------------------------
//***** インデックス番号取得
Int32 FJIsam::DataRecord::getIdxNum( void )
{
	return IdxNum;
}

//***** サブインデックスレコード取得
SubIndexRecord^ FJIsam::DataRecord::getSubIdxRec( void )
{
	return IdxRec;
}

//------------------------------------------------------------------------------
// レコード状態
//------------------------------------------------------------------------------
//***** 同一キーフラグ取得
Byte FJIsam::DataRecord::getEachKeyFlg( void )
{
	//メインインデックス検索時
	if( IdxNum == 1 )
	{
		return EachKeyFLG;
	}
	
	//サブインデックス検索時
	if( IdxRec != nullptr)
	{
		return IdxRec->getEachKeyFlg();
	}

	//サブインデックス検索時にインデックスファイルがない
	FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスが不正です。");
	tmpFJIsamException->ErrorCode = 2002;
	tmpFJIsamException->ErrorCode2 = 1;
	throw tmpFJIsamException;
	
}

//***** 削除フラグ取得
Byte FJIsam::DataRecord::getDelFlg( void )
{
	return DelFLG;
}

//***** レコードの状態取得
DataRecStat_EN FJIsam::DataRecord::getRecStat( void )
{
	return RecStat;
}


//***** レコード削除
Int32 FJIsam::DataRecord::DeleteRec( void )
{
	DelFLG = IS_RECDEL;
	RecStat = DataRecStat_EN::REC_DEL;

	return 0;
}

//*****同一キーフラグ設定
Int32 FJIsam::DataRecord::setEachKeyFlg( Byte inEachKeyFlg )
{
	if( inEachKeyFlg <= IS_EACHKEYLST )
	{
		EachKeyFLG = inEachKeyFlg;
		return 0;
	}

	return -1;
}

//レコードの状態設定
Int32 FJIsam::DataRecord::setRecStat( DataRecStat_EN inStat )
{
	//新規レコードに変換
	if( inStat == DataRecStat_EN::REC_NEW )
	{
		ParentFile = nullptr;							//Isamファイル
		BuildID = 0;									//ISAMビルドカウンタ
		RecStat = DataRecStat_EN::REC_NEW;				//レコード状態
		IsHeaderOnly = true;							//レコード読込状態
		RecPos = 0L;									//レコード位置

		IdxNum = 1;										//検索サブインデックス番号
		if( IdxRec != nullptr ) delete IdxRec;			//検索サブインデックスレコード
		IdxRec = nullptr;			

		DataLen = 0;									//データ長
		DelFLG = IS_RECNOR;								//削除フラグ
		EachKeyFLG = IS_NOEACHKEY;						//同一キーフラグ
		PrevRecPos = 0L;								//前レコード位置
		NextRecPos = 0L;								//次レコード位置
		ParentRecPos = 0L;								//親レコード位置
		LtChildRecPos = 0L;								//子レコード位置(キー小)
		GtChildRecPos = 0L;								//子レコード位置(キー大)

		if( Data != nullptr ) delete Data;				//データ
		Data = nullptr;				
	}

	//レコード更新
	if( inStat == DataRecStat_EN::REC_OLD )
	{
		RecStat = inStat;
	}

	//レコード削除
	if( inStat == DataRecStat_EN::REC_DEL )
	{
		DeleteRec();
	}
	
	return 0;
}

//------------------------------------------------------------------------------
// レコード位置情報
//------------------------------------------------------------------------------
//***** 前レコード位置取得
Int64 FJIsam::DataRecord::getPrevRecPos( void )
{
	return PrevRecPos;
}

//***** 次レコード位置取得
Int64 FJIsam::DataRecord::getNextRecPos( void )
{
	return NextRecPos;
}

//***** 二分木親レコード位置取得
Int64 FJIsam::DataRecord::getParentRecPos( void )
{
	return ParentRecPos;
}

//***** 二分木キー小レコード位置取得
Int64 FJIsam::DataRecord::getLtChildRecPos( void )
{
	return LtChildRecPos;
}

//***** 二分木キー大レコード位置取得
Int64 FJIsam::DataRecord::getGtChildRecPos( void )
{
	return GtChildRecPos;
}

//***** 前レコード位置設定
Int32 FJIsam::DataRecord::setPrevRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	PrevRecPos = inRecPos;

	return 0;
}
//***** 次レコード位置設定
Int32 FJIsam::DataRecord::setNextRecPos( Int64 inRecPos)
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	NextRecPos = inRecPos;
	
	return 0;
}

//***** 二分木親レコード位置設定
Int32 FJIsam::DataRecord::setParentRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	ParentRecPos = inRecPos;

	return 0;
}

//***** 二分木キー小レコード位置設定
Int32 FJIsam::DataRecord::setLtChildRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	LtChildRecPos = inRecPos;

	return 0;
}

//***** 二分木キー大レコード位置設定
Int32 FJIsam::DataRecord::setGtChildRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	GtChildRecPos = inRecPos;

	return 0;
}

//------------------------------------------------------------------------------
// データアクセス
//------------------------------------------------------------------------------
//***** データ取得
array<Byte>^ FJIsam::DataRecord::getData( array<Byte>^ otBuff )
{
	array<Byte>^ tmpBuff;

	//データ長0なら何もしない
	if( DataLen <= 0 )
	{
		return otBuff;
	}

	//バッファが空
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>(Data->Length);
	}else if( otBuff->Length < DataLen )
	{
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファが小さすぎます。");
		tmpFJIsamException->ErrorCode = 2003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//データコピー
	try
	{
		Buffer::BlockCopy( Data, 0, tmpBuff, 0, DataLen );
	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファへコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 2003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//終了
	return tmpBuff;

}

//***** データ設定
void FJIsam::DataRecord::setData( array<Byte>^ inBuff, Int32 inLength )
{
	FIsam^ tmpParent;

	//-- 引数チェック --//
	if( inBuff == nullptr )				//バッファが空

	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データが未設定です。" );
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inLength <= 0 )					//データ長が不正
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データ長が不正です。" );
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inBuff->Length < inLength )		//データがデータ長より小さい
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データ長が短すぎます。" );
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	//キーに満たない
	if( RecStat != DataRecStat_EN::REC_NEW )
	{
		try
		{
			tmpParent = safe_cast<FIsam^>(ParentFile);
			if( inLength < tmpParent->getKeyLastPos() + 1 )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データ長が短すぎます。" );
				tmpFJIsamException->ErrorCode = 2004;
				tmpFJIsamException->ErrorCode2 = 4;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データへコピー中にエラーが発生しました。", exp);
			tmpFJIsamException->ErrorCode = 2004;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
	}


	//-- データコピー --//
	try
	{
		//データが空(新規作成)
		if( DataLen == 0)
		{
			Data = gcnew array<Byte>(inLength);
			Buffer::BlockCopy( inBuff, 0, Data, 0, inLength );
			DataLen = inLength;

		//データ長が大きくなる(データを作り変え)
		}else if( Data->Length < inLength ){
			Array::Resize( Data, inLength);
			Buffer::BlockCopy( inBuff, 0, Data, 0, inLength );
			DataLen = inLength;

		//データ長が同じ、または小さくなる場合はそのままコピー
		}else{
			Buffer::BlockCopy( inBuff, 0, Data, 0, inLength );
			DataLen = inLength;
		}
		IsHeaderOnly = false;

	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データへコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return;

}

//***** キーデータ取得
array<Byte>^ FJIsam::DataRecord::getKeyData( array<Byte>^ otBuff, KeyInfo^ inKeyInfo,	
								Boolean KeyOnly )
{
	Int32			tmpDataLen;		//返却するデータ長
	array<Byte>^	tmpBuff;

	//-- 引数チェック --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー情報が未設定です。");
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- データ長チェック --//
	if( DataLen <= 0 || IsHeaderOnly == true )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードのデータが空です。");
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;

	}
	//コピーデータ長チェック
	if( KeyOnly == true )
	{
		tmpDataLen = inKeyInfo->getKeyLen();
	}else{
		tmpDataLen = inKeyInfo->getKeyLen() + inKeyInfo->getSrtKeyLen();
	}

	//バッファ作成
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>(tmpDataLen);

	}else if( otBuff->Length < tmpDataLen )
	{
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファが小さすぎます。");
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//-- キーデータコピー --//
	try
	{
		Buffer::BlockCopy( Data, inKeyInfo->getKeyPos() , tmpBuff, 0, inKeyInfo->getKeyLen() );
	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーをコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- キーのみのコピーなら終了　--//
	if( KeyOnly == true ||
		inKeyInfo->getSrtKeyLen() == 0 )
	{
		return tmpBuff;
	}

	//-- ソートキーデータコピー --//
	try
	{
		Buffer::BlockCopy( Data, inKeyInfo->getSrtKeyPos() , tmpBuff, 
							inKeyInfo->getKeyLen(), inKeyInfo->getSrtKeyLen() );
	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ソートキーをコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return tmpBuff;

}
//***** ソートキーデータ取得
array<Byte>^ FJIsam::DataRecord::getSrtKeyData( array<Byte>^ otBuff,					
								KeyInfo^ inKeyInfo )
{
	Int32			tmpDataLen;		//返却するデータ長
	array<Byte>^	tmpBuff;

	//-- 引数チェック --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー情報が未設定です。");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKeyInfo->getSrtKeyLen() == 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ソートキー長が0です。");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}


	//-- データ長チェック --//
	if( DataLen <= 0 || IsHeaderOnly == true )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードのデータが空です。");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;

	}

	//-- キーデータコピー --//
	tmpDataLen = inKeyInfo->getSrtKeyLen();


	//バッファ作成
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>(tmpDataLen);

	}else if( otBuff->Length < tmpDataLen )
	{
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファが小さすぎます。");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//-- ソートキーデータコピー --//
	try
	{
		Buffer::BlockCopy( Data, inKeyInfo->getSrtKeyPos() , tmpBuff, 
							0, inKeyInfo->getSrtKeyLen() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ソートキーをコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return tmpBuff;
}

//------------------------------------------------------------------------------
// レコードデータアクセス
//------------------------------------------------------------------------------
//***** データ書込
array<Byte>^ FJIsam::DataRecord::WriteRecData( array<Byte>^ otBuff,
									   Boolean inHeaderOnly )
{
	Int32 tmpPos;	//オフセット

	if( otBuff == nullptr )
	{
		if( inHeaderOnly == true )
		{
			otBuff = gcnew array<Byte>(DataRecord::getHeaderLength() );
		}else{
			otBuff = gcnew array<Byte>(getRecLength() );
		}
	}
	//-- 引数チェック --//
	if( ( inHeaderOnly == true && otBuff->Length < DataRecord::getHeaderLength() ) ||
		( inHeaderOnly == false && otBuff->Length < getRecLength() ) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"出力バッファが短すぎます。" );
		tmpFJIsamException->ErrorCode = 2007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	tmpPos = 0;
	//--　データ書込 --//
	try
	{
		//データ長
		FJIFuncs::Int32ToArray( DataLen, otBuff, tmpPos );
		tmpPos += sizeof( DataLen );
		
		//削除フラグ
		otBuff[tmpPos] = DelFLG;
		tmpPos++;

		//同一キーフラグ
		otBuff[tmpPos] = EachKeyFLG;
		tmpPos++;

		//前レコード位置
		FJIFuncs::Int64ToArray( PrevRecPos, otBuff, tmpPos );
		tmpPos += sizeof( PrevRecPos );

		//次レコード位置
		FJIFuncs::Int64ToArray( NextRecPos, otBuff, tmpPos );
		tmpPos += sizeof( NextRecPos );

		//二分木親レコード位置
		FJIFuncs::Int64ToArray( ParentRecPos, otBuff, tmpPos );
		tmpPos += sizeof( ParentRecPos );

		//二分木キー小レコード位置
		FJIFuncs::Int64ToArray( LtChildRecPos, otBuff, tmpPos );
		tmpPos += sizeof( LtChildRecPos );

		//二分木キー小レコード位置
		FJIFuncs::Int64ToArray( GtChildRecPos, otBuff, tmpPos );
		tmpPos += sizeof( GtChildRecPos );
	
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードヘッダ部書出に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 2007;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//ヘッダのみのレコード、またはヘッダのみの指定指示ならならココで終了
	if( IsHeaderOnly == true ||
		inHeaderOnly == true)
	{
		return otBuff;
	}

	//データ部
	try
	{
		Buffer::BlockCopy( Data, 0, otBuff, tmpPos, DataLen );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードデータ部書出に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 2007;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return otBuff;
}

//------------------------------------------------------------------------------
// Private関数
//------------------------------------------------------------------------------
//******* データレコード初期化
void FJIsam::DataRecord::Set( 
					array<Byte>^ inBuff,
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec )
{
	Int32 tmpPos;
	FIsam^ tmpParent;

	//-- 引数チェック --//
	if( inBuff == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データリーダーが未設定です。");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	try
	{
		tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^ exp){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAMファイルが正しくありません。", exp);
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inIdxNum < 1 ||
		( inIdxNum == 1 && inIdxRec != nullptr ) ||
		( inIdxNum < IdxNum && inIdxRec == nullptr ) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックス指定が不正です。");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	if( inRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- 管理情報設定 --//
	ParentFile = inParent;
	BuildID = tmpParent->getBuildID();
	RecStat = DataRecStat_EN::REC_OLD;
	IdxNum = inIdxNum;
	IdxRec = inIdxRec;
	RecPos = inRecPos;

	tmpPos = 0;
	//-- 制御部設定 --//
	try
	{
		//データ長
		DataLen = FJIFuncs::ArrayToInt32( inBuff, tmpPos );
		tmpPos += sizeof( DataLen );

		//削除フラグ
		DelFLG = inBuff[tmpPos];
		tmpPos += sizeof(DelFLG);

		if( DelFLG != IS_RECNOR && DelFLG != IS_RECDEL )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"削除フラグが不正です。");
			tmpFJIsamException->ErrorCode = 2008;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
	
		//同一キーフラグ
		EachKeyFLG = inBuff[tmpPos];
		tmpPos += sizeof(EachKeyFLG);

		if( EachKeyFLG > IS_EACHKEYLST )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"同一キーフラグが不正です。");
			tmpFJIsamException->ErrorCode = 2008;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}

		//前レコード位置
		PrevRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( PrevRecPos );

		//次レコード位置
		NextRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( NextRecPos );

		//親レコード位置
		ParentRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( ParentRecPos );

		//子レコード位置(キー小)
		LtChildRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( LtChildRecPos );
		
		//子レコード位置(キー大)
		GtChildRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( GtChildRecPos );

		//-- ヘッダのみ読込時 --// 
		if( inHeaderOnly == true )
		{
			IsHeaderOnly = true;
			Data = nullptr;

			return;

		//--　データ部読込 --//
		}

		//ストリームのデータ長の残りがデータ長より短ければ読み込まない
		if( inBuff->Length - tmpPos < DataLen )
		{
			IsHeaderOnly = true;
			Data = nullptr;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードデータ部のデータが欠落しています。");
			tmpFJIsamException->ErrorCode = 2008;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;

		//データ読込
		}else{
			IsHeaderOnly = false;
			Data = gcnew array<Byte>(DataLen);
			Buffer::BlockCopy( inBuff, tmpPos, Data, 0, DataLen );
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコードデータ部の読込に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;
	}

}
