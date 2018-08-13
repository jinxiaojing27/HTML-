#include<windows.h>
#include<stdio.h>
#include <Shlwapi.h>
#include<time.h>
int background[20][10] = {0};//背景数组
int square[4][4] = {0};
int whichsquare =0;
long int score = 0;
int Xmark = 0;//记录方块位置（行）
int Ymark = 3;//记录方块位置（列）

#define down  1
#define left  0
#define right 2
bool CheckGameover(void);//游戏结束
void CopyTop(void);//接触顶部
void AccountTime(HWND hwnd);//定时器
void OnCreate(void);//初始化
void ProduceSquare(void);//设定不同方块
void EverySquare(int n);//每个方块儿代码
void CopySquare(void);//方块放在背景上
void DrawSqare(HDC indc);//画方块儿
void OnPaint(HDC hdc);//paint函数
void SquareDown(void);//方块自动下落
void OnTimer(HWND hwnd);//定时器响应
bool CheckBottom(void);//检查是否停在最后一行
void ChangeNumber(void);//将最后一行1改成2，保证方块儿下落
bool CheckCrash(int check);//检测方块儿是否落在下面的方块儿上
void LeftMove(void);//左移
void RightMove(void);//右移
bool CheckBother(int j);//方块触碰边界判定
bool CheckChangeSquare(int i);//检查普通方块能否变形
bool CheckBarChange(int number);//检查条状方块能否变形
void ChooseSquare(int number);//确定变形后的方块儿
void ChangeShape(void);//方块儿变形实现
void ClearFull(void);//清除行
bool CheckTop(void);//检测是否需要生成新块
LRESULT CALLBACK ROSE(HWND hwnd, UINT NMsg, WPARAM wparam, WPARAM Iparam);//回调函数

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hinprestance, LPSTR lpCmdLine, int nCmdshow)
//LPSTR 必须用这个 否则无法重载函数
//SW_SHOWNORMAL 表示窗口正常显示 1
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG news;//消息
	
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);//系统光标(NULL,IDC_HELP),自定义光标，资源文件——添加——资源——光标
											//找资源ID，用MAKEINTRESOURCE转换，添加头文件。	
	wc.hIcon =0;//状态栏图标,如果hIconSM为空，则左上角图标和状态栏图标相同，同上，右键——属性，修改ID

	wc.hIconSm = NULL;//左上角图标，同上
	wc.hInstance = hinstance;
	wc.lpfnWndProc = (WNDPROC)ROSE; //返回函数地址,函数名
	wc.lpszClassName = ("windows");
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	
	if(0==RegisterClassEx(&wc))
	{
		printf("注册失败");
		int a = GetLastError();//返回错误码，可以查找错误原因，工具——错误查找
		return 0;

	}//注册窗口对象，返回值为零，则注册失败，先判断注册是否成功
	
	 
	 //创建窗口
	hwnd = CreateWindowEx(WS_EX_TOPMOST,"windows","俄罗斯方块",WS_OVERLAPPEDWINDOW,100,100,500,700,NULL,NULL,hinstance,NULL);//窗口句柄，窗口唯一标识,WS_EX_TOPMOST窗口在最前端
	//窗口位置，窗口类名称，窗口名称，窗口样式，窗口位置横纵坐标，窗口宽高，小窗口，当前句柄，多文档，
	
	if (NULL == hwnd)
	{
		printf("失败");
	}
	
	
	ShowWindow(hwnd,SW_SHOWNORMAL);//传递窗口，窗口显示方式
	UpdateWindow(hwnd);//刷新窗口API
	

	while(GetMessage(&news, NULL, 0, 0))
	
	{
		TranslateMessage(&news);
		//负责将按键盘转化成字符消息
		DispatchMessage(&news);
		//分发消息，对消息分类处理
	}
	//GetMessage得到消息，第一个参数是键盘输入结构体，第二个参数表示可以处理的窗口个数，NULL表示全部
	//GetMessage第三四个表示可以取到的键盘输入范围，0,0表示全部
	
	return 0;
}

LRESULT CALLBACK ROSE(HWND hwnd, UINT uMsg, WPARAM wParam, WPARAM Iparam)//回调函数 //UINT NMsg 消息ID
{
	PAINTSTRUCT pt;
	HDC hdc;//可操作区域标识

	switch (uMsg)
	{
	case WM_CREATE://窗口创建初期只产生一次
		OnCreate();
	//数据初始化
		break;

	case WM_TIMER:
		OnTimer(hwnd);
		break;
		//定时器
	
	
	case WM_PAINT://产生变化的消息
		hdc = BeginPaint(hwnd,&pt);//一参窗口句柄，二参结构体指针,申请
		//由此之后是正式内容
		OnPaint(hdc);
		

		
		EndPaint(hwnd,&pt);//释放
		break;


	case WM_KEYDOWN://按键
	
		switch (wParam)
		{
		case VK_RETURN://回车
			AccountTime(hwnd);
			break;
		case VK_LEFT:
			if (CheckBother(0) && (!CheckCrash(left)))
			{
				LeftMove();
				Ymark--;
				hdc = BeginPaint(hwnd, &pt);
				OnPaint(hdc);
				EndPaint(hwnd, &pt);
			}
			break;
		case VK_RIGHT:
			if (CheckBother(9) && (!CheckCrash(right)))
			{
				RightMove();
				Ymark++;
				hdc = BeginPaint(hwnd, &pt);
				OnPaint(hdc);
				EndPaint(hwnd, &pt);
			}
			break;
		case VK_UP:
			if (CheckBottom())
			{
				ChangeShape();
				hdc = BeginPaint(hwnd, &pt);
				OnPaint(hdc);
				EndPaint(hwnd, &pt);
			}
			break;
		case VK_DOWN:
			if ((CheckBottom()) && (!(CheckCrash(down))))
			{
				
				SquareDown();
				Xmark++;
				hdc = BeginPaint(hwnd, &pt);
				OnPaint(hdc);
				EndPaint(hwnd, &pt);
			}

			break;
		}
		

		break;



	case WM_DESTROY://关闭窗口是系统发送的消息
		KillTimer(hwnd, 1123);
		PostQuitMessage(0);//发送退出消息GetMessage收到消息后将return 0,主函数退出消息循环
		break;
	
	default:
		return DefWindowProc(hwnd, uMsg, wParam, Iparam);//不处理的消息交给系统处理。
	}
	return 0;
}

void OnPaint(HDC hdc)
{
	char showscore[10] = {0};
	HDC indc = CreateCompatibleDC(hdc);//创建兼容性DC
	HBITMAP inmap = CreateCompatibleBitmap(hdc, 500, 700);//创建位图,一参可操作区域标识，二三参宽高
	SelectObject(indc, inmap);//将兼容性DC与位图相关联

	DrawSqare(indc);//画方块儿
	Rectangle(indc,300,0,500,600);
	_itoa_s(score, showscore, 10);//一参数字，二参字符地址，三参进制

	TextOut(indc, 400, 100, showscore, strlen(showscore));//一参DC，二参三参横纵坐标，四参字符地址，五参长度
	
	BitBlt(hdc, 0, 0, 500, 600, indc, 0, 0, SRCCOPY);//一参目标DC，二参三参起始位置坐标,四参五参终了坐标，六参兼容DC，七八参兼容起始位置，九参传递方式

	DeleteDC(indc);//释放DC
	DeleteObject(inmap);//释放位图


}

void OnTimer(HWND hwnd)
{
	HDC hdc = GetDC(hwnd);

	ClearFull();
	if (CheckGameover())
	{
		KillTimer(hwnd, 1123);
		MessageBox(NULL, "游戏结束", "提示", MB_OK);
		

		//一参窗口句柄，可为空，二参内容，三参标题，四参风格

	}

	if ((CheckBottom()) && (!(CheckCrash(down))))
	{
		SquareDown();
		Xmark++;
	}
	else 
	{
		ChangeNumber();
		ProduceSquare();
		if (CheckTop())
		{
			CopySquare();
			Xmark = 0;
			Ymark = 3;
		}
		else
		{
			CopyTop();
		}
	}

	OnPaint(hdc);
	ReleaseDC(hwnd, hdc);

}
void OnCreate(void)
{
	srand((unsigned int)time(NULL));
	ProduceSquare();
	CopySquare();

}

void ProduceSquare(void)
{
	int n = rand() % 19;
	EverySquare(n);
	

	whichsquare = n;
}
void CopySquare(void)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if(background[i][j + 3]!=2)
			background[i][j + 3]= square[i][j]; 


		}

	}


}

void EverySquare(int n)
{
	switch (n)
	{
	case 0:
		square[0][0] = 1; square[0][1] = 1; square[0][2] = 1; square[0][3] = 1;
		square[1][0] = 0; square[1][1] = 0; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 1:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 1; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 1; square[3][2] = 0; square[3][3] = 0;
		break;
	case 2:
		square[0][0] = 1; square[0][1] = 1; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 3:
		square[0][0] = 0; square[0][1] = 0; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 1; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 4:
		square[0][0] = 1; square[0][1] = 0; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 1; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 5:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 1; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 6:
		square[0][0] = 1; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 0; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 7:
		square[0][0] = 0; square[0][1] = 0; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 0; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 1; square[2][2] = 1; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 8:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 9:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 1; square[1][1] = 1; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 10:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 1; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 11:
		square[0][0] = 0; square[0][1] = 0; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 1; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 12:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 1; square[2][2] = 1; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 13:
		square[0][0] = 1; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 1; square[1][1] = 0; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 14:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 0; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 1; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 15:
		square[0][0] = 0; square[0][1] = 0; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 1; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 16:
		square[0][0] = 1; square[0][1] = 1; square[0][2] = 1; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 0; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 17:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 0; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 1; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	case 18:
		square[0][0] = 0; square[0][1] = 1; square[0][2] = 0; square[0][3] = 0;
		square[1][0] = 1; square[1][1] = 1; square[1][2] = 1; square[1][3] = 0;
		square[2][0] = 0; square[2][1] = 0; square[2][2] = 0; square[2][3] = 0;
		square[3][0] = 0; square[3][1] = 0; square[3][2] = 0; square[3][3] = 0;
		break;
	}


	whichsquare = n;

}


void DrawSqare(HDC indc)
{
	Rectangle(indc, 0, 0, 300, 600);//画大方块儿，一参窗口可执行区域句柄(的兼容性DC)，之后是起始点的终止点的横纵坐标（先横后纵）
	int i, j;
	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (0 != background[i][j])
			{
				//画方块儿
				if (2 == background[i][j])
				{
					HGDIOBJ newbrush,oldbrush;
					newbrush = CreateSolidBrush(RGB(87, 199, 219));
					oldbrush = SelectObject(indc, newbrush);
					Rectangle(indc, j * 30, i * 30, j * 30 + 30, i * 30 + 30);
					newbrush = SelectObject(indc, oldbrush);
					DeleteObject(newbrush);
				}
				
				else
				Rectangle(indc, j*30,i*30 ,j*30+30 ,i*30+30 );
			}

		}
	}

}

void AccountTime(HWND hwnd)
{
	//打开定时器,产生定时器消息，一参窗口句柄，二参定时器ID，三参间隔时差，四参相关函数
	SetTimer(hwnd, 1123, 500, NULL);


}

void SquareDown(void)
{
	int i, j;
	for (i = 19; i >= 0; i--)
	{
		for (j = 9; j >= 0; j--)
		{

			if (1 == background[i][j])
			{
				background[i + 1][j] = background[i][j];
				background[i][j] = 0;

			}
		}

	}


}



bool CheckBottom(void)
{
	int i = 0;
	for (i = 0; i < 10; i++)
	{
		if (1== background[19][i])
		{
			return false;

		}


	}

	return true;

}

void ChangeNumber()
{
	int i, j;
	for(i=0;i<20;i++)
		for (j = 0; j < 10; j++)
		{
			if (background[i][j] == 1 )
			{
				
					background[i][j] = 2;
			}
		}


}

bool CheckCrash(int check)
{
	int i, j;
	switch (check)
	{
	case down:
	{
		for (i = 0; i < 20; i++)
		{
			for (j = 0; j < 10; j++)
			{
				if (background[i][j] == 1 && background[i + 1][j] == 2)
					return true;
			}

		}
	}
	break;

	case left :
	{
		for (i = 0; i < 20; i++)
		{
			for (j = 0; j < 10; j++)
			{
				if (background[i][j] == 1 && background[i ][j-1] == 2)
					return true;
			}

		}
	}
	break;
	case right:
	{
		for (i = 0; i < 20; i++)
		{
			for (j = 0; j < 10; j++)
			{
				if (background[i][j] == 1 && background[i ][j+1] == 2)
					return true;
			}

		}
	}
	break;

	}
		return false;

}

void LeftMove(void)
{
	int i, j;

	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (background[i][j] == 1)
			{


				background[i][j - 1] = background[i][j] ;
					background[i][j] = 0;
			}

		}
	}


}
void RightMove(void)
{
	int i, j,t;

	
	for (i = 0; i < 20; i++)
	{
		for (j = 9; j >=0; j--)
		{
			if (background[i][j] == 1)
			{
				
				background[i][j + 1] = background[i][j] ;
				background[i][j] = 0;
			}

		}
	}



}

bool CheckBother(int j)
{
	int i ;
	for (i = 0; i < 20; i++)
	{

		if (background[i][j] == 1)
			return false;
	}

	return true;
}

void ChangeShape(void)
{
	int i,j;
	switch (whichsquare)
	{
	case 0:
		if(CheckBarChange(0))
			ChooseSquare(1);
		break;
	case 1:
		if (CheckBarChange(1)&& CheckChangeSquare(left)&&Ymark<7)
			ChooseSquare(0);
		break;
	case 2:
		if(CheckChangeSquare(down))
		ChooseSquare(3);
		break;
	case 3:
		if (CheckChangeSquare(left))
		ChooseSquare(2);
		break;
	case 4:
		if (CheckChangeSquare(down))
		ChooseSquare(5);
		break;
	case 5:
		if (CheckChangeSquare(left))
		ChooseSquare(6);
		break;
	case 6:
		if (CheckChangeSquare(down))
		ChooseSquare(7);
		break;
	case 7:
		if (CheckChangeSquare(left))
		ChooseSquare(4);
		break;
	case 8:
		if (CheckChangeSquare(left))
		ChooseSquare(8);
		break;
	case 9:
		if (CheckChangeSquare(down))
		ChooseSquare(10);
		break;
	case 10:
		if (CheckChangeSquare(left))
		ChooseSquare(9);
		break;
	case 11:
		if (CheckChangeSquare(down))
		ChooseSquare(12);
		break;
	case 12:
		if (CheckChangeSquare(left))
		ChooseSquare(13);
		break;
	case 13:
		if (CheckChangeSquare(down))
		ChooseSquare(14);
		break;
	case 14:
		if (CheckChangeSquare(left))
		ChooseSquare(11);
		break;
	case 15:
		if (CheckChangeSquare(left))
		ChooseSquare(16);
		break;
	case 16:
		if (CheckChangeSquare(down))
		ChooseSquare(17);
		break;
	case 17:
		if (CheckChangeSquare(left))
		ChooseSquare(18);
		break;
	case 18:
		if (CheckChangeSquare(down))
		ChooseSquare(15);
		break;

	}


}

void ChooseSquare(int number)
{
	EverySquare(number);

	int i, j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)

		{
			if(background[Xmark + i][Ymark + j ]!=2)
			background[Xmark+i][Ymark+j] = square[i][j];


		}

	}


}

bool CheckChangeSquare(int number)
{
	int i, j;
	switch(number)
	{ 
	case down: 
	{
		if (background[Xmark + 2][Ymark]==0&& background[Xmark + 2][Ymark+1] == 0 && background[Xmark + 2][Ymark+2] == 0 )
		{
			return true;

		}
	}
			   break;

	case left:
	{
		if (background[Xmark ][Ymark] == 0 && background[Xmark + 1][Ymark ] == 0 && background[Xmark + 2][Ymark ] == 0)
		{
			if(Ymark>=0)
			return true;

		}
	}
		break;
	}
	
	return false;
}
bool CheckBarChange(int number)
{
	switch (number)
	{
	case 0:
		if (background[Xmark + 3][Ymark] == 0 && background[Xmark + 3][Ymark + 1] == 0 && background[Xmark + 3][Ymark + 2] == 0 && background[Xmark + 3][Ymark + 3] == 0)
			return true;
			break;
	case 1:
		if (background[Xmark ][Ymark+ 3] == 0 && background[Xmark + 1][Ymark+ 3 ] == 0 && background[Xmark + 2][Ymark+ 3 ] == 0 && background[Xmark + 3][Ymark + 3] == 0)
			return true;
			break;

	}

	return false;
}

void ClearFull(void)
{
	int i, j,total;
	int t, k;
	total = 0;
	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 10; j++)
		{
			if (background[i][j] == 2)
			{
				total++;
			}
			
			
		}
		
		if (total == 10)
		{
			
			for (t = i; t >= 0; t--)
			{
				for (k = 0; k < 10; k++)
				{
					if (background[t - 1][k] != 1 && background[t][k] != 1)
					{
						background[t][k] = background[t - 1][k];
					}
				}
			}

			score++;
		}
			total = 0;
	}
}

bool CheckTop(void)
{
	
	if (background[2][3]==2 || background[2][4] ==2 || background[2][5] ==2 )
	{
		return false;

	}
	return true;
}

void CopyTop(void)
{
	if (!(background[1][3] == 2 || background[1][4] == 2 || background[1][5] == 2))
	{
		background[0][4] = 2;
		background[1][3] = 2;
		background[1][4] = 2;
		background[1][5] = 2;
	}
	else
	{
		
		background[0][3] = 2;
		background[0][4] = 2;
		background[0][5] = 2;
		background[0][6] = 2;

	}
}

bool CheckGameover(void)
{
	int i;
	for (i = 0; i < 10; i++)
	{
		if (background[0][i] == 2)
			return true;

	}

	return false;
}
