//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　サブインデックスレコード　クラス　ソース
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
///	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndexRecord	:サブインデックスレコード
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndexRecord.h"		//サブインデックスレコード
#include "SubIndex.h"			//サブインデックス
#include "FJIsam.h"				//Isamクラス
#include "FJIsamFunc.h"			//Isam用関数
#include "FJIsamException.h"	//FJIsam例外クラス

using namespace FJIsam;

//------------------------------------------------------------------------------
//	class	:SubIndexRecord	:サブインデックスレコード
//------------------------------------------------------------------------------
//***** コンストラクタ(新規)
FJIsam::SubIndexRecord::SubIndexRecord( Object^ inParent )
	:	RecStat(DataRecStat_EN::REC_NEW),	//レコード状態
		IsHeaderOnly(false),				//レコード読込状態
		RecPos(0L),							//レコード位置
		DelFLG(IS_RECNOR),					//削除フラグ
		EachKeyFLG(IS_NOEACHKEY),			//同一キーフラグ
		PrevRecPos(0L),						//前レコード位置
		NextRecPos(0L),						//次レコード位置
		ParentRecPos(0L),					//親レコード位置
		LtChildRecPos(0L),					//子レコード位置(キー小)
		GtChildRecPos(0L),					//子レコード位置(キー大)
		DataRecPos( 0L ),					//データレコード本体位置
		KeyData(nullptr),					//キーデータ
		SrtKeyData(nullptr)					//ソートキーデータ
{
	//-- 引数チェック --//
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 4001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- サブインデックスファイルを設定 --//
	try
	{
		 SubIndex^ tmpParent = safe_cast<SubIndex^>(inParent);
		 ParentFile = inParent;
		 BuildID = tmpParent->getBuildID();

	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが不正です。");
		tmpFJIsamException->ErrorCode = 4001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
}

//***** コンストラクタ(既存)
FJIsam::SubIndexRecord::SubIndexRecord( 
					array<Byte>^ inBuff,		
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly )
{
	//--レコード情報セット --//
	Set( inBuff, inParent, inRecPos, inHeaderOnly );
}

//***** コピーコンストラクタ
FJIsam::SubIndexRecord::SubIndexRecord(SubIndexRecord^ inRec )
	:	RecStat(DataRecStat_EN::REC_NEW),		//レコード状態
		RecPos(0L),								//レコード位置
		DelFLG(IS_RECNOR),						//削除フラグ
		EachKeyFLG(IS_NOEACHKEY),				//同一キーフラグ
		PrevRecPos(0L),							//前レコード位置
		NextRecPos(0L),							//次レコード位置
		ParentRecPos(0L),						//親レコード位置
		LtChildRecPos(0L),						//子レコード位置(キー小)
		GtChildRecPos(0L),						//子レコード位置(キー大)
		DataRecPos(0L)							//データレコード本体位置
{
	//-- サブインデックスファイルを設定 --//
	ParentFile = inRec->ParentFile;
	BuildID = inRec->BuildID;

	//--各情報をコピーする
	IsHeaderOnly = inRec->IsHeaderOnly;		//レコード読込状態

	//レコード制御情報
	if( IsHeaderOnly == true )
	{
		KeyData = nullptr;
		SrtKeyData = nullptr;
	}else{
		//データ
		KeyData = gcnew array<Byte>(inRec->KeyData->Length);
		Buffer::BlockCopy( inRec->KeyData, 0, KeyData, 0, KeyData->Length );
		SrtKeyData = gcnew array<Byte>(inRec->SrtKeyData->Length);
		Buffer::BlockCopy( inRec->SrtKeyData, 0, SrtKeyData, 0, SrtKeyData->Length );
	}
}

//------------------------------------------------------------------------------
//***** デストラクタ
FJIsam::SubIndexRecord::~SubIndexRecord()
{
	if( KeyData != nullptr ) delete KeyData;
	if( SrtKeyData != nullptr ) delete SrtKeyData;
}

//------------------------------------------------------------------------------
//サブインデックスレコード情報 *****
//------------------------------------------------------------------------------
//***** レコード位置取得
Int64 FJIsam::SubIndexRecord::getRecPos( void )
{
	return RecPos;
}

//***** Isamファイル取得
Object^ FJIsam::SubIndexRecord::getParentFile( void )
{
	return ParentFile;
}

//***** BuildID取得
Int32 FJIsam::SubIndexRecord::getBuildID( void )
{
	return BuildID;
}

//***** レコード長取得(全体)
Int32 FJIsam::SubIndexRecord::getRecLength( void )
{
	return getHeaderLength() + getDataLength();
}

//***** データ長取得
Int32 FJIsam::SubIndexRecord::getDataLength( void )
{
	if( IsHeaderOnly ) return 0;

	return KeyData->Length + SrtKeyData->Length;
}

//***** レコード位置設定
void FJIsam::SubIndexRecord::setRecPos( Int64 inRecPos )
{
	//サブインデックスファイルなし
	if( ParentFile == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 4009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//ファイル位置が不正
	if( inRecPos < (Int64)SubIndex::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスレコード挿入位置が未設定です。");
		tmpFJIsamException->ErrorCode = 4009;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	RecPos = inRecPos;

	return;
}

//***** Isamファイル設定
void FJIsam::SubIndexRecord::setParentFile( Object^ inParent )
{
	SubIndex^ tmpParent;

	//引数チェック
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 4010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	try
	{
		tmpParent = safe_cast<SubIndex^>(inParent);
	}catch(InvalidCastException^ exp) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが不正です。", exp);
		tmpFJIsamException->ErrorCode = 4010;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( tmpParent->isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルがオープンされていません。");
		tmpFJIsamException->ErrorCode = 4009;
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
Boolean FJIsam::SubIndexRecord::isHeaderOnly( void )
{
	return IsHeaderOnly;
}

//***** レコードヘッダ長取得
Int32 FJIsam::SubIndexRecord::getHeaderLength( void )
{
	return	sizeof( Byte ) +		//削除フラグ
			sizeof( Byte ) +		//同一キーフラグ
			sizeof( Int64 ) +		//前レコード位置
			sizeof( Int64 ) +		//次レコード位置
			sizeof( Int64 ) +		//二分木親レコード位置
			sizeof( Int64 ) +		//二分木キー小レコード位置
			sizeof( Int64 ) +		//二分木キー大レコード位置
			sizeof( Int64 );		//データレコード本体位置

}
//------------------------------------------------------------------------------
// レコード状態
//------------------------------------------------------------------------------
//***** 同一キーフラグ取得
Byte FJIsam::SubIndexRecord::getEachKeyFlg( void )
{
	return EachKeyFLG;	
}

//***** 削除フラグ取得
Byte FJIsam::SubIndexRecord::getDelFlg( void )
{
	return DelFLG;
}

//***** レコードの状態取得
DataRecStat_EN FJIsam::SubIndexRecord::getRecStat( void )
{
	return RecStat;
}


//***** レコード削除
Int32 FJIsam::SubIndexRecord::DeleteRec( void )
{
	DelFLG = IS_RECDEL;
	RecStat = DataRecStat_EN::REC_DEL;

	return 0;
}

//***** 同一キーフラグ設定
Int32 FJIsam::SubIndexRecord::setEachKeyFlg( Byte inEachKeyFlg )
{
	if( inEachKeyFlg <= IS_EACHKEYLST )
	{
		EachKeyFLG = inEachKeyFlg;
		return 0;
	}

	return -1;
}

//レコードの状態設定
Int32 FJIsam::SubIndexRecord::setRecStat( DataRecStat_EN inStat )
{
	//新規レコードに変換
	if( inStat == DataRecStat_EN::REC_NEW )
	{
		ParentFile = nullptr;								//Isamファイル
		BuildID = 0;										//ISAMビルドカウンタ
		RecStat = DataRecStat_EN::REC_NEW;					//レコード状態
		IsHeaderOnly = true;								//レコード読込状態
		RecPos = 0L;										//レコード位置

		DelFLG = IS_RECNOR;									//削除フラグ
		EachKeyFLG = IS_NOEACHKEY;							//同一キーフラグ
		PrevRecPos = 0L;									//前レコード位置
		NextRecPos = 0L;									//次レコード位置
		ParentRecPos = 0L;									//親レコード位置
		LtChildRecPos = 0L;									//子レコード位置(キー小)
		GtChildRecPos = 0L;									//子レコード位置(キー大)

		if( KeyData != nullptr ) delete KeyData;			//キーデータ
		KeyData = nullptr;				
		if( SrtKeyData != nullptr ) delete SrtKeyData;		//ソートキーデータ
		SrtKeyData = nullptr;				
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
Int64 FJIsam::SubIndexRecord::getPrevRecPos( void )
{
	return PrevRecPos;
}

//***** 次レコード位置取得
Int64 FJIsam::SubIndexRecord::getNextRecPos( void )
{
	return NextRecPos;
}

//***** 二分木親レコード位置取得
Int64 FJIsam::SubIndexRecord::getParentRecPos( void )
{
	return ParentRecPos;
}

//***** 二分木キー小レコード位置取得
Int64 FJIsam::SubIndexRecord::getLtChildRecPos( void )
{
	return LtChildRecPos;
}

//***** 二分木キー大レコード位置取得
Int64 FJIsam::SubIndexRecord::getGtChildRecPos( void )
{
	return GtChildRecPos;
}

//***** データレコード本体位置取得
Int64 FJIsam::SubIndexRecord::getDataRecPos( void )
{
	return DataRecPos;
}

//------------------------------------------------------------------------------
//***** 前レコード位置設定
Int32 FJIsam::SubIndexRecord::setPrevRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos < SubIndex::Length() )
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
Int32 FJIsam::SubIndexRecord::setNextRecPos( Int64 inRecPos)
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos < SubIndex::Length() )
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
Int32 FJIsam::SubIndexRecord::setParentRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos < SubIndex::Length() )
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
Int32 FJIsam::SubIndexRecord::setLtChildRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos < SubIndex::Length() )
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
Int32 FJIsam::SubIndexRecord::setGtChildRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos < SubIndex::Length() )
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

//***** 二分木キー大レコード位置設定
Int32 FJIsam::SubIndexRecord::setDataRecPos( Int64 inRecPos )
{
	//Isamファイルがなし
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//ファイル位置が不正
	if( inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	DataRecPos = inRecPos;

	return 0;
}
//------------------------------------------------------------------------------
// データアクセス
//------------------------------------------------------------------------------
//***** キーデータ取得
array<Byte>^ FJIsam::SubIndexRecord::getKeyData( array<Byte>^ otBuff )
{
	array<Byte>^	tmpBuff;

	//データ長0なら何もしない
	if( KeyData == nullptr )
	{
		return otBuff;
	}

	//バッファが空
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>( KeyData->Length );
	}else if( otBuff->Length < KeyData->Length )
	{
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファが小さすぎます。");
		tmpFJIsamException->ErrorCode = 4003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//データコピー
	try
	{
		Buffer::BlockCopy( KeyData, 0, tmpBuff, 0, KeyData->Length );
	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファへコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 4003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//終了
	return tmpBuff;

}

//***** ソートキーデータ取得
array<Byte>^ FJIsam::SubIndexRecord::getSrtKeyData( array<Byte>^ otBuff )
{
	array<Byte>^	tmpBuff;

	//データ長0なら何もしない
	if( SrtKeyData == nullptr )
	{
		return otBuff;
	}

	//バッファが空
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>( SrtKeyData->Length );
	}else if( otBuff->Length < SrtKeyData->Length )
	{
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファが小さすぎます。");
		tmpFJIsamException->ErrorCode = 4004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//データコピー
	try
	{
		Buffer::BlockCopy( SrtKeyData, 0, tmpBuff, 0, SrtKeyData->Length );
	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"バッファへコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode = 4004;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//終了
	return tmpBuff;

}

//***** キーデータ設定
void FJIsam::SubIndexRecord::setKeyData( array<Byte>^ inKey )
{
	SubIndex^ tmpParent = safe_cast<SubIndex^>(ParentFile);

	//-- 引数チェック --//
	if( inKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キーが未設定です。" );
		tmpFJIsamException->ErrorCode =4005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKey->Length < tmpParent->getKeyLen() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"キー長が短すぎます。" );
		tmpFJIsamException->ErrorCode =4005;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- コピー --//
	try
	{
		//データが空なら作成
		if( KeyData == nullptr )
		{
			KeyData = gcnew array<Byte>( tmpParent->getKeyLen() );
		}
		Buffer::BlockCopy( inKey, 0, KeyData, 0, KeyData->Length );

		if( KeyData != nullptr && SrtKeyData != nullptr )
		{
			IsHeaderOnly = false;
		}

	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データへコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode =4005;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return;

}

//***** ソートキーデータ設定
void FJIsam::SubIndexRecord::setSrtKeyData( array<Byte>^ inSrtKey )
{
	SubIndex^ tmpParent = safe_cast<SubIndex^>(ParentFile);

	//-- 引数チェック --//
	//バッファが空
	if( inSrtKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ソートキーが未設定です。" );
		tmpFJIsamException->ErrorCode =4006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inSrtKey->Length < tmpParent->getSrtKeyLen() )
	{ 
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ソートキー長が短すぎます。" );
		tmpFJIsamException->ErrorCode =4006;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- コピー --//
	try
	{
		//データが空なら作成
		if( SrtKeyData == nullptr )
		{
			SrtKeyData = gcnew array<Byte>( tmpParent->getSrtKeyLen() );
		}
		
		Buffer::BlockCopy( inSrtKey, 0, SrtKeyData, 0, SrtKeyData->Length );

		if( KeyData != nullptr && SrtKeyData != nullptr )
		{
			IsHeaderOnly = false;
		}

	}catch( Exception^ exp ){
		//バッファ長が短い
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データへコピー中にエラーが発生しました。", exp);
		tmpFJIsamException->ErrorCode =4006;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return;

}
//------------------------------------------------------------------------------
// データアクセス
//------------------------------------------------------------------------------
//***** データ取得
array<Byte>^ FJIsam::SubIndexRecord::WriteRecData( array<Byte>^ otBuff,
										   Boolean inHeaderOnly )
{
	Int32 tmpPos;
	array<Byte>^ tmpBuff;
	SubIndex^ tmpParent;

	//--　バッファチェック --//
	if( otBuff == nullptr )
	{
		if( inHeaderOnly == true )
		{
			tmpBuff = gcnew array<Byte>(DataRecord::getHeaderLength() );
		}else{
			tmpBuff = gcnew array<Byte>(getRecLength() );
		}
	}else{
		tmpBuff = otBuff;
	}

	//-- 引数チェック --//
	if( ( inHeaderOnly == true && otBuff->Length < SubIndexRecord::getHeaderLength() ) ||
		( inHeaderOnly == false && otBuff->Length < getRecLength() ) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"出力バッファが短すぎます。" );
		tmpFJIsamException->ErrorCode = 4007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//--　データ書込 --//
	tmpPos = 0;

	try
	{
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
		
		//データレコード位置
		FJIFuncs::Int64ToArray( DataRecPos, otBuff, tmpPos );
		tmpPos += sizeof( DataRecPos );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスレコードヘッダ部書出に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 4007;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//ヘッダのみのレコード、またはヘッダのみの出力ならココで終了
	if( IsHeaderOnly == true || 
		inHeaderOnly == true )
	{
		return otBuff;
	}

	//データ部
	try
	{
		tmpParent = safe_cast<SubIndex^>(ParentFile);
		Buffer::BlockCopy( KeyData, 0, otBuff, tmpPos, tmpParent->getKeyLen() );
		tmpPos += tmpParent->getKeyLen();
		Buffer::BlockCopy( SrtKeyData, 0, otBuff, tmpPos, tmpParent->getSrtKeyLen() );
		tmpPos += tmpParent->getSrtKeyLen();
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスレコードキー部書出に失敗しました。", exp );
		tmpFJIsamException->ErrorCode = 4007;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- 正常終了 --//
	return otBuff;
}

//------------------------------------------------------------------------------
// Private関数
//------------------------------------------------------------------------------
//***** レコード設定
void FJIsam::SubIndexRecord::Set( 
					array<Byte>^ inBuff,		
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly )
{
	Int32	  tmpPos;
	SubIndex^ tmpParent;

	//-- 引数チェック --//
	if( inBuff == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"データリーダーが未設定です。");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが未設定です。");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	try
	{
		 tmpParent = safe_cast<SubIndex^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスファイルが不正です。");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inRecPos < SubIndex::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード位置が不正です。");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}
	if( inBuff->Length < SubIndexRecord::getHeaderLength() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"レコード制御部の読込データが欠落しています。");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- 管理部設定ー --//
	ParentFile = inParent;
	BuildID = tmpParent->getBuildID();
	RecStat = DataRecStat_EN::REC_OLD;
	RecPos = inRecPos;

	tmpPos = 0;
	//-- 制御部設定 --//
	try
	{
		//削除フラグ
		DelFLG = inBuff[tmpPos];
		tmpPos += sizeof(DelFLG);

		if( DelFLG != IS_RECNOR && DelFLG != IS_RECDEL )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"削除フラグが不正です。");
			tmpFJIsamException->ErrorCode = 4008;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
	
		//同一キーフラグ
		EachKeyFLG = inBuff[tmpPos];
		tmpPos += sizeof(EachKeyFLG);

		if( EachKeyFLG > IS_EACHKEYLST )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"同一キーフラグが不正です。");
			tmpFJIsamException->ErrorCode = 4008;
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

		//データレコード位置
		DataRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( GtChildRecPos );

		//-- キー部 --//
		if( inHeaderOnly == true )
		{
			IsHeaderOnly = true;
			KeyData = nullptr;
			SrtKeyData = nullptr;

			return;
		}

		//ストリームのデータ長の残りがキーデータ長より短ければ読み込まない
		if( inBuff->Length - tmpPos < 
			tmpParent->getKeyLen() + tmpParent->getSrtKeyLen() )
		{
			IsHeaderOnly = true;
			KeyData = nullptr;
			SrtKeyData = nullptr;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスレコードキー部のデータが欠落しています。");
			tmpFJIsamException->ErrorCode = 4008;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		//キーデータ読込
		}else{
			IsHeaderOnly = false;
			KeyData = gcnew array<Byte>( tmpParent->getKeyLen() );
			Buffer::BlockCopy( inBuff, tmpPos, KeyData, 0, tmpParent->getKeyLen() );
			tmpPos += tmpParent->getKeyLen();
			SrtKeyData = gcnew array<Byte>( tmpParent->getSrtKeyLen() );
			Buffer::BlockCopy( inBuff, tmpPos, SrtKeyData, 0, tmpParent->getSrtKeyLen() );
			tmpPos += tmpParent->getSrtKeyLen();
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"サブインデックスレコードキー部の読込に失敗しました。", exp);
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}
}
