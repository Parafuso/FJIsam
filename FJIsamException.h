//FJIsamException.h
//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ISAM例外 クラス
//	Version 1.2		Create Date 2006/03/01 .net 2.0 で作成(新規)
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	classes
//		FJIsamException		:例外クラス
//------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
//	class	:FJIsamException	:例外クラス
//------------------------------------------------------------------------------
namespace FJIsam
{
	public ref class FJIsamException :public System::Exception
	{
	public :
		//コンストラクタ
		FJIsamException( );
		FJIsamException(String^ msg);
		FJIsamException(String^ msg, Exception^ inner );

		//メンバー
		int	ErrorCode;
		int	ErrorCode2;
	};
}