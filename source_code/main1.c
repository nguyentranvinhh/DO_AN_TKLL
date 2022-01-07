#include "main.h"
// Noi khai bao hang so
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
// Noi khai bao bien toan cuc
char second = 0,minute = 0,hour = 0;
char day = 0;

char date = 0,month = 0,year = 0;
unsigned char count = 0;
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);

void SetupTimeForRealTime();
void BaiTap_I2C();
void DisplayTime();
void SetUpTime();

void SetHour();
void SetMinute();
void SetDay();
void SetDate();
void SetMonth();
void SetYear();



//Chuong trinh Setup time
#define INIT_SYSTEM     0
#define SET_HOUR        1
#define SET_MINUTE      2
#define SET_DAY         3
#define SET_DATE        4
#define SET_MONTH       5
#define SET_YEAR        6

unsigned char isButtonMode();
unsigned char isButtonUp();
unsigned char isButtonDown();
unsigned char timeBlink = 0;
unsigned char statusSetupTime = INIT_SYSTEM;
////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
    unsigned int k = 0;
    init_system();
    lcd_clear();
    LcdClearS();
    delay_ms(1000);
//    write_ds1307(0x10, 0);
    count = read_ds1307(0x10);
    count ++;
    write_ds1307(0x10, count);
    LcdPrintNum(0,8, count);
    delay_ms(2000);
    //SetupTimeForRealTime();
    while (1)
    {
        while (!flag_timer3);
        flag_timer3 = 0;
        scan_key_matrix();

        DisplayTime();
        SetUpTime();
        DisplayLcdScreenOld();
    }
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
    int i,j;
    for(i=0;i<value;i++)
        for(j=0;j<238;j++);
}

void init_system(void)
{
    TRISB = 0x00;		//setup PORTB is output
    TRISD = 0x00;
    init_lcd();
    LED = 0x00;
    init_interrupt();
    delay_ms(1000);
    init_timer0(4695);//dinh thoi 1ms sai so 1%
    init_timer1(9390);//dinh thoi 2ms
    init_timer3(46950);//dinh thoi 10ms
    SetTimer0_ms(2);
    SetTimer1_ms(10);
    SetTimer3_ms(15); //Chu ky thuc hien viec xu ly input,proccess,output
    init_key_matrix();
    init_i2c();
}


///////I2C
void SetupTimeForRealTime()
{
    second = 55;
    minute = 59;
    hour = 23;
    day = 7;
    date = 31;
    month = 12;
    year = 13;
    write_ds1307(ADDRESS_SECOND, second);
    write_ds1307(ADDRESS_MINUTE, minute);
    write_ds1307(ADDRESS_HOUR, hour);
    write_ds1307(ADDRESS_DAY, day);
    write_ds1307(ADDRESS_DATE, date);
    write_ds1307(ADDRESS_MONTH, month);
    write_ds1307(ADDRESS_YEAR, year);
}

void BaiTap_I2C()
{
    second = read_ds1307(ADDRESS_SECOND);
    minute = read_ds1307(ADDRESS_MINUTE);
    hour = read_ds1307(ADDRESS_HOUR);
    day = read_ds1307(ADDRESS_DAY);
    date = read_ds1307(ADDRESS_DATE);
    month = read_ds1307(ADDRESS_MONTH);
    year = read_ds1307(ADDRESS_YEAR);
    
    LcdPrintNumS(0,0,year);
    LcdPrintNumS(0,3,month);
    LcdPrintNumS(0,6,date);
    LcdPrintNumS(0,9,day);
    LcdPrintNumS(1,0,hour);
    LcdPrintNumS(1,3,minute);
    LcdPrintNumS(1,6,second);
}

void DisplayTime()
{
    second = read_ds1307(ADDRESS_SECOND);
    minute = read_ds1307(ADDRESS_MINUTE);
    hour = read_ds1307(ADDRESS_HOUR);
    day = read_ds1307(ADDRESS_DAY);
    date = read_ds1307(ADDRESS_DATE);
    month = read_ds1307(ADDRESS_MONTH);
    year = read_ds1307(ADDRESS_YEAR);

    //////day
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
    }
    LcdPrintNumS(0,4,hour/10);
    LcdPrintNumS(0,5,hour%10);
    LcdPrintStringS(0,6,":");
    LcdPrintNumS(0,7,minute/10);
    LcdPrintNumS(0,8,minute%10);
    LcdPrintStringS(0,9,":");
    LcdPrintNumS(0,10,second/10);
    LcdPrintNumS(0,11,second%10);
    
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
    }

    LcdPrintStringS(1,5," ");
    if(date < 10)
    {
        LcdPrintStringS(1,6," ");
        LcdPrintNumS(1,7,date);
    }
    else
        LcdPrintNumS(1,6,date);
    LcdPrintStringS(1,8," ");
    LcdPrintNumS(1,9,20);
    LcdPrintNumS(1,11,year);
}

void SetUpTime()
{
    switch(statusSetupTime)
    {
        case INIT_SYSTEM:         
            if(isButtonMode())
                statusSetupTime = SET_HOUR;
            break;
        case SET_HOUR:
            SetHour();
            if(isButtonMode())
                statusSetupTime = SET_MINUTE;
            break;
        case SET_MINUTE:
            SetMinute();
            if(isButtonMode())
                statusSetupTime = SET_DAY;
            break;
        case SET_DAY:
            SetDay();
            if(isButtonMode())
                statusSetupTime = SET_DATE;
            break;
        case SET_DATE:
            SetDate();
            if(isButtonMode())
                statusSetupTime = SET_MONTH;
            break;
        case SET_MONTH:
            SetMonth();
            if(isButtonMode())
                statusSetupTime = SET_YEAR;
            break;
        case SET_YEAR:
            SetYear();
            if(isButtonMode())
                statusSetupTime = INIT_SYSTEM;
            break;
        default:
            statusSetupTime = INIT_SYSTEM;
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

unsigned char isButtonUp()
{
    if ((key_code[5] == 1) || (key_code[5] >= 10 && key_code[5]%4 == 0))
        return 1;
    else
        return 0;
}

unsigned char isButtonDown()
{
    if ((key_code[9] == 1) || (key_code[9] >= 10 && key_code[9]%4 == 0))
        return 1;
    else
        return 0;
}

void SetHour()
{
    timeBlink = (timeBlink + 1)%20;
    if(timeBlink < 5)
        LcdPrintStringS(0,4,"  ");
    if(isButtonUp())
    {
        hour ++;
        if(hour > 23)
            hour = 0;  
        write_ds1307(ADDRESS_HOUR, hour);
    }
    if(isButtonDown())
    {
        hour --;
        if(hour < 0)
            hour = 23;
        write_ds1307(ADDRESS_HOUR, hour);
    }    
}
void SetMinute()
{
    timeBlink = (timeBlink + 1)%20;
    if(timeBlink < 5)
        LcdPrintStringS(0,7,"  ");
    if(isButtonUp())
    {
        minute ++;
        if(minute > 59)
            minute = 0;  
        write_ds1307(ADDRESS_MINUTE, minute);
    }
    if(isButtonDown())
    {
        minute --;
        if(minute < 0)
            minute = 59;
        write_ds1307(ADDRESS_MINUTE, minute);
    }
}
void SetDay()
{
}
void SetDate()
{
}
void SetMonth()
{
}
void SetYear()
{
}