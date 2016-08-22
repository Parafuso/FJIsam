//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�\�[�X
//	Version 1.0		create date 2003/02/02
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		FJIsam	: ISAM�{�� ���R�[�h�폜�����֌W�\�[�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"					//ISAM�N���X
#include "FJIsamFunc.h"				//ISAM�p�֐�
#include "FJIsamException.h"		//��O�N���X

using namespace FJIsam;
//------------------------------------------------------------------------------
//	class	:FJIsam	:ISAM�{��(�����֐�:���R�[�h�폜)
//------------------------------------------------------------------------------
//***** ���R�[�h�폜
void FJIsam::FIsam::DeleteRecord( DataRecord^ inRec)
{
	DataRecord^		oldRec;				//�m�F�p���R�[�h
	DataRecord^		tmpOldRec;			//�폜�O���R�[�h�����[�V�����ۑ��p���R�[�h

	DataRecordPos_T^ tmpDelPos;			//�f�[�^���R�[�h�����[�V����

	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->getParentFile()->Equals( this ) == false ||
		inRec->getBuildID() != BuildID )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃��R�[�h���폜���錠��������܂���B" );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- �f�[�^������ --//
	tmpDelPos = gcnew DataRecordPos_T;

	//-- �m�F�p���R�[�h�擾 --//
	try
	{
		oldRec = getRecord( inRec->getRecPos() );
		if( oldRec->getDelFlg() == IS_RECDEL )
		{
			delete oldRec;

			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"���̃��R�[�h�͕ʂ̏����ɂ���č폜�ς݂ł��B" );
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;		
		}

		//�m�F�p���R�[�h�̃����[�V�������ۑ�
		tmpOldRec = gcnew DataRecord();
		tmpOldRec->setRecPos( oldRec->getRecPos() );
		tmpOldRec->setPrevRecPos( oldRec->getPrevRecPos() );
		tmpOldRec->setNextRecPos( oldRec->getNextRecPos() );
		tmpOldRec->setParentRecPos( oldRec->getParentRecPos() );
		tmpOldRec->setLtChildRecPos( oldRec->getLtChildRecPos() );
		tmpOldRec->setGtChildRecPos( oldRec->getGtChildRecPos() );

	//���R�[�h�Ǎ����s
	}catch( Exception^ exp ){	
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�폜���R�[�h�̏󋵊m�F���ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- �폜�폜��񌟍� --//	
	try
	{
		ChkDeleteRecordPos( oldRec, tmpDelPos );
	}catch( Exception^ exp )	//���R�[�h�Ǎ����s
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�폜���R�[�h�����[�V�������`�F�b�N�ŃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;		
	}

	//-- �����[�V�������郌�R�[�h�w�b�_������ --//
	try
	{
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
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�����[�V�������R�[�h�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 7;
	}

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
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 8;
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
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;		
		}

		//�폜���R�[�h�X�V
		oldRec->setPrevRecPos( DelRecLastPos );			//�폜���R�[�h�̑O���R�[�h�ɐݒ�

		//�폜���X�g�X�V����
		delete tmpDelRec;
	}

	//-- �폜���R�[�h�w�b�_�X�V --//
	try
	{
		//-- ���R�[�h�f�[�^�Z�b�g --//
		oldRec->DeleteRec();
		oldRec->setParentRecPos( 0L );
		oldRec->setNextRecPos( 0L );
		oldRec->setLtChildRecPos( 0L );
		oldRec->setGtChildRecPos( 0L );

		WriteRecHeader( oldRec );
	}catch( Exception^ exp )	//���R�[�h�Ǎ����s
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�폜���R�[�h�����ŃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;		
	}

	//-- ISAM�w�b�_������ --//
	if( oldRec->getEachKeyFlg() == IS_NOEACHKEY )	//�L�[�J�E���g�_�E��	
	{
		KeyCnt--;
	}

	RecCnt--;										//�L�����R�[�h�J�E���g�_�E��
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
		if( tmpOldRec->getEachKeyFlg() == IS_EACHKEYFST )
		{
			RecRootPos = tmpDelPos->NextPos->getRecPos();
		}else if( RecCnt == 0 ){
			RecRootPos = 0L;
		}
	}

	if( DelRecFirstPos == 0L )						//�폜���R�[�h�擪�ɐݒ�(���R�[�h�����̏ꍇ)
	{
		DelRecFirstPos = oldRec->getRecPos();			
	}
	DelRecLastPos = oldRec->getRecPos();			//�폜���R�[�h�Ō��ݒ�

	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"ISAM�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 1501;
		tmpFJIsamException->ErrorCode2 = 11;
		throw tmpFJIsamException;		
	}

	//-- �T�u�C���f�b�N�X�̒ǉ� --//
	for( Int32 i = 2; i <= IdxCnt; i++)
	{
		SubIndex^ chkSubIdx;
		KeyInfo^  chkKeyInfo;
		array<Byte>^ tmpKey;

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
			tmpKey = oldRec->getKeyData( nullptr, chkKeyInfo, false );
			if( inRec->getIdxNum() == i )
			{
				chkSubIdx->DeleteRecord( tmpKey, oldRec->getRecPos(), 0L );
			}else{
				chkSubIdx->DeleteRecord( tmpKey, oldRec->getRecPos(), inRec->getSubIdxRec()->getRecPos() );
			}
			//������̃f�[�^�폜
			delete tmpKey;

		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"�T�u�C���f�b�N�X���R�[�h�폜���ɃG���[���������܂���(�C���f�b�N�X({0:d2})�B", i),  exp );
			tmpFJIsamException->ErrorCode = 1501;
			tmpFJIsamException->ErrorCode2 = 12;
			throw tmpFJIsamException;		
		}
	}

	//-- �f�[�^�폜 --//
	delete oldRec;
	delete tmpOldRec;

	if( tmpDelPos->PrevPos != nullptr ) delete tmpDelPos->PrevPos; 
	if( tmpDelPos->NextPos != nullptr ) delete tmpDelPos->NextPos; 
	if( tmpDelPos->ParentPos != nullptr ) delete tmpDelPos->ParentPos; 
	if( tmpDelPos->LtChildPos != nullptr ) delete tmpDelPos->LtChildPos; 
	if( tmpDelPos->GtChildPos != nullptr ) delete tmpDelPos->GtChildPos; 
	if( tmpDelPos->ExtRecPos != nullptr ) delete tmpDelPos->ExtRecPos; 
	delete tmpDelPos;

	//-- ����I�� --//
	return;
}

//------------------------------------------------------------------------------
// Private �֐�
//------------------------------------------------------------------------------
//***** ���R�[�h�폜�ʒu�`�F�b�N
void FJIsam::FIsam::ChkDeleteRecordPos( DataRecord^ inRec, DataRecordPos_T^ inDelPos )
{
	DataRecord^			tmpRec;				//��ƃ��R�[�h
	DataRecord^			tmpOldRec;			//��Ƌ����R�[�h

	DataRecord^			tmpParentRec;		//��Ɠ񕪖ؐe���R�[�h
	DataRecord^			tmpLtChildRec;		//��Ɠ񕪖؃L�[�����R�[�h
	DataRecord^			tmpGtChildRec;		//��Ɠ񕪖؃L�[�僌�R�[�h

	Int32				tmpLtChildDep;		//�񕪖؏��q�}���K�w
	Int32				tmpGtChildDep;		//�񕪖ؑ�q�}���K�w
	DataRecord^			tmpLtChildEndRec;	//�񕪖؃L�[�����ŉ��w���R�[�h
	DataRecord^			tmpGtChildEndRec;	//�񕪖؃L�[�呤�ŉ��w���R�[�h

	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�����ݒ�ł��B" );
		tmpFJIsamException->ErrorCode = 1502;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inDelPos == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�����[�V����������񂪖��ݒ�ł��B" );
		tmpFJIsamException->ErrorCode = 1502;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	//-- �f�[�^������ --//
	inDelPos->PrevPos = nullptr;
	inDelPos->NextPos = nullptr;
	inDelPos->ParentPos = nullptr;
	inDelPos->LtChildPos = nullptr;
	inDelPos->GtChildPos = nullptr;
	inDelPos->ExtRecPos = nullptr;
	tmpOldRec = nullptr;
	tmpParentRec = nullptr;
	tmpLtChildRec = nullptr;
	tmpGtChildRec = nullptr;

	//-- ���X�g�O���R�[�h���� --//
	if( inRec->getPrevRecPos() != 0L )
	{
		//���R�[�h�Ǐo
		try
		{
			tmpRec = getRecHeader( inRec->getPrevRecPos() );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�폜�O���R�[�h�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;		
		}
		//�O���R�[�h�̐ݒ�
		inDelPos->PrevPos = tmpRec;
		inDelPos->PrevPos->setNextRecPos( inRec->getNextRecPos() );		//�����R�[�h�ʒu

		//����L�[���X�V(�폜���R�[�h������L�[�Ō�̏ꍇ)
		if( inRec->getEachKeyFlg() == IS_EACHKEYLST )
		{
			//�O���R�[�h������L�[�̍ŏ��Ȃ�A����L�[�Ȃ��ɐݒ�
			if( inDelPos->PrevPos->getEachKeyFlg() == IS_EACHKEYFST )
			{
				 inDelPos->PrevPos->setEachKeyFlg( IS_NOEACHKEY );
			//��L�ȊO(����L�[�r���������肦�Ȃ�)�Ȃ�A����L�[�Ō�ɐݒ�
			}else{
				inDelPos->PrevPos->setEachKeyFlg( IS_EACHKEYLST );
			}
		}
	}

	//-- ���X�g�����R�[�h���� --//
	if( inRec->getNextRecPos() != 0L )
	{
		//���R�[�h�Ǐo
		try
		{
			tmpRec = getRecHeader( inRec->getPrevRecPos() );
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�폜�����R�[�h�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;		
		}
		//�����R�[�h�̐ݒ�
		inDelPos->NextPos = tmpRec;
		inDelPos->NextPos->setPrevRecPos( inRec->getPrevRecPos() );		//�����R�[�h�ʒu

		//�폜���R�[�h������L�[�擪�̏ꍇ�A�����R�[�h�̓���L�[�A�񕪖؂̃c���[�X�V
		if( inRec->getEachKeyFlg() == IS_EACHKEYFST )
		{
			//�����R�[�h������L�[�Ō�̏ꍇ�A����L�[�Ȃ��ɐݒ肷��
			if( inDelPos->NextPos->getEachKeyFlg() == IS_EACHKEYLST )
			{
				inDelPos->NextPos->setEachKeyFlg( IS_NOEACHKEY );
			}else{
				inDelPos->NextPos->setEachKeyFlg( IS_EACHKEYFST );
			}
			inDelPos->NextPos->setParentRecPos( inRec->getParentRecPos() );		//�񕪖ؐe�L�[��ݒ�
			inDelPos->NextPos->setLtChildRecPos( inRec->getLtChildRecPos() );	//�񕪖؃L�[����ݒ�
			inDelPos->NextPos->setGtChildRecPos( inRec->getGtChildRecPos() );	//�񕪖؃L�[���ݒ�
		}
	}
	//-- �񕪖؍X�V���K�v�Ȃ��ꍇ�i����L�[�̓r���A�Ō�j�Ȃ�I�� --//
	if( inRec->getEachKeyFlg() == IS_EACHKEYMID ||
		inRec->getEachKeyFlg() == IS_EACHKEYLST  )
	{
		return;
	}

	//-- ����L�[�̐擪�̏ꍇ --//
	if( inRec->getEachKeyFlg() == IS_EACHKEYFST )
	{
		//�񕪖ؐe���R�[�h��ݒ�
		if( inRec->getParentRecPos() != 0L )
		{
			//�O���R�[�h�Ɠ񕪖ؐe���R�[�h������̏ꍇ
			if( inRec->getPrevRecPos() == inRec->getParentRecPos() )
			{
				inDelPos->PrevPos->setGtChildRecPos( inDelPos->NextPos->getRecPos() );
			//�O���R�[�h�Ɠ񕪖ؐe���R�[�h������łȂ��ꍇ
			}else{
				tmpRec = getRecHeader( inRec->getParentRecPos() );
				if( tmpRec == nullptr )
				{
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"�폜�񕪖ؐe���R�[�h�������ɃG���[���������܂����B" );
					tmpFJIsamException->ErrorCode = 1502;
					tmpFJIsamException->ErrorCode2 = 4;
					throw tmpFJIsamException;		
				}
				inDelPos->LtChildPos = tmpRec;

				if( tmpRec->getLtChildRecPos() == inRec->getRecPos() )
				{
					tmpRec->setLtChildRecPos( inDelPos->NextPos->getRecPos() );
				}else{
					tmpRec->setGtChildRecPos( inDelPos->NextPos->getRecPos() );
				}
			}
		}

		//�񕪖؃L�[�����R�[�h��ݒ�
		if( inRec->getLtChildRecPos() != 0L )
		{
			//�O���R�[�h�Ɠ񕪖؃L�[�����R�[�h������̏ꍇ
			if( inRec->getPrevRecPos() == inRec->getLtChildRecPos() )
			{
				inDelPos->PrevPos->setParentRecPos( inDelPos->NextPos->getRecPos() );
			//�O���R�[�h�Ɠ񕪖؃L�[�����R�[�h������łȂ��ꍇ
			}else{
				tmpRec = getRecHeader( inRec->getLtChildRecPos() );
				if( tmpRec == nullptr )
				{
					FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
						L"�폜�񕪖؃L�[�����R�[�h�������ɃG���[���������܂����B" );
					tmpFJIsamException->ErrorCode = 1502;
					tmpFJIsamException->ErrorCode2 = 5;
					throw tmpFJIsamException;		
				}
				inDelPos->ParentPos = tmpRec;
				tmpRec->setParentRecPos( inDelPos->NextPos->getRecPos() );
			}
		}
	
		//�񕪖؃L�[�僌�R�[�h��ݒ�
		if( inRec->getGtChildRecPos() != 0L )
		{
			tmpRec = getRecHeader( inRec->getGtChildRecPos() );
			if( tmpRec == nullptr )
			{
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"�폜�񕪑�L�[�����R�[�h�������ɃG���[���������܂����B" );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 6;
				throw tmpFJIsamException;		
			}
			inDelPos->GtChildPos = tmpRec;
			tmpRec->setParentRecPos( inDelPos->NextPos->getRecPos() );
		}

		//-- ����I�� --//
		return;
	}

	//-- �񕪖ؐe���R�[�h --//
	if( inRec->getParentRecPos() != 0L )
	{
		//�e���R�[�h�ݒ�
		if( inRec->getParentRecPos() == inRec->getPrevRecPos() )		//�O���R�[�h�Ɠ���
		{
			tmpParentRec = inDelPos->PrevPos;
		}else if( inRec->getParentRecPos() == inRec->getPrevRecPos() ){	//�����R�[�h�Ɠ���
			tmpParentRec = inDelPos->PrevPos;
		}else{															//��L�Ƃ͈قȂ郌�R�[�h
			try
			{
				tmpRec = getRecHeader( inRec->getParentRecPos() );
				inDelPos->ParentPos = tmpRec;
				tmpParentRec = tmpRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"�폜�񕪖ؐe���R�[�h�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 7;
				throw tmpFJIsamException;		
			}
		}
	}

	//-- �񕪖؂ɃL�[��A�����R�[�h�̗����Ȃ��ꍇ�A�e���R�[�h�̎q�����[�V�������Ȃ����ďI�� --//
	if( inRec->getLtChildRecPos() == 0L && 
		inRec->getGtChildRecPos() == 0L )
	{
		//�폜���R�[�h���L�[���Ƀ����[�V����
		if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )	
		{
			tmpParentRec->setLtChildRecPos( 0L );
		//�폜���R�[�h���L�[��Ƀ����[�V����
		}else{
			inDelPos->ParentPos->setGtChildRecPos( 0L );
		}
		return;
	}

	//-- �L�[���݂̂��Ȃ��ꍇ(�L�[���e���R�[�h�Ƀ����[�V����) --//
	if( inRec->getLtChildRecPos() == 0 && 
		inRec->getGtChildRecPos() > 0 )
	{
		//�񕪖؃L�[�僌�R�[�h�ݒ�
		if( inRec->getGtChildRecPos() == inRec->getNextRecPos() )		//�����R�[�h�Ɠ���
		{
			tmpGtChildRec = inDelPos->NextPos;
		}else{															//��L�ȊO
			try
			{
				tmpRec = getRecHeader( inRec->getGtChildRecPos() );
				inDelPos->GtChildPos = tmpRec;
				tmpGtChildRec = tmpRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"�폜�񕪖؃L�[�僌�R�[�h�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 8;
				throw tmpFJIsamException;		
			}
		}
		//�e���R�[�h���Ȃ��ꍇ�A�L�[�僌�R�[�h���e���R�[�h�ɂȂ�
		if( tmpParentRec == nullptr )
		{
			tmpGtChildRec->setParentRecPos( 0L );
			//�L�[�僌�R�[�h�������R�[�h�Ɠ����ꍇ�A�����R�[�h��e���R�[�h�Ɉړ�
			if( tmpGtChildRec->Equals( inDelPos->NextPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->NextPos;
				inDelPos->NextPos = nullptr;
			//�L�[�僌�R�[�h�������R�[�h�ƈقȂ�ꍇ�A�L�[�僌�R�[�h��e���R�[�h�Ɉړ�
			}else{
				inDelPos->ParentPos = inDelPos->GtChildPos;
				inDelPos->GtChildPos = nullptr;
			}
		}else{
			tmpGtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//�L�[�僌�R�[�h�̐e���R�[�h��ݒ�

			//�e���R�[�h
			//�폜���R�[�h���L�[���Ƀ����[�V����	
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpGtChildRec->getRecPos() ); 
			//�폜���R�[�h���L�[��Ƀ����[�V����
			}else{
				tmpParentRec->setGtChildRecPos( tmpGtChildRec->getRecPos() ); 
			}	
		}

		//-- ����I�� --//
		return;
	}

	//-- �L�[��݂̂��Ȃ��ꍇ(�L�[����e���R�[�h�Ƀ����[�V����) --//
	if( inRec->getLtChildRecPos() > 0 && 
		inRec->getGtChildRecPos() == 0 )
	{
		//�񕪖؃L�[�����R�[�h�ݒ�
		if( inRec->getLtChildRecPos() == inRec->getPrevRecPos() )		//�����R�[�h�Ɠ���
		{
			tmpLtChildRec = inDelPos->PrevPos;
		}else{															//��L�ȊO
			try
			{
				tmpRec = getRecHeader( inRec->getLtChildRecPos() );
				inDelPos->LtChildPos = tmpRec;
				tmpLtChildRec = tmpRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"�폜�񕪖؃L�[�����R�[�h�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 9;
				throw tmpFJIsamException;		
			}
		}
		//�e���R�[�h���Ȃ��ꍇ�A�L�[�����R�[�h���e���R�[�h�ɂȂ�
		if( tmpParentRec == nullptr )
		{
			tmpLtChildRec->setParentRecPos( 0L );
			//�L�[�����R�[�h���O���R�[�h�Ɠ����ꍇ�A�O���R�[�h��e���R�[�h�Ɉړ�
			if( tmpLtChildRec->Equals( inDelPos->PrevPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->PrevPos;
				inDelPos->PrevPos = nullptr;
			//�L�[�����R�[�h�������R�[�h�ƈقȂ�ꍇ�A�L�[�����R�[�h��e���R�[�h�Ɉړ�
			}else{
				inDelPos->ParentPos = inDelPos->LtChildPos;
				inDelPos->LtChildPos = nullptr;
			}
		}else{
			tmpLtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//�L�[�僌�R�[�h�̐e���R�[�h��ݒ�

			//�e���R�[�h
			//�폜���R�[�h���L�[���Ƀ����[�V����
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpLtChildRec->getRecPos() ); 
			//�폜���R�[�h���L�[��Ƀ����[�V����
			}else{
				tmpParentRec->setGtChildRecPos( tmpLtChildRec->getRecPos() ); 
			}
		}

		//-- ����I�� --//
		return;
	}

	//--�񕪖؃L�[�����R�[�h���� --//
	//�񕪖؃L�[�����R�[�h�ݒ�
	if( inRec->getLtChildRecPos() == inRec->getPrevRecPos() )		//�O���R�[�h�Ɠ���
	{
		tmpLtChildRec = inDelPos->PrevPos;
	}else{															//��L�ȊO
		try
		{
			tmpRec = getRecHeader( inRec->getLtChildRecPos() );
			inDelPos->LtChildPos = tmpRec;
			tmpLtChildRec = tmpRec;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"�폜�񕪖؃L�[�����R�[�h�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;		
		}
	}

	//�񕪖؃L�[���̍ŉ��w���R�[�h(�L�[�呤������)
	tmpLtChildDep = 0;
	if( tmpLtChildRec->getGtChildRecPos() == 0 )
	{
		tmpLtChildEndRec = tmpLtChildRec;
	}else{
		try
		{
			tmpRec = getRecHeader(  tmpLtChildRec->getGtChildRecPos() );
			tmpLtChildDep++;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"�폜�񕪖؃L�[�����R�[�h�[�x�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 11;
			throw tmpFJIsamException;		
		}
		while( true )
		{
			//�񕪖؍ŉ��w�Ȃ�I��
			if(  tmpRec->getGtChildRecPos() == 0 )
			{
				tmpLtChildEndRec = tmpRec;
				break;
			}
			//���̃��R�[�h��ǂ�
			try
			{
				tmpOldRec = tmpRec;
				tmpRec = getRecHeader( tmpLtChildRec->getGtChildRecPos() );
				tmpLtChildDep++;
				delete tmpOldRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"�폜�񕪖؃L�[�����R�[�h�[�x�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 12;
				throw tmpFJIsamException;
			}
		}
	}

	//--�񕪖؃L�[�僌�R�[�h���� --//
	//�񕪖؃L�[�僌�R�[�h�ݒ�
	if( inRec->getGtChildRecPos() == inRec->getNextRecPos() )		//�����R�[�h�Ɠ���
	{
		tmpGtChildRec = inDelPos->NextPos;
		tmpGtChildEndRec = inDelPos->NextPos;
	}else{															//��L�ȊO
		try
		{
			tmpRec = getRecHeader( inRec->getGtChildRecPos() );
			inDelPos->GtChildPos = tmpRec;
			tmpGtChildRec = tmpRec;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"�폜�񕪖؃L�[�僌�R�[�h�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 13;
			throw tmpFJIsamException;		
		}
	}

	//�񕪖؃L�[���̍ŉ��w���R�[�h(�L�[����������)
	tmpGtChildDep = 0;
	if( tmpGtChildRec->getLtChildRecPos() == 0 )
	{
		tmpGtChildEndRec = tmpGtChildRec;
	}else{
		try
		{
			tmpRec = getRecHeader(  tmpLtChildRec->getLtChildRecPos() );
			tmpGtChildDep++;
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				L"�폜�񕪖؃L�[�僌�R�[�h�[�x�������ɃG���[���������܂����B", exp );
			tmpFJIsamException->ErrorCode = 1502;
			tmpFJIsamException->ErrorCode2 = 14;
			throw tmpFJIsamException;		
		}
		while( true )
		{
			//�񕪖؍ŉ��w�Ȃ�I��
			if(  tmpRec->getLtChildRecPos() == 0 )
			{
				tmpGtChildEndRec = tmpRec;
				break;
			}
			//���̃��R�[�h��ǂ�
			try
			{
				tmpOldRec = tmpRec;
				tmpRec = getRecHeader( tmpLtChildRec->getLtChildRecPos() );
				tmpLtChildDep++;
				delete tmpOldRec;
			}catch( Exception^ exp ){
				FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
					L"�폜�񕪖؃L�[�僌�R�[�h�[�x�������ɃG���[���������܂����B", exp );
				tmpFJIsamException->ErrorCode = 1502;
				tmpFJIsamException->ErrorCode2 = 15;
				throw tmpFJIsamException;
			}
		}
	}

	//-- �񕪖ؐ[�x�`�F�b�N(�ǂ��炪�e���R�[�h�ƃ����[�V�������邩) --//
	//�L�[���̕����[�x���󂢏ꍇ�A�L�[��̓L�[���̉��ɂ�
	if( tmpLtChildDep < tmpGtChildDep )
	{
		//�e���R�[�h���Z�b�g
		//�e���R�[�h���Ȃ��ꍇ�A�L�[�����R�[�h���e���R�[�h�ɂȂ�
		if( tmpParentRec == nullptr )
		{
			tmpLtChildRec->setParentRecPos( 0L );
			RecRootPos = tmpLtChildRec->getRecPos();				//���[�g���R�[�h�ɐݒ�

			//�L�[�����R�[�h���O���R�[�h�Ɠ����ꍇ�A�O���R�[�h��e���R�[�h�Ɉړ�
			if( tmpLtChildRec->Equals( inDelPos->PrevPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->PrevPos;
				inDelPos->PrevPos = nullptr;
			//�L�[�����R�[�h���O���R�[�h�ƈقȂ�ꍇ�A�L�[�����R�[�h��e���R�[�h�Ɉړ�
			}else{
				inDelPos->ParentPos = inDelPos->LtChildPos;
				inDelPos->LtChildPos = nullptr;
			}
		}else{
			tmpLtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//�L�[�僌�R�[�h�̐e���R�[�h��ݒ�
			//�e���R�[�h
			//�폜���R�[�h���L�[���Ƀ����[�V����
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpLtChildRec->getRecPos() ); 
			//�폜���R�[�h���L�[��Ƀ����[�V����
			}else{
				tmpParentRec->setGtChildRecPos( tmpLtChildRec->getRecPos() ); 
			}
		}

		//�������R�[�h�̉��ɃL�[�僌�R�[�h���ʒu�Â���
		tmpLtChildEndRec->setGtChildRecPos( tmpGtChildRec->getRecPos() );		//�L�[���̍ŉ��w�ɃL�[�僌�R�[�h��ݒ�
		tmpGtChildRec->setParentRecPos( tmpLtChildEndRec->getRecPos() );		//�L�[�僌�R�[�h�̐e���R�[�h�ɃL�[���̍ŉ��w���R�[�h��ݒ�
		
		//�g�����R�[�h���L�[�����R�[�h�Ɠ����łȂ���΁A�������ʂɐݒ�
		if( tmpLtChildEndRec->Equals( tmpLtChildRec ) == false )
		{
			inDelPos->ExtRecPos = tmpLtChildEndRec;
		}

	//�L�[��̕����[�x���󂢂��A���ғ����ꍇ�A�L�[��̓L�[���̉��ɂ�
	}else{
		//�e���R�[�h���Z�b�g
		//�e���R�[�h���Ȃ��ꍇ�A�L�[�����R�[�h���e���R�[�h�ɂȂ�
		if( tmpParentRec == nullptr )
		{
			tmpGtChildRec->setParentRecPos( 0L );
			RecRootPos = tmpGtChildRec->getRecPos();				//���[�g���R�[�h�ɐݒ�

			//�L�[�����R�[�h�������R�[�h�Ɠ����ꍇ�A�����R�[�h��e���R�[�h�Ɉړ�
			if( tmpGtChildRec->Equals( inDelPos->NextPos ) == true )
			{
				inDelPos->ParentPos = inDelPos->NextPos;
				inDelPos->NextPos = nullptr;
			//�L�[�僌�R�[�h�������R�[�h�ƈقȂ�ꍇ�A�L�[�僌�R�[�h��e���R�[�h�Ɉړ�
			}else{
				inDelPos->ParentPos = inDelPos->GtChildPos;
				inDelPos->GtChildPos = nullptr;
			}
		}else{
			tmpGtChildRec->setParentRecPos( tmpParentRec->getRecPos() );		//�L�[�僌�R�[�h�̐e���R�[�h��ݒ�
			//�e���R�[�h
			//�폜���R�[�h���L�[���Ƀ����[�V����
			if( tmpParentRec->getLtChildRecPos() == inRec->getRecPos() )
			{
				tmpParentRec->setLtChildRecPos( tmpGtChildRec->getRecPos() ); 
			//�폜���R�[�h���L�[��Ƀ����[�V����
			}else{
				tmpParentRec->setGtChildRecPos( tmpGtChildRec->getRecPos() ); 
			}
		}

		//�������R�[�h�̉��ɃL�[�僌�R�[�h���ʒu�Â���
		tmpGtChildEndRec->setGtChildRecPos( tmpLtChildRec->getRecPos() );		//�L�[��̍ŉ��w�ɃL�[�����R�[�h��ݒ�
		tmpLtChildRec->setParentRecPos( tmpGtChildEndRec->getRecPos() );		//�L�[�����R�[�h�̐e���R�[�h�ɃL�[��̍ŉ��w���R�[�h��ݒ�
		
		//�g�����R�[�h���L�[�僌�R�[�h�Ɠ����łȂ���΁A�������ʂɐݒ�
		if( tmpGtChildEndRec->Equals( tmpGtChildRec ) == false )
		{
			inDelPos->ExtRecPos = tmpGtChildEndRec;
		}
	}
}
