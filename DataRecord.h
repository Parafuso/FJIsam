//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　DataRecord クラス　ヘッダ
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.1.1	create date 2004/06/03
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	classes
//		DataRecord	: データレコード
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM定数定義
#include "KeyInfo.h"				//キー情報
#include "SubIndexRecord.h"			//サブインデックスレコード
#include "FJIsamEnum.h"				//Isam用Enum定義

//------------------------------------------------------------------------------
//	class	:DataRecord	: データレコード
//------------------------------------------------------------------------------
namespace FJIsam
{

	public ref class DataRecord
	{
	public :

		DataRecord();														//コンストラクタ(新規レコード)
		DataRecord( array<Byte>^ inBuff,
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec );
		DataRecord( DataRecord^ inRec );									//コピーコンストラクタ
		~DataRecord();														//デストラクタ
	
		//-- データレコード情報 --//
		Int64 getRecPos( void );											//レコード位置取得
		Object^ getParentFile( void );										//Isamファイル取得
		Int32 getBuildID( void );											//BuildID取得
		Int32 getRecLength( void );											//レコード長取得(全体)
		Int32 getDataLength( void );										//データ長取得
		void setRecPos( Int64 inRecPos );									//レコード位置設定
		void setParentFile( Object^ inParent );
		Boolean isHeaderOnly( void );										//レコード読込チェック

		static Int32 getHeaderLength( void );								//レコードヘッダ長取得

		//-- インデックス情報 --//
		Int32 getIdxNum( void );											//インデックス番号取得
		SubIndexRecord^ getSubIdxRec( void );								//サブインデックスレコード取得

		//-- レコード状態取得 --//
		Byte getDelFlg( void );												//削除フラグ取得
		Byte getEachKeyFlg( void );											//同一キーフラグ取得
		DataRecStat_EN getRecStat( void );									//レコードの状態取得
		Int32 DeleteRec( void );											//レコード削除
		Int32 setEachKeyFlg( Byte inEachKeyFlg );							//同一キーフラグ設定
		Int32 setRecStat( DataRecStat_EN inStat );							//レコードの状態設定

		//-- レコード位置情報 --//
		Int64 getPrevRecPos( void );										//前レコード位置取得
		Int64 getNextRecPos( void );										//次レコード位置取得
		Int64 getParentRecPos( void );										//二分木親レコード位置取得
		Int64 getLtChildRecPos( void );										//二分木キー小レコード位置取得
		Int64 getGtChildRecPos( void );										//二分木キー大レコード位置取得
		
		Int32 setPrevRecPos( Int64 inRecPos );								//前レコード位置設定
		Int32 setNextRecPos( Int64 inRecPos);								//次レコード位置設定
		Int32 setParentRecPos( Int64 inRecPos );							//二分木親レコード位置設定
		Int32 setLtChildRecPos( Int64 inRecPos );							//二分木キー小レコード位置設定
		Int32 setGtChildRecPos( Int64 inRecPos );							//二分木キー大レコード位置設定


		//-- データアクセス --//
		array<Byte>^ getData( array<Byte>^ otBuff );						//データ取得
		void	 setData( array<Byte>^ inBuff, Int32 inLength );			//データ設定
		array<Byte>^ getKeyData( array<Byte>^ otBuff, KeyInfo^ inKeyInfo,	//キーデータ取得
								Boolean KeyOnly );
		array<Byte>^ getSrtKeyData( array<Byte>^ otBuff,					//ソートキーデータ取得
								KeyInfo^ inKeyInfo );
		//-- レコードデータアクセス --//
		array<Byte>^ WriteRecData( array<Byte>^ otWriter,					//レコードデータ書込
									Boolean inHeaderOnly );		
	
	private :
		//====　データ ====//
		//-- 管理情報 --//
		Object^			ParentFile;								//ISAM
		Int32			BuildID;								//再編成やクローズ処理を行った場合の識別カウンタ
		DataRecStat_EN	RecStat;								//レコードの状態
		Boolean			IsHeaderOnly;							//レコード読込状態(ヘッダのみ、データ含む)
		Int64			RecPos;									//現在のレコード位置

		Int32 			IdxNum;									//インデックス番号
		SubIndexRecord^	IdxRec;									//インデックスレコード

		//-- レコードヘッダ情報（ファイルデータ）--//
		Int32			DataLen;								//レコードデータのサイズ
		Byte			DelFLG;									//削除フラグ
		Byte			EachKeyFLG;								//同一キーフラグ
		//リスト用ポインタ
		Int64			PrevRecPos;								//前レコード位置
		Int64			NextRecPos;								//後レコード位置
		//2分木用ポインタ
		Int64			ParentRecPos;							//親木レコード位置
		Int64			LtChildRecPos;							//子木小レコード位置
		Int64			GtChildRecPos;							//子木大レコード位置

		//-- レコードデータ(ファイルデータ） --//
		array<Byte>^	Data;									//データ領域

		//-- private関数 --//
		void Set( array<Byte>^ inBuff,							//データレコード初期化
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec );

	};

	//データレコードリレーション検索情報
	public ref struct DataRecordPos_T
	{
		DataRecord^ PrevPos;				//前レコード
		DataRecord^ NextPos;				//次レコード
		DataRecord^ ParentPos;				//二分木親レコード
		DataRecord^ LtChildPos;				//二分木キー小レコード
		DataRecord^ GtChildPos;				//二分木キー大レコード
		DataRecord^ ExtRecPos;				//データ削除時等、間接的にリレーションがあったレコード
	};

}
