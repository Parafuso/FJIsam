//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�T�u�C���f�b�N�X�@�\�[�X
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		SubIndex	:�T�u�C���f�b�N�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "SubIndex.h"			//�T�u�C���f�b�N�X�w�b�_
#include "FJIsam.h"				//ISAM�t�@�C��
#include "FJIsamException.h"	//FJIsam��O�N���X
#include "FJIsamEnum.h"

using namespace FJIsam;

//------------------------------------------------------------------------------
//  class	:SubIndex	:�T�u�C���f�b�N�X
//------------------------------------------------------------------------------
//***** �R���X�g���N�^ *****//
FJIsam::SubIndex::SubIndex( Object^ inParent, int inIdxNum, IsamOpenMode inMode )
	: FStream(nullptr)		//�t�@�C���n���h���N���A
{
	FIsam^ tmpParent;
	String^ tmpFilePath;

	//-- �����`�F�b�N --//
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	try{
		 tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�����s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( !tmpParent->isOpen() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����I�[�v������Ă��܂���B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	if(inIdxNum < 1 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" �C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	if( inMode != IsamOpenMode::READ_WRITE &&					//�t�@�C���I�[�v�����[�h�`�F�b�N
		inMode != IsamOpenMode::READ_ONLY )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C���I�[�v�����@���s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X�t�@�C���I�[�v�� --//

	//�t�@�C���p�X�擾
	try
	{
		tmpFilePath = CreateFileName( tmpParent->getFilePath(nullptr), inIdxNum );
	}catch( Exception^ exp){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������s���ł��B",exp );
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}
	//�t�@�C�����݃`�F�b�N
	if( !File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�w�肳�ꂽ�t�@�C�������݂��܂���B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}
	//�I�[�v��
	try{
		//�ǂݏ�����
		if( inMode == IsamOpenMode::READ_WRITE )
		{
			FStream = File::Open( tmpFilePath,		//�t�@�C����
						FileMode::Open,				//�쐬���[�h
						FileAccess::ReadWrite,		//�A�N�Z�X���[�h
						FileShare::None );			//���L:����
		
		//�Ǎ���p
		}else{
			FStream = File::Open( tmpFilePath,		//�t�@�C����
						FileMode::Open,				//�쐬���[�h
						FileAccess::Read,			//�A�N�Z�X���[�h
						FileShare::Read );			//���L:�Ǎ��̂݉�
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�t�@�C���̃I�[�v���Ɏ��s���܂����B",exp );
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//�t�@�C���Ǎ��o�b�t�@��`
	FileBuff = gcnew array<Byte>(4096);

	//-- ���ڃZ�b�g --//
	ParentFile = inParent;							//ISAM�t�@�C��
	FilePath = tmpFilePath;							//�t�@�C���p�X
	IdxNum = inIdxNum;								//�C���f�b�N�X�ԍ�

	//-- �w�b�_���ǂݍ��� --//
	try
	{
		ReadHeader();
	}catch( Exception^ exp ){
		Close();

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�w�b�_��񂪓ǂݍ��߂܂���B", exp);
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}

	//--�w�b�_�`�F�b�N --//
	String^ tmpFID = tmpParent->getFID( nullptr);
	if( FID->Contains( tmpFID ) == 0 ||
		FID->Length != tmpFID->Length )
	{
		Close();

		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"FID����v���܂���B" );
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 10;
		throw tmpFJIsamException;
	}
	delete tmpFID;
}

//***** �R���X�g���N�^(�V�K)
FJIsam::SubIndex::SubIndex(System::Object ^inParent, int inIdxNum,
						   FJIsam::KeyInfo ^inKeyInfo)
{
	FIsam^ tmpParent;
	String^ tmpFilePath;

	//-- �����`�F�b�N --//
	if( inParent == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	try{
		 tmpParent = safe_cast<FIsam^>(inParent);
	}catch(InvalidCastException^) {	
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����s���ł��B");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( !tmpParent->isOpen() )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"ISAM�t�@�C�����I�[�v������Ă��܂���B");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	if( inIdxNum <= 1 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L" �C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//�t�@�C���p�X�擾
	try
	{
		tmpFilePath = CreateFileName( tmpParent->getFilePath(nullptr), inIdxNum );
	}catch( Exception^ exp){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�T�u�C���f�b�N�X�t�@�C�������s���ł��B",exp );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//�t�@�C�����݃`�F�b�N
	if( File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�������O�̃t�@�C���������݂��܂��B");
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;

	}

	//�t�@�C���Ǎ��o�b�t�@��`
	FileBuff = gcnew array<Byte>(4096);

	//-- ISAM�V�K�쐬 --//
	try
	{
		FStream = File::Create( tmpFilePath,
						FileBuff->Length,
						FileOptions::RandomAccess,
						gcnew Security::AccessControl::FileSecurity( tmpParent->getFilePath(nullptr),
								Security::AccessControl::AccessControlSections::Access ) );
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C���̍쐬�Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 7;

		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X���ݒ� --//
	try
	{
		ParentFile = inParent;									//ISAM�t�@�C��
		FID = tmpParent->getFID( nullptr );						//�t�@�C��ID�ݒ�
		ISAMVer = tmpParent->getVersion( nullptr );				//ISAM�o�[�W�����ݒ�
		SubKeyInfo = gcnew KeyInfo( inKeyInfo );				//�L�[���ݒ�

		//���R�[�h��񏉊���
		AllRecCnt = 0;
		KeyCnt = 0;
		RecCnt = 0;
		RecFirstPos = 0L;
		RecLastPos = 0L;
		RecRootPos = 0L;
	
		DelRecCnt = 0;
		DelRecFirstPos = 0L;
		DelRecLastPos = 0L;
		SubKeyInfo = gcnew KeyInfo( inKeyInfo );

		//-- �T�u�C���f�b�N�X���䕔���� --//
		WriteHeader();
	}catch(Exception^ exp)
	{
		Close();

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�T�u�C���f�b�N�X���̍쐬�Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 8;

		throw tmpFJIsamException;
	}

	//�t�@�C�����Z�b�g
	FilePath = tmpFilePath;
	IdxNum = inIdxNum;

}
//------------------------------------------------------------------------------
//***** �f�X�g���N�^
FJIsam::SubIndex::~SubIndex( void )
{
	//ISAM�t�@�C���N���[�Y
	Close();

	//�t�@�C���p�X
	if( FilePath != nullptr ) delete FilePath;

	//�t�@�C���o�b�t�@�폜
	if( FileBuff != nullptr ) delete FileBuff;

	//�w�b�_���
	if( FID != nullptr ) delete FID;
	if( ISAMVer != nullptr ) delete ISAMVer;
	if( SubKeyInfo != nullptr ) delete SubKeyInfo;
}

//------------------------------------------------------------------------------
// �t�@�C���A�N�Z�X ��{
//------------------------------------------------------------------------------
//***** �t�@�C���N���[�Y
Int32 FJIsam::SubIndex::Close(void)
{

	//���Ƀt�@�C�����N���[�Y�Ȃ牽�����Ȃ�
	if( FStream == nullptr )
	{
		return 0;
	}

	//�t�@�C��������ɂ���
	delete FilePath;

	//ISAM�t�@�C���N���[�Y
	FStream->Close();
	delete FStream;
	FStream = nullptr;

	BuildID = 0;

	//���N���A
	if( FID != nullptr ) delete FID;
	if( ISAMVer != nullptr ) delete ISAMVer;
	AllRecCnt = 0;
	KeyCnt = 0;

	RecCnt = 0;
	RecFirstPos = 0L;
	RecLastPos = 0L;
	RecRootPos = 0L;

	DelRecCnt = 0;
	DelRecFirstPos = 0L;
	DelRecLastPos = 0L;

	//�L�[���N���A
	if( SubKeyInfo != nullptr ) delete SubKeyInfo;
	SubKeyInfo = nullptr;

	//-- ����I�� --//
	return 0;
}

//------------------------------------------------------------------------------
//�T�u�C���f�b�N�X�󋵎擾
//------------------------------------------------------------------------------
//***** �t�@�C���p�X�擾
String^ FJIsam::SubIndex::getFilePath( String^ otPath )
{
	//�t�@�C�����I�[�v�����Ă��Ȃ����nullptr��Ԃ�
	if( !isOpen()  )
	{
		return nullptr;
	}

	//otPath��
	if( otPath == nullptr )
	{
		return String::Copy( FilePath );
	//otPath����
	}else{
		otPath->Remove( 0 );
		otPath->CopyTo( 0, FilePath->ToCharArray(), 0, FilePath->Length );
		return otPath;
	}
}

//***** BuildID�擾
Int32 FJIsam::SubIndex::getBuildID( void )
{
	return BuildID;
}

//***** �t�@�C���I�[�v���`�F�b�N
Boolean FJIsam::SubIndex::isOpen( void )
{
	if( FStream == nullptr )
	{
		return false;
	}else{
		return true;
	}	
}
//------------------------------------------------------------------------------
//ISAM ���擾�p�֐�
//------------------------------------------------------------------------------
//***** �t�@�C��ID�擾 *****
String^ FJIsam::SubIndex::getFID( String^ otFID )
{
	//-- �����`�F�b�N --//
	if( otFID == nullptr )
	{
		return String::Copy(FID);
	}

	//-- �f�[�^�R�s�[ --//
	otFID->CopyTo( 0, FID->ToCharArray(), 0, IS_FIDLEN );

	//-- ����I�� --//
	return otFID;
}

//***** �t�@�C���쐬ISAM�o�[�W�����擾
String^ FJIsam::SubIndex::getVersion( String^ otVer )
{
	//-- �����`�F�b�N --//
	if( otVer == nullptr )
	{
		return String::Copy(ISAMVer);
	}

	//-- �f�[�^�R�s�[ --//
	otVer->CopyTo( 0, ISAMVer->ToCharArray(), 0, IS_VERLEN );

	//-- ����I�� --//
	return otVer;
}

//***** ���R�[�h���擾 
UInt32 FJIsam::SubIndex::getRecCnt( void )
{
	return RecCnt;
}

//***** �폜���R�[�h���擾
UInt32 FJIsam::SubIndex::getDelRecCnt( void )
{
	return DelRecCnt;
}

//***** �S���R�[�h���擾
UInt32 FJIsam::SubIndex::getAllRecCnt( void )
{
	return AllRecCnt;
}

//***** �L�[���擾
UInt32 FJIsam::SubIndex::getKeyCnt( void )
{
	return KeyCnt;
}

//***** ���䕔�f�[�^���擾
Int32 FJIsam::SubIndex::Length( void )
{
	return	sizeof(Char) * IS_FIDLEN +	//�t�@�C��ID
			sizeof(Char) * IS_VERLEN +	//ISAM�o�[�W����

			sizeof(UInt32) +			//�S���R�[�h��
			sizeof(UInt32) +			//�L�[��

			sizeof(UInt32) +			//�L�����R�[�h��
			sizeof(Int64) +				//�擪���R�[�h
			sizeof(Int64) +				//�ŏI���R�[�h
			sizeof(Int64) +				//�񕪖؃��[�g

			sizeof(UInt32) +			//�폜���R�[�h��
			sizeof(Int64) +				//�擪���R�[�h
			sizeof(Int64) +				//�ŏI���R�[�h

			FJIsam::KeyInfo::Length();	//�L�[���
}
//------------------------------------------------------------------------------
//�L�[���擾
//------------------------------------------------------------------------------
//***** �L�[���擾
KeyInfo^ FJIsam::SubIndex::getKeyInfo( void )
{
	return gcnew KeyInfo( SubKeyInfo );	
}
//***** �C���f�b�N�X���擾
String^ FJIsam::SubIndex::getKeyName( String^ otStr )
{
	return SubKeyInfo->getKeyName( otStr );
}

//-- �L�[�ʒu�擾
Int32 FJIsam::SubIndex::getKeyPos( void )
{
	return SubKeyInfo->getKeyPos();
}

//-- �L�[���擾
Int32 FJIsam::SubIndex::getKeyLen( void )
{
	return SubKeyInfo->getKeyLen();
}

//***** �L�[�^�C�v�擾
keyType_EN	FJIsam::SubIndex::getKeyType( void )
{
	return SubKeyInfo->getKeyType();
}

//***** �L�[�����擾
Byte FJIsam::SubIndex::getKeyOrder( void )
{
	return SubKeyInfo->getKeyOrder();
}

//***** ���L�[�N�L�[�t���O�擾
Byte FJIsam::SubIndex::getKeyUniqueFlg( void )
{
	return SubKeyInfo->getKeyUniqueFlg();
}

//***** �\�[�g�L�[�ʒu�擾
Int32 FJIsam::SubIndex::getSrtKeyPos( void )
{
	return SubKeyInfo->getSrtKeyPos();
}

//***** �\�[�g�L�[���擾
Int32 FJIsam::SubIndex::getSrtKeyLen( void )
{
	return SubKeyInfo->getSrtKeyLen();
}

//***** �\�[�g�L�[�^�C�v�擾
keyType_EN FJIsam::SubIndex::getSrtKeyType( void )
{
	return SubKeyInfo->getSrtKeyType();
}

//***** �\�[�g�L�[�����擾
Byte FJIsam::SubIndex::getSrtKeyOrder( void )
{
	return SubKeyInfo->getSrtKeyOrder();
}

