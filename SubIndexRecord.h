//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�T�u�C���f�b�N�X���R�[�h�@�N���X�@�\�[�X
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndexRecord	:�T�u�C���f�b�N�X���R�[�h
//------------------------------------------------------------------------------
#pragma	once

#include "FJIsamDef.h"			//ISAM�萔��`
#include "FJIsamEnum.h"			//Isam�pEnum��`

//------------------------------------------------------------------------------
//	class	:SubIndexRecord	:�T�u�C���f�b�N�X���R�[�h
//------------------------------------------------------------------------------
namespace FJIsam
{
	public ref class SubIndexRecord
	{
	public :
		SubIndexRecord( Object^ inParent );									//�R���X�g���N�^(�V�K)
		SubIndexRecord( array<Byte>^ inBuff,									//�R���X�g���N�^(����)
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly );
		SubIndexRecord( SubIndexRecord^ inRec );							//�R�s�[�R���X�g���N�^
		~SubIndexRecord();													//�f�X�g���N�^

		//-- �T�u�C���f�b�N�X���R�[�h��� --//
		Int64 getRecPos( void );											//���R�[�h�ʒu�擾
		Object^ getParentFile( void );										//�T�u�C���f�b�N�X�t�@�C���擾
		Int32 getBuildID( void );											//BuildID�擾
		Int32 getRecLength( void );											//���R�[�h���擾(�S��)
		Int32 getDataLength( void );										//�f�[�^���擾
		void setRecPos( Int64 inRecPos );									//���R�[�h�ʒu�ݒ�
		void setParentFile( Object^ inParent );								//�T�u�C���f�b�N�X�t�@�C���ݒ�
		Boolean isHeaderOnly( void );										//���R�[�h�Ǎ��`�F�b�N

		static Int32 getHeaderLength( void );								//���R�[�h�w�b�_���擾

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
		Int64 getDataRecPos( void );										//�f�[�^���R�[�h�{�̈ʒu�擾

		Int32 setPrevRecPos( Int64 inRecPos );								//�O���R�[�h�ʒu�ݒ�
		Int32 setNextRecPos( Int64 inRecPos);								//�����R�[�h�ʒu�ݒ�
		Int32 setParentRecPos( Int64 inRecPos );							//�񕪖ؐe���R�[�h�ʒu�ݒ�
		Int32 setLtChildRecPos( Int64 inRecPos );							//�񕪖؃L�[�����R�[�h�ʒu�ݒ�
		Int32 setGtChildRecPos( Int64 inRecPos );							//�񕪖؃L�[�僌�R�[�h�ʒu�ݒ�
		Int32 setDataRecPos( Int64 inRecPos );								//�f�[�^���R�[�h�{�̈ʒu�ݒ�

		//-- �f�[�^�A�N�Z�X --//
		array<Byte>^ getKeyData( array<Byte>^ otBuff );						//�L�[�f�[�^�擾
		array<Byte>^ getSrtKeyData( array<Byte>^ otBuff );					//�\�[�g�f�[�^�擾
		void setKeyData( array<Byte>^ inBuff );								//�L�[�f�[�^�ݒ�
		void setSrtKeyData( array<Byte>^ inBuff );							//�\�[�g�f�[�^�ݒ�

		//-- ���R�[�h�f�[�^�A�N�Z�X --//
		array<Byte>^ WriteRecData( array<Byte>^ otBuff,						//���R�[�h�f�[�^����
						Boolean inHeaderOnly );


	private :
		//====�@�f�[�^ ====//
		//-- �Ǘ���� --//
		Object^			ParentFile;											//�T�u�C���f�b�N�X�t�@�C��
		Int32			BuildID;											//�ĕҐ���N���[�Y�������s�����ꍇ�̎��ʃJ�E���^
		DataRecStat_EN	RecStat;											//���R�[�h�̏��
		Boolean			IsHeaderOnly;										//�Ǎ����(�w�b�_�̂݁A�f�[�^�܂�)
		Int64			RecPos;												//���݂̃��R�[�h�ʒu

		//-- ���R�[�h�w�b�_���i�t�@�C���f�[�^�j--//
		Byte			DelFLG;												//�폜�t���O
		Byte			EachKeyFLG;											//����L�[�t���O
		//���X�g�p�|�C���^
		Int64			PrevRecPos;											//�O���R�[�h�ʒu
		Int64			NextRecPos;											//�ヌ�R�[�h�ʒu
		//�j���ؗp�|�C���^
		Int64			ParentRecPos;										//�e�؃��R�[�h�ʒu
		Int64			LtChildRecPos;										//�q�؏����R�[�h�ʒu
		Int64			GtChildRecPos;										//�q�ؑ僌�R�[�h�ʒu
		//�f�[�^���R�[�h���
		Int64			DataRecPos;											//�f�[�^���R�[�h�{�̂̈ʒu

		//-- ���R�[�h�f�[�^ --//
		array<Byte>^	KeyData;											//�L�[�f�[�^
		array<Byte>^	SrtKeyData;											//�\�[�g�L�[�f�[�^
		
		//-- Private�֐� --//
		void Set( array<Byte>^ inReader,											//���R�[�h�Z�b�g
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly );

	};

	//�T�u�C���f�b�N�X���R�[�h�����[�V���������p�\����
	public ref struct SubIndexRecordPos_T
	{
		SubIndexRecord^ PrevPos;
		SubIndexRecord^ NextPos;
		SubIndexRecord^ ParentPos;
		SubIndexRecord^ LtChildPos;
		SubIndexRecord^ GtChildPos;
		SubIndexRecord^ ExtRecPos;
	};
}
