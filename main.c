#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>

#include "audio.h"
#include "console_draw3.h"
#include "font.h"
#include "kb_input2.h"
#include "image.h"

typedef struct {
    int x;
    int y;
    int fgcolor;
    int bgcolor;
    int cursor;
    int num_options;
    int state[15];
    char text[15][30]; /* 最多存十個字串  每個字串長度最多 79 個字元 */
    char alt_text[15][30]; /* 最多存十個字串  每個字串長度最多 79 個字元 */
    Font *large_font;
} Menu;

#define NUM_KEYS 9
#define REFRESH_RATE 10
#define OFFSET_X 10
#define OFFSET_Y 10
#define MAP_SIZE_R 5
#define MAP_SIZE_C 8
Font *large_font ;
Image * background;
Image * girl [5];
Audio audio[6];

int key_val[NUM_KEYS] = {VK_UP, VK_DOWN, VK_ESCAPE, VK_RETURN, VK_SPACE, VK_RIGHT, VK_LEFT,VK_L, VK_R};
int timer(void);
void setMainmenu(Menu *m);
int map[MAP_SIZE_R][MAP_SIZE_C] ;
void gameover(void);
void gamewin (void)
{
    Image *gamewin ;
    gamewin = read_image("win.pixel", "win.color");
    //playAudio( &audio[2]);
    clearScreen();
    show_image(gamewin, 0, 0);
    drawCmdWindow();
    Sleep(1500);

}
int alive_or_not(int *row,int col)
{
    int i;
    if ((*row) == MAP_SIZE_R) return 0;
    else {
        for(i=0;i+(*row)<MAP_SIZE_R;i++){
            if(map[(*row)+i][col]){
                    (*row)=(*row)+i;
                    return 1;
            }
        }
        return 0;
    }
}
void my_game_one(int * key_down)
{

    int alive, i, j, k, temp,row,col;
    int star_c, star_r;
    Image *star;
    star = read_image( "star.pixel","star.color");

    for ( i=0; i<MAP_SIZE_R; i++ ) {
        for ( j=0; j<MAP_SIZE_C; j++ )  map[i][j] = 0;
    }
    alive = 1;

    clearScreen();

    for ( i=0; i<4; i++ ) {
        for ( j=0; j<6; j++ )  map[i][j] = 1;
    }
    srand(time(NULL));
    for ( i=0; i< MAP_SIZE_R; i++ ){
        for ( j=0; j<MAP_SIZE_C; j++ ){
            row = rand()%MAP_SIZE_R;
            col = rand()%MAP_SIZE_C;
            temp = map [row][col];
            map[row][col] = map[i][j];
            map[i][j] = temp;

        }
    }

    star_c = rand()%MAP_SIZE_C;
    star_r = 0;
    col = 0;
    row = 4;

    map[star_r][star_c]=2;
    map[row][col]=3;

    FILE *hp;

    hp = fopen("out.txt","w");
    for ( i=0; i<MAP_SIZE_R; i++ ){
        for  ( j=0; j<MAP_SIZE_C; j++ ){
            fprintf( hp," %d ",map[i][j] );
        }
        fprintf(hp,"\n");
    }

    while (alive){
        clearScreen();

        for ( i=0; i<MAP_SIZE_R; i++ ) {
            for ( j=0; j<MAP_SIZE_C; j++ ) {
                if ( map[i][j] ){
                    for(k=0;k<20;k++){
                        putASCII2( j*40+k, i*15+15, '-', 9, 9 );
                        putASCII2( j*40+k, i*15+1+15, '-', 9, 9 );
                        putASCII2( j*40+k, i*15+2+15, '-', 9, 9 );
                    }
                }
            }
        }
        show_image(star, 40*star_c, 15*star_r);
        show_image(girl[3],40*col,15*row);


        drawCmdWindow();

        for (k=0; k<NUM_KEYS; k++) {

            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }


        for (k=0; k<NUM_KEYS; k++) {

            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    //playAudio(&audio[1]);
                    row --;
                    if ( row < 0 ) row = 0;
                    alive = alive_or_not(&row,col);
                    if (!alive) gameover();
                    break;
                case VK_DOWN:
                    row ++;
                    //playAudio(&audio[1]);
                    alive = alive_or_not(&row,col);
                    if (!alive) gameover();
                    break;
                case VK_LEFT:
                    col --;
                    //playAudio(&audio[1]);
                    if ( col <0 ) col =0;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();

                    break;
                case VK_RIGHT:
                    col ++;
                    //playAudio(&audio[1]);
                    if ( col == MAP_SIZE_C ) col = MAP_SIZE_C - 1;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();

                    break;
                case VK_L:
                    row --;
                    col --;
                    //playAudio(&audio[1]);
                    if ( col < 0 ) col = 0;
                    if ( row < 0 ) row = 0;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();
                    break;
                case VK_R:
                    col ++;
                    row --;
                    //playAudio(&audio[1]);
                    if ( col > MAP_SIZE_C ) col =MAP_SIZE_C - 1;
                    if ( row < 0 ) row = 0;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();
                    break;
               case VK_SPACE:
                    playAudio(&audio[3]);
                    show_image( girl[4],col, row );
                    drawCmdWindow();
                    if ( col == star_c && row == star_r){
                        destroy_image(star);
                        drawCmdWindow();
                        gamewin();
                        alive=0;
                    }

                    break;
                case VK_ESCAPE:
                    alive=0;
                    clearScreen();
                    break;
                }


            }
        }
    }


}

void my_game_two( int *key_down )
{
    int alive, i, j, k, temp,row,col;
    int star_c, star_r, t;
    Image *star;
    star = read_image( "star.pixel","star.color");

    for ( i=0; i<MAP_SIZE_R; i++ ) {
        for ( j=0; j<MAP_SIZE_C; j++ )  map[i][j] = 0;
    }
    alive = 1;

    clearScreen();

    for ( i=0; i<5; i++ ) {
        for ( j=0; j<4; j++ )  map[i][j] = 1;
    }
    srand(time(NULL));
    for ( i=0; i< MAP_SIZE_R; i++ ){
        for ( j=0; j<MAP_SIZE_C; j++ ){
            row = rand()%MAP_SIZE_R;
            col = rand()%MAP_SIZE_C;
            temp = map [row][col];
            map[row][col] = map[i][j];
            map[i][j] = temp;

        }
    }

    star_c = rand()%MAP_SIZE_C;
    star_r = 0;
    col = 0;
    row = 4;

    map[star_r][star_c]=2;
    map[row][col]=3;

    FILE *hp;

    hp = fopen("out.txt","w");
    for ( i=0; i<MAP_SIZE_R; i++ ){
        for  ( j=0; j<MAP_SIZE_C; j++ ){
            fprintf( hp," %d ",map[i][j] );
        }
        fprintf(hp,"\n");
    }
    t = 0;
    while (alive){
        clearScreen();
        if ( t < 500 ) {
            t++;
        }else t=0;
        if ( t == 0 ){
            star_r = rand()%MAP_SIZE_R;
            star_c = rand()%MAP_SIZE_C;
        }
        for ( i=0; i<MAP_SIZE_R; i++ ) {
            for ( j=0; j<MAP_SIZE_C; j++ ) {
                if ( map[i][j] ){
                    for(k=0;k<20;k++){
                        putASCII2( j*40+k, i*15+15, '-', 9, 9 );
                        putASCII2( j*40+k, i*15+1+15, '-', 9, 9 );
                        putASCII2( j*40+k, i*15+2+15, '-', 9, 9 );
                    }
                }
            }
        }


        show_image(star, 40*star_c, 15*star_r);
        show_image(girl[3],40*col,15*row);

        drawCmdWindow();
        if ( waitForKeyDown(0.01)) {
            for (k=0; k<NUM_KEYS; k++) {

            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }


      for (k=0; k<NUM_KEYS; k++) {

            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    row --;
                    //playAudio(&audio[1]);
                    if ( row < 0 ) row = 0;
                    alive = alive_or_not(&row,col);
                    if (!alive) gameover();
                    break;
                case VK_DOWN:
                    row ++;
                    //playAudio(&audio[1]);
                    alive = alive_or_not(&row,col);
                    if (!alive) gameover();
                    break;
                case VK_LEFT:
                    col --;
                    //playAudio(&audio[1]);
                    if ( col <0 ) col =0;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();

                    break;
                case VK_RIGHT:
                    col ++;
                    //playAudio(&audio[1]);
                    if ( col == MAP_SIZE_C ) col = MAP_SIZE_C - 1;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();

                    break;
                case VK_L:
                    row --;
                    col --;
                    //playAudio(&audio[1]);
                    if ( col < 0 ) col = 0;
                    if ( row < 0 ) row = 0;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();
                    break;
                case VK_R:
                    col ++;
                    row --;
                    //playAudio(&audio[1]);
                    if ( col == MAP_SIZE_C ) col = MAP_SIZE_C - 1;
                    if ( row < 0 ) row = 0;
                    alive = alive_or_not(&row,col);
                    if (!alive)gameover();
                    break;
               case VK_SPACE:
                    playAudio(&audio[3]);
                    show_image( girl[4],col,row );
                    drawCmdWindow();
                    if ( col == star_c && row == star_r){
                        destroy_image(star);
                        drawCmdWindow();
                        gamewin();
                        alive=0;
                    }

                    break;
                case VK_ESCAPE:
                    alive=0;
                    clearScreen();
                    break;
                }


            }
        }
    }



}
}
void gameover(void)
{
    int i;
    Image *gameover ;
    gameover = read_image("game-over.pixel", "game-over.color");
    clearScreen();
    show_image(gameover, 0, 0);
    drawCmdWindow();
    Sleep(1500);
    for ( i=0; i<=100; i++){
        show_image(girl[0], 100, i);
        show_image(girl[1],40,i*2);
        show_image(girl[2],20,i*6);
        show_image(girl[1],60,i*1.5);
        show_image(girl[1],80,i);
        show_image(girl[1],110,i*2);
        show_image(girl[1],130,i*5);
        show_image(girl[1],160,i*3);
        drawCmdWindow();
        Sleep(100);
    }

}
void show_guide()
{
    int isEnding = 0;
    large_font = read_font("font.txt");
    clearScreen();
    while ( isEnding == 0 ) {
        if ( KEY_DOWN ( VK_ESCAPE) ) isEnding = 1;
        putStringLarge(large_font, 4, 15, " control the direction of the girl ",14);
        putStringLarge(large_font, 4, 22," by the keys up down left L R and right ", 14);
        putStringLarge(large_font, 4, 29, " to catch the stars",14 );
        putStringLarge(large_font, 4, 36, " L to upper left R to upper right",14 );
        putStringLarge(large_font, 4, 50, " If she fell off the board ",14 );
        putStringLarge(large_font, 4, 57," and then game over",14);
        drawCmdWindow();
    }





}
int IsOnItem(Menu *m, int index)
{
    return (m->state[index] & 1); /* 用位元運算來判斷目前選擇的項目 */
}
int IsItemSelected(Menu *m, int index)
{
    return (m->state[index] & 2);
}
void showMenu(Menu *m)
{
    int i;
    int fg;
    char *str;

    for (i=0; i< m->num_options; i++) {

        if ( !IsOnItem(m, i) ) {
            fg = m->fgcolor;
        } else {
            fg = m->bgcolor;
        }

        if ( !IsItemSelected(m, i) ) {
            str = m->text[i];
        } else {
            str = m->alt_text[i];
        }

        putStringLarge(m->large_font, (m->x)*(m->large_font->width+2),
            (m->y+i)*(m->large_font->height+2), str, fg);
    }
}

void scrollMenu(Menu *m, int diff)
{
    m->state[m->cursor] = m->state[m->cursor] & (~1);  /* 把目前游標所在的選項狀態清除 */
    m->cursor = (m->cursor + diff + m->num_options) % m->num_options; /* 把遊標移到下一個選項 */
    m->state[m->cursor] = m->state[m->cursor] | 1; /* 選擇目前游標所在的選項 */
}

void radioMenu(Menu *m)
{
    int i;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = m->state[i] & (~2); /* 清掉全部的選項 */
    }
    m->state[m->cursor] = m->state[m->cursor] | 2; /* 設定目前游標 */
}
void toggleMenu(Menu *m)
{
    m->state[m->cursor] = m->state[m->cursor] ^ 2;  /* 利用位元運算 產生 toggle 的效果 */
}
void bye()
{
    Image * bye = read_image("bye.pixel","bye.color");
    clearScreen();
    show_image(bye, 45,15);
    drawCmdWindow();
    Sleep(1500);
}

int main ()
{
    int IsEnding = 0;
    int k,i;
    char str[40] = {'\0'};
    int key_down[NUM_KEYS] = {0};

    int cur_tick, last_tick;
    int flag=0;
    int choose = 0;
    large_font = read_font("font.txt");
    openAudioFile("1-st song.wav", &audio[0]);
    openAudioFile("mainmenu.wav", &audio[1]);
    openAudioFile("applause.wav", &audio[2]);
    openAudioFile("catch.wav", &audio[3]);


    background = read_image("a_star.pixel", "a_star.color");
    girl [0] = read_image("girl0.pixel","girl0.color");
    girl [1] = read_image("girlstand.pixel","girlstand.color");
    girl [2] = read_image("girlwalk.pixel","girlwalk.color");
    girl [3] = read_image("girlup.pixel","girlup.color");
    girl [4] = read_image("girldown.pixel","girldown.color");

    playAudio(&audio[0]);

    Menu mainmenu;

    /* 啟動鍵盤控制 整個程式中只要做一次就行了*/
    initializeKeyInput();
    cur_tick = last_tick = timer();

    setMainmenu(&mainmenu);

   // saveScreen();
    for ( i = 0; i <= 100; i += 5 ) {
        show_image(background, 0, 0);
        flag = ( flag + 1 ) % 2;
        if (flag == 0 ) show_image( girl[0], i%100, 50 );
        else show_image( girl [2], i%100, 50 );
        saveScreen();
        drawCmdWindow();
        Sleep(200);
        clearScreen();
    }
    while (!IsEnding) {
        show_image(background, 0, 0);
        show_image( girl [0], 100, 50 );


        /* 每經過 REFRESH_RATE 個 ticks 才會更新一次畫面 */
        cur_tick = timer(); /* 每個 tick 0.01 秒 */
        sprintf(str, "%10d", cur_tick/1000);

        if (cur_tick-last_tick > REFRESH_RATE) {

            last_tick = cur_tick;
            clearScreen();
            restoreScreen();
            //putStringLarge(mainmenu.large_font, OFFSET_X, OFFSET_Y-1, str, 14);

            /* 把選單畫出來 */
            showMenu(&mainmenu);

            /* 為了要讓一連串 putASCII2() 的動作產生效果
               必須要呼叫一次 drawCmdWindow() 把之前畫的全部內容一次顯示到螢幕上 */
            drawCmdWindow();

        } /* end of if (cur_tick % REFRESH_RATE == 0 ... */

        for (k=0; k<NUM_KEYS; k++) {
            /* 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況 */
            if(KEY_UP(key_val[k]) && key_down[k]) {
                key_down[k] = 0;
            }
        }

        /* 鍵盤控制 處理按著不放的狀況 */
        for (k=0; k<NUM_KEYS; k++) {
            /* 按鍵從原本被按下的狀態 變成放開的狀態  這是為了處理按著不放的情況 */
            if(KEY_DOWN(key_val[k]) && !key_down[k]) {
                key_down[k] = 1;

                switch (key_val[k]) {
                case VK_UP:
                    scrollMenu(&mainmenu, -1);
                    choose = ( choose + 3 ) % 4;
                    playAudio(&audio[1]);
                    break;
                case VK_DOWN:
                    scrollMenu(&mainmenu, +1);
                    choose = ( choose + 1 ) % 4;
                    playAudio(&audio[1]);
                    break;

                case VK_RETURN:
                      if ( choose == 0 ) {
                            my_game_one(key_down);
                            clearScreen();
                      }
                      else if ( choose == 1 ) {
                            my_game_two(key_down);
                            clearScreen();
                      }
                      else if ( choose == 2 ) {
                            show_guide();
                      }
                      else {
                            IsEnding = 1 ;
                            bye();
                        }

                    break;
                }

            }
        }
        clearScreen();
    } /* while (IsEnding) */


    return 0;
}

int timer(void)
{
    return (clock()/(0.001*CLOCKS_PER_SEC));
}
void setMainmenu(Menu *m)
{
    int i;

    m->large_font = read_font("font.txt");
    m->x = 2;
    m->y = 2;
    m->fgcolor = 7;
    m->bgcolor = 15;
    m->num_options = 4;
    m->cursor = 0;
    for (i=0; i<m->num_options; i++) {
        m->state[i] = 0;
    }
    m->state[m->cursor] = m->state[m->cursor] | 1;  /* 目前選擇的項目 */

    strcpy(m->text[0], "FIRST LEVEL");
    strcpy(m->text[1], "SECOND LEVEL");
    strcpy(m->text[2], "GUIDE");
    strcpy(m->text[3], "EXIT");


    strcpy(m->alt_text[0], "FIRST LEVEL*");
    strcpy(m->alt_text[1], "SECOND LEVEL*");
    strcpy(m->alt_text[2], "GUIDE *");
    strcpy(m->alt_text[3], "EXIT *");

}
