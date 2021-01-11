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

	int* digits;
	int size;
}Value;


typedef struct Looper { 

	Entry* entries;
	int entries_size;
	int entries_n;
	Value *vals;
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


Value* initializeVariables(){
	Value *v;
	v  = malloc(ALPHABET_LENGTH*(sizeof(Value)));
	for(int i = 0; i < ALPHABET_LENGTH; i++){
		Value this;
		this.digits = malloc((unsigned)sizeof(int));
		this.size = 1;
		this.digits[0] = 0; 
		v[i] = this;
	}
	return v;
}

void printVariableValue(Value *v, char c){
	Value val = v[c - ASCII_SHIFT];
	int *d = v->digits;
	while(d != NULL){
		printf("%d",*d);
		d++;
	}
}

int getLen(int* a){
	int i = 0;
	while(a != NULL){
		a++;
		i++;
	}
	return i;
}

void detectifyLengths(int *a, int *b, int **shorter, int **longer){
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

Value add(Value *a, Value *b){
	
	int *shorter = NULL,*longer = NULL;
	int maxLen,minLen;
	if(a->size > b->size){
		shorter = b->digits;
		longer = a->digits;
		maxLen = a->size;
		minLen = b->size;
	}else{
		shorter = a->digits;
		longer = b->digits;
		maxLen = b->size;
		minLen = a->size;
	}
	int next = 0;
	int *temp = realloc(temp,(unsigned)(maxLen+1)*sizeof(int));
	int diff = maxLen - minLen;
	int to_add;
	for(int i = maxLen - 1; i >= 0; i--){
		if(i < diff){
			to_add = 0;
		}else{
			to_add = shorter[i-diff];
		}
		int val =  to_add + longer[i] + next;
		temp[i+1] = val % BASE; 
		next = (val >= BASE)?1:0;
	}
	Value new_value;
	if(next == 1){
		temp[0] = 1;
		new_value.size = maxLen+1;
		new_value.digits = malloc((unsigned)(maxLen+1)*sizeof(int));
		// a->digits[0] = 1;
		// (a->size)++;
	}else{
		temp++;
		new_value.size = maxLen;
		new_value.digits = malloc((unsigned)(maxLen)*sizeof(int));
		// (a->digits)++;
		// a->digits = temp;
	}
	new_value.digits = temp;
	return new_value;

}

Looper initializeLooper(){
	Looper l;
	l.vals = initializeVariables();
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
}

void clear(Value *a){
	a->digits = realloc(a->digits,sizeof(int));
	a->digits = 0;
	a->size = 1;
}


void inc(Value *a){
	int* b = malloc(sizeof(int));
	*b = 1;
	Value x;
	x.size = 1;
	x.digits = malloc(sizeof(int));
	x.digits[0] = 1;
	add(a,&x);
}

bool isZero(int *a){
	while(a != NULL && *a == 0){
		a++;
	}
	return (a == NULL);
}

int charPointerToInt(char* a){
	int num = 0;
	while(*a) { 
		num = ((*a) - '0') + num * 10;
		a++;   
	}
	return num;
}


void decrement(Value *v){
	int i = v->size - 1;
	int *a = v->digits;
	while(i >= 0 && *(a+i) == 0){
		*(a+i) = 9;
		i--;
	}
	int g = *(a+i) - 1;
	*(a+i) = g; 
}

void freeResources(Looper *l){
	free(l->entries);
	l->entries_n = 0;
	l->entries_size = 0;
}




void interpreteCode(Looper *l){
	Value *v = l->vals;
	int i = 0;
	while(i < l->entries_n){
		Entry act = l->entries[i];
		char* p1 = act.parameter1;
		char* p2 = act.parameter2;
		switch(act.instruction){
				case ADD:
					v[p1[0] - ASCII_SHIFT] = add(&(v[p1[0] - ASCII_SHIFT]),&v[p2[0] - ASCII_SHIFT]);
					i++;
				break;
			case INC:
					inc(&v[p1[0] - ASCII_SHIFT]);
					i++;
				break;
			case JMP:	
					i = charPointerToInt(p1);
				break;
			case DJZ:
				if(isZero(v[p1[0] -ASCII_SHIFT].digits)){
					i = charPointerToInt(p2);
				}else{
					decrement(&v[p1[0]-ASCII_SHIFT]);
					i++;
				}
				break;
			case HLT:
				i++;
				break;
			case CLR:
				clear(&v[p1[0] -ASCII_SHIFT]);
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
			printVariableValue(l->vals,currentLine[1]);
		}else{
			compileLine(l,currentLine,len);
			interpreteCode(l);
		}
	}
}

void printValue(Value *v){
	for(int i = 0 ; i < v->size; i++){
		printf("%d",*(v->digits));
		(v->digits)++;
	}
	printf("\n");
}


int main(void){	
	Looper l = initializeLooper();
	read(&l);

}