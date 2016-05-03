// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 KDUI32_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// KDUI32_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
