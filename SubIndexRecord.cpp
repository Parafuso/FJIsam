//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�T�u�C���f�b�N�X���R�[�h�@�N���X�@�\�[�X
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
///	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndexRecord	:�T�u�C���f�b�N�X���R�[�h
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndexRecord.h"		//�T�u�C���f�b�N�X���R�[�h
#include "SubIndex.h"			//�T�u�C���f�b�N�X
#include "FJIsam.h"				//Isam�N���X
#include "FJIsamFunc.h"			//Isam�p�֐�
#include "FJIsamException.h"	//FJIsam��O�N���X

using namespace FJIsam;

//------------------------------------------------------------------------------
//	class	:SubIndexRecord	:�T�u�C���f�b�N�X���R�[�h
//------------------------------------------------------------------------------
//***** �R���X�g���N�^(�V�K)
FJIsam::SubIndexRecord::SubIndexRecord( Object^ inParent )
	:	RecStat(DataRecStat_EN::REC_NEW),	//���R�[�h���
		IsHeaderOnly(false),				//���R�[�h�Ǎ����
		RecPos(0L),							//���R�[�h�ʒu
		DelFLG(IS_RECNOR),					//�폜�t���O
		EachKeyFLG(IS_NOEACHKEY),			//����L�[�t���O
		PrevRecPos(0L),						//�O���R�[�h�ʒu
		NextRecPos(0L),						//�����R�[�h�ʒu
		ParentRecPos(0L),					//�e���R�[�h�ʒu
		LtChildRecPos(0L),					//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos(0L),					//�q���R�[�h�ʒu(�L�[��)
		DataRecPos( 0L ),					//�f�[�^���R�[�h�{�̈ʒu
		KeyData(nullptr),					//�L�[�f�[�^
		SrtKeyData(nullptr)					//�\�[�g�L�[�f�[�^
{
	//-- �����`�F�b�N --//
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 4001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X�t�@�C����ݒ� --//
	try
	{
		 SubIndex^ tmpParent = safe_cast<SubIndex^>(inParent);
		 ParentFile = inParent;
		 BuildID = tmpParent->getBuildID();

	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�����s���ł��B");
		tmpFJIsamException->ErrorCode = 4001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
}

//***** �R���X�g���N�^(����)
FJIsam::SubIndexRecord::SubIndexRecord( 
					array<Byte>^ inBuff,		
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly )
{
	//--���R�[�h���Z�b�g --//
	Set( inBuff, inParent, inRecPos, inHeaderOnly );
}

//***** �R�s�[�R���X�g���N�^
FJIsam::SubIndexRecord::SubIndexRecord(SubIndexRecord^ inRec )
	:	RecStat(DataRecStat_EN::REC_NEW),		//���R�[�h���
		RecPos(0L),								//���R�[�h�ʒu
		DelFLG(IS_RECNOR),						//�폜�t���O
		EachKeyFLG(IS_NOEACHKEY),				//����L�[�t���O
		PrevRecPos(0L),							//�O���R�[�h�ʒu
		NextRecPos(0L),							//�����R�[�h�ʒu
		ParentRecPos(0L),						//�e���R�[�h�ʒu
		LtChildRecPos(0L),						//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos(0L),						//�q���R�[�h�ʒu(�L�[��)
		DataRecPos(0L)							//�f�[�^���R�[�h�{�̈ʒu
{
	//-- �T�u�C���f�b�N�X�t�@�C����ݒ� --//
	ParentFile = inRec->ParentFile;
	BuildID = inRec->BuildID;

	//--�e�����R�s�[����
	IsHeaderOnly = inRec->IsHeaderOnly;		//���R�[�h�Ǎ����

	//���R�[�h������
	if( IsHeaderOnly == true )
	{
		KeyData = nullptr;
		SrtKeyData = nullptr;
	}else{
		//�f�[�^
		KeyData = gcnew array<Byte>(inRec->KeyData->Length);
		Buffer::BlockCopy( inRec->KeyData, 0, KeyData, 0, KeyData->Length );
		SrtKeyData = gcnew array<Byte>(inRec->SrtKeyData->Length);
		Buffer::BlockCopy( inRec->SrtKeyData, 0, SrtKeyData, 0, SrtKeyData->Length );
	}
}

//------------------------------------------------------------------------------
//***** �f�X�g���N�^
FJIsam::SubIndexRecord::~SubIndexRecord()
{
	if( KeyData != nullptr ) delete KeyData;
	if( SrtKeyData != nullptr ) delete SrtKeyData;
}

//------------------------------------------------------------------------------
//�T�u�C���f�b�N�X���R�[�h��� *****
//------------------------------------------------------------------------------
//***** ���R�[�h�ʒu�擾
Int64 FJIsam::SubIndexRecord::getRecPos( void )
{
	return RecPos;
}

//***** Isam�t�@�C���擾
Object^ FJIsam::SubIndexRecord::getParentFile( void )
{
	return ParentFile;
}

//***** BuildID�擾
Int32 FJIsam::SubIndexRecord::getBuildID( void )
{
	return BuildID;
}

//***** ���R�[�h���擾(�S��)
Int32 FJIsam::SubIndexRecord::getRecLength( void )
{
	return getHeaderLength() + getDataLength();
}

//***** �f�[�^���擾
Int32 FJIsam::SubIndexRecord::getDataLength( void )
{
	if( IsHeaderOnly ) return 0;

	return KeyData->Length + SrtKeyData->Length;
}

//***** ���R�[�h�ʒu�ݒ�
void FJIsam::SubIndexRecord::setRecPos( Int64 inRecPos )
{
	//�T�u�C���f�b�N�X�t�@�C���Ȃ�
	if( ParentFile == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 4009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < (Int64)SubIndex::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�}���ʒu�����ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 4009;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	RecPos = inRecPos;

	return;
}

//***** Isam�t�@�C���ݒ�
void FJIsam::SubIndexRecord::setParentFile( Object^ inParent )
{
	SubIndex^ tmpParent;

	//�����`�F�b�N
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 4010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	try
	{
		tmpParent = safe_cast<SubIndex^>(inParent);
	}catch(InvalidCastException^ exp) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�����s���ł��B", exp);
		tmpFJIsamException->ErrorCode = 4010;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( tmpParent->isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�����I�[�v������Ă��܂���B");
		tmpFJIsamException->ErrorCode = 4009;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//�f�[�^�Z�b�g
	ParentFile = inParent;
	BuildID = tmpParent->getBuildID();
	
	//����I��
	return;
}

//***** ���R�[�h�Ǎ��`�F�b�N
Boolean FJIsam::SubIndexRecord::isHeaderOnly( void )
{
	return IsHeaderOnly;
}

//***** ���R�[�h�w�b�_���擾
Int32 FJIsam::SubIndexRecord::getHeaderLength( void )
{
	return	sizeof( Byte ) +		//�폜�t���O
			sizeof( Byte ) +		//����L�[�t���O
			sizeof( Int64 ) +		//�O���R�[�h�ʒu
			sizeof( Int64 ) +		//�����R�[�h�ʒu
			sizeof( Int64 ) +		//�񕪖ؐe���R�[�h�ʒu
			sizeof( Int64 ) +		//�񕪖؃L�[�����R�[�h�ʒu
			sizeof( Int64 ) +		//�񕪖؃L�[�僌�R�[�h�ʒu
			sizeof( Int64 );		//�f�[�^���R�[�h�{�̈ʒu

}
//------------------------------------------------------------------------------
// ���R�[�h���
//------------------------------------------------------------------------------
//***** ����L�[�t���O�擾
Byte FJIsam::SubIndexRecord::getEachKeyFlg( void )
{
	return EachKeyFLG;	
}

//***** �폜�t���O�擾
Byte FJIsam::SubIndexRecord::getDelFlg( void )
{
	return DelFLG;
}

//***** ���R�[�h�̏�Ԏ擾
DataRecStat_EN FJIsam::SubIndexRecord::getRecStat( void )
{
	return RecStat;
}


//***** ���R�[�h�폜
Int32 FJIsam::SubIndexRecord::DeleteRec( void )
{
	DelFLG = IS_RECDEL;
	RecStat = DataRecStat_EN::REC_DEL;

	return 0;
}

//***** ����L�[�t���O�ݒ�
Int32 FJIsam::SubIndexRecord::setEachKeyFlg( Byte inEachKeyFlg )
{
	if( inEachKeyFlg <= IS_EACHKEYLST )
	{
		EachKeyFLG = inEachKeyFlg;
		return 0;
	}

	return -1;
}

//���R�[�h�̏�Ԑݒ�
Int32 FJIsam::SubIndexRecord::setRecStat( DataRecStat_EN inStat )
{
	//�V�K���R�[�h�ɕϊ�
	if( inStat == DataRecStat_EN::REC_NEW )
	{
		ParentFile = nullptr;								//Isam�t�@�C��
		BuildID = 0;										//ISAM�r���h�J�E���^
		RecStat = DataRecStat_EN::REC_NEW;					//���R�[�h���
		IsHeaderOnly = true;								//���R�[�h�Ǎ����
		RecPos = 0L;										//���R�[�h�ʒu

		DelFLG = IS_RECNOR;									//�폜�t���O
		EachKeyFLG = IS_NOEACHKEY;							//����L�[�t���O
		PrevRecPos = 0L;									//�O���R�[�h�ʒu
		NextRecPos = 0L;									//�����R�[�h�ʒu
		ParentRecPos = 0L;									//�e���R�[�h�ʒu
		LtChildRecPos = 0L;									//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos = 0L;									//�q���R�[�h�ʒu(�L�[��)

		if( KeyData != nullptr ) delete KeyData;			//�L�[�f�[�^
		KeyData = nullptr;				
		if( SrtKeyData != nullptr ) delete SrtKeyData;		//�\�[�g�L�[�f�[�^
		SrtKeyData = nullptr;				
	}

	//���R�[�h�X�V
	if( inStat == DataRecStat_EN::REC_OLD )
	{
		RecStat = inStat;
	}

	//���R�[�h�폜
	if( inStat == DataRecStat_EN::REC_DEL )
	{
		DeleteRec();
	}
	
	return 0;
}

//------------------------------------------------------------------------------
// ���R�[�h�ʒu���
//------------------------------------------------------------------------------
//***** �O���R�[�h�ʒu�擾
Int64 FJIsam::SubIndexRecord::getPrevRecPos( void )
{
	return PrevRecPos;
}

//***** �����R�[�h�ʒu�擾
Int64 FJIsam::SubIndexRecord::getNextRecPos( void )
{
	return NextRecPos;
}

//***** �񕪖ؐe���R�[�h�ʒu�擾
Int64 FJIsam::SubIndexRecord::getParentRecPos( void )
{
	return ParentRecPos;
}

//***** �񕪖؃L�[�����R�[�h�ʒu�擾
Int64 FJIsam::SubIndexRecord::getLtChildRecPos( void )
{
	return LtChildRecPos;
}

//***** �񕪖؃L�[�僌�R�[�h�ʒu�擾
Int64 FJIsam::SubIndexRecord::getGtChildRecPos( void )
{
	return GtChildRecPos;
}

//***** �f�[�^���R�[�h�{�̈ʒu�擾
Int64 FJIsam::SubIndexRecord::getDataRecPos( void )
{
	return DataRecPos;
}

//------------------------------------------------------------------------------
//***** �O���R�[�h�ʒu�ݒ�
Int32 FJIsam::SubIndexRecord::setPrevRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < SubIndex::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	PrevRecPos = inRecPos;

	return 0;
}
//***** �����R�[�h�ʒu�ݒ�
Int32 FJIsam::SubIndexRecord::setNextRecPos( Int64 inRecPos)
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < SubIndex::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	NextRecPos = inRecPos;
	
	return 0;
}

//***** �񕪖ؐe���R�[�h�ʒu�ݒ�
Int32 FJIsam::SubIndexRecord::setParentRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < SubIndex::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	ParentRecPos = inRecPos;

	return 0;
}

//***** �񕪖؃L�[�����R�[�h�ʒu�ݒ�
Int32 FJIsam::SubIndexRecord::setLtChildRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < SubIndex::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	LtChildRecPos = inRecPos;

	return 0;
}

//***** �񕪖؃L�[�僌�R�[�h�ʒu�ݒ�
Int32 FJIsam::SubIndexRecord::setGtChildRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < SubIndex::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	GtChildRecPos = inRecPos;

	return 0;
}

//***** �񕪖؃L�[�僌�R�[�h�ʒu�ݒ�
Int32 FJIsam::SubIndexRecord::setDataRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < FIsam::Length() )
	{
		return -1;
	}
	if( inRecPos == RecPos )
	{
		return -1;
	}

	DataRecPos = inRecPos;

	return 0;
}
//------------------------------------------------------------------------------
// �f�[�^�A�N�Z�X
//------------------------------------------------------------------------------
//***** �L�[�f�[�^�擾
array<Byte>^ FJIsam::SubIndexRecord::getKeyData( array<Byte>^ otBuff )
{
	array<Byte>^	tmpBuff;

	//�f�[�^��0�Ȃ牽�����Ȃ�
	if( KeyData == nullptr )
	{
		return otBuff;
	}

	//�o�b�t�@����
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>( KeyData->Length );
	}else if( otBuff->Length < KeyData->Length )
	{
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�����������܂��B");
		tmpFJIsamException->ErrorCode = 4003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//�f�[�^�R�s�[
	try
	{
		Buffer::BlockCopy( KeyData, 0, tmpBuff, 0, KeyData->Length );
	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�փR�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 4003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//�I��
	return tmpBuff;

}

//***** �\�[�g�L�[�f�[�^�擾
array<Byte>^ FJIsam::SubIndexRecord::getSrtKeyData( array<Byte>^ otBuff )
{
	array<Byte>^	tmpBuff;

	//�f�[�^��0�Ȃ牽�����Ȃ�
	if( SrtKeyData == nullptr )
	{
		return otBuff;
	}

	//�o�b�t�@����
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>( SrtKeyData->Length );
	}else if( otBuff->Length < SrtKeyData->Length )
	{
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�����������܂��B");
		tmpFJIsamException->ErrorCode = 4004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//�f�[�^�R�s�[
	try
	{
		Buffer::BlockCopy( SrtKeyData, 0, tmpBuff, 0, SrtKeyData->Length );
	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�փR�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 4004;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//�I��
	return tmpBuff;

}

//***** �L�[�f�[�^�ݒ�
void FJIsam::SubIndexRecord::setKeyData( array<Byte>^ inKey )
{
	SubIndex^ tmpParent = safe_cast<SubIndex^>(ParentFile);

	//-- �����`�F�b�N --//
	if( inKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����ݒ�ł��B" );
		tmpFJIsamException->ErrorCode =4005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKey->Length < tmpParent->getKeyLen() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����Z�����܂��B" );
		tmpFJIsamException->ErrorCode =4005;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- �R�s�[ --//
	try
	{
		//�f�[�^����Ȃ�쐬
		if( KeyData == nullptr )
		{
			KeyData = gcnew array<Byte>( tmpParent->getKeyLen() );
		}
		Buffer::BlockCopy( inKey, 0, KeyData, 0, KeyData->Length );

		if( KeyData != nullptr && SrtKeyData != nullptr )
		{
			IsHeaderOnly = false;
		}

	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�փR�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode =4005;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return;

}

//***** �\�[�g�L�[�f�[�^�ݒ�
void FJIsam::SubIndexRecord::setSrtKeyData( array<Byte>^ inSrtKey )
{
	SubIndex^ tmpParent = safe_cast<SubIndex^>(ParentFile);

	//-- �����`�F�b�N --//
	//�o�b�t�@����
	if( inSrtKey == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�\�[�g�L�[�����ݒ�ł��B" );
		tmpFJIsamException->ErrorCode =4006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inSrtKey->Length < tmpParent->getSrtKeyLen() )
	{ 
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�\�[�g�L�[�����Z�����܂��B" );
		tmpFJIsamException->ErrorCode =4006;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- �R�s�[ --//
	try
	{
		//�f�[�^����Ȃ�쐬
		if( SrtKeyData == nullptr )
		{
			SrtKeyData = gcnew array<Byte>( tmpParent->getSrtKeyLen() );
		}
		
		Buffer::BlockCopy( inSrtKey, 0, SrtKeyData, 0, SrtKeyData->Length );

		if( KeyData != nullptr && SrtKeyData != nullptr )
		{
			IsHeaderOnly = false;
		}

	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�փR�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode =4006;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return;

}
//------------------------------------------------------------------------------
// �f�[�^�A�N�Z�X
//------------------------------------------------------------------------------
//***** �f�[�^�擾
array<Byte>^ FJIsam::SubIndexRecord::WriteRecData( array<Byte>^ otBuff,
										   Boolean inHeaderOnly )
{
	Int32 tmpPos;
	array<Byte>^ tmpBuff;
	SubIndex^ tmpParent;

	//--�@�o�b�t�@�`�F�b�N --//
	if( otBuff == nullptr )
	{
		if( inHeaderOnly == true )
		{
			tmpBuff = gcnew array<Byte>(DataRecord::getHeaderLength() );
		}else{
			tmpBuff = gcnew array<Byte>(getRecLength() );
		}
	}else{
		tmpBuff = otBuff;
	}

	//-- �����`�F�b�N --//
	if( ( inHeaderOnly == true && otBuff->Length < SubIndexRecord::getHeaderLength() ) ||
		( inHeaderOnly == false && otBuff->Length < getRecLength() ) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�̓o�b�t�@���Z�����܂��B" );
		tmpFJIsamException->ErrorCode = 4007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//--�@�f�[�^���� --//
	tmpPos = 0;

	try
	{
		//�폜�t���O
		otBuff[tmpPos] = DelFLG;
		tmpPos++;

		//����L�[�t���O
		otBuff[tmpPos] = EachKeyFLG;
		tmpPos++;

		//�O���R�[�h�ʒu
		FJIFuncs::Int64ToArray( PrevRecPos, otBuff, tmpPos );
		tmpPos += sizeof( PrevRecPos );

		//�����R�[�h�ʒu
		FJIFuncs::Int64ToArray( NextRecPos, otBuff, tmpPos );
		tmpPos += sizeof( NextRecPos );

		//�񕪖ؐe���R�[�h�ʒu
		FJIFuncs::Int64ToArray( ParentRecPos, otBuff, tmpPos );
		tmpPos += sizeof( ParentRecPos );

		//�񕪖؃L�[�����R�[�h�ʒu
		FJIFuncs::Int64ToArray( LtChildRecPos, otBuff, tmpPos );
		tmpPos += sizeof( LtChildRecPos );

		//�񕪖؃L�[�����R�[�h�ʒu
		FJIFuncs::Int64ToArray( GtChildRecPos, otBuff, tmpPos );
		tmpPos += sizeof( GtChildRecPos );
		
		//�f�[�^���R�[�h�ʒu
		FJIFuncs::Int64ToArray( DataRecPos, otBuff, tmpPos );
		tmpPos += sizeof( DataRecPos );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�w�b�_�����o�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 4007;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//�w�b�_�݂̂̃��R�[�h�A�܂��̓w�b�_�݂̂̏o�͂Ȃ�R�R�ŏI��
	if( IsHeaderOnly == true || 
		inHeaderOnly == true )
	{
		return otBuff;
	}

	//�f�[�^��
	try
	{
		tmpParent = safe_cast<SubIndex^>(ParentFile);
		Buffer::BlockCopy( KeyData, 0, otBuff, tmpPos, tmpParent->getKeyLen() );
		tmpPos += tmpParent->getKeyLen();
		Buffer::BlockCopy( SrtKeyData, 0, otBuff, tmpPos, tmpParent->getSrtKeyLen() );
		tmpPos += tmpParent->getSrtKeyLen();
		
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�L�[�����o�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 4007;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return otBuff;
}

//------------------------------------------------------------------------------
// Private�֐�
//------------------------------------------------------------------------------
//***** ���R�[�h�ݒ�
void FJIsam::SubIndexRecord::Set( 
					array<Byte>^ inBuff,		
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly )
{
	Int32	  tmpPos;
	SubIndex^ tmpParent;

	//-- �����`�F�b�N --//
	if( inBuff == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^���[�_�[�����ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	try
	{
		 tmpParent = safe_cast<SubIndex^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�����s���ł��B");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inRecPos < SubIndex::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}
	if( inBuff->Length < SubIndexRecord::getHeaderLength() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h���䕔�̓Ǎ��f�[�^���������Ă��܂��B");
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- �Ǘ����ݒ�[ --//
	ParentFile = inParent;
	BuildID = tmpParent->getBuildID();
	RecStat = DataRecStat_EN::REC_OLD;
	RecPos = inRecPos;

	tmpPos = 0;
	//-- ���䕔�ݒ� --//
	try
	{
		//�폜�t���O
		DelFLG = inBuff[tmpPos];
		tmpPos += sizeof(DelFLG);

		if( DelFLG != IS_RECNOR && DelFLG != IS_RECDEL )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�폜�t���O���s���ł��B");
			tmpFJIsamException->ErrorCode = 4008;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
	
		//����L�[�t���O
		EachKeyFLG = inBuff[tmpPos];
		tmpPos += sizeof(EachKeyFLG);

		if( EachKeyFLG > IS_EACHKEYLST )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"����L�[�t���O���s���ł��B");
			tmpFJIsamException->ErrorCode = 4008;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}

		//�O���R�[�h�ʒu
		PrevRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( PrevRecPos );

		//�����R�[�h�ʒu
		NextRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( NextRecPos );

		//�e���R�[�h�ʒu
		ParentRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( ParentRecPos );

		//�q���R�[�h�ʒu(�L�[��)
		LtChildRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( LtChildRecPos );
		
		//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( GtChildRecPos );

		//�f�[�^���R�[�h�ʒu
		DataRecPos = FJIFuncs::ArrayToInt64( inBuff, tmpPos);
		tmpPos += sizeof( GtChildRecPos );

		//-- �L�[�� --//
		if( inHeaderOnly == true )
		{
			IsHeaderOnly = true;
			KeyData = nullptr;
			SrtKeyData = nullptr;

			return;
		}

		//�X�g���[���̃f�[�^���̎c�肪�L�[�f�[�^�����Z����Γǂݍ��܂Ȃ�
		if( inBuff->Length - tmpPos < 
			tmpParent->getKeyLen() + tmpParent->getSrtKeyLen() )
		{
			IsHeaderOnly = true;
			KeyData = nullptr;
			SrtKeyData = nullptr;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�L�[���̃f�[�^���������Ă��܂��B");
			tmpFJIsamException->ErrorCode = 4008;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		//�L�[�f�[�^�Ǎ�
		}else{
			IsHeaderOnly = false;
			KeyData = gcnew array<Byte>( tmpParent->getKeyLen() );
			Buffer::BlockCopy( inBuff, tmpPos, KeyData, 0, tmpParent->getKeyLen() );
			tmpPos += tmpParent->getKeyLen();
			SrtKeyData = gcnew array<Byte>( tmpParent->getSrtKeyLen() );
			Buffer::BlockCopy( inBuff, tmpPos, SrtKeyData, 0, tmpParent->getSrtKeyLen() );
			tmpPos += tmpParent->getSrtKeyLen();
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���R�[�h�L�[���̓Ǎ��Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 4008;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}
}
