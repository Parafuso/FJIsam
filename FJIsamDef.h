// FJIsamDef.h
///------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@��{�N���X�@��`�t�@�C��
//	Version 1.0		create date 2002/10/08
//	Version	1.1		create date 2004/02/22
//	Version 1.2		Create Date 2006/03/01 .net 2.0 �ɍ��ς�
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
#pragma once

namespace FJIsam
{

	//���R�[�h�폜�t���O(ISAM�A�T�u�C���f�b�N�X����)
	static const Byte IS_RECNOR = '\0';				//���R�[�h�L��
	static const Byte IS_RECDEL = '\xff';			//���R�[�h�폜�ς�

	//���R�[�h����L�[�t���O
	static const Byte IS_NOEACHKEY = 0;				//����L�[�Ȃ�
	static const Byte IS_EACHKEYFST = 1;			//����L�[�擪
	static const Byte IS_EACHKEYMID = 2;			//����L�[�O�゠��
	static const Byte IS_EACHKEYLST = 3;			//����L�[�Ō�

	//���j�[�N�L�[�t���O
	static const Byte IS_UNIQUEKEY = '\0';			//���j�[�N�L�[�̂݋���
	static const Byte IS_DUPLICATEKEY = '\xff';		//�d���L�[����

	//�f�[�^��
	static const Int32 IS_FIDLEN = 16;				//�t�@�C��ID
	static const Int32 IS_VERLEN = 16;				//�t�@�C���쐬ISAM���C�u�����o�[�W����
	static const Int32 IS_UINFLEN = 256;			//���[�U�[���

	static const Int32 IS_KEYNAMELEN = 20;			//�L�[����

	//�g���q
	#define IS_EXTSTR  L".fis"						//ISAM�g���q

}