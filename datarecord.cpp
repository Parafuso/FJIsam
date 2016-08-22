//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@DataRecord �N���X�@�\�[�X
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.1.1	create date 2004/06/03
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	classes
//		DataRecordA	: �f�[�^���R�[�h
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "DataRecord.h"
#include "FJIsam.h"				//ISAM�w�b�_
#include "FJIsamFunc.h"			//ISAM�p�֐�
#include "FJIsamException.h"	//ISAM�p��O�N���X

using namespace FJIsam;

//------------------------------------------------------------------------------
//	class : DataRecord	:�f�[�^���R�[�h��
//------------------------------------------------------------------------------
//***** �R���X�g���N�^�P
FJIsam::DataRecord::DataRecord()
	:	ParentFile(nullptr),					//ISAM�t�@�C��
		BuildID(0),								//ISAM�r���h�J�E���^
		RecStat(DataRecStat_EN::REC_NEW),		//���R�[�h���
		IsHeaderOnly(false),					//���R�[�h�Ǎ����
		IdxNum(1),								//�����T�u�C���f�b�N�X�ԍ�
		IdxRec(nullptr),						//�����T�u�C���f�b�N�X���R�[�h
		RecPos(0L),								//���R�[�h�ʒu
		DataLen(0),								//�f�[�^��
		DelFLG(IS_RECNOR),						//�폜�t���O
		EachKeyFLG(IS_NOEACHKEY),				//����L�[�t���O
		PrevRecPos(0L),							//�O���R�[�h�ʒu
		NextRecPos(0L),							//�����R�[�h�ʒu
		ParentRecPos(0L),						//�e���R�[�h�ʒu
		LtChildRecPos(0L),						//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos(0L),						//�q���R�[�h�ʒu(�L�[��)
		Data(nullptr)							//�f�[�^
{
}

//***** �R���X�g���N�^�Q
FJIsam::DataRecord::DataRecord( 
					array<Byte>^ inBuff,
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec )
{
	//-- �f�[�^���R�[�h������ --//
	Set( inBuff, inParent, inRecPos, inHeaderOnly, inIdxNum, inIdxRec );
}

//***** �R���X�g���N�^�R
FJIsam::DataRecord::DataRecord(DataRecord^ inRec )
	:	ParentFile(nullptr),					//ISAM�t�@�C��
		BuildID(0),								//ISAM�r���h�J�E���^
		RecStat(DataRecStat_EN::REC_NEW),		//���R�[�h���
		IdxNum(1),								//�����T�u�C���f�b�N�X�ԍ�
		IdxRec(nullptr),						//�����T�u�C���f�b�N�X���R�[�h
		RecPos(0L),								//���R�[�h�ʒu
		DelFLG(IS_RECNOR),						//�폜�t���O
		EachKeyFLG(IS_NOEACHKEY),				//����L�[�t���O
		PrevRecPos(0L),							//�O���R�[�h�ʒu
		NextRecPos(0L),							//�����R�[�h�ʒu
		ParentRecPos(0L),						//�e���R�[�h�ʒu
		LtChildRecPos(0L),						//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos(0L)						//�q���R�[�h�ʒu(�L�[��)
{
	//--�e�����R�s�[����
	IsHeaderOnly = inRec->IsHeaderOnly;		//���R�[�h�Ǎ����

	//���R�[�h������
	if( IsHeaderOnly == true )
	{
		Data = nullptr;
	}else{
		//�f�[�^
		DataLen = inRec->DataLen;				
		Data = gcnew array<Byte>(inRec->DataLen);
		Buffer::BlockCopy( inRec->Data, 0, Data, 0, inRec->DataLen );
	}
		
}

//------------------------------------------------------------------------------
//***** �f�X�g���N�^
FJIsam::DataRecord::~DataRecord()
{
	if( Data != nullptr ) delete Data;
	if( IdxRec != nullptr ) delete IdxRec;
}

//------------------------------------------------------------------------------
// ���R�[�h���
//------------------------------------------------------------------------------
//***** ���R�[�h�ʒu�擾
Int64 FJIsam::DataRecord::getRecPos( void )
{
	return RecPos;
}

//***** Isam�t�@�C���擾
Object^ FJIsam::DataRecord::getParentFile( void )
{
	return ParentFile;
}

//***** BuildID�擾
Int32 FJIsam::DataRecord::getBuildID( void )
{
	return BuildID;
}

//***** ���R�[�h���擾(�S��)
Int32 FJIsam::DataRecord::getRecLength( void )
{
	return getHeaderLength() + getDataLength();
}


//***** �f�[�^���擾
Int32 FJIsam::DataRecord::getDataLength( void )
{
	if( IsHeaderOnly ) return 0;

	return DataLen;
}						
//***** ���R�[�h�ʒu�ݒ�
void FJIsam::DataRecord::setRecPos( Int64 inRecPos )
{
	//Isam�t�@�C���Ȃ�
	if( ParentFile == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isam�t�@�C�����ݒ肳��Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 2009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}

	//�t�@�C���ʒu���s��
	if( inRecPos < (Int64)FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B" );
		tmpFJIsamException->ErrorCode = 2009;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}

	RecPos = inRecPos;

	return;
}

//***** Isam�t�@�C���ݒ�
void FJIsam::DataRecord::setParentFile( Object^ inParent )
{
	FIsam^ tmpParent;

	//�����`�F�b�N
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isam�t�@�C�����ݒ肳��Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 2010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;		
	}
	try
	{
		tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isam�t�@�C�����s���ł��B" );
		tmpFJIsamException->ErrorCode = 2010;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;		
	}
	if( tmpParent->isOpen() == false )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"Isam�t�@�C�����I�[�v������Ă��܂���B" );
		tmpFJIsamException->ErrorCode = 2010;
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
Boolean FJIsam::DataRecord::isHeaderOnly( void )
{
	return IsHeaderOnly;
}

//***** ���R�[�h�w�b�_���擾
Int32 FJIsam::DataRecord::getHeaderLength( void )
{
	return	sizeof( Int32 ) +		//�f�[�^��
			sizeof( Byte ) +		//�폜�t���O
			sizeof( Byte ) +		//����L�[�t���O
			sizeof( Int64 ) +		//�O���R�[�h�ʒu
			sizeof( Int64 ) +		//�����R�[�h�ʒu
			sizeof( Int64 ) +		//�񕪖ؐe���R�[�h�ʒu
			sizeof( Int64 ) +		//�񕪖؃L�[�����R�[�h�ʒu
			sizeof( Int64 );		//�񕪖؃L�[�僌�R�[�h�ʒu
}

//------------------------------------------------------------------------------
// �C���f�b�N�X���
//------------------------------------------------------------------------------
//***** �C���f�b�N�X�ԍ��擾
Int32 FJIsam::DataRecord::getIdxNum( void )
{
	return IdxNum;
}

//***** �T�u�C���f�b�N�X���R�[�h�擾
SubIndexRecord^ FJIsam::DataRecord::getSubIdxRec( void )
{
	return IdxRec;
}

//------------------------------------------------------------------------------
// ���R�[�h���
//------------------------------------------------------------------------------
//***** ����L�[�t���O�擾
Byte FJIsam::DataRecord::getEachKeyFlg( void )
{
	//���C���C���f�b�N�X������
	if( IdxNum == 1 )
	{
		return EachKeyFLG;
	}
	
	//�T�u�C���f�b�N�X������
	if( IdxRec != nullptr)
	{
		return IdxRec->getEachKeyFlg();
	}

	//�T�u�C���f�b�N�X�������ɃC���f�b�N�X�t�@�C�����Ȃ�
	FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X���s���ł��B");
	tmpFJIsamException->ErrorCode = 2002;
	tmpFJIsamException->ErrorCode2 = 1;
	throw tmpFJIsamException;
	
}

//***** �폜�t���O�擾
Byte FJIsam::DataRecord::getDelFlg( void )
{
	return DelFLG;
}

//***** ���R�[�h�̏�Ԏ擾
DataRecStat_EN FJIsam::DataRecord::getRecStat( void )
{
	return RecStat;
}


//***** ���R�[�h�폜
Int32 FJIsam::DataRecord::DeleteRec( void )
{
	DelFLG = IS_RECDEL;
	RecStat = DataRecStat_EN::REC_DEL;

	return 0;
}

//*****����L�[�t���O�ݒ�
Int32 FJIsam::DataRecord::setEachKeyFlg( Byte inEachKeyFlg )
{
	if( inEachKeyFlg <= IS_EACHKEYLST )
	{
		EachKeyFLG = inEachKeyFlg;
		return 0;
	}

	return -1;
}

//���R�[�h�̏�Ԑݒ�
Int32 FJIsam::DataRecord::setRecStat( DataRecStat_EN inStat )
{
	//�V�K���R�[�h�ɕϊ�
	if( inStat == DataRecStat_EN::REC_NEW )
	{
		ParentFile = nullptr;							//Isam�t�@�C��
		BuildID = 0;									//ISAM�r���h�J�E���^
		RecStat = DataRecStat_EN::REC_NEW;				//���R�[�h���
		IsHeaderOnly = true;							//���R�[�h�Ǎ����
		RecPos = 0L;									//���R�[�h�ʒu

		IdxNum = 1;										//�����T�u�C���f�b�N�X�ԍ�
		if( IdxRec != nullptr ) delete IdxRec;			//�����T�u�C���f�b�N�X���R�[�h
		IdxRec = nullptr;			

		DataLen = 0;									//�f�[�^��
		DelFLG = IS_RECNOR;								//�폜�t���O
		EachKeyFLG = IS_NOEACHKEY;						//����L�[�t���O
		PrevRecPos = 0L;								//�O���R�[�h�ʒu
		NextRecPos = 0L;								//�����R�[�h�ʒu
		ParentRecPos = 0L;								//�e���R�[�h�ʒu
		LtChildRecPos = 0L;								//�q���R�[�h�ʒu(�L�[��)
		GtChildRecPos = 0L;								//�q���R�[�h�ʒu(�L�[��)

		if( Data != nullptr ) delete Data;				//�f�[�^
		Data = nullptr;				
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
Int64 FJIsam::DataRecord::getPrevRecPos( void )
{
	return PrevRecPos;
}

//***** �����R�[�h�ʒu�擾
Int64 FJIsam::DataRecord::getNextRecPos( void )
{
	return NextRecPos;
}

//***** �񕪖ؐe���R�[�h�ʒu�擾
Int64 FJIsam::DataRecord::getParentRecPos( void )
{
	return ParentRecPos;
}

//***** �񕪖؃L�[�����R�[�h�ʒu�擾
Int64 FJIsam::DataRecord::getLtChildRecPos( void )
{
	return LtChildRecPos;
}

//***** �񕪖؃L�[�僌�R�[�h�ʒu�擾
Int64 FJIsam::DataRecord::getGtChildRecPos( void )
{
	return GtChildRecPos;
}

//***** �O���R�[�h�ʒu�ݒ�
Int32 FJIsam::DataRecord::setPrevRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
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
Int32 FJIsam::DataRecord::setNextRecPos( Int64 inRecPos)
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
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
Int32 FJIsam::DataRecord::setParentRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
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
Int32 FJIsam::DataRecord::setLtChildRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
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
Int32 FJIsam::DataRecord::setGtChildRecPos( Int64 inRecPos )
{
	//Isam�t�@�C�����Ȃ�
	if( ParentFile == nullptr )
	{
		return -1;
	}

	//�t�@�C���ʒu���s��
	if( inRecPos != 0L &&
		inRecPos < FIsam::Length() )
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

//------------------------------------------------------------------------------
// �f�[�^�A�N�Z�X
//------------------------------------------------------------------------------
//***** �f�[�^�擾
array<Byte>^ FJIsam::DataRecord::getData( array<Byte>^ otBuff )
{
	array<Byte>^ tmpBuff;

	//�f�[�^��0�Ȃ牽�����Ȃ�
	if( DataLen <= 0 )
	{
		return otBuff;
	}

	//�o�b�t�@����
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>(Data->Length);
	}else if( otBuff->Length < DataLen )
	{
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�����������܂��B");
		tmpFJIsamException->ErrorCode = 2003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//�f�[�^�R�s�[
	try
	{
		Buffer::BlockCopy( Data, 0, tmpBuff, 0, DataLen );
	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�փR�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 2003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//�I��
	return tmpBuff;

}

//***** �f�[�^�ݒ�
void FJIsam::DataRecord::setData( array<Byte>^ inBuff, Int32 inLength )
{
	FIsam^ tmpParent;

	//-- �����`�F�b�N --//
	if( inBuff == nullptr )				//�o�b�t�@����

	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�����ݒ�ł��B" );
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inLength <= 0 )					//�f�[�^�����s��
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�����s���ł��B" );
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inBuff->Length < inLength )		//�f�[�^���f�[�^����菬����
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�����Z�����܂��B" );
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	//�L�[�ɖ����Ȃ�
	if( RecStat != DataRecStat_EN::REC_NEW )
	{
		try
		{
			tmpParent = safe_cast<FIsam^>(ParentFile);
			if( inLength < tmpParent->getKeyLastPos() + 1 )
			{
				FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�����Z�����܂��B" );
				tmpFJIsamException->ErrorCode = 2004;
				tmpFJIsamException->ErrorCode2 = 4;
				throw tmpFJIsamException;
			}
		}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�փR�s�[���ɃG���[���������܂����B", exp);
			tmpFJIsamException->ErrorCode = 2004;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
	}


	//-- �f�[�^�R�s�[ --//
	try
	{
		//�f�[�^����(�V�K�쐬)
		if( DataLen == 0)
		{
			Data = gcnew array<Byte>(inLength);
			Buffer::BlockCopy( inBuff, 0, Data, 0, inLength );
			DataLen = inLength;

		//�f�[�^�����傫���Ȃ�(�f�[�^�����ς�)
		}else if( Data->Length < inLength ){
			Array::Resize( Data, inLength);
			Buffer::BlockCopy( inBuff, 0, Data, 0, inLength );
			DataLen = inLength;

		//�f�[�^���������A�܂��͏������Ȃ�ꍇ�͂��̂܂܃R�s�[
		}else{
			Buffer::BlockCopy( inBuff, 0, Data, 0, inLength );
			DataLen = inLength;
		}
		IsHeaderOnly = false;

	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^�փR�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 2004;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return;

}

//***** �L�[�f�[�^�擾
array<Byte>^ FJIsam::DataRecord::getKeyData( array<Byte>^ otBuff, KeyInfo^ inKeyInfo,	
								Boolean KeyOnly )
{
	Int32			tmpDataLen;		//�ԋp����f�[�^��
	array<Byte>^	tmpBuff;

	//-- �����`�F�b�N --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[��񂪖��ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �f�[�^���`�F�b�N --//
	if( DataLen <= 0 || IsHeaderOnly == true )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�̃f�[�^����ł��B");
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;

	}
	//�R�s�[�f�[�^���`�F�b�N
	if( KeyOnly == true )
	{
		tmpDataLen = inKeyInfo->getKeyLen();
	}else{
		tmpDataLen = inKeyInfo->getKeyLen() + inKeyInfo->getSrtKeyLen();
	}

	//�o�b�t�@�쐬
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>(tmpDataLen);

	}else if( otBuff->Length < tmpDataLen )
	{
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�����������܂��B");
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//-- �L�[�f�[�^�R�s�[ --//
	try
	{
		Buffer::BlockCopy( Data, inKeyInfo->getKeyPos() , tmpBuff, 0, inKeyInfo->getKeyLen() );
	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[���R�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- �L�[�݂̂̃R�s�[�Ȃ�I���@--//
	if( KeyOnly == true ||
		inKeyInfo->getSrtKeyLen() == 0 )
	{
		return tmpBuff;
	}

	//-- �\�[�g�L�[�f�[�^�R�s�[ --//
	try
	{
		Buffer::BlockCopy( Data, inKeyInfo->getSrtKeyPos() , tmpBuff, 
							inKeyInfo->getKeyLen(), inKeyInfo->getSrtKeyLen() );
	}catch( Exception^ exp ){
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�\�[�g�L�[���R�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 2005;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return tmpBuff;

}
//***** �\�[�g�L�[�f�[�^�擾
array<Byte>^ FJIsam::DataRecord::getSrtKeyData( array<Byte>^ otBuff,					
								KeyInfo^ inKeyInfo )
{
	Int32			tmpDataLen;		//�ԋp����f�[�^��
	array<Byte>^	tmpBuff;

	//-- �����`�F�b�N --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[��񂪖��ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKeyInfo->getSrtKeyLen() == 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�\�[�g�L�[����0�ł��B");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}


	//-- �f�[�^���`�F�b�N --//
	if( DataLen <= 0 || IsHeaderOnly == true )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�̃f�[�^����ł��B");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;

	}

	//-- �L�[�f�[�^�R�s�[ --//
	tmpDataLen = inKeyInfo->getSrtKeyLen();


	//�o�b�t�@�쐬
	if( otBuff == nullptr )
	{
		tmpBuff = gcnew array<Byte>(tmpDataLen);

	}else if( otBuff->Length < tmpDataLen )
	{
		//�o�b�t�@�����Z��
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�b�t�@�����������܂��B");
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}else{
		tmpBuff = otBuff;
	}

	//-- �\�[�g�L�[�f�[�^�R�s�[ --//
	try
	{
		Buffer::BlockCopy( Data, inKeyInfo->getSrtKeyPos() , tmpBuff, 
							0, inKeyInfo->getSrtKeyLen() );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�\�[�g�L�[���R�s�[���ɃG���[���������܂����B", exp);
		tmpFJIsamException->ErrorCode = 2006;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return tmpBuff;
}

//------------------------------------------------------------------------------
// ���R�[�h�f�[�^�A�N�Z�X
//------------------------------------------------------------------------------
//***** �f�[�^����
array<Byte>^ FJIsam::DataRecord::WriteRecData( array<Byte>^ otBuff,
									   Boolean inHeaderOnly )
{
	Int32 tmpPos;	//�I�t�Z�b�g

	if( otBuff == nullptr )
	{
		if( inHeaderOnly == true )
		{
			otBuff = gcnew array<Byte>(DataRecord::getHeaderLength() );
		}else{
			otBuff = gcnew array<Byte>(getRecLength() );
		}
	}
	//-- �����`�F�b�N --//
	if( ( inHeaderOnly == true && otBuff->Length < DataRecord::getHeaderLength() ) ||
		( inHeaderOnly == false && otBuff->Length < getRecLength() ) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�o�̓o�b�t�@���Z�����܂��B" );
		tmpFJIsamException->ErrorCode = 2007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	tmpPos = 0;
	//--�@�f�[�^���� --//
	try
	{
		//�f�[�^��
		FJIFuncs::Int32ToArray( DataLen, otBuff, tmpPos );
		tmpPos += sizeof( DataLen );
		
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
	
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�w�b�_�����o�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 2007;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//�w�b�_�݂̂̃��R�[�h�A�܂��̓w�b�_�݂̂̎w��w���Ȃ�Ȃ�R�R�ŏI��
	if( IsHeaderOnly == true ||
		inHeaderOnly == true)
	{
		return otBuff;
	}

	//�f�[�^��
	try
	{
		Buffer::BlockCopy( Data, 0, otBuff, tmpPos, DataLen );

	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�f�[�^�����o�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 2007;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return otBuff;
}

//------------------------------------------------------------------------------
// Private�֐�
//------------------------------------------------------------------------------
//******* �f�[�^���R�[�h������
void FJIsam::DataRecord::Set( 
					array<Byte>^ inBuff,
					Object^		inParent,
					Int64		inRecPos,
					Boolean		inHeaderOnly,
					Int32		inIdxNum,
					SubIndexRecord^ inIdxRec )
{
	Int32 tmpPos;
	FIsam^ tmpParent;

	//-- �����`�F�b�N --//
	if( inBuff == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�f�[�^���[�_�[�����ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	try
	{
		tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^ exp){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C��������������܂���B", exp);
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inIdxNum < 1 ||
		( inIdxNum == 1 && inIdxRec != nullptr ) ||
		( inIdxNum < IdxNum && inIdxRec == nullptr ) )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�w�肪�s���ł��B");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	if( inRecPos < FIsam::Length() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�ʒu���s���ł��B");
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- �Ǘ����ݒ� --//
	ParentFile = inParent;
	BuildID = tmpParent->getBuildID();
	RecStat = DataRecStat_EN::REC_OLD;
	IdxNum = inIdxNum;
	IdxRec = inIdxRec;
	RecPos = inRecPos;

	tmpPos = 0;
	//-- ���䕔�ݒ� --//
	try
	{
		//�f�[�^��
		DataLen = FJIFuncs::ArrayToInt32( inBuff, tmpPos );
		tmpPos += sizeof( DataLen );

		//�폜�t���O
		DelFLG = inBuff[tmpPos];
		tmpPos += sizeof(DelFLG);

		if( DelFLG != IS_RECNOR && DelFLG != IS_RECDEL )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�폜�t���O���s���ł��B");
			tmpFJIsamException->ErrorCode = 2008;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
	
		//����L�[�t���O
		EachKeyFLG = inBuff[tmpPos];
		tmpPos += sizeof(EachKeyFLG);

		if( EachKeyFLG > IS_EACHKEYLST )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"����L�[�t���O���s���ł��B");
			tmpFJIsamException->ErrorCode = 2008;
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

		//-- �w�b�_�̂ݓǍ��� --// 
		if( inHeaderOnly == true )
		{
			IsHeaderOnly = true;
			Data = nullptr;

			return;

		//--�@�f�[�^���Ǎ� --//
		}

		//�X�g���[���̃f�[�^���̎c�肪�f�[�^�����Z����Γǂݍ��܂Ȃ�
		if( inBuff->Length - tmpPos < DataLen )
		{
			IsHeaderOnly = true;
			Data = nullptr;

			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�f�[�^���̃f�[�^���������Ă��܂��B");
			tmpFJIsamException->ErrorCode = 2008;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;

		//�f�[�^�Ǎ�
		}else{
			IsHeaderOnly = false;
			Data = gcnew array<Byte>(DataLen);
			Buffer::BlockCopy( inBuff, tmpPos, Data, 0, DataLen );
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���R�[�h�f�[�^���̓Ǎ��Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 2008;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;
	}

}
