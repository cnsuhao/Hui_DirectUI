// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� KDUI32_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// KDUI32_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#include "stringex.h"
#include <deque>
using namespace std;

#include "gdiplus.h" 

using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

#include "..\DirectUICom\DirectUICom.h"
#include "IRenderImage_.h"
#include "IRenderDC_.h"
#include "SkinObjResBase_.h"
#include "SkinPicture_.h"
#include "SkinImageBase_.h"
#include "SkinShareObj_.h"
