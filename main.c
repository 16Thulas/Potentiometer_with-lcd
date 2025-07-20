#include<xc.h>
#define _XTAL_FREQ 6000000 //6mhz 
void adc_lcd_init();
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_print_number(const char *label, unsigned int value, unsigned char line_addr, unsigned char max_digits);
void lcd_set_cursor(unsigned char);
//global variables
unsigned int adc_value;
unsigned char adc_high,adc_low;
unsigned int adc1_last =9999;
unsigned int adc5_last = 9999;
void adc_lcd_init(){
	// lcd init
	TRISC = 0x00; //all pins are op
	TRISD = 0x00;
	PORTD = 0x00;
	PORTC = 0x00; //all pins are low
    //lcd_commands
	lcd_command(0x30);
	__delay_ms(100);
	lcd_command(0x30);
	__delay_ms(100);
	lcd_command(0x30);
	__delay_ms(100);
	lcd_command(0x38);
	__delay_ms(100);
	lcd_command(0x06);
	__delay_ms(100);
	lcd_command(0x0C);
	__delay_ms(100);
	lcd_command(0x01);
	__delay_ms(100);
	//ADC initialization
    TRISA1 =1;//SET RA1 as input(AN1)
    TRISA5 =1;//SET RA5 (AN4) as input
	ADCON1 = 0x80; //0x8E
	ADCON0 = 0x11; //0x81
	__delay_ms(10); //acquisition time
}
void main(){
	adc_lcd_init();
	while(1){
        //to read CH1(AN1)
		ADCON0 = 0x89;  //select AN1 (0x09)
        __delay_ms(10);  // Wait for acquisition time
        ADCON0 |= 0x04; // Start conversion
		while(ADCON0 & 0x04); // Wait until ADC conversion is complete
        adc_value = ((ADRESH << 8) + ADRESL); // Combine 10-bit result from ADRESH and ADRESL
        if (adc_value != adc1_last) {
            adc1_last = adc_value;
         lcd_print_number("CH1:",adc_value, 0x80,4); }
         __delay_ms(100);
		//adc_high = ADRESH;
		//adc_low =  ADRESL;
    
		//--READ CH2 (AN4)---
		ADCON0 = 0xA1;  // Select Channel 4 (AN4), ADC ON
        __delay_us(10);
        ADCON0 = 0xA5;            //  ADCON0 |= 0x04; //start conversion(0x04)
        while (ADCON0 & 0x04); //0x04
        adc_value = ((ADRESH << 8) + ADRESL);  // Read ADC result
        adc_value =((unsigned long)adc_value * 135) / 1023; // scale 0–135
        if (adc_value != adc5_last) {
            adc5_last = adc_value;
        lcd_print_number("CH4:",adc_value, 0xC0,4); }
        // Line 2
         __delay_ms(100);
	}
}
void lcd_command(unsigned char i){
	PORTC&=~0x08; //RS=>RC3
	PORTD=i;
	PORTC|= 0x01;  //RC0 =1
	PORTC&=~0x01;  //RC0=0
	__delay_ms(100);
}
void lcd_data(unsigned char i){
	PORTC|=0x08; //RS->RC3->1
	PORTD=i;
	PORTC|=0x01;  //RC0 =1
	PORTC&=~0x01;  //RC0=0
	__delay_ms(100);
}

void lcd_set_cursor(unsigned char address) {
    lcd_command(address);
}
// Print number with space clearing
void lcd_print_number(const char *label, unsigned int value, unsigned char line_addr, unsigned char max_digits) {
    unsigned char str[5] = {' ', ' ', ' ', ' ', '\0'};
    int i = max_digits -1;
    //unsigned char j;
    // Convert number to string (right to left)
    if (value == 0) {
        str[i] = '0';
    } else {
        while (value > 0 && i >= 0) {
            str[i--] = (value % 10) + '0';
            value /= 10;
        }
    }
    // Display the 4-digit number 
    lcd_set_cursor(line_addr);
    for (i = 0; label[i] != '\0'; i++) {
        lcd_data(label[i]);
    }
    // Print number after label 
    for (i = 0; i < max_digits; i++) {
        lcd_data(str[i]);
    }
    lcd_data('v');
}

