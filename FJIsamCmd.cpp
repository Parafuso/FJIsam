//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�w�b�_
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		FJIsam :	FJIsam ���������A�쐬�A��񕔃\�[�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndex.h"			//�T�u�C���f�b�N�X�w�b�_
#include "FJIsam.h"				//ISAM�t�@�C��
#include "FJIsamFunc.h"			//ISAM�p�֐�
#include "FJIsamException.h"	//FJIsam��O�N���X

using namespace FJIsam;
using namespace System;
using namespace System::IO;

//------------------------------------------------------------------------------
//  class	:FJIsam	:ISAM�@���C���N���X
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// �w�b�_�����o��(Private)
//------------------------------------------------------------------------------
//***** �w�b�_���Ǎ�
void FJIsam::FIsam::ReadHeader( void )
{
	Int32 readLen;
	Int32 tmpPos;			//�o�b�t�@�Ǎ��ʒu

	//-- �t�@�C���Ǎ� --//
	try
	{
		FStream->Seek( 0, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff , 0, FIsam::Length() );
		if( readLen != FIsam::Length() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C���Ƀw�b�_��񂪂���܂���B");
			tmpFJIsamException->ErrorCode = 1601;
			tmpFJIsamException->ErrorCode2 = 1;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�w�b�_��񂪓ǂݍ��߂܂���B", exp);
		tmpFJIsamException->ErrorCode = 1601;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- �o�b�t�@����w�b�_�\���̂Ƀf�[�^���R�s�[ --//
	tmpPos = 0;
	try
	{
		//�t�@�C��ID
		delete FID;														
		FID = FJIFuncs::ArrayToString( FileBuff, 0, IS_FIDLEN * sizeof(Char) );
		tmpPos = IS_FIDLEN * sizeof( Char );
	
		//�t�@�C���o�[�W����
		delete ISAMVer;												
		ISAMVer = FJIFuncs::ArrayToString( FileBuff, tmpPos, IS_VERLEN * sizeof(Char) );
		tmpPos += IS_VERLEN * sizeof( Char );

		//�S���R�[�h��
		AllRecCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos);
		tmpPos += sizeof(AllRecCnt);

		//�L�[��
		KeyCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos );		
		tmpPos += sizeof(KeyCnt);

		//�L�����R�[�h��
		RecCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos );		
		tmpPos += sizeof(RecCnt);

		//���X�g�擪
		RecFirstPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(RecFirstPos);

		//���X�g�Ō�
		RecLastPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(RecLastPos);

		//�񕪖ؖ؃��[�g
		RecRootPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(RecRootPos);					
			
		//�폜���R�[�h��
		DelRecCnt = FJIFuncs::ArrayToUInt32( FileBuff, tmpPos );
		tmpPos += sizeof(DelRecCnt);

		//���X�g�擪
		DelRecFirstPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(DelRecFirstPos);

		//���X�g�Ō�
		DelRecLastPos = FJIFuncs::ArrayToInt64( FileBuff, tmpPos );
		tmpPos += sizeof(DelRecLastPos);

		//���R�[�h�ő咷
		RecMaxLen = FJIFuncs::ArrayToInt32( FileBuff, tmpPos );
		tmpPos += sizeof(RecMaxLen);										

		//���R�[�h�ŏ���
		RecMinLen = FJIFuncs::ArrayToInt32( FileBuff, tmpPos );
		tmpPos += sizeof(RecMinLen);

		//�C���f�b�N�X��
		IdxCnt = FJIFuncs::ArrayToInt32( FileBuff, tmpPos );
		tmpPos += sizeof(IdxCnt);								
	
		//�L�[���
		ISKeyInfo = gcnew KeyInfo();
		tmpPos = ISKeyInfo->Read( FileBuff, tmpPos );

		//���[�U�[���
		delete UserInfo;
		UserInfo = FJIFuncs::ArrayToString( FileBuff, tmpPos, IS_UINFLEN * sizeof(Char) );
		tmpPos += IS_UINFLEN * sizeof( Char );
		
	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAM�w�b�_���擾�Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 1601;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;
}

//------------------------------------------------------------------------------
//***** �w�b�_��񏑍�
void FJIsam::FIsam::WriteHeader( void )
{
	Int32 tmpPos;	

	//�I�t�Z�b�g������
	tmpPos = 0;

	try
	{
		//�t�@�C��ID
		FJIFuncs::StringToArray( FID, FileBuff, tmpPos, IS_FIDLEN * sizeof(Char)  );
		tmpPos += IS_FIDLEN * sizeof( Char );

		//�t�@�C���o�[�W����
		FJIFuncs::StringToArray( ISAMVer, FileBuff, tmpPos, IS_FIDLEN * sizeof(Char) );
		tmpPos += IS_FIDLEN * sizeof( Char );

		//�S���R�[�h��
		FJIFuncs::UInt32ToArray( AllRecCnt, FileBuff, tmpPos );
		tmpPos += sizeof( AllRecCnt );

		//�L�[��
		FJIFuncs::UInt32ToArray( KeyCnt, FileBuff, tmpPos );
		tmpPos += sizeof( KeyCnt );

		//�L�����R�[�h��
		FJIFuncs::UInt32ToArray( RecCnt, FileBuff, tmpPos );
		tmpPos += sizeof( RecCnt );

		//���X�g�擪
		FJIFuncs::Int64ToArray( RecFirstPos, FileBuff, tmpPos );
		tmpPos += sizeof( RecFirstPos );

		//���X�g�Ō�
		FJIFuncs::Int64ToArray( RecLastPos, FileBuff, tmpPos );
		tmpPos += sizeof( RecLastPos );

		//�񕪖ؖ؃��[�g
		FJIFuncs::Int64ToArray( RecRootPos, FileBuff, tmpPos );
		tmpPos += sizeof( RecRootPos );

		//�폜���R�[�h��
		FJIFuncs::UInt32ToArray( DelRecCnt, FileBuff, tmpPos );
		tmpPos += sizeof( DelRecCnt );

		//���X�g�擪
		FJIFuncs::Int64ToArray( DelRecFirstPos, FileBuff, tmpPos );
		tmpPos += sizeof( DelRecFirstPos );

		//���X�g�Ō�
		FJIFuncs::Int64ToArray( DelRecLastPos, FileBuff, tmpPos );
		tmpPos += sizeof( DelRecLastPos );

		//���R�[�h�ő咷
		FJIFuncs::Int32ToArray( RecMaxLen, FileBuff, tmpPos );
		tmpPos += sizeof( RecMaxLen );

		//���R�[�h�ŏ���
		FJIFuncs::Int32ToArray( RecMinLen, FileBuff, tmpPos );
		tmpPos += sizeof( RecMinLen );

		//�C���f�b�N�X��
		FJIFuncs::Int32ToArray( IdxCnt, FileBuff, tmpPos );
		tmpPos += sizeof( IdxCnt );

		//�L�[���
		tmpPos = ISKeyInfo->Write( FileBuff, tmpPos );

		//���[�U�[���
		FJIFuncs::StringToArray( UserInfo, FileBuff, tmpPos, IS_UINFLEN * sizeof(Char) );
		tmpPos += IS_UINFLEN * sizeof( Char );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ISAM�w�b�_��񏑍��Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 1602;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//--�t�@�C���֏���
	try
	{
		FStream->Seek( 0, SeekOrigin::Begin );
		FStream->Write( FileBuff, 0, FIsam::Length() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"ISAM�w�b�_���̃t�@�C���ւ̏����Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 1602;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;

}

//------------------------------------------------------------------------------		
// ISAM���m�F
//------------------------------------------------------------------------------
//***** �L�[�̊J�n�A�I���ʒu���`�F�b�N
Int32 FJIsam::FIsam::ChkKeyPositions( void )
{
	Int32		tmpStartPos;			//�S�C���f�b�N�X���̃L�[�擪�ʒu
	Int32		tmpLastPos;				//�S�C���f�b�N�X���̃L�[�Ō���ʒu

	//-- �t�@�C�����I�[�v������Ă��Ȃ���΃G���[ --//
	if( !isOpen() ) return -1;

	//�S�C���f�b�N�X���̃L�[�̐擪�ʒu�ƍŌ���̈ʒu�𒲍�
	try
	{
		
		KeyStartPos = getKeyPos( 1 );
		KeyLastPos = KeyStartPos + getKeyLen( 1 ) - 1;
		for( Int32 i = 2; i < IdxCnt; i++ )
		{
			tmpStartPos = getKeyPos( i );
			tmpLastPos = tmpStartPos + getKeyLen( i ) - 1;
			
			if( KeyStartPos > tmpStartPos ) KeyStartPos = tmpStartPos;
			if( KeyLastPos > tmpLastPos ) KeyLastPos = tmpLastPos;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�ʒu�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1603;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return 0;
}

//***** BuildID����
Int32 FJIsam::FIsam::CreateBuildID( void )
{
	DateTime tmpDate;
	Int32	tmpBuildID;

	while( true )
	{
		tmpDate = DateTime::Now;
		
		tmpBuildID = tmpDate.Year + tmpDate.Month + tmpDate.Day +
			tmpDate.Hour + tmpDate.Minute + tmpDate.Millisecond;

		if( tmpBuildID != BuildID )
		{
			BuildID = tmpBuildID;
			return 0;
		}
	}
}									
