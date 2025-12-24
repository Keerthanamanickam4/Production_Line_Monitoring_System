#include <pic.h>
#define _XTAL_FREQ 8000000

// LCD Control Pins (4-bit Mode)
#define rs RB0
#define rw RB1
#define en RB2

#define IR_SENSOR RA0
#define buzzer RB5

#define delay for(i=0;i<500;i++)

int i;

// Variables
unsigned char current_state = 2; 
unsigned int run_time = 0;       
unsigned char gsm_send_flag = 0; 
void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(const char *s);
void show_number(unsigned int n);
void int_to_str(unsigned int n, char *buf);

void uart_init();
void send_char_uart(unsigned char c);
void send_string_uart(const char *s);
void send_sms(const char *message);
void send_sms_with_time(unsigned int t);

void main()
{
    TRISA = 0xFF;   
    PORTA = 0xFF;

    TRISB = 0x00;   
    PORTB = 0x00;
    buzzer = 0;

    ADCON1 = 0x06;

    lcd_init();
    show("Line Monitoring");
    cmd(0xC0);
    show("Initializing...");
    __delay_ms(1500);

    uart_init();

    cmd(0x01); // Clear LCD
    __delay_ms(20);
    show("System Ready");
    __delay_ms(1000);
    cmd(0x01);
    show("Line Monitoring");
      __delay_ms(1000);
    while(1)
    {
        
        if(IR_SENSOR == 0)
        {
            
            if(current_state != 1)
            {
                current_state = 1;
                gsm_send_flag = 1;  
                run_time = 0;       // Reset timer
            }

            
            cmd(0x80);
            show("Line Running   ");
            cmd(0xC0);
            show("Time: ");
            show_number(run_time);
            show(" sec  ");

            buzzer = 0;
            run_time++;  // Increment running time every second

            
            if(gsm_send_flag)
            {
                send_sms("Line Running\r\n"); 
                gsm_send_flag = 0;           // Reset flag
            }
        }
        else 
        {
            
            cmd(0x80);
            show("Line Stopped   ");
            cmd(0xC0);
            show("Time: ");
            show_number(run_time);
            show(" sec  ");

            buzzer = 1;

         
            if(current_state != 0)
            {
                current_state = 0;
                send_sms_with_time(run_time); 
                run_time = 0;                 
            }
        }

        __delay_ms(1000);  
    }
}

// LCD Functions 
void lcd_init()
{
    TRISD = 0x00;

    cmd(0x33);
    cmd(0x32);
    cmd(0x28);
    cmd(0x0C);
    cmd(0x06);
    cmd(0x80);
}

void cmd(unsigned char a)
{
    PORTD = (PORTD & 0x0F) | (a & 0xF0);
    rs = 0; rw = 0; en = 1; delay; en = 0;

    PORTD = (PORTD & 0x0F) | ((a << 4) & 0xF0);
    rs = 0; rw = 0; en = 1; delay; en = 0;
}

void dat(unsigned char b)
{
    PORTD = (PORTD & 0x0F) | (b & 0xF0);
    rs = 1; rw = 0; en = 1; delay; en = 0;

    PORTD = (PORTD & 0x0F) | ((b << 4) & 0xF0);
    rs = 1; rw = 0; en = 1; delay; en = 0;
}

void show(const char *s)
{
    while(*s) dat(*s++);
}

void show_number(unsigned int n)
{
    unsigned char buf[6];
    int p = 0;

    if(n == 0)
    {
        dat('0');
        return;
    }

    while(n > 0)
    {
        buf[p++] = (n % 10) + '0';
        n /= 10;
    }

    while(p--)
        dat(buf[p]);
}

void int_to_str(unsigned int n, char *buf)
{
    int p = 0;
    if(n == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while(n > 0)
    {
        buf[p++] = (n % 10) + '0';
        n /= 10;
    }

    for(int i = 0; i < p/2; i++)
    {
        char t = buf[i];
        buf[i] = buf[p-i-1];
        buf[p-i-1] = t;
    }

    buf[p] = '\0';
}

// UART 
void uart_init()
{
    TRISC6 = 0;
    TRISC7 = 1;
    SPBRG = 51;       // 9600 @ 8MHz (BRGH=1)
    BRGH = 1;
    SYNC = 0;
    SPEN = 1;
    TXEN = 1;
    CREN = 1;
}

void send_char_uart(unsigned char c)
{
    while(!TXIF);
    TXREG = c;
}

void send_string_uart(const char *s)
{
    while(*s) send_char_uart(*s++);
}

void send_sms(const char *message)
{
    __delay_ms(100);
    send_string_uart("AT\r\n");
    __delay_ms(100);
    send_string_uart("AT+CMGF=1\r\n");
    __delay_ms(100);
    send_string_uart("AT+CMGS=\"+919363200465\"\r\n");
    __delay_ms(100);
    send_string_uart(message);
    send_char_uart(0x1A);   
    __delay_ms(500);
}

void send_sms_with_time(unsigned int t)
{
    char msg[40];  
    char buf[6];
    int_to_str(t, buf);

    int k = 0;
    char temp[] = "Line Stopped: ";
    char sec_text[] = " sec\r\n";

    for(int j = 0; temp[j] != '\0'; j++)
        msg[k++] = temp[j];

    for(int j = 0; buf[j] != '\0'; j++)
        msg[k++] = buf[j];

   
    for(int j = 0; sec_text[j] != '\0'; j++)
        msg[k++] = sec_text[j];

    msg[k] = '\0'; 

    send_sms(msg);
}