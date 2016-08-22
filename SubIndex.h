//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�T�u�C���f�b�N�X�@�\�[�X
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:�T�u�C���f�b�N�X
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM��`�萔
#include "FJIsamEnum.h"				//Isam�penum��`
#include "KeyInfo.h"				//�L�[���

#include "SubIndexRecord.h"			//�T�u�C���f�b�N�X���R�[�h

//------------------------------------------------------------------------------
//	class	:SubIndex	:�T�u�C���f�b�N�X
//------------------------------------------------------------------------------
namespace FJIsam
{
	ref class SubIndex
	{
	public :
		SubIndex( Object^ inParent, int inIdxNum, IsamOpenMode inMode );			//�R���X�g���N�^(����)
		SubIndex( Object^ inParent, int inIdxNum,									//�R���X�g���N�^(�V�K)
				  KeyInfo^ inKeyInfo );	
		~SubIndex();																//�f�X�g���N�^

		//--�A�N�Z�X(����)--//
		int	Close(void);															//�t�@�C���N���[�Y

		//-- �T�u�C���f�b�N�X�󋵎擾 --//
		String^ getFilePath( String^ otPath );										//�t�@�C���p�X�擾
		Int32 getBuildID( void );													//BuildID�擾
		Boolean isOpen( void );														//�t�@�C���I�[�v���`�F�b�N

		//--�T�u�C���f�b�N�X���擾--//
		String^	getFID( String^ otFID );											//�t�@�C��ID�擾
		String^	getVersion(  String^ otVer );										//ISAM�o�[�W�����擾
		UInt32 getRecCnt( void );													//���R�[�h���擾
		UInt32 getDelRecCnt( void );												//�폜���R�[�h���擾
		UInt32 getAllRecCnt( void );												//�S���R�[�h���擾
		UInt32 getKeyCnt( void );													//�L�[���擾
	
		static Int32	Length( void );												//���䕔�f�[�^�����擾

		//--���擾--//
		KeyInfo^	getKeyInfo( void );												//�L�[���擾
		String^		getKeyName(String^ otStr );										//�L�[���擾
		Int32		getKeyLen( void );												//�L�[���擾
		Int32		getKeyPos( void );												//�L�[�ʒu�擾
		keyType_EN	getKeyType( void );												//�L�[�^�C�v�擾
		Byte		getKeyOrder( void );											//�L�[����
		Byte		getKeyUniqueFlg( void );										//���j�[�N�L�[�t���O�擾
		Int32		getSrtKeyLen( void );											//�\�[�g�L�[���擾
		Int32		getSrtKeyPos( void );											//�\�[�g�L�[�ʒu�擾
		keyType_EN	getSrtKeyType( void );											//�\�[�g�L�[�^�C�v�擾
		Byte		getSrtKeyOrder( void );											//�\�[�g�L�[����

		//-- �A�N�Z�X(�ǂݏo��) --//
		SubIndexRecord^ ReadRecord( array<Byte>^ inKey );							//���R�[�h�Ǎ�
		SubIndexRecord^ FirstRecord( void );										//�ŏ��̃��R�[�h�Ǎ�
		SubIndexRecord^ LastRecord( void  );										//�Ō�̃��R�[�h�Ǎ�
		SubIndexRecord^ NextRecord( SubIndexRecord^ inRec );						//���̃f�[�^�Ǎ�
		SubIndexRecord^ PrevRecord( SubIndexRecord^ inRec );						//�O�̃f�[�^�Ǎ�
		List<SubIndexRecord^>^ ReadRecordWords( String^ inKey );					//���R�[�h�Ǎ� �����O����v

		//-- �A�N�Z�X(����) --//
		void AddRecord( SubIndexRecord^ inRec);										//���R�[�h�ǉ�
		void UpdateRecord(  array<Byte>^ inKey, array<Byte>^ inNewKey,				//���R�[�h�X�V
							Int64 inRecPos, Int64 inNewRecPos, Int64 inSubIdxPos );					
		void DeleteRecord(  array<Byte>^ inKey, Int64 inRecPos,						//���R�[�h�폜
							 Int64 inSubIdxPos );

	private :

		//*****�@�f�[�^ *****//
		//-- ����f�[�^ --//
		Object^					ParentFile;											//�eISAM�t�@�C��
		String^					FilePath;											//�g�p�t�@�C����
		Int32					IdxNum;												//�C���f�b�N�X�ԍ�
		FileStream^				FStream;											//�C���f�b�N�X�t�@�C���n���h��
		Int32					BuildID;											//�ĕҐ���N���[�Y�������s�����ꍇ�̎��ʃJ�E���^

		array<Byte>^			FileBuff;											//�t�@�C�����o�̓o�b�t�@

		//---- ���R�[�h���(�t�@�C���f�[�^�j ----//
		//-- �T�u�C���f�b�N�X������ --//
		String^			FID;														//�t�@�C��ID
		String^			ISAMVer;													//�t�@�C���쐬���C�u�����o�[�W����

		UInt32			AllRecCnt;													//�S���R�[�h��
		UInt32			KeyCnt;														//�L�[��

		UInt32			RecCnt;														//�L�����R�[�h��
		Int64			RecFirstPos;												//���X�g�擪���R�[�h�ʒu
		Int64			RecLastPos;													//���X�g�ŏI���R�[�h�ʒu
		Int64			RecRootPos;													//�񕪖؃��[�g���R�[�h�ʒu

		UInt32			DelRecCnt;													//�폜���R�[�h��
		Int64 			DelRecFirstPos;												//�폜���X�g�擪���R�[�h�ʒu
		Int64			DelRecLastPos;												//�폜���X�g�ŏI���R�[�h�ʒu

		KeyInfo^		SubKeyInfo;													//�L�[���

		//*****�@Private�֐� *****//
		//-- �T�u�C���f�b�N�X�t�@�C�������� --//
		String^	CreateFileName( String^ inFname, int inIdxNum );

		//-- �w�b�_���� --//
		void			ReadHeader( void );											//�t�@�C���������݃o�b�t�@�쐬
		void			WriteHeader( void );										//�t�@�C���ǂݏo���f�[�^�擾

		//--�A�N�Z�X(�Ǎ�)--//
		SubIndexRecord^ getRecord( Int64 inRecPos );								//���R�[�h�Ǎ�
		SubIndexRecord^ getRecHeader( Int64 inRecPos );								//���R�[�h�w�b�_�Ǐo

		//--�A�N�Z�X(���o)--//
		void WriteRecord( SubIndexRecord^ inRec );									//�T�u�C���f�b�N�X���R�[�h����
		void WriteRecHeader( SubIndexRecord^ inRec );								//�T�u�C���f�b�N�X���R�[�h�w�b�_����

		void FindInsertRecordPos( SubIndexRecord^ inRec,							//�T�u�C���f�b�N�X���R�[�h�}���ʒu����
									SubIndexRecordPos_T^ inAddPos );
		void ChkDeleteRecordPos( SubIndexRecord^ inRec,								//�T�u�C���f�b�N�X���R�[�h�폜�ʒu�`�F�b�N
									SubIndexRecordPos_T^ inDelPos );
	};

}
