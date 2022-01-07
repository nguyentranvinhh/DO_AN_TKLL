#include "main.h"
#define     LED     PORTD
#define     ON      1
#define     OFF     0


#define     ADDRESS_SECOND  0x00
#define     ADDRESS_MINUTE  0x01
#define     ADDRESS_HOUR    0x02
#define     ADDRESS_DAY     0x03
#define     ADDRESS_DATE    0x04
#define     ADDRESS_MONTH   0x05
#define     ADDRESS_YEAR    0x06

char second = 0,minute = 0,hour = 0;
char day = 0;
char date = 0,month = 0,year = 0;
unsigned char enableBT7 = 0;
unsigned char cnt_1s =0;
unsigned char cnt = 0;


void init_system(void);
void delay_ms(int value);

void SetupTimeForRealTime();
void SimulatorTime();


void CLOCK();
void SelectMode();
void StatusInit();

void DisplayTime();
void SetTime();
void SetHour();
void SetMinute();
void SetDay();
void SetDate();
void SetMonth();
void SetYear();

void SetAlarm();
void DisplayAlarm();
void Alarm();

void SW(void);
void DisplaySW(void);
void ControlSW(void);

void SetCoundown();
void Countdown();
void DisplayCD();

void TimeMode();

void WorldTime();
void DisplayWT(char n);

#define INIT_SYSTEM     0
#define SET_HOUR        1
#define SET_MINUTE      2
#define SET_DAY         3
#define SET_DATE        4
#define SET_MONTH       5
#define SET_YEAR        6

#define INIT_MODE       7
#define SET_TIME        8
#define SET_ALARM       9
#define STOP_WATCH      10
#define COUNT_DOWN      11

#define SET_HOUR_ALARM      12
#define SET_MINUTE_ALARM    13
#define BIT_ALARM           14
#define BIT_REPEAT          15

#define ALARM           16
#define COMPARE         17

#define SET_HOUR_CD     18
#define SET_MINUTE_CD   19
#define SET_SECOND_CD   20
#define BIT_CD          21

#define COUNTDOWN       22

#define TIME_MODE       23
#define WORLD_TIME      24

unsigned char isButtonMode();
unsigned char isButtonUp();
unsigned char isButtonSet();
unsigned char isButtonModeHold();
unsigned char isButtonReset();
unsigned char enableBlink(void);
unsigned char CompareTime();
unsigned char CompareCD();

unsigned char statusSelect = INIT_MODE;
unsigned char statusSetTime = SET_HOUR;
unsigned char statusSetAlarm = INIT_SYSTEM;
unsigned char statusAlarm = COMPARE;
unsigned char statusMode = -1;
unsigned char statusSetCountDown = INIT_SYSTEM;
unsigned char statusCD = COMPARE;
unsigned char statusWT = INIT_MODE, oldstatusWT = -1;

unsigned char timeBlink = 0;
unsigned char hourAlarm = 0, minuteAlarm = 0, bitAlarm = 0, bitRepeat = 0;
char hourCD = 0, minuteCD = 0, secondCD = 0, bitCD = 0;
char bitSelect = 0;


unsigned char bitAM = 0;
unsigned char stop_sec = 0, stop_min = 0, stop_hour = 0;

unsigned int timeAlarm = 0, timeCD = 0;
unsigned char flagAlarm = 0;
unsigned char flagCD = 0, CD_Run = 0;

char bitTimeMode = 0;
char bitWT = 0;

void main(void)
{
    unsigned int k = 0;
    init_system();
    lcd_clear();
    LcdClearS();
    SetupTimeForRealTime();
    
    
    while (1)
    {
        
        if (flag_timer3) {
            flag_timer3 = 0;
        
            scan_key_matrix();
            
            SimulatorTime();
            SelectMode();
            CLOCK();
            Countdown();
            Alarm();
            PORTD = PORTD ^ 0x01;
            
            DisplayLcdScreen();
        }
    }
}

void delay_ms(int value)
{
    int i,j;
    for(i = 0; i < value; i++)
        for(j = 0; j < 160; j++);
}

void init_system(void)
{
    TRISB = 0x00;		//setup PORTB is output
    TRISD = 0x00;
    init_lcd();
    LED = 0x00;
    init_interrupt();
    delay_ms(1000);
    init_timer0(4695);  //dinh thoi 1ms
    init_timer1(9390);  //dinh thoi 2ms
    init_timer3(46950); //dinh thoi 10ms
    SetTimer0_ms(1);
    SetTimer1_ms(10);
    SetTimer3_ms(50);   //Chu ky thuc hien viec xu ly input,proccess,output
    init_key_matrix();
    // init_i2c();
    
}

void SetupTimeForRealTime()
{
    second = 30;
    minute = 32;
    hour = 20;
    day = 3;
    date = 4;
    month = 1;
    year = 22;
}

void SimulatorTime() {
    cnt_1s=(cnt_1s + 1)%20;
    
    if (cnt_1s == 0)
        second++;
    
    if (second >= 60 ) 
    {
        second = 0;
        minute++;
    }
    if (minute >= 60) 
    {
        minute = 0;
        hour++;
    }
    if (hour >= 24) 
    {
        hour = 0;
        date++;
        day++;
    }
    if (day > 7) {
        day = 1;
    }
    if (date > 28 && month == 2){
        if (year % 4 != 0) {
            month++;
            date = 1;
        }     
    }
    if (date > 29 && month == 2 ) {
            month++;
            date = 1;
    }
    if (date > 30) {
        if (month == 4 || month == 6 || month == 9 || month == 11) {
            date = 1;
            month++;
        }
    } 
    if (date > 31) {
        if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
            date = 1;
            month++;
        }
    }    
    if (month > 12) {
        year++;
        month = 1;
    }
    
}

void SelectMode(){
    switch (statusSelect) {
        case INIT_MODE:
            LcdClearS();
//            DisplayTime();
            statusMode = INIT_MODE;
            if (isButtonMode()){
                statusMode = -1;
                statusSelect = 1;
                bitSelect = 0;
                LcdClearS();
            }
            break;
        case 1:
            if (isButtonMode()) {
                StatusInit();
                statusSelect = INIT_MODE;
            }
                
            LcdPrintStringS(0,0,"1. SET TIME     ");
            LcdPrintStringS(1,0,"2. SET ALARM    ");
            if (bitSelect % 2 == 0)
                LcdPrintStringS(0,14,"<");
            else LcdPrintStringS(1,14,"<");
            
            if (statusMode == -1 && statusAlarm != ALARM && statusCD != COUNTDOWN) {
                if (isButtonSet()) {
                    bitSelect++;
                    if (bitSelect % 2 == 0) {
                        statusSelect = 2;
                        bitSelect = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitSelect--;
                    if (bitSelect < 0) {
                        bitSelect = 1;
                        statusSelect = 3;
                    }
                }
                if (isButtonReset() && bitSelect % 2 == 0){
                    LcdClearS();
                    statusMode = SET_TIME;
                }

                if (isButtonReset() && bitSelect % 2 == 1) {
                    LcdClearS();
                    statusMode = SET_ALARM;
                }
            }
                
            break;
        case 2:
            if (isButtonMode()) {
                StatusInit();
                statusSelect = INIT_MODE;
            }
                
            LcdPrintStringS(0,0,"3. STOPWATCH     ");
            LcdPrintStringS(1,0,"4. COUNTDOWN     ");
            if (bitSelect % 2 == 0)
                LcdPrintStringS(0,14,"<");
            else LcdPrintStringS(1,14,"<");
            
            if (statusMode == -1 && statusAlarm != ALARM && statusCD != COUNTDOWN) {
                if (isButtonSet()) {
                    bitSelect++;
                    if (bitSelect % 2 == 0) {
                        statusSelect = 3;
                        bitSelect = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitSelect--;
                    if (bitSelect < 0) {
                        bitSelect = 1;
                        statusSelect = 1;
                    }
                }

                if (isButtonReset() && bitSelect % 2 == 0){
                    statusSelect = 5;
                    LcdClearS();
                    statusMode = STOP_WATCH;
                }
                
                if (isButtonReset() && bitSelect % 2 == 1) {
                    statusMode = COUNT_DOWN;
                }
            }
                
            break;
        case 3:
            if (isButtonMode()) {
                StatusInit();
                statusSelect = INIT_MODE;
            }
                
            LcdPrintStringS(0,0,"5. TIME MODE    ");
            LcdPrintStringS(1,0,"6. WORLD TIME   ");
            if (bitSelect % 2 == 0)
                LcdPrintStringS(0,14,"<");
            else LcdPrintStringS(1,14,"<");
            
            if (statusMode == -1 && statusAlarm != ALARM && statusCD != COUNTDOWN) {
                if (isButtonSet()) {
                    bitSelect++;
                    if (bitSelect % 2 == 0) {
                        statusSelect = 1;
                        bitSelect = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitSelect--;
                    if (bitSelect < 0) {
                        bitSelect = 1;
                        statusSelect = 2;
                    }
                }

                if (isButtonReset() && bitSelect % 2 == 0){
                    statusSelect = 5;
                    if (enableBT7 == 1) enableBT7 = 0;
                    LcdClearS();
                    statusMode = TIME_MODE;
                }
                
                if (isButtonReset() && bitSelect % 2 == 1) {
                    statusSelect = 4;
                    bitWT = 0;
                    LcdClearS();
                    if (enableBT7 == 1) enableBT7 = 0;
                    statusWT = 1;
                    statusMode = WORLD_TIME;
                }
            }
            break;
        case 4:
            break;
        default:
            if (isButtonMode())
                statusSelect = INIT_MODE;
            break;
    }
}

void CLOCK() {
    switch (statusMode) {
        case INIT_MODE:
          DisplayTime();
////            if (isButtonMode())
////                statusMode = SET_TIME;
            break;
        case SET_TIME:
            LcdClearS();
            SetTime();
//            if (isButtonMode()){
//                LcdClearS();
//                statusSetTime = SET_HOUR;
//                statusMode = SET_ALARM;
//            }
//            if (isButtonModeHold()){
//                LcdClearS();
//                statusSetTime = SET_HOUR;
//                statusSelect = INIT_MODE;
//            }
            break;
        case SET_ALARM:
            LcdClearS();
            SetAlarm();
//            if (isButtonMode()){
//                LcdClearS();
//                statusSetAlarm = INIT_SYSTEM;
//                statusMode = STOP_WATCH;
//            }
//            if (isButtonModeHold()){
//                LcdClearS();
//                statusSetAlarm = INIT_SYSTEM;
//                statusSelect = INIT_MODE;
//            }
            break;
        case STOP_WATCH:
//            LcdClearS();
            SW();
            DisplaySW();
            ControlSW();
//            if (isButtonMode()){
//                LcdClearS();
//                statusSetCountDown = INIT_SYSTEM;
//                statusMode = COUNT_DOWN;
//            }
//            if (isButtonMode){
//                LcdClearS();
//                statusSetCountDown = INIT_SYSTEM;
//                statusSelect = INIT_MODE;
//            }
            break;
        case COUNT_DOWN:
            LcdClearS();
            SetCoundown();
//            if (isButtonMode()){
//                LcdClearS();
//                statusMode = INIT_MODE;
//            }
//           
            
            break;
        case TIME_MODE:
            LcdClearS();
            TimeMode();
            break;
        case WORLD_TIME:
//            LcdClearS();
            WorldTime();
            break;
        default:
            break;
    }
}

void StatusInit() {
    statusSetTime = SET_HOUR;
    statusSetAlarm = INIT_SYSTEM;
    statusSetCountDown = INIT_SYSTEM;
}

void SW(void) {
    
    if (mil_sec >= 100) {
        mil_sec = 0;
        stop_sec++;
    }
    if (stop_sec >= 60) {
        stop_min++;
        stop_sec = 0;
    }
    if (stop_min >= 60) {
        stop_min = 0;
        stop_hour++;
    }
} 


void DisplaySW(void) {
    if (flag_start == 2) LcdPrintStringS(0,0,"   STOPWATCH  ");
    LcdPrintStringS(1,2,"00:00:00:00");
    if (mil_sec < 10)
        LcdPrintNumS(1,12,mil_sec);
    else LcdPrintNumS(1,11,mil_sec);
    if (stop_sec < 10)
        LcdPrintNumS(1,9,stop_sec);
    else LcdPrintNumS(1,8,stop_sec);
    if (stop_min < 10)
        LcdPrintNumS(1,6,stop_min);
    else LcdPrintNumS(1,5,stop_min);
    if (stop_hour < 10)
        LcdPrintNumS(1,3,stop_hour);
    else LcdPrintNumS(1,2,stop_hour);
}

void ControlSW(void) {
    if (isButtonUp()) 
    {
        flag_start = 1;

    }
    if (isButtonUp() && flag_start == 1) {
        if (first_start == 0) 
        {
            LcdClearRow0();
            LcdPrintStringS(0,0,"REC:");
            LcdPrintStringS(0,5,"00:00:00:00");
            if (mil_sec < 10)
                LcdPrintNumS(0,15,mil_sec);
            else LcdPrintNumS(0,14,mil_sec);
            if (stop_sec < 10)
                LcdPrintNumS(0,12,stop_sec);
            else LcdPrintNumS(0,11,stop_sec);
            if (stop_min < 10)
                LcdPrintNumS(0,9,stop_min);
            else LcdPrintNumS(0,8,stop_min);
            if (stop_hour < 10)
                LcdPrintNumS(0,6,stop_hour);
            else LcdPrintNumS(0,5,stop_hour);
        }
        first_start = 0;
    }
    if (isButtonSet())
    {
        flag_start = 0;
        first_start = 1;
    }
    if (isButtonReset()) {
        LcdClearRow0();
        mil_sec = 0;
        stop_sec = 0;
        stop_min = 0;
        stop_hour = 0;
        flag_start = 2;
        first_start = 1;
    }
    
}



void DisplayTime()
{
//    second = read_ds1307(ADDRESS_SECOND);
//    minute = read_ds1307(ADDRESS_MINUTE);
//    hour = read_ds1307(ADDRESS_HOUR);
//    day = read_ds1307(ADDRESS_DAY);
//    date = read_ds1307(ADDRESS_DATE);
//    month = read_ds1307(ADDRESS_MONTH);
//    year = read_ds1307(ADDRESS_YEAR);

    switch(day)
    {
        case 1:
            LcdPrintStringS(0,0,"SUN");
            break;
        case 2:
            LcdPrintStringS(0,0,"MON");
            break;
        case 3:
            LcdPrintStringS(0,0,"TUE");
            break;
        case 4:
            LcdPrintStringS(0,0,"WED");
            break;
        case 5:
            LcdPrintStringS(0,0,"THU");
            break;
        case 6:
            LcdPrintStringS(0,0,"FRI");
            break;
        case 7:
            LcdPrintStringS(0,0,"SAT");
            break;
        default:
            LcdPrintStringS(0,0,"ERR");
            break;    
    }
    if (bitAM == 0) {
        LcdPrintNumS(0,5,hour/10);
        LcdPrintNumS(0,6,hour%10);
        LcdPrintStringS(0,7,":");
        LcdPrintNumS(0,8,minute/10);
        LcdPrintNumS(0,9,minute%10);
    }
    else {
        if (hour < 12) {
            LcdPrintNumS(0,4,hour/10);
            LcdPrintNumS(0,5,hour%10);
            LcdPrintStringS(0,10,"AM");
        }
        else if (hour == 12){
                LcdPrintNumS(0,4,hour/10);
                LcdPrintNumS(0,5,hour%10);
                LcdPrintStringS(0,10,"PM");
            }
            else {
                LcdPrintNumS(0,4,(hour - 12)/10);
                LcdPrintNumS(0,5,(hour - 12)%10);
                LcdPrintStringS(0,10,"PM");
            }
        LcdPrintStringS(0,6,":");
        LcdPrintNumS(0,7,minute/10);
        LcdPrintNumS(0,8,minute%10);
    }
//    LcdPrintStringS(0,9,":");
//    LcdPrintNumS(0,10,second/10);
//    LcdPrintNumS(0,11,second%10);
    
    switch(month)
    {
        case 1:
            LcdPrintStringS(1,2,"JAN");
            break;
        case 2:
            LcdPrintStringS(1,2,"FEB");
            break;
        case 3:
            LcdPrintStringS(1,2,"MAR");
            break;
        case 4:
            LcdPrintStringS(1,2,"APR");
            break;
        case 5:
            LcdPrintStringS(1,2,"MAY");
            break;
        case 6:
            LcdPrintStringS(1,2,"JUN");
            break;
        case 7:
            LcdPrintStringS(1,2,"JUL");
            break;
        case 8:
            LcdPrintStringS(1,2,"AUG");
            break;
        case 9:
            LcdPrintStringS(1,2,"SEP");
            break;
        case 10:
            LcdPrintStringS(1,2,"OCT");
            break;
        case 11:
            LcdPrintStringS(1,2,"NOV");
            break;
        case 12:
            LcdPrintStringS(1,2,"DEC");
            break;
        default:
            LcdPrintStringS(1,2,"ERR");
            break;    
    }
    switch(bitAlarm)
    {
        case 0:
            LcdPrintStringS(0,13,"OFF");
            break;
        case 1:
            LcdPrintStringS(0,13,"ON ");
            break;
    }

    LcdPrintStringS(1,5," ");
    LcdPrintNumS(1,6,date/10);
    LcdPrintNumS(1,7,date%10);
    LcdPrintStringS(1,8," ");
    LcdPrintNumS(1,9,20);
    LcdPrintNumS(1,11,year/10);
    LcdPrintNumS(1,12,year%10);
}

void SetTime()
{
    switch(statusSetTime)
    {
//        case INIT_SYSTEM:         
//            DisplayTime();
//            if(isButtonSet())
//                statusSetTime = SET_HOUR;            
//            break;
        case SET_HOUR:
            DisplayTime();
            SetHour();
            if(isButtonSet())
                statusSetTime = SET_MINUTE;
            
            if (isButtonModeHold()){
                LcdClearS();
                statusMode = INIT_MODE;
            }
            break;
        case SET_MINUTE:
            DisplayTime();
            SetMinute();
            if(isButtonSet())
                statusSetTime = SET_DAY;
            break;
        case SET_DAY:
            DisplayTime();
            SetDay();
            if(isButtonSet())
                statusSetTime = SET_DATE;
            break;
        case SET_DATE:
            DisplayTime();
            SetDate();
            if(isButtonSet())
                statusSetTime = SET_MONTH;
            break;
        case SET_MONTH:
            DisplayTime();
            SetMonth();
            if(isButtonSet())
                statusSetTime = SET_YEAR;
            break;
        case SET_YEAR:
            DisplayTime();
            SetYear();
            if(isButtonSet())
                statusSetTime = SET_HOUR;
            break;  
        
        default:
            break; 
    }
    
}

void SetAlarm() {
    switch (statusSetAlarm) {
        case INIT_SYSTEM:
            DisplayAlarm();
            if (isButtonSet())
                statusSetAlarm = SET_HOUR_ALARM;
            break;
        case SET_HOUR_ALARM:
            DisplayAlarm();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,2,"  ");
            if (isButtonUp())
                hourAlarm = (hourAlarm + 1)%24;
            if(isButtonSet())
                statusSetAlarm = SET_MINUTE_ALARM;
            break;
        case SET_MINUTE_ALARM:
            DisplayAlarm();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,5,"  ");
            if (isButtonUp())
                minuteAlarm = (minuteAlarm + 1)%60;
            if(isButtonSet())
                statusSetAlarm = BIT_ALARM;
            break;
        case BIT_ALARM:
            DisplayAlarm();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,9,"   ");
            if (isButtonUp()) {
                bitAlarm = (bitAlarm + 1)%2;
            }
            if (isButtonSet()) {
                statusSetAlarm = BIT_REPEAT;
            }
            break;
        case BIT_REPEAT:
            DisplayAlarm();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,13,"   ");
            if (isButtonUp()) {
                bitRepeat = (bitRepeat + 1)%2;
            }
            if (isButtonSet()) {
                statusSetAlarm = SET_HOUR_ALARM;
            }
            break;    
        default:
            break;
    }
  
}

void DisplayAlarm() {
    LcdPrintStringS(0,0,"   ALARM MODE   ");
    LcdPrintStringS(1,0,"  ");
    if(hourAlarm < 10)
    {
        LcdPrintStringS(1,2,"0");
        LcdPrintNumS(1,3,hourAlarm);
    }
    else
        LcdPrintNumS(1,2,hourAlarm);
    LcdPrintStringS(1,4,":");
    if(minuteAlarm < 10)
    {
        LcdPrintStringS(1,5,"0");
        LcdPrintNumS(1,6,minuteAlarm);
    }
    else
        LcdPrintNumS(1,5,minuteAlarm);
    LcdPrintStringS(1,7,"   ");
    switch(bitAlarm)
    {
        case 0:
            LcdPrintStringS(1,9,"OFF");
            break;
        case 1:
            LcdPrintStringS(1,9,"ON ");
            break;
    }
//    LcdPrintStringS(1,13,"    ");
    switch (bitRepeat) {
        case 0:
            LcdPrintStringS(1,13,"1T");
            break;
        case 1:
            LcdPrintStringS(1,13,"ED");
            break;
    }
}

unsigned char isButtonMode()
{
    if (key_code[4] == 1)
        return 1;
    else
        return 0;
}

unsigned char isButtonModeHold()
{
    if (key_code[4] == 10)
        return 1;
    else
        return 0;
}

unsigned char isButtonUp()
{
    if ((key_code[5] == 1) || (key_code[5] >= 10 && key_code[5]%2 == 0))
        return 1;
    else
        return 0;
}

unsigned char isButtonSet()
{
    if ((key_code[9] == 1) || (key_code[9] >= 10 && key_code[9]%2 == 0))
        return 1;
    else
        return 0;
}

unsigned char isButtonReset() {
    if (key_code[8] == 1)
        return 1;
    else
        return 0;
}

unsigned char enableBlink(void)
{
    if (key_code[5] == 0 && key_code[9] == 0)
        return 1;
    else 
        return 0;
}

void SetHour()
{
    timeBlink = (timeBlink + 1) % 20;
    if (timeBlink < 10 && enableBlink()) 
        if (bitAM == 0) LcdPrintStringS(0,5,"  ");
        else LcdPrintStringS(0,4,"  ");
    if(isButtonUp())
    {
        hour = hour + 1;
        if(hour > 23)
            hour = 0;  
//        write_ds1307(ADDRESS_HOUR, hour);
    }

}
void SetMinute() {
    timeBlink = (timeBlink + 1) % 20;
    if (timeBlink < 10 && enableBlink()) 
        if (bitAM == 0) LcdPrintStringS(0,8,"  ");
        else LcdPrintStringS(0,7,"  ");
    if(isButtonUp())
    {
        minute = (minute + 1)%60;
    }

}
void SetDay(){
    timeBlink = (timeBlink + 1) % 20;
    if (timeBlink < 10 && enableBlink()) 
        LcdPrintStringS(0,0,"   ");
    if(isButtonUp())
    {
        day = day + 1;
                if(day > 7)
                    day = 1;
    }
}
void SetDate(){
    timeBlink = (timeBlink + 1) % 20;
    if (timeBlink < 10 && enableBlink()) 
        LcdPrintStringS(1,6,"  ");
    if(isButtonUp())
    {
        date = date + 1;
        
        if(date > 31)
            date = 1;
        
        
    }
}
void SetMonth() {
    timeBlink = (timeBlink + 1) % 20;
    if (timeBlink < 10 && enableBlink()) 
        LcdPrintStringS(1,2,"   ");
    if(isButtonUp()) {
        month = month + 1;
        if(month > 12)
            month = 1;
    }
} 
void SetYear() {
    timeBlink = (timeBlink + 1) % 20;
    if (timeBlink < 10 && enableBlink()) 
        LcdPrintStringS(1,9,"    ");
    if(isButtonUp()) {
        year = year + 1;
        if(year > 99)
            year = 0;
    }
}

unsigned char CompareTime(){
    if((hour == hourAlarm) && (minute == minuteAlarm) && (second == 0) && (bitAlarm == ON) )
        return 1;
    else
        return 0;
}

void Alarm(){
    static unsigned char timeBlink = 0;
    switch (statusAlarm) {
        case COMPARE:
            if (CompareTime()){
                flagAlarm = 1;
            }
            if (flagAlarm) {
                statusAlarm = ALARM;
            }
            break;
        case ALARM:
            LcdClearS();
            LcdPrintStringS(0,0,"    ALARM!!!");
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10) 
                LcdClearS();
            timeAlarm++;
            if (timeAlarm > 400 || isButtonReset()) {
                flagAlarm = 0;
                timeAlarm = 0;
                LcdClearS();
                statusAlarm = COMPARE;
                statusSetAlarm = INIT_SYSTEM;
                if (bitRepeat == 0) bitAlarm = 0;
            }
            break;
    }
}

void SetCoundown() {
    switch(statusSetCountDown) {
        case INIT_SYSTEM:
            DisplayCD();
            if (isButtonSet())
                statusSetCountDown = SET_HOUR_CD;
            break;
        case SET_HOUR_CD:
            DisplayCD();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,1,"  ");
            if (isButtonUp())
                hourCD = (hourCD + 1)%24;
            if(isButtonSet())
                statusSetCountDown = SET_MINUTE_CD;
            break;
        case SET_MINUTE_CD:
            DisplayCD();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,4,"  ");
            if (isButtonUp())
                minuteCD = (minuteCD + 1)%60;
            if(isButtonSet())
                statusSetCountDown = SET_SECOND_CD;
            break;
        case SET_SECOND_CD:
            DisplayCD();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink()) 
                LcdPrintStringS(1,7,"  ");
            if (isButtonUp())
                secondCD = (secondCD + 1)%60;
            if(isButtonSet())   {
                if (hourCD != 0 || minuteCD != 0 || secondCD != 0)
                {
//                    bitCD = (bitCD + 1)%2;
                    statusSetCountDown = BIT_CD;
                }       
                else  statusSetCountDown = SET_HOUR_CD;
            }   
                
            break;
        case BIT_CD:
            DisplayCD();
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10 && enableBlink() && CD_Run == 0) 
                LcdPrintStringS(1,9,"      ");
            if (isButtonUp()) {
                bitCD = (bitCD + 1)%2;
            }
            if (isButtonSet()&& bitCD == 0) {
                statusSetCountDown = SET_HOUR_CD;
                CD_Run = 0;
            }
            break;
        default:
            break;
    }
}

void DisplayCD() {
    if (flagCD == 0) LcdPrintStringS(0,0," COUNTDOWN MODE  ");
    if (flagCD == 0){
        LcdPrintStringS(1,0," ");
        if(hourCD < 10)
        {
            LcdPrintStringS(1,1,"0");
            LcdPrintNumS(1,2,hourCD);
        }
        else
            LcdPrintNumS(1,1,hourCD);
        LcdPrintStringS(1,3,":");
        if(minuteCD < 10)
        {
            LcdPrintStringS(1,4,"0");
            LcdPrintNumS(1,5,minuteCD);
        }
        else
            LcdPrintNumS(1,4,minuteCD);
        LcdPrintStringS(1,6,":");
        if(secondCD < 10)
        {
            LcdPrintStringS(1,7,"0");
            LcdPrintNumS(1,8,secondCD);
        }
        else
            LcdPrintNumS(1,7,secondCD);
        LcdPrintStringS(1,9,"  ");
        switch(bitCD)
        {
            case 0:
                LcdPrintStringS(1,10,"START");
                break;
            case 1:
                LcdPrintStringS(1,10,"STOP ");
                CD_Run = 1;
                break;
        }
    }
//    LcdPrintStringS(1,13,"    ");
}

void Countdown() {
    static unsigned char timeBlink = 0;
    switch(statusCD) {
        case COMPARE:
            cnt = (cnt + 1) % 20;
            if (cnt == 0 && bitCD)
                secondCD--;
            if (secondCD < 0) {
                secondCD = 59;
                minuteCD--;
            }
            if (minuteCD < 0) {
                minuteCD = 59;
                hourCD--;
            }
            if (CompareCD()) 
                flagCD = 1;
            if (flagCD == 1)
                statusCD = COUNTDOWN;
            break;
        case COUNTDOWN:
            LcdClearRow0();  
            bitCD = 0;
            LcdPrintStringS(0,0,"    COUNTDOWN ");
            LcdPrintStringS(1,0,"     IS OVER ");
            timeBlink = (timeBlink + 1) % 20;
            if (timeBlink < 10) 
                LcdClearS();
            timeCD++;
            if (timeCD > 400 || isButtonReset()) { 
                flagCD = 0; 
                CD_Run = 0;
                LcdClearS();
                statusSetCountDown = INIT_SYSTEM;
                statusCD = COMPARE;
            }
            break;
    }

    
}
unsigned char CompareCD() {
    if (hourCD == 0 && minuteCD ==0 && secondCD == 0 && bitCD == 1)
        return 1;
    else return 0;
}

void TimeMode() {
    LcdPrintStringS(0,0,"1. 12H MODE");
    LcdPrintStringS(1,0,"2. 24H MODE");

    if (bitTimeMode % 2 == 0)
        LcdPrintStringS(0,14,"<");
    else LcdPrintStringS(1,14,"<");
    
        if (isButtonSet()) 
            bitTimeMode++;
              
        if (isButtonUp()) 
            bitTimeMode--;

        if (isButtonReset() && bitTimeMode % 2 == 0 && enableBT7 == 1){
            bitAM = 1;
            statusSelect = INIT_MODE;
        }
        if (isButtonReset() && bitTimeMode % 2 == 1 && enableBT7 == 1) {
            bitAM = 0;
            statusSelect = INIT_MODE;
        }
    enableBT7 = 1;
    
}
void WorldTime(){
    switch(statusWT) {
        case 1:
            LcdClearS();
            LcdPrintStringS(0,0,"1. SYDNEY");
            LcdPrintStringS(1,0,"2. TOKYO ");
            if (isButtonMode())
                statusSelect = INIT_MODE;
            if (bitWT % 2 == 0) {
                LcdPrintStringS(1,14," ");
                LcdPrintStringS(0,14,"<");
            }
            else {
                LcdPrintStringS(0,14," ");
                LcdPrintStringS(1,14,"<");
            }
            if (isButtonSet()) {
                    bitWT++;
                    if (bitWT % 2 == 0) {
                        statusWT = 2;
                        bitWT = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitWT--;
                    if (bitWT < 0) {
                        bitWT = 1;
                        statusWT = 4;
                    }
                }
                if (isButtonReset() && bitWT % 2 == 0 && enableBT7 == 1){
                    statusWT = 5;
                    oldstatusWT = 1;
                    LcdClearS();
                    if (hour + 4 > 23) LcdPrintStringS(0,0,"TOMOROW, +4H");
                    else LcdPrintStringS(0,0,"TODAY, +4H");
                    DisplayWT(4);  
                   
                }

                if (isButtonReset() && bitWT % 2 == 1 && enableBT7 == 1) {
                    statusWT = 5;
                    oldstatusWT = 1;
                    LcdClearS();
                    if (hour + 2 > 23) LcdPrintStringS(0,0,"TOMOROW, +2H");
                    else LcdPrintStringS(0,0,"TODAY, +2H");
                    DisplayWT(2); 
                }
            enableBT7 = 1;
            break;
        case 2:
            LcdClearS();
            LcdPrintStringS(0,0,"3. LONDON");
            LcdPrintStringS(1,0,"4. PARIS ");
            if (isButtonMode())
                statusSelect = INIT_MODE;
            if (bitWT % 2 == 0) {
                LcdPrintStringS(1,14," ");
                LcdPrintStringS(0,14,"<");
            }
            else {
                LcdPrintStringS(0,14," ");
                LcdPrintStringS(1,14,"<");
            }
            if (isButtonSet()) {
                    bitWT++;
                    if (bitWT % 2 == 0) {
                        statusWT = 3;
                        bitWT = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitWT--;
                    if (bitWT < 0) {
                        bitWT = 1;
                        statusWT = 1;
                    }
                }
                if (isButtonReset() && bitWT % 2 == 0 && enableBT7 == 1){
                    statusWT = 5;
                    oldstatusWT = 2;
                    LcdClearS();
                    if (hour - 7 < 0) LcdPrintStringS(0,0,"YESTERDAY, -7H");
                    else LcdPrintStringS(0,0,"TODAY, -7H");
                    DisplayWT(-7);  
                   
                }

                if (isButtonReset() && bitWT % 2 == 1 && enableBT7 == 1) {
                    statusWT = 5;
                    oldstatusWT = 2;
                    LcdClearS();
                    if (hour - 6 < 0) LcdPrintStringS(0,0,"YESTERDAY, -6H");
                    else LcdPrintStringS(0,0,"TODAY, -6H");
                    DisplayWT(-6); 
                }
            enableBT7 = 1;
            break;
        case 3:
            LcdClearS();
            LcdPrintStringS(0,0,"5. CAIRO");
            LcdPrintStringS(1,0,"6. DUBAI ");
            if (isButtonMode())
                statusSelect = INIT_MODE;
            if (bitWT % 2 == 0) {
                LcdPrintStringS(1,14," ");
                LcdPrintStringS(0,14,"<");
            }
            else {
                LcdPrintStringS(0,14," ");
                LcdPrintStringS(1,14,"<");
            }
            if (isButtonSet()) {
                    bitWT++;
                    if (bitWT % 2 == 0) {
                        statusWT = 4;
                        bitWT = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitWT--;
                    if (bitWT < 0) {
                        bitWT = 1;
                        statusWT = 2;
                    }
                }
                if (isButtonReset() && bitWT % 2 == 0 && enableBT7 == 1){
                    statusWT = 5;
                    oldstatusWT = 3;
                    LcdClearS();
                    if (hour - 5 < 0) LcdPrintStringS(0,0,"YESTERDAY, -5H");
                    else LcdPrintStringS(0,0,"TODAY, -5H");
                    DisplayWT(-5);  
                   
                }

                if (isButtonReset() && bitWT % 2 == 1 && enableBT7 == 1) {
                    statusWT = 5;
                    oldstatusWT = 3;
                    LcdClearS();
                    if (hour - 3 < 0) LcdPrintStringS(0,0,"YESTERDAY, -3H");
                    else LcdPrintStringS(0,0,"TODAY, -3H");
                    DisplayWT(-3); 
                }
            enableBT7 = 1;
            break;
        case 4:
            LcdClearS();
            LcdPrintStringS(0,0,"7. LOS ANGELES");
            LcdPrintStringS(1,0,"8. NEW YORK ");
            if (isButtonMode())
                statusSelect = INIT_MODE;
            if (bitWT % 2 == 0) {
                LcdPrintStringS(1,14," ");
                LcdPrintStringS(0,14,"<");
            }
            else {
                LcdPrintStringS(0,14," ");
                LcdPrintStringS(1,14,"<");
            }
            if (isButtonSet()) {
                    bitWT++;
                    if (bitWT % 2 == 0) {
                        statusWT = 1;
                        bitWT = 0;
                    }
                }
                    
                if (isButtonUp()) {
                    bitWT--;
                    if (bitWT < 0) {
                        bitWT = 1;
                        statusWT = 3;
                    }
                }
                if (isButtonReset() && bitWT % 2 == 0 && enableBT7 == 1){
                    statusWT = 5;
                    oldstatusWT = 4;
                    LcdClearS();
                    if (hour - 15 < 0) LcdPrintStringS(0,0,"YESTERDAY, -15H");
                    else LcdPrintStringS(0,0,"TODAY, -15H");
                    DisplayWT(-15);  
                   
                }

                if (isButtonReset() && bitWT % 2 == 1 && enableBT7 == 1) {
                    statusWT = 5;
                    oldstatusWT = 4;
                    LcdClearS();
                    if (hour - 12 < 0) LcdPrintStringS(0,0,"YESTERDAY, -12H");
                    else LcdPrintStringS(0,0,"TODAY, -12H");
                    DisplayWT(-12); 
                }
            enableBT7 = 1;
            break;
        default:
            if (isButtonMode()){
                statusWT = oldstatusWT;
                oldstatusWT = 5;
            }
            break;
            
            
    }
}
void DisplayWT(char n){
    char hour1 = hour + n;
    if (hour1 > 23) hour1 -= 24;
    else if (hour1 < 0) hour1 += 24;
    if (bitAM == 0) {
        LcdPrintNumS(1,5,hour1/10);
        LcdPrintNumS(1,6,hour1%10);
        LcdPrintStringS(1,7,":");
        LcdPrintNumS(1,8,minute/10);
        LcdPrintNumS(1,9,minute%10);
    }
    else {
        if (hour1 < 12) {
            LcdPrintNumS(1,4,hour1/10);
            LcdPrintNumS(1,5,hour1%10);
            LcdPrintStringS(1,10,"AM");
        }
        else if (hour1 == 12){
                LcdPrintNumS(1,4,hour1/10);
                LcdPrintNumS(1,5,hour1%10);
                LcdPrintStringS(1,10,"PM");
            }
            else {
                LcdPrintNumS(1,4,(hour1 - 12)/10);
                LcdPrintNumS(1,5,(hour1 - 12)%10);
                LcdPrintStringS(1,10,"PM");
            }
        LcdPrintStringS(1,6,":");
        LcdPrintNumS(1,7,minute/10);
        LcdPrintNumS(1,8,minute%10);
    }
    
    
}