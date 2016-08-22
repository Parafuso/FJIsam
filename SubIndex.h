//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　サブインデックス　ソース
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:サブインデックス
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM定義定数
#include "FJIsamEnum.h"				//Isam用enum定義
#include "KeyInfo.h"				//キー情報

#include "SubIndexRecord.h"			//サブインデックスレコード

//------------------------------------------------------------------------------
//	class	:SubIndex	:サブインデックス
//------------------------------------------------------------------------------
namespace FJIsam
{
	ref class SubIndex
	{
	public :
		SubIndex( Object^ inParent, int inIdxNum, IsamOpenMode inMode );			//コンストラクタ(既存)
		SubIndex( Object^ inParent, int inIdxNum,									//コンストラクタ(新規)
				  KeyInfo^ inKeyInfo );	
		~SubIndex();																//デストラクタ

		//--アクセス(初期)--//
		int	Close(void);															//ファイルクローズ

		//-- サブインデックス状況取得 --//
		String^ getFilePath( String^ otPath );										//ファイルパス取得
		Int32 getBuildID( void );													//BuildID取得
		Boolean isOpen( void );														//ファイルオープンチェック

		//--サブインデックス情報取得--//
		String^	getFID( String^ otFID );											//ファイルID取得
		String^	getVersion(  String^ otVer );										//ISAMバージョン取得
		UInt32 getRecCnt( void );													//レコード数取得
		UInt32 getDelRecCnt( void );												//削除レコード数取得
		UInt32 getAllRecCnt( void );												//全レコード数取得
		UInt32 getKeyCnt( void );													//キー数取得
	
		static Int32	Length( void );												//制御部データ長長取得

		//--情報取得--//
		KeyInfo^	getKeyInfo( void );												//キー情報取得
		String^		getKeyName(String^ otStr );										//キー名取得
		Int32		getKeyLen( void );												//キー長取得
		Int32		getKeyPos( void );												//キー位置取得
		keyType_EN	getKeyType( void );												//キータイプ取得
		Byte		getKeyOrder( void );											//キー順序
		Byte		getKeyUniqueFlg( void );										//ユニークキーフラグ取得
		Int32		getSrtKeyLen( void );											//ソートキー長取得
		Int32		getSrtKeyPos( void );											//ソートキー位置取得
		keyType_EN	getSrtKeyType( void );											//ソートキータイプ取得
		Byte		getSrtKeyOrder( void );											//ソートキー順序

		//-- アクセス(読み出し) --//
		SubIndexRecord^ ReadRecord( array<Byte>^ inKey );							//レコード読込
		SubIndexRecord^ FirstRecord( void );										//最初のレコード読込
		SubIndexRecord^ LastRecord( void  );										//最後のレコード読込
		SubIndexRecord^ NextRecord( SubIndexRecord^ inRec );						//次のデータ読込
		SubIndexRecord^ PrevRecord( SubIndexRecord^ inRec );						//前のデータ読込
		List<SubIndexRecord^>^ ReadRecordWords( String^ inKey );					//レコード読込 文字前方一致

		//-- アクセス(書込) --//
		void AddRecord( SubIndexRecord^ inRec);										//レコード追加
		void UpdateRecord(  array<Byte>^ inKey, array<Byte>^ inNewKey,				//レコード更新
							Int64 inRecPos, Int64 inNewRecPos, Int64 inSubIdxPos );					
		void DeleteRecord(  array<Byte>^ inKey, Int64 inRecPos,						//レコード削除
							 Int64 inSubIdxPos );

	private :

		//*****　データ *****//
		//-- 制御データ --//
		Object^					ParentFile;											//親ISAMファイル
		String^					FilePath;											//使用ファイル名
		Int32					IdxNum;												//インデックス番号
		FileStream^				FStream;											//インデックスファイルハンドル
		Int32					BuildID;											//再編成やクローズ処理を行った場合の識別カウンタ

		array<Byte>^			FileBuff;											//ファイル入出力バッファ

		//---- レコード情報(ファイルデータ） ----//
		//-- サブインデックス制御情報 --//
		String^			FID;														//ファイルID
		String^			ISAMVer;													//ファイル作成ライブラリバージョン

		UInt32			AllRecCnt;													//全レコード数
		UInt32			KeyCnt;														//キー数

		UInt32			RecCnt;														//有効レコード数
		Int64			RecFirstPos;												//リスト先頭レコード位置
		Int64			RecLastPos;													//リスト最終レコード位置
		Int64			RecRootPos;													//二分木ルートレコード位置

		UInt32			DelRecCnt;													//削除レコード数
		Int64 			DelRecFirstPos;												//削除リスト先頭レコード位置
		Int64			DelRecLastPos;												//削除リスト最終レコード位置

		KeyInfo^		SubKeyInfo;													//キー情報

		//*****　Private関数 *****//
		//-- サブインデックスファイル名生成 --//
		String^	CreateFileName( String^ inFname, int inIdxNum );

		//-- ヘッダ操作 --//
		void			ReadHeader( void );											//ファイル書き込みバッファ作成
		void			WriteHeader( void );										//ファイル読み出しデータ取得

		//--アクセス(読込)--//
		SubIndexRecord^ getRecord( Int64 inRecPos );								//レコード読込
		SubIndexRecord^ getRecHeader( Int64 inRecPos );								//レコードヘッダ読出

		//--アクセス(書出)--//
		void WriteRecord( SubIndexRecord^ inRec );									//サブインデックスレコード書込
		void WriteRecHeader( SubIndexRecord^ inRec );								//サブインデックスレコードヘッダ書込

		void FindInsertRecordPos( SubIndexRecord^ inRec,							//サブインデックスレコード挿入位置検索
									SubIndexRecordPos_T^ inAddPos );
		void ChkDeleteRecordPos( SubIndexRecord^ inRec,								//サブインデックスレコード削除位置チェック
									SubIndexRecordPos_T^ inDelPos );
	};

}
