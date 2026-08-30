#include<stdio.h>
#include<termios.h>
#include<sys/select.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define BOUND_TOP 1
#define BOUND_BOTTOM 40
#define BOUND_LEFT 1
#define BOUND_RIGHT 79


typedef struct Segment{
	int row;
	int col;
} Segment;

enum Collision{
	NO_COL,
	FOOD,
	WALL,
	BODY,
};

enum Direction{
	NO_DIR,
	RIGHT,
	LEFT,
	UP,
	DOWN
};

enum GameState{
	PLAYING,
	PAUSED,
	GAMEOVER
};

enum Input{
	INPUT_NONE,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_PAUSE,
	INPUT_ENTER,
	INPUT_QUIT
};

void draw_sidebar(enum GameState state, int score, int high_score){
	printf("\033[1;83H----------------");
	printf("\033[2;83HSTATUS");

	printf("\033[4;83HScore     : %d", score);
	printf("\033[5;83HHigh Score: %d", high_score);
	printf("\033[6;83H----------------");

	printf("\033[10;83HCONTROLS");
	printf("\033[11;83HW : Up");
	printf("\033[12;83HS : Down");
	printf("\033[13;83HA : Left");
	printf("\033[14;83HD : Right");
	printf("\033[15;83HP : Pause");
	printf("\033[16;83HQ : Quit");

	if (state == PLAYING)
		printf("\033[22;83HPlaying");
	else if (state == PAUSED)
		printf("\033[22;83HPaused");
}

void move_snake(Segment *snake, int *head, const int capacity, const enum  Direction direction){
	int new_head = (*head + 1) % capacity;
	snake[new_head] = snake[*head];
	*head = new_head;
	if(direction == RIGHT)
		snake[*head].col += 2;
	else if(direction == LEFT)
		snake[*head].col -= 2;
	else if(direction == UP)
		snake[*head].row--;
	else if(direction == DOWN)
		snake[*head].row++;
}

void draw_board(void){
	printf("\033[2J\033[?25l");
	for (int col = BOUND_LEFT; col <= BOUND_RIGHT; col += 2) {
		printf("\033[%d;%dH--", BOUND_TOP, col);
		printf("\033[%d;%dH--", BOUND_BOTTOM, col);
	}

	for (int row = BOUND_TOP; row <= BOUND_BOTTOM; row++) {
		printf("\033[%d;%dH--", row, BOUND_LEFT);
		printf("\033[%d;%dH--", row, BOUND_RIGHT);
	}
}

void draw_snake(const Segment *snake, const int head){
	printf("\033[%d;%dH♦", snake[head].row, snake[head].col);
}

void draw_food(const Segment food){
	printf("\033[31m\033[%d;%dH✦\033[0m", food.row, food.col);
}

void erase_segment(const Segment *segment){
	printf("\033[%d;%dH  ", segment->row, segment->col);
}

void generate_food(Segment *food){
	food->row = rand() % (BOUND_BOTTOM - BOUND_TOP - 1) + BOUND_TOP + 1;
	food->col = 2 * (rand() % ((BOUND_RIGHT - BOUND_LEFT - 3) / 2 + 1)) + BOUND_LEFT + 2;
}
	
enum Collision collision(const Segment *snake, const Segment food_seg, const int len, const int head, const int tail, const int capacity){
	if(snake[head].row == BOUND_TOP || snake[head].row == BOUND_BOTTOM || snake[head].col == BOUND_RIGHT || snake[head].col == BOUND_LEFT)
{
		printf("\033[100;100H%s","hi");
		return WALL ;
}
	for(int i = 0; i < len - 1; i++)
		if (snake[head].row == snake[(tail + i) % capacity].row && snake[head].col== snake[(tail + i) % capacity].col){
		printf("\033[100;100H%s","hlo hi");
			return BODY;
	
	}
	if(snake[head].row == food_seg.row && snake[head].col == food_seg.col)
		return FOOD;

	return NO_COL;
	
}
		

enum Input read_input(void){
	fd_set set;
	struct timeval timeout;
	char ch;

	FD_ZERO(&set);
	FD_SET(STDIN_FILENO, &set);

	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	if (select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout) <= 0){
		return INPUT_NONE;
	}

	read(STDIN_FILENO, &ch, 1);

	switch (ch){
		case 'w':
			return INPUT_UP;

		case 's':
			return INPUT_DOWN;

		case 'a':
			return INPUT_LEFT;

		case 'd':
			return INPUT_RIGHT;

		case 'p':
			return INPUT_PAUSE;

		case '\n':
		case '\r':
			return INPUT_ENTER;

		case 'q':
			return INPUT_QUIT;

		default:
			return INPUT_NONE;
	}
}

void array_reverse(Segment *start, Segment *end){
	Segment tmp;
	while(start < end){
		tmp = *start;
		*start++  = *end;
		*end-- = tmp;
	}
}

void normalize(Segment *snake, int *head, int *tail, const int len){
	if(*tail){
		array_reverse(snake, snake + *tail -1);
		array_reverse(snake + *tail, snake + len - 1);
		array_reverse(snake, snake + (len - 1));
		*tail = 0;
		*head = len - 1;
	}
}


void draw_gameover(int score)
{
	const char *text[] = {
		" GGG   AAA  MM MM  EEEEE       OOO  V   V EEEEE RRR  ",
		"G     A   A M M M  E          O   O V   V E     R  R ",
		"G GG  AAAAA M   M  EEEE       O   O V   V EEEE  RRR  ",
		"G  G  A   A M   M  E          O   O  V V  E     R R  ",
		" GGG  A   A M   M  EEEEE       OOO    V   EEEEE R  R"
	};

	for (int i = 0; i < 5; i++)
		printf("\033[%d;%dH%s", 15 + i, 15, text[i]);

	char score_text[50];

	snprintf(score_text, sizeof(score_text), "SCORE : %d", score);

	int col = 40 - strlen(score_text) / 2;

	printf("\033[%d;%dH%s\n", 21, col, score_text);
}

int main(){
	int i, j, len = 2, capacity = len * 2, head = 1, tail = 0, score = 0, high_score = 0;
	srand(time(NULL));
	enum GameState state = PAUSED;
	enum Direction direction = NO_DIR;
	enum Input input;
	enum Collision collision_stat;
	struct termios old, new;
	tcgetattr(STDIN_FILENO, &old);
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
	Segment *snake = malloc(sizeof(Segment) * capacity);
	snake[0] = (Segment){2, 3};
	snake[1] = (Segment){2, 5};
	Segment old_tail, food_seg;

	draw_board();
	draw_sidebar(state, score, high_score);
	draw_snake(snake, head);
	printf("\033[%d;%dH♦", snake[tail].row, snake[tail].col);
	generate_food(&food_seg);
	fflush(stdout);			

	while(1){
		input =	read_input();
		if(input > 0 && input < 5){
			state = PLAYING;
			printf("\033[22;83HPlaying\n");
			if (direction != LEFT && input == INPUT_RIGHT)
				direction = RIGHT;
			else if(direction != RIGHT &&  input == INPUT_LEFT)
				direction = LEFT;
			else if(direction != UP && input == INPUT_DOWN)
				direction = DOWN;
			else if(direction != DOWN && input == INPUT_UP)
				direction = UP; 
		}
		else if(input == INPUT_PAUSE){
			state = PAUSED;
			printf("\033[22;83HPaused \n");
		}
		else if(input == INPUT_QUIT)
			break;
		if(state == PLAYING){
			old_tail = snake[tail];
			move_snake(snake, &head, capacity, direction);
			collision_stat = collision(snake, food_seg, len, head, tail, capacity);
			if (collision_stat == FOOD){
				generate_food(&food_seg);
				score += 5;
				if(score > high_score)
					high_score = score;
				printf("\033[4;83HScore     : %-5d", score);
				printf("\033[5;83HHigh Score: %-5d", high_score);
				len++;
				if(len == capacity){
					normalize(snake, &head, &tail, len);
					capacity *= 2;
					Segment *tmp = realloc(snake, sizeof(Segment) * capacity);
					if(tmp)
						snake = tmp;
					else 
						break;
				}

			}
			else{
				erase_segment(&old_tail);
				tail = (tail + 1) % capacity;

			}
			draw_food(food_seg);
			draw_snake(snake, head);

			fflush(stdout);
			usleep(100000);

			if(collision_stat == WALL || collision_stat == BODY){
				state = GAMEOVER;
			}
		}
		if( state == GAMEOVER){
			printf("\033[1m\033[31m\033[22;83HGAME OVER!\033[0m\n");
			draw_gameover(score);
			sleep(3);
			len = 2;
			capacity = 4;
			score = 0;
			Segment *tmp = realloc(snake, sizeof(Segment) * capacity);
			if(tmp)
				snake = tmp;
			else
				break;
			snake[0] = (Segment){2, 3};
			snake[1] = (Segment){2, 5};
			tail = 0;
			head = 1;
			direction = NO_DIR;
			draw_board();
			draw_sidebar(state, score, high_score);
			draw_snake(snake, head);
			printf("\033[%d;%dH♦", snake[tail].row, snake[tail].col);
			generate_food(&food_seg);
			fflush(stdout);	
			tcflush(STDIN_FILENO, TCIFLUSH);
			state = PAUSED;
		}
	}
	free(snake);
	snake = NULL;
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	printf("\033[?25h\033[H\033[2J");
	return 0;
}
