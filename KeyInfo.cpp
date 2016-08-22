//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@ISAM�L�[��� �N���X�@�\�[�X
//	Version 1.0		create date 2002/07/24
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	classes
//		keyinfo	:ISAM�L�[���
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "KeyInfo.h"			//�L�[���
#include "FJIsamFunc.h"			//ISAM�p�֐�
#include "FJIsamException.h"	//FJIsam��O�N���X

using namespace FJIsam;

//------------------------------------------------------------------------------
//***** �R���X�g���N�^�P *****
FJIsam::KeyInfo::KeyInfo()
{
}

//***** �R���X�g���N�^�Q *****
FJIsam::KeyInfo::KeyInfo(
						String^		inKeyName,
						Int32		inKeyPos,
						Int32		inKeyLen,
						keyType_EN	inKeyType,
						Byte		inKeyOrder,
						Byte		inKeyUniqueFlg,
						Int32		inSrtKeyPos,
						Int32		inSrtKeyLen,
						keyType_EN	inSrtKeyType,
						Byte		inSrtKeyOrder
						)
{
	//������
	Set( inKeyName, inKeyPos, inKeyLen, inKeyType, inKeyOrder,inKeyUniqueFlg, 
		inSrtKeyPos, inSrtKeyLen, inSrtKeyType, inSrtKeyOrder );

}

//***** �R���X�g���N�^�R *****
FJIsam::KeyInfo::KeyInfo( KeyInfo^ source )
{
	//������
	Set( source->KeyName, source->KeyPos, source->KeyLen, 
		source->KeyType, source->KeyOrder,source->KeyUniqueFlg, 
		source->SrtKeyPos, source->SrtKeyLen, source->SrtKeyType, source->SrtKeyOrder );

}

//------------------------------------------------------------------------------
//***** �f�X�g���N�^ *****
FJIsam::KeyInfo::~KeyInfo()
{
	delete KeyName;	
}

//------------------------------------------------------------------------------
//�L�[���擾
//------------------------------------------------------------------------------
//***** �L�[���擾 *****
String^ FJIsam::KeyInfo::getKeyName(String^ otStr )
{
	//-- �����`�F�b�N --//
	if( otStr == nullptr )
	{
		return String::Copy( KeyName );
	}

	//-- �L�[���Z�b�g --//
	otStr->CopyTo(0, KeyName->ToCharArray(), 0, IS_KEYNAMELEN );

	//����I��
	return otStr;

}

//***** �L�[���擾 *****
Int32 FJIsam::KeyInfo::getKeyLen( void )
{
	return KeyLen;
}

//***** �L�[�ʒu�擾
Int32 FJIsam::KeyInfo::getKeyPos( void )
{
	return KeyPos;
}

//***** �L�[�^�C�v�擾 *****
keyType_EN FJIsam::KeyInfo::getKeyType( void )
{
	return KeyType;
}

//***** �L�[�����擾 *****
Byte FJIsam::KeyInfo::getKeyOrder( void )
{
	return KeyOrder;
}

//***** ���j�[�N�L�[�t���O�擾 *****
Byte FJIsam::KeyInfo::getKeyUniqueFlg( void )
{
	return KeyUniqueFlg;
}

//***** �\�[�g�L�[���擾 *****
Int32 FJIsam::KeyInfo::getSrtKeyLen( void )
{
	return SrtKeyLen;
}

//***** �\�[�g�L�[�ʒu�擾 *****
Int32 FJIsam::KeyInfo::getSrtKeyPos( void )
{
	return SrtKeyPos;
}

//***** �\�[�g�L�[�^�C�v�擾 *****
keyType_EN FJIsam::KeyInfo::getSrtKeyType( void )
{
	return SrtKeyType;
}

//***** �\�[�g�L�[�����擾 *****
Byte FJIsam::KeyInfo::getSrtKeyOrder( void )
{
	return SrtKeyOrder;
}


//------------------------------------------------------------------------------
//�L�[���ݒ�
//------------------------------------------------------------------------------
Int32 FJIsam::KeyInfo::Set(
						String^		inKeyName,
						Int32		inKeyPos,
						Int32		inKeyLen,
						keyType_EN	inKeyType,
						Byte		inKeyOrder,
						Byte		inKeyUniqueFlg,
						Int32		inSrtKeyPos,
						Int32		inSrtKeyLen,
						keyType_EN	inSrtKeyType,
						Byte		inSrtKeyOrder
				)
{
	//-- �P�̏����`�F�b�N --//
	//�L�[��
	if( inKeyName == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[������ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}
	if( inKeyName->Length > IS_KEYNAMELEN )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�����������܂��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}
	if( KeyName != nullptr ) delete KeyName;
	KeyName = String::Copy( inKeyName );

	//�L�[�ʒu
	KeyPos = inKeyPos;

	//�L�[��
	if( inKeyLen == 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[����0�ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}
	KeyLen = inKeyLen;

	//�L�[�^�C�v
	KeyType = inKeyType;

	//�L�[����
	if( inKeyOrder != 'A' && inKeyOrder != 'D' )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�������s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}
	KeyOrder = inKeyOrder;

	//���j�[�N�L�[�t���O
	if( inKeyUniqueFlg != IS_UNIQUEKEY &&
		inKeyUniqueFlg != IS_DUPLICATEKEY )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���j�[�N�L�[�t���O���s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 5;
		throw tmpFJIsamException;
	}
	KeyUniqueFlg = inKeyUniqueFlg;

	//�\�[�g�L�[�ʒu
	SrtKeyPos = inSrtKeyPos;

	//�\�[�g�L�[��
	SrtKeyLen = inSrtKeyLen;

	//�\�[�g�L�[�^�C�v
	SrtKeyType = inSrtKeyType;

	//�\�[�g�L�[����
	if( inSrtKeyLen != 0 &&	( inSrtKeyOrder != 'A' && inSrtKeyOrder != 'D') )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�\�[�g�L�[�������s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 6;
		throw tmpFJIsamException;
	}
	SrtKeyOrder = inSrtKeyOrder;

	//-- ���������`�F�b�N --//
	//�L�[�Ō���ʒu�`�F�b�N
	if( (Int64)(KeyPos + KeyLen) > Int32::MaxValue )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�ʒu���s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 7;
		throw tmpFJIsamException;
	}

	//�\�[�g�Ō���ʒu�`�F�b�N
	if( (Int64)(SrtKeyPos + SrtKeyLen) > Int32::MaxValue )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�\�[�g�L�[�ʒu���s���ł��B");
		tmpFJIsamException->ErrorCode = 3001;
		tmpFJIsamException->ErrorCode2 = 8;
		throw tmpFJIsamException;
	}

	//�L�[�`���������`�F�b�N
	switch( KeyType )
	{
	case keyType_EN::ISKEY_UNICODE :
		if( KeyLen % 2 == 1 )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�����s���ł�(UNICODE char)�B");
			tmpFJIsamException->ErrorCode = 3001;
			tmpFJIsamException->ErrorCode2 = 9;
			throw tmpFJIsamException;
		}
		break;

	case keyType_EN::ISKEY_SNUM :
	case keyType_EN::ISKEY_USNUM :
		if( KeyLen != 1 &&
			KeyLen != 2 &&
			KeyLen != 4 &&
			KeyLen != 8 )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�����s���ł�(numeric)�B");
			tmpFJIsamException->ErrorCode = 3001;
			tmpFJIsamException->ErrorCode2 = 10;
			throw tmpFJIsamException;
		}
	}

	//-- ����I�� --//
	return 0;
}

//------------------------------------------------------------------------------
//�t�@�C�����o�͗p�f�[�^�A�N�Z�X
//------------------------------------------------------------------------------
//***** �t�@�C�����o�͗p�f�[�^���擾 *****//
Int32 FJIsam::KeyInfo::Length( void )
{
	return sizeof(Char) *20 +
			sizeof(Int32) +
			sizeof(Int32) +
			sizeof(keyType_EN) +
			sizeof(Byte) +
			sizeof(Byte) +
			sizeof(Int32) +
			sizeof(Int32) +
			sizeof(keyType_EN) +
			sizeof(Byte);
}

//------------------------------------------------------------------------------
//�t�@�C���ǂݏo���f�[�^�擾
//------------------------------------------------------------------------------
Int32 FJIsam::KeyInfo::Read( array<Byte>^ inBuff, Int32 offset  )
{
	int tmpKeyType;

	//-- �����`�F�b�N --//
	if(  inBuff  == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�ǂݎ��̈悪��ł��B" );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	if( offset < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�I�t�Z�b�g�l���s���ł��B" );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	if( inBuff->Length - offset < KeyInfo::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�ǂݎ��̈悪���������܂��B" );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- �������Z�b�g --//
	try
	{
		//�L�[��
		KeyName = FJIFuncs::ArrayToString( inBuff, offset, IS_KEYNAMELEN );
		offset += IS_KEYNAMELEN * sizeof( Char );

		//�L�[�ʒu
		KeyPos = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( KeyPos );
		//�L�[��
		KeyLen = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( KeyLen );

		//�L�[�^�C�v
		tmpKeyType = FJIFuncs::ArrayToInt32( inBuff, offset );
		if( (keyType_EN)tmpKeyType < keyType_EN::ISKEY_UNICODE &&
			(keyType_EN)tmpKeyType > keyType_EN::ISKEY_USNUM )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�^�C�v���s���ł��B" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 4;
			throw tmpFJIsamException;
		}
		KeyType = (keyType_EN)tmpKeyType;
		offset += sizeof( tmpKeyType );

		//�L�[����
		KeyOrder = inBuff[offset];
		if( KeyOrder != 'A' && KeyOrder != 'D' && KeyOrder != ' ' )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[�������s���ł��B" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 5;
			throw tmpFJIsamException;
		}
		offset++;

		//���j�[�N�L�[�t���O
		KeyUniqueFlg = inBuff[offset];
		if( KeyUniqueFlg != IS_UNIQUEKEY &&
			KeyUniqueFlg != IS_DUPLICATEKEY )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"���j�[�N�L�[�t���O���s���ł��B" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 6;
			throw tmpFJIsamException;
		}
		offset++;

		//�\�[�g�L�[�ʒu
		SrtKeyPos = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( SrtKeyPos );

		//�\�[�g�L�[��
		SrtKeyLen = FJIFuncs::ArrayToInt32( inBuff, offset );
		offset += sizeof( SrtKeyLen );

		//�\�[�g�L�[�^�C�v
		tmpKeyType = FJIFuncs::ArrayToInt32( inBuff, offset );
		if( (keyType_EN)tmpKeyType < keyType_EN::ISKEY_UNICODE &&
			(keyType_EN)tmpKeyType > keyType_EN::ISKEY_USNUM )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�\�[�g�L�[�^�C�v���s���ł��B" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 7;
			throw tmpFJIsamException;
		}
		SrtKeyType = (keyType_EN)tmpKeyType;
		offset += sizeof( tmpKeyType );

		//�L�[����
		SrtKeyOrder = inBuff[offset];
		if( SrtKeyOrder != 'A' && SrtKeyOrder != 'D' && SrtKeyOrder != ' ' )
		{
			FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�\�[�g�L�[�������s���ł��B" );
			tmpFJIsamException->ErrorCode = 3002;
			tmpFJIsamException->ErrorCode2 = 8;
			throw tmpFJIsamException;
		}
		offset++;

	}catch(Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[���ݒ�Ɏ��s���܂����B", exp );
		tmpFJIsamException->ErrorCode = 3002;
		tmpFJIsamException->ErrorCode2 = 9;
		throw tmpFJIsamException;
	}

	//-- ����I�� --//
	return offset;
}

//------------------------------------------------------------------------------
//�L�[��񏑍�
//------------------------------------------------------------------------------
int FJIsam::KeyInfo::Write( array<Byte>^ otBuff, Int32 offset )
{
	//-- �����`�F�b�N --//
	//�o�b�t�@�L��
	if( otBuff == nullptr )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�����̈悪��ł��B");
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 1;
		throw tmpFJIsamException;
	}

	//�I�t�Z�b�g�`�F�b�N
	if( offset < 0 )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�I�t�Z�b�g�l���s���ł��B");
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 2;
		throw tmpFJIsamException;
	}

	//�o�b�t�@�T�C�Y
	if(otBuff->Length - offset < KeyInfo::Length() )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�����̈悪���������܂��B");
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 3;
		throw tmpFJIsamException;
	}

	//-- �������Z�b�g --//
	try
	{
		//�L�[��
		FJIFuncs::StringToArray( KeyName, otBuff, offset, IS_KEYNAMELEN );
		offset += IS_KEYNAMELEN * sizeof( Char );
	
		//�L�[�ʒu
		FJIFuncs::Int32ToArray( KeyPos, otBuff, offset );
		offset += sizeof( KeyPos );

		//�L�[��
		FJIFuncs::Int32ToArray( KeyLen, otBuff, offset );
		offset += sizeof( KeyLen );

		//�L�[���
		FJIFuncs::Int32ToArray( (Int32)KeyType, otBuff, offset );
		offset += sizeof( Int32 );

		//�L�[����
		otBuff[offset] = KeyOrder;
		offset++;

		//���j�[�N�L�[�t���O
		otBuff[offset] = KeyUniqueFlg;
		offset++;

		//�\�[�g�L�[�ʒu
		FJIFuncs::Int32ToArray( SrtKeyPos, otBuff, offset );
		offset += sizeof( SrtKeyPos );

		//�\�[�g�L�[��
		FJIFuncs::Int32ToArray( SrtKeyLen, otBuff, offset );
		offset += sizeof( SrtKeyLen );

		//�\�[�g�L�[���
		FJIFuncs::Int32ToArray( (Int32)SrtKeyType, otBuff, offset );
		offset += sizeof( Int32 );

		//�\�[�g�L�[����
		otBuff[offset] = SrtKeyOrder;
		offset++;

	}catch( Exception^ exp )
	{
		FJIsamException^ tmpFJIsamException= gcnew FJIsamException( L"�L�[���o�͂Ɏ��s���܂����B", exp);
		tmpFJIsamException->ErrorCode = 3003;
		tmpFJIsamException->ErrorCode2 = 4;
		throw tmpFJIsamException;
	}

	//--����I�� --//
	return offset;
}
