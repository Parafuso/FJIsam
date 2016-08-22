//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@���������\�[�X
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		FJIsam	: FJIsam ���������֌W�\�[�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAM�N���X
#include "FJIsamFunc.h"				//ISAM�p�֐�
#include "FJIsamException.h"		//��O�N���X

using namespace FJIsam;
//------------------------------------------------------------------------------
//***** ���R�[�h��������(Private)
void FJIsam::FIsam::WriteRecord( DataRecord^ inRec )
{
	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->isHeaderOnly() == true )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�̃f�[�^��������܂���B" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃��R�[�h�͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	try
	{
		//�����o�b�t�@�쐬
		inRec->WriteRecData( FileBuff, false );

		//���R�[�h�ʒu�t��
		FStream->Seek( inRec->getRecPos(), SeekOrigin::Begin );
		//���R�[�h����

		FStream->Write( FileBuff, 0, inRec->getRecLength() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�������ɃG���[���������܂����B",exp );
		tmpFJIsamException->ErrorCode = 1201;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;
}

//------------------------------------------------------------------------------
//***** ���R�[�h�w�b�_������(Private)
void FJIsam::FIsam::WriteRecHeader( DataRecord^ inRec )
{
	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃��R�[�h�͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	try
	{
		//�����o�b�t�@�쐬
		inRec->WriteRecData( FileBuff, true );

		//���R�[�h�ʒu�t��
		FStream->Seek( inRec->getRecPos(), SeekOrigin::Begin );
		//���R�[�h����
		FStream->Write( FileBuff, 0, DataRecord::getHeaderLength() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�������ɃG���[���������܂����B",exp );
		tmpFJIsamException->ErrorCode = 1202;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;
}
