/*Brici Project by Franz and Ella
C22522443 and C22396101

date: 09/03/2023*/

#include <stm32f031x6.h> 
#include "display.h"
#include "musical_notes.h"

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
volatile uint32_t milliseconds;
void initTimer(void);
void playNote(uint32_t Freq,uint32_t duration);
void playTune(uint32_t tune[],uint32_t time, uint32_t count);

//Game start tune
uint32_t myTune[]={322, 424, 526, 628};

//Lose ball tune
uint32_t myTune2[] = {628,322};

//Game over tune
uint32_t myTune3[] = {628, 526, 424, 322};
uint32_t myTime[]={100, 100, 100, 200};

//serialisation
void SerialBegin(void);
void eputchar(char);
char egetchar(void);
void eputs(char *String);

//Pause game function
void paused();

//difficulty detection
void easy(void);
void medium(void);
void hard(void);

//custom gamemodes
void blocked(void);
void noMiss(void);
void slimBat(void);
void reset(void);

//width and length of bat
int bat_W = 10;
int bat_H = 3;

//changed from symbolic name to global variable as it will change
int BAT_WIDTH = 10;

//detect if a key is pressed
int keyPressed(void);


#define BW 16
#define BH 9

#define MAX_BRICI_LEVELS 4

uint32_t ball_x;
uint32_t ball_y;
uint32_t bat_x;
uint32_t bat_y;

//Pause function global variable
int pause = 0;



typedef struct  {
      uint16_t colour;
      uint16_t x;
      uint16_t y; 
      uint16_t visible;
} block_t;

#define YELLBLK 0x1f0e
#define GREENBLK 0x001f
#define BLUEBLK 0x00f8
#define NEWBLK1 0xff09
#define NEWBLK2 0xe9f0
#define NEWBLK3 0xf040
#define NEWBLK4 0x00f7
#define NEWBLK5 0xf0f0
#define BLOCKCOUNT 64

block_t Blocks[BLOCKCOUNT] = {
{ YELLBLK, 0, 18, 0 }, { YELLBLK, 16, 18, 0 }, { YELLBLK, 32, 18, 0 }, { YELLBLK, 48, 18, 0 }, { YELLBLK, 64, 18, 0 }, { YELLBLK, 80, 18, 0}, { YELLBLK, 96, 18, 0 }, { YELLBLK, 112, 18, 0 }, 
{ GREENBLK, 0, 27, 0 }, { GREENBLK, 16, 27, 0 }, { GREENBLK, 32, 27, 0 }, { GREENBLK, 48, 27, 0 }, { GREENBLK, 64, 27, 0 }, { GREENBLK, 80, 27, 0 }, { GREENBLK, 96, 27, 0 }, { GREENBLK, 112, 27, 0 },
{ BLUEBLK, 0, 36, 0 }, { BLUEBLK, 16, 36, 0 }, { BLUEBLK, 32, 36, 0 }, { BLUEBLK, 48, 36, 0 }, { BLUEBLK, 64, 36, 0}, { BLUEBLK, 80, 36, 0 }, { BLUEBLK, 96, 36, 0 }, { BLUEBLK, 112, 36, 0 }, 
{ NEWBLK1, 0, 45, 0}, { NEWBLK1, 16, 45, 0}, { NEWBLK1, 32, 45, 0}, { NEWBLK1, 48, 45, 0}, { NEWBLK1, 64, 45, 0}, { NEWBLK1, 80, 45, 0}, { NEWBLK1, 96, 45, 0}, { NEWBLK1, 112, 45, 0},
{ NEWBLK2, 0, 54, 0}, { NEWBLK2, 16, 54, 0}, { NEWBLK2, 32, 54, 0}, { NEWBLK2, 48, 54, 0}, { NEWBLK2, 64, 54, 0}, { NEWBLK2, 80, 54, 0}, { NEWBLK2, 96, 54, 0}, { NEWBLK2, 112, 54, 0},
{ NEWBLK3, 0, 63, 0}, { NEWBLK3, 16, 63, 0}, { NEWBLK3, 32, 63, 0}, { NEWBLK3, 48, 63, 0}, { NEWBLK3, 64, 63, 0}, { NEWBLK3, 80, 63, 0}, { NEWBLK3, 96, 63, 0}, { NEWBLK3, 112, 63, 0},
{ NEWBLK4, 0, 72, 0}, { NEWBLK4, 16, 72, 0}, { NEWBLK4, 32, 72, 0}, { NEWBLK4, 48, 72, 0}, { NEWBLK4, 64, 72, 0}, { NEWBLK4, 80, 72, 0}, { NEWBLK4, 96, 72, 0}, { NEWBLK4, 112, 72, 0},
{ NEWBLK5, 0, 81, 0}, { NEWBLK5, 16, 81, 0}, { NEWBLK5, 32, 81, 0}, { NEWBLK5, 48, 81, 0}, { NEWBLK5, 64, 81, 0}, { NEWBLK5, 80, 81, 0}, { NEWBLK5, 96, 81, 0}, { NEWBLK5, 112, 81, 0},
};
  
void hideBlock(uint32_t index);
void showBlock(uint32_t index);
void hideBall(void);
void showBall(void);
void moveBall(uint32_t newX, uint32_t newY);
void hideBat(void);
void showBat(void);
void moveBat(uint32_t newX, uint32_t newY);
int blockTouching(int Index,uint16_t ball_x,uint16_t ball_y);
int UpPressed(void);
int DownPressed(void);
int LeftPressed(void);
int RightPressed(void);
void randomize(void);
uint32_t random(uint32_t lower, uint32_t upper);
void playBrici(void);

//Variable for game speed
int speed = 5;

//Shows a certain amount of blocks
int blocks = 64;

//Determines base difficulty
int difficulty = 0;

//Toggles choosing from regular or custom modes
int custom = 0;

//Getting character input
char key = ' ';

//Now global because it will be changed
unsigned int BallCount = 5;

//Player score
int score = 0;

int main()
{
  initClock();
  RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
  initSysTick();
  initTimer();
  display_begin();
  //invertDisplay();// Uncomment this line if you have a display with a blue PCB
  pinMode(GPIOB,0,1);
  pinMode(GPIOA,0,1);
  pinMode(GPIOA,2,1);
  pinMode(GPIOB,4,0);
  pinMode(GPIOB,5,0);
  pinMode(GPIOA,8,0);
  pinMode(GPIOA,1,1);
  pinMode(GPIOA,12,1);
  enablePullUp(GPIOB,0);
  enablePullUp(GPIOB,4);
  enablePullUp(GPIOB,5);
  enablePullUp(GPIOA,11);
  enablePullUp(GPIOA,8);
  enablePullUp(GPIOA,8);
  enablePullUp(GPIOA,12);
      
  while(1)
  {
    playBrici();
  }
}
void playBrici()
{
  SerialBegin();
  int Level = MAX_BRICI_LEVELS;
  int LevelComplete = 0;
  unsigned int Index;
  int32_t BallXVelocity = 1;
  int32_t BallYVelocity = 1;
  // Blank the screen to start with
  fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  bat_x =20;
  bat_y = SCREEN_HEIGHT - 20; 
    
  printText("Brici", 10, 40, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  printText("Press Left = play", 10, 50, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  printText("up/down = difficulty:", 10, 60, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  printText("Easy", 10, 70, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  printText("Normal", 10, 80, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  printText("Hard", 10, 90, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
      
	
	playTune(myTune, myTime[0], 4);
	
  //default difficulty
  difficulty = 1;
      
      
  eputs("________  ________  ________  ___               ________  ________  _____ ______   _______\n\r");
  eputs("|\\   ____\\|\\   __  \\|\\   __  \\|\\  \\             |\\   ____\\|\\   __  \\|\\   _ \\  _   \\|\\  ___ \\    \n\r");
  eputs("\\ \\  \\___|\\ \\  \\|\\  \\ \\  \\|\\  \\ \\  \\            \\ \\  \\___|\\ \\  \\|\\  \\ \\  \\\\\\__\\ \\  \\ \\   __/|\n\r");
  eputs(" \\ \\  \\    \\ \\  \\\\ \\ \\ \\  \\\\\\  \\ \\  \\            \\ \\  \\  __\\ \\   __  \\ \\  \\\\|__| \\  \\ \\  \\_|/__\n\r");
  eputs("  \\ \\  \\____\\ \\  \\\\\\  \\ \\  \\\\\\  \\ \\  \\____        \\ \\  \\|\\  \\ \\  \\ \\  \\ \\  \\    \\ \\  \\ \\  \\_|\\ \\ \n\r");
  eputs("   \\ \\_______\\ \\_______\\ \\_______\\ \\_______\\       \\ \\_______\\ \\__\\ \\__\\ \\__\\    \\ \\__\\ \\_______\\      \n\r");
  eputs("    \\|_______|\\|_______|\\|_______|\\|_______|        \\|_______|\\|__|\\|__|\\|__|     \\|__|\\|_______|\n\r");
  eputs("\n\rControls:\n\r");
  eputs("\rA/D: move bat\n\r");
  eputs("\n\rAlternate Modes:\n\r");
  eputs("\nP to play\n\r");
  eputs("\nW/S to choose difficulty\n\r");
  eputs("\rPress C for custom modes\n\r");
      
      
      
  // Treat the while loop as the menu
  while(!LeftPressed() && key != 'p')
  {
    //Do egecthar() ONLY when a key is pressed
    if (keyPressed())
    {
      key = egetchar();
                  
    } //end IF
            
    //Prevent key from staying on one input forever
    else
    {
      key = ' ';
                  
    } //end ELSE
            
		//User is now selecting from custom game modes
    if(key == 'c')
    {
      eputs("\rCustom modes:\n\r");
      eputs("\rB = Blocked up, N = No missses,  V = Verticle Bat\n\r");
      eputs("\rPress R to go back to regular difficulty settings\n\r");
      reset();
      custom = 1;
                  
    } //end if
            
    //User is now selecting from regular difficulty modes
    else if (key == 'r')
    {
      eputs("\nW/S to choose difficulty\n\r");
      eputs("\rPress C for custom modes\n\r");
      reset();
      custom = 0 ;
                  
    } //end if
            
    //Only allow to change if not choosing custom modes
    if(custom == 0)
    {
    //move difficulty choice up
    if ( UpPressed() || key == 'w')
    {
      difficulty--;
      playNote(1,20);
                        
    } //end IF
                  //Stop continuous input
                  while (UpPressed());
                  
   //move difficulty choice down
   if ( DownPressed() ||key == 's')
   {
     difficulty++;
     playNote(1,20);
                        
   } //end IF
                  //Stop continuous input
                  while (DownPressed());
                  
   //Switch statement for difficulty choice menu
   switch (difficulty)
   {     
    //Allow choice to wrap around 
     case -1:
     {
       difficulty = 2;
       hard();
       break;
                              
     } //end case -1
                        
     //difficulty = 0, easy 
     case 0:
     {
       easy();
       break;
                              
     } //end case 0
                        
     //difficulty = 1, normal
     case 1:
     {
       medium();
       break;
                              
     }// end case 1
                        
     //difficulty = 2, hard
     case 2:
     {
       hard();
       break;
                              
     } //end case 2
                        
     //Allow choice to wrap around 
     case 3:
     {
       difficulty = 0;
       easy();
                              
     } //end case 3
                        
   } //end SWITCH
                  
  }//end IF
            
  //Only allow these options of user chooses custom modes
  if(custom == 1)
  {     
  //Turn off LEDS to avoid confusion of difficulty
     GPIOA->ODR &= ~(1<<12);
     GPIOA->ODR &= ~(1<<0);
     GPIOA->ODR &= ~(1<<1);
     switch(key)
     {     
       //User chooses the "Blocked" gamemode
       case 'b':
       {
         eputs("\rBlocked!\n\r");
         blocked();
		     break;
       } //end case 'b'
                              
       //User chooses the "No Miss" gamemode
       case 'n':
       {
         eputs("\rDont Miss!\n\r");
         noMiss();
			   break;
       } //end case 'n'
                              
       //user chooses the "Verticle Bat" option
       case 'v':
       {
         slimBat();
         eputs("\rVerticle Bat!\n\r");
         break;
       } //end case 'v'
                              
     } //end SWITCH
                  
    } //end IF
            
   } //end WHILE
   randomize();  
   ball_x = random(0,SCREEN_WIDTH);
  ball_y = random(50,(bat_y - 10));       
      // draw the red squares indicating the number of remaining lives.
      for (Index = BallCount; Index > 0; Index--)
            fillRectangle(SCREEN_WIDTH - Index * 8, SCREEN_HEIGHT-10, 7, 7, RGBToWord(0xff, 0xf, 0xf));
  while (Level > 0)
  {
    moveBall(random(10, SCREEN_WIDTH-10), 100);
    if (random(0,2) > 0) // "flip a coin" to choose ball x velocity
      BallXVelocity = 1;
    else
      BallXVelocity = -1;
    LevelComplete = 0;
    BallYVelocity = -1;  // initial ball motion is up the screen
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0); // clear the screen
            // draw the blocks.
    for (Index=0;Index<blocks;Index++)
    {
      showBlock(Index);
    }
    showBall();
    showBat();
    printText("Level", 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    printNumber(MAX_BRICI_LEVELS - Level + 1, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
            
    while (!LevelComplete)
    {       
     //For controls WASD
     //Only while the key is held down
       if (keyPressed())
       {
         key = egetchar();
                        
       } //end IF
                  
       // reset key when nothing is pressed
       else
       {
          key = ' ';
                        
       } //end ELSE
            
       //Pause menu 
       if (DownPressed() ||key == 's')
       {
         paused();
         fillRectangle(15, 90, SCREEN_WIDTH, 20, 0);
                        
         //Toggle button as to avoid continuous input
         while (DownPressed()); 
                        
       } //end IF
                  
       //Pauses the game and displays menu
       if (pause == 1){
          printText("Paused", 50, 90, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
          printText("Down to unpause", 15, 100, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
          continue;
                        
       } //end IF
                  
      
     //if right button or 'd' pressed
      if (RightPressed() || key == 'd')
      {
        // Move right
        if (bat_x < (SCREEN_WIDTH - BAT_WIDTH))
        {
          moveBat(bat_x + 2, bat_y); // Move the bat faster than the ball
                              
        } //end inner if
                        
      } //end outer if
                  
                  //if left button or 'a' pressed
      if (LeftPressed() || key == 'a')
      {
        // Move left
        if (bat_x > 0)
        {
          moveBat(bat_x - 2, bat_y); // Move the bat faster than the ball
        } //end inner if
                        
      } //end outer if
                  
      if ((ball_y == bat_y) && (ball_x >= bat_x) && (ball_x <= bat_x + BAT_WIDTH))
      {
       //Play note if ball touches the bat
        playNote(1,20);
        BallYVelocity = -BallYVelocity;
      }
      showBat(); // redraw bat as it might have lost a pixel due to collisions

      moveBall(ball_x+BallXVelocity,ball_y+BallYVelocity);
      
      if (ball_x == 2)
      {
        playNote(1,20);
        BallXVelocity = -BallXVelocity;
      }
       
      if (ball_x == SCREEN_WIDTH - 2)
      {
        playNote(1,20);
        BallXVelocity = -BallXVelocity;
      }
      if (ball_y == 2)
      {
        playNote(1,20);
        BallYVelocity = -BallYVelocity;
      }
                  

      if (ball_y >= bat_y+3)  // has the ball pass the bat vertically?
      {
				playTune(myTune2,myTime[0],2);
        BallCount--;
        if (BallCount == 0)
        {
          fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
					playTune(myTune3,myTime[0],4);
          printText("GAME OVER", 40, 100, RGBToWord(0xff, 0xff, 0xff), 0);
          printText("Up to restart", 18, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
          eputs("\nLooks like someone lost.\n\r");
          //Display their score
          eputs("\n This is your final score in 0s\n\r");
          for(int i = 0; i< score; i++)
				  {
						eputs("\n0\n\r");
					}
                              
          while (!UpPressed());
          return;
        }
        if (random(0,100) & 1)
          BallXVelocity = 1;
        else
          BallXVelocity = -1;

        BallYVelocity = -1;
        moveBall(random(10, SCREEN_WIDTH - 10), random(90, 120));
        fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);
        for (Index = BallCount; Index > 0; Index--)
            fillRectangle(SCREEN_WIDTH - Index * 8, SCREEN_HEIGHT-10, 7, 7, RGBToWord(0xff, 0xf, 0xf));
      }
     // check for ball hitting blocks and if this level is done.
     LevelComplete = 1;
     for (Index = 0; Index < BLOCKCOUNT; Index++)
     {
       int touch = blockTouching(Index,ball_x,ball_y);
       if (touch)
       {
          //Play note if ball hits a block
          playNote(1,20);

          //add 100 to player score
          score++;
          hideBlock(Index);
          if ( (touch == 1) || (touch == 3) )
            BallYVelocity = -BallYVelocity;
          if ( (touch == 2) || (touch == 4) )
            BallXVelocity = -BallXVelocity;
        }
        if (Blocks[Index].visible) // any blocks left?
         LevelComplete = 0;
      }
      // No Blocks left, Move to next level.
      if ((LevelComplete == 1) && (Level > 0))
      {
				playTune(myTune,myTime[0],2);
        Level--;
        printText("Level",5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
        printNumber(MAX_BRICI_LEVELS - Level + 1, 45, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));

      }
      //Speed variable as it is changing with difficulty
      delay(10+Level*speed); // Slow the game to human speed and make it level dependant.
    }
            
  }
  fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, RGBToWord(0, 0, 0xff));
  printText("VICTORY!",40, 100, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  printText("Up to restart", 18, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  eputs("\nYou won somehow!\n\r");
  //Display score
  eputs("\n This is your final score in 0s\n\r");
  for(int i = 0; i< score; i++)
	{
		eputs("\n0\n\r");
	}
  while (!UpPressed());
      
  return;
}



void initSysTick(void)
{
   SysTick->LOAD = 48000;
   SysTick->CTRL = 7;
   SysTick->VAL = 10;
   __asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
   milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
// Set the PLL up
// First ensure PLL is disabled
   RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
  // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
  // inserted into Flash memory interface
                        
    FLASH->ACR |= (1 << 0);
    FLASH->ACR &=~((1u << 2) | (1u<<1));
    // Turn on FLASH prefetch buffer
    FLASH->ACR |= (1 << 4);
    // set PLL multiplier to 12 (yielding 48MHz)
    RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
    RCC->CFGR |= ((1<<21) | (1<<19) ); 

    // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
    RCC->CFGR |= (1<<14);

    // and turn the PLL back on again
    RCC->CR |= (1<<24);        
    // set PLL as system clock source 
    RCC->CFGR |= (1<<1);
}

void delay(volatile uint32_t dly)
{
   uint32_t end_time = dly + milliseconds;
   while(milliseconds != end_time)
   __asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
   Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
   Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
  uint32_t mode_value = Port->MODER;
  Mode = Mode << (2 * BitNumber);
  mode_value = mode_value & ~(3u << (BitNumber * 2));
  mode_value = mode_value | Mode;
  Port->MODER = mode_value;
}
void initTimer()
{
   // Power up the timer module
   RCC->APB1ENR |= (1 << 8);
   pinMode(GPIOB,1,2); // Assign a non-GPIO (alternate) function to GPIOB bit 1
   GPIOB->AFR[0] &= ~(0x0fu << 4); // Assign alternate function 0 to GPIOB 1 (Timer 14 channel 1)
   TIM14->CR1 = 0; // Set Timer 14 to default values
   TIM14->CCMR1 = (1 << 6) + (1 << 5);
   TIM14->CCER |= (1 << 0);
   TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
   TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)440);
   TIM14->CCR1 = TIM14->ARR/2;   
   TIM14->CNT = 0;
}

void playNote(uint32_t Freq,uint32_t duration)
{
  TIM14->CR1 = 0; // Set Timer 14 to default values
  TIM14->CCMR1 = (1 << 6) + (1 << 5);
  TIM14->CCER |= (1 << 0);
  TIM14->PSC = 48000000UL/65536UL; // yields maximum frequency of 21kHz when ARR = 2;
  TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)Freq);
  TIM14->CCR1 = TIM14->ARR/2;   
  TIM14->CNT = 0;
  TIM14->CR1 |= (1 << 0);
  uint32_t end_time=milliseconds+duration;
      while(milliseconds < end_time);
  TIM14->CR1 &= ~(1u << 0);
}

int UpPressed(void)
{     
  //Added up button Functionality, PA8
    if ( (GPIOA->IDR & (1<<8)) == 0)
          return 1;
    else
          return 0;
}
int DownPressed(void)
{     
   //Added down button Functionality, PA11
   if ( (GPIOA->IDR & (1<<11)) == 0)
         return 1;
   else
         return 0;
}
int LeftPressed(void)
{
   if ( (GPIOB->IDR & (1<<5)) == 0)
         return 1;
   else
         return 0;
}
int RightPressed(void)
{
   if ( (GPIOB->IDR & (1<<4)) == 0)
         return 1;
   else
         return 0;
}


void hideBlock(uint32_t index)
{
    fillRectangle(Blocks[index].x,Blocks[index].y,BW,BH,0);
    Blocks[index].visible = 0;
}
void showBlock(uint32_t index)
{     
    fillRectangle(Blocks[index].x,Blocks[index].y,BW,BH,Blocks[index].colour);
    Blocks[index].visible = 1;
}
void hideBall(void)
{
    fillRectangle(ball_x,ball_y,2,2,0);
}
void showBall(void)
{
    fillRectangle(ball_x,ball_y,2,2,RGBToWord(255,255,0));
}
void moveBall(uint32_t newX, uint32_t newY)
{
   hideBall();
   ball_x = newX;
   ball_y = newY;
   showBall();
}

void hideBat(void)
{
   fillRectangle(bat_x,bat_y,bat_W,bat_H,0);
}
void showBat(void)
{
   fillRectangle(bat_x,bat_y,bat_W,bat_H,RGBToWord(127,127,255));
}
void moveBat(uint32_t newX, uint32_t newY)
{
   hideBat();
   bat_x = newX;
   bat_y = newY;
   showBat();
}
int blockTouching(int Index,uint16_t x,uint16_t y)
{
      
     // This function returns a non zero value if the object at x,y touches the sprite
    // The sprite is assumed to be rectangular and returns a value as follows:
    // 0 : not hit
    // 1 : touching on top face (lesser Y value)
    // 2 : touching on left face (lesser X value)
    // 3 : touching on bottom face (greater Y value)    
    // 4 : touching on right face (greater X value)
    if (Blocks[Index].visible == 0)
        return 0;
    if ( Blocks[Index].y == ball_y  )
    {  // top face?
      if ( (x>=Blocks[Index].x) && ( x < (Blocks[Index].x+BW) ) )
        return 1;      
    }
    if ( x == Blocks[Index].x )
    {
      // left face
      if ( (y>=Blocks[Index].y) && ( y < (Blocks[Index].y+BH) ) )
        return 2;
    }
    if ( y == (Blocks[Index].y+BH-1)  )
    {  // bottom face?
      if ( (x>=Blocks[Index].x) && ( x < (Blocks[Index].x+BW) ) )
        return 3;      
    }
    if ( x == (Blocks[Index].x + BW-1) )
    {
      // right face
      if ( (y>=Blocks[Index].y) && ( y < (Blocks[Index].y+BH) ) )
        return 4;
    }

    return 0; // not touching
}
static uint32_t prbs_shift_register=0;
void randomize(void)
{
    while(prbs_shift_register ==0) // can't have a value of zero here
          prbs_shift_register=milliseconds;
      
}
uint32_t random(uint32_t lower, uint32_t upper)
{
   uint32_t new_bit=0;     
   uint32_t return_value;
   new_bit= ((prbs_shift_register & (1<<27))>>27) ^ ((prbs_shift_register & (1<<30))>>30);
   new_bit= ~new_bit;
   new_bit = new_bit & 1;
   prbs_shift_register=prbs_shift_register << 1;
   prbs_shift_register=prbs_shift_register | (new_bit);
   return_value = prbs_shift_register;
   return_value = (return_value)%(upper-lower)+lower;
   return return_value;
}

//Toggle the pause option
void paused()
{
   if(pause == 0)
   {
       pause = 1;
   }
   else
   {
       pause = 0;
   }
}

//Easy mode, less blocks and more lives
void easy(void)
{
   printText("Easy", 10, 70, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 255, 0));
   printText("Normal", 10, 80, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
   printText("Hard", 10, 90, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
   GPIOA->ODR |= (1<<12);
   GPIOA->ODR &= ~(1<<1);
   GPIOA->ODR &= ~(1<<0);
   speed = 5;
   blocks = 16;
   BallCount = 7;
}

//Medium / default difficulty, base unchanged game
void medium(void)
{
   printText("Easy", 10, 70, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
   printText("Normal", 10, 80, RGBToWord(0xff, 0xff, 0xff), RGBToWord(255, 255, 0));
   printText("Hard", 10, 90, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
   GPIOA->ODR |= (1<<1);
   GPIOA->ODR &= ~(1<<12);
   GPIOA->ODR &= ~(1<<0);
   reset();
}

//HArder difficulty, faster with another block layer and less lives
void hard(void)
{
   printText("Easy", 10, 70, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
   printText("Normal", 10, 80, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
   printText("Hard", 10, 90, RGBToWord(0xff, 0xff, 0xff), RGBToWord(255, 0, 0));
   GPIOA->ODR |= (1<<0);
   GPIOA->ODR &= ~(1<<12);
   GPIOA->ODR &= ~(1<<1);
   speed = 2;
   blocks = 32;
   BallCount = 3;
}

//serialisation

void SerialBegin()
{
   /* On the nucleo board, TX is on PA2 while RX is on PA15 */
   RCC->AHBENR |= (1 << 17); // enable GPIOA
   RCC->APB2ENR |= (1 << 14); // enable USART1
   pinMode(GPIOA,2,2); // enable alternate function on PA2
   pinMode(GPIOA,15,2); // enable alternate function on PA15
   // AF1 = USART1 TX on PA2
   GPIOA->AFR[0] &= 0xfffff0ff;
   GPIOA->AFR[0] |= (1 << 8);
   // AF1 = USART1 RX on PA2
   GPIOA->AFR[1] &= 0x0fffffff;
   GPIOA->AFR[1] |= (1 << 28);
   // De-assert reset of USART1 
   RCC->APB2RSTR &= ~(1u << 14);
      
   USART1->CR1 = 0; // disable before configuration
   USART1->CR3 |= (1 << 12); // disable overrun detection
   USART1->BRR = 48000000/9600; // assuming 48MHz clock and 9600 bps data rate
   USART1->CR1 |= (1 << 2) + (1 << 3); // enable Transmistter and receiver
   USART1->CR1 |= 1; // enable the UART

}

//Display a character
void eputchar(char c)
{
      while( (USART1->ISR & (1 << 6)) == 0); // wait for any ongoing
      USART1->ICR=0xffffffff;
      // transmission to finish
      USART1->TDR = c;
}

//Grab a character input for assignment
char egetchar()
{
      while( (USART1->ISR & (1 << 5)) == 0); // wait for a character
   return (char)USART1->RDR;
}

//Display a string
void eputs(char *String)
{
   while(*String) // keep printing until a NULL is found
   {
      eputchar(*String);
      String++;
   }
}

//Detect if a key is pressed
int keyPressed(void)
{
  if((USART1->ISR & (1 << 5)) != 0)
  {
   // Get the character
      return 1;
  }
  else
  {
     return 0;
  }
}

//Adds a lot of blocks
void blocked(void)
{
  reset();
  blocks = 64;
}

//Gives you only one life
void noMiss(void)
{
   reset();
   BallCount = 1;
}

//Changes the speed to extremely fast


//Makes the bat thing and verticle
void slimBat(void)
{
  reset();
  bat_W = 3;
  bat_H = 12;
  BAT_WIDTH = 3;
}

//Default settings for to undo previous gamemode settings to avoid overlap
void reset(void)
{
  bat_W = 10;
  bat_H = 3;
  BAT_WIDTH = 10;
  BallCount = 5;
  blocks = 24;
  speed = 5;
}

void playTune(uint32_t tune[],uint32_t time, uint32_t count)
{ 
  uint32_t index=0;
	while(index < count)
  {
     playNote(myTune[index], myTime[index]);
     delay(myTime[index]); //delays the tune
     index++; //starts and ends loop
  } 
}