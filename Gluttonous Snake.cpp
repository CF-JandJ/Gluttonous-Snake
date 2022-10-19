#include <iostream>
#include <graphics.h>		// 引用图形库头文件
#include <vector>
#include <conio.h>

using namespace std;

constexpr int width = 640;
constexpr int height = 480;

enum class DIR {
	left,
	right,
	top,
	bottom
};

//碰撞检测             小矩形 ， 大矩形 
bool RectDuangRect(RECT& r1, RECT& r2)
{
	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;

	return (r.left <= r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
}


class Snake {
public:
	Snake(IMAGE& imag) :imag_snake(imag),score(0), dir(DIR::right) {
		rect_snake.left   = 0;
		rect_snake.top    = 0;
		rect_snake.right  = imag.getwidth();
		rect_snake.bottom = imag.getheight();
	}
	void Control() {
		ExMessage mess;
		peekmessage(&mess, EX_KEY);
		if      (mess.vkcode == VK_LEFT)  dir = DIR::left;
		else if (mess.vkcode == VK_UP)    dir = DIR::top;
		else if (mess.vkcode == VK_RIGHT) dir = DIR::right;
		else if (mess.vkcode == VK_DOWN)  dir = DIR::bottom;
	}
	bool Show() {
		if (rect_snake.right > width || rect_snake.left < 0 || rect_snake.top < 0 || rect_snake.bottom > height)return false;
		if      (dir == DIR::left) {
			rect_snake.left  -= 4;
			rect_snake.right -= 4;
		}
		else if (dir == DIR::right) {
			rect_snake.left  += 4;
			rect_snake.right += 4;
		}
		else if (dir == DIR::top) {
			rect_snake.top    -= 4;
			rect_snake.bottom -= 4;
		}
		else if (dir == DIR::bottom) {
			rect_snake.top    += 4;
			rect_snake.bottom += 4;
		}
		putimage(rect_snake.left, rect_snake.top, &imag_snake);
		return true;
	}
	RECT& GetRect() { return rect_snake; }
	int AddScore() { return ++score; }
	int GetScore() { return score; }
	DIR GetDir() { return dir; }
protected:
	IMAGE& imag_snake;//图形
	RECT rect_snake;

	int score;//分数
	DIR dir;  //方向
};

class Body{
public:
	Body(IMAGE& imag, Snake& s, vector<Body*>& bd) : imag_body(imag), local(bd.size() + 1){
		rect_body.left   = 0;
		rect_body.right  = 0;
		rect_body.top    = 0;
		rect_body.bottom = 0;
	}
	RECT& GetRect() { return rect_body; }
private:
	IMAGE& imag_body;
	RECT   rect_body;
	int    local;
};

bool Show(Snake& s, vector<Body*>& bd, IMAGE& imag) {
	int i = bd.size();
	if (i == 0)return true;

	for (int j = i - 1; j > 0; j--) {
		bd[j]->GetRect().left = bd[j - 1]->GetRect().left;
		bd[j]->GetRect().right = bd[j - 1]->GetRect().right;
		bd[j]->GetRect().top = bd[j - 1]->GetRect().top;
		bd[j]->GetRect().bottom = bd[j - 1]->GetRect().bottom;
	}

	if      (s.GetDir() == DIR::left) {
		bd[0]->GetRect().left   = s.GetRect().right;
		bd[0]->GetRect().right  = bd[0]->GetRect().left + imag.getwidth();
		bd[0]->GetRect().top    = s.GetRect().top;
		bd[0]->GetRect().bottom = bd[0]->GetRect().top + imag.getheight();
	}
	else if (s.GetDir() == DIR::right) {
		bd[0]->GetRect().left   = s.GetRect().left - imag.getwidth();
		bd[0]->GetRect().right  = s.GetRect().left;
		bd[0]->GetRect().top    = s.GetRect().top;
		bd[0]->GetRect().bottom = bd[0]->GetRect().top + imag.getheight();
	}
	else if (s.GetDir() == DIR::top) {
		bd[0]->GetRect().left   = s.GetRect().left;
		bd[0]->GetRect().right  = bd[0]->GetRect().left + imag.getwidth();
		bd[0]->GetRect().top    = s.GetRect().bottom;
		bd[0]->GetRect().bottom = bd[0]->GetRect().top + imag.getheight();
	}
	else if (s.GetDir() == DIR::bottom) {
		bd[0]->GetRect().left   = s.GetRect().left;
		bd[0]->GetRect().right  = bd[0]->GetRect().left + imag.getwidth();
		bd[0]->GetRect().top    = s.GetRect().top - imag.getheight();
		bd[0]->GetRect().bottom = s.GetRect().top;
	}

	for (auto& c : bd) {
		putimage(c->GetRect().left, c->GetRect().top, &imag);
	}
	return true;
}

class Food {
public:
	Food(IMAGE& imag) :imag(imag) {
		srand(time(0));
		rect.left   = rand() % (width - imag.getwidth());
		rect.top    = rand() % (height - imag.getheight());
		rect.right  = rect.left + imag.getwidth();
		rect.bottom = rect.top + imag.getheight();
	}
	bool Show() {
		putimage(rect.left, rect.top, &imag);
		return true;
	}
	RECT& GetRect() { return rect; }
private:
	IMAGE& imag;
	RECT rect;
};

void GameDraw() {
	setbkcolor(WHITE);
	cleardevice();

	IMAGE bk;
	loadimage(&bk, _T("images/OIP-C.jpg"));
	putimage(0, 0, &bk);
}

//结算界面
bool Over(int score) {
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str, 128, _T("分数：%llu"), score);

	settextcolor(RED);
	outtextxy(width / 2 - textwidth(str) / 2, height / 5, str);


	//键盘返回 Enter
	LPCTSTR info1 = _T("按Enter 继续");
	settextstyle(20, 0, _T("黑体"));
	outtextxy(width - textwidth(info1), height - textheight(info1), info1);
	LPCTSTR info2 = _T("按Exc 退出");
	settextstyle(20, 0, _T("黑体"));
	outtextxy(width - textwidth(info1) - textwidth(info2), height - textheight(info2), info2);

	while (true)
	{
		ExMessage mess;
		getmessage(&mess, EX_KEY);
		if (mess.vkcode == VK_RETURN) {
			return true;
		}
		else if (mess.vkcode == VK_ESCAPE) {
			return false;
		}
	}
}

bool Play() {
	
	IMAGE snake, body, food;
	loadimage(&snake, _T("images/right.png"));
	loadimage(&body , _T("images/body.png"));
	loadimage(&food, _T("images/food.png"));

	Snake sk(snake);
	Food* fd = new Food(food);
	vector<Body*>bd;

	//限制帧数
	int bsing = 0;

	bool is_play = true;
	
	GameDraw();

	while (is_play)
	{
		//防止屏闪，批量绘图
		BeginBatchDraw();

		Sleep(20);
		GameDraw();
		flushmessage();
		Sleep(20);

		fd->Show();
		sk.Control();
		if (!sk.Show()) is_play = false;

		if (RectDuangRect(fd->GetRect(), sk.GetRect())) {
			sk.AddScore();
			bd.push_back(new Body(body, sk, bd));
			delete fd;
			fd = new Food(food);
		}

		Show(sk, bd, body);

		//结束时关闭批量绘图
		EndBatchDraw();
	}

	return Over(sk.GetScore());
}

int main() {
	initgraph(width, height);	// 创建绘图窗口，大小为 640x480 像素

	bool is_play = true;

	while (is_play)
	{
		is_play = Play();
	}
	
	system("pause");
	closegraph();			// 关闭绘图窗口
	return 0;
}