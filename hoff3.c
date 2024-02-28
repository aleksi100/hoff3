#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./argparser.h"


#define FILENAME "data.hoff3"


typedef struct CODE{
    int code;
    int len;
} CODE;
typedef struct NODE{
    char letter;
    __uint8_t freq;
    char used;
    struct NODE *left;
    struct NODE *right;
    struct CODE code;
} NODE;





#define set_bit(bf, num) bf |= ((__uint64_t)0x1<<num)
#define clr_bit(bf, num) bf &= ~((__uint64_t)0x1<<num)
#define test(bf, num) (bf>>num & 1)



void print_tabs(int tabs){
    for(int i=0;i<tabs;i++){
        printf("\t");
    }
}
void print_nodes(NODE *node, int level){
    if(node->letter != 0){
        print_tabs(level);
        
        return;
    }else{
        print_tabs(level);
        printf("node \n");
    }
    

    print_tabs(level);
    printf("LEFT:\n");
    print_nodes(node->left, level+1);

    print_tabs(level);
    printf("RIGHT:\n");
    print_nodes(node->right, level+1);

    print_tabs(level);
    printf("Done\n");
}



int make_letter_nodes(char *str, NODE *node){
    NODE *ptr;
    char is_found;
    int how_many = 0;
    while(*str != 0){
       if(*str < 0) {
           str++;
           continue;
       }
        ptr = node;
        is_found = 0;
        while(ptr->letter != 0){
            if(ptr->letter == *str){
               
                ptr->freq += 1;
                is_found = 1;
                break;
            }
            ptr++;
        }
        
        if(is_found==0){
            
            how_many++;
            ptr->letter = *str;
            ptr->freq = 1;
        }
        str++;
    }
    return how_many;
}

NODE * give_lowest(NODE *ptr){
    int lowest = 1000;
    NODE *return_ptr = ptr;
    while(ptr->freq > 0){
        if(ptr->freq < lowest && ptr->used == 0) {
            lowest = ptr->freq; 
            return_ptr = ptr;
        }
        ptr++;
    }
   
    return return_ptr; 
}

int make_parent_node(NODE *node){
    NODE *eka = give_lowest(node);
    eka->used = 1;
    NODE *toka = give_lowest(node);
    if(toka->used == 1){
        eka->used = 0;
        return 1;
    }

    toka->used = 1;
    NODE *parent = node;
    while(parent->freq > 0){
        parent++;
    }
    parent->freq = eka->freq + toka->freq;

    parent->left = eka;
    parent->right = toka;
    return 0;
}

void code_for_node(NODE *root, CODE code){
    if(root->letter > 0){
        root->code = code;
        return;
    }
    // left
    code.len++;
    code_for_node(root->left, code);
    code.len--;

    // right
    set_bit(code.code, code.len);
    code.len++;
    code_for_node(root->right, code);
    
}

int make_code(char *str, NODE *node, __uint8_t *final_code){
    NODE *ptr;
    int len = 0;// code len in bits
    int bytes = 0;
    int calc = 0;
    while(*str != 0){
        ptr = node;

        while(ptr->letter != 0){
            
            if(ptr->letter == *str){
            
                for(int i=0;i<ptr->code.len;i++){
                    bytes = len/8;
                    calc = len - bytes*8;
                    
                    if(calc == 0 && len > 1) final_code += 1;
                    
                    if(test(ptr->code.code, i)){
                        
                        set_bit(*final_code, calc);
                    }else{
                        
                        clr_bit(*final_code, calc);
                    }
                    
                    len++;
                }
                break;
            }
            ptr++;
        }
        str++;
    }
    
    return len;
}
void store
(__uint8_t letters_len, __uint16_t code_len, NODE *node, __uint8_t *code){
    FILE *file = fopen(FILENAME, "wb");
    if(file == NULL) return;

    fwrite(&letters_len, sizeof(letters_len), 1, file);
    fwrite(&code_len, sizeof(code_len), 1, file);

    for(int i=0;i<letters_len;i++){
        fwrite(&node->letter, sizeof(char), 1, file);
        fwrite(&node->freq, sizeof(__uint8_t), 1, file);
        node += 1;
    }
    
    while(1){
        // write every code byte to file
        fwrite(code, sizeof(__uint8_t), 1, file);
        if(code_len<=8) break;
        code_len -= 8;
        code += 1;
    }

    fclose(file);
}

void map_text(__uint8_t *code, int len, NODE *root, char *text){
    NODE *ptr = root;
    int bytes = 0;
    int calc = 0;
    for(int i=0; i<len; i++){
       
        bytes = i/8;
        calc = i - bytes*8;
        
        if(calc == 0 && i>1){
            code += 1;
        }
        
        if(test(*code, calc)){
            ptr = ptr->right;
        }else{
            ptr = ptr->left;
        }

        // found letter
        if(ptr->letter != 0){
            
            strncat(text, &ptr->letter, 1);
            ptr = root;
        }
        
    }    
}


NODE *find_root(NODE *node){
    while(node->used == 1){
        node++;
    }
    return node;
}

int generate_code(NODE *node, char *str, __uint8_t *final_code){
    NODE *root = find_root(node);
    CODE code;
    code.code = 0;
    code.len = 0;
    code_for_node(root, code);

    
    int len = make_code(str, node, final_code);
    return len;
}

void initalize(NODE *node, int len){
    
    for(int i=0;i<len;i++){
        node->letter = 0;
        node->freq = 0;
        node->used = 0;
        node->left = NULL;
        node->right = NULL;
        node->code.code = 0;
        node->code.len = 0;
        node++;
    }
}

int encode(char *my_text){
    

    int how_long_text = strlen(my_text);

   
    NODE *nodes = malloc(150*sizeof(NODE));
    if(nodes == NULL) {
        printf("couldnt malloc memory for nodes\n");
        return 1;
    }

    // setting to zero
    initalize(nodes, 150);

    // make node for every diffrent letter
    __uint8_t letters = make_letter_nodes(my_text, nodes);
    
    int i=0;
    int total_nodes = letters;
    // loop to make the tree
    while(i==0){
        i = make_parent_node(nodes);
        total_nodes += 1;
    }
    
    
    
    
    
    // taking space for the code
    int estimated_code_len = how_long_text*5/8+1;
    
    __uint8_t *final_code = malloc(estimated_code_len * sizeof(__uint8_t));

    if(final_code == NULL){
        printf("couldnt malloc memory for code \n");
        return 1;
    }
    
    __uint_least16_t code_len = generate_code(nodes, my_text, final_code);
    


    store(letters, code_len, nodes, final_code);

    
    free(final_code);
    free(nodes);  
    return 0;
}
int decode(){
    FILE *file = fopen(FILENAME, "rb");
    if(file == NULL) {
        printf("couldnt open a file\n");
        return 1;
    }

    __uint8_t letters_len;
    __uint16_t code_len;
    fread(&letters_len, sizeof(letters_len), 1, file);
    fread(&code_len, sizeof(code_len), 1, file);

    
    
    
    NODE *nodes = malloc(150*sizeof(NODE));

    __uint8_t *code = malloc(10*(code_len/8+1) * sizeof(__uint8_t));
    if(nodes == NULL || code == NULL) {
        printf("couldnt malloc memory for code or nodes \n");
        fclose(file);
        return 1;
    }

    // setting to zero
    initalize(nodes, 150);
    

    NODE *nodes_ptr = nodes;
    for(int i=0;i<letters_len;i++){
        fread(&nodes_ptr->letter, sizeof(char), 1, file);
        fread(&nodes_ptr->freq, sizeof(__uint8_t), 1, file);
        nodes_ptr += 1; // next node
    }

    // temp ptr for code
    __uint8_t *code_ptr = code;
    while(1){
        fread(code_ptr, sizeof(__uint8_t), 1, file);
        if(*code_ptr == 0) break;
        code_ptr += 1;
    }
    fclose(file);
    

    int i=0;
    // loop to make the tree
    while(i==0){
        i = make_parent_node(nodes);
        
    }
    
    char *text;
    text = malloc(1000*sizeof(char));
    if(text == NULL){
        printf("couldnt malloc memory for text");
        return 1;
    }
    NODE *root = find_root(nodes);
    
    map_text(code, code_len, root, text);

    
    printf("%s\n", text);

    free(nodes);
    free(code);
    free(text);
    return 0;
}

// tarkista koodi muisti

int main(int argc, char *argv[]){
    struct OPTIONS_STRUCT *OPTIONS = parse(argc, argv);
    printf("operation = %x\n", OPTIONS->operation);
    printf("inFile = %s\n", OPTIONS->inFile);
    printf("outFile = %s\n", OPTIONS->outFile);

    // if(argc == 1) return 1;


    // if(strcmp(argv[1], "encode") == 0) encode(argv[2]);
    // if(strcmp(argv[1], "decode") == 0) decode();
    
}