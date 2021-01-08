#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALPHABET_LENGTH 26
#define ASCII_SHIFT 97
#define ASCII_SHIFT_NUMBER 48
#define EMPTY_STACK -1
#define BASE 10
#define READ '='


enum Instruction {
	INC, ADD, CLR, DJZ, JMP, HLT
};


typedef struct Entry{

	int id;
	enum Instruction Instruction;
	char *parameter1;
	char *parameter2;

}Entry;

typedef struct Value{

	char* digits;
}Value;

typedef struct Variables{

	Value* values;
}Variables;

typedef struct Looper { 

	Entry* entries;
	int entries_n;
	Variables vals;
}Looper;


typedef struct Stack{

	int* content;
	int stack_size;
	int content_size;
}Stack;

void pushStack(Stack *s, int a){
	if(s->stack_size == 0){
		s->content = malloc((unsigned)sizeof(int));
		s->stack_size = 1;
	}
	if(s->content_size == s->stack_size){
		int doublesize = 2*(s->stack_size);
		s->content = realloc(s->content,(unsigned)doublesize*sizeof(int));
		s->stack_size *= 2;
	}
	s->content[s->content_size] = a;
	s->content_size += 1;
}

int popStack(Stack *s){
	if(s->content_size == 0){
		return EMPTY_STACK;
	}
	int res = s->content[s->content_size-1];
	s->content_size -= 1;
	int half = (int)0.5*s->stack_size;
	if(s->content_size < half){
		s->content = realloc(s->content,(unsigned)half*sizeof(int));
		s->stack_size /= 2;
	}
	return res;
}

void printStack(Stack *s){
	for(int i = 0; i < s->content_size; i++){
		printf("%d ",s->content[i]);
	}
	printf("\n");
}


Variables initializeVariables(){
	Variables v;
	v.values = malloc(ALPHABET_LENGTH*(sizeof(Value)));
	for(int i = 0; i < ALPHABET_LENGTH; i++){
		v.values[i].digits = malloc(sizeof(char));
		v.values[i].digits[0] = '0';
	}
	return v;
}

void printVariableValue(Variables *v, char c){
	Value val = v->values[c - ASCII_SHIFT];
	printf("%s\n",val.digits);
}

int getLen(char* a){
	int i = 0;
	while(*(a+i) != '\0'){
		i++;
	}
	return i;
}

void detectifyLengths(char *a, char *b, char **shorter, char **longer){
	int b_len = getLen(b);
	int a_len = getLen(a);
	if(a_len > b_len){
		*shorter = b;
		*longer = a;
	}else{
		*shorter = a;
		*longer = b;
	}
}

void add(char *a, char *b){
	
	char *shorter = NULL,*longer = NULL;
	detectifyLengths(a,b,&shorter,&longer);
	int minLen = getLen(shorter),maxLen = getLen(longer);
	int next = 0;
	a = realloc(a,(unsigned)(maxLen+1)*sizeof(char));
	int diff = maxLen - minLen;
	int to_add;
	for(int i = maxLen - 1; i >= 0; i--){
		if(i < diff){
			to_add = 0;
		}else{
			to_add = shorter[i-diff] - ASCII_SHIFT_NUMBER;
		}
		int val =  to_add + (longer[i] - ASCII_SHIFT_NUMBER) + next;
		a[i+1] = (char)((val % BASE) + ASCII_SHIFT_NUMBER); 
		next = (val >= BASE)?1:0;
	}
	if(next == 1){
		a[0] = '1';
	}else{
		a[0] = '0';
	}
}

Looper initializeLooper(){
	Looper l;
	l.vals = initializeVariables();
	l.entries_n = 0;
	return l;
}


char* line(bool *isEnd){
	char* res = malloc(sizeof(char));
	int letters = 0, size = 1;
	char c;
	while((c = getchar()) != '\n' && c != EOF){
		if(letters == size){
			res = realloc(res,2*size);
			size *= 2;
		}
		res[letters] = c;
		letters++;
	}
	if(c == EOF){
		*isEnd = true;
	}
	return res;
}



void read(Looper *l){
	
	bool isEnd = false;
	while(!isEnd){
		char* currentLine = line(&isEnd);
		// line analysis here
		if(currentLine[0] == READ){
			printVariableValue(&l->vals,currentLine[1]);
		}else{
			printf("Ble ble");
		}
	}
	
}





int main(void){	
	Looper l = initializeLooper();
	read(&l);
}


