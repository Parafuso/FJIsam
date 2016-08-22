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
//	class	:SubIndex	:�T�u�C���f�b�N�X(�����֐�:���R�[�h�X�V)
//------------------------------------------------------------------------------
//***** ���R�[�h�X�V
void FJIsam::SubIndex::UpdateRecord(  array<Byte>^ inKey, array<Byte>^ inNewKey,				
							Int64 inRecPos, Int64 inNewRecPos, Int64 inSubIdxPos )
{
	SubIndexRecord^		oldRec;			//�m�F�p�����R�[�h
	SubIndexRecord^		tmpOldRec;		//�폜�O���R�[�h�����[�V�����ۑ��p���R�[�h
	
	array<Byte>^		tmpData;		//�L�[�R�s�[�p�f�[�^
	Int32			chkKeyChgFlg;		//�L�[�ύX�t���O

	SubIndexRecordPos_T^ tmpDelPos;			//�f�[�^���R�[�h�����[�V����(���R�[�h�ړ���)
	SubIndexRecordPos_T^ tmpMvPos;			//�f�[�^���R�[�h�����[�V����(�L�[�ϊ���)

	//-- �����`�F�b�N --//
	if( inKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�����L�[�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inNewKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�X�V�L�[�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( inNewRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�V�����f�[�^���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- �m�F�p���R�[�h�Ǎ� --/
	try
	{
		//���R�[�h�ʒu���킩���Ă���ꍇ(�������ɃT�u�C���f�b�N�X���g�p)
		if( inSubIdxPos != 0 )
		{
			oldRec = getRecord( inSubIdxPos );
		//���R�[�h�ʒu���킩��Ȃ��ꍇ(�������Ƀ��C���܂��͕ʂ̃T�u�C���f�b�N�X���g�p)
		}else{
			SubIndexRecord^ tmpSchRec;
			SubIndexRecord^ tmpOldSchRec;
			tmpSchRec = ReadRecord( inKey );
			tmpOldSchRec = nullptr;
			while( true )
			{
				if( tmpSchRec == nullptr )
				{
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"�폜�Ώۂ̃��R�[�h������܂���B" );
					tmpFJIsamException->ErrorCode = 3401;
					tmpFJIsamException->ErrorCode2 = 5;
					throw tmpFJIsamException;		
				}
				if( tmpSchRec->getDataRecPos() == inRecPos )
				{
					oldRec = tmpSchRec;
					if( tmpOldSchRec != nullptr ) delete tmpOldSchRec;
					break;
				}
				//���������L�[�̃��R�[�h���Ȃ�
				if( tmpSchRec->getEachKeyFlg() == IS_NOEACHKEY || 
					tmpSchRec->getEachKeyFlg() == IS_EACHKEYLST ||
					tmpSchRec->getNextRecPos() == 0L )
				{
					delete tmpSchRec;
					if( tmpOldSchRec != nullptr ) delete tmpOldSchRec;
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"�폜�Ώۂ̃��R�[�h������܂���B" );
					tmpFJIsamException->ErrorCode = 3401;
					tmpFJIsamException->ErrorCode2 = 6;
					throw tmpFJIsamException;		
				}
				//--���̃��R�[�h��ǂݍ���
				tmpOldSchRec = tmpSchRec;
				tmpSchRec = NextRecord( tmpOldRec );
			}
		}
		if( oldRec->getDelFlg() == IS_RECDEL )
		{
			delete oldRec;

			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"���̃��R�[�h�͕ʂ̏����ɂ���č폜�ς݂ł��B" );
			tmpFJIsamException->ErrorCode = 3401;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;		
		}

		if( oldRec->getDataRecPos() != inRecPos )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"�폜�Ώۂ̃��R�[�h������܂���B" );
			tmpFJIsamException->ErrorCode = 3401;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;		
		}

		//�m�F�p���R�[�h�̃����[�V�������ۑ�
		tmpOldRec = gcnew SubIndexRecord( this );
		tmpOldRec->setRecStat( oldRec->getRecStat() );
		tmpOldRec->setRecPos( oldRec->getRecPos() );
		tmpOldRec->setPrevRecPos( oldRec->getPrevRecPos() );
		tmpOldRec->setNextRecPos( oldRec->getNextRecPos() );
		tmpOldRec->setParentRecPos( oldRec->getParentRecPos() );
		tmpOldRec->setLtChildRecPos( oldRec->getLtChildRecPos() );
		tmpOldRec->setGtChildRecPos( oldRec->getGtChildRecPos() );
		tmpOldRec->setDataRecPos( oldRec->getDataRecPos() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�󋵌������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;		
	}

	//-- �L�[�̕ύX�����������`�F�b�N --//
	chkKeyChgFlg = FJIFuncs::KeyCompare( inKey, inNewKey, SubKeyInfo, IdxNum, KeyCmpOpt_EN::All );

	//-- �L�[�̕ύX���Ȃ��ꍇ --//
	//���������I��
	if( chkKeyChgFlg != 0 )
	{
		delete oldRec;
		delete tmpOldRec;
		return;

	//-- �L�[�̕ύX���� --//
	//���R�[�h�̃����[�V�����폜���A�V���ɑ}���ʒu����������
	}else{
		tmpDelPos = gcnew SubIndexRecordPos_T;
		tmpDelPos->PrevPos = nullptr;
		tmpDelPos->NextPos = nullptr;
		tmpDelPos->ParentPos = nullptr;
		tmpDelPos->LtChildPos = nullptr;
		tmpDelPos->GtChildPos = nullptr;
		tmpDelPos->ExtRecPos = nullptr;

		tmpMvPos = gcnew SubIndexRecordPos_T;
		tmpMvPos->PrevPos = nullptr;
		tmpMvPos->NextPos = nullptr;
		tmpMvPos->ParentPos = nullptr;
		tmpMvPos->LtChildPos = nullptr;
		tmpMvPos->GtChildPos = nullptr;
		tmpMvPos->ExtRecPos = nullptr;
		try
		{
			//���R�[�h���폜����
			ChkDeleteRecordPos( oldRec, tmpDelPos );
			//�ړ��O�̃L�[�폜������
			//�O���R�[�h
			if( tmpDelPos->PrevPos != nullptr )				
			{
				WriteRecHeader( tmpDelPos->PrevPos );
			}
			//�����R�[�h
			if( tmpDelPos->NextPos != nullptr )				
			{
				WriteRecHeader( tmpDelPos->NextPos );
			}
			//�񕪖ؐe���R�[�h
			if( tmpDelPos->ParentPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->ParentPos );
			}
			//�񕪖؃L�[�����R�[�h
			if( tmpDelPos->LtChildPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->LtChildPos );
			}
			//�񕪖؃L�[�僌�R�[�h
			if( tmpDelPos->GtChildPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->GtChildPos );
			}
			//�֘A���R�[�h
			if( tmpDelPos->ExtRecPos != nullptr )
			{
				WriteRecHeader( tmpDelPos->ExtRecPos );
			}

			//���R�[�h��ύX
			oldRec->setEachKeyFlg( IS_NOEACHKEY );		//����L�[�t���O
			oldRec->setPrevRecPos( 0L );					//�O���R�[�h�ʒu
			oldRec->setNextRecPos( 0L );					//�����R�[�h�ʒu
			oldRec->setParentRecPos( 0L );				//�񕪖ؐe���R�[�h�ʒu
			oldRec->setLtChildRecPos( 0L );				//�񕪖؃L�[�����R�[�h�ʒu
			oldRec->setGtChildRecPos( 0L );				//�񕪖؃L�[�僌�R�[�h�ʒu
			tmpData = gcnew array<Byte>( SubKeyInfo->getKeyLen() );
			Buffer::BlockCopy( inNewKey, 0, tmpData, 0, SubKeyInfo->getKeyLen() );
			oldRec->setKeyData( tmpData );
			delete tmpData;
			tmpData = gcnew array<Byte>( SubKeyInfo->getSrtKeyLen() );
			Buffer::BlockCopy( inNewKey, SubKeyInfo->getKeyLen(), 
									tmpData, 0, SubKeyInfo->getSrtKeyLen() );
			oldRec->setKeyData( tmpData );
			delete tmpData;

			//���R�[�h�}���ʒu������
			FindInsertRecordPos( oldRec, tmpMvPos );
			//���R�[�h�}���ʒu������
			//�O���R�[�h
			if( tmpMvPos->PrevPos != nullptr )				
			{
				WriteRecHeader( tmpMvPos->PrevPos );
			}
			//�����R�[�h
			if( tmpMvPos->NextPos != nullptr )				
			{
				WriteRecHeader( tmpMvPos->NextPos );
			}
			//�񕪖ؐe���R�[�h
			if( tmpMvPos->ParentPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->ParentPos );
			}
			//�񕪖؃L�[�����R�[�h
			if( tmpMvPos->LtChildPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->LtChildPos );
			}
			//�񕪖؃L�[�僌�R�[�h
			if( tmpMvPos->GtChildPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->GtChildPos );
			}
			//�֘A���R�[�h
			if( tmpMvPos->ExtRecPos != nullptr )
			{
				WriteRecHeader( tmpMvPos->ExtRecPos );
			}

			WriteRecord( oldRec );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"���R�[�h�̃L�[�ύX�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 3401;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;
		}
	}

	//-- �T�u�C���f�b�N�X�w�b�_������ --//

	//�L�[���ύX�ɂȂ����ꍇ�A�L�[���A���R�[�h�����X�V
	if( chkKeyChgFlg != 0 )
	{
		if( tmpOldRec->getEachKeyFlg() == IS_NOEACHKEY && 
			oldRec->getEachKeyFlg() != IS_NOEACHKEY )	
		{
			KeyCnt--;									//�P��L�[������L�[�ύX������L�[�����炷
		}else if( tmpOldRec->getEachKeyFlg() != IS_NOEACHKEY && 
					oldRec->getEachKeyFlg() == IS_NOEACHKEY )	
		{
			KeyCnt++;									//����L�[���P��L�[�ύX������L�[�𑝂₷
		}
	}
	//Isam�w�b�_�X�V
	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�T�u�C���f�b�N�X�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3401;
		tmpFJIsamException->ErrorCode2 = 11;
		throw tmpFJIsamException;		
	}


	//-- �f�[�^�폜 --//
	delete oldRec;
	delete tmpOldRec;

	if( tmpDelPos != nullptr )
	{
		if( tmpDelPos->PrevPos != nullptr ) delete tmpDelPos->PrevPos; 
		if( tmpDelPos->NextPos != nullptr ) delete tmpDelPos->NextPos; 
		if( tmpDelPos->ParentPos != nullptr ) delete tmpDelPos->ParentPos; 
		if( tmpDelPos->LtChildPos != nullptr ) delete tmpDelPos->LtChildPos; 
		if( tmpDelPos->GtChildPos != nullptr ) delete tmpDelPos->GtChildPos; 
		if( tmpDelPos->ExtRecPos != nullptr ) delete tmpDelPos->ExtRecPos; 

		delete tmpDelPos;
	}
	if( tmpMvPos != nullptr )
	{
		if( tmpMvPos->PrevPos != nullptr ) delete tmpDelPos->PrevPos; 
		if( tmpMvPos->NextPos != nullptr ) delete tmpDelPos->NextPos; 
		if( tmpMvPos->ParentPos != nullptr ) delete tmpDelPos->ParentPos; 
		if( tmpMvPos->LtChildPos != nullptr ) delete tmpDelPos->LtChildPos; 
		if( tmpMvPos->GtChildPos != nullptr ) delete tmpDelPos->GtChildPos; 
		if( tmpMvPos->ExtRecPos != nullptr ) delete tmpDelPos->ExtRecPos; 

		delete tmpDelPos;
	}

	//-- ����I�� --//
	return;
}

