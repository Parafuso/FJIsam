//KeyInfo.h
//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@ISAM�L�[��� �N���X
//	Version	1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	classes
//		Keyinfo		:ISAM�L�[���
//------------------------------------------------------------------------------
#pragma once

#include "FJIsamDef.h"				//ISAM�萔��`

using namespace System;

//------------------------------------------------------------------------------
//	class	:keyInfo	:�L�[���
//------------------------------------------------------------------------------
namespace FJIsam
{
	//�L�[���
	public enum class keyType_EN	
	{
			ISKEY_UNICODE = 0,		//���j�R�[�h�FChars
			ISKEY_CHAR,				//���C�h�L�����N�^�[�FChar(SJIS��)
			ISKEY_BYTE,				//�o�C�g�f�[�^�FByte
			ISKEY_SNUM,				//�����t���l�FInt8�`Int64
			ISKEY_USNUM				//�����Ȃ����l�FUInt8�`UInt64
	};

	public ref class KeyInfo sealed 
	{
	public :
		KeyInfo();												//�R���X�g���N�^
		KeyInfo(		String^		inKeyName,
						Int32		inKeyPos,
						Int32		inKeyLen,
						keyType_EN	inKeyType,
						Byte		inKeyOrder,
						Byte		inKeyUniqueFlg,
						Int32		inSrtKeyPos,
						Int32		inSrtKeyLen,
						keyType_EN	inSrtKeyType,
						Byte		inSrtKeyOrder
				);
		KeyInfo( KeyInfo^ source );								//�R�s�[�R���X�g���N�^
		~KeyInfo();												//�f�X�g���N�^

		//--�L�[���擾--//
		String^		getKeyName(String^ otStr );					//�L�[���擾
		Int32		getKeyLen( void );							//�L�[���擾
		Int32		getKeyPos( void );							//�L�[�ʒu�擾
		keyType_EN	getKeyType( void );							//�L�[�^�C�v�擾
		Byte		getKeyOrder( void );						//�L�[����
		Byte		getKeyUniqueFlg( void );					//���j�[�N�L�[�t���O�擾
		Int32		getSrtKeyLen( void );						//�\�[�g�L�[���擾
		Int32		getSrtKeyPos( void );						//�\�[�g�L�[�ʒu�擾
		keyType_EN	getSrtKeyType( void );						//�\�[�g�L�[�^�C�v�擾
		Byte		getSrtKeyOrder( void );						//�\�[�g�L�[����
	
		//--�L�[���ݒ�--//
		Int32		Set(										//�L�[���ݒ�
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
						);

		//-- �t�@�C�����o�͗p�f�[�^�A�N�Z�X --//
		static Int32 Length( void );								//�t�@�C�����o�͗p�f�[�^���擾
		Int32		Read( array<Byte>^ inBuff, Int32 offset );		//�L�[�f�[�^�o��
		Int32		Write( array<Byte>^ otBuff, Int32 offset );		//�L�[�f�[�^����

	private :
		//-- �����o --//
		String^			KeyName;								//�L�[��
		Int32			KeyPos;									//�L�[�ʒu
		Int32			KeyLen;									//�L�[��
		keyType_EN		KeyType;								//�L�[�`��
		Byte			KeyOrder;								//�L�[����
		Byte			KeyUniqueFlg;							//���j�[�N�L�[�t���O
		Int32			SrtKeyPos;								//�\�[�g�L�[�ʒu
		Int32			SrtKeyLen;								//�\�[�g�L�[��
		keyType_EN		SrtKeyType;								//�\�[�g�L�[�`��
		Byte			SrtKeyOrder;							//�L�[����
	};
}