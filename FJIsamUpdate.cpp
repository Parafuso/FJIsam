//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�\�[�X
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
//	class	:FJIsam	:ISAM�{��(�����֐�:���R�[�h�㏑)
//------------------------------------------------------------------------------
//***** ���R�[�h�㏑
void FJIsam::FIsam::UpdateRecord( DataRecord^ inRec )
{
	DataRecord^		oldRec;			//�m�F�p�����R�[�h
	DataRecord^		tmpOldRec;			//�폜�O���R�[�h�����[�V�����ۑ��p���R�[�h

	array<Byte>^	tmpData;		//���R�[�h�f�[�^
	array<Byte>^	oldData;		//���R�[�h�f�[�^

	Boolean			chkMoveFlg;			//���R�[�h�u�������t���O
	Int32			chkKeyChgFlg;		//�L�[�ύX�t���O

	DataRecordPos_T^ tmpDelPos;		//�f�[�^���R�[�h�����[�V����(���R�[�h�ړ���)
	DataRecordPos_T^ tmpMvPos;			//�f�[�^���R�[�h�����[�V����(�L�[�ϊ���)

	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getParentFile()->Equals( this ) == false ||
		inRec->getBuildID() != BuildID )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃��R�[�h���X�V���錠��������܂���B" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_OLD )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃��R�[�h�͍X�V�ł��܂���B" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- �m�F�p���R�[�h�Ǎ� --/
	try
	{
		oldRec = getRecord( inRec->getRecPos() );

		//�m�F�p���R�[�h�̃����[�V�������ۑ�
		tmpOldRec = gcnew DataRecord();
		tmpOldRec->setRecPos( oldRec->getRecPos() );
		tmpOldRec->setPrevRecPos( oldRec->getPrevRecPos() );
		tmpOldRec->setNextRecPos( oldRec->getNextRecPos() );
		tmpOldRec->setParentRecPos( oldRec->getParentRecPos() );
		tmpOldRec->setLtChildRecPos( oldRec->getLtChildRecPos() );
		tmpOldRec->setGtChildRecPos( oldRec->getGtChildRecPos() );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�󋵌������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}
	if( oldRec->getRecStat() == DataRecStat_EN::REC_DEL )		//���R�[�h���폜�ς݂Ȃ�G���[
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃��R�[�h�͍폜����Ă��܂��B" );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;		
	}

	//-- ���R�[�h�̈ړ��K�v���`�F�b�N(���R�[�h�����傫���Ȃ�΃t�@�C���ʒu���ړ�) --//
	if( inRec->getDataLength() > oldRec->getDataLength() )
	{
		chkMoveFlg = true;
	}
	//-- �L�[�̕ύX�����������`�F�b�N --//
	tmpData = inRec->getData( nullptr );
	oldData = oldRec->getData( nullptr );
	chkKeyChgFlg = FJIFuncs::KeyCompare( tmpData, oldData, ISKeyInfo, -1, KeyCmpOpt_EN::All );

	//-- ���R�[�h�ړ��s�v�ŁA�L�[�̕ύX���Ȃ� --//
	//�Y�����R�[�h����������
	if( chkMoveFlg == false && chkKeyChgFlg == 0 )
	{
		tmpDelPos = nullptr;
		tmpMvPos = nullptr;

		inRec->setEachKeyFlg( oldRec->getEachKeyFlg() );		//����L�[�t���O�`�F�b�N
		inRec->setPrevRecPos( oldRec->getPrevRecPos() );		//�O���R�[�h�ʒu
		inRec->setNextRecPos( oldRec->getNextRecPos() );		//�����R�[�h�ʒu
		inRec->setParentRecPos( oldRec->getParentRecPos() );	//�񕪖ؐe���R�[�h�ʒu
		inRec->setLtChildRecPos( oldRec->getLtChildRecPos() );	//�񕪖؃L�[�����R�[�h�ʒu
		inRec->setGtChildRecPos( oldRec->getGtChildRecPos() );	//�񕪖؃L�[�僌�R�[�h�ʒu
		try
		{
			WriteRecord( inRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"���R�[�h�X�V���ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;		
		}
			
	//-- ���R�[�h�ړ��K�v�ŁA�L�[�̕ύX���Ȃ� --//
	//���R�[�h���Ō���Ɉړ����A�����[�V�������郌�R�[�h���X�V����
	}else if( chkMoveFlg == true && chkKeyChgFlg == 0 ){

		tmpDelPos = nullptr;
		tmpMvPos = nullptr;

		//���R�[�h��ύX
		inRec->setRecPos( FStream->Length );						//���R�[�h�ʒu��ύX
		inRec->setEachKeyFlg( oldRec->getEachKeyFlg() );		//����L�[�t���O�`�F�b�N
		inRec->setPrevRecPos( oldRec->getPrevRecPos() );		//�O���R�[�h�ʒu
		inRec->setNextRecPos( oldRec->getNextRecPos() );		//�����R�[�h�ʒu
		inRec->setParentRecPos( oldRec->getParentRecPos() );	//�񕪖ؐe���R�[�h�ʒu
		inRec->setLtChildRecPos( oldRec->getLtChildRecPos() );	//�񕪖؃L�[�����R�[�h�ʒu
		inRec->setGtChildRecPos( oldRec->getGtChildRecPos() );	//�񕪖؃L�[�僌�R�[�h�ʒu
		try
		{
			//���R�[�h����
			WriteRecord( inRec );
			//�����[�V�������郌�R�[�h������
			WriteMoveRecordPos( inRec, inRec->getRecPos() );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"���R�[�h�ړ����X�V���ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;		
		}

	//-- ���R�[�h�ړ��s�v�ŁA�L�[�̕ύX���� --//
	//���R�[�h�̃����[�V�����폜���A�V���ɑ}���ʒu����������
	}else if(  chkKeyChgFlg != 0 ){
		tmpDelPos = gcnew DataRecordPos_T;
		tmpDelPos->PrevPos = nullptr;
		tmpDelPos->NextPos = nullptr;
		tmpDelPos->ParentPos = nullptr;
		tmpDelPos->LtChildPos = nullptr;
		tmpDelPos->GtChildPos = nullptr;
		tmpDelPos->ExtRecPos = nullptr;

		tmpMvPos = gcnew DataRecordPos_T;
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
			if( chkMoveFlg == true )					//���R�[�h�ʒu�ړ�������ΕύX
			{
				inRec->setRecPos( FStream->Length );
			}

			inRec->setEachKeyFlg( IS_NOEACHKEY );		//����L�[�t���O
			inRec->setPrevRecPos( 0L );					//�O���R�[�h�ʒu
			inRec->setNextRecPos( 0L );					//�����R�[�h�ʒu
			inRec->setParentRecPos( 0L );				//�񕪖ؐe���R�[�h�ʒu
			inRec->setLtChildRecPos( 0L );				//�񕪖؃L�[�����R�[�h�ʒu
			inRec->setGtChildRecPos( 0L );				//�񕪖؃L�[�僌�R�[�h�ʒu

			//���R�[�h�}���ʒu������
			FindInsertRecordPos( inRec, tmpMvPos );
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

			//���R�[�h����
			WriteRecord( inRec );

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"���R�[�h�̃L�[�ύX�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}
	}


	//-- ���R�[�h���ړ������Ȃ�A�����R�[�h���폜���R�[�h�ɂ��� --//
	if( chkMoveFlg == true )
	{
		//-- �폜���X�g�X�V --//
		if( DelRecLastPos != 0 )
		{
			DataRecord^ tmpDelRec;				//�폜���R�[�h
	
			//�폜���X�g�Ō�����R�[�h�擾
			try
			{
				tmpDelRec = getRecord( DelRecLastPos );
			}catch(  Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
					L"�폜�ς݃��R�[�h���X�g�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1401;
				tmpFJIsamException->ErrorCode2 = 10;
				throw tmpFJIsamException;		
			}
			//�폜�������R�[�h�������R�[�h�ɐݒ�
			tmpDelRec->setNextRecPos( oldRec->getRecPos() );
	
			//�w�b�_��������
			try
			{
				WriteRecHeader( tmpDelRec );
	
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
					L"�폜�ς݃��R�[�h���X�g�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1401;
				tmpFJIsamException->ErrorCode2 = 11;
				throw tmpFJIsamException;		
			}

			//�폜���R�[�h�X�V
			oldRec->setPrevRecPos( DelRecLastPos );			//�폜���R�[�h�̑O���R�[�h�ɐݒ�

			//�폜���X�g�X�V����
			delete tmpDelRec;
		}
		//�폜���R�[�h�w�b�_�X�V
		try
		{
			//���R�[�h�f�[�^�Z�b�g//
			oldRec->DeleteRec();
			oldRec->setParentRecPos( 0L );
			oldRec->setNextRecPos( 0L );
			oldRec->setLtChildRecPos( 0L );
			oldRec->setGtChildRecPos( 0L );

			WriteRecHeader( oldRec );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�폜���R�[�h�����ŃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;		
		}
	}

	//-- ISAM�w�b�_������ --//
	//���R�[�h�ő咷�A���R�[�h�ŏ������X�V
	if( RecMaxLen < inRec->getDataLength() )
	{
		RecMaxLen = inRec->getDataLength();
	}
	if( RecMinLen > inRec->getDataLength() )
	{
		RecMinLen = inRec->getDataLength();
	}

	//�L�[���ύX�ɂȂ����ꍇ�A�L�[�����X�V
	if( chkKeyChgFlg != 0 )
	{
		if( oldRec->getEachKeyFlg() == IS_NOEACHKEY && 
			inRec->getEachKeyFlg() != IS_NOEACHKEY )	
		{
			KeyCnt--;									//�P��L�[������L�[�ύX������L�[�����炷
		}else if( oldRec->getEachKeyFlg() != IS_NOEACHKEY && 
			inRec->getEachKeyFlg() == IS_NOEACHKEY )	
		{
			KeyCnt++;									//����L�[���P��L�[�ύX������L�[�𑝂₷
		}
	}
	//���R�[�h���ړ������ꍇ�A���R�[�h��
	if( chkMoveFlg == true )
	{
		AllRecCnt++;									//�S���R�[�h�����J�E���g�A�b�v
		DelRecCnt++;									//�폜���R�[�h�J�E���g�A�b�v
		//Isam���R�[�h���X�V
		if( RecFirstPos == tmpOldRec->getRecPos() )		//�擪���R�[�h��ݒ�
		{
			RecFirstPos = tmpOldRec->getNextRecPos();
		}
		if( RecLastPos == tmpOldRec->getRecPos() )			//�Ōヌ�R�[�h��ݒ�
		{
			RecLastPos = tmpOldRec->getPrevRecPos();
		}
		if( RecRootPos == tmpOldRec->getRecPos() )			//�񕪖؃��[�g���R�[�h��ݒ�
		{
			if( tmpDelPos->ParentPos == nullptr )
			{
				RecRootPos = 0L;
			}else{
				RecRootPos = tmpDelPos->ParentPos->getRecPos();
			}
		}
		if( DelRecFirstPos == 0L )						//�폜���R�[�h�擪��ݒ�
		{
			DelRecFirstPos = oldRec->getRecPos();
		}
		DelRecLastPos = oldRec->getRecPos();			//�폜���R�[�h�Ō��ݒ�
	}
	//Isam�w�b�_�X�V
	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"ISAM�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1401;
		tmpFJIsamException->ErrorCode2 = 13;
		throw tmpFJIsamException;		
	}


	//-- �T�u�C���f�b�N�X�̒ǉ� --//
	for( Int32 i = 2; i <= IdxCnt; i++)
	{
		SubIndex^ chkSubIdx;
		KeyInfo^  chkKeyInfo;

		array<Byte>^ tmpKey;
		array<Byte>^ tmpOldKey;

		//�T�u�C���f�b�N�X�t�@�C���擾(nullptr�Ȃ�X�L�b�v)
		chkSubIdx = SubIdx[i-2];
		if( chkSubIdx == nullptr )
		{
			continue;
		}

		//�T�u�C���f�b�N�X���R�[�h�폜
		try
		{
			chkKeyInfo = chkSubIdx->getKeyInfo();
			//�L�[�̕ύX���Ȃ��A���R�[�h�̈ړ����Ȃ���Ή������Ȃ�
			if( FJIFuncs::KeyCompare( tmpData, oldData, chkKeyInfo, -1, KeyCmpOpt_EN::All ) ==0 &&
				chkMoveFlg == true )
			{
				continue;
			}

			//�T�u�C���f�b�N�X���R�[�h�X�V//
			tmpKey = inRec->getKeyData( nullptr, chkKeyInfo, false );
			tmpOldKey = oldRec->getKeyData( nullptr, chkKeyInfo, false );
			if( inRec->getIdxNum() == i )
			{
				chkSubIdx->UpdateRecord( tmpOldKey, tmpKey, oldRec->getRecPos(),
											inRec->getRecPos(), 0L );
			}else{
				chkSubIdx->UpdateRecord( tmpOldKey, tmpKey, oldRec->getRecPos(),
											inRec->getRecPos(), inRec->getSubIdxRec()->getRecPos() );
			}
			//������̃f�[�^�폜
			delete tmpKey;
			delete tmpOldKey;

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�폜���ɃG���[���������܂���(�C���f�b�N�X({0:d2})�B", i),  exp );
			tmpFJIsamException->ErrorCode = 1401;
			tmpFJIsamException->ErrorCode2 = 14;
			throw tmpFJIsamException;		
		}
	}


	//-- �f�[�^�폜 --//
	delete oldRec;
	delete tmpOldRec;
	delete tmpData;
	delete oldData;

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

//------------------------------------------------------------------------------
//���R�[�h�X�V���ꏊ�ړ��`�F�b�N	
void FJIsam::FIsam::WriteMoveRecordPos( DataRecord^ inRec, Int64 inNewRecPos )
{
	DataRecordPos_T^ tmpPos;	//�X�V�����[�V����

	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inNewRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�V�������R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//-- ������ --//
	tmpPos = gcnew DataRecordPos_T;
	tmpPos->PrevPos = nullptr;
	tmpPos->NextPos = nullptr;
	tmpPos->ParentPos = nullptr;
	tmpPos->LtChildPos = nullptr;
	tmpPos->GtChildPos = nullptr;

	//-- �����[�V�������郌�R�[�h������ --//
	try
	{
		//-- �O���R�[�h���� --//
		if( inRec->getPrevRecPos() != 0L)
		{
			tmpPos->PrevPos = getRecord( inRec->getPrevRecPos() );
			tmpPos->PrevPos->setNextRecPos( inNewRecPos );
		}
		//-- �����R�[�h���� --//
		if( inRec->getNextRecPos() != 0L)
		{
			tmpPos->NextPos = getRecHeader( inRec->getNextRecPos() );
			tmpPos->NextPos->setPrevRecPos( inNewRecPos );
		}
		//-- �񕪖ؐe���R�[�h���� --//
		if( inRec->getParentRecPos() != 0L )
		{
			//�O���R�[�h�Ɛe���R�[�h�������Ȃ�A�O���R�[�h�̓񕪖؃L�[�僌�R�[�h�ɐݒ�
			if( inRec->getParentRecPos() == inRec->getPrevRecPos() )
			{
				tmpPos->PrevPos->setGtChildRecPos( inNewRecPos );

			//�����R�[�h�Ɛe���R�[�h�������Ȃ�A�O���R�[�h�̓񕪖؃L�[�����R�[�h�ɐݒ�
			}else if( inRec->getParentRecPos() == inRec->getNextRecPos()){
				tmpPos->NextPos->setLtChildRecPos( inNewRecPos );
			//�Ⴄ���R�[�h�Ȃ�A���R�[�h��Ǎ��A�X�V
			}else{
				tmpPos->ParentPos = getRecHeader( inRec->getParentRecPos() );
				if( tmpPos->ParentPos->getLtChildRecPos() == inRec->getRecPos() )
				{
					tmpPos->ParentPos->setLtChildRecPos( inNewRecPos );
				}else{
					tmpPos->ParentPos->setGtChildRecPos( inNewRecPos );
				}
			}
		}
		//-- �񕪖؃L�[�����R�[�h���� --//
		if( inRec->getLtChildRecPos() != 0L )
		{
			//�񕪖؃L�[�����R�[�h�ƑO���R�[�h�������Ȃ�A�O���R�[�h�̓񕪖ؐe���R�[�h�ɐݒ�
			if( inRec->getLtChildRecPos() == inRec->getPrevRecPos() )
			{
				tmpPos->PrevPos->setParentRecPos( inNewRecPos );
			
			//�Ⴄ���R�[�h�Ȃ�A���R�[�h��Ǎ��A�X�V
			}else{
				tmpPos->LtChildPos = getRecHeader( inRec->getLtChildRecPos() );
				tmpPos->LtChildPos->setParentRecPos( inNewRecPos );
			}
		}
		//-- �񕪖؃L�[�僌�R�[�h���� --//
		if( inRec->getGtChildRecPos() != 0L )
		{
			//�񕪖؃L�[�����R�[�h�Ǝ����R�[�h�������Ȃ�A�����R�[�h�̓񕪖ؐe���R�[�h�ɐݒ�
			if( inRec->getGtChildRecPos() == inRec->getPrevRecPos() )
			{
				tmpPos->NextPos->setParentRecPos( inNewRecPos );
			
			//�Ⴄ���R�[�h�Ȃ�A���R�[�h��Ǎ��A�X�V
			}else{
				tmpPos->GtChildPos = getRecHeader( inRec->getGtChildRecPos() );
				tmpPos->GtChildPos->setParentRecPos( inNewRecPos );
			}
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
						L"�����[�V�������R�[�h�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- �����[�V�������郌�R�[�h�w�b�_������ --//
	try
	{
		//�O���R�[�h
		if( tmpPos->PrevPos != nullptr )				
		{
			WriteRecHeader( tmpPos->PrevPos );
		}
		//�����R�[�h
		if( tmpPos->NextPos != nullptr )				
		{
			WriteRecHeader( tmpPos->NextPos );
		}
		//�񕪖ؐe���R�[�h
		if( tmpPos->ParentPos != nullptr )
		{
			WriteRecHeader( tmpPos->ParentPos );
		}
		//�񕪖؃L�[�����R�[�h
		if( tmpPos->LtChildPos != nullptr )
		{
			WriteRecHeader( tmpPos->LtChildPos );
		}
		//�񕪖؃L�[�僌�R�[�h
		if( tmpPos->GtChildPos != nullptr )
		{
			WriteRecHeader( tmpPos->GtChildPos );
		}
		//�֘A���R�[�h
		if( tmpPos->ExtRecPos != nullptr )
		{
			WriteRecHeader( tmpPos->ExtRecPos );
		}
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�����[�V�������R�[�h�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1402;
		tmpFJIsamException->ErrorCode2 = 4;
	}
}

