//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�T�u�C���f�b�N�X�@�\�[�X
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:�T�u�C���f�b�N�X�������݊֘A�\�[�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAM�N���X
#include "FJIsamFunc.h"				//ISAM�p�֐�
#include "FJIsamException.h"		//��O�N���X

using namespace FJIsam;
//------------------------------------------------------------------------------
//	class	:SubIndex	:�T�u�C���f�b�N�X(�����֐�)
//------------------------------------------------------------------------------
void FJIsam::SubIndex::WriteRecord( SubIndexRecord^ inRec )
{
	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->isHeaderOnly() == true )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�̃f�[�^��������܂���B" );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃T�u�C���f�b�N�X���R�[�h�͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 3201;
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
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�������ɃG���[���������܂����B",exp );
		tmpFJIsamException->ErrorCode = 3201;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;
}

//------------------------------------------------------------------------------
//***** ���R�[�h�w�b�_������(Private)
void FJIsam::SubIndex::WriteRecHeader( SubIndexRecord^ inRec )
{
	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 3202;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW &&
		!inRec->getParentFile()->Equals( this ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃T�u�C���f�b�N�X���R�[�h�͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 3202;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 3202;
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
		FStream->Write( FileBuff, 0, SubIndexRecord::getHeaderLength() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�������ɃG���[���������܂����B",exp );
		tmpFJIsamException->ErrorCode = 3202;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;
}