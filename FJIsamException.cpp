//FJIsamException.CPP
//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@ISAM��O �N���X
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	classes
//		FJIsamException		:��O�N���X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsamException.h"

//------------------------------------------------------------------------------
//	class	:FJIsamException	:��O�N���X
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//***** �R���X�g���N�^ *****
FJIsam::FJIsamException::FJIsamException()
			: System::Exception(), ErrorCode(0), ErrorCode2(0)
{
};
//***** �R���X�g���N�^ *****
FJIsam::FJIsamException::FJIsamException(String^ msg)
			: System::Exception( msg ), ErrorCode(0), ErrorCode2(0)
{
};
//***** �R���X�g���N�^ *****
FJIsam::FJIsamException::FJIsamException(String^ msg, Exception^ inner )
			: System::Exception( msg, inner  ), ErrorCode(0), ErrorCode2(0)
{
};
