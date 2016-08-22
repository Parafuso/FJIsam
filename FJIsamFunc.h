//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@���ʊ֐��@�w�b�_
//	Version 1.2		Create Date 2006/03/01 �V�K�쐬
//  Version 1.2.1	Change Date 2011/07/08 KeyStringConv �C���i'��󔒂𖳎�����)
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		KeyCompare	:�L�[��r
//------------------------------------------------------------------------------
#pragma once

#include "FJIsam.h"					//ISAM�N���X

namespace FJIsam
{

	//�L�[�����I�v�V����
	public enum class KeyCmpOpt_EN
	{
		Key = 0,					//�L�[�̂ݔ�r
		Srt,						//�\�[�g�L�[�̂ݔ�r
		All							//�L�[���\�[�g�L�[�̏��Ŕ�r
	};

	public ref class FJIFuncs
	{
	public :
	//-- �֐� --//
	static Int32 KeyCompare( array<Byte>^ inKey, array<Byte>^ inData,						//�L�[��r
						KeyInfo^ inKeyInfo, Int32 inIdxNum, KeyCmpOpt_EN Opt );

	static String^ ArrayToString( array<Byte>^ inData, Int32 inOffset, Int32 inLen );		//String(Chars)�ɕϊ�
	static SByte ArrayToSByte( array<Byte>^ inData, Int32 inOffset );						//SByte�ɕϊ�
	static Int16 ArrayToInt16( array<Byte>^ inData, Int32 inOffset );						//Int16�ɕϊ�
	static UInt16 ArrayToUInt16( array<Byte>^ inData, Int32 inOffset );					//UInt16�ɕϊ�
	static Int32 ArrayToInt32( array<Byte>^ inData, Int32 inOffset );						//Int32�ɕϊ�
	static UInt32 ArrayToUInt32( array<Byte>^ inData, Int32 inOffset );					//UInt32�ɕϊ�
	static Int64 ArrayToInt64( array<Byte>^ inData, Int32 inOffset );						//Int64�ɕϊ�
	static UInt64 ArrayToUInt64( array<Byte>^ inData, Int32 inOffset );					//UInt64�ɕϊ�

	static array<Byte>^ StringToArray( String^ inSrc, array<Byte>^ inDest,					//String(Chars)����ϊ�
								Int32 inOffset, Int32 inLen );						
	static array<Byte>^ SByteToArray( SByte inSrc, array<Byte>^ inDest,					//SByte����ϊ�
								Int32 inOffset );
	static array<Byte>^ Int16ToArray( Int16 inSrc, array<Byte>^ inDest,					//Int16����ϊ�
								Int32 inOffset );
	static array<Byte>^ UInt16ToArray( UInt16 inSrc, array<Byte>^ inDest,					//UInt16����ϊ�
								Int32 inOffset );
	static array<Byte>^ Int32ToArray( Int32 inSrc, array<Byte>^ inDest,						//Int32����ϊ�
								Int32 inOffset );
	static array<Byte>^ UInt32ToArray( UInt32 inSrc, array<Byte>^ inDest,					//UInt32����ϊ�
								Int32 inOffset );
	static array<Byte>^ Int64ToArray( Int64 inSrc, array<Byte>^ inDest,						//Int64����ϊ�
								Int32 inOffset );
	static array<Byte>^ UInt64ToArray( UInt64 inSrc, array<Byte>^ inDest,					//UInt64����ϊ�
								Int32 inOffset );

	static String^ KeyStringConv( String^ inStr );											//��r�p�̕�����ɕԊ�

	};
}