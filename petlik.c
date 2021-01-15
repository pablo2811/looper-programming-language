#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ALPHABET_LENGTH 26
#define ASCII_SHIFT 97
#define EMPTY_STACK -1
#define BASE 10
#define READ '='
#define OPEN_BRACKETS '('
#define CLOSE_BRACKETS ')'
#define NO_PARAM -2


enum Instruction {
    INC, ADD, CLR, DJZ, JMP, HLT
};


typedef struct Entry{
    enum Instruction instruction;
    int parameter1;
    int parameter2;
}Entry;

typedef struct Looper{
    int** variables;
    int* lengths;
}Looper;

typedef struct Stack{

    int* content;
    int stack_size;
    int content_size;
}Stack;

// dodawanie nawiasów na stos
void pushStack(Stack *s, int a){
    if(s->stack_size == 0){
        s->content = malloc((unsigned)sizeof(int));
        s->stack_size = 1;
    }
    if(s->content_size == s->stack_size){
        int double_size = 2*(s->stack_size);
        s->content = realloc(s->content,(unsigned)double_size*sizeof(int));
        s->stack_size *= 2;
    }
    s->content[s->content_size] = a;
    s->content_size += 1;
}

// usuwanie nawiasów ze stosu
int popStack(Stack *s){
    if(s->content_size == 0){
        return EMPTY_STACK;
    }
    int res = s->content[s->content_size-1];
    s->content_size -= 1;
    int half = (int)0.5*s->stack_size;
    if(s->content_size < half){
        s->content = realloc(s->content,(long unsigned int)half*sizeof(int));
        s->stack_size /= 2;
    }
    return res;
}

// wypisywanie wartosci zmiennej
void printVariable(int* val,int n){
    for(int i = n - 1; i >= 0; i--){
        printf("%d",val[i]);
    }
    printf("\n");
}

//utworzenie stosu 
Stack* makeStack(){
    Stack *s = malloc(sizeof(Stack));
    s->content = malloc(sizeof(int));
    s->stack_size = 1;
    s->content_size = 0;
    return s;
}

// utworzenie głównego structa
Looper* makeLooper(){
    Looper* l = malloc(sizeof(Looper));

    int **arr = malloc((ALPHABET_LENGTH+1)*sizeof(int *));
    int *len = malloc((ALPHABET_LENGTH+1)*sizeof(int));
    for(int i = 0; i < ALPHABET_LENGTH; i++){
        arr[i] = malloc(sizeof(int));
        arr[i][0] = 0;
        len[i] = 1;
    }
    arr[ALPHABET_LENGTH] = malloc(sizeof(int*));
    arr[ALPHABET_LENGTH][0] = 1;
    len[ALPHABET_LENGTH] = 1;
    l->variables = arr;
    l->lengths = len;
    return l;
}

// czytanie lini podanych na wejściu
int* line(int *len, bool *isEnd){
    int* res = malloc(sizeof(int));
    int letters = 0, size = 1;
    int c;
    while((c = getchar()) != '\n' && c != EOF){
        if(letters == size){
            int m = 2*size;
            res = realloc(res,(long unsigned int)m*sizeof(int));
            size *= 2;
        }
        res[letters] = c;
        letters++;
    }
    res = realloc(res,(long unsigned int)letters*sizeof(int));
    if(c == EOF){
        *isEnd = true;
    }
    *len = letters;
    return res;
}

// dodawanie nowego rekordu w kodzie maszynowym
void addEntry(Entry **entries, Entry new_entry, int *n, int *size){
    if(*n == *size){
        int new_size = 2*(*n);
        *entries = realloc(*entries,(long unsigned int) new_size*sizeof(Entry));
        *size *=2;
    }
    (*entries)[*n] = new_entry;
    (*n)++;
}

// tworzenie nowego rekordu do kodu maszynowego
Entry makeEntry(enum Instruction i,int param1,int param2){
    Entry e;
    e.instruction = i;
    e.parameter1 = param1;
    e.parameter2 = param2;
    return e;
}

// kompilacja programu w języku "Pętlik"
Entry* compile(const int* cline, int n, int *number_of_entries){
    Stack *s = makeStack();
    int i = 0;
    Entry* entries = malloc(sizeof(Entry));
    *number_of_entries = 0;
    int size = 1;
    while(i < n){
        if(cline[i] == OPEN_BRACKETS){
            int j = i + 1;
            while(cline[j] != OPEN_BRACKETS && cline[j] != CLOSE_BRACKETS){
                j++;
            }
            bool opt = (cline[j] == CLOSE_BRACKETS);
            int mainVar = cline[++i];
            if(opt){
                i++;
                while(i < j){
                    int variable_to_add = cline[i];
                    Entry e = makeEntry(ADD,variable_to_add,mainVar);
                    addEntry(&entries,e,number_of_entries,&size);
                    i++;
                }
                Entry closing = makeEntry(CLR,mainVar,NO_PARAM);
                addEntry(&entries,closing,number_of_entries,&size);
                i++;
            }else{
                pushStack(s,*number_of_entries);
                i++;
                Entry djz = makeEntry(DJZ,mainVar,NO_PARAM);
                addEntry(&entries,djz,number_of_entries,&size);
                while(i < j){
                    int variable_to_inc = cline[i];
                    Entry e_inc = makeEntry(INC,variable_to_inc,NO_PARAM);
                    addEntry(&entries,e_inc,number_of_entries,&size);
                    i++;
                }
            }
        }else if(cline[i] == CLOSE_BRACKETS){
            int j = popStack(s);
            entries[j].parameter2 = (*number_of_entries) + 1;
            Entry e_jmp = makeEntry(JMP,j,NO_PARAM);
            addEntry(&entries,e_jmp,number_of_entries,&size);
            i++;
        }else{
            int variable_to_inc = cline[i];
            Entry e_inc = makeEntry(INC,variable_to_inc,NO_PARAM);
            addEntry(&entries,e_inc,number_of_entries,&size);
            i++;
        }
    }
    Entry e_hlt = makeEntry(HLT,NO_PARAM,NO_PARAM);
    addEntry(&entries,e_hlt,number_of_entries,&size);
    free(s->content);
    free(s);
    return entries;
}

// czyszczenie wartości zmiennej
void clear(int **a,int *lenA){
    (*a)[0] = 0;
    *a = realloc(*a,(long unsigned int)sizeof(int));
    *lenA = 1;
}

// sprawdzanie czy wartość zmiennej jest zerowa
bool isZero(const int *a,int n){
    return(n == 1 && *a == 0);
}

// dekrementacja wartości zmiennej
void decrement(int **vars,int *lens,int dec){
    if(vars[dec][0] == 1 && lens[dec] == 1){
        vars[dec][0] = 0;
    }else{
        int i = 0;
        while(vars[dec][i] == 0 && i < lens[dec]){
            vars[dec][i] = 9;
            i++;
        }
        vars[dec][i]--;
        if(vars[dec][i] == 0 && i == lens[dec] - 1){
            lens[dec]--;
            vars[dec] = realloc(vars[dec],(long unsigned int)lens[dec]*sizeof(int));
        }
    }
}

// dodowanie dwóch zmiennych do siebie
void add(int **vars, int *lens, int addTo, int added) {
    int *shorter = NULL;
    int *longer = NULL;
    int minLen,maxLen;
    bool isLonger = false;
    if(lens[addTo] > lens[added]){
        minLen = lens[added];
        maxLen = lens[addTo];
        isLonger = true;
    }else{
        minLen = lens[addTo];
        maxLen = lens[added];
    }
    int next = 0;
    int more_space = maxLen + 1;
    vars[addTo] = realloc(vars[addTo],(long unsigned int)more_space*sizeof(int));
    if(isLonger){
        shorter = vars[added];
        longer = vars[addTo];
    }else{
        shorter = vars[addTo];
        longer = vars[added];
    }
    for(int i = 0; i < maxLen; i++){
        int val = 0;
        if(i <  minLen){
            val = shorter[i];
        }
        int x = val + longer[i] + next;
        vars[addTo][i] = x%BASE;
        next = x/BASE;
    }
    if(next){
        vars[addTo][maxLen]= 1;
        lens[addTo] = maxLen + 1;
    }else{
        vars[addTo] = realloc(vars[addTo],(long unsigned int)maxLen*sizeof(int));
        lens[addTo] = maxLen;
    }
}


// interpretacja kodu
void interpret(Entry *entries, int n, Looper *l) {
    int i = 0;
    int **vars = l->variables;
    int *lens = l->lengths;
    while(i < n){
        Entry e = entries[i];
        int f_param_shifted = e.parameter1-ASCII_SHIFT;
        int s_param_shifted = e.parameter2-ASCII_SHIFT;
        switch (e.instruction) {
            case ADD:
               add(vars,lens,f_param_shifted,s_param_shifted);
                i++;
                break;
            case INC:
                add(vars,lens,f_param_shifted,ALPHABET_LENGTH);
                i++;
                break;
            case JMP:
                i = f_param_shifted+ASCII_SHIFT;
                break;
            case CLR:
                clear(&(vars[f_param_shifted]),&(lens[f_param_shifted]));
                i++;
                break;
            case DJZ:
                if(isZero(vars[f_param_shifted],lens[f_param_shifted])){
                    i = s_param_shifted+ASCII_SHIFT;
                }else{
                    decrement(vars,lens,f_param_shifted);
                    i++;
                }
                break;
            case HLT:
                i++;
                break;
        }
    }
}


// główna funkcja sterująca przepływem
void run(){
    bool isEnd = false;
    int len;
    Looper *lp = makeLooper();
    while(!isEnd){
        int *l = line(&len,&isEnd);
        if(isEnd){
            break;
        }
        if(l[0] == READ){
            int var = l[1];
            int *number = lp->variables[var - ASCII_SHIFT];
            int n = lp->lengths[var-ASCII_SHIFT];
            printVariable(number,n);
        }else{
            int n_entries;
            Entry *entries = compile(l,len,&n_entries);
            interpret(entries,n_entries,lp);
            free(entries);
        }
        free(l);
    }

    for(int i = 0; i < ALPHABET_LENGTH + 1; i++){
        free(lp->variables[i]);
    }
    free(lp->variables);
    free(lp->lengths);
    free(lp);
}



int main(void){
    run();
}