//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@DataRecord �N���X�@�w�b�_
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.1.1	create date 2004/06/03
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	classes
//		DataRecord	: �f�[�^���R�[�h
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM�萔��`
#include "KeyInfo.h"				//�L�[���
#include "SubIndexRecord.h"			//�T�u�C���f�b�N�X���R�[�h
#include "FJIsamEnum.h"				//Isam�pEnum��`

//------------------------------------------------------------------------------
//	class	:DataRecord	: �f�[�^���R�[�h
//------------------------------------------------------------------------------
namespace FJIsam
{

	public ref class DataRecord
	{
	public :

		DataRecord();														//�R���X�g���N�^(�V�K���R�[�h)
		DataRecord( array<Byte>^ inBuff,
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec );
		DataRecord( DataRecord^ inRec );									//�R�s�[�R���X�g���N�^
		~DataRecord();														//�f�X�g���N�^
	
		//-- �f�[�^���R�[�h��� --//
		Int64 getRecPos( void );											//���R�[�h�ʒu�擾
		Object^ getParentFile( void );										//Isam�t�@�C���擾
		Int32 getBuildID( void );											//BuildID�擾
		Int32 getRecLength( void );											//���R�[�h���擾(�S��)
		Int32 getDataLength( void );										//�f�[�^���擾
		void setRecPos( Int64 inRecPos );									//���R�[�h�ʒu�ݒ�
		void setParentFile( Object^ inParent );
		Boolean isHeaderOnly( void );										//���R�[�h�Ǎ��`�F�b�N

		static Int32 getHeaderLength( void );								//���R�[�h�w�b�_���擾

		//-- �C���f�b�N�X��� --//
		Int32 getIdxNum( void );											//�C���f�b�N�X�ԍ��擾
		SubIndexRecord^ getSubIdxRec( void );								//�T�u�C���f�b�N�X���R�[�h�擾

		//-- ���R�[�h��Ԏ擾 --//
		Byte getDelFlg( void );												//�폜�t���O�擾
		Byte getEachKeyFlg( void );											//����L�[�t���O�擾
		DataRecStat_EN getRecStat( void );									//���R�[�h�̏�Ԏ擾
		Int32 DeleteRec( void );											//���R�[�h�폜
		Int32 setEachKeyFlg( Byte inEachKeyFlg );							//����L�[�t���O�ݒ�
		Int32 setRecStat( DataRecStat_EN inStat );							//���R�[�h�̏�Ԑݒ�

		//-- ���R�[�h�ʒu��� --//
		Int64 getPrevRecPos( void );										//�O���R�[�h�ʒu�擾
		Int64 getNextRecPos( void );										//�����R�[�h�ʒu�擾
		Int64 getParentRecPos( void );										//�񕪖ؐe���R�[�h�ʒu�擾
		Int64 getLtChildRecPos( void );										//�񕪖؃L�[�����R�[�h�ʒu�擾
		Int64 getGtChildRecPos( void );										//�񕪖؃L�[�僌�R�[�h�ʒu�擾
		
		Int32 setPrevRecPos( Int64 inRecPos );								//�O���R�[�h�ʒu�ݒ�
		Int32 setNextRecPos( Int64 inRecPos);								//�����R�[�h�ʒu�ݒ�
		Int32 setParentRecPos( Int64 inRecPos );							//�񕪖ؐe���R�[�h�ʒu�ݒ�
		Int32 setLtChildRecPos( Int64 inRecPos );							//�񕪖؃L�[�����R�[�h�ʒu�ݒ�
		Int32 setGtChildRecPos( Int64 inRecPos );							//�񕪖؃L�[�僌�R�[�h�ʒu�ݒ�


		//-- �f�[�^�A�N�Z�X --//
		array<Byte>^ getData( array<Byte>^ otBuff );						//�f�[�^�擾
		void	 setData( array<Byte>^ inBuff, Int32 inLength );			//�f�[�^�ݒ�
		array<Byte>^ getKeyData( array<Byte>^ otBuff, KeyInfo^ inKeyInfo,	//�L�[�f�[�^�擾
								Boolean KeyOnly );
		array<Byte>^ getSrtKeyData( array<Byte>^ otBuff,					//�\�[�g�L�[�f�[�^�擾
								KeyInfo^ inKeyInfo );
		//-- ���R�[�h�f�[�^�A�N�Z�X --//
		array<Byte>^ WriteRecData( array<Byte>^ otWriter,					//���R�[�h�f�[�^����
									Boolean inHeaderOnly );		
	
	private :
		//====�@�f�[�^ ====//
		//-- �Ǘ���� --//
		Object^			ParentFile;								//ISAM
		Int32			BuildID;								//�ĕҐ���N���[�Y�������s�����ꍇ�̎��ʃJ�E���^
		DataRecStat_EN	RecStat;								//���R�[�h�̏��
		Boolean			IsHeaderOnly;							//���R�[�h�Ǎ����(�w�b�_�̂݁A�f�[�^�܂�)
		Int64			RecPos;									//���݂̃��R�[�h�ʒu

		Int32 			IdxNum;									//�C���f�b�N�X�ԍ�
		SubIndexRecord^	IdxRec;									//�C���f�b�N�X���R�[�h

		//-- ���R�[�h�w�b�_���i�t�@�C���f�[�^�j--//
		Int32			DataLen;								//���R�[�h�f�[�^�̃T�C�Y
		Byte			DelFLG;									//�폜�t���O
		Byte			EachKeyFLG;								//����L�[�t���O
		//���X�g�p�|�C���^
		Int64			PrevRecPos;								//�O���R�[�h�ʒu
		Int64			NextRecPos;								//�ヌ�R�[�h�ʒu
		//2���ؗp�|�C���^
		Int64			ParentRecPos;							//�e�؃��R�[�h�ʒu
		Int64			LtChildRecPos;							//�q�؏����R�[�h�ʒu
		Int64			GtChildRecPos;							//�q�ؑ僌�R�[�h�ʒu

		//-- ���R�[�h�f�[�^(�t�@�C���f�[�^�j --//
		array<Byte>^	Data;									//�f�[�^�̈�

		//-- private�֐� --//
		void Set( array<Byte>^ inBuff,							//�f�[�^���R�[�h������
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec );

	};

	//�f�[�^���R�[�h�����[�V�����������
	public ref struct DataRecordPos_T
	{
		DataRecord^ PrevPos;				//�O���R�[�h
		DataRecord^ NextPos;				//�����R�[�h
		DataRecord^ ParentPos;				//�񕪖ؐe���R�[�h
		DataRecord^ LtChildPos;				//�񕪖؃L�[�����R�[�h
		DataRecord^ GtChildPos;				//�񕪖؃L�[�僌�R�[�h
		DataRecord^ ExtRecPos;				//�f�[�^�폜�����A�ԐړI�Ƀ����[�V���������������R�[�h
	};

}
