#include "stdafx.h"

using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;

//
// �A�Z���u���Ɋւ����ʏ��͈ȉ��̑����Z�b�g���Ƃ����Đ��䂳��܂��B
// �A�Z���u���Ɋ֘A�t�����Ă������ύX����ɂ́A
// �����̑����l��ύX���Ă��������B
//
[assembly:AssemblyTitleAttribute("ISAM���C�u����")];
[assembly:AssemblyDescriptionAttribute("���� ���Y")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("")];
[assembly:AssemblyProductAttribute("FJIsam")];
[assembly:AssemblyCopyrightAttribute("Copyright (C) ���� ���Y 2005")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// �A�Z���u���̃o�[�W�������́A�ȉ��� 4 �̒l�ō\������Ă��܂�:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// ���ׂĂ̒l���w�肷�邩�A���̂悤�� '*' ���g���ă��r�W��������уr���h�ԍ���
// ����l�ɂ��邱�Ƃ��ł��܂�:

[assembly:AssemblyVersionAttribute("1.2.1.0")];

//
// �A�Z���u���ɏ�������ɂ́A�g�p����L�[���w�肵�Ȃ���΂Ȃ�܂���B�A�Z���u�������Ɋւ���
// �ڍׂɂ��ẮAMicrosoft CLR Framework �h�L�������g���Q�Ƃ��Ă��������B
//
// ���̑������g�p���āA�����Ɏg���L�[�𐧌䂷�邱�Ƃ��ł��܂��B
// �����ɂ́A���ʃv���p�e�B�̃v���W�F�N�g �v���p�e�B �y�[�W���g�p���āA
// ��������ݒ肷�邱�Ƃ��ł��܂��B
//
// ����:
//   (*) �L�[���w�肳��Ȃ��ƁA�A�Z���u���͏�������܂���B
//   (*) KeyName �́A�R���s���[�^�ɃC���X�g�[������Ă���Í��T�[�r�X �v���o�C�_ (CSP) ��
//       �\���܂��BKeyFile �́A�L�[���܂�
//       �t�@�C���ł��B
//   (*) KeyFile ����� KeyName �̒l�����Ɏw�肳��Ă���ꍇ�A
//       �ȉ��̏������������܂�:
//       (1) KeyName �� CSP �Ɍ��������ꍇ�A���̃L�[���g���܂��B
//       (2) KeyName �����݂����AKeyFile �����݂���ꍇ�A
//           KeyFile �ɂ���L�[�� CSP �ɃC���X�g�[������A�g���܂��B
//   (*) KeyFile ���쐬����ɂ́Asn.exe (Strong Name) ���[�e�B���e�B���g���Ă��������B
//        KeyFile ���w�肷��Ƃ��AKeyFile �̏ꏊ�́A
//        �v���W�F�N�g�o�� �f�B���N�g���ւ̑��΃p�X�łȂ���΂Ȃ�܂���B
//   (*) �x�������͍��x�ȃI�v�V�����ł��B�ڍׂɂ��Ă� Microsoft CLR Framework
//       �h�L�������g���Q�Ƃ��Ă��������B
//
//[assembly:AssemblyDelaySignAttribute(false)];
//[assembly:AssemblyKeyFileAttribute("")];
//[assembly:AssemblyKeyNameAttribute("")];

[assembly:ComVisible(false)];

