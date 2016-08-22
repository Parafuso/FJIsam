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
//  class	:SubIndex	:�T�u�C���f�b�N�X(�����֐�:���R�[�h�ǉ�)
//------------------------------------------------------------------------------
void FJIsam::SubIndex::AddRecord( SubIndexRecord^ inRec)
{
	SubIndexRecordPos_T^ tmpAddPos;			//�f�[�^���R�[�h�����[�V����

	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inRec->isHeaderOnly() == true )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�̃f�[�^��������܂���B" );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( inRec->getRecStat() != DataRecStat_EN::REC_NEW )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃T�u�C���f�b�N�X���R�[�h�͒ǉ��ł��܂���B" );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	if( FStream->CanWrite == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���̃t�@�C���͏����݂�������܂���B" );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	//-- �f�[�^������ --//
	tmpAddPos = gcnew SubIndexRecordPos_T;

	//-- ���R�[�h��{���ݒ� --//
	try
	{
		inRec->setParentFile( this );					//�T�u�C���f�b�N�X�t�@�C���ݒ�
		inRec->setRecPos( FStream->Length );			//���R�[�h�ǉ��ʒu�ݒ�

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h��{���ݒ莞�ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	//-- ���R�[�h�ǉ��ʒu����(���X�g�A�񕪖�) --//
	try
	{
		FindInsertRecordPos( inRec, tmpAddPos );

	}catch( FJIsamException^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"���R�[�h�ǉ��ʒu�����ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;		
	}

	//-- ���R�[�h���� --//
	try
	{
		WriteRecord( inRec );					//���R�[�h���t�@�C���ɏ���
		inRec->setRecStat( DataRecStat_EN::REC_OLD );			//���R�[�h��Ԃ�ݒ�

	}catch( FJIsamException^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�T�u�C���f�b�N�X���R�[�h�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;		
	}

	//-- �����[�V�������郌�R�[�h�w�b�_������ --//
	try
	{
		//�O���R�[�h
		if( tmpAddPos->PrevPos != nullptr )				
		{
			WriteRecHeader( tmpAddPos->PrevPos );
		}
		//�����R�[�h
		if( tmpAddPos->NextPos != nullptr )				
		{
			WriteRecHeader( tmpAddPos->NextPos );
		}
		//�񕪖ؐe���R�[�h
		if( tmpAddPos->ParentPos != nullptr )
		{
			WriteRecHeader( tmpAddPos->ParentPos );
		}
		//�񕪖؃L�[�����R�[�h
		if( tmpAddPos->LtChildPos != nullptr )
		{
			WriteRecHeader( tmpAddPos->LtChildPos );
		}
		//�񕪖؃L�[�僌�R�[�h
		if( tmpAddPos->GtChildPos != nullptr )
		{
			WriteRecHeader( tmpAddPos->GtChildPos );
		}
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�����[�V�������R�[�h�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;		
	}

	//-- �T�u�C���f�b�N�X�w�b�_������ --//	
	AllRecCnt++;									//�S���R�[�h�J�E���g�A�b�v
	if( inRec->getEachKeyFlg() == IS_NOEACHKEY )	//�L�[�J�E���g�A�b�v	
	{
		KeyCnt++;
	}
	RecCnt++;										//�L�����R�[�h�J�E���g�A�b�v
	if( RecCnt == 1 )
	{
		RecRootPos = inRec->getRecPos();			//���[�g���R�[�h
		RecFirstPos = inRec->getRecPos();			//�擪���R�[�h
		RecLastPos = inRec->getRecPos();			//�Ōヌ�R�[�h
	}else{
		if( inRec->getEachKeyFlg() == IS_EACHKEYFST &&
			inRec->getNextRecPos() == RecRootPos )	//���[�g���R�[�h
		{
			RecRootPos = inRec->getRecPos();
		}
		if( RecFirstPos == inRec->getNextRecPos() )	//�擪���R�[�h
		{
			RecFirstPos = inRec->getRecPos();
		}
		if( RecLastPos == inRec->getPrevRecPos() )	//�Ōヌ�R�[�h
		{
			RecLastPos = inRec->getRecPos();
		}
	}

	try
	{
		WriteHeader();
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
			L"�T�u�C���f�b�N�X�w�b�_�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3301;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;		
	}

	//-- �f�[�^�폜 --//
	if( tmpAddPos->PrevPos != nullptr ) delete tmpAddPos->PrevPos; 
	if( tmpAddPos->NextPos != nullptr ) delete tmpAddPos->NextPos; 
	if( tmpAddPos->ParentPos != nullptr ) delete tmpAddPos->ParentPos; 
	if( tmpAddPos->LtChildPos != nullptr ) delete tmpAddPos->LtChildPos; 
	if( tmpAddPos->GtChildPos != nullptr ) delete tmpAddPos->GtChildPos; 
	delete tmpAddPos;

	//-- ����I�� --//
	return;
}

//------------------------------------------------------------------------------
//***** ���R�[�h�}���ʒu����(Private)
void FJIsam::SubIndex::FindInsertRecordPos( SubIndexRecord^ inRec, SubIndexRecordPos_T^ inAddPos )
{
	SubIndexRecord^		tmpRec;				//�������R�[�h
	SubIndexRecord^		tmpOldRec;			//���������R�[�h

	array<Byte>^		chkKey;				//�����L�[���
	array<Byte>^		chkSrtKey;			//�����p�\�[�g�L�[���

	Int64				chkRecPos;			//�`�F�b�N�p���R�[�h�ʒu

	Int32				chkVal;				//�L�[�`�F�b�N����
	
	//-- �����`�F�b�N --//
	if( inRec == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�����ݒ�ł��B" );
		tmpFJIsamException->ErrorCode = 3302;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	if( inAddPos == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�����[�V����������񂪖��ݒ�ł��B" );
		tmpFJIsamException->ErrorCode = 3302;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}


	//-- �f�[�^������ --//
	inAddPos->PrevPos = nullptr;
	inAddPos->NextPos = nullptr;
	inAddPos->ParentPos = nullptr;
	inAddPos->LtChildPos = nullptr;
	inAddPos->GtChildPos = nullptr;
	tmpOldRec = nullptr;

	//-- ISAM��1���ڂ̃��R�[�h�Ȃ�ISAM�ł̃��R�[�h�ʒu���Z�b�g���ďI�� --//
	if( RecCnt == 0 )
	{
		RecRootPos = inRec->getRecPos();
		RecFirstPos = inRec->getRecPos();
		RecLastPos = inRec->getRecPos();

		return;
	}

	//-- �`�F�b�N�p�ϐ������� --//
	try
	{		
		chkKey = inRec->getKeyData( nullptr );
		chkSrtKey = inRec->getSrtKeyData( nullptr);

		chkRecPos = RecRootPos;
		tmpOldRec = nullptr;

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�ǉ��ʒu������񏉊������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3302;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;		
	}

	//-- �񕪖ؑ}���ʒu���� --//
	try
	{
		tmpRec = getRecord( RecRootPos );
	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�񕪖؃��[�g���R�[�h�̎擾�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3302;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;		
	}

	try
	{
		while( true )
		{
			chkVal = FJIFuncs::KeyCompare( chkKey, tmpRec->getKeyData( nullptr ), SubKeyInfo, IdxNum, KeyCmpOpt_EN::Key );

			//�}�����R�[�h�̃L�[�ƌ������R�[�h�̃L�[������(�񕪖،����I��)
			if( chkVal == 0 )
			{
				inAddPos->ParentPos = tmpRec;
				break;

			//�}�����R�[�h�̃L�[��������(�񕪖؃L�[�����R�[�h��Ǎ�)
			}else if( chkVal < 0 ){
				chkRecPos = tmpRec->getLtChildRecPos();

			//�}�����R�[�h�̃L�[��������(�񕪖؃L�[�僌�R�[�h��Ǎ�)
			}else{
				chkRecPos = tmpRec->getGtChildRecPos(); 
			}

			//�񕪖؂̎q���R�[�h�������Ȃ�ΏI��
			if( chkRecPos == 0L )
			{
				inAddPos->ParentPos = tmpRec;
				break;
			}
	
			//���̃��R�[�h������
			delete tmpOldRec;								//�O�񌟍��̃��R�[�h���폜
			tmpOldRec = tmpRec;								//�������R�[�h���o�b�N�A�b�v
			tmpRec = getRecord( chkRecPos );				//���̃��R�[�h��Ǎ�
		}

		//-- �}�����R�[�h�̃L�[������L�[�̏ꍇ�̏��� --//
		//�\�[�g�L�[�𒲂ׂ�
		if( chkVal == 0 )
		{
			chkVal = FJIFuncs::KeyCompare( chkSrtKey, tmpRec->getSrtKeyData( nullptr ), SubKeyInfo, IdxNum, KeyCmpOpt_EN::Srt );

			//�\�[�g�L�[���������ꍇ�A���R�[�h�͓���L�[�̐擪�ɂȂ�B
			if( chkVal < 0 )
			{
				//���R�[�h�̏󋵂��Z�b�g
				inRec->setEachKeyFlg( IS_EACHKEYFST );							//����L�[�t���O
				inRec->setPrevRecPos( tmpRec->getPrevRecPos() );				//�O���R�[�h�ʒu
				inRec->setNextRecPos( tmpRec->getRecPos() );					//�����R�[�h�ʒu
				inRec->setParentRecPos( tmpRec->getParentRecPos() );		//�񕪖ؐe���R�[�h�ʒu
				inRec->setLtChildRecPos( tmpRec->getLtChildRecPos() );			//�񕪖؃L�[�����R�[�h�ʒu
				inRec->setGtChildRecPos( tmpRec->getGtChildRecPos() );			//�񕪖؃L�[�僌�R�[�h�ʒu
				inAddPos->NextPos = tmpRec;

				//�e���R�[�h��ݒ�
				if( tmpOldRec != nullptr )
				{
					inAddPos->ParentPos = tmpOldRec;
					//�e���R�[�h�̓񕪖؂̎q���R�[�h��ݒ�
					if( inAddPos->ParentPos->getLtChildRecPos() == tmpRec->getRecPos() )
					{
						inAddPos->ParentPos->setLtChildRecPos( inRec->getRecPos() );
					}else{
						inAddPos->ParentPos->setGtChildRecPos( inRec->getRecPos() );
					}

					//�e���R�[�h�̎����R�[�h�������R�[�h�Ȃ�A�������R�[�h���Z�b�g
					if( inAddPos->ParentPos->getNextRecPos() == inAddPos->NextPos->getRecPos() )
					{
						inAddPos->ParentPos->setNextRecPos( inRec->getRecPos() );
					}
				}

				//�O���R�[�h��ݒ�
				//�擪�̃��R�[�h���`�F�b�N
				if( inRec->getPrevRecPos() != 0L )	
				{
					//�O���R�[�h���ʂŁA���݂���Ȃ�O���R�[�h��Ǎ��A�O���R�[�h�̎����R�[�h�ɑ}�����R�[�h���Z�b�g
					if( inRec->getParentRecPos() != inRec->getPrevRecPos() )
					{
						inAddPos->PrevPos = getRecHeader( inRec->getPrevRecPos() );	
						inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );
					}
				}
				
				//�����R�[�h(����L�[���R�[�h)��ݒ�
				if( tmpRec->getEachKeyFlg() == IS_EACHKEYFST )		//����L�[�t���O
				{
					tmpRec->setEachKeyFlg( IS_EACHKEYMID );
				}else if( tmpRec->getEachKeyFlg() == IS_NOEACHKEY ){
					tmpRec->setEachKeyFlg( IS_EACHKEYLST );
				}
				tmpRec->setPrevRecPos( inRec->getRecPos() );		//�O���R�[�h�͑}�����R�[�h
				tmpRec->setParentRecPos( 0L );						//�񕪖ؐe���R�[�h����
				tmpRec->setLtChildRecPos( 0L );						//�񕪖؃L�[�����R�[�h����
				tmpRec->setGtChildRecPos( 0L );						//�񕪖؃L�[�僌�R�[�h����

				//�񕪖؃L�[�����R�[�h��ݒ�
				if( inRec->getLtChildRecPos() != 0L )
				{
					if( inRec->getLtChildRecPos() != inRec->getPrevRecPos() || 
						inAddPos->PrevPos == nullptr )
					{
						inAddPos->LtChildPos = getRecHeader( inRec->getLtChildRecPos() );	//�񕪖؃L�[�����R�[�h�Ǎ�
						inAddPos->LtChildPos->setParentRecPos( inRec->getRecPos() );		//�e���R�[�h�ɐݒ�
					}else{
						inAddPos->PrevPos->setParentRecPos( inRec->getRecPos() );
					}
				}

				//�񕪖؃L�[�僌�R�[�h��ݒ�
				if( inRec->getGtChildRecPos() != 0L )
				{
					inAddPos->GtChildPos = getRecHeader( inRec->getGtChildRecPos() );	//�񕪖؃L�[�僌�R�[�h�Ǎ�
					inAddPos->GtChildPos->setParentRecPos( inRec->getRecPos() );		//�e���R�[�h�ɐݒ�
				}

			//����L�[�̐擪�ł͂Ȃ��ꍇ
			}else{
				//�}�����R�[�h��ݒ�
				inAddPos->ParentPos = nullptr;
				inRec->setParentRecPos( 0L );
				inRec->setLtChildRecPos( 0L );
				inRec->setGtChildRecPos( 0L );
				
				while( true )
				{
					//����L�[�t���O������L�[�Ō�A�܂��͓���L�[�����Ȃ猟���I��
					if( tmpRec->getEachKeyFlg() == IS_EACHKEYLST ||
						tmpRec->getEachKeyFlg() == IS_NOEACHKEY )
					{
						//�������R�[�h��ݒ�
						inRec->setPrevRecPos( tmpRec->getRecPos() );				//�O���R�[�h�ɐݒ�
						inRec->setNextRecPos( tmpRec->getNextRecPos() );			//�����R�[�h��ݒ�
						inRec->setEachKeyFlg( IS_EACHKEYLST );						//����L�[�t���O���Ō���ɐݒ�
						
						//�O���R�[�h��ݒ�
						inAddPos->PrevPos = tmpRec;
						if( tmpRec->getEachKeyFlg() == IS_EACHKEYLST )
						{
							inAddPos->PrevPos->setEachKeyFlg( IS_EACHKEYMID );		//����L�[�t���O�F�^��
						}else{
							inAddPos->PrevPos->setEachKeyFlg( IS_EACHKEYFST );		//����L�[�t���O�F�Ō�
						}
						inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );		//�������R�[�h�������R�[�h�ɐݒ�
						
						//�����R�[�h��ݒ�
						if( inRec->getNextRecPos() != 0L )
						{
							if( tmpOldRec == nullptr )
							{
								inAddPos->NextPos = getRecHeader( inRec->getNextRecPos() );

							}else if( inRec->getNextRecPos() == tmpOldRec->getRecPos() ) {
								inAddPos->NextPos = tmpOldRec;

							}else{
								inAddPos->NextPos = getRecHeader( inRec->getNextRecPos() );
							}
							inAddPos->NextPos->setPrevRecPos( inRec->getRecPos() );
						}
						break;
					}

					//���̃��R�[�h���r
					delete tmpOldRec;
					tmpOldRec = tmpRec;
					tmpRec = getRecord( tmpOldRec->getNextRecPos() );
	
					//�Ǎ����R�[�h�̃L�[��菬�����Ȃ�ΏI��
					if( FJIFuncs::KeyCompare( chkSrtKey, tmpRec->getSrtKeyData( nullptr ), SubKeyInfo, IdxNum, KeyCmpOpt_EN::Srt ) < 0 )
					{
						//�}�����R�[�h��ݒ�
						inRec->setEachKeyFlg( IS_EACHKEYMID );
						inRec->setPrevRecPos( tmpOldRec->getRecPos() );
						inRec->setNextRecPos( tmpRec->getRecPos() );

						//�O���R�[�h��ݒ�
						inAddPos->PrevPos = tmpOldRec;
						inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );

						//�����R�[�h��ݒ�
						inAddPos->NextPos = tmpRec;
						inAddPos->NextPos->setPrevRecPos( inRec->getRecPos() );
		
						break;
					}
				}
			}
		//-- �}�����R�[�h�̃L�[���������ꍇ�A�e���R�[�h�̑僌�R�[�h�A�����R�[�h�ɐݒ� --//
		}else if( chkVal < 0 ){
			//�e���R�[�h��ݒ�
			tmpRec->setLtChildRecPos( inRec->getRecPos() );				//�񕪖؃L�[�����R�[�h�ɐݒ�

			//�}�����R�[�h��ݒ�
			inRec->setParentRecPos( tmpRec->getRecPos() );						//�e���R�[�h��ݒ�
			inRec->setPrevRecPos( tmpRec->getPrevRecPos() );					//�O���R�[�h��ݒ�
			inRec->setNextRecPos( tmpRec->getRecPos() );						//�����R�[�h��ݒ�
			
			//�O���R�[�h��ݒ�
			if( inRec->getPrevRecPos() != 0L )
			{
				inAddPos->PrevPos = getRecHeader( inRec->getPrevRecPos() );		//�O���R�[�h�擾
				inAddPos->PrevPos->setNextRecPos( inRec->getRecPos() );			//�����R�[�h�ɐݒ�
			}

			//�����R�[�h��ݒ�
			tmpRec->setPrevRecPos( inRec->getRecPos() );				//�O���R�[�h�ɐݒ�

		//�}�����R�[�h�̃L�[���傫���ꍇ�A�e���R�[�h�̑僌�R�[�h�A�����R�[�h�ɐݒ� --//
		}else{

			//�e���R�[�h��ݒ�
			tmpRec->setGtChildRecPos( inRec->getRecPos() );						//�񕪖؃L�[�僌�R�[�h�ɐݒ�

			//�}�����R�[�h��ݒ�
			inRec->setParentRecPos( tmpRec->getRecPos() );						//�e���R�[�h��ݒ�
			while( true )
			{
				if( tmpRec->getEachKeyFlg() == IS_NOEACHKEY ||
					tmpRec->getEachKeyFlg() == IS_EACHKEYLST )
				{
					break;
				}
				delete tmpOldRec;
				tmpOldRec = tmpRec;
				tmpRec = getRecord( tmpOldRec->getNextRecPos() );
			}
			inRec->setPrevRecPos( tmpRec->getRecPos() );						//�O���R�[�h��ݒ�
			inRec->setNextRecPos( tmpRec->getNextRecPos() );					//�����R�[�h��ݒ�
			
			//�O���R�[�h��ݒ�
			tmpRec->setNextRecPos( inRec->getRecPos() );						//�����R�[�h�ɐݒ�
			if( inAddPos->ParentPos != tmpRec )									//�e���R�[�h�ƑO���R�[�h���قȂ�ΑO���R�[�h��ݒ�
			{
				inAddPos->PrevPos = tmpRec;
			}

			//�����R�[�h��ݒ�
			if( inRec->getNextRecPos() != 0L )
			{
				inAddPos->NextPos = getRecHeader( inRec->getNextRecPos() );		//�����R�[�h�擾
				inAddPos->NextPos->setPrevRecPos( inRec->getRecPos() );			//�O���R�[�h�ɐݒ�
			}
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�f�[�^���R�[�h�}���ʒu�������ɃG���[���������܂����B", exp );
		tmpFJIsamException->ErrorCode = 3302;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;		
	}

	delete chkKey;
	delete chkSrtKey;

	//-- ����I�� --//
	return;
}

