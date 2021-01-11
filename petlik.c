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


typedef struct Looper {

	Entry* entries;
	int entries_size;
	int entries_n;
	char** variables;
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



void printVariableValue(Looper *l, char c){
	char* value = l->variables[c - ASCII_SHIFT];
	printf("%s\n",value);
}

int getLen(const char* a){
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

void add(char **a, char *b){

	char *shorter = NULL,*longer = NULL;
	detectifyLengths(*a,b,&shorter,&longer);
	int minLen = getLen(shorter);
	int maxLen = getLen(longer);
	int next = 0;
	char *temp = malloc((unsigned)(maxLen+2)*sizeof(char));
	for(int i = 0; i < maxLen+2; i++){
		temp[i] = '0';
	}
	temp[maxLen+1] = '\0';
	int diff = maxLen - minLen;
	int to_add;
	for(int i = maxLen - 1; i >= 0; i--){
		if(i < diff){
			to_add = 0;
		}else{
			to_add = shorter[i-diff] - ASCII_SHIFT_NUMBER;
		}
		int val =  to_add + (longer[i] - ASCII_SHIFT_NUMBER) + next;
		temp[i+1] = (char)((val % BASE) + ASCII_SHIFT_NUMBER);
		next = (val >= BASE)?1:0;
	}
	if(next == 1){
		temp[0] = '1';
	}else{
		temp++;
	}
	*a = temp;
}

Looper initializeLooper(){
	Looper l;
	l.variables = malloc(ALPHABET_LENGTH*sizeof(char*));
	for(int x = 0; x < ALPHABET_LENGTH;x++){
		l.variables[x] = "0";
	}
	l.entries_n = 0;
	l.entries_size = 1;
	return l;
}


char* line(int *len, bool *isEnd){
	char* res = malloc(sizeof(char));
	int letters = 0, size = 1;
	int c;
	while((c = getchar()) != '\n' && c != EOF){
		if(letters == size){
			int m = 2*size;
			res = realloc(res,(unsigned)m*sizeof(char));
			size *= 2;
		}
		res[letters] = (char)c;
		letters++;
	}
	if(c == EOF){
		*isEnd = true;
	}
	*len = letters;
	return res;
}

void addEntry(Entry e,Looper *l){
	if(l->entries_size == 0){
		l->entries = malloc((unsigned)sizeof(Entry));
		l->entries_size = 1;
	}
	if(l->entries_size == l->entries_n){
		int m = 2*(l->entries_size);
		l->entries = realloc(l->entries,(unsigned)m*sizeof(Entry));
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

// Only 4 testing purposes.
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
        num[i] = (char)(a%BASE + '0');
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

void killStack(Stack *s){
	free(s->content);
	free(s);
}


void compileLine(Looper *l,char* cline,int n){
	Stack s;
	initializeStack(&s);
	int i = 0;
	l->entries = malloc(sizeof(Entry));
	l->entries_size = 1;
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
	//killStack(&s);
}

void clear(char **a){
	*a = "0";
}


void inc(char **a){
	char *b = "1";
	add(a,b);
}

bool isZero(char *a){
	while(*a == '0'){
		a++;
	}
	return (*a == '\0');
}

int charPointerToInt(char* a){
	int num = 0;
	while(*a) {
		num = ((*a) - '0') + num * 10;
		a++;
	}
	return num;
}


void decrement(char *a){
	int i = getLen(a) - 1;
	while(i >= 0 && a[i] == '0'){
		a[i] = '9';
		i--;
	}
	char g = a[i] - 1;
	a[i] = g;
}

void freeResources(Looper *l){
	for(int i = 0; i < l->entries_n; i++){
		free(l->entries[i].parameter1);
		free(l->entries[i].parameter2);
	}
	free(l->entries);
	l->entries_n = 0;
	l->entries_size = 0;
}

void free2D(char **ptr,int n){
	for(int x = 0; x < n; x++){
		free(ptr[x]);
	}
	free(ptr);
}


void interpreteCode(Looper *l){
	char **variableTable = l->variables;
	int i = 0;
	while(i < l->entries_n){
		Entry act = l->entries[i];
		char *p1 = act.parameter1;
		char *p2 = act.parameter2;
		switch(act.instruction){
				case ADD:
					add(&variableTable[p1[0]-ASCII_SHIFT],variableTable[p2[0]-ASCII_SHIFT]);
					i++;
				break;
			case INC:
					inc(&variableTable[p1[0]-ASCII_SHIFT]);
					i++;
				break;
			case JMP:
					i = charPointerToInt(p1);
				break;
			case DJZ:
				if(isZero(variableTable[p1[0]-ASCII_SHIFT])){
					i = charPointerToInt(p2);
				}else{
					decrement(variableTable[p1[0]-ASCII_SHIFT]);
					i++;
				}
				break;
			case HLT:
				i++;
				break;
			case CLR:
				clear(&variableTable[p1[0]-ASCII_SHIFT]);
				i++;
				break;
		}
	}

	freeResources(l);
}




void read(Looper *l){

	bool isEnd = false;
	while(!isEnd){
		int len;
		char* currentLine = line(&len,&isEnd);
		// line analysis here
		if(currentLine[0] == READ){
			printVariableValue(l,currentLine[1]);
		}else{
			compileLine(l,currentLine,len);
			interpreteCode(l);
		}
//		 free(currentLine);
	}
	free2D(l->variables,ALPHABET_LENGTH);
	free(l);
}


int main(void){
	Looper l = initializeLooper();
	read(&l);
}
