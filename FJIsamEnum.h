//------------------------------------------------------------------------------
//	ISAM�A�N�Z�X���C�u�����@����enum�@�w�b�_
//	Version 1.2		Create Date 2006/03/01 �V�K�쐬
//	Copyright		���䌳�Y
//------------------------------------------------------------------------------
//	Classes
//		DataRecStat_EN	:���R�[�h����
//------------------------------------------------------------------------------
#pragma once

namespace FJIsam
{
	//-- ���R�[�h����
	public enum class DataRecStat_EN
	{
			REC_NEW = 0,			//�V�K�쐬(�t�@�C���ɂ͂Ȃ�)
			REC_OLD,				//����(�t�@�C������Ǎ�)
			REC_DEL					//�폜�i�\��j
	};

	//-- �t�@�C���I�[�v�����[�h
	public enum class IsamOpenMode
	{
		READ_WRITE = 0,				//�ǂݏ����\
		READ_ONLY					//�Ǎ���p(���L�\)
	};
}