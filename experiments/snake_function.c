#include<stdio.h>
#include<termios.h>
#include<sys/select.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>

#define BOUND_TOP 1
#define BOUND_BOTTOM 40
#define BOUND_LEFT 1
#define BOUND_RIGHT 79


struct Segment{
	int row;
	int col;
};

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

void move_snake(struct Segment *snake, const int len, const enum  Direction direction){
	for(int i = len - 1; i > 0; i-- )
		snake[i] = snake[i-1];
	if(direction == RIGHT)
		snake[0].col += 2;
	else if(direction == LEFT)
		snake[0].col -= 2;
	else if(direction == UP)
		snake[0].row--;
	else if(direction == DOWN)
		snake[0].row++;
}

void draw_snake(const struct Segment *snake, const int len){
	printf("\033[%d;%dH♦", snake[0].row, snake[0].col);
	for(int i=1; i < len; i++)
		printf("\033[%d;%dH♢", snake[i].row, snake[i].col);
}

void draw_food(const struct Segment food){
	printf("\033[%d;%dH✦", food.row, food.col);
}

void erase_segment(struct Segment *segment){
	printf("\033[%d;%dH  ", segment->row, segment->col);
}

void generate_food(struct Segment *food){
	food->row = rand() % (BOUND_BOTTOM - BOUND_TOP - 1) + BOUND_TOP + 1;
	food->col = 2 * (rand() % ((BOUND_RIGHT - BOUND_LEFT - 3) / 2 + 1)) + BOUND_LEFT + 2;
}
	
enum Collision collision(const struct Segment *snake, const struct Segment food_seg, int len){
	if(snake[0].row == BOUND_TOP || snake[0].row == BOUND_BOTTOM || snake[0].col == BOUND_RIGHT || snake[0].col == BOUND_LEFT)
		return WALL ;

	for(int i = 1; i < len; i++)
		if (snake[0].row == snake[i].row && snake[0].col== snake[i].col)
				return BODY;

	if(snake[0].row == food_seg.row && snake[0].col == food_seg.col)
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
void draw_game_over(void)
{
    const char *text[] = {
        " GGG   AAA  MM MM  EEEEE       OOO  VV VV EEEEE RRR  ",
        "G     A   A M M M  E          O   O VV VV E     R  R ",
        "G GG  AAAAA M   M  EEEE       O   O VV VV EEEE  RRR  ",
        "G  G  A   A M   M  E          O   O  V V  E     R R  ",
        " GGG  A   A M   M  EEEEE       OOO   V V  EEEEE R  RR"
    };

    for (int i = 0; i < 5; i++)
        printf("\033[%d;%dH%s", 15 + i, 10, text[i]);
}

int main(){
	int i, j, len = 5;
	srand(time(NULL));
	enum Direction direction = NO_DIR;
	enum Input input;
	enum Collision collision_stat;
	struct termios old, new;
	tcgetattr(STDIN_FILENO, &old);
	new = old;
	new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
	struct Segment snake[5] = {{2, 11}, {2, 9}, {2, 7}, {2, 5}, {2, 3}}, old_tail, food_seg;
	printf("\033[2J");
	printf("\033[?25l");

	for(i=BOUND_TOP;i<=BOUND_BOTTOM;i++)
		for(j=BOUND_LEFT;j<=BOUND_RIGHT;j+=2)
			if(i==1||j==1||i==40||j==79)
				printf("\033[%d;%dH--",i,j);

	draw_snake(snake, len);
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
		old_tail = snake[len - 1];
		if(direction)
			move_snake(snake, len, direction);
		collision_stat = collision(snake, food_seg, len);
		erase_segment(&old_tail);
		if (collision_stat == FOOD){
			generate_food(&food_seg);
			//len++;
		}
		draw_food(food_seg);
		draw_snake(snake,len);
		fflush(stdout);
		if(collision_stat == WALL || collision_stat == BODY){
			direction = NO_DIR;
			draw_game_over();
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
