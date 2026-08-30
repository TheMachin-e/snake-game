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

void draw_snake(const Segment *snake, const int head){
	printf("\033[%d;%dH♦", snake[head].row, snake[head].col);
}

void draw_food(const Segment food){
	printf("\033[%d;%dH✦", food.row, food.col);
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
		return WALL ;

	for(int i = 0; i < len - 1; i++)
		if (snake[head].row == snake[(tail + i) % capacity].row && snake[head].col== snake[(tail + i) % capacity].col){
			printf("\033[2;90H%s","HI");	
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


void draw_game_over(int score)
{
	const char *text[] = {
		" GGG   AAA  MM MM  EEEEE       OOO  V   V EEEEE RRR  ",
		"G     A   A M M M  E          O   O V   V E     R  R ",
		"G GG  AAAAA M   M  EEEE       O   O V   V EEEE  RRR  ",
		"G  G  A   A M   M  E          O   O  V V  E     R R  ",
		" GGG  A   A M   M  EEEEE       OOO    V   EEEEE R  R"
	};

	for (int i = 0; i < 5; i++)
		printf("\033[%d;%dH%s", 15 + i, 10, text[i]);

	char score_text[50];

	snprintf(score_text, sizeof score_text,
			"SCORE : %d", score);

	int col = 40 - strlen(score_text) / 2;

	printf("\033[%d;%dH%s", 21, col, score_text);
}

int main(){
	int i, j, len = 2, capacity = len * 2, head = 1, tail = 0, score = 0;
	srand(time(NULL));
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
	printf("\033[2J");
	printf("\033[?25l");

	for(i=BOUND_TOP;i<=BOUND_BOTTOM;i++)
		for(j=BOUND_LEFT;j<=BOUND_RIGHT;j+=2)
			if(i==1||j==1||i==40||j==79)
				printf("\033[%d;%dH--",i,j);

	draw_snake(snake, head);
	printf("\033[%d;%dH♦", snake[tail].row, snake[tail].col);
	generate_food(&food_seg);
	fflush(stdout);			

	while(1){
		input =	read_input();
		if(input > 0 && input < 5){
			if (direction != LEFT && input == INPUT_RIGHT)
				direction = RIGHT;
			else if(direction != RIGHT &&  input == INPUT_LEFT)
				direction = LEFT;
			else if(direction != UP && input == INPUT_DOWN)
				direction = DOWN;
			else if(direction != DOWN && input == INPUT_UP)
				direction = UP; 
		}
		else if(input == INPUT_QUIT)
			break;
		old_tail = snake[tail];
		if(direction)
			move_snake(snake, &head, capacity, direction);
		collision_stat = collision(snake, food_seg, len, head, tail, capacity);
		if (collision_stat == FOOD){
			generate_food(&food_seg);
			score += 5;
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
		else if(direction){
			erase_segment(&old_tail);
			tail = (tail + 1) % capacity;
			
		}
		draw_food(food_seg);
		draw_snake(snake, head);

		fflush(stdout);
		if(collision_stat == WALL || collision_stat == BODY){
			free(snake);
			snake = NULL;
			draw_game_over(score);
			fflush(stdout);
			usleep(5000000);
			break;
		}
		usleep(100000);
	}
	tcflush(STDIN_FILENO, TCIFLUSH);
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
	printf("\033[?25h");
	printf("\033[H");
	printf("\033[2J");
	return 0;
}
