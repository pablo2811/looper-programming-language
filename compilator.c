#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALPHABET_LENGTH 26
#define ASCII_SHIFT 97
#define ASCII_SHIFT_NUMBER 48
#define EMPTY_STACK -1
#define BASE 10
#define READ '='
#define OPEN_BRACKETS '('
#define CLOSE_BRACKETS ')'



enum Instruction {
	INC, ADD, CLR, DJZ, JMP, HLT
};


typedef struct Entry{

	int id;
	enum Instruction instruction;
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
	int entries_size;
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
	l.entries_size = 1;
	l.entries = malloc(sizeof(Entry));
	return l;
}


char* line(int *len, bool *isEnd){
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
	*len = letters;
	return res;
}

void addEntry(Entry e,Looper *l){
	if(l->entries_size == l->entries_n){
		l->entries = realloc(l->entries,(unsigned)2*(l->entries_size)*sizeof(Entry));
		l->entries_size = 2*(l->entries_size);
	}
	l->entries[l->entries_n] = e;
	l->entries_n += 1;
}

Entry createEntry(int id, enum Instruction i, char* parameter1, char* parameter2){
	Entry e;
	e.id = id;
	e.instruction = i;
	e.parameter1 = parameter1; 
	e.parameter2 = parameter2;	
	return e;
}

void printEntries(Looper *l){
	printf("--------------------------\n");
	for(int i = 0; i < l->entries_n; i++){
		Entry e = l->entries[i];
		char *pol;
		switch(e.instruction){
			case ADD:
				pol = "ADD";
				break;
			case INC:
				pol = "INC";
				break;
			case JMP:	
				pol = "JMP";
				break;
			case DJZ:
				pol = "DJZ";
				break;
			case HLT:
				pol = "HLT";
				break;
			case CLR:
				pol = "CLR";
				break;
		}
		char* val1 = "";
		char* val2 = "";
		if(e.parameter1 != NULL){
			val1 = e.parameter1;
		}
		if(e.parameter2 != NULL){
			val2 = e.parameter2;
		}
		printf("%3d.| %3s | %7s | %7s |\n", e.id,pol,val1,val2);
	}
	printf("--------------------------\n");


}


int countDigits(int a){
	int count = 0;
	do
    {
        count++;
        a /= BASE;
    }while(a != 0);
    return count;
}

char* intToCharPointer(int a){
	int n = countDigits(a);
	char *num = malloc((unsigned) n*sizeof(char));
	int i = n - 1;
	do
    {
        num[i] = a%BASE + '0';
        a /= BASE;
        i--;
    }while(i >= 0);
    return num;
}

void initializeStack(Stack *s){
	s->content = malloc(sizeof(int));
	s->stack_size = 1;
	s->content_size = 0;
}




void compileLine(Looper *l,char* cline,int n){
	Stack s;
	initializeStack(&s);
	int i = 0;
	while(i < n){
		if(cline[i] == OPEN_BRACKETS){
			int j = i + 1;
			while(cline[j] != OPEN_BRACKETS && cline[j] != CLOSE_BRACKETS){
				j++;
			}
			bool optimizable = (cline[j] == CLOSE_BRACKETS);
			char *mainVariable = malloc((unsigned)sizeof(char));
			mainVariable[0] = cline[++i];
			if(optimizable){
				i++;
				while(i < j){
					char *variable_to_add = malloc((unsigned)sizeof(char));
					variable_to_add[0] = cline[i];
					Entry e = createEntry(l->entries_n,ADD,variable_to_add,mainVariable);
					addEntry(e,l);
					i++;
					}
				Entry closing = createEntry(l->entries_n,CLR,mainVariable,NULL);
				addEntry(closing,l);
				i++;
			}else{ 
				pushStack(&s,l->entries_n);
				i++;
				Entry djz = createEntry(l->entries_n,DJZ,mainVariable,NULL);
				addEntry(djz,l);
				while(i < j){
					char *variable_to_inc = malloc((unsigned)sizeof(char));
					variable_to_inc[0] = cline[i];
					Entry e_inc = createEntry(l->entries_n,INC,variable_to_inc,NULL);
					addEntry(e_inc,l);
					i++;
				}
			}
		}else if(cline[i] == CLOSE_BRACKETS){
			int j = popStack(&s);
			l->entries[j].parameter2 = intToCharPointer(l->entries_n+1);
			Entry e_jmp = createEntry(l->entries_n,JMP,intToCharPointer(j),NULL);
			addEntry(e_jmp,l);
			i++;
		}else{

			char *variable_to_inc = malloc((unsigned)sizeof(char));
			variable_to_inc[0] = cline[i];
			Entry e_inc = createEntry(l->entries_n,INC,variable_to_inc,NULL);
			addEntry(e_inc,l);
			i++;
		}
	}
	addEntry(createEntry(l->entries_n,HLT,NULL,NULL),l);
	printEntries(l);

}





void read(Looper *l){
	
	bool isEnd = false;
	while(!isEnd){
		int len;
		char* currentLine = line(&len,&isEnd);
		// line analysis here
		if(currentLine[0] == READ){
			printVariableValue(&l->vals,currentLine[1]);
		}else{
			compileLine(l,currentLine,len);
		}
	}
	
}





int main(void){	
	Looper l = initializeLooper();
	read(&l);
}


