//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@�w�b�_
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		FJIsam :	FJIsam ���������A�쐬�A��񕔃\�[�X
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "FJIsam.h"				//ISAM�t�@�C��
#include "FJIsamFunc.h"			//ISAM�p�֐�
#include "FJIsamException.h"	//FJIsam��O�N���X

using namespace FJIsam;
using namespace System;
using namespace System::IO;

//------------------------------------------------------------------------------
//  class	:FJIsam	:ISAM�@���C���N���X
//------------------------------------------------------------------------------

//***** �R���X�g���N�^�P *****
FJIsam::FIsam::FIsam()
{
}

//***** �R���X�g���N�^�R *****
FJIsam::FIsam::FIsam( String^ inFname, Int32 inBufsz, IsamOpenMode inMode )
	:	BuildID(0)						//�t�@�C���Ґ��J�E���^
{
	//�t�@�C���I�[�v��
	if( inFname != nullptr &&
		inFname->Length > 0 )
	{
		this->Open( inFname, inBufsz, inMode );
	}
}

//------------------------------------------------------------------------------
//***** �f�X�g���N�^ *****
FJIsam::FIsam::~FIsam()
{
	//ISAM�t�@�C���N���[�Y
	Close();
}


//------------------------------------------------------------------------------
// �t�@�C���A�N�Z�X ��{
//------------------------------------------------------------------------------
//***** ISAM�t�@�C���I�[�v��
Int32 FJIsam::FIsam::Open( String^ inFname, Int32 inBufsz, IsamOpenMode inMode )
{
	SubIndex^		tmpSubIdx;			//�T�u�C���f�b�N�X���[�N
	String^			tmpFilePath;		//�t�@�C���p�X
	String^			tmpExt;				//�g���q

	//-- ���łɃt�@�C�����I�[�v�����Ă���΃G���[
	if( FStream != nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���Ƀt�@�C�����J���Ă��܂��B");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N
	if( inFname == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C���������w��ł��B");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( inFname->Length == 0 )			//�t�@�C���������w��
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C��������ł��B");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inMode != IsamOpenMode::READ_WRITE &&		//�t�@�C���I�[�v�����[�h�`�F�b�N
		inMode != IsamOpenMode::READ_ONLY )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C���I�[�v�����@���s���ł��B");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//�t�@�C���o�b�t�@�쐬
	if( inBufsz < 4096 )
	{
		inBufsz = 4096;
	}

	//�t�@�C���̕����`�F�b�N(���p�ł��Ȃ����������邩)
	if( inFname->IndexOfAny( Path::GetInvalidPathChars() ) != -1 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C�����ɕs���ȕ��������p����Ă��܂��B");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}	
	//�g���q�`�F�b�N( .fis���H)
	tmpExt = gcnew String(IS_EXTSTR);
	if( String::Compare( Path::GetExtension( inFname ), tmpExt , true ) == 0 )
	{
		// .fis �Ȃ炻�̂܂�
		tmpFilePath = String::Copy( inFname );
	}else {
		// .fis �ȊO�Ȃ� .fis ��t��
		tmpFilePath = String::Copy( inFname );
		tmpFilePath->Concat( tmpExt  );
	}

	//�t�@�C�����݃`�F�b�N
	if( !File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�w�肳�ꂽ�t�@�C�������݂��܂���B");
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}

	//�t�@�C���o�b�t�@��`
	FileBuff = gcnew array<Byte>(inBufsz);

	//-- �w�b�_���̈��` --//
	FID = gcnew String( L'\x0000', IS_FIDLEN );
	ISAMVer = gcnew String( L'\x0000', IS_VERLEN );
	ISKeyInfo = nullptr;
	UserInfo = gcnew String( L'\x0000', IS_UINFLEN );

	//�T�u�C���f�b�N�X���
	SubIdx = gcnew List<SubIndex^>();

	//�t�@�C�����Z�b�g
	FilePath = tmpFilePath;

	//-- �t�@�C���I�[�v��(�����̂�)
	try{
		//�ǂݏ����\
		if( inMode == IsamOpenMode::READ_WRITE )
		{
			FStream = File::Open( tmpFilePath,		//�t�@�C����
						FileMode::Open,				//�쐬���[�h
						FileAccess::ReadWrite,		//�A�N�Z�X���[�h
						FileShare::None );			//���L�F����

		//�Ǎ���p
		}else{
			FStream = File::Open( tmpFilePath,		//�t�@�C����
						FileMode::Open,				//�쐬���[�h
						FileAccess::Read,			//�A�N�Z�X���[�h
						FileShare::Read );			//���L�F�Ǎ���
		}
	}catch( Exception^ exp ){
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�t�@�C���̃I�[�v���Ɏ��s���܂����B",exp );
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//-- �w�b�_���ǂݍ���
	try
	{
		ReadHeader();
	}catch( Exception^ exp ){
		FStream->Close();
		delete FStream;
		FStream = nullptr;

		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�w�b�_�Ǎ��Ɏ��s���܂����B",exp );
		tmpFJIsamException->ErrorCode = 1001;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X�t�@�C���I�[�v��
	for( Int32 i = 2; i <= IdxCnt ; i++)
	{
		try
		{
			tmpSubIdx = gcnew SubIndex( this, i, inMode );
			SubIdx->Add( tmpSubIdx );

		//�T�u�C���f�b�N�X�̃I�[�v���Ɏ��s������S�̂��N���[�Y
		}catch(FJIsamException^ tmpFJIsamException2 )
		{
			delete tmpSubIdx;

			for( Int32 j = 0; j < i-2; j++ )
			{
				tmpSubIdx->Close();
				delete tmpSubIdx;
				SubIdx[i-2] = nullptr;
			}

			FStream->Close();
			delete FStream;
			FStream = nullptr;

			//��O�𑗐M
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( String::Format( L"�T�u�C���f�b�N�X{0:d2}�̃I�[�v���Ɏ��s���܂����B", i ), 
														tmpFJIsamException2 );
			tmpFJIsamException->ErrorCode = 1001;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}

	}

	//-- ISAM�����W --//
	CreateBuildID();
	try
	{
		if( ChkKeyPositions() != 0 )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�L�[���擾�Ɏ��s���܂����B" );														
			tmpFJIsamException->ErrorCode = 1001;
			tmpFJIsamException->ErrorCode2 = 10;
		}
	}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�̃L�[���擾�Ɏ��s���܂����B", exp );														
			tmpFJIsamException->ErrorCode = 1001;
			tmpFJIsamException->ErrorCode2 = 11;
	}

	//-- ����I��--//
	delete tmpExt;

	return 0;
}

//------------------------------------------------------------------------------
//***** ISAM�t�@�C���N���[�Y
Int32 FJIsam::FIsam::Close(void)
{
	SubIndex^	tmpSubIdx;						//�T�u�C���f�b�N�X���[�N

	//���Ƀt�@�C�����N���[�Y�Ȃ牽�����Ȃ�
	if( FStream == nullptr )
	{
		return 0;
	}

	//�t�@�C���o�b�t�@
	if( FileBuff != nullptr ) delete FileBuff;
	FileBuff = nullptr;

	//�w�b�_���̈��`
	if( FID != nullptr ) delete FID;
	FID = nullptr;
	if( ISAMVer != nullptr ) delete ISAMVer;
	ISAMVer = nullptr;
	if( ISKeyInfo != nullptr ) delete ISKeyInfo;
	ISKeyInfo = nullptr;
	if( UserInfo != nullptr ) delete UserInfo;
	UserInfo = nullptr;
	//�t�@�C��������ɂ���
	if( FilePath != nullptr ) delete FilePath;
	FilePath = nullptr;

	//ISAM�t�@�C���N���[�Y
	FStream->Close();
	delete FStream;
	FStream = nullptr;

	BuildID = 0;

	//�T�u�C���f�b�N�X�t�@�C���N���[�Y
	for( Int32 i = 2; i <= IdxCnt ; i++)
	{
		tmpSubIdx = SubIdx[i-2];
		if(tmpSubIdx != nullptr )
		{
			tmpSubIdx->Close();
			delete tmpSubIdx;
		}
	}
	SubIdx->Clear();
	if( SubIdx != nullptr ) delete SubIdx;

	//���N���A
	AllRecCnt = 0;
	KeyCnt = 0;

	RecCnt = 0;
	RecFirstPos = 0L;
	RecLastPos = 0L;
	RecRootPos = 0L;

	DelRecCnt = 0;
	DelRecFirstPos = 0L;
	DelRecLastPos = 0L;

	RecMaxLen = 0;
	RecMinLen = 0;

	IdxCnt = 0;

	//-- ����I�� --//
	return 0;
}

//------------------------------------------------------------------------------
//***** ISAM�t�@�C���쐬
Int32 FJIsam::FIsam::Create( String^ inFname, String^ inFID,
						  KeyInfo^ inKeyInfo, String^ inUserInfo,
						  Int32 inBufsz,
						  System::Security::AccessControl::FileSecurity^ inFileSecurity  )
{
	String^			tmpFilePath;		//�t�@�C���p�X
	String^			tmpExt;				//�g���q

	//-- ISAM�̏�ԃ`�F�b�N�i�t�@�C�����J���Ă���ƃG���[�ɂ���)
	if( FStream != nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���Ƀt�@�C�����J���Ă��܂��B�t�@�C���������A�V�K�쐬���Ă��������B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �����`�F�b�N
	//�t�@�C����
	if( inFname == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�t�@�C���������w��ł��B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( inFname->Length == 0 )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�t�@�C���������w��ł��B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	if( inFID == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�t�@�C��ID�����w��ł��B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}
	if( inFID->Length == 0 || inFID->Length > IS_FIDLEN )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�t�@�C��ID�̒������s���ł�(1�`16����)�B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}
	//�L�[���
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�L�[�����w�肵�Ă��������B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}
	//���[�U�[���
	if( inUserInfo != nullptr
		&& inUserInfo->Length > IS_UINFLEN)
	{
		FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"���[�U�[���̒������s���ł�(0�`256����)�B" );
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//�t�@�C���o�b�t�@�`�F�b�N
	if( inBufsz < 4096 )
	{
		inBufsz = 4096;
	}

	//-- �t�@�C�����`�F�b�N
	//�t�@�C���̕����`�F�b�N
	if( inFname->IndexOfAny( Path::GetInvalidPathChars() ) != -1 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C�����ɕs���ȕ��������p����Ă��܂��B");
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}	
	//�g���q�`�F�b�N( .fis���H)
	tmpExt = gcnew String( IS_EXTSTR );
	if( String::Compare( Path::GetExtension( inFname ), tmpExt, true ) == 0 )
	{
		// .fis �Ȃ炻�̂܂�
		tmpFilePath = String::Copy( inFname );
	}else {
		// .fis �ȊO�Ȃ� .fis ��t��
		tmpFilePath = String::Copy( inFname );
		tmpFilePath->Concat( tmpExt );
	}

	//�t�@�C�����݃`�F�b�N
	if( File::Exists( tmpFilePath ) )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�������O�̃t�@�C���������݂��܂��B");
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;

	}

	//-- ISAM�V�K�쐬 --//
	//�t�@�C���o�b�t�@��`
	FileBuff = gcnew array<Byte>(inBufsz);

	//�T�u�C���f�b�N�X���
	SubIdx = gcnew List<SubIndex^>();

	try
	{
		FStream = nullptr;

		if( inFileSecurity == nullptr )
		{
			FStream  = File::Create( tmpFilePath,
						FileBuff->Length,
						FileOptions::RandomAccess );
		}else{
			FStream = File::Create( tmpFilePath,
						FileBuff->Length,
						FileOptions::RandomAccess,
						inFileSecurity);
		}
	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�t�@�C���̍쐬�Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 10;

		throw tmpFJIsamException;
	}

	//-- ISAM������ݒ�
	//�t�@�C��ID�ݒ�
	FID = String::Copy( inFID );

	//ISAM�o�[�W�����ݒ�
	try
	{
		ISAMVer = L" 1. 2. 0. 0";
	}catch(Exception^ exp ){
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAM�̃o�[�W�����擾�Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 11;

		throw tmpFJIsamException;
	}
	//�L�[���ݒ�
	ISKeyInfo = gcnew KeyInfo( inKeyInfo );

	//���[�U�[���ݒ�(�C��)
	if( inUserInfo != nullptr )
	{
		UserInfo = String::Copy( inUserInfo );
	}else{
		UserInfo = L"";
	}

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
	
	RecMaxLen = 0;
	RecMinLen = 0;

	IdxCnt = 1;

	//-- ISAM���䕔����
	try
	{
		WriteHeader();
	}catch(Exception^ exp)
	{
		FStream->Close();
		delete FStream;
		FStream = nullptr;

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAM���̍쐬�Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 1002;
		tmpFJIsamException->ErrorCode2 = 11;

		throw tmpFJIsamException;
	}

	//�t�@�C�����Z�b�g
	FilePath = String::Copy( tmpFilePath );

	//-- ISAM�����W --//
	CreateBuildID();
	try
	{
		if( ChkKeyPositions() != 0 )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�L�[���擾�Ɏ��s���܂����B" );														
			tmpFJIsamException->ErrorCode = 1002;
			tmpFJIsamException->ErrorCode2 = 12;
		}
	}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�̃L�[���擾�Ɏ��s���܂����B", exp );														
			tmpFJIsamException->ErrorCode = 1002;
			tmpFJIsamException->ErrorCode2 = 13;
	}

	delete tmpExt;
	//-- ����I�� --//
	return 0;
}

//------------------------------------------------------------------------------
//***** �T�u�C���f�b�N�X�쐬 *****
Int32	FJIsam::FIsam::CreateSubIndex( KeyInfo^ inKeyInfo )
{
	SubIndex^		tmpSubIdx;		//�T�u�C���f�b�N�X

	//-- �����쐬 --//
	if( inKeyInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�L�[���ݒ肳��Ă��܂���B");
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//�쐬�\�`�F�b�N(���̂Ƃ��냌�R�[�h���Ȃ��ꍇ��������)
	if( AllRecCnt > 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�T�u�C���f�b�N�X�̍쐬���o���܂���B");
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//-- �L�[�ʒu�A�\�[�g�L�[�ʒu�̐������`�F�b�N(�����I�Ȃ���) --//
	if( RecCnt > 0 )
	{
		if( RecMinLen > inKeyInfo->getKeyPos() + inKeyInfo->getKeyLen() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"�L�[���ŏ����R�[�h�������ɂ���܂��B�L�[��ݒ�ł��܂���B");
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 3;
			throw tmpFJIsamException;
		}
		if( RecMinLen > inKeyInfo->getSrtKeyPos() + inKeyInfo->getSrtKeyLen() )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				L"�\�[�g�L�[���ŏ����R�[�h�������ɂ���܂��B�L�[��ݒ�ł��܂���B");
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
	}
	//-- �T�u�C���f�b�N�X�쐬 --//
	
	try
	{
		tmpSubIdx = gcnew SubIndex( this, IdxCnt+1, inKeyInfo );

	}catch( Exception^ ext )
	{
		delete tmpSubIdx;

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�T�u�C���f�b�N�X�̍쐬�Ɏ��s���܂����B",
													ext );
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}

	//-- �T�u�C���f�b�N�X��List�ɒǉ� --//
	SubIdx->Add( tmpSubIdx );
	IdxCnt++;

	//-- ���䕔�������� --//
	try
	{
		WriteHeader();
	}catch( Exception^ ext )
	{
		SubIndex^ chkSubIdx = SubIdx[IdxCnt-2];
		chkSubIdx->Close();
		delete chkSubIdx;

		SubIdx->RemoveAt(IdxCnt-2);

		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"ISAM�w�b�_�̍X�V�Ɏ��s���܂����B", ext );
		tmpFJIsamException->ErrorCode = 1003;
		tmpFJIsamException->ErrorCode = 6;
		throw tmpFJIsamException;
	}

	//-- ISAM�����W --//
	CreateBuildID();
	try
	{
		if( ChkKeyPositions() != 0 )
		{
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�L�[���擾�Ɏ��s���܂����B" );														
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 7;
		}
	}catch( Exception^ exp ){
			FJIsamException^ tmpFJIsamException = gcnew FJIsamException( L"�C���f�b�N�X�̃L�[���擾�Ɏ��s���܂����B", exp );														
			tmpFJIsamException->ErrorCode = 1003;
			tmpFJIsamException->ErrorCode2 = 8;
	}

	//-- ����I�� --//
	return 0;
}

//------------------------------------------------------------------------------
//ISAM�󋵎擾
//------------------------------------------------------------------------------
//***** �t�@�C���p�X�擾
String^ FJIsam::FIsam::getFilePath( String^ otPath )
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
Int32 FJIsam::FIsam::getBuildID( void )
{
	return BuildID;
}

//***** �t�@�C���I�[�v���`�F�b�N
Boolean FJIsam::FIsam::isOpen( void )
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
String^ FJIsam::FIsam::getFID( String^ otFID )
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

//***** �t�@�C���쐬ISAM�o�[�W�����擾 *****
String^ FJIsam::FIsam::getVersion( String^ otVer )
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


//***** ���[�U�[��`���擾 *****
String^ FJIsam::FIsam::getUserInfo( String^ otUInfo )
{
	//-- �����`�F�b�N --//
	if( otUInfo == nullptr )
	{
		return String::Copy( UserInfo );
	}

	otUInfo->CopyTo( 0, UserInfo->ToCharArray(), 0, UserInfo->Length );

	//-- ����I�� --//
	return otUInfo;
}

//***** ���R�[�h���擾 ***** 
UInt32 FJIsam::FIsam::getRecCnt( void )
{
	return RecCnt;
}

//***** �폜���R�[�h���擾 *****
UInt32 FJIsam::FIsam::getDelRecCnt( void )
{
	return DelRecCnt;
}

//***** �S���R�[�h���擾 *****
UInt32 FJIsam::FIsam::getAllRecCnt( void )
{
	return AllRecCnt;
}

//***** �L�[���擾 *****
UInt32 FJIsam::FIsam::getKeyCnt( void )
{
	return KeyCnt;
}

//***** ���R�[�h�ő咷 *****
Int32 FJIsam::FIsam::getRecMaxLen( void )
{
	return RecMaxLen;
}

//***** ���R�[�h�ŏ��� *****
Int32 FJIsam::FIsam::getRecMinLen()
{
	return RecMinLen;

}

//***** �C���f�b�N�X���擾
Int32 FJIsam::FIsam::getIdxCnt( void )
{
	return IdxCnt;
}

//***** �L�[�̍ŏ��̈ʒu�擾
Int32 FJIsam::FIsam::getKeyStartPos( void )
{
	return KeyStartPos;
}

											
//***** �L�[�̍Ō�̈ʒu�擾
Int32 FJIsam::FIsam::getKeyLastPos( void )
{
	return KeyLastPos;
}											

//***** ���䕔�f�[�^���擾
Int32 FJIsam::FIsam::Length( void )
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

			sizeof(Int32) +				//�ő僌�R�[�h��
			sizeof(Int32) +				//�ŏ����R�[�h��

			sizeof(Int32) +				//�C���f�b�N�X��

			FJIsam::KeyInfo::Length() +		//�L�[���
			
			sizeof(Char) * IS_UINFLEN;	//���[�U�[���

}

//------------------------------------------------------------------------------
//�L�[���擾
//------------------------------------------------------------------------------
//***** �C���f�b�N�X�ԍ��擾
Int32 FJIsam::FIsam::getIdxNum(String^ inKeyName )
{
	//-- �����`�F�b�N
	if( inKeyName == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException("�L�[�������ݒ�ł��B");
		tmpFJIsamException->ErrorCode = 1004;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- �L�[������(��L�[)
	if( String::Compare( ISKeyInfo->getKeyName( nullptr ), inKeyName ) == 0 )
	{
		return 1;
	}

	//-- �L�[������(�T�u�C���f�b�N�X�L�[) --//
	for( int i = 0; i < IdxCnt - 2; i++ )
	{
		SubIndex^ tmpSubIdx = SubIdx[i];
		if(tmpSubIdx == nullptr )
		{
			continue;
		}

		if( String::Compare( tmpSubIdx->getKeyName( nullptr ), inKeyName ) == 0 )
		{
			return i+2;
		}
	}

	//-- �Y���Ȃ��@--//
	FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�Y������L�[������܂���B");
	tmpFJIsamException->ErrorCode2 = 1004;
	tmpFJIsamException->ErrorCode2 = 2;
	throw tmpFJIsamException;
}

//***** �L�[���擾
KeyInfo^ FJIsam::FIsam::getKeyInfo( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1005;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return gcnew KeyInfo( ISKeyInfo );

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum-2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�Y������T�u�C���f�b�N�X������܂���B");
			tmpFJIsamException->ErrorCode = 1005;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyInfo();
	}
	
}

//***** �C���f�b�N�X�L�[���擾
String^ FJIsam::FIsam::getKeyName( Int32 inIdxNum, String^ otStr )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1006;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyName( otStr );

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum-2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�Y������T�u�C���f�b�N�X������܂���B");
			tmpFJIsamException->ErrorCode = 1006;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyName( otStr );
	}

}

//-- �L�[�ʒu�擾
Int32 FJIsam::FIsam::getKeyPos( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N --//
	if( inIdxNum > IdxCnt )	//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1007;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//-- ���C���C���f�b�N�X --//
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyPos();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1007;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyPos();
	}
}

//-- �L�[���擾
Int32 FJIsam::FIsam::getKeyLen( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1008;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyLen();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1008;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyLen();
	}
}

//***** �L�[�^�C�v�擾
keyType_EN	FJIsam::FIsam::getKeyType( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1009;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyType();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( 
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1009;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyType();
	}
}

//***** �L�[�����擾
Byte		FJIsam::FIsam::getKeyOrder( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1010;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyOrder();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1010;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyOrder();
	}
}

//***** ���L�[�N�L�[�t���O�擾
Byte		FJIsam::FIsam::getKeyUniqueFlg( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1011;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getKeyUniqueFlg();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1011;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getKeyUniqueFlg();
	}
}

//***** �\�[�g�L�[�ʒu�擾
Int32		FJIsam::FIsam::getSrtKeyPos( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1012;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyPos();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1012;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyPos();
	}
}

//***** �\�[�g�L�[���擾
Int32		FJIsam::FIsam::getSrtKeyLen( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1013;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyLen();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1013;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyLen();
	}
}

//***** �\�[�g�L�[�^�C�v�擾
keyType_EN	FJIsam::FIsam::getSrtKeyType( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1014;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyType();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1014;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyType();
	}
}

//***** �\�[�g�L�[�����擾
Byte		FJIsam::FIsam::getSrtKeyOrder( Int32 inIdxNum )
{
	SubIndex^	chkSubIdx;			//�`�F�b�N�pSubIndex

	//-- �����`�F�b�N
	if( inIdxNum > IdxCnt )			//�C���f�b�N�X�ԍ�
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"�C���f�b�N�X�ԍ����s���ł��B");
		tmpFJIsamException->ErrorCode = 1015;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;

	}

	//-- ���C���C���f�b�N�X
	if( inIdxNum == 1 )
	{
		return ISKeyInfo->getSrtKeyOrder();

	//-- �T�u�C���f�b�N�X --//
	}else{
		chkSubIdx = SubIdx[inIdxNum - 2];
		//�Y���C���f�b�N�X�Ȃ�
		if(chkSubIdx == nullptr )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException(
				String::Format( L"�Y������T�u�C���f�b�N�X������܂���(�C���f�b�N�X{0:d2})�B", inIdxNum ) );
			tmpFJIsamException->ErrorCode = 1015;
			tmpFJIsamException->ErrorCode2 = 2;
			throw tmpFJIsamException;
		}
		return chkSubIdx->getSrtKeyOrder();
	}
}

//------------------------------------------------------------------------------
//���[�U�[��`���ݒ�
//------------------------------------------------------------------------------
Int32 FJIsam::FIsam::setUserInfo( String^ inUInfo )
{
	array<Byte>^ otUserInfo;

	//-- �����`�F�b�N --//
	if( inUInfo == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"���[�U�[��񂪋�ł��B");
		tmpFJIsamException->ErrorCode = 1016;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inUInfo->Length > IS_UINFLEN )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException(L"���[�U�[��񂪒������܂�(�ő�256����)�B");
		tmpFJIsamException->ErrorCode = 1016;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//���[�U�[�����X�V
	UserInfo->Remove(0);
	UserInfo->Concat(inUInfo );

	//�o�b�t�@�쐬	
	otUserInfo = FJIFuncs::StringToArray( UserInfo, nullptr, 0, IS_UINFLEN );	

	//�t�@�C���ɏ���
	FStream->Seek( FIsam::Length() - sizeof(Char) * IS_UINFLEN , SeekOrigin::Begin );
	FStream->Write( otUserInfo, 0, sizeof(Char) * IS_UINFLEN );

	//-- ����I��
	return 0;
}
