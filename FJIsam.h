// FJIsam.h
//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u���� Main
//	Version	1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//  Version 1.2.1	Change Date 2011/07/08 FJIFunc::KeyStringConv �C���i'��󔒂𖳎�����)
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		FJIsam	:ISAM �{��
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM�萔��`
#include "KeyInfo.h"				//�L�[���
#include "SubIndex.h"				//�T�u�C���f�b�N�X
#include "DataRecord.h"				//���R�[�h���
#include "FJIsamEnum.h"				//Isam�pEnum��`

namespace FJIsam
{
	public ref class FIsam
	{
	public:
		FIsam( void );															//�R���X�g���N�^
		FIsam( String^ inFname, Int32 inBufsz, IsamOpenMode inMode );
		~FIsam();																//�f�X�g���N�^

		//-- �t�@�C���A�N�Z�X ��{ --//
		Int32	Open( String^ inFname, Int32 inBufsz, IsamOpenMode inMode );	//�t�@�C���I�[�v��
		Int32	Close(void);													//�t�@�C���N���[�Y

		//--�@�A�N�Z�X(�쐬�E�Ǘ�) --//
		Int32	Create( String^ inFname, String^ inFID,							//ISAM�쐬
				KeyInfo^ inKeyInfo, String^ inUserInfo,
				Int32 inBufsz,
			    System::Security::AccessControl::FileSecurity^ inFileSecurity);						
		Int32	CreateSubIndex( KeyInfo^ inKeyInfo );							//�T�u�C���f�b�N�X�쐬
//		Int32	reorganization(void);											//ISAM�ĕҐ�

		//-- ISAM�󋵎擾 --//
		String^ getFilePath( String^ otPath );									//�t�@�C���p�X�擾
		Int32 getBuildID( void );												//BuildID�擾
		Boolean isOpen( void );													//�t�@�C���I�[�v���`�F�b�N

		//--ISAM���擾--//
		String^	getFID( String^ otFID );										//�t�@�C��ID�擾
		String^	getVersion(  String^ otVer );									//ISAM�o�[�W�����擾
		String^	getUserInfo( String^ otUInfo );									//���[�U�[���擾
		UInt32 getRecCnt( void );												//���R�[�h���擾
		UInt32 getDelRecCnt( void );											//�폜���R�[�h���擾
		UInt32 getAllRecCnt( void );											//�S���R�[�h���擾
		UInt32 getKeyCnt( void );												//�L�[���擾
		Int32 getRecMaxLen( void );												//���R�[�h�ő咷
		Int32 getRecMinLen( void );												//���R�[�h�ۏ���
		Int32 getIdxCnt( void );												//�C���f�b�N�X���擾
		Int32 getKeyStartPos( void );											//�L�[�̍ŏ��̈ʒu�擾
		Int32 getKeyLastPos( void );											//�L�[�̍Ō�̈ʒu�擾
		static Int32	Length( void );											//���䕔�f�[�^���擾

		//-- �L�[���擾 --//
		Int32		getIdxNum( String^ inKeyName );								//�C���f�b�N�X�ԍ��擾
		KeyInfo^	getKeyInfo( Int32 inIdxNum );								//�L�[���擾
		String^		getKeyName( Int32 inIdxNum, String^ otStr );				//�L�[���擾
		Int32		getKeyPos( Int32 inIdxNum );								//�L�[�ʒu�擾
		Int32		getKeyLen( Int32 inIdxNum );								//�L�[���擾
		keyType_EN	getKeyType( Int32 inIdxNum );								//�L�[�^�C�v�擾	
		Byte		getKeyOrder( Int32 inIdxNum );								//�L�[�����擾
		Byte		getKeyUniqueFlg( Int32 inIdxNum );							//���j�[�N�L�[�t���O�擾
		Int32		getSrtKeyPos( Int32 inIdxNum );								//�\�[�g�L�[�ʒu�擾
		Int32		getSrtKeyLen( Int32 inIdxNum );								//�\�[�g�L�[���擾
		keyType_EN	getSrtKeyType( Int32 inIdxNum );							//�\�[�g�L�[�^�C�v�擾	
		Byte		getSrtKeyOrder( Int32 inIdxNum );							//�\�[�g�L�[�����擾

		//-- ISAM��񏑍� --//
		Int32 setUserInfo( String^ inUInfo );									//���[�U�[���ݒ�

		//--�A�N�Z�X(�ǂݏo��)--//
		DataRecord^ ReadRecord( array<Byte>^ inKey, Int32 inIdxNum );			//���R�[�h�Ǎ�
		DataRecord^ FirstRecord( Int32 inIdxNum );								//�ŏ��̃��R�[�h�Ǎ�
		DataRecord^ LastRecord( Int32 inIdxNum );								//�Ō�̃��R�[�h�Ǎ�
		DataRecord^ NextRecord( DataRecord^ inRec );							//���̃f�[�^�Ǎ�
		DataRecord^ PrevRecord( DataRecord^ inRec );							//�O�̃f�[�^�Ǎ�
		List<DataRecord^>^ ReadRecordWords( String^ inKey, Int32 inIdxNum );	//���R�[�h�Ǎ� �����O����v

		//--�A�N�Z�X(��������)--//
		void AddRecord( DataRecord^ inRec);										//���R�[�h�ǉ�
		void UpdateRecord( DataRecord^ inRec );									//���R�[�h�㏑
		void DeleteRecord( DataRecord^ inRec);									//���R�[�h�폜
	
	private :
		//*****�@�f�[�^ *****//
		//-- ����f�[�^ --//
		String^					FilePath;										//�g�p�t�@�C����
		FileStream^				FStream;										//ISAM�t�@�C���X�g���[��
		Int32					BuildID;										//�ĕҐ���N���[�Y�������s�����ꍇ�̎��ʃJ�E���^
		List<SubIndex^>^		SubIdx;											//�C���f�b�N�X�t�@�C���n���h��

		array<Byte>^			FileBuff;										//�t�@�C�����o�͗p�o�b�t�@

		//-- �L�[�ʒu���
		Int32					KeyStartPos;									//�S�C���f�b�N�X���̃L�[�̐擪�ʒu
		Int32					KeyLastPos;										//�S�C���f�b�N�X���̃L�[�̍Ō���ʒu

		//-- �t�@�C���f�[�^ --//
		//--- ISAM���䕔(�t�@�C���f�[�^�j ---//
		String^			FID;													//�t�@�C��ID
		String^			ISAMVer;												//�t�@�C���쐬ISAM���C�u�����o�[�W����
		
		UInt32			AllRecCnt;												//�S���R�[�h��
		UInt32			KeyCnt;													//�L�[��

		UInt32			RecCnt;													//�L�����R�[�h��
		Int64			RecFirstPos;											//���X�g�擪���R�[�h�ʒu
		Int64			RecLastPos;												//���X�g�ŏI���R�[�h�ʒu
		Int64			RecRootPos;												//�񕪖؃��[�g���R�[�h�ʒu

		UInt32			DelRecCnt;												//�폜���R�[�h��
		Int64			DelRecFirstPos;											//�폜���X�g�擪���R�[�h�ʒu
		Int64			DelRecLastPos;											//�폜���X�g�ŏI���R�[�h�ʒu

		Int32			RecMaxLen;												//�ő僌�R�[�h��
		Int32			RecMinLen;												//�ŏ����R�[�h��

		Int32			IdxCnt;													//�C���f�b�N�X��(��L�[�܂�)
		KeyInfo^		ISKeyInfo;												//��L�[���

		String^			UserInfo;												//���[�U�[���

		//*****�@Private�֐� *****//
		
		//-- �w�b�_���� --//
		void			ReadHeader( void );										//���䕔���t�@�C������Ǎ�
		void			WriteHeader( void );									//���䕔���t�@�C���ɏ��o

		//-- ISAM���m�F --//
		Int32			ChkKeyPositions( void );								//�L�[�̊J�n�A�I���ʒu���`�F�b�N
		Int32			CreateBuildID( void );									//BuildID����

		//--�A�N�Z�X(�Ǎ�)--//
		DataRecord^ getRecord( Int64 inRecPos );								//���R�[�h�Ǎ�
		DataRecord^ getRecord( Int32 inIdxNum, SubIndexRecord^ inIdxRec );		//���R�[�h�Ǎ�(�T�u�C���f�b�N�X���)
		DataRecord^ getRecHeader( Int64 inRecPos );								//���R�[�h�w�b�_�Ǐo


		//--�A�N�Z�X(���o)--//
		void WriteRecord( DataRecord^ inRec );										//�f�[�^���R�[�h����
		void WriteRecHeader( DataRecord^ inRec );									//�f�[�^���R�[�h�w�b�_����

		void FindInsertRecordPos( DataRecord^ inRec, DataRecordPos_T^ inAddPos );	//���R�[�h�}���ʒu����
		void ChkDeleteRecordPos( DataRecord^ inRec, DataRecordPos_T^ inDelPos );	//���R�[�h�폜�ʒu�`�F�b�N
		void WriteMoveRecordPos( DataRecord^ inRec, Int64 inNewRecPos );			//���R�[�h�X�V���ꏊ�ړ��`�F�b�N	

		//--�@�A�N�Z�X(�Ǘ�) --//
		/*
		int	reorganizationList( HANDLE tmpHandle,								//ISAM�ĕҐ����X�g�쐬���R�s�[
							int inRootChkCnt, rootChkPos_T *ioRootChkPos,
							IsamInfo_T *ioIsamInfo );
		int	reorganizationTree( HANDLE tmpHandle,								//ISAM�ĕҐ��񕪖؍쐬
							int inRootChkCnt, rootChkPos_T *ioRootChkPos,
							IsamInfo_T *ioIsamInfo );
		int chkTreePos( HANDLE tmpHandle,										//ISAM�ĕҐ��񕪖؃|�C���g�`�F�b�N
							int inRootChkCnt, rootChkPos_T *ioRootChkPos,
							IsamInfo_T *ioIsamInfo );
		int chkEachKey( HANDLE tmpHandle, rootChkPos_T *ioRootChkPos );			//����L�[�͈̔̓`�F�b�N
		*/

	};
}
