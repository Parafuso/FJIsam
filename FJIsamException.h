//FJIsamException.h
//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@ISAM��O �N���X
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ō쐬(�V�K)
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	classes
//		FJIsamException		:��O�N���X
//------------------------------------------------------------------------------
#pragma once

//------------------------------------------------------------------------------
//	class	:FJIsamException	:��O�N���X
//------------------------------------------------------------------------------
namespace FJIsam
{
	public ref class FJIsamException :public System::Exception
	{
	public :
		//�R���X�g���N�^
		FJIsamException( );
		FJIsamException(String^ msg);
		FJIsamException(String^ msg, Exception^ inner );

		//�����o�[
		int	ErrorCode;
		int	ErrorCode2;
	};
}