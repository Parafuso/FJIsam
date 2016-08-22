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
List<SubIndexRecord^>^ FJIsam::SubIndex::ReadRecordWords( String^ inKey )
{
	List<SubIndexRecord^>^	rtList;			//�擾���R�[�h���X�g
	SubIndexRecord^		baseRec;			//�擾���R�[�h�x�[�X
	String^				tmpKeyStr;			//�����L�[

	SubIndexRecord^		tmpRec;				//�Ǎ��T�u�C���f�b�N�X���R�[�h
	Int64				chkRecPos;			//�`�F�b�N�p���R�[�h�ʒu

	Int32				chkKeyFlg;			//�L�[�`�F�b�N����

	//-- �����`�F�b�N --//
	if( inKey == nullptr )		//�L�[�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����w��ł��B");
		tmpFJIsamException->ErrorCode = 3701;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	if( getKeyType() != keyType_EN::ISKEY_UNICODE )		//�L�[�^�C�v�`�F�b�N
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�^�C�v��UNICODE�ł͂���܂���B");
		tmpFJIsamException->ErrorCode = 3701;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( inKey->Length > getKeyLen() )					//�L�[���`�F�b�N
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����s���ł��B");
		tmpFJIsamException->ErrorCode = 3701;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h��0���Ȃ� nullptr ��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- �T�u�C���f�b�N�X���� --//
	chkRecPos = RecRootPos;
	tmpKeyStr = FJIFuncs::KeyStringConv( inKey );

	//�L�[����v����܂Ō���
	while( true )
	{
		//���R�[�h���擾���ăL�[���r
		try
		{
			baseRec = getRecord( chkRecPos );
			if(baseRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"���R�[�h�����݂��܂���B" );
				tmpFJIsamException->ErrorCode = 3701;
				tmpFJIsamException->ErrorCode2 = 4;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"���R�[�h�ǂݍ��݂Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
		//�L�[��r
		try
		{
			chkKeyFlg = String::Compare( tmpKeyStr , 0,
					 FJIFuncs::KeyStringConv(
						FJIFuncs::ArrayToString( baseRec->getKeyData(nullptr), 0, getKeyLen() ) ), 0, 
					 tmpKeyStr->Length, StringComparison::Ordinal );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				L"���R�[�h�̓��e���s���ł��B", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}

		//�L�[����v(����I��)
		if( chkKeyFlg == 0 )
		{
			break;

		//��������L�[�̕���������(���͏��̎q�}�������j
		}else if( chkKeyFlg < 0 )
		{
			chkRecPos = baseRec->getLtChildRecPos();
		//��������L�[�̕����傫��(���͑�̎q�}�������j
		}else {
			chkRecPos = baseRec->getGtChildRecPos();
		}
		delete baseRec;		//�s�v���R�[�h�폜

		//-- ���R�[�h���Ȃ��Ȃ�ΏI�� --//
		if( chkRecPos == 0 )
		{
			return nullptr;
		}
	}

	//-- �O��̃��R�[�h�擾 --//
	rtList = gcnew List<SubIndexRecord^>();
	rtList->Add( baseRec );
	tmpRec = baseRec;

	//�O���R�[�h
	while( true )
	{
		if( tmpRec->getPrevRecPos() == 0L )
		{
			break;
		}

		//���R�[�h���擾���ăL�[���r
		try
		{
			tmpRec = PrevRecord( tmpRec );
			if(tmpRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"���R�[�h�����݂��܂���B" );
				tmpFJIsamException->ErrorCode = 3701;
				tmpFJIsamException->ErrorCode2 = 7;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"���R�[�h�ǂݍ��݂Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}

		//�L�[��r
		try
		{
			chkKeyFlg = String::Compare( tmpKeyStr, 0, 
				FJIFuncs::KeyStringConv( 
					FJIFuncs::ArrayToString( tmpRec->getKeyData(nullptr), 0, getKeyLen() ) ), 0,
					tmpKeyStr->Length, StringComparison::Ordinal );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
			L"���R�[�h�̓��e���s���ł��B", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

		//���R�[�h�ǉ�
		if( chkKeyFlg == 0 )
		{
			rtList->Insert( 0, tmpRec );
		}else{
			break;
		}
	}

	//�ヌ�R�[�h
	tmpRec = baseRec;
	while( true )
	{
		if( tmpRec->getNextRecPos() == 0L )
		{
			break;
		}

		//���R�[�h���擾���ăL�[���r
		try
		{
			tmpRec = NextRecord( tmpRec );
			if(tmpRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"���R�[�h�����݂��܂���B" );
				tmpFJIsamException->ErrorCode = 3701;
				tmpFJIsamException->ErrorCode2 = 10;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
				L"���R�[�h�ǂݍ��݂Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 11;
			throw tmpFJIsamException;
		}

		//�L�[��r
		try
		{
			chkKeyFlg = String::Compare( tmpKeyStr, 0, 
				FJIFuncs::KeyStringConv( 
					FJIFuncs::ArrayToString( tmpRec->getKeyData(nullptr), 0, getKeyLen() ) ), 0, 
					tmpKeyStr->Length, StringComparison::Ordinal );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				L"���R�[�h�̓��e���s���ł��B", exp );
			tmpFJIsamException->ErrorCode = 3701;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;
		}

		//���R�[�h�ǉ�
		if( chkKeyFlg == 0 )
		{
			rtList->Add( tmpRec );
		}else{
			break;
		}
	}

	//--����I�� --//

	return rtList;

}