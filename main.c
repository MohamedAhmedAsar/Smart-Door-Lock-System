#include <reg51.h>


#define lcd P1
#define keyport P2

//sbits
sbit en = P3 ^ 0;// LED pin define
sbit rs = P3 ^ 1;

sbit servo_motor=P3 ^ 4;

sbit buzzer = P3^ 5;

sbit motor_1=P3^4;
sbit motor_2=P3^6;

code unsigned char keypad[4][4] = {//real
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};


// code unsigned char keypad[4][4] ={	//proteus
// 	{'7','8','9','/'},
// 	{'4','5','6','x'},
// 	{'1','2','3','-'},
// 	{'*','0','=','+'}
// };


code unsigned char msg_1[]="Password :";
code unsigned char msg_2[]="wrong_";
code unsigned char new[]="new ";
unsigned char k=0;
unsigned char count=0;
unsigned char num_entered=0;
unsigned char password[]="123456";
unsigned char input[]="000000";
unsigned char colloc, rowloc;
unsigned char input_c;
bit flag=0;
bit changed=0;


// functions def..
void delay(int);
void enable();
void print_lcd(char);
void start_lcd(bit);
unsigned char key_detect();
void hide_password();
unsigned char hidden(unsigned char);
void check_password();
void wrong_password();
void correct_password();
bit compare();
void backspace();
void new_password();
void change_password();
void program(bit);


//main
void main(){

	motor_1=0;
	motor_2=0;
	changed=0;
	EA=1;
	EX0=1;
	T0=0;
	buzzer=0;
    start_lcd(0);
	count=0;
	
    while (1){
		program(0);
	}
}

// functions implentaions 
void delay(int i){
    int j;
    while (i--){
        j=1275;
        while (j--){}
    }
}

void enable(){
    en=0;
    delay(5);
    en=1;
}

void print_lcd(char c){
    if(c!=0){
        lcd=c;
        enable();
    }
}

void start_lcd(bit f){
    rs=0;
    lcd=0x0F;
    enable();
    lcd=0x01;
    enable();
    lcd=0x02;
    enable();
    lcd=0x38;
    enable();
	rs=1;

	if(f){
			k=0;
			while (new[k]!='\0'){
				print_lcd(new[k]);
			k++;
		}
		}
    k=0;

    while (msg_1[k]!='\0'){
        print_lcd(msg_1[k]);
        k++;
    }

    rs=0;
    lcd=0xc0;
    enable();
    rs=1;
}


unsigned char key_detect()
{

	keyport=0xF0;			/*set port direction as input-output*/
	do
	{
		keyport = 0xF0;				
		colloc = keyport;
		colloc&= 0xF0;	/* mask port for column read only */
	}while(colloc != 0xF0);		/* read status of column */
	
	do
	{
		do
		{
			delay(20);	/* 20ms key debounce time */
			colloc = (keyport & 0xF0);	/* read status of column */
		}while(colloc == 0xF0);	/* check for any key press */

		if(changed){
		changed=0;
		start_lcd(0);
		count=0;
		}

		delay(1);
		colloc = (keyport & 0xF0);
	}while(colloc == 0xF0);
			
	while(1)
	{
		/* now check for rows */
		keyport= 0xFE;/* check for pressed key in 1st row */
		colloc = (keyport & 0xF0);
		if(colloc != 0xF0)
		{
			rowloc = 0;
			break;
		}

		keyport = 0xFD;/* check for pressed key in 2nd row */
		colloc = (keyport & 0xF0);
		if(colloc != 0xF0)
		{
			rowloc = 1;
			break;
		}
			
	keyport = 0xFB;/* check for pressed key in 3rd row */
	colloc = (keyport & 0xF0);
	if(colloc != 0xF0)
	{
		rowloc = 2;
		break;
	}

	keyport = 0xF7;/* check for pressed key in 4th row */
	colloc = (keyport & 0xF0);
	if(colloc != 0xF0)
	{
		rowloc = 3;
		break;
	}	
}
	
	if(colloc == 0xE0)
	{
		return(keypad[rowloc][0]);
	}		
	else if(colloc == 0xD0)
	{
		return(keypad[rowloc][1]);
	}	
	else if(colloc == 0xB0)
	{
		return(keypad[rowloc][2]);
	}	
	else
	{
		return(keypad[rowloc][3]);
	}	
}


void hide_password(){
	rs=0;
    lcd=0xc0;
    enable();
    rs=1;
	for (k = 0; k < count; k++)
	{
		print_lcd(hidden(input[k]));
	}
	
}


unsigned char hidden(unsigned char c){
	if (flag) return '*';
	return c;

}


void check_password(){
	if(compare()){
		correct_password();
	}
	else{
		wrong_password();
	}
}


void correct_password(){
	rs=0;
    lcd=0x01;
    enable();
	motor_1=1;
	delay(30);

	motor_1=0;
	delay(100);

	motor_2=1;
	delay(30);
	motor_2=0;
	start_lcd(0);
}


void wrong_password(){
	char i;

		rs=0;
		lcd=0x01;
		enable();
		lcd=0x81;
		enable();
		rs=1;
		k=0;
		while (msg_2[k]!='\0'){
			print_lcd(msg_2[k]);
			k++;
		}
		k=0;
		while (msg_1[k]!=' '){
			print_lcd(msg_1[k]);
			k++;
		}
		delay (50);
		
		for ( i = 0; i < 5; i++)
		{	
			buzzer=1;
			rs=0;
			lcd=0x0f;
			enable();
			delay(60);
			buzzer=0;
			lcd=0x0b;
			enable();
			delay(60);
			
		}
		

	start_lcd(0);
	count=0;
}


bit compare(){
	if(count!=6)return 0;
	k=0;
	while (password[k]!='\0'){
		if(input[k]!=password[k])return 0;
		k++;
	}
	count=0;
	return 1;
}


void new_password() interrupt 0{

	start_lcd(1);
	count=0;
	while(1){
		if(changed){break;}
		program(1);
	}
}


void backspace(){

	if(count!=0){
		rs=0;
		lcd=0x10;
		enable();
		rs=1;
		print_lcd(' ');
		rs=0;
		lcd=0x10;
		enable();
		rs=1;
		count--;
	}

}


void program(bit f){
	input_c=key_detect();
	if(input_c=='*'){
		flag=!flag;
		hide_password();
	}

	else if(input_c=='B'){
		backspace();
	}
	else if(input_c=='C'){
		start_lcd(f);
		count=0;
	}
	else if(input_c =='D'){
		if(f) change_password();
		else check_password();
	}
	else{
		if(input_c!='A'){
			print_lcd(hidden(input_c));
			input[count]=input_c;
			count++;
		}
	}
}


void change_password(){
	char i;
	if(count==6){
	
	for (i = 0; i < 6; i++)
	{
		changed =1;
		password[i]=input[i];
	}}
}
