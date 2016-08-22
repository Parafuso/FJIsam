//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�w�b�_
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		FJIsam : FJIsam �Ǎ������֌W�\�[�X
//------------------------------------------------------------------------------
#pragma once 

#include "stdafx.h"
#include "FJIsam.h"
#include "FJIsamFunc.h"
#include "FJIsamException.h"

using namespace FJIsam;
using namespace System;
using namespace System::IO;
//------------------------------------------------------------------------------
//	class	:FJIsam	:FJIsam(�Ǎ��֐�)
//------------------------------------------------------------------------------
//***** ���R�[�h�Ǎ�
DataRecord^ FJIsam::FIsam::ReadRecord( array<Byte>^ inKey, Int32 inIdxNum )
{
	DataRecord^		rtRec;			//�擾���R�[�h
	Int64			chkRecPos;		//�`�F�b�N�p���R�[�h�ʒu
	SubIndex^		chkSubIdx;		//�`�F�b�N�p�T�u�C���f�b�N�X
	SubIndexRecord^	chkIdxRec;		//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h

	Int32			chkKeyFlg;		//�L�[�`�F�b�N����

	//-- �����`�F�b�N --//
	if( inKey == nullptr )		//�L�[�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����w��ł��B");
		tmpFJIsamException->ErrorCode = 1101;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inIdxNum < 1 ||			//�C���f�b�N�X�ُ�
		inIdxNum >IdxCnt )			
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�̎w�肪�s���ł��B");
		tmpFJIsamException->ErrorCode = 1101;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inKey->Length != getKeyLen( inIdxNum ) && 
		getKeyType( inIdxNum ) != keyType_EN::ISKEY_UNICODE )			//�L�[���`�F�b�N
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����s���ł��B");
		tmpFJIsamException->ErrorCode = 1101;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h��0���Ȃ� nullptr ��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- ���C���C���f�b�N�X���� --//
	if( inIdxNum == 1 )
	{
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
				tmpFJIsamException->ErrorCode = 1101;
				tmpFJIsamException->ErrorCode2 = 4;
				throw tmpFJIsamException;
			}
			//�L�[��r
			try
			{
				chkKeyFlg = FJIFuncs::KeyCompare( inKey , rtRec->getData(nullptr), 
								ISKeyInfo, 1, KeyCmpOpt_EN::Key );
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"���R�[�h�̓��e���s���ł��B", exp );
				tmpFJIsamException->ErrorCode = 1101;
				tmpFJIsamException->ErrorCode2 = 5;
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

	//-- �T�u�C���f�b�N�X���� --//
	}else{
		//�T�u�C���f�b�N�X�擾
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
		//�T�u�C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}

		//�ΏۃT�u�C���f�b�N�X���R�[�h�擾
		try{
			chkIdxRec = chkSubIdx->ReadRecord( inKey );
			if( chkIdxRec == nullptr )	//�Y�����R�[�h�Ȃ�
			{
				return nullptr;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}

		//�f�[�^���R�[�h
		try
		{
			rtRec = getRecord( inIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�L�[�ɑ΂��郌�R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1101;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}

		//-- ����I�� --//
		return rtRec;

	}
}

//------------------------------------------------------------------------------
//***** �擪���R�[�h�Ǎ�
DataRecord^ FJIsam::FIsam::FirstRecord( Int32 inIdxNum )
{
	DataRecord^		rtRec;		//�ԋp�p���R�[�h
	SubIndex^		chkSubIdx;	//�`�F�b�N�p�T�u�C���f�b�N�X
	SubIndexRecord^	chkIdxRec;	//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h

	//-- �����`�F�b�N --//
	if( inIdxNum < 1 ||
		inIdxNum > IdxCnt )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�ԍ����s���ł��B" );
		tmpFJIsamException->ErrorCode = 1102;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h���Ȃ���΋��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- ���C���C���f�b�N�X���� --//
	if( inIdxNum == 1 )
	{
		try
		{
			rtRec = getRecord( RecFirstPos );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�̓Ǎ��Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		//-- ����I�� --//
		return rtRec;

	//-- �T�u�C���f�b�N�X���� --//
	}else{
		//�T�u�C���f�b�N�X�擾
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		//�T�u�C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
		//�ΏۃT�u�C���f�b�N�X���R�[�h�擾
		try
		{
			chkIdxRec = chkSubIdx->FirstRecord();
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}

		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//�f�[�^���R�[�h�擾
		try
		{
			rtRec = getRecord( inIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�L�[�ɑ΂��郌�R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1102;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//-- ����I�� --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
//***** �Ōヌ�R�[�h����
DataRecord^ FJIsam::FIsam::LastRecord( Int32 inIdxNum )
{
	DataRecord^		rtRec;		//�ԋp�p���R�[�h
	SubIndex^		chkSubIdx;	//�`�F�b�N�p�T�u�C���f�b�N�X
	SubIndexRecord^	chkIdxRec;	//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h

	//-- �����`�F�b�N --//
	if( inIdxNum < 1 ||
		inIdxNum > IdxCnt )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�ԍ����s���ł��B" );
		tmpFJIsamException->ErrorCode = 1103;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h���Ȃ���΋��Ԃ� --//
	if( RecCnt == 0 )
	{
		return nullptr;
	}

	//-- ���C���C���f�b�N�X���� --//
	if( inIdxNum == 1 )
	{
		try
		{
			rtRec = getRecord( RecLastPos );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�̓Ǎ��Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		//-- ����I�� --//
		return rtRec;

	//-- �T�u�C���f�b�N�X���� --//
	}else{
		//�T�u�C���f�b�N�X�擾
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		//�T�u�C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
		//�ΏۃT�u�C���f�b�N�X���R�[�h�擾
		try
		{
			chkIdxRec = chkSubIdx->LastRecord();
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}

		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//�f�[�^���R�[�h�擾
		try
		{
			rtRec = getRecord( inIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�L�[�ɑ΂��郌�R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1103;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}

		//-- ����I�� --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
//***** �����R�[�h����
DataRecord^ FJIsam::FIsam::NextRecord( DataRecord^ inRec )
{
	Int32			chkIdxNum;		//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h
	DataRecord^		chkRec;			//�`�F�b�N�p���R�[�h
	Int64			chkRecPos;		//�`�F�b�N�p�����R�[�h�ʒu
	SubIndex^		chkSubIdx;		//�`�F�b�N�p�T�u�C���f�b�N�X
	SubIndexRecord^ chkIdxRec;		//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h
	DataRecord^		rtRec;			//�Ǎ����R�[�h

	//-- �����`�F�b�N --//
	if( inRec == nullptr )		//���R�[�h�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���̓��R�[�h������܂���B" );
		tmpFJIsamException->ErrorCode = 1104;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//�ʂ�ISAM�܂��́A�ĕҐ���ăI�[�v��������Ă���΃G���[
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����R�[�h������������܂���B" );
		tmpFJIsamException->ErrorCode = 1104;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	chkIdxNum = inRec->getIdxNum();

	if( chkIdxNum < 1 ||			//�C���f�b�N�X�ԍ����s��
		chkIdxNum > IdxCnt )	
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����C���f�b�N�X�ԍ����s���ł��B" );
		tmpFJIsamException->ErrorCode = 1104;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ���C���C���f�b�N�X���猟�� --//
	if( chkIdxNum == 1 )
	{
		//�ŐV��Ԃ̃��R�[�h�擾
		try{

			chkRec = getRecHeader( inRec->getRecPos() );

		}catch( Exception^ exp )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h��Ԏ擾�Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 4;
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
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		return rtRec;

	//-- �T�u�C���f�b�N�X���猟�� --//
	}else{
		chkIdxRec = inRec->getSubIdxRec();
		//�T�u�C���f�b�N�X���R�[�h�擾
		if( chkIdxRec == nullptr )		//�T�u�C���f�b�N�X���R�[�h�Ȃ�
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����T�u�C���f�b�N�X���R�[�h������܂���ł��B" );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		try
		{
			chkSubIdx = SubIdx[chkIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
		//�T�u�C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", chkIdxNum) );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}
		//�ΏۃT�u�C���f�b�N�X���R�[�h�擾
		try
		{
			chkIdxRec = chkSubIdx->NextRecord( chkIdxRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//�f�[�^���R�[�h�擾
		try
		{
			rtRec = getRecord( chkIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�L�[�ɑ΂��郌�R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1104;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

		//-- ����I�� --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
//***** �O���R�[�h����
DataRecord^ FJIsam::FIsam::PrevRecord( DataRecord^ inRec )
{
	Int32			chkIdxNum;		//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h
	DataRecord^		chkRec;			//�`�F�b�N�p���R�[�h
	Int64			chkRecPos;		//�`�F�b�N�p�O���R�[�h�ʒu
	SubIndex^		chkSubIdx;		//�`�F�b�N�p�T�u�C���f�b�N�X
	SubIndexRecord^ chkIdxRec;		//�`�F�b�N�p�T�u�C���f�b�N�X���R�[�h
	DataRecord^		rtRec;			//�Ǎ����R�[�h

	//-- �����`�F�b�N --//
	if( inRec == nullptr )		//���R�[�h�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���̓��R�[�h������܂���B" );
		tmpFJIsamException->ErrorCode = 1105;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( this->Equals( inRec->getParentFile() ) != true ||	//�ʂ�ISAM�܂��́A�ĕҐ���ăI�[�v��������Ă���΃G���[
		inRec->getBuildID() != BuildID )				
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�O���R�[�h������������܂���B" );
		tmpFJIsamException->ErrorCode = 1105;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	chkIdxNum = inRec->getIdxNum();

	if( chkIdxNum < 1 ||			//�C���f�b�N�X�ԍ����s��
		chkIdxNum > IdxCnt )	
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����C���f�b�N�X�ԍ����s���ł��B" );
		tmpFJIsamException->ErrorCode = 1105;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ���C���C���f�b�N�X���猟�� --//
	if( chkIdxNum == 1 )
	{
		//�ŐV��Ԃ̃��R�[�h�擾
		try{

			chkRec = getRecHeader( inRec->getRecPos() );

		}catch( Exception^ exp )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h��Ԏ擾�Ɏ��s���܂����B", exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 4;
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
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		return rtRec;

	//-- �T�u�C���f�b�N�X���猟�� --//
	}else{
		chkIdxRec = inRec->getSubIdxRec();
		//�T�u�C���f�b�N�X���R�[�h�擾
		if( chkIdxRec == nullptr )		//�T�u�C���f�b�N�X���R�[�h�Ȃ�
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�����T�u�C���f�b�N�X���R�[�h������܂���ł��B" );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}

		try
		{
			chkSubIdx = SubIdx[chkIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
		//�T�u�C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", chkIdxNum) );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}
		//�ΏۃT�u�C���f�b�N�X���R�[�h�擾
		try
		{
			chkIdxRec = chkSubIdx->NextRecord( chkIdxRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
		if( chkIdxRec == nullptr )
		{
			return nullptr;
		}
		//�f�[�^���R�[�h�擾
		try
		{
			rtRec = getRecord( chkIdxNum, chkIdxRec );
		}catch( Exception^ exp ){
			delete chkIdxRec;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�L�[�ɑ΂��郌�R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", chkIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1105;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

		//-- ����I�� --//
		return rtRec;
	}
}

//------------------------------------------------------------------------------
// Private�֐�
//------------------------------------------------------------------------------
//***** ���R�[�h�Ǎ�
DataRecord^ FJIsam::FIsam::getRecord( Int64 inRecPos )
{
	DataRecord^	rtRec;				//�Ǎ����R�[�h
	Int32		chkDatLen;			//�`�F�b�N�p���R�[�h�f�[�^��
	Int32		readLen;			//�f�[�^�Ǎ���

	//-- ��ԃ`�F�b�N
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����I�[�v������Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N
	if( inRecPos == 0L )					//���R�[�h�ʒu��0�Ȃ���Ԃ��B			
	{
		return nullptr;
	}
	if( inRecPos < FIsam::Length() )		//���R�[�h�ʒu�����䕔�ɂ���Ȃ�G���[��Ԃ��B
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h�Ǎ� --//
	//�w�b�_���Ǎ�
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff , 0, DataRecord::getHeaderLength() );
		if( readLen != DataRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 1106;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//���R�[�h�̃f�[�^���`�F�b�N
	chkDatLen = FJIFuncs::ArrayToInt32( FileBuff, 0 );
	if( chkDatLen <= 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^����ł��B" );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//�f�[�^���Ǎ�
	try
	{
		readLen = FStream->Read( FileBuff, DataRecord::getHeaderLength(), chkDatLen );
		if( readLen != chkDatLen )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 1106;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//-- �f�[�^���R�[�h��` --//
	try
	{
		rtRec = gcnew DataRecord( FileBuff, this, inRecPos, false, 1, nullptr );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h��`�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 1106;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//����I��
	return rtRec;
}

//***** ���R�[�h�Ǎ�(�T�u�C���f�b�N�X���)
DataRecord^ FJIsam::FIsam::getRecord( Int32 inIdxNum, SubIndexRecord^ inIdxRec )
{
	DataRecord^	rtRec;				//�Ǎ����R�[�h
	Int64		chkRecPos;			//�`�F�b�N�p���R�[�h�ʒu
	Int32		chkDatLen;			//�`�F�b�N�p���R�[�h�f�[�^��
	Int32		readLen;			//�f�[�^�Ǎ���

	//-- ��ԃ`�F�b�N
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����I�[�v������Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N
	if( inIdxNum < 1 &&
		inIdxNum > IdxCnt )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�ԍ����s���ł��B" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inIdxRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h���w�肳��Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h�ʒu�`�F�b�N
	chkRecPos = inIdxRec->getDataRecPos();

	if( chkRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h�Ǎ� --//
	//�w�b�_���Ǎ�
	try
	{
		FStream->Seek( chkRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, DataRecord::getHeaderLength() );
		if( readLen != DataRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 1107;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	chkDatLen = FJIFuncs::ArrayToInt32( FileBuff, 0 );
	if( chkDatLen < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^����ł��B" );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//�f�[�^���Ǎ�
	try
	{
		readLen = FStream->Read( FileBuff, DataRecord::getHeaderLength(), chkDatLen );
		if( readLen != chkDatLen )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 1107;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}

	//-- �f�[�^���R�[�h��` --//
	try
	{
		rtRec = gcnew DataRecord( FileBuff, this, inIdxRec->getDataRecPos(), false, inIdxNum, inIdxRec );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h��`�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 1107;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;
	}

	//����I��
	return rtRec;
}

//***** ���R�[�h�w�b�_�Ǐo
DataRecord^ FJIsam::FIsam::getRecHeader( Int64 inRecPos )
{
	DataRecord^	rtRec;				//�Ǎ����R�[�h
	Int32		chkDatLen;			//�`�F�b�N�p���R�[�h�f�[�^��
	Int32		readLen;			//�f�[�^�Ǎ���

	//-- ��ԃ`�F�b�N
	if( isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����I�[�v������Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N
	if( inRecPos == 0L )					//���R�[�h�ʒu��0�Ȃ���Ԃ��B			
	{
		return nullptr;
	}
	if( inRecPos < FIsam::Length() )		//���R�[�h�ʒu�����䕔�ɂ���Ȃ�G���[��Ԃ��B
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- ���R�[�h�Ǎ� --//
	//�w�b�_���Ǎ�
	try
	{
		FStream->Seek( inRecPos, SeekOrigin::Begin );
		readLen = FStream->Read( FileBuff, 0, DataRecord::getHeaderLength() );
		if( readLen != DataRecord::getHeaderLength() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪂���܂���B" );
			tmpFJIsamException->ErrorCode = 1108;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�w�b�_��񂪓ǂݍ��߂܂���B", exp );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	chkDatLen = FJIFuncs::ArrayToInt32( FileBuff, 0 );
	if( chkDatLen < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�f�[�^����ł��B" );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- �f�[�^���R�[�h��` --//
	try
	{
		rtRec = gcnew DataRecord( FileBuff, this, inRecPos, true, 1, nullptr );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h��`�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 1108;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//����I��
	return rtRec;
}

