// FJIsam.h
//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ Main
//	Version	1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//  Version 1.2.1	Change Date 2011/07/08 FJIFunc::KeyStringConv 修正（'や空白を無視する)
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		FJIsam	:ISAM 本体
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM定数定義
#include "KeyInfo.h"				//キー情報
#include "SubIndex.h"				//サブインデックス
#include "DataRecord.h"				//レコード情報
#include "FJIsamEnum.h"				//Isam用Enum定義

namespace FJIsam
{
	public ref class FIsam
	{
	public:
		FIsam( void );															//コンストラクタ
		FIsam( String^ inFname, Int32 inBufsz, IsamOpenMode inMode );
		~FIsam();																//デストラクタ

		//-- ファイルアクセス 基本 --//
		Int32	Open( String^ inFname, Int32 inBufsz, IsamOpenMode inMode );	//ファイルオープン
		Int32	Close(void);													//ファイルクローズ

		//--　アクセス(作成・管理) --//
		Int32	Create( String^ inFname, String^ inFID,							//ISAM作成
				KeyInfo^ inKeyInfo, String^ inUserInfo,
				Int32 inBufsz,
			    System::Security::AccessControl::FileSecurity^ inFileSecurity);						
		Int32	CreateSubIndex( KeyInfo^ inKeyInfo );							//サブインデックス作成
//		Int32	reorganization(void);											//ISAM再編成

		//-- ISAM状況取得 --//
		String^ getFilePath( String^ otPath );									//ファイルパス取得
		Int32 getBuildID( void );												//BuildID取得
		Boolean isOpen( void );													//ファイルオープンチェック

		//--ISAM情報取得--//
		String^	getFID( String^ otFID );										//ファイルID取得
		String^	getVersion(  String^ otVer );									//ISAMバージョン取得
		String^	getUserInfo( String^ otUInfo );									//ユーザー情報取得
		UInt32 getRecCnt( void );												//レコード数取得
		UInt32 getDelRecCnt( void );											//削除レコード数取得
		UInt32 getAllRecCnt( void );											//全レコード数取得
		UInt32 getKeyCnt( void );												//キー数取得
		Int32 getRecMaxLen( void );												//レコード最大長
		Int32 getRecMinLen( void );												//レコード際小長
		Int32 getIdxCnt( void );												//インデックス数取得
		Int32 getKeyStartPos( void );											//キーの最初の位置取得
		Int32 getKeyLastPos( void );											//キーの最後の位置取得
		static Int32	Length( void );											//制御部データ長取得

		//-- キー情報取得 --//
		Int32		getIdxNum( String^ inKeyName );								//インデックス番号取得
		KeyInfo^	getKeyInfo( Int32 inIdxNum );								//キー情報取得
		String^		getKeyName( Int32 inIdxNum, String^ otStr );				//キー名取得
		Int32		getKeyPos( Int32 inIdxNum );								//キー位置取得
		Int32		getKeyLen( Int32 inIdxNum );								//キー長取得
		keyType_EN	getKeyType( Int32 inIdxNum );								//キータイプ取得	
		Byte		getKeyOrder( Int32 inIdxNum );								//キー順序取得
		Byte		getKeyUniqueFlg( Int32 inIdxNum );							//ユニークキーフラグ取得
		Int32		getSrtKeyPos( Int32 inIdxNum );								//ソートキー位置取得
		Int32		getSrtKeyLen( Int32 inIdxNum );								//ソートキー長取得
		keyType_EN	getSrtKeyType( Int32 inIdxNum );							//ソートキータイプ取得	
		Byte		getSrtKeyOrder( Int32 inIdxNum );							//ソートキー順序取得

		//-- ISAM情報書込 --//
		Int32 setUserInfo( String^ inUInfo );									//ユーザー情報設定

		//--アクセス(読み出し)--//
		DataRecord^ ReadRecord( array<Byte>^ inKey, Int32 inIdxNum );			//レコード読込
		DataRecord^ FirstRecord( Int32 inIdxNum );								//最初のレコード読込
		DataRecord^ LastRecord( Int32 inIdxNum );								//最後のレコード読込
		DataRecord^ NextRecord( DataRecord^ inRec );							//次のデータ読込
		DataRecord^ PrevRecord( DataRecord^ inRec );							//前のデータ読込
		List<DataRecord^>^ ReadRecordWords( String^ inKey, Int32 inIdxNum );	//レコード読込 文字前方一致

		//--アクセス(書き込み)--//
		void AddRecord( DataRecord^ inRec);										//レコード追加
		void UpdateRecord( DataRecord^ inRec );									//レコード上書
		void DeleteRecord( DataRecord^ inRec);									//レコード削除
	
	private :
		//*****　データ *****//
		//-- 制御データ --//
		String^					FilePath;										//使用ファイル名
		FileStream^				FStream;										//ISAMファイルストリーム
		Int32					BuildID;										//再編成やクローズ処理を行った場合の識別カウンタ
		List<SubIndex^>^		SubIdx;											//インデックスファイルハンドル

		array<Byte>^			FileBuff;										//ファイル入出力用バッファ

		//-- キー位置情報
		Int32					KeyStartPos;									//全インデックス中のキーの先頭位置
		Int32					KeyLastPos;										//全インデックス中のキーの最後尾位置

		//-- ファイルデータ --//
		//--- ISAM制御部(ファイルデータ） ---//
		String^			FID;													//ファイルID
		String^			ISAMVer;												//ファイル作成ISAMライブラリバージョン
		
		UInt32			AllRecCnt;												//全レコード数
		UInt32			KeyCnt;													//キー数

		UInt32			RecCnt;													//有効レコード数
		Int64			RecFirstPos;											//リスト先頭レコード位置
		Int64			RecLastPos;												//リスト最終レコード位置
		Int64			RecRootPos;												//二分木ルートレコード位置

		UInt32			DelRecCnt;												//削除レコード数
		Int64			DelRecFirstPos;											//削除リスト先頭レコード位置
		Int64			DelRecLastPos;											//削除リスト最終レコード位置

		Int32			RecMaxLen;												//最大レコード長
		Int32			RecMinLen;												//最小レコード長

		Int32			IdxCnt;													//インデックス数(主キー含む)
		KeyInfo^		ISKeyInfo;												//主キー情報

		String^			UserInfo;												//ユーザー情報

		//*****　Private関数 *****//
		
		//-- ヘッダ操作 --//
		void			ReadHeader( void );										//制御部をファイルから読込
		void			WriteHeader( void );									//制御部をファイルに書出

		//-- ISAM情報確認 --//
		Int32			ChkKeyPositions( void );								//キーの開始、終了位置をチェック
		Int32			CreateBuildID( void );									//BuildID生成

		//--アクセス(読込)--//
		DataRecord^ getRecord( Int64 inRecPos );								//レコード読込
		DataRecord^ getRecord( Int32 inIdxNum, SubIndexRecord^ inIdxRec );		//レコード読込(サブインデックスより)
		DataRecord^ getRecHeader( Int64 inRecPos );								//レコードヘッダ読出


		//--アクセス(書出)--//
		void WriteRecord( DataRecord^ inRec );										//データレコード書込
		void WriteRecHeader( DataRecord^ inRec );									//データレコードヘッダ書込

		void FindInsertRecordPos( DataRecord^ inRec, DataRecordPos_T^ inAddPos );	//レコード挿入位置検索
		void ChkDeleteRecordPos( DataRecord^ inRec, DataRecordPos_T^ inDelPos );	//レコード削除位置チェック
		void WriteMoveRecordPos( DataRecord^ inRec, Int64 inNewRecPos );			//レコード更新＆場所移動チェック	

		//--　アクセス(管理) --//
		/*
		int	reorganizationList( HANDLE tmpHandle,								//ISAM再編成リスト作成＆コピー
							int inRootChkCnt, rootChkPos_T *ioRootChkPos,
							IsamInfo_T *ioIsamInfo );
		int	reorganizationTree( HANDLE tmpHandle,								//ISAM再編成二分木作成
							int inRootChkCnt, rootChkPos_T *ioRootChkPos,
							IsamInfo_T *ioIsamInfo );
		int chkTreePos( HANDLE tmpHandle,										//ISAM再編成二分木ポイントチェック
							int inRootChkCnt, rootChkPos_T *ioRootChkPos,
							IsamInfo_T *ioIsamInfo );
		int chkEachKey( HANDLE tmpHandle, rootChkPos_T *ioRootChkPos );			//同一キーの範囲チェック
		*/

	};
}
