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
List<DataRecord^>^ FJIsam::FIsam::ReadRecordWords( String^ inKey, Int32 inIdxNum )
{
	List<DataRecord^>^	rtList;				//�擾���R�[�h���X�g
	DataRecord^			baseRec;			//�擾���R�[�h�x�[�X
	String^				tmpKeyStr;			//�����L�[
	Int32				tmpKeyLen;			//�����L�[��
	Int32				tmpKeyLenT;			//�����L�[��

	DataRecord^			tmpRec;				//�擾���R�[�h

	Int64				chkRecPos;			//�`�F�b�N�p���R�[�h�ʒu
	SubIndex^			chkSubIdx;			//�`�F�b�N�p�T�u�C���f�b�N�X

	Int32				chkKeyFlg;			//�L�[�`�F�b�N����


	//-- �����`�F�b�N --//
	if( inKey == nullptr )		//�L�[�Ȃ�
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����w��ł��B");
		tmpFJIsamException->ErrorCode = 1701;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inIdxNum < 1 ||			//�C���f�b�N�X�ُ�
		inIdxNum > IdxCnt )			
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�̎w�肪�s���ł��B");
		tmpFJIsamException->ErrorCode = 1701;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( getKeyType( inIdxNum ) != keyType_EN::ISKEY_UNICODE )		//�L�[�^�C�v�`�F�b�N
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�^�C�v��UNICODE�ł͂���܂���B");
		tmpFJIsamException->ErrorCode = 1701;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;

	}

	//-- �����������L�[����蒷����� nullptr ��Ԃ� --//
	tmpKeyLen = inKey->Length;										//�L�[���擾
	if( tmpKeyLen > getKeyLen( inIdxNum ) )							//�L�[���`�F�b�N
	{
		return nullptr;
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
		tmpKeyStr = FJIFuncs::KeyStringConv( inKey );
		tmpKeyLenT = tmpKeyStr->Length;

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
					tmpFJIsamException->ErrorCode = 1701;
					tmpFJIsamException->ErrorCode2 = 5;
					throw tmpFJIsamException;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"���R�[�h�ǂݍ��݂Ɏ��s���܂����B", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 6;
				throw tmpFJIsamException;
			}
			//�L�[��r
			try
			{
				chkKeyFlg = String::Compare( tmpKeyStr, 0, 
					FJIFuncs::KeyStringConv( 
						FJIFuncs::ArrayToString( baseRec->getData(nullptr), getKeyPos(1), getKeyLen(1) ) ), 0,
						tmpKeyLenT, StringComparison::Ordinal );

			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"���R�[�h�̓��e���s���ł��B", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 7;
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
		rtList = gcnew List<DataRecord^>();
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
					tmpFJIsamException->ErrorCode = 1701;
					tmpFJIsamException->ErrorCode2 = 8;
					throw tmpFJIsamException;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"���R�[�h�ǂݍ��݂Ɏ��s���܂����B", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 9;
				throw tmpFJIsamException;
			}

			//�L�[��r
			try
			{
				chkKeyFlg = String::Compare( tmpKeyStr, 0, 
					FJIFuncs::KeyStringConv( 
						FJIFuncs::ArrayToString( tmpRec->getData(nullptr), getKeyPos(1), getKeyLen(1) ) ), 0, 
						tmpKeyLenT, StringComparison::Ordinal );

			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"���R�[�h�̓��e���s���ł��B", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 10;
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
					tmpFJIsamException->ErrorCode = 1701;
					tmpFJIsamException->ErrorCode2 = 11;
					throw tmpFJIsamException;
				}
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException(
					L"���R�[�h�ǂݍ��݂Ɏ��s���܂����B", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 12;
				throw tmpFJIsamException;
			}

			//�L�[��r
			try
			{
				chkKeyFlg = String::Compare( tmpKeyStr, 0, 
					FJIFuncs::KeyStringConv( 
						FJIFuncs::ArrayToString( tmpRec->getData(nullptr), getKeyPos(1), getKeyLen(1) ) ), 0,
						tmpKeyLenT, StringComparison::Ordinal );

			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					L"���R�[�h�̓��e���s���ł��B", exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 13;
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

	//-- �T�u�C���f�b�N�X���� --//
	}else{

		List<SubIndexRecord^>^ tmpSubList;

		//�T�u�C���f�b�N�X�擾
		try
		{
			chkSubIdx = SubIdx[inIdxNum-2];
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1701;
			tmpFJIsamException->ErrorCode2 = 14;
			throw tmpFJIsamException;
		}
		//�T�u�C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X�����݂��܂���B�C���f�b�N�X({0:d2})", inIdxNum) );
			tmpFJIsamException->ErrorCode = 1701;
			tmpFJIsamException->ErrorCode2 = 15;
			throw tmpFJIsamException;
		}

		//�ΏۃT�u�C���f�b�N�X���R�[�h�擾
		try{
			tmpSubList = chkSubIdx->ReadRecordWords( inKey );
			if( tmpSubList == nullptr )	//�Y�����R�[�h�Ȃ�
			{
				return nullptr;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2})", inIdxNum), exp );
			tmpFJIsamException->ErrorCode = 1701;
			tmpFJIsamException->ErrorCode2 = 16;
			throw tmpFJIsamException;
		}

		//�f�[�^���R�[�h
		rtList = gcnew List<DataRecord^>(tmpSubList->Count);

		for( Int32 i = 0; i < tmpSubList->Count; i++ )
		{

			try
			{
				tmpRec = getRecord( inIdxNum, tmpSubList[i] );
				rtList->Add( tmpRec );

			}catch( Exception^ exp ){
				for( Int32 j = 0; j < tmpSubList->Count; j++ )
				{
					if( i > j )
					{
						delete rtList[j];
					}else{
						delete tmpSubList[j];
					}
				}
				delete tmpSubList;
				delete rtList;

				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( 
					String::Format( L"�T�u�C���f�b�N�X�L�[�ɑ΂��郌�R�[�h�Ǎ��Ɏ��s���܂����B�C���f�b�N�X({0:d2}) ���X�g�ԍ�{1:d2}", inIdxNum, i), exp );
				tmpFJIsamException->ErrorCode = 1701;
				tmpFJIsamException->ErrorCode2 = 17;
				throw tmpFJIsamException;
			}
		}
		delete tmpSubList;
	}

	//-- ����I�� --//
	return rtList;

}
