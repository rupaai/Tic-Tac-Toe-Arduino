#include <LiquidCrystal.h>
#include <Keypad.h>
#include <elapsedMillis.h>
#include <Arduino.h>

elapsedMillis timeElapsed; //declare global if you don't want it reset every time loop runs

						   // delay in milliseconds between blinks of the LED
const int interval = 2000;

bool result;
bool g = false, d = false;

int match_rnd = 0;
bool player = true;
//to enter the keypad loop
bool m[10], rp[10], gp[10];//rp, gp => score near jonno,m=> ith index e press hoye geche..abar press korle ar jolbe na
const int pr = 24, pg = 33;
int rg[10], kg[10];//rg => red led jalanor jonno,kg = green led
int twoD_ttt[3][3];

const int ROWS = 4, COLS = 3;

char hexaKeys[ROWS][COLS] = {
	{ '1','2','3' },
{ '4','5','6' },
{ '7','8','9' },
{ '*','0','#' }
};
const int r1 = 42, r2 = 43, r3 = 45, r4 = 44;
const int c1 = 46, c2 = 47, c3 = 49;
byte rowPins[ROWS] = { r1, r2, r3, r4 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { c1, c2, c3 }; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int char_row = 0, cursor_r = 1, cursor_c = -1, char_col = 0;
char prev_char = '\0', present_char = '\0', store_char = '\0';

int fetch_row_col[9][2] = {
	{2,0},//press 1
	{2,1}, //press 2
	{2,2}, //press 3
	{1,0}, //press 4
	{1,1}, //press 5
	{1,2}, // press 6
	{0,0}, // press 7
	{0,1}, // press 8
	{0,2} //press 9
};

String char_arr[9] = {
	"1ABC",
	"2DEF",
	"3GHI",
	"4JKL",
	"5MNO",
	"6PQR",
	"7STU",
	"8VWX",
	"9YZ"
};

struct player {
	String name;
	int score = 0;
	int highscore = 0;
}p[2];

struct game_flags {
	boolean take_names[2] = { true,false };
	boolean reset_values = false;
	boolean lvl[2] = { false,false };
	boolean match_res = false;
	boolean lcd_flag[2] = { false,false };
	int draw_flag = 0;
}fg;

//functions
void predict_draw();
bool cond_check(int fg_g, int fg_r,int indx);
void reset_Round();
void end_game();

void take_name_lvl();
void setup_player_names(int p_index);
void valueAssignment();
void redWin();
void greenWin();
void drawMatch();
void keySelection(char key);
bool resCheck();

void setup() {
	// set up the LCD's number of columns and rows:
	lcd.begin(16, 2);
	analogWrite(6, 40);
	lcd.clear();

	valueAssignment();

	for (int i = 1; i <= 9; i++)
	{
		pinMode(kg[i], OUTPUT);
		pinMode(rg[i], OUTPUT);
	}
}

void loop() {

	if (fg.lvl[0] == false)
	{
		take_name_lvl();
	}
	else if (fg.lvl[1] == false)
	{
		if (fg.reset_values == true)
		{
			match_rnd++;
			lcd.clear();
			delay(1000);
			lcd.print("Round " + String(match_rnd));
			fg.reset_values = false;
		}
		char key = customKeypad.getKey();

		if (key)
		{
			if (key == '0')//do another round
			{
				reset_Round();
			}
			else if (key == '#')//end game
			{
				end_game();
				fg.lvl[1] = true;
			}
			else
				keySelection(key);
		}
		if (fg.match_res == false)
		{
			if (p[0].score >= 3 || p[1].score >= 3)
			{
				result = resCheck();
				if (result)
				{
					fg.match_res = true;

					lcd.clear();
					delay(1000);
					if (d)
					{
						lcd.print("Match is Draw!!!");
						drawMatch();
					}
					else
					{
						if (g)
						{
							lcd.print("Player " + p[0].name + " won");
							p[0].highscore++;
							greenWin();
						}
						else
						{
							lcd.print("Player " + p[1].name + " won");
							p[1].highscore++;
							redWin();
						}
					}
					lcd.clear();
					delay(1000);
					lcd.setCursor(0, 0);
					lcd.print("Play again?");
					lcd.setCursor(0, 1);
					lcd.print("'0'=Y, '#'= N");
				}
			}
		}
	}
}

void reset_Round()
{
	fg.reset_values = true;
	fg.match_res = false;
	p[0].score = 0;
	p[1].score = 0;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			twoD_ttt[i][j] = 0;
		}
	}

	for (int i = 1; i <= 9; i++)
	{
		m[i] = false;
		rp[i] = false;
		gp[i] = false;
		digitalWrite(kg[i], LOW);
		digitalWrite(rg[i], LOW);
	}

	for (int i = 0; i < 2; i++)
	{
		fg.lcd_flag[i] = false;
	}

	player = true;
	g = false;
	d = false;

	char_row = 0, cursor_r = 1, cursor_c = -1, char_col = 0; 
	prev_char = '\0', present_char = '\0', store_char = '\0';
}

void end_game()
{
	lcd.clear();
	delay(1000);
	lcd.print(p[0].name + "  " + p[1].name);
	lcd.setCursor(0, 1);
	lcd.print(String(p[0].highscore) + "  " + String(p[1].highscore));
	delay(1000);
	
	lcd.clear();
	delay(1000);

	if (p[0].highscore > p[1].highscore)
	{
		lcd.print("Congrats " + p[0].name+"!!!");
	}
	else
	{
		if (p[0].highscore == p[1].highscore)
		{
			lcd.print("Everyone's a winner");
		}
		else {
			lcd.print("Congrats "+p[1].name+"!!!");
		}
	}
}

void take_name_lvl()
{
	if (fg.take_names[0] == true)
	{
		if (fg.lcd_flag[0] == false)
		{
			lcd.print("Player 1(Red):");
			fg.lcd_flag[0] = true;
		}
		setup_player_names(0);
	}
	else if (fg.take_names[1] == true)
	{
		if (fg.reset_values == false)
		{
			lcd.clear();
			delay(1000);
			cursor_c = -1;
			store_char = prev_char = present_char = '\0';
			fg.reset_values = true;
			lcd.print("Player 2(Green):");
		}
		setup_player_names(1);
	}
	else if (fg.take_names[0] == false && fg.take_names[1] == false)
	{
		lcd.clear();
		delay(1000);
		lcd.print("Player 1 name:");
		lcd.setCursor(0, 1);
		lcd.print(p[0].name);
		delay(2000);

		lcd.clear();
		delay(1000);
		lcd.print("Player 2 name:");
		lcd.setCursor(0, 1);
		lcd.print(p[1].name);
		delay(2000);
		fg.lvl[0] = true;
	}
}

void setup_player_names(int p_index)
{
	char customKey = customKeypad.getKey();

	if (customKey) {
		if (customKey == '*')
		{
			p[p_index].name += store_char;
			fg.take_names[p_index] = false;

			if (p_index + 1 < 2)
			{
				fg.take_names[p_index + 1] = true;
			}
		}
		else
		{
			present_char = customKey;

			//search for the character
			char_row = (customKey - '0') - 1;

			if (timeElapsed < interval)
			{
				if (prev_char == present_char)
				{
					lcd.setCursor(cursor_c, cursor_r);
					lcd.print(char_arr[char_row][++char_col]);
					store_char = char_arr[char_row][char_col];
				}
				else
				{
					char_col = 0;

					if (store_char != '\0')
					{
						p[p_index].name += store_char;
					}

					cursor_c++;
					lcd.setCursor(cursor_c, cursor_r);
					lcd.print(char_arr[char_row][char_col]);
					store_char = char_arr[char_row][char_col];
				}
			}
			else
			{
				if (store_char != '\0')
				{
					p[p_index].name += store_char;
				}
				char_col = 0;
				cursor_c++;
				lcd.setCursor(cursor_c, cursor_r);
				lcd.print(customKey);
				store_char = customKey;
			}
			prev_char = present_char;

			timeElapsed = 0;

			if (char_row == 8 && char_col >= 3)
			{
				char_col = 0;
			}
			else if (char_row < 8 && char_col >= 4)
			{
				char_col = 0;
			}
		}
	}
}

void redWin()
{
	for (int i = 1; i <= 9; i++)
	{
		digitalWrite(rg[i], HIGH);
		digitalWrite(kg[i], LOW);
	}
	if(d == false)
		delay(1000);
	
}

void greenWin()
{
	for (int i = 1; i <= 9; i++)
	{
		digitalWrite(kg[i], HIGH);
		digitalWrite(rg[i], LOW);
	}
	if (d == false)
		delay(1000);
}

void drawMatch()
{
	redWin();
	delay(1000);
	greenWin();
	delay(1000);
}

void keySelection(char key)
{
	int indx = key - '0';
	if (m[indx] == false)
	{
		m[indx] = true;
		int fetch_row = fetch_row_col[indx - 1][0];
		int fetch_col = fetch_row_col[indx - 1][1];

		if (player == true)//red player
		{
			twoD_ttt[fetch_row][fetch_col] = 1; //1 for red
			gp[indx] = true;//green player er score near jonno
			p[0].score++;
			digitalWrite(kg[indx], HIGH);
		}
		else//green player
		{
			twoD_ttt[fetch_row][fetch_col] = 2; //2 for green
			rp[indx] = true;//red player er score near jonno.
			p[1].score++;
			digitalWrite(rg[indx], HIGH);
		}
		player = !player;
	}
}

bool resCheck()
{
	if (p[0].score >= 3)
	{
		if (gp[1] == true)
		{
			if (gp[2] == true && gp[3] == true)
			{
				g = true;
				return true;
			}
			else if (gp[4] == true && gp[7] == true)
			{
				g = true;
				return true;
			}
			else if (gp[5] == true && gp[9] == true)
			{
				g = true;
				return true;
			}
		}

		else if (gp[5] == true)
		{
			if (gp[2] == true && gp[8] == true)
			{
				g = true;
				return true;
			}
			else if (gp[3] == true && gp[7] == true)
			{
				g = true;
				return true;
			}
			else if (gp[4] == true && gp[6] == true)
			{
				g = true;
				return true;
			}
		}
		else if (gp[9] == true)
		{
			if (gp[6] == true && gp[3] == true)
			{
				g = true;
				return true;
			}
			if (gp[8] == true && gp[7] == true)
			{
				g = true;
				return true;
			}
		}
		int cnt = 0;

		//draw match

		for (int i = 1; i <= 9; i++)
		{
			if (rp[i] == true || gp[i] == true)
			{
				cnt++;
			}
		}
		if (cnt >= 7)
		{
			fg.draw_flag = 0;
			
			if (cnt < 9)
			{
				predict_draw();

				if (fg.draw_flag == 8)
				{
					if (fg.lcd_flag[1] == false)
					{
						lcd.setCursor(0, 1);
						lcd.print("Will become draw");
						fg.lcd_flag[1] = true;
					}
				}
			}
			else if (cnt == 9)
			{
				d = true;
				return true;
			}
		}
	}
	if (p[1].score >= 3)
	{
		if (rp[1] == true)
		{
			if (rp[2] == true && rp[3] == true)
			{
				return true;
			}
			if (rp[4] == true && rp[7] == true)
			{
				return true;
			}
			if (rp[5] == true && rp[9] == true)
			{
				return true;
			}
		}

		else if (rp[5] == true)
		{
			if (rp[2] == true && rp[8] == true)
			{
				return true;
			}
			if (rp[3] == true && rp[7] == true)
			{
				return true;
			}
			if (rp[4] == true && rp[6] == true)
			{
				return true;
			}
		}
		else if (rp[9] == true)
		{
			if (rp[6] == true && rp[3] == true)
			{
				return true;
			}
			if (rp[8] == true && rp[7] == true)
			{
				return true;
			}
		}
	}
	else
	{
		return false;
	}
}

bool cond_check(int fg_g, int fg_r,int indx)
{
	if (fg_g + fg_r == 3)
	{
		fg.draw_flag++;
		return true;
	}
	else
	{
		if ( 9 - (p[0].score + p[1].score) == 1 )
		{
			if (fg_g == 2 && player == true)//player = true(red), green = false
			{
				fg.draw_flag++;
				return true;
			}
			else if (fg_r == 2 && player == false)
			{
				fg.draw_flag++;
				return true;
			}
			else if (fg_g < 2 && fg_r < 2)
			{
				fg.draw_flag++;
				return true;
			}
			return false;
		}
		if (fg_g < 2 && fg_r < 2)
		{
			fg.draw_flag++;
			return true;
		}
		return false;
	}
}

void predict_draw()
{
	int fg_g, fg_r;
	int fg_g2, fg_r2;
	int fg_g3, fg_r3;
	int fg_g4, fg_r4;

	for (int i = 0; i < 3; i++)
	{
		fg_g = fg_g2 = 0;
		fg_r = fg_r2 = 0;

		fg_g3 = fg_r3 = 0;
		fg_g4 = fg_r4 = 0;

		//column and row
		for (int j = 0, c=2; j < 3; c--,j++)
		{
			//column
			if (twoD_ttt[j][i] == 2)
			{
				fg_g++;
			}
			else if (twoD_ttt[j][i] == 1)
			{
				fg_r++;
			}

			//row
			if (twoD_ttt[i][j] == 2)
			{
				fg_g2++;
			}
			else if (twoD_ttt[i][j] == 1)
			{
				fg_r2++;
			}

			//diagonal
			if (i == 0)
			{
				if (twoD_ttt[j][j] == 2)
				{
					fg_g3++;
				}
				else if (twoD_ttt[j][j] == 1)
				{
					fg_r3++;
				}
			}
			else if (i == 2)
			{
				if (twoD_ttt[j][c] == 2)
				{
					fg_g4++;
				}
				else if (twoD_ttt[j][c] == 1)
				{
					fg_r4++;
				}
			}
		}
		if (cond_check(fg_g, fg_r,i) == false) {
			return;
		}
		if (cond_check(fg_g2, fg_r2,i) == false) {
			return;
		}
		if (i == 0)
		{
			if (cond_check(fg_g3, fg_r3, i) == false) {
				return;
			}
		}
		else if (i == 2)
		{
			if (cond_check(fg_g4, fg_r4, i) == false) {
				return;
			}
		}
	}
}

void valueAssignment()
{
	kg[1] = 24, kg[2] = 27, kg[3] = 30, kg[4] = 23, kg[5] = 26, kg[6] = 29, kg[7] = 22, kg[8] = 25, kg[9] = 28;
	rg[1] = 33, rg[2] = 36, rg[3] = 39, rg[4] = 32, rg[5] = 35, rg[6] = 40, rg[7] = 31, rg[8] = 34, rg[9] = 37;
	for (int i = 1; i <= 9; i++)
	{
		m[i] = false;
		rp[i] = false;
		gp[i] = false;
	}
}

