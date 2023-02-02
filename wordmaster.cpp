#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <stdio.h>
#include <Windows.h>
#include<mmsystem.h>
#include<time.h>
#include "dirent.h"
#undef WHITE
#undef BLACK
#define WHITE RGB(245,245,245)
#define BLACK RGB(10,10,10)

struct word
{
	char word[100];
	char mean[100];
};

struct buttonXY
{
	int left;
	int top;
	int right;
	int bottom;
	const char* title;
};
struct WrongTopic
{
	bool choice;
	struct ChoiceQuestionsWrongTopic* ChoiceWrong;
	struct dictationWrongTopic* DictationWrong;
};
struct ChoiceQuestionsWrongTopic
{
	char title[50];
	char choiceWord[4][50];
	int RrightWord;
};
struct dictationWrongTopic
{
	char dictationMean[50];
	char dictationRightWord[50];
	char dictationWrongWord[50];

};

int getLine(const char* name);//获取文件的行数
void fget(FILE* fp, char* words);//一个用于读取文件内容的函数，遇到逗号，换行符，结束符就停止
void getContents(char* name, int line, struct word* words);//获取文件内容
void downloadMusic(const int line, const struct word* words);//下载音乐
void playMusic(const char* Mname);//播放
int findFile(char fileName[][20]);//查找文件夹下的所有文件（有序号）
void findFile_nSerial(char fileName[][20]);//查找文件夹下的所有文件（无序号）
//处理开始界面选择按钮被按下
void hadlePressUp(const int count, const int add, char* numUnit, const char fileName_nSerial[][20]);
void hadlePressDown(const int count, const int add, const  char fileName[][20], const int rehabilitation);
//过渡界面
void transition(const char* words);
//输出正确的单词
void PutRightWord(const int x, const int y, const char* word1, const char* word2);
//设置字体
void easySetTextStyle(const int textHeigh, const int textColor, const char* textFont);
//创建圆角矩形按钮
void createRoundrectButton(const struct buttonXY button, const int font_height, const char* font, const int fillcolor, const int ellipsewidth, const int ellipseheight);
//生成随机数
int Generate_random_numbers(int range1, int range2);
//初始化ButtonXY
void initButtonXY(struct buttonXY *button, int top, int left, int bottom, int right, const char* title);
bool cheekButton(struct buttonXY button, int mouseX, int mouseY);
//选择题模块
struct ChoiceQuestionsWrongTopic* choiceQuestions(struct word* words, int line, int questionNumber, bool Chinese);
//听写题模块
struct dictationWrongTopic* dictation(struct word* words, int line, int questionNumber);
void ShowWrongQuestion(struct WrongTopic Topic[], int line);

void startPage(char* cTitle);//开始页面
void Positive_memory_Page(struct word* words,const int line);//正向记忆
void Reverse_memory_Page(const int index, const int index_max, struct word* words);//逆向记忆
void Intensive_review_Page(const int index, const  int index_max, struct word* words);//强化复习
void Mandatory_testing_Page(struct word* words, const int line);

// 实现文本框控件
class EasyTextBox
{
private:
	int left = 0, top = 0, right = 0, bottom = 0;	// 控件坐标
	char* text = NULL;							// 控件内容
	size_t maxlen = 0;									// 文本框最大内容长度

public:
	void Create(int x1, int y1, int x2, int y2, int max)
	{
		maxlen = max;
		text = new char[maxlen];
		text[0] = 0;
		left = x1, top = y1, right = x2, bottom = y2;

		// 绘制用户界面
		Show();
	}

	~EasyTextBox()
	{
		if (text != NULL)
			delete[] text;
	}

	char* Text()
	{
		return text;
	}

	bool Check(int x, int y)
	{
		return (left <= x && x <= right && top <= y && y <= bottom);
	}

	// 绘制界面
	void Show()
	{
		// 备份环境值
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();
		int oldtextcolor = gettextcolor();

		setlinecolor(LIGHTGRAY);		// 设置画线颜色
		setbkcolor(0xeeeeee);			// 设置背景颜色
		setfillcolor(0xeeeeee);			// 设置填充颜色
		settextcolor(RGB(128,128,128));		//设置字体颜色
		fillrectangle(left, top, right, bottom);
		outtextxy(left + 10, top + 5, text);

		// 恢复环境值
		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);
		settextcolor(oldtextcolor);
	}

	bool OnMessage()
	{
		// 备份环境值
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();
		int oldtextcolor = gettextcolor();

		setlinecolor(BLACK);			// 设置画线颜色
		setbkcolor(WHITE);				// 设置背景颜色
		setfillcolor(WHITE);			// 设置填充颜色
		settextcolor(BLACK);		//设置字体颜色
		settextstyle(30, 0, "微软雅黑", 0, 0, 0, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
		fillrectangle(left, top, right, bottom);
		outtextxy(left + 10, top + 5, text);

		int width = textwidth(text);	// 字符串总宽度
		int counter = 0;				// 光标闪烁计数器
		bool binput = true;				// 是否输入中
		bool enter = false;

		ExMessage msg;
		while (binput)
		{
			while (binput && peekmessage(&msg, EM_MOUSE | EM_CHAR, false))	// 获取消息，但不从消息队列拿出
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					// 如果鼠标点击文本框外面，结束文本输入
					if (msg.x < left || msg.x > right || msg.y < top || msg.y > bottom)
					{
						binput = false;
						break;
					}
				}
				else if (msg.message == WM_CHAR)
				{
					size_t len = strlen(text);
					switch (msg.ch)
					{
					case '\b':				// 用户按退格键，删掉一个字符
						if (len > 0)
						{
							text[len - 1] = 0;
							width = textwidth(text);
							counter = 0;
							clearrectangle(left + 10 + width, top + 1, right - 1, bottom - 1);
						}
						break;
					case '\r':				// 用户按回车键，结束文本输入
					case '\n':
						binput = false;
						enter = true;
						break;
					default:				// 用户按其它键，接受文本输入
						if (len < maxlen - 1)
						{
							text[len++] = msg.ch;
							text[len] = 0;

							clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// 清除画的光标
							width = textwidth(text);				// 重新计算文本框宽度
							counter = 0;
							outtextxy(left + 10, top + 5, text);		// 输出新的字符串
						}
					}
				}
				peekmessage(NULL, EM_MOUSE | EM_CHAR);				// 从消息队列抛弃刚刚处理过的一个消息
			}

			// 绘制光标（光标闪烁周期为 20ms * 32）
			counter = (counter + 1) % 32;
			if (counter < 16)
				line(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);				// 画光标
			else
				clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);		// 擦光标

			// 延时 20ms
			Sleep(20);
		}

		clearrectangle(left + 10 + width + 1, top + 3, left + 10 + width + 1, bottom - 3);	// 擦光标

		// 恢复环境值
		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);
		settextcolor(oldtextcolor);
		Show();
		return enter;
	}
};


class EasyButton
{
private:
	int left = 0, top = 0, right = 0, bottom = 0;	// 控件坐标
	char* text = NULL;							// 控件内容

public:
	void Create(int x1, int y1, int x2, int y2, const char* title)
	{
		text = new char[strlen(title) + 1];
		strcpy_s(text, strlen(title) + 1, title);
		left = x1, top = y1, right = x2, bottom = y2;

		// 绘制用户界面
		Show();
	}

	~EasyButton()
	{
		if (text != NULL)
			delete[] text;
	}

	bool Check(int x, int y)
	{
		return (left <= x && x <= right && top <= y && y <= bottom);
	}

	// 绘制界面
	void Show()
	{
		int oldlinecolor = getlinecolor();
		int oldbkcolor = getbkcolor();
		int oldfillcolor = getfillcolor();
		int oldtextcolor = gettextcolor();

		setbkmode(TRANSPARENT);
		setlinecolor(BLACK);			// 设置画线颜色
		setbkcolor(WHITE);				// 设置背景颜色
		setfillcolor(WHITE);			// 设置填充颜色
		settextcolor(BLACK);
		settextstyle(25, 0, "微软雅黑", 0, 0, 0, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
		fillrectangle(left, top, right, bottom);
		outtextxy(left + (right - left - textwidth(text) + 1) / 2, top + (bottom - top - textheight(text) + 1) / 2, text);

		setlinecolor(oldlinecolor);
		setbkcolor(oldbkcolor);
		setfillcolor(oldfillcolor);
		settextcolor(oldtextcolor);
	}
};


class WrongQuestionBox
{
private:
	struct buttonXY bottommost;
	struct buttonXY Title;

public:
	void CreateWorngQuestionBox(struct ChoiceQuestionsWrongTopic* wrongTopic,int x1,int y1,int x2,int y2)
	{
		struct buttonXY Choice[4];
		bottommost.left = x1;
		bottommost.top = y1;
		bottommost.right = x2;
		bottommost.bottom = y2;
		bottommost.title = NULL;
		
		Title.left = x1 + 10;
		Title.top = y1 + 20;
		Title.right = x2 - 10;
		Title.bottom = y1 + 50;
		Title.title = NULL;
		
		for (unsigned int i = 0; i < 4; i++)
			initButtonXY(&Choice[i], y1 + 60 + 32 * i, x1 + 10, y1 + 90 + 32 * i, x2 - 10,wrongTopic->choiceWord[i]);

		Show(wrongTopic,Choice);
		return;
	}
	void CreateWorngQuestionBox(struct dictationWrongTopic* wrongTopic, int x1, int y1, int x2, int y2)
	{
		struct buttonXY dictation;
		bottommost.left = x1;
		bottommost.top = y1;
		bottommost.right = x2;
		bottommost.bottom = y2;
		bottommost.title = NULL;

		Title.left = x1 + 10;
		Title.top = y1 + 20;
		Title.right = x2 - 10;
		Title.bottom = y1 + 50;
		Title.title = NULL;

		easySetTextStyle(23, BLACK, "微软雅黑");
		initButtonXY(&dictation, y1+ (y2 - (y1 + 50)) / 2+15, x1 + 10+textwidth("输入内容: "), y1 + (y2 - (y1 + 50)) / 2 + 40, x2 - 10, NULL);

		Show(wrongTopic,dictation);
		return;
	}
	void Show(struct dictationWrongTopic* WrongTopic, struct buttonXY dictation)
	{
		struct buttonXY dictation2;
		createRoundrectButton(bottommost, 0, "微软雅黑", RGB(139, 184, 202), 20, 20);
		createRoundrectButton(Title, 0, "微软雅黑", WHITE, 15, 15);
		easySetTextStyle(30, BLACK, "微软雅黑");
		outtextxy(Title.left + 10, Title.top + (Title.bottom - Title.top - textheight(WrongTopic->dictationMean) + 1) / 2, WrongTopic->dictationMean);

		easySetTextStyle(23, BLACK, "微软雅黑");
		outtextxy(dictation.left-textwidth("输入内容: "), dictation.top, "输入内容: ");
		createRoundrectButton(dictation, 0, "微软雅黑", RGB(206, 229, 243), 15, 15);
		outtextxy(dictation.left + 10, dictation.top + (dictation.bottom - dictation.top - textheight(WrongTopic->dictationWrongWord) + 1) / 2, WrongTopic->dictationWrongWord);


		initButtonXY(&dictation2, dictation.bottom+10, dictation.left, dictation.bottom + 35, dictation.right, NULL);
		createRoundrectButton(dictation2, 0, "微软雅黑", RGB(206, 229, 243), 15, 15);
		easySetTextStyle(23, BLACK, "微软雅黑");
		outtextxy(dictation2.left - textwidth("正确答案: "), dictation2.top, "正确答案: ");
		easySetTextStyle(23, RED, "微软雅黑");
		outtextxy(dictation2.left + 10, dictation2.top + (dictation2.bottom - dictation2.top - textheight(WrongTopic->dictationRightWord) + 1) / 2,WrongTopic->dictationRightWord);
	}
	void Show(struct ChoiceQuestionsWrongTopic* WrongTopic, struct buttonXY *Choice)
	{
		BeginBatchDraw();
		createRoundrectButton(bottommost, 0, "微软雅黑", RGB(139, 184, 202), 20, 20);
		createRoundrectButton(Title, 0, "微软雅黑", WHITE, 15, 15);
		easySetTextStyle(30, BLACK, "微软雅黑");
		outtextxy(Title.left + 10, Title.top + (Title.bottom - Title.top - textheight(WrongTopic->title) + 1) / 2,  WrongTopic->title);
		for (unsigned int i = 0; i < 4; i++)
			createRoundrectButton(Choice[i], 20, "微软雅黑", RGB(206, 229, 243), 15, 15);
		EndBatchDraw();
	}
};
	// 定义控件
	EasyTextBox txtName;
	EasyTextBox txtPwd;
	EasyButton btnOK;
	WrongQuestionBox hello;
void CreateSRK(int pwdx1, int pwdy1, int pwdx2, int pwdy2, int textmax,int btnwidth, const char* title, char* text)
{

	txtPwd.Create(pwdx1, pwdy1, pwdx2, pwdy2, textmax);						// 创建密码文本框控件
	btnOK.Create(pwdx2, pwdy1, pwdx2+textwidth(title)+btnwidth, pwdy2, title);	// 创建按钮控件

}

static  const int HIGH = 700;
static  const int WIDTH = 1024;

int main()
{
	initgraph(1024, 700);
	setbkcolor(WHITE);//设置背景颜色
	cleardevice();//清屏

	setbkcolor(RGB(0, 50, 50));//设置背景颜色
	cleardevice();

	char	cNumUnit[20] = "Unit1";
	//startPage(cNumUnit);

	int line = 0;
	line = getLine(cNumUnit);
	static struct word* words = (struct word*)malloc(sizeof(struct word) * line);//等效替代了这句：struct word words[line];
	getContents(cNumUnit, line, words);
	downloadMusic(line, words);
	Mandatory_testing_Page(words, line);
	//Positive_memory_Page(words,line);
	getchar();
	closegraph();
	return 0;
}

void startPage(char* Title)
{
		char fileName[20][20] = { 0 };
		char fileName_nSerial[20][20] = { 0 };//没有序号的文件名
		findFile_nSerial(fileName_nSerial);
		int numFile=findFile(fileName);
		int count = 0;//代表每页第一个按钮所对应数组的值
		int numPress = -1;
		char numUnit[20] = { 0 };
		setbkcolor(RGB(245, 245, 245));//设置背景颜色
		cleardevice();
		BeginBatchDraw();//避免闪屏
		struct buttonXY startButton = { 400, 550, 624, 600, "开始学习" };
		struct buttonXY selectBookButton[6];
		{
			setfillcolor(RGB(0, 140, 140));
			easySetTextStyle(30, BLACK, "微软雅黑");
			solidroundrect(200, 100, 824, 150, 30, 30);
			int wordwidth = 624 / 2 - textwidth("Welcome") / 2;
			int wordheight =  50 / 2 - textheight("Welcome") / 2;
			outtextxy(wordwidth + 200, wordheight + 100, "Welcome");
		}
		{
			setfillcolor(RGB(10, 170, 170));
			solidroundrect(200, 200, 824, 650, 35, 35);
			createRoundrectButton(startButton, 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
			EndBatchDraw();
		}

		for (int i = 0; i < 6; i++)
		{
			initButtonXY(&selectBookButton[i], 200 + i * 52,300, 250 + i * 52, 724,NULL);//初始化selectBookButton
		}

		if(numFile<=6)
		{ 
			for (int i = 0; i < numFile; i++)
			{
				selectBookButton[i].title = fileName[i];
				createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
			}
		}
		else
		{
			for (int i = 0; i < 6; i++)
			{
				selectBookButton[i].title = fileName[i];
				createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
			}
		}
		

		ExMessage msg;
		while (true)//消息循环 
		{
			if (peekmessage(&msg)) {

				switch (msg.message)
				{
				case WM_LBUTTONDOWN/*鼠标左键被按下*/:
					if (cheekButton(startButton,msg.x,msg.y))
					{
						createRoundrectButton(startButton, 30, "微软雅黑", RGB(50, 120, 120), 35, 35);
					}
						
					for (int i = 0; i <= 5; i++)
					{
						if (cheekButton(selectBookButton[i], msg.x, msg.y) && numFile >= i+1)
						{
							hadlePressDown(count, i, fileName, 0);
							numPress = i;
						}
					}

					break;
				case WM_LBUTTONUP:
					if (cheekButton(startButton, msg.x, msg.y))
					{
						cleardevice();//清屏
						if(strlen(numUnit)!=0)
							sprintf_s(Title, sizeof(Title), "%s", numUnit);
						return;
					}
					else
					{
						createRoundrectButton(startButton, 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
					}

					for (int i = 0; i <= 5; i++)
					{
						if (cheekButton(selectBookButton[i], msg.x, msg.y) && numFile >= i + 1)
						{
							hadlePressUp(count, i, numUnit, fileName_nSerial);
						}
						else if (numPress == i)
						{
							hadlePressDown(count, i, fileName, 1);
						}
						else
						{
							for (int j = 0; j <= 5; j++)
							{
								BeginBatchDraw();//避免闪屏
								if(j!=numPress)
									hadlePressDown(count, j, fileName, 1);

								if (count + 6 >= numFile)
								{
									for (int x = numFile - count; x < 6; x++)
									{
										selectBookButton[x].title = NULL;
										createRoundrectButton(selectBookButton[x], 30, "微软雅黑", RGB(10, 170, 170), 35, 35);
									}
								}
								EndBatchDraw();
							}
							
						}
					}
					break;
				case WM_MOUSEWHEEL:
					if (msg.x >= 200 && msg.x <= 824 && msg.y >= 200 && msg.y <= 650 && numFile > 6)
					{
						int wheel = msg.wheel/-120;//wheel是鼠标滚轮旋转的120倍
						if (wheel>0)//当鼠标滚轮往下旋转时
						{
							if(count!=numFile-numFile%6)
								count = count + 6;
							for (int i = 0; i < 6; i++)
							{
								selectBookButton[i].title = fileName[count+i];
								createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
							}
							if (count+6>=numFile)
							{
								for (int i = numFile-count; i < 6; i++)
								{
									selectBookButton[i].title =NULL;
									createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(10, 170, 170), 35, 35);
								}
							}
						}
						else
						{
							if(count!=0)
								count = count - 6;
							for (int i = 0; i < 6; i++)
							{
								selectBookButton[i].title = fileName[i];
								createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
							}
						}
					}
					break;

				case WM_KEYUP/*判断键盘某个键被按下*/:
					if (msg.vkcode == VK_DECIMAL)
					{
						if (count != numFile - numFile % 6)
							count = count + 6;
						for (int i = 0; i < 6; i++)
						{
							selectBookButton[i].title = fileName[count + i];
							createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
						}
						if (count + 6 >= numFile)
						{
							for (int i = numFile - count; i < 6; i++)
							{
								selectBookButton[i].title = NULL;
								createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(10, 170, 170), 35, 35);
							}
						}
					}
					
					if (msg.vkcode == VK_UP)
					{
						if (count != 0)
							count = count - 6;
						for (int i = 0; i < 6; i++)
						{
							selectBookButton[i].title = fileName[i];
							createRoundrectButton(selectBookButton[i], 30, "微软雅黑", RGB(50, 140, 140), 35, 35);
						}
					}
					break;

				default:
					break;
				}
			}
		}
		return;	
}

void Positive_memory_Page(struct word *words,const int line)
{
	int jump = 0;//用于判断是否跳出while消息循环
	int Face = 0;
	bool WhetherTimeOut = true;
	int count = 1;
	IMAGE voice,voice_1,face,face_1,True,False,answer_next;
	char progress[20] = { 0 };
	char timeout[20] = { 0 };
	time_t start;

	for (int i = 0; i < line; i++)
	{
			jump = 0;
			WhetherTimeOut = true;
			
			BeginBatchDraw();//避免闪屏
			setbkcolor(RGB(0, 50, 50));//设置背景颜色
			cleardevice();
		
			setfillcolor(RGB(245, 245, 245));//设置填充颜色
			solidroundrect(100, 100,WIDTH-100, HIGH-100, 35, 35);//画一个圆角矩形

			setfillcolor(RGB(222, 224, 225));
			solidrectangle(100 + 60, 100 + 50, 100 + 65, 100 + 90);
			solidrectangle(120, HIGH - 220, WIDTH - 120, HIGH - 218);
			easySetTextStyle(30, BLACK, "微软雅黑");
			outtextxy(200,150,words[i].word);
			
			int wordwidth =  textwidth(words[i].word);
			int wordheight = textheight(words[i].word);
			
			/*
			*	载入图片
			*/

			loadimage(&voice, "Image\\voice.png", 20, 20);
			loadimage(&voice_1, "Image\\voice_1.png", 20, 20);
			loadimage(&face, "Image\\face.png", 50, 50);
			loadimage(&face_1, "Image\\face_1.png", 50, 50);
			loadimage(&True, "Image\\ture.png", 50, 50);
			loadimage(&False, "Image\\false.png", 50, 50);
			loadimage(&answer_next, "Image\\answer_next.png", 50, 50);

			if (line > 10)
			{
				if (line - line%10 <= i)
					sprintf_s(progress, sizeof(progress), "%d/%d", i % 10+1, line % 10);
				else
					sprintf_s(progress, sizeof(progress), "%d/%d", i % 10+1, 10);
			}
			else
			{
				sprintf_s(progress, sizeof(progress), "%d/%d", i % 10+1, line);
			}
			easySetTextStyle(20, BLACK, "得意黑");
			outtextxy(WIDTH-200, HIGH-200, progress);
		
			/*
			* 将图片输出到画布上
			*/
			putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2, &face_1);
			putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2 + 70, &face);

			sprintf_s(timeout, sizeof(timeout), "请在%d秒内说出词义", 6);
			outtextxy(200, HIGH / 2-100 , timeout);
			EndBatchDraw();
			
			putimage(wordwidth + 300, 155, &voice);
			playMusic(words[i].word);
			putimage(wordwidth + 300, 155, &voice_1);

			
			time(&start);

			/*
			消息循环
			*/
			ExMessage msg;
			while (jump==0)
			{
				if (time(NULL) != start && 6 - (time(NULL) - start) >= 0 && WhetherTimeOut == true)
				{
					easySetTextStyle(20, BLACK, "得意黑");
					BeginBatchDraw();//避免闪屏
					setfillcolor(RGB(245, 245, 245));
					fillrectangle(200, HIGH / 2 - 100, 200 + textwidth(timeout), HIGH / 2 - 100 + textheight(timeout));
					sprintf_s(timeout, sizeof(timeout), "请在%d秒内说出词义",(int) (6 - (time(NULL) - start)));
					outtextxy(200, HIGH / 2 - 100, timeout);
					EndBatchDraw();	
				}

				if (peekmessage(&msg)) {

					switch (msg.message)
					{
					case WM_LBUTTONUP:
						if (msg.x >= wordwidth + 300 && msg.x <= wordwidth + 300 + 25 && msg.y >= 155 && msg.y <= 155 + 20)
						{	
							putimage(wordwidth + 300, 155, &voice);
							playMusic(words[i].word);
							printf("\n");
							Sleep(1000);
							putimage(wordwidth + 300, 155, &voice_1);
						}
						if (msg.x >= WIDTH / 2 + 100 && msg.x <= WIDTH / 2 + 100 + 50 && msg.y >= (HIGH - 100 - 100) / 2 && msg.y <= (HIGH - 100 - 100) / 2 + 50)
						{

							if (Face == 0)
							{
								setfillcolor(RGB(245, 245, 245));
								fillrectangle(200, HIGH / 2 - 100, 200 + textwidth(timeout), HIGH / 2 - 100 + textheight(timeout));
								easySetTextStyle(25, BLACK, "微软雅黑");
								outtextxy(200, (HIGH - 100 - 100) / 2, words[i].mean);
								putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2, &True);
								putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2 + 70, &False);
								Face = 1;
								WhetherTimeOut = false;
							}
							else
							{
								Face = 0;
								jump = 1;
							}
						}
						if (msg.x >= WIDTH / 2 + 100 && msg.x <= WIDTH / 2 + 100 + 50 && msg.y >= (HIGH - 100 - 100) / 2 + 70 && msg.y <= (HIGH - 100 - 100) / 2 + 70 + 50)
						{
							if (Face == 0)
							{
								setfillcolor(RGB(245, 245, 245));
								fillrectangle(200, HIGH / 2 - 100, 200 + textwidth(timeout), HIGH / 2 - 100 + textheight(timeout));
								fillrectangle(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2, WIDTH - 350, HIGH - 330);
								putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2 + 35, &answer_next);
								Face = 2;
								WhetherTimeOut = false;
							}
							else
							{
								Face = 0;
								jump = 1;
							}
						}
						if (msg.x >= WIDTH / 2 + 100 && msg.x <= WIDTH / 2 + 100 + 50 && msg.y >= (HIGH - 100 - 100) / 2 + 35 && msg.y <= (HIGH - 100 - 100) / 2 + 35 + 50)
						{
							if (Face == 2)
							{	
								Face = 0;
								jump = 1;
							}
						}
						break;
					default:
						break;
					}
				}
			}

			char Reverse_memory[] = "逆向记忆";
			char Intensive_review[] = "强化复习";
			if (i == line - 1)
			{
				transition(Reverse_memory);
				if (line > 10)
				{
					Reverse_memory_Page((line - line % 10) - 1, i, words);
					transition(Intensive_review);
					Intensive_review_Page((line - line % 10) - 1, i, words);
					
				}
				else
				{
					Reverse_memory_Page(0, line-1, words);
					transition(Intensive_review);
					Intensive_review_Page(0, line-1, words);
				}
			}
			if (i % 9 == 0&&i!=0)
			{
				transition(Reverse_memory);
				Reverse_memory_Page(i-9, i, words);
				transition(Intensive_review);
				Intensive_review_Page(i - 9, i, words);
			}
	}
	transition("强制测试");
	Mandatory_testing_Page(words, line);
}

void Reverse_memory_Page(const int index,const int index_max,struct word* words)
{
	int jump = 0;//用于判断是否跳出while消息循环
	IMAGE voice, voice_1, face, face_1, Ture, False, answer_next;
	int numTF = 0;
	int Face = 0;//用于判断按下哪个按钮
	char progress[20] = { 0 };
	int count = 1;
	char timeout[20] = { 0 };
	time_t start;
	bool WhetherTimeOut = false;

	for (int i = index; i <= index_max; i++)
	{
		jump = 0;
		WhetherTimeOut = true;

		BeginBatchDraw();//避免闪屏
	
		setbkcolor(RGB(0, 50, 50));//设置背景颜色
		cleardevice();

		setfillcolor(RGB(245, 245, 245));//设置填充颜色
		solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形
		setfillcolor(RGB(222, 224, 225));
		solidrectangle(100 + 60, 100 + 50, 100 + 65, 100 + 90);
		solidrectangle(120, HIGH - 220, WIDTH - 120, HIGH - 218);
		easySetTextStyle(25, BLACK, "微软雅黑");
		outtextxy(200, 150, words[i].mean);

		int wordwidth = textwidth(words[i].mean);
		int wordheight = textheight(words[i].mean);
		/*
		*	载入图片
		*/

		loadimage(&voice, "Image\\voice.png", 20, 20);
		loadimage(&voice_1, "Image\\voice_1.png", 20, 20);
		loadimage(&face, "Image\\face.png", 50, 50);
		loadimage(&face_1, "Image\\face_1.png", 50, 50);
		loadimage(&Ture, "Image\\ture.png", 50, 50);
		loadimage(&False, "Image\\false.png", 50, 50);
		loadimage(&answer_next, "Image\\answer_next.png", 50, 50);

		sprintf_s(progress, sizeof(progress), "%d/%d", count, index_max - index+1);
		count++;
		easySetTextStyle(20, BLACK, "得意黑");
		outtextxy(WIDTH - 200, HIGH - 200, progress);

		/*
		* 将图片输出到画布上
		*/
		putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2, &face_1);
		putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2 + 70, &face);

		sprintf_s(timeout, sizeof(timeout), "请在%d秒内说出词义", 6);
		outtextxy(200, HIGH / 2 - 100, timeout);
		
		EndBatchDraw();

		putimage(wordwidth + 300, 155, &voice);
		playMusic(words[i].word);
		putimage(wordwidth + 300, 155, &voice_1);

		start=time(NULL);//获取时间

		/*
		* 消息循环
		*/

		ExMessage msg;
		while (jump == 0)
		{
			if (time(NULL) != start && 6 - (time(NULL) - start) >= 0 && WhetherTimeOut == true)
			{
				easySetTextStyle(20, BLACK, "得意黑");
				BeginBatchDraw();//避免闪屏
				setfillcolor(RGB(245, 245, 245));
				fillrectangle(200, HIGH / 2 - 100, 200 + textwidth(timeout), HIGH / 2 - 100 + textheight(timeout));
				sprintf_s(timeout, sizeof(timeout), "请在%d秒内说出词义", (int)(6 - (time(NULL) - start)));
				outtextxy(200, HIGH / 2 - 100, timeout);
				EndBatchDraw();
			}
			if (peekmessage(&msg)) {

				switch (msg.message)
				{
				case WM_LBUTTONUP:
					if (msg.x >= wordwidth + 300 && msg.x <= wordwidth + 300 + 25 && msg.y >= 155 && msg.y <= 155 + 20)
					{
						putimage(wordwidth + 300, 155, &voice);
						playMusic(words[i].word);
						printf("\n");
						Sleep(1000);
						putimage(wordwidth + 300, 155, &voice_1);
					}
					if (msg.x >= WIDTH / 2 + 100 && msg.x <= WIDTH / 2 + 100 + 50 && msg.y >= (HIGH - 100 - 100) / 2 && msg.y <= (HIGH - 100 - 100) / 2 + 50)
					{

						if (Face == 0)
						{
							setfillcolor(RGB(245, 245, 245));
							fillrectangle(200, HIGH / 2 - 100, 200 + textwidth(timeout), HIGH / 2 - 100 + textheight(timeout));
							easySetTextStyle(30, BLACK, "微软雅黑");
							outtextxy(200, (HIGH - 100 - 100) / 2, words[i].word);
							putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2, &Ture);
							putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2 + 70, &False);
							WhetherTimeOut = false;
							Face = 1;
						}
						else
						{
							
							Face = 0;
							jump = 1;
						}
					}
					if (msg.x >= WIDTH / 2 + 100 && msg.x <= WIDTH / 2 + 100 + 50 && msg.y >= (HIGH - 100 - 100) / 2 + 70 && msg.y <= (HIGH - 100 - 100) / 2 + 70 + 50)
					{
						if (Face == 0)
						{
							setfillcolor(RGB(245, 245, 245));
							fillrectangle(200, HIGH / 2 - 100, 200 + textwidth(timeout), HIGH / 2 - 100 + textheight(timeout));
							fillrectangle(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2, WIDTH - 350, HIGH - 330);
							putimage(WIDTH / 2 + 100, (HIGH - 100 - 100) / 2 + 35, &answer_next);
							WhetherTimeOut = false;
							Face = 2;
						}
						else
						{
							
							Face = 0;
							jump = 1;
						}
					}
					if (msg.x >= WIDTH / 2 + 100 && msg.x <= WIDTH / 2 + 100 + 50 && msg.y >= (HIGH - 100 - 100) / 2 + 35 && msg.y <= (HIGH - 100 - 100) / 2 + 35 + 50)
					{
						if (Face == 2)
						{
							
							Face = 0;
							jump = 1;
						}
					}


					break;
				default:
					break;
				}
			}
		}
	}
	return;
}

void Intensive_review_Page(const int index,const int index_max, struct word* words)
{
	char progress[20] = { 0 };
	int jump = 0;
	int count = 1;
	bool enter = false;
	int wordwidth = 0,wordheigh=0;
	IMAGE voice, voice_1;
	

	for (int i = index; i <= index_max; i++)
	{
		char inputword[100] = { 0 };
		jump = 0;

		BeginBatchDraw();
		setbkcolor(RGB(0, 50, 50));//设置背景颜色
		cleardevice();

		setfillcolor(RGB(245, 245, 245));//设置填充颜色
		solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形
		setfillcolor(RGB(222, 224, 225));
		solidrectangle(120, HIGH - 220, WIDTH - 120, HIGH - 218);
		
		easySetTextStyle(25, BLACK, "微软雅黑");
		
		wordwidth = textwidth(words[i].mean);
		wordheigh = textheight(words[i].mean);
		outtextxy(200, HIGH / 2 - 20 - wordheigh, words[i].mean);

		setfillcolor(RGB(245, 245, 245));
		sprintf_s(progress, sizeof(progress), "%d/%d", count, index_max - index+1);
		easySetTextStyle(20, BLACK, "得意黑");
		outtextxy(WIDTH - 200, HIGH -200, progress);

		loadimage(&voice, "Image\\voice.png", 20, 20);
		loadimage(&voice_1, "Image\\voice_1.png", 20, 20);
		putimage(300 + wordwidth, HIGH / 2 - 42, &voice_1);
		
		CreateSRK(200, HIGH / 2 - 20, WIDTH - 250, HIGH / 2 + 20, 90, 45, "Enter", inputword);
		EndBatchDraw();
		
		putimage(300 + wordwidth, HIGH / 2 - 42, &voice);
		playMusic(words[i].word);
		putimage(300 + wordwidth, HIGH / 2 - 42, &voice_1);
		

		enter = txtPwd.OnMessage();
		if (enter == true)
		{
			sprintf_s(inputword, 90, "%s", txtPwd.Text());
			jump = 1;
		}

		ExMessage msg;
		while (jump==0)
		{
			peekmessage(&msg);			// 获取消息输入

			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				if (txtName.Check(msg.x, msg.y))	enter = txtName.OnMessage();

				// 判断控件
				if (txtPwd.Check(msg.x, msg.y))		enter = txtPwd.OnMessage();


				// 判断控件
				if (btnOK.Check(msg.x, msg.y) || enter == true)
				{
					sprintf_s(inputword, 90, "%s", txtPwd.Text());
					jump = 1;
				}
				break;
			case WM_LBUTTONUP:
				if (msg.x >= 300 + wordwidth && msg.x <= 300 + wordwidth + 25 && msg.y >= HIGH / 2 - 42 && msg.y <= HIGH / 2 - 42 + 25)
				{
					putimage(300 + wordwidth, HIGH / 2 - 42, &voice);
					playMusic(words[i].word);
					printf("\n");
					Sleep(1000);
					putimage(300 + wordwidth, HIGH / 2 - 42, &voice_1);
				}
				break;
			default:
				break;
			}
		}
		
		int j = strcmp(words[i].word, inputword);
		if (j!= 0)
		{
			PutRightWord(200, HIGH / 2 + 20, words[i].word, inputword);
			Sleep(1000);
			setfillcolor(RGB(245, 245, 245));
			fillrectangle(200, HIGH / 2 +20, 200 + textwidth(words[i].word), HIGH / 2 +20 + textheight(words[i].word));
			i--;
			count--;
		}

		count++;
	}
	return;
}

void Mandatory_testing_Page(struct word* words, const int line)
{
	int random=0;
	char progress[20] = { 0 };
	IMAGE voice, voice_1;
	int correctNumber = 0;
	char fraction[50] = { 0 };
	struct WrongTopic* wrongTopic = (struct WrongTopic*)malloc(sizeof(struct WrongTopic) * line);

	for (int i = 0; i < line; i++)
	{
		BeginBatchDraw();
		setbkcolor(RGB(0,50,50));//设置背景颜色
		cleardevice();

		setfillcolor(RGB(245, 245, 245));//设置填充颜色
		solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形

		setfillcolor(RGB(222, 224, 225));

		setfillcolor(RGB(222, 224, 225));
		solidrectangle(120, HIGH - 220, WIDTH - 120, HIGH - 218);
		sprintf_s(progress, sizeof(progress), "%d/%d", i + 1, line);
		easySetTextStyle(20, BLACK, "得意黑");
		outtextxy(WIDTH - 200, HIGH - 200, progress);


		loadimage(&voice, "Image\\voice.png", 20, 20);
		loadimage(&voice_1, "Image\\voice_1.png", 20, 20);
		EndBatchDraw();


		random = Generate_random_numbers(0, 3);
		if (random == 0)
		{
			wrongTopic[i].choice = true;
			wrongTopic[i].ChoiceWrong = choiceQuestions(words, line, i, true);
			if (wrongTopic[i].ChoiceWrong == NULL)
				correctNumber++;
		}
		else if (random == 1)
		{
			wrongTopic[i].choice = true;
			wrongTopic[i].ChoiceWrong = choiceQuestions(words, line, i, false);
			if (wrongTopic[i].ChoiceWrong == NULL)
				correctNumber++;
		}
		else
		{
			wrongTopic[i].choice = false;
			wrongTopic[i].DictationWrong = dictation(words, line, i);
			if (wrongTopic[i].DictationWrong == NULL)
				correctNumber++;
		}
	}
	cleardevice();

	char cOUTPUT[40] = { 0 };
	struct buttonXY yes;
	struct buttonXY exit;
	setbkcolor(RGB(0, 50, 50));//设置背景颜色
	cleardevice();
	BeginBatchDraw();
	setfillcolor(RGB(245, 245, 245));//设置填充颜色
	solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形
	setfillcolor(RGB(100, 170, 170));//设置填充颜色
	solidroundrect(WIDTH / 2 - 210, HIGH / 2 - 80, WIDTH / 2 + 190, HIGH / 2 + 60, 20, 20);

	setfillcolor(RGB(160, 202, 202));//设置填充颜色
	solidroundrect(WIDTH / 2 - 200, HIGH / 2 - 70, WIDTH / 2 + 200, HIGH / 2 + 70, 20, 20);

	easySetTextStyle(25, BLACK, "得意黑");
	outtextxy(WIDTH / 2 - 190, HIGH / 2 - 65, "提示");

	easySetTextStyle(15, BLACK, "得意黑");
	sprintf_s(cOUTPUT, sizeof(cOUTPUT), "测试完成，你的分数是%f", 100.0 / line * correctNumber);
	outtextxy(WIDTH / 2 - 190, HIGH / 2 - 25, cOUTPUT);

	initButtonXY(&yes, HIGH / 2 + 10, WIDTH / 2 + 80, HIGH / 2 + 50, WIDTH / 2 + 180, "查看错题");
	initButtonXY(&exit, HIGH / 2 + 10, WIDTH / 2 - 80, HIGH / 2 + 50, WIDTH / 2 - 180, "退出");
	createRoundrectButton(yes, 20, "微软雅黑", RGB(50, 150, 150), 15, 15);
	createRoundrectButton(exit, 20, "微软雅黑", RGB(50, 150, 150), 15, 15);
	EndBatchDraw();

	ExMessage msg;
	while (true)
	{
		if (peekmessage(&msg)) {

			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				if (cheekButton(yes,msg.x,msg.y))
				{
					createRoundrectButton(yes, 20, "微软雅黑", RGB(100, 170, 170), 15, 15);
				}
				break;
			case WM_LBUTTONUP:
				if (cheekButton(yes, msg.x, msg.y))
				{
					cleardevice();//清屏
					goto EXIT;
				}
				else
				{
					createRoundrectButton(yes, 20, "微软雅黑", RGB(50, 150, 150), 15, 15);
				}
				break;
			default:
				break;

			}

		}

	}
EXIT:
	ShowWrongQuestion(wrongTopic, line);
	free(wrongTopic);
	return;
}

void ShowWrongQuestion(struct WrongTopic Topic[], int line)
{
	WrongQuestionBox wrong;
	IMAGE nextRight, nextLeft;
	char progress[20] = { 0 };
	bool jump = true;
	int count = 0; 
	int wrong_num=0;
	loadimage(&nextRight, "Image\\nextright.png", 50, 50);
	loadimage(&nextLeft, "Image\\nextleft.png", 50, 50);

	BeginBatchDraw();
	setbkcolor(RGB(0, 50, 50));//设置背景颜色
	cleardevice();

	setfillcolor(RGB(245, 245, 245));//设置填充颜色
	solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形
	solidrectangle(120, HIGH - 220, WIDTH - 120, HIGH - 218);
	easySetTextStyle(40, BLACK, "微软雅黑");
	outtextxy(WIDTH / 2-textwidth("查看错题")/2, 120, "查看错题");
	putimage(WIDTH / 2 - 350, HIGH / 2 - 30, &nextLeft);
	putimage(WIDTH / 2 + 300, HIGH / 2 - 30, &nextRight);
	
	setfillcolor(RGB(222, 224, 225));
	solidrectangle(120, HIGH - 220, WIDTH - 120, HIGH - 218);
	EndBatchDraw();
	for (int i = 0; i < line; i++)
	{
		wrong_num++;
	}



	for (int i = 0; i < line; i++)
	{
		jump = true;
		if (Topic[i].ChoiceWrong!=NULL|| Topic[i].DictationWrong != NULL)
		{
			count = i;	sprintf_s(progress, sizeof(progress), "%d/%d", i+1, wrong_num);
			easySetTextStyle(20, BLACK, "得意黑");
			outtextxy(WIDTH - 200, HIGH - 200, progress);
			if(Topic[i].choice==true)
				wrong.CreateWorngQuestionBox(Topic[i].ChoiceWrong, WIDTH / 2 - 200, HIGH / 2 - 100, WIDTH / 2 + 200, HIGH / 2 + 100);
			else
				wrong.CreateWorngQuestionBox(Topic[i].DictationWrong, WIDTH / 2 - 200, HIGH / 2 - 100, WIDTH / 2 + 200, HIGH / 2 + 100);

			ExMessage msg;
			while (jump)
			{
				if (peekmessage(&msg)) {
					switch (msg.message)
					{
					case WM_LBUTTONDOWN:
						if (msg.x >= WIDTH / 2 - 350 && msg.x <= WIDTH / 2 - 350 + 50 && msg.y >= HIGH / 2 - 30 && msg.y <= HIGH / 2 - 30 + 50)
						{
							i = count-1;
							jump = false;
						}
						if (msg.x >= WIDTH / 2 + 300 && msg.x <= WIDTH / 2 + 300 + 50 && msg.y >= HIGH / 2 - 30 && msg.y <= HIGH / 2 - 30 + 50)
						{
							jump = false;
						}
						break;
					}
				}
			}
			
		}
	}
}
struct ChoiceQuestionsWrongTopic* choiceQuestions(struct word* words, int line, int questionNumber, bool Chinese)
{
	int answersAppeared[4] = { line, line,line,line };
	char timeout[20] = { 0 };
	time_t start;
	struct ChoiceQuestionsWrongTopic* wrongTopic = (struct ChoiceQuestionsWrongTopic*)malloc(sizeof(struct ChoiceQuestionsWrongTopic));
	BeginBatchDraw();
	setfillcolor(RGB(10, 170, 170));

	solidroundrect(150, HIGH / 2 - 140, WIDTH - 200, HIGH / 2 - 100, 20, 20);

	easySetTextStyle(25, BLACK, "得意黑");
	outtextxy(WIDTH - 200 - textwidth("听写") - 10, HIGH / 2 - 140 + (40 / 2 - textheight("听写") / 2), "听写");

	easySetTextStyle(25, BLACK, "微软雅黑");
	if (Chinese)
	{
		outtextxy(160, HIGH / 2 - 140 + (40 / 2 - textheight(words[questionNumber].word) / 2), words[questionNumber].mean);
		sprintf(wrongTopic->title,"%s", words[questionNumber].mean);
	}
	else
	{
		outtextxy(160, HIGH / 2 - 140 + (40 / 2 - textheight(words[questionNumber].word) / 2), words[questionNumber].word);
		sprintf(wrongTopic->title, "%s", words[questionNumber].word);
	}
	int headBottom = HIGH / 2 - 100;

	playMusic(words[questionNumber].word);

	struct buttonXY selectButton[4];

	int answerNumber = Generate_random_numbers(0, 4);//用于存放正确答案所在的位置
	for (int j = 0; j < 4; j++)
	{
		int randnum = 0;

		if (answerNumber == j)
		{
			if (Chinese)
			{
				initButtonXY(&selectButton[j], headBottom + j * 40, 150, (headBottom + 40) + j * 40 - 2, (int)((WIDTH - 300) * 0.7), (words + questionNumber)->word);
				sprintf(wrongTopic->choiceWord[j], "%s", (words + questionNumber)->word);
			}
			else
			{
				initButtonXY(&selectButton[j], headBottom + j * 40, 150, (headBottom + 40) + j * 40 - 2, (int)((WIDTH - 300) * 0.7), (words + questionNumber)->mean);
				sprintf(wrongTopic->choiceWord[j], "%s", (words + questionNumber)->mean);
			}
			wrongTopic->RrightWord = j;
			createRoundrectButton(selectButton[j], 20, "微软雅黑", RGB(206, 229, 243), 25, 25);
		}
		else
		{
			randnum = Generate_random_numbers(0, line);
			while (randnum == questionNumber || answersAppeared[0] == randnum || answersAppeared[1] == randnum || answersAppeared[2] == randnum || answersAppeared[3] == randnum)
				randnum = Generate_random_numbers(0, line);
			answersAppeared[j] = randnum;
			if (Chinese)
			{
				initButtonXY(&selectButton[j], headBottom + j * 40, 150, (headBottom + 40) + j * 40 - 2, (int)((WIDTH - 300) * 0.7), (words + randnum)->word);
				sprintf(wrongTopic->choiceWord[j], "%s", (words + questionNumber)->word);
			}
			else
			{
				initButtonXY(&selectButton[j], headBottom + j * 40, 150, (headBottom + 40) + j * 40 - 2, (int)((WIDTH - 300) * 0.7), (words + randnum)->mean);
				sprintf(wrongTopic->choiceWord[j], "%s", (words + questionNumber)->mean);
			}
			createRoundrectButton(selectButton[j], 20, "微软雅黑", RGB(206, 229, 243), 25, 25);
		}
	}
	EndBatchDraw();
	start = time(NULL);//获取时间

	ExMessage msg;
	while (1)
	{
		peekmessage(&msg);			// 获取消息输入

		if (time(NULL) != start && 20 - (time(NULL) - start) >= 0)
		{
			easySetTextStyle(20, BLACK, "得意黑");
			BeginBatchDraw();//避免闪屏
			setfillcolor(RGB(245, 245, 245));
			fillrectangle(200, HIGH - 200, 200 + textwidth(timeout), HIGH - 200 + textheight(timeout));
			sprintf_s(timeout, sizeof(timeout), "请在%d秒内正确作答", (int)(20 - (time(NULL) - start)));
			outtextxy(200, HIGH - 200, timeout);
			EndBatchDraw();
		}
		else if(20 - (time(NULL) - start) < 0)
		{
			return NULL;
		}

		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
			for (int i = 0; i < 4; i++)
			{
				if (cheekButton(selectButton[i], msg.x, msg.y))
					if (answerNumber == i)
						return NULL;
					else
						return wrongTopic;
			}
			break;
		default:
			break;
		}
	}
}

struct dictationWrongTopic* dictation(struct word* words, int line, int questionNumber)
{
	char inputword[100] = { 0 };
	char timeout[20] = { 0 };
	bool enter = false;
	int jump = 0;
	struct dictationWrongTopic* wrongTopic = (struct dictationWrongTopic*)malloc(sizeof(struct dictationWrongTopic));
	BeginBatchDraw();

	setfillcolor(RGB(10, 170, 170));
	solidroundrect(150, HIGH / 2 - 140, WIDTH - 200, HIGH / 2 - 100, 20, 20);
	
	easySetTextStyle(20, BLACK, "得意黑");
	outtextxy(WIDTH - 200 - textwidth("听写") - 10, HIGH / 2 - 140 + (40 / 2 - textheight("听写") / 2), "听写");
	
	easySetTextStyle(25, BLACK, "微软雅黑");
	outtextxy(160, HIGH / 2 - 140 + (40 / 2 - textheight(words[questionNumber].mean) / 2), words[questionNumber].mean);
	sprintf(wrongTopic->dictationMean, "%s", words[questionNumber].mean);
	sprintf(wrongTopic->dictationRightWord, "%s", words[questionNumber].word);
	EndBatchDraw();

	playMusic(words[questionNumber].word);

	CreateSRK(150, HIGH / 2 - 40, WIDTH - 300, HIGH / 2 + 0, 90, 45, "Enter", inputword);

	enter = txtPwd.OnMessage();
	if (enter == true)
	{
		sprintf_s(inputword, 90, "%s", txtPwd.Text());
		jump = 1;
	}

	ExMessage msg;
	while (jump == 0)
	{
		peekmessage(&msg);			// 获取消息输入

		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
			if (txtName.Check(msg.x, msg.y))	enter = txtName.OnMessage();

			// 判断控件
			if (txtPwd.Check(msg.x, msg.y))		enter = txtPwd.OnMessage();


			// 判断控件
			if (btnOK.Check(msg.x, msg.y) || enter == true)
			{
				sprintf_s(inputword, 90, "%s", txtPwd.Text());
				jump = 1;
			}
			break;
		default:
			break;
		}
	}
	sprintf(wrongTopic->dictationWrongWord, "%s", inputword);
	if (strcmp(words[questionNumber].word, inputword) != 0)
		return NULL;
	else
		return wrongTopic;
}
void initButtonXY(struct buttonXY *button, int top, int left, int bottom, int right, const char* title)
{
	(*button).top = top;
	(*button).left = left;
	(*button).bottom = bottom;
	(*button).right = right;
	(*button).title = title;
}
int Generate_random_numbers(int range1,int range2)
{
	srand((unsigned)clock());
	Sleep(1);//延时1微秒，避免生成相同的随机数
	int random = rand() % range2 + range1;
	return random;
}

int findFile(char fileName[][20])
{
	DIR* dp = NULL;

	dp = opendir("Words\\");
	if (!dp) {
		fprintf(stderr, "opendir: %s\n", strerror(errno));
		return -1;
	}

	struct dirent* dirp;
	int i = 0;
		while ((dirp = readdir(dp)))
		{
			if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
				continue;
			sprintf_s(fileName[i], 20, "%d. %s",i+1, dirp->d_name); 
			int cLong = (int)strlen(fileName[i]);
			for (int j = 0; j < 5; j++)
			{
				fileName[i][cLong - j] = '\0';
			}
			i++;
		}
		closedir(dp);

		return i;
}

void findFile_nSerial(char fileName[][20])
{
	DIR* dp = NULL;

	dp = opendir("Words\\");
	if (!dp) {
		fprintf(stderr, "opendir: %s\n", strerror(errno));
		return;
	}

	struct dirent* dirp;
	int i = 0;
	while ((dirp = readdir(dp))) {
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
			continue;
		sprintf(fileName[i], "%s", dirp->d_name);
		int cLong=(int)strlen(fileName[i]);
		for (int j = 0; j < 5; j++)
		{
			fileName[i][cLong - j] = '\0';
		}
		i++;
	}
	closedir(dp);

	return;
}

int getLine(const char* name)
{
	FILE* fp;
	char file_location[100] = { 0 };//用于储存文件位置
	char cushioning = 0;
	int line = 1;
	sprintf_s(file_location, sizeof(file_location), "%s%s%s", "Words/", name, ".csv");
	fp = fopen(file_location, "r");
	while (cushioning != EOF)
	{
		cushioning = getc(fp);
		if (cushioning == '\n')
			line++;
	}
	fclose(fp);
	return line;
}

void getContents(char* name, int line, struct word* words)
{
	FILE* fp;
	char file_location[100] = { 0 };//用于储存文件位置
	sprintf_s(file_location, sizeof(file_location), "%s%s%s", "Words/", name, ".csv");
	if ((fp = fopen(file_location, "r")) == NULL)
	{
		puts("文件打开错误");
		return;
	}

	for (int i = 0; i < line; i++)
	{
		for (int j = 0; j < 100; j++)
		{
			(words + i)->word[j] = '\0';
			(words + i)->mean[j] = '\0';
		}
		fget(fp, (words + i)->word);
		fget(fp, (words + i)->mean);
	}
	return;
}

void fget(FILE* fp, char* words)
{
	char cushioning = 0;
	for (int i = 0; i < 100; i++)
	{
		cushioning = fgetc(fp);
		if ((cushioning != ',') && (cushioning != '\n') && (cushioning != EOF))
		{
			words[i] = cushioning;
		}
		else
		{
			break;
		}
	}
	return;
}

void downloadMusic(const int line, const struct word* words)
{
	setbkcolor(RGB(0, 50, 50));//设置背景颜色
	cleardevice();
	setfillcolor(RGB(245, 245, 245));//设置填充颜色
	solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形
	easySetTextStyle(35, BLACK, "得意黑");
	int wordwidth = WIDTH / 2 - textwidth("loading") / 2;
	int wordheight = 50 / 2 - textheight("loading") / 2;
	outtextxy(wordwidth, HIGH/2, "loading...");
	for (int i = 0; i < line; i++)
	{
		if(strlen(words[i].word)!=0)
			ShellExecute(NULL, "open", "download.exe", words[i].word, NULL, SW_HIDE);
	}
}

void playMusic(const char* Mname)
{
	if(strlen(Mname)!=0)
		ShellExecute(NULL, "open", "play.exe", Mname, NULL, SW_HIDE);
	return;
}

void hadlePressUp(const int count,const int add,char* numUnit ,const char fileName_nSerial[][20])
{
	sprintf(numUnit, "%s", fileName_nSerial[count+add]);
	setfillcolor(RGB(0, 140, 140));
	solidroundrect(200, 100, 824, 150, 30, 30);
	easySetTextStyle(30, BLACK, "微软雅黑");
	int wordwidth = 624 / 2 - textwidth(numUnit) / 2;
	int wordheight = 50 / 2 - textheight(numUnit) / 2;
	outtextxy(wordwidth + 200, wordheight + 100, numUnit);
}


void hadlePressDown(const int count,const  int add, const char fileName[][20],const int rehabilitation)
{
	if (rehabilitation == 0)
		setfillcolor(RGB(0, 90, 90));
	else
		setfillcolor(RGB(50, 140, 140));
	easySetTextStyle(30, BLACK, "微软雅黑");
	solidroundrect(300, 200 + add * 52, 724, 250 + add * 52, 35, 35);
	int wordwidth = 424 / 2 - textwidth(fileName[count + add]) / 2;
	int wordheight = 50 / 2 - textheight(fileName[count + add]) / 2;
	outtextxy(wordwidth + 300, wordheight + 200 + add * 52, fileName[count +add]);
}

void transition(const char *words)
{
	char cOUTPUT[40] = { 0 };
	setbkcolor(RGB(0, 50, 50));//设置背景颜色
	cleardevice();
	BeginBatchDraw();
	setfillcolor(RGB(245, 245, 245));//设置填充颜色
	solidroundrect(100, 100, WIDTH - 100, HIGH - 100, 35, 35);//画一个圆角矩形
	setfillcolor(RGB(100,170,170));//设置填充颜色
	solidroundrect(WIDTH / 2 - 210, HIGH / 2 - 80, WIDTH / 2 + 190, HIGH / 2 + 60, 20, 20);

	setfillcolor(RGB(160, 202, 202));//设置填充颜色
	solidroundrect(WIDTH/2-200, HIGH/2-70, WIDTH/2 +200, HIGH /2+ 70, 20, 20);

	easySetTextStyle(25, BLACK, "得意黑");
	outtextxy(WIDTH / 2 - 190, HIGH / 2 - 65, "提示");

	easySetTextStyle(20, BLACK, "得意黑");
	sprintf_s(cOUTPUT, sizeof(cOUTPUT), "学习完成，开始%s",words);
	outtextxy(WIDTH / 2 - 190, HIGH / 2 - 25, cOUTPUT);

	setfillcolor(RGB(50,150,150));
	solidroundrect(WIDTH / 2+80, HIGH / 2 + 10, WIDTH / 2 + 180, HIGH / 2 + 50, 15, 15);
	easySetTextStyle(25, BLACK, "得意黑");
	int wordwidth = 100 / 2 - textwidth("确定") / 2;
	int wordheight = 40 / 2 - textheight("确定") / 2;
	outtextxy(WIDTH / 2 + 80+wordwidth, HIGH / 2 + 10+wordheight, "确定");
	EndBatchDraw();

	ExMessage msg;
	while (true)
	{
		if (peekmessage(&msg)) {

			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				if (msg.x >= WIDTH / 2 + 80 && msg.x <= WIDTH / 2 + 180 && msg.y >= HIGH / 2 + 10 && msg.y <= HIGH / 2 + 50)
				{
					setfillcolor(RGB(100, 170, 170));
					solidroundrect(WIDTH / 2 + 80, HIGH / 2 + 10, WIDTH / 2 + 180, HIGH / 2 + 50, 15, 15);
					easySetTextStyle(25, BLACK, "得意黑");
					int wordwidth = 100 / 2 - textwidth("确定") / 2;
					int wordheight = 40 / 2 - textheight("确定") / 2;
					outtextxy(WIDTH / 2 + 80 + wordwidth, HIGH / 2 + 10 + wordheight, "确定");
				}
				break;
			case WM_LBUTTONUP:
				if (msg.x >= WIDTH / 2 + 80 && msg.x <= WIDTH / 2 + 180 && msg.y >= HIGH / 2 + 10 && msg.y <= HIGH / 2 + 50)
				{
					cleardevice();//清屏
					return;
				}
				else
				{
					setfillcolor(RGB(50, 150, 150));
					solidroundrect(WIDTH / 2 + 80, HIGH / 2 + 10, WIDTH / 2 + 180, HIGH / 2 + 50, 15, 15);
					easySetTextStyle(25, BLACK, "得意黑");
					int wordwidth = 100 / 2 - textwidth("确定") / 2;
					int wordheight = 40 / 2 - textheight("确定") / 2;
					outtextxy(WIDTH / 2 + 80 + wordwidth, HIGH / 2 + 10 + wordheight, "确定");
				}
				break;
			default:
				break;
			
			}
	
		}
	
	}
}	

//word1为rightWord,word2为wrongWord
void PutRightWord(const int x,const int y,const char* word1, const char* word2)
{
	easySetTextStyle(25, BLACK, "微软雅黑");
	int lasttextwidth = x + textwidth(word1[1]);
	for (int i = 0; i < (int)strlen(word1); i++)
	{
		if (word1[i] == word2[i])
		{	
			settextcolor(BLACK);
			if (i == 0)
			{
				outtextxy(x, y, word1[i]);
			}
			else
			{
				outtextxy(lasttextwidth, y, word1[i]);
				lasttextwidth = lasttextwidth + textwidth(word1[i]);
			}
		}
		else
		{
			settextcolor(RED);
			if (i == 0)
			{
				outtextxy(x, y, word1[i]);
			}
			else
			{
				outtextxy(lasttextwidth, y, word1[i]);
				lasttextwidth = lasttextwidth + textwidth(word1[i]);
			}
		}
	}
}
void easySetTextStyle(const int textHeigh,const int textColor,const char *textFont)
{
	settextstyle(textHeigh, 0, textFont, 0, 0, 0, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH);
	settextcolor(textColor);
	setbkmode(TRANSPARENT);
}

bool cheekButton(struct buttonXY button,int mouseX,int mouseY)
{
	return (button.left <= mouseX && mouseX <= button.right && button.top <= mouseY && mouseY <= button.bottom);
}

void createRoundrectButton(const struct buttonXY button,const int font_height,const char *font,const int fillcolor,const int ellipsewidth, const int ellipseheight)
{
	int oldlinecolor = getlinecolor();
	int oldbkcolor = getbkcolor();
	int oldfillcolor = getfillcolor();
	int oldtextcolor = gettextcolor();

	setbkmode(TRANSPARENT);
	setlinecolor(BLACK);			// 设置画线颜色
	setbkcolor(RGB(245, 245, 245));				// 设置背景颜色
	setfillcolor(fillcolor);			// 设置填充颜色
	easySetTextStyle(font_height, BLACK, font);
	solidroundrect(button.left, button.top, button.right, button.bottom, ellipsewidth, ellipseheight);
	if(button.title!=NULL)
		outtextxy(button.left + (button.right - button.left - textwidth(button.title) + 1) / 2, button.top + (button.bottom - button.top - textheight(button.title) + 1) / 2, button.title);

	setlinecolor(oldlinecolor);
	setbkcolor(oldbkcolor);
	setfillcolor(oldfillcolor);
	settextcolor(oldtextcolor);
}