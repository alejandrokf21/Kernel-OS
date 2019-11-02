#include "kernel.h"
#include "utils.h"
#include "char.h"

uint32 vga_index;
static uint32 next_line_index = 1;
uint8 g_fore_color = RED, g_back_color = BLACK;
int digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

int opcion;
int operacion;
int operando1;
int operando2;
int total;

uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color) 
{
  uint16 ax = 0;
  uint8 ah = 0, al = 0;

  ah = back_color;
  ah <<= 4;
  ah |= fore_color;
  ax = ah;
  ax <<= 8;
  al = ch;
  ax |= al;

  return ax;
}

void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
  uint32 i;
  for(i = 0; i < BUFSIZE; i++){
    (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
  }
  next_line_index = 1;
  vga_index = 0;
}

void init_vga(uint8 fore_color, uint8 back_color)
{
  vga_buffer = (uint16*)VGA_ADDRESS;
  clear_vga_buffer(&vga_buffer, fore_color, back_color);
  g_fore_color = fore_color;
  g_back_color = back_color;
}

void print_new_line()
{
  if(next_line_index >= 55){
    next_line_index = 0;
    clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
  }
  vga_index = 80*next_line_index;
  next_line_index++;
}

void print_char(char ch)
{
  vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
  vga_index++;
}

void print_string(char *str)
{
  uint32 index = 0;
  while(str[index]){
    print_char(str[index]);
    index++;
  }
}

void print_int(int num)
{
  char str_num[digit_count(num)+1];
  itoa(num, str_num);
  print_string(str_num);
}

uint8 inb(uint16 port)
{
  uint8 ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
  return ret;
}

void outb(uint16 port, uint8 data)
{
  asm volatile("outb %0, %1" : "=a"(data) : "d"(port));
}

char get_input_keycode()
{
  char ch = 0;
  while((ch = inb(KEYBOARD_PORT)) != 0){
    if(ch > 0)
      return ch;
  }
  return ch;
}

/*
keep the cpu busy for doing nothing(nop)
so that io port will not be processed by cpu
here timer can also be used, but lets do this in looping counter
*/
void wait_for_io(uint32 timer_count)
{
  while(1){
    asm volatile("nop");
    timer_count--;
    if(timer_count <= 0)
      break;
    }
}

void sleep(uint32 timer_count)
{
  wait_for_io(timer_count);
}


void asignar_opcion()
{
  char ch = 0;
  char keycode = 0;
  do{
    keycode = get_input_keycode();
    ch = get_ascii_char(keycode); 
    if(keycode == KEY_ENTER){
      print_new_line();
      ch=0;
    }else{
      print_char(ch);
    }
    sleep(0x02FFFFFF);
  }while(ch > 0);
}

void scan(int * variable)
{
  char ch = 0;
  char keycode = 0;
  do{
    keycode = get_input_keycode();
    ch = get_ascii_char(keycode); 
    if(keycode == KEY_ENTER){
      print_new_line();
      ch=0;
    }else{
      *variable = (int)ch - 48;	
      print_char(ch);
    }
    sleep(0x02FFFFFF);
  }while(ch > 0);
}

void clear(){
init_vga(RED, BLACK);
}


//Realizar sonido con codigo assembler
void realizar_sonido(){


/*	asm(
"Beep PROC USES AX BX CX\n\t"
"IN AL, 61h\n\t"  
"PUSH AX\n\t"  
"MOV BX, 6818\n\t"
"MOV AL, 6Bh\n\t"
"OUT 43h, AL\n\t" 
"MOV AX, BX\n\t" 
"OUT 24h, AL\n\t"
"MOV AL, AH\n\t"  
"OUT 42h, AL\n\t"
"IN AL, 61h\n\t"
"OR AL, 3h\n\t"      
"OUT 61h, AL\n\t"
"MOV CX, 03h\n\t" 
"MOV DX 0D04h\n\t"
"MOV AX, 86h\n\t"
"INT 15h\n\t"
"POP AX\n\t"
"OUT 61h, AL\n\t"
"RET\n\t"
"BEEP ENDP\n\t"
);*/
    	/*asm(
	"in %al, $61h \n"
	"or %al, $00000011B\n"
	"out $61h, %al\n"
	"mov %cx, $10\n"	
	);*/
	/*
	"DELAY_LOOP:\n"
	"LOOP DELAY_LOOP\n"
	"IN %AL, 61H\n"
	"AND %AL, 11111100B\n"
	"OUT 61H, %AL\n"
	*/
}

//Apagar el sistema con codigo assembler
void apagar(){
	asm(
 	"movw $0x1000,%ax\n\t"
	"movw %ax,%ss\n\t"
	"movw $0xf000,%sp\n\t"
	"movw $0x5307,%ax\n\t"
	"movw $1,%bx\n\t"
	"movw $3,%cx\n\t"
	"int  $0x15\n\t"
	);
};

void kernel_main(void) 
{
	opcion = 9;	

	while(opcion != 3){
	clear();
  	print_string("Bienvenido a Cuervo OS");
  	print_new_line();print_new_line();
	print_string("MENU");
  	print_new_line();
	print_string("1. Calculadora");
  	print_new_line();	
	print_string("2. Emitir sonido");
  	print_new_line();
	print_string("3. Apagar el sistema");
	print_new_line();
	print_string("4. Integrantes");
  	print_new_line();print_new_line();
	print_string(" Ingrese el numero de opcion que desea realizar >> ");
  	scan(&opcion);
	print_new_line();

	switch(opcion)
	{
	case 1:
		opcion = 9;
        	//sleep(0x02FFFFFF);
		clear();	
		print_string("CALCULADORA");
		print_new_line();
		print_string("1. Suma");
  		print_new_line();	
		print_string("2. Resta");
		print_new_line();	
		print_string("3. Multiplicacion");
		print_new_line();	
		print_string("4. Division");
  		print_new_line();print_new_line();
		print_string(" Ingrese el numero de opcion que desea realizar >> ");
		scan(&opcion);
		switch(opcion)
		{
		case 1:
			clear();
			print_new_line();
			print_string("OPERACION SUMA");
			print_new_line();
			print_string("Ingrese lo que se le solicita:");	  		
			print_new_line();
			print_string("Primer sumando >> ");
			scan(&operando1);		
			print_new_line();
			print_string("Segundo sumando >> ");
			scan(&operando2);
			print_new_line();
			total = 0;
			total = operando1 + operando2;
			print_string("El total de ");
			print_int(operando1);
			print_string(" + ");
			print_int(operando2);
			print_string(" = ");
			print_int(total);
			print_new_line();
			print_string("Presione ENTER para continuar");
			scan(&opcion);
		break;
		case 2:
			clear();
			print_new_line();
			print_string("OPERACION RESTA");
			print_new_line();
			print_string("Ingrese lo que se le solicita:");	  			  		
			print_new_line();
			print_string("Minuendo >> ");
			scan(&operando1);		
			print_new_line();
			print_string("Sustraendo >> ");
			scan(&operando2);
			print_new_line();
			total = 0;
			print_string("La resta de ");
			print_int(operando1);
			print_string(" - ");
			print_int(operando2);
			print_string(" = ");
			if(operando1<operando2){
				print_string(" -");
				total = operando2 - operando1;
				print_int(total);
			}else{
				total = operando1 - operando2; 
				print_int(total);
			}
			print_new_line();
			print_string("Presione ENTER para continuar");
			scan(&opcion);
		break;
		case 3:
			clear();
			print_new_line();
			print_string("OPERACION MULTIPLICACION");
			print_new_line();
			print_string("Ingrese lo que se le solicita:");	  		
			print_new_line();
			print_string("Primer factor >> ");
			scan(&operando1);		
			print_new_line();
			print_string("Segundo factor >> ");
			scan(&operando2);
			print_new_line();
			total = 0;
			total = operando1 * operando2;
			print_string("El producto de ");
			print_int(operando1);
			print_string(" * ");
			print_int(operando2);
			print_string(" = ");
			print_int(total);
			print_new_line();
			print_string("Presione ENTER para continuar");
			scan(&opcion);
		break;
		case 4:
			clear();
			print_new_line();
			print_string("OPERACION DIVISION");
			print_new_line();
			print_string("Ingrese lo que se le solicita:");	  		
			print_new_line();
			print_string("Dividendo >> ");
			scan(&operando1);		
			print_new_line();
			print_string("Divisor >> ");
			scan(&operando2);
			print_new_line();
			total = 0;
			total = operando1 / operando2;
			print_string("El coeficiente de ");
			print_int(operando1);
			print_string(" / ");
			print_int(operando2);
			print_string(" = ");
			print_int(total);
			print_new_line();
			print_string("Presione ENTER para continuar");
			scan(&opcion);
		break;		
		}

		

    	break;
	case 2:
		clear();
		print_string("EMITIR SONIDO");
		realizar_sonido();
		print_string("Presione ENTER para continuar");
		scan(&opcion);
	break;
	case 4:
		clear();
		print_string("INTEGRANTES:");
		print_new_line();
		print_string("Carlos Emmanuel Lopez Ruano");
		print_new_line();
		print_string("Victor Daniel Salguero Sanabria");
		print_new_line();
		print_string("Manuel Alejandro Oliva Ruano");
		print_new_line();
		print_new_line();
		print_string("Presione ENTER para continuar");
		scan(&opcion);	

	break;
	case 3:
		opcion = 3;	
		clear();
		apagar();
	break;	    	
		
	}//Fin Switch - Fin Implementacion de Funcionalidades
	
	} // Fin del while
	
}

