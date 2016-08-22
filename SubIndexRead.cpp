//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�T�u�C���f�b�N�X�@�\�[�X
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:�T�u�C���f�b�N�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndex.h"			//�T�u�C���f�b�N�X�w�b�_
#include "FJIsamFunc.h"			//Isam�p�֐�
#include "FJIsamException.h"	//��O�N���X


using namespace FJIsam;

//------------------------------------------------------------------------------
//	class	:SubIndex	:�T�u�C���f�b�N�X(�Ǎ��֐�)
//------------------------------------------------------------------------------
//***** ���R�[�h�Ǎ�
SubIndexRecord^ FJIsam::SubIndex::ReadRecord( array<Byte>^ inKey )
{
	SubIndexRecord^	rtRec;			//�Ǎ��T�u�C���f�b�N�X���R�[�h
	Int64			chkRecPos;		//�`�F�b�N�p���R�[�h�ʒu

	Int32			chkKeyFlg;		//�L�[�`�F�b�N����

	//-- �����`�F�b�N --//
	if( inKey == nullptr )		//�L�[�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����w��ł��B");
		tmpFJIsamException->ErrorCode = 3101;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKey->Length != getKeyLen() && 
		getKeyType() != keyType_EN::ISKEY_UNICODE )			//�L�[���`�F�b�N
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����s���ł��B");
		tmpFJIsamException->ErrorCode = 3101;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h��0���Ȃ� nullptr ��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- �T�u�C���f�b�N�X���� --//
	chkRecPos = RecRootPos;

	//�L�[����v����܂Ō���
	while( true )
	{
		//���R�[�h���擾���ăL�[���r
		try
		{
			rtRec = getRecord( chkRecPos );
			if(rtRec == nullptr )
			{
				return nullptr;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"���R�[�h���ǂݍ��݂Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 3101;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		//�L�[��r
		try
		{
			chkKeyFlg = FJIFuncs::KeyCompare( inKey , rtRec->getKeyData(nullptr), 
				SubKeyInfo, IdxNum, KeyCmpOpt_EN::Key );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				L"���R�[�h�̓��e���s���ł��B", exp );
			tmpFJIsamException->ErrorCode = 3101;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//�L�[����v(����I��)
		if( chkKeyFlg == 0 )
		{
			return rtRec;

		//��������L�[�̕���������(���͏��̎q�}�������j
		}else if( chkKeyFlg < 0 )
		{
			chkRecPos = rtRec->getLtChildRecPos();
		//��������L�[�̕����傫��(���͑�̎q�}�������j
		}else {
			chkRecPos = rtRec->getGtChildRecPos();
		}
		delete rtRec;		//�s�v���R�[�h�폜

		//-- ���R�[�h���Ȃ��Ȃ�ΏI�� --//
		if( chkRecPos == 0 )
		{
			return nullptr;
		}
	}
}

//------------------------------------------------------------------------------
//***** �ŏ��̃��R�[�h�Ǎ�
SubIndexRecord^ FJIsam::SubIndex::FirstRecord( void )
{
	SubIndexRecord^	rtRec;		//�ԋp�p���R�[�h

	//-- ���R�[�h���Ȃ���΋��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- ���C���C���f�b�N�X���� --//
	try
	{
		rtRec = getRecord( RecFirstPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�̓Ǎ��Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3102;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	//-- ����I�� --//
	return rtRec;

}

//------------------------------------------------------------------------------
//***** ���R�[�h����(�Ō�̃��R�[�h)
SubIndexRecord^ FJIsam::SubIndex::LastRecord( void )
{
	SubIndexRecord^		rtRec;	//�ԋp�p���R�[�h

	//-- ���R�[�h���Ȃ���΋��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- �T�u�C���f�b�N�X���� --//
	try
	{
		rtRec = getRecord( RecLastPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�̓Ǎ��Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3103;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	
	//-- ����I�� --//
	return rtRec;
}

//------------------------------------------------------------------------------
//***** �����R�[�h����
SubIndexRecord^ FJIsam::SubIndex::NextRecord( SubIndexRecord^ inRec )
{
	SubIndexRecord^	rtRec;			//�Ǎ����R�[�h
	SubIndexRecord^	chkRec;			//�`�F�b�N�p���R�[�h
	Int64			chkRecPos;		//�`�F�b�N�p�����R�[�h�ʒu

	//-- �����`�F�b�N --//
	if( inRec == nullptr )		//���R�[�h�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���̓��R�[�h������܂���B" );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//�ʂ�ISAM�܂��́A�ĕҐ���ăI�[�v��������Ă���΃G���[
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����R�[�h������������܂���B" );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X���猟�� --//
	//�ŐV��Ԃ̃��R�[�h�擾
	try{
		chkRec = getRecHeader( inRec->getRecPos() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h��Ԏ擾�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//�ŐV���R�[�h�`�F�b�N�����̃��R�[�h�ʒu�擾
	chkRecPos = chkRec->getNextRecPos();
	delete chkRec;
	if( chkRecPos == 0L )	
	{
		return nullptr;
	}

	//���̃��R�[�h�擾
	try
	{
		rtRec = getRecord( chkRecPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����R�[�h�Ǎ��Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3104;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return rtRec;
}

//------------------------------------------------------------------------------
//***** �O���R�[�h����
SubIndexRecord^ FJIsam::SubIndex::PrevRecord( SubIndexRecord^ inRec )
{
	SubIndexRecord^	rtRec;			//�Ǎ����R�[�h
	SubIndexRecord^	chkRec;			//�`�F�b�N�p���R�[�h
	Int64			chkRecPos;		//�`�F�b�N�p�O���R�[�h�ʒu

	//-- �����`�F�b�N --//
	if( inRec == nullptr )		//���R�[�h�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���̓��R�[�h������܂���B" );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//�ʂ�ISAM�܂��́A�ĕҐ���ăI�[�v��������Ă���΃G���[
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�O���R�[�h������������܂���B" );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X���猟�� --//
	//�ŐV��Ԃ̃��R�[�h�擾
	try{
		chkRec = getRecHeader( inRec->getRecPos() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h��Ԏ擾�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//�ŐV���R�[�h�`�F�b�N�����̃��R�[�h�ʒu�擾
	chkRecPos = chkRec->getPrevRecPos();
	delete chkRec;
	if( chkRecPos == 0L )	
	{
		return nullptr;
	}

	//���̃��R�[�h�擾
	try
	{
		rtRec = getRecord( chkRecPos );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�O���R�[�h�Ǎ��Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3105;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return rtRec;
}

//------------------------------------------------------------------------------
// Private�֐�
//------------------------------------------------------------------------------
//***** ���R�[�h�Ǎ�
SubIndexRecord^ FJIsam::SubIndex::getRecord( Int64 inRecPos )
{
	SubIndexRecord^	rtRec;				//�Ǎ����R�[�h
	Int32			chkDataLen;			//�`�F�b�N�p���R�[�h�f�[�^��
	Int32			readLen;			//�f�[�^�Ǎ���

	//-- ��ԃ`�F�b�N --//
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�����I�[�v������Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N --//
	if( inRecPos == 0L )					//���R�[�h�ʒu��0�Ȃ���Ԃ��B			
	{
		return nullptr;
	}
	if( inRecPos < SubIndex::Length() )		//���R�[�h�ʒu�����䕔�ɂ���Ȃ�G���[��Ԃ��B
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h�Ǎ� --//
	//�w�b�_���Ǎ�
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, SubIndexRecord::getHeaderLength() );
		if( readLen != SubIndexRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 3106;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//�f�[�^���Ǎ�
	try
	{
		chkDataLen = getKeyLen() + getSrtKeyLen();
		readLen = FStream->Read( FileBuff, SubIndexRecord::getHeaderLength(), chkDataLen );
		if( readLen != chkDataLen )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�L�[��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 3106;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�L�[��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	//-- �f�[�^���R�[�h��` --//
	try
	{
		rtRec = gcnew SubIndexRecord( FileBuff, this, inRecPos, false );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h��`�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3106;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//����I��
	return rtRec;
}

//------------------------------------------------------------------------------
//***** ���R�[�h�w�b�_�Ǐo
SubIndexRecord^ FJIsam::SubIndex::getRecHeader( Int64 inRecPos )
{
	SubIndexRecord^	rtRec;				//�Ǎ����R�[�h
	Int32			readLen;			//�f�[�^�Ǎ���

	//-- ��ԃ`�F�b�N --//
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����I�[�v������Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N --//
	if( inRecPos == 0L )					//���R�[�h�ʒu��0�Ȃ���Ԃ��B			
	{
		return nullptr;
	}
	if( inRecPos < SubIndex::Length() )		//���R�[�h�ʒu�����䕔�ɂ���Ȃ�G���[��Ԃ��B
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h�Ǎ� --//
	//�w�b�_���Ǎ�
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, SubIndexRecord::getHeaderLength() );
		if( readLen != SubIndexRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 3108;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- �f�[�^���R�[�h��` --//
	try
	{
		rtRec = gcnew SubIndexRecord( FileBuff, this, inRecPos, true );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h��`�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3108;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return rtRec;
}