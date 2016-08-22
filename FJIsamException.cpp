//FJIsamException.CPP
//------------------------------------------------------------------------------
//	ISAMアクセスライブラリ　ISAM例外 クラス
//	Version 1.2		Create Date 2006/03/01 .net 2.0 に作り変え
//	Copyright		藤井元雄
//------------------------------------------------------------------------------
//	classes
//		FJIsamException		:例外クラス
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsamException.h"

//------------------------------------------------------------------------------
//	class	:FJIsamException	:例外クラス
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//***** コンストラクタ *****
FJIsam::FJIsamException::FJIsamException()
			: System::Exception(), ErrorCode(0), ErrorCode2(0)
{
};
//***** コンストラクタ *****
FJIsam::FJIsamException::FJIsamException(String^ msg)
			: System::Exception( msg ), ErrorCode(0), ErrorCode2(0)
{
};
//***** コンストラクタ *****
FJIsam::FJIsamException::FJIsamException(String^ msg, Exception^ inner )
			: System::Exception( msg, inner  ), ErrorCode(0), ErrorCode2(0)
{
};
