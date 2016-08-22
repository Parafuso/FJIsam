//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　サブインデックスレコード　クラス　ソース
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndexRecord	:サブインデックスレコード
//------------------------------------------------------------------------------
#pragma	once

#include "FJIsamDef.h"			//ISAM定数定義
#include "FJIsamEnum.h"			//Isam用Enum定義

//------------------------------------------------------------------------------
//	class	:SubIndexRecord	:サブインデックスレコード
//------------------------------------------------------------------------------
namespace FJIsam
{
	public ref class SubIndexRecord
	{
	public :
		SubIndexRecord( Object^ inParent );									//コンストラクタ(新規)
		SubIndexRecord( array<Byte>^ inBuff,									//コンストラクタ(既存)
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly );
		SubIndexRecord( SubIndexRecord^ inRec );							//コピーコンストラクタ
		~SubIndexRecord();													//デストラクタ

		//-- サブインデックスレコード情報 --//
		Int64 getRecPos( void );											//レコード位置取得
		Object^ getParentFile( void );										//サブインデックスファイル取得
		Int32 getBuildID( void );											//BuildID取得
		Int32 getRecLength( void );											//レコード長取得(全体)
		Int32 getDataLength( void );										//データ長取得
		void setRecPos( Int64 inRecPos );									//レコード位置設定
		void setParentFile( Object^ inParent );								//サブインデックスファイル設定
		Boolean isHeaderOnly( void );										//レコード読込チェック

		static Int32 getHeaderLength( void );								//レコードヘッダ長取得

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
		Int64 getDataRecPos( void );										//データレコード本体位置取得

		Int32 setPrevRecPos( Int64 inRecPos );								//前レコード位置設定
		Int32 setNextRecPos( Int64 inRecPos);								//次レコード位置設定
		Int32 setParentRecPos( Int64 inRecPos );							//二分木親レコード位置設定
		Int32 setLtChildRecPos( Int64 inRecPos );							//二分木キー小レコード位置設定
		Int32 setGtChildRecPos( Int64 inRecPos );							//二分木キー大レコード位置設定
		Int32 setDataRecPos( Int64 inRecPos );								//データレコード本体位置設定

		//-- データアクセス --//
		array<Byte>^ getKeyData( array<Byte>^ otBuff );						//キーデータ取得
		array<Byte>^ getSrtKeyData( array<Byte>^ otBuff );					//ソートデータ取得
		void setKeyData( array<Byte>^ inBuff );								//キーデータ設定
		void setSrtKeyData( array<Byte>^ inBuff );							//ソートデータ設定

		//-- レコードデータアクセス --//
		array<Byte>^ WriteRecData( array<Byte>^ otBuff,						//レコードデータ書込
						Boolean inHeaderOnly );


	private :
		//====　データ ====//
		//-- 管理情報 --//
		Object^			ParentFile;											//サブインデックスファイル
		Int32			BuildID;											//再編成やクローズ処理を行った場合の識別カウンタ
		DataRecStat_EN	RecStat;											//レコードの状態
		Boolean			IsHeaderOnly;										//読込状態(ヘッダのみ、データ含む)
		Int64			RecPos;												//現在のレコード位置

		//-- レコードヘッダ情報（ファイルデータ）--//
		Byte			DelFLG;												//削除フラグ
		Byte			EachKeyFLG;											//同一キーフラグ
		//リスト用ポインタ
		Int64			PrevRecPos;											//前レコード位置
		Int64			NextRecPos;											//後レコード位置
		//ニ分木用ポインタ
		Int64			ParentRecPos;										//親木レコード位置
		Int64			LtChildRecPos;										//子木小レコード位置
		Int64			GtChildRecPos;										//子木大レコード位置
		//データレコード情報
		Int64			DataRecPos;											//データレコード本体の位置

		//-- レコードデータ --//
		array<Byte>^	KeyData;											//キーデータ
		array<Byte>^	SrtKeyData;											//ソートキーデータ
		
		//-- Private関数 --//
		void Set( array<Byte>^ inReader,											//レコードセット
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly );

	};

	//サブインデックスレコードリレーション検索用構造体
	public ref struct SubIndexRecordPos_T
	{
		SubIndexRecord^ PrevPos;
		SubIndexRecord^ NextPos;
		SubIndexRecord^ ParentPos;
		SubIndexRecord^ LtChildPos;
		SubIndexRecord^ GtChildPos;
		SubIndexRecord^ ExtRecPos;
	};
}
