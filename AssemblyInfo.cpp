#include "stdafx.h"

using namespace System::Reflection;
using namespace System::Runtime::CompilerServices;
using namespace System::Runtime::InteropServices;

//
// アセンブリに関する一般情報は以下の属性セットをとおして制御されます。
// アセンブリに関連付けられている情報を変更するには、
// これらの属性値を変更してください。
//
[assembly:AssemblyTitleAttribute("ISAMライブラリ")];
[assembly:AssemblyDescriptionAttribute("藤井 元雄")];
[assembly:AssemblyConfigurationAttribute("")];
[assembly:AssemblyCompanyAttribute("")];
[assembly:AssemblyProductAttribute("FJIsam")];
[assembly:AssemblyCopyrightAttribute("Copyright (C) 藤井 元雄 2005")];
[assembly:AssemblyTrademarkAttribute("")];
[assembly:AssemblyCultureAttribute("")];

//
// アセンブリのバージョン情報は、以下の 4 つの値で構成されています:
//
//      Major Version
//      Minor Version
//      Build Number
//      Revision
//
// すべての値を指定するか、下のように '*' を使ってリビジョンおよびビルド番号を
// 既定値にすることができます:

[assembly:AssemblyVersionAttribute("1.2.1.0")];

//
// アセンブリに署名するには、使用するキーを指定しなければなりません。アセンブリ署名に関する
// 詳細については、Microsoft CLR Framework ドキュメントを参照してください。
//
// 下の属性を使用して、署名に使うキーを制御することができます。
// 署名には、共通プロパティのプロジェクト プロパティ ページを使用して、
// 同じ情報を設定することもできます。
//
// メモ:
//   (*) キーが指定されないと、アセンブリは署名されません。
//   (*) KeyName は、コンピュータにインストールされている暗号サービス プロバイダ (CSP) を
//       表します。KeyFile は、キーを含む
//       ファイルです。
//   (*) KeyFile および KeyName の値が共に指定されている場合、
//       以下の処理が発生します:
//       (1) KeyName が CSP に見つかった場合、そのキーが使われます。
//       (2) KeyName が存在せず、KeyFile が存在する場合、
//           KeyFile にあるキーが CSP にインストールされ、使われます。
//   (*) KeyFile を作成するには、sn.exe (Strong Name) ユーティリティを使ってください。
//        KeyFile を指定するとき、KeyFile の場所は、
//        プロジェクト出力 ディレクトリへの相対パスでなければなりません。
//   (*) 遅延署名は高度なオプションです。詳細については Microsoft CLR Framework
//       ドキュメントを参照してください。
//
//[assembly:AssemblyDelaySignAttribute(false)];
//[assembly:AssemblyKeyFileAttribute("")];
//[assembly:AssemblyKeyNameAttribute("")];

[assembly:ComVisible(false)];

