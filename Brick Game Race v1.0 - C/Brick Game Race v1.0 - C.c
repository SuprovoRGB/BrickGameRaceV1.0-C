//====================================================================================================
//====================================================================================================
// Program : C - Brick Game Race v1.0
// By      : Suprovo Basak
// Date    : 7-January-2020
//====================================================================================================
//====================================================================================================

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

//====================================================================================================

#define TRUE 1
#define FALSE 0

//==================================================

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define YELLOW 6
#define GREY 8
#define WHITE 15
#define LIGHT_GREEN 10

//==================================================

#define WINDOW_X 39
#define WINDOW_Y 27
#define DEFAULT_FPS_DELAY 65
#define LEFT 0
#define RIGHT 1
#define LIFE 5
#define MAX_SCORE 9999

//====================================================================================================

int colors[6] = {BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW};

//==================================================

int bricks[24][10];

//==================================================

int shape_wall[6] =
{
    1, 4,
    0,
    1,
    1,
    1
};

int shape_car[14] =
{
    3, 4,
    0, 1, 0,
    1, 1, 1,
    0, 1, 0,
    1, 0, 1
};

//--------------------------------------------------

int shape_burst1[18] =
{
    4, 4,
    1, 0, 0, 1,
    0, 1, 1, 0,
    0, 1, 1, 0,
    1, 0, 0, 1
};

int shape_burst2[18] =
{
    4, 4,
    0, 1, 1, 0,
    1, 0, 0, 1,
    1, 0, 0, 1,
    0, 1, 1, 0
};

//==================================================

char input = '\0';
int main_car_side = LEFT;
int main_car_collision_status = FALSE;
int total_life = LIFE;
int current_life = TRUE;
unsigned long int score = 0;
int win_status = FALSE;
int fps_delay_as_level = DEFAULT_FPS_DELAY;

//====================================================================================================

void set_window_size(int x, int y)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT windowSize = {0, 0, x, y};
    SetConsoleWindowInfo(consoleHandle, 1, &windowSize);
    COORD dwSize = {10, 10};
    SetConsoleScreenBufferSize(consoleHandle, dwSize);
}

//--------------------------------------------------

void set_cursor_visibility(int status)
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 10;
    cursorInfo.bVisible = status;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

//--------------------------------------------------

void reset_line_position()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = 0;
    dwPos.Y = 0;
    SetConsoleCursorPosition(consoleHandle, dwPos);
}

//--------------------------------------------------

void set_text_and_back_color(int foreground_color, int background_color)
{
    WORD w_color = ((background_color & 0x0F) << 4) + (foreground_color & 0x0F);;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), w_color);
}

//==================================================

int generate_color()
{
    static int index;
    index = rand() % 6;
    return colors[index];
}

//--------------------------------------------------

int generate_color_without_color(int color)
{
    static int temporary_colors[5], index, n1, n2 = 0;
    for (n1 = 0; n1 < 6; n1++)
        if (colors[n1] != color)
        {
            temporary_colors[n2] = colors[n1];
            n2++;
        }
    n2 = 0;
    index = rand() % 5;
    return temporary_colors[index];
}

//==================================================

int generate_car_location()
{
    static int side;
    side = rand() % 2;
    return side;
}

//==================================================

char get_key_input()
{
    static char key;
    key = '\0';
    if (kbhit())
        if (getch() == 224)
            switch (getch())
            {
            case 72:
                key = 'U';
                break;
            case 80:
                key = 'D';
                break;
            case 75:
                key = 'L';
                break;
            case 77:
                key = 'R';
                break;
            }
    return key;
}

//--------------------------------------------------

void set_main_car_side()
{
    if (input == 'L')
        main_car_side = LEFT;
    else if (input == 'R')
        main_car_side = RIGHT;
}

//==================================================

void set_shape_to_bricks(int shape[], int color, int x, int y)
{
    static int x1, y1, count;
    count = 2;
    for (y1 = y; y1 < y + shape[1]; y1++)
        for (x1 = x; x1 < x + shape[0]; x1++)
        {
            if (shape[count] == 1)
                bricks[y1][x1] = color;
            count++;
        }
}

//--------------------------------------------------

void set_wall()
{
    static int count = 0;
    if (count > 3)
    {
        count = 0;
        set_shape_to_bricks(shape_wall, GREY, 0, 0);
        set_shape_to_bricks(shape_wall, GREY, 9, 0);
    }
    count++;
}

//--------------------------------------------------

void set_enemy_car()
{
    static int check_life = LIFE, wait = 25, count = 0, color = BLACK;
    if (total_life != check_life)
    {
        wait = 25;
        check_life = total_life;
    }
    if (wait == FALSE)
    {
        if (count > 8)
        {
            count = 0;
            color = generate_color_without_color(color);
            if (generate_car_location() == LEFT)
                set_shape_to_bricks(shape_car, color, 2, 0);
            else
                set_shape_to_bricks(shape_car, color, 5, 0);
        }
    }
    else
        wait--;
    count++;
}

//--------------------------------------------------

void set_main_car()
{
    if (main_car_side == LEFT)
        set_shape_to_bricks(shape_car, LIGHT_GREEN, 2, 20);
    else
        set_shape_to_bricks(shape_car, LIGHT_GREEN, 5, 20);
}

//--------------------------------------------------

void erase_main_car()
{
    if (main_car_side == LEFT)
        set_shape_to_bricks(shape_car, BLACK, 2, 20);
    else
        set_shape_to_bricks(shape_car, BLACK, 5, 20);
}

//--------------------------------------------------

void shift_bricks_y()
{
    static int x, y;
    for (y = 23; y > 0; y--)
        for (x = 0; x < 10; x++)
            bricks[y][x] = bricks[y - 1][x];
    for (x = 0; x < 10; x++)
        bricks[0][x] = 0;
}

//--------------------------------------------------

void clear_bricks()
{
    for (int y = 0; y < 24; y++)
        for (int x = 0; x < 10; x++)
            bricks[y][x] = 0;
}

//==================================================

void check_main_car_collision()
{
    static int x;
    if (main_car_side == LEFT)
        x = 3;
    else
        x = 6;
    if (bricks[20][x] != 0 || bricks[21][x] != 0 || bricks[22][x] != 0)
        main_car_collision_status = TRUE;
}

//==================================================

void update_score()
{
    static int x, count = 0;
    if (main_car_side == LEFT)
        x = 6;
    else
        x = 3;
    if (count == 3)
    {
        score++;
        count = 0;
    }
    else if (bricks[23][x] != 0)
        count++;
}

//--------------------------------------------------

void check_win_status()
{
    if (score == MAX_SCORE)
    {
        win_status = TRUE;
        current_life = FALSE;
        total_life = FALSE;
    }
}

//==================================================

void set_level()
{
    static int count = 0, level_fps_delay = DEFAULT_FPS_DELAY;
    if (level_fps_delay > 5)
        if (score % 25 == 0 && count != score / 25)
        {
            count = score / 25;
            level_fps_delay = level_fps_delay - 10;
        }
    fps_delay_as_level = level_fps_delay;
}

//==================================================

void play_burst_animation()
{
    static int x, step = 1;
    if (main_car_side == LEFT)
        x = 2;
    else
        x = 4;
    if (step % 2 != 0)
    {
        set_shape_to_bricks(shape_burst2, BLACK, x, 20);
        set_shape_to_bricks(shape_burst1, YELLOW, x, 20);
    }
    else
    {
        set_shape_to_bricks(shape_burst1, BLACK, x, 20);
        set_shape_to_bricks(shape_burst2, RED, x, 20);
    }
    if (step < 16)
        step++;
    else
    {
        step = 1;
        current_life = FALSE;
    }
}

//==================================================

void intro()
{
    set_text_and_back_color(LIGHT_GREEN, BLACK);
    printf("\n               BRICK GAME\n");
    printf("\n\n\n");
    printf("\n              === RACE ===\n");
    printf("\n\n");
    printf("\n                Welcome\n");
    printf("\n                  To\n");
    printf("\n               The Game\n");
    printf("\n\n");
    printf("\n       Controls : 'Left', 'Right'\n");
    printf("\n\n\n");
    printf("\n          By : Suprovo Basak.\n");
    printf("\n");
    printf("\n        Press Any Key To Play...");
}

//--------------------------------------------------

void outro()
{
    set_text_and_back_color(LIGHT_GREEN, BLACK);
    printf("\n               BRICK GAME\n");
    printf("\n\n\n");
    if (win_status == TRUE)
        printf("\n             *** WINNER ***\n");
    else
        printf("\n           --- GAME OVER ---\n");
    printf("\n\n");
    printf("\n               Thank You\n");
    printf("\n                  For\n");
    printf("\n              Playing Game\n");
    printf("\n\n");
    printf("\n           Final Score = %lu\n", score);
    printf("\n\n\n");
    printf("\n          By : Suprovo Basak.\n");
    printf("\n");
    printf("\n        Press Any Key To Exit...");
}

//==================================================

void render()
{
    static int x, y;
    reset_line_position();
    set_text_and_back_color(LIGHT_GREEN, BLACK);
    printf("\n               BRICK GAME\n");
    set_text_and_back_color(WHITE, BLACK);
    printf("\n         .====================.\n");
    for (y = 4; y < 24; y++)
    {
        printf("         |");
        for (x = 0; x < 10; x++)
        {
            set_text_and_back_color(bricks[y][x], BLACK);
            printf("[]");
        }
        set_text_and_back_color(WHITE, BLACK);
        printf("|\n");
    }
    printf("         '===================='\n");
    set_text_and_back_color(LIGHT_GREEN, BLACK);
    printf("\n         Life = %d, Score = %lu", total_life, score);
}

//====================================================================================================
//====================================================================================================

int main()
{
    SetConsoleTitle("C - Brick Game Race v1.0");
    set_window_size(WINDOW_X, WINDOW_Y);
    set_cursor_visibility(TRUE);
    srand(time(NULL));
    intro();
    getch();
    system("cls");
    while(total_life)
    {
        main_car_collision_status = FALSE;
        current_life = TRUE;
        while (current_life)
        {
            input = get_key_input();
            if (main_car_collision_status == FALSE)
            {
                set_wall();
                set_enemy_car();
                erase_main_car();
                set_main_car_side();
                shift_bricks_y();
            }
            check_main_car_collision();
            if (main_car_collision_status == FALSE)
            {
                update_score();
                check_win_status();
                set_main_car();
                set_level();
            }
            else
            {
                play_burst_animation();
                fps_delay_as_level = DEFAULT_FPS_DELAY;
            }
            set_window_size(WINDOW_X, WINDOW_Y);
            set_cursor_visibility(FALSE);
            render();
            Sleep(fps_delay_as_level);
        }
        clear_bricks();
        if (win_status == FALSE)
            total_life--;
    }
    system("cls");
    set_cursor_visibility(TRUE);
    outro();
    getch();
    return 0;
}

//====================================================================================================
//====================================================================================================
// The End.
//====================================================================================================
//====================================================================================================
