#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


#define RIGHT 0
#define DOWN  1
#define LEFT  2
#define UP    3

#define NO_PRESS 4

static uint8_t grid[96][65] = {0};

void fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool clear)
{
    uint8_t i,j;
    for(i = 0; i<w; i++)
    {
        for(j = 0; j<h; j++)
        {
            if(clear)
            {
                grid[x+i][y+j] = 0;
            }
            else
            {
                grid[x+i][y+j] = 1;
            }
        }
    }
}

void printGrid(void)
{
    uint8_t i,j;
    for(i = 0; i<65; i++)
    {
        for(j = 0; j<96; j++)
        {
            if(grid[j][i] == 0)
            {
                printf(".");
            }
            else
            {
                printf("#");
            }
        }
        printf("\r\n");
    }
}

void setTimeout(void* function, uint32_t time)
{
    
}
void resetGame(void);
void mainLoop(void);
void gameOver(void);
void endGame(void);
void reduceScore(void);
void drawHead(uint8_t direction, uint8_t x, uint8_t y);
void drawBody(uint8_t direction, uint8_t x, uint8_t y, bool ate, uint8_t headDirection);
void drawTail(uint8_t direction, uint8_t x, uint8_t y);

const uint8_t bodyPattern[4] = {0b0000,
                                0b1101,
                                0b1011,
                                0b0000};
                   
const uint8_t emptyPattern[4] = {};
                   
const uint8_t foodPattern[4] = {0b0100,
                                0b1010,
                                0b0100,
                                0b0000};
                  
const uint8_t headPattern[4] = {0b1000,
                                0b0110,
                                0b1110,
                                0b0000};
                  
const uint8_t eatingPattern[4] = {0b1010,
                     0b0100,
                     0b1100,
                     0b0010};
                    
const uint8_t cornerPattern[4] = {0b0110,
                     0b1010,
                     0b1100,
                     0b0000};
const uint8_t cornerPattern2[4] = {0b0000,
                      0b1100,
                      0b1010,
                      0b0110};
                    
const uint8_t cornerEatenPattern[4] = {0b1110,
                          0b1010,
                          0b1100,
                          0b0000};
const uint8_t cornerEatenPattern2[4] = {0b0000,
                           0b1100,
                           0b1010,
                           0b1110};
                  
const uint8_t eatenPattern[4] = {0b0110,
                    0b1101,
                    0b1011,
                    0b0110};

const uint8_t tailPattern[4] = {0b0000,
                   0b0011,
                   0b1111,
                   0b0000};
                  
const uint8_t digits[10][5] = {
{0b111,
 0b101,
 0b101,
 0b101,
 0b111},

{0b010,
 0b110,
 0b010,
 0b010,
 0b010},

{0b111,
 0b001,
 0b111,
 0b100,
 0b111},

{0b111,
 0b001,
 0b111,
 0b001,
 0b111},

{0b101,
 0b101,
 0b111,
 0b001,
 0b001},

{0b111,
 0b100,
 0b111,
 0b001,
 0b111},

{0b111,
 0b100,
 0b111,
 0b101,
 0b111},

{0b111,
 0b001,
 0b010,
 0b010,
 0b010},

{0b111,
 0b101,
 0b111,
 0b101,
 0b111},

{0b111,
 0b101,
 0b111,
 0b001,
 0b111}
};

const char* bigGameOver = " ########   ########   ##### #####   ########    ########  ###    ###  ######## ######### "
                          "########## ########## ############# #########   ########## ###    ### ######### ##########"
                          "###    ### ###    ### ############# ###         ###    ### ###    ### ###       ###    ###"
                          "###        ###    ### ###  ###  ### ###         ###    ### ###    ### ###       ###    ###"
                          "###  ##### ########## ###  ###  ### ########    ###    ### ###    ### ########  ######### "
                          "###  ##### ########## ###  ###  ### ########    ###    ### ###   #### ########  ######### "
                          "###    ### ###    ### ###  ###  ### ###         ###    ### #### ####  ###       ###    ###"
                          "###    ### ###    ### ###  ###  ### ###         ###    ###  #######   ###       ###    ###"
                          "########## ###    ### ###  ###  ### #########   ##########   #####    ######### ###    ###"
                          " ########  ###    ### ###  ###  ###  ########    ########     ###      ######## ###    ###";

void drawScore(uint16_t score)
{
     // Clear the score
    
    fillRect(0, 0, 16, 5, true);
    
    //Print each digit
    
    for(uint8_t i = 0; i < 4; i++)
    {
    	uint8_t digit = (score/pow(10,3-i));
    	score = score - (digit * pow(10,3-i));
        const uint8_t* pattern = digits[digit];
        for (uint8_t j = 0; j < 15; j++)
        {
            const uint8_t c = (pattern[j/3] >> j%3) & 1;
            if(c)
            {
                fillRect(1 + (i*4) + (2-(j%3)), (j/3), 1, 1, false);
            }
        }
    }
}            
             
void drawGrid(const uint8_t* pattern, uint8_t x, uint8_t y, uint8_t direction)
{
    // Clear the square
    
    fillRect(2 + (x*4), 11 + (y*4), 4, 4, true);
    
    // Print the pattern
    
    for (uint8_t i = 0; i < 16; i++)
    {
        uint8_t c = (pattern[i/4] >> (i%4)) & 1;
        if(c)
        {
            switch(direction){
            case 0:
                fillRect(2 + (x*4) + (3 - (i%4)), 11 + (y*4) + (i/4), 1, 1, false);
                break;
            case 1:
                fillRect(2 + (x*4) + (i/4), 11 + (y*4) + (3 - (i%4)), 1, 1, false);
                break;
            case 2:
                fillRect(2 + (x*4) + (i%4), 11 + (y*4) + (i/4), 1, 1, false);
                break;
            case 3:
                fillRect(2 + (x*4) + (i/4), 11 + (y*4) + (i%4), 1, 1, false);
                break;
            }
        }
    }
}

uint8_t lastPressedKey = NO_PRESS;
bool waitingToStart = false;

void keyListener(uint8_t code)
{
    switch(code) {
    case 37:
        lastPressedKey = LEFT;
        break;
    case 38:
        lastPressedKey = UP;
        break;
    case 39:
        lastPressedKey = RIGHT;
        break;
    case 40:
        lastPressedKey = DOWN;
        break;
    }
    if(waitingToStart)
    {
        resetGame();
    }
}

uint8_t direction;
uint8_t tailIndex;
uint8_t headIndex;
uint8_t snakeX[23*13];
uint8_t snakeY[23*13];
uint8_t snakeDir[23*13];
uint8_t bitmap[23*13];
uint16_t emptyCells;
uint8_t flashes;

uint8_t getBitmap(uint8_t x, uint8_t y)
{
    return bitmap[(y * 23) + x];
}
void setBitmap(uint8_t variable, uint8_t x, uint8_t y)
{
    bitmap[(y * 23) + x] = variable;
}

uint8_t foodX;
uint8_t foodY;
uint8_t ateFood;

void setFood(void)
{
    time_t t;
    srand((unsigned) time(&t));
    uint16_t searchNum = rand() /(RAND_MAX/emptyCells);
    uint16_t i = 0;
    while(searchNum--)
    {
        i++;
        while(bitmap[i] == 1)
        {
            i++;
        }
    }
    foodX = i % 23;
    foodY = (i / 23);
    drawGrid(foodPattern, foodX, foodY, 0);
    setBitmap(foodX, foodY, 1);
    emptyCells--;
}

uint16_t score;

int main()
{
    printf("Resetting game\r\n");
    resetGame();
    printGrid();
}
void resetGame(void)
{
    
    fillRect(0, 9, 96, 56, false);
    fillRect(0, 6, 96, 1, false);

    
    fillRect(1, 10, 94, 54, true);

    direction = RIGHT;

    tailIndex = 0;
    headIndex = 5;
    
    //Reset snakeX, snakeY, snakeDir

    flashes = 8;
    waitingToStart = false;

    emptyCells = 23*13;
    lastPressedKey = UP;
    snakeX[0] = 5;
    snakeX[1] = 6;
    snakeX[2] = 7;
    snakeX[3] = 8;
    snakeX[4] = 9;
    snakeX[5] = 10;

    snakeY[0] = 6;
    snakeY[1] = 6;
    snakeY[2] = 6;
    snakeY[3] = 6;
    snakeY[4] = 6;
    snakeY[5] = 6;
    drawTail(0, snakeX[tailIndex], snakeY[tailIndex]);
    emptyCells--;
    setBitmap(1, snakeX[tailIndex], snakeY[tailIndex]);
    for(uint8_t i = tailIndex+1; i < headIndex; i++)
    {
        drawBody(0, snakeX[i], snakeY[i], false, 0);
        emptyCells--;
        setBitmap(1, snakeX[i], snakeY[i]);
    }
    drawHead(0, snakeX[headIndex], snakeY[headIndex]);
    emptyCells--;
    setBitmap(1, snakeX[headIndex], snakeY[headIndex]);
    ateFood = false;
    setFood();
    score = 0;
    drawScore(score);
    mainLoop();
}
void mainLoop(void)
{
    uint8_t keepRunning = true;
    uint8_t newHeadIndex = (headIndex + 1) % 300;
    // Get any new direction
    if(lastPressedKey == DOWN && direction != UP)
    {
        direction = DOWN;
    }
    else if(lastPressedKey == UP && direction != DOWN)
    {
        direction = UP;
    }
    else if(lastPressedKey == RIGHT && direction != LEFT)
    {
        direction = RIGHT;
    }
    else if(lastPressedKey == LEFT && direction != RIGHT)
    {
        direction = LEFT;
    }
    lastPressedKey = NO_PRESS;
    
    if(direction == DOWN)
    {
        snakeY[newHeadIndex] = (snakeY[headIndex] + 1) % 13;
        snakeX[newHeadIndex] = snakeX[headIndex];
        snakeDir[newHeadIndex] = 1;
    }
    else if(direction == UP)
    {
        snakeY[newHeadIndex] = snakeY[headIndex] - 1;
        if(snakeY[newHeadIndex] == 255)
        {
            snakeY[newHeadIndex] += 13;
        }
        snakeX[newHeadIndex] = snakeX[headIndex];
        snakeDir[newHeadIndex] = 3;
    }
    else if(direction == RIGHT)
    {
        snakeX[newHeadIndex] = (snakeX[headIndex] + 1) % 23;
        snakeY[newHeadIndex] = snakeY[headIndex];
        snakeDir[newHeadIndex] = 0;
    }
    else if(direction == LEFT)
    {
        snakeX[newHeadIndex] = snakeX[headIndex] - 1;
        if(snakeX[newHeadIndex] == 255)
        {
            snakeX[newHeadIndex] += 23;
        }
        snakeY[newHeadIndex] = snakeY[headIndex];
        snakeDir[newHeadIndex] = 2;
    }
    
    setBitmap(0, snakeX[tailIndex], snakeY[tailIndex]);
    uint8_t oldTailIndex = tailIndex;
    uint8_t oldAteFood = ateFood;
    if(snakeX[newHeadIndex] == foodX && snakeY[newHeadIndex] == foodY)
    {
        drawGrid(emptyPattern, snakeX[oldTailIndex], snakeY[oldTailIndex], 0);
        setBitmap(1, snakeX[newHeadIndex], snakeY[newHeadIndex]);
        setFood();
        ateFood = true;
        score++;
		drawScore(score);
    }
    else if(getBitmap(snakeX[newHeadIndex], snakeY[newHeadIndex]) != 0)
    {
        endGame();
        keepRunning = false;
    }
    else
    {
        drawGrid(emptyPattern, snakeX[oldTailIndex], snakeY[oldTailIndex], 0);
        ateFood = false;
        setBitmap(1, snakeX[newHeadIndex], snakeY[newHeadIndex]);
        tailIndex = (tailIndex + 1) % 300;
    }
    if(keepRunning)
    {
        drawTail(snakeDir[tailIndex], snakeX[tailIndex], snakeY[tailIndex]);
        drawBody(snakeDir[headIndex], snakeX[headIndex], snakeY[headIndex], oldAteFood, snakeDir[newHeadIndex]);
        drawHead(snakeDir[newHeadIndex], snakeX[newHeadIndex], snakeY[newHeadIndex]);
        snakeDir[headIndex] = snakeDir[newHeadIndex];
        headIndex = newHeadIndex;
        setTimeout(mainLoop, 480);
    }
}
void endGame(void)
{
    flashes--;
    if(flashes % 2)
    {
        
        fillRect(0, 0, 16, 5, true);
    }
    else
    {
        drawScore(score);
    }
    
    if(flashes > 0)
    {
        setTimeout(endGame, 500);
    }
    else
    {
        reduceScore();
    }
}

void reduceScore(void)
{
    score--;
    drawScore(score);
    if(score == 0)
    {
        gameOver();
    }
    else
    {
        setTimeout(reduceScore, 100);
    }
}

void gameOver(void)
{
    
    fillRect(2, 31, 92, 12, true);
    
    //Print game over
    
    for (uint8_t i = 0; i < sizeof(bigGameOver); i++)
    {
        uint8_t c = bigGameOver[i];
        if(c == '#')
        {
            fillRect(3 + i%90, 32 + (i/90), 1, 1, false);
        }
    }
    waitingToStart = true;
}
void drawHead(uint8_t direction, uint8_t x, uint8_t y)
{
	uint8_t eating = false;
	switch(direction){
    case 0:
    	if(foodX == x+1 && y == foodY)
        {
        	eating = true;
        }
    	break;
    case 1:
    	if(foodX == x && y+1 == foodY)
        {
        	eating = true;
        }
    	break;
    case 2:
    	if(foodX == x-1 && y == foodY)
        {
        	eating = true;
        }
    	break;
    case 3:
    	if(foodX == x && y-1 == foodY)
        {
        	eating = true;
        }
    	break;
    }
    if(eating)
    {
    	drawGrid(eatingPattern, x, y, direction);
    }
    else
    {
    	drawGrid(headPattern, x, y, direction);
    }
}
void drawBody(uint8_t direction, uint8_t x, uint8_t y, bool ateFood, uint8_t headDirection)
{
    if(ateFood)
    {
        if(direction == headDirection)
        {
            drawGrid(eatenPattern, x, y, direction);
        }
        else if(headDirection < 2)
        {
            drawGrid(cornerEatenPattern2, x, y, direction);
        }
        else
        {
            drawGrid(cornerEatenPattern, x, y, direction);
        }
    }
    else
    {
        if(direction == headDirection)
        {
            drawGrid(bodyPattern, x, y, direction);
        }
        else if(headDirection < 2)
        {
            drawGrid(cornerPattern2, x, y, direction);
        }
        else
        {
            drawGrid(cornerPattern, x, y, direction);
        }
    }
}
void drawTail(uint8_t direction, uint8_t x, uint8_t y)
{
    drawGrid(tailPattern, x, y, direction);
}
