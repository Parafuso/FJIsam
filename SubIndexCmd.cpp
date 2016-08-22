//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�w�b�_
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:�T�u�C���f�b�N�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"				//ISAM�N���X
#include "FJIsamFunc.h"			//ISAM�p�֐�
#include "FJIsamException.h"	//��O�N���X

using namespace FJIsam;
//------------------------------------------------------------------------------
// �w�b�_�����o��(Private)
//------------------------------------------------------------------------------
//***** �w�b�_���Ǎ�
void FJIsam::SubIndex::ReadHeader( void )
{
	Int32 readLen;
	Int32 tmpPos;	//

	//-- �t�@�C���Ǎ� --//
	try
	{
		FStream->Seek( 0, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff , 0, SubIndex::Length() );
		if( readLen != SubIndex::Length() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�t�@�C���Ƀw�b�_��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 3601;
			tmpFJIsamException->ErrorCode2 = 1;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�w�b�_��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 3601;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	tmpPos = 0;

	//-- �o�b�t�@����w�b�_�\���̂Ƀf�[�^���R�s�[ --//
	try
	{
		//�t�@�C��ID
		delete FID;														
		FID = FJIFuncs::ArrayToString( FileBuff, 0, IS_FIDLEN * sizeof( Char ) );
		tmpPos = IS_FIDLEN * sizeof( Char );
	
		//�t�@�C���o�[�W����
		delete ISAMVer;												
		ISAMVer = FJIFuncs::ArrayToString( FileBuff, tmpPos, IS_VERLEN * sizeof( Char ) );
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
	
		//�L�[���
		SubKeyInfo = gcnew KeyInfo();
		tmpPos = SubKeyInfo->Read( FileBuff, tmpPos );
		tmpPos += KeyInfo::Length();

	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X�w�b�_���擾�Ɏ��s���܂����B�B", exp );
		tmpFJIsamException->ErrorCode = 3601;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;
}

//***** �w�b�_��񏑍�
void FJIsam::SubIndex::WriteHeader( void )
{
	Int32 tmpPos;

	//�I�t�Z�b�g������
	tmpPos = 0;

	try
	{
		//�t�@�C��ID
		FJIFuncs::StringToArray( FID, FileBuff, tmpPos, IS_FIDLEN * sizeof( Char ) );
		tmpPos += IS_FIDLEN * sizeof( Char );

		//�t�@�C���o�[�W����
		FJIFuncs::StringToArray( ISAMVer, FileBuff, tmpPos, IS_FIDLEN * sizeof( Char ) );
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

		//�L�[���
		tmpPos = SubKeyInfo->Write( FileBuff, tmpPos );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException(L"�T�u�C���f�b�N�X�w�b�_��񏑍��Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 1017;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//--�t�@�C���֏��� --//
	try
	{
		Int32 tst = SubIndex::Length();
		FStream->Seek( 0, SeekOrigin::Begin );
		FStream->Write( FileBuff, 0, SubIndex::Length() );
	
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X�w�b�_���̃t�@�C���ւ̏����Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 1602;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- ����I�� --//
	return;

}

//------------------------------------------------------------------------------
//�T�u�C���f�b�N�X�t�@�C��������(Private)
//------------------------------------------------------------------------------
//***** �T�u�C���f�b�N�X�t�@�C��������
String^	FJIsam::SubIndex::CreateFileName( String^ inFname, int inIdxNum )
{
	//-- �����`�F�b�N --//
	if( inFname == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C���������ݒ�ł��B" );
		tmpFJIsamException->ErrorCode = 3603;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//�T�u�C���f�b�N�X���쐬
	return  String::Concat( Path::GetDirectoryName( inFname ), L"\\", 
					Path::GetFileNameWithoutExtension( inFname ),
					String::Format( L".fs{0:d2}", inIdxNum ) );

}

