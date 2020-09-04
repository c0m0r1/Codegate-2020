#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PAGE_SIZE 0x1000
#define PAGE_ROUND_UP(x) ( (((__uint64_t)(x)) + PAGE_SIZE-1)  & (~(PAGE_SIZE-1)) ) 

typedef struct bytecode_ {
    struct bytecode_* prev;
    struct bytecode_* next;
    unsigned char opcode;
    __uint64_t data[];
} bytecode;

typedef struct matrix_ {
    struct matrix_* prev;
    struct matrix_* next;
    __uint64_t id;
    __uint64_t row;
    __uint64_t col;
    __uint64_t **data;
} matrix;

matrix * search_matrix(matrix * head, __uint64_t id){
    while(head != NULL && head->id != id) {
        //printf("%s : %p (id %lu, row %lu, col %lu)\n", __FUNCTION__, head, head->id, head->row, head->col);
        head = head->next;
    }
    //printf("return %p\n",head);
    return head;
}

matrix * make_matrix(__uint64_t row, __uint64_t col, __uint64_t ** data){
    matrix * new = malloc(sizeof(matrix));
    new->prev = new->next = NULL;
    new->row = row;
    new->col = col;
    new->data = data;
    return new;
}

void push_matrix(matrix * head, matrix * new, __uint64_t id){
    if (search_matrix(head, id) != NULL){
        fprintf(stderr, "redefinition of dest_id %lu\n", id);
        exit(1);
    }
    new->id = id;

    new->prev = head;
    new->next = head->next;

    if(new->prev != NULL) new->prev->next = new;
    if(new->next != NULL) new->next->prev = new;
}

matrix * get_matrix(matrix * head, __uint64_t id){
    matrix * e;
    if ((e = search_matrix(head, id)) == NULL){
        fprintf(stderr, "usage of undefined id %lu\n", id);
        exit(1);
    }

    return e;
}

bytecode * jump_bytecode(bytecode * pc, int64_t offset){
    bool isprev = false;
    if (offset < 0){
        offset = -offset;
        isprev = true;
    }
    for(int64_t i = 0; i < offset; i++){
        if(isprev) pc = pc->prev;
        else pc = pc->next;
        if(pc == NULL){
            fprintf(stderr, "invalid jump\n");
            exit(1);
        }
    }
    return pc;
}

void push_bytecode(bytecode * head, bytecode * elem){
    while(head->next != NULL) head = head->next;
    elem->prev = head;
    head->next = elem;
}

bytecode * parse(const char * code, off_t size){
    bytecode * head = NULL, * new;
    const char * cursor = code;
    unsigned char opcode;
    __uint64_t dest_id, row, col;

    while (cursor < code + size){
        opcode = *cursor;
        switch (opcode)
        {
            case 0:{ // new_matrix
                cursor ++;
                if(cursor + 3 * sizeof(__uint64_t) > code + size){
                    fprintf(stderr, "truncated bytecode\n");
                    exit(1);
                }
                dest_id = ((__uint64_t *)cursor)[0];
                row = ((__uint64_t *)cursor)[1];
                col = ((__uint64_t *)cursor)[2];
                //printf("new matrix : row %lu col %lu\n", row, col);
                cursor += 3 * sizeof(__uint64_t);
                if(cursor + row * col * sizeof(__uint64_t) > code + size){
                    fprintf(stderr, "truncated bytecode\n");
                    exit(1);
                }
                new = malloc(sizeof(bytecode) + 3 * sizeof(__uint64_t) + sizeof(__uint64_t) * row * col);
                if (new == NULL) perror("malloc error");
                memcpy(&new->data, cursor - 3 * sizeof(__uint64_t), 3 * sizeof(__uint64_t) + sizeof(__uint64_t) * row * col);
                cursor += sizeof(__uint64_t) * row * col;
            }   break;
            case 1: // mul_matrix
            case 2: // index_read
            case 3:{// index_write
                //printf("opcode %d\n", opcode);
                cursor ++;
                if(cursor + 3 * sizeof(__uint64_t) > code + size){
                    fprintf(stderr, "truncated bytecode\n");
                    exit(1);
                }
                new = malloc(sizeof(bytecode) + 3 * sizeof(__uint64_t));
                if (new == NULL) perror("malloc error");
                memcpy(&new->data, cursor, 3 * sizeof(__uint64_t));
                cursor += 3 * sizeof(__uint64_t);
            }   break;
            case 4:{ // index_move
                //printf("index_move\n");
                cursor ++;
                if(cursor + 6 * sizeof(__uint64_t) > code + size){
                    fprintf(stderr, "truncated bytecode\n");
                    exit(1);
                }
                new = malloc(sizeof(bytecode) + 6 * sizeof(__uint64_t));
                if (new == NULL) perror("malloc error");
                memcpy(&new->data, cursor, 6 * sizeof(__uint64_t));
                cursor += 6 * sizeof(__uint64_t);
            }   break;
            case 5:{ // branch
                //printf("branch\n");
                cursor ++;
                if(cursor + 2 * sizeof(__uint64_t) > code + size){
                    fprintf(stderr, "truncated bytecode\n");
                    exit(1);
                }
                new = malloc(sizeof(bytecode) + 2 * sizeof(__uint64_t));
                if (new == NULL) perror("malloc error");
                memcpy(&new->data, cursor, 2 * sizeof(__uint64_t));
                cursor += 2 * sizeof(__uint64_t);
            }   break;
            default :{
                fprintf(stderr, "unknown opcode %u\n", *cursor);
                exit(1);
            }   break;
        }
        new->prev = new->next = NULL;
        new->opcode = opcode;
        if (head != NULL) push_bytecode(head, new);
        else head = new;
    }

    return head;
}

void run(bytecode * bc){
    matrix * matrix_table = NULL;
    matrix * target, * op1, * op2;
    bytecode * pc = bc, *npc;
    __uint64_t row, col, val, op1r, op2r, op1c, op2c;
    __uint64_t ** tmp_data;


    while(pc != NULL){
        //printf("-------------------------------------------------------------------\n");
        //printf("pc : %p (opcode %u)\n",pc, pc->opcode);
        switch (pc->opcode)
        {
            case 0:{ // new_matrix
                val = pc->data[0];
                row = pc->data[1];
                col = pc->data[2];
                //printf("id : %lu, row : %lu, col : %lu\n", val, row, col);
                if(row > 100 || col > 100 || row == 0 || col == 0){
                    fprintf(stderr, "matrix is too large or too small ( %lu x %lu )\n", row, col);
                    exit(1);
                }
                tmp_data = malloc(sizeof(__uint64_t *) * row);
                for(__uint64_t i = 0; i < row; i++){
                    tmp_data[i] = malloc(sizeof(__uint64_t) * col);
                    for(__uint64_t j = 0; j < col; j++){
                        tmp_data[i][j] = pc->data[i * col + j + 3];
                        //printf("%lu ", tmp_data[i][j]);
                    }
                    //printf("\n");
                }

                target = make_matrix(row, col, tmp_data);
                if(matrix_table == NULL) {
                    target->id = val;
                    matrix_table = target;
                }
                else push_matrix(matrix_table, target, val);

                npc = pc->next;
            }   break;
            case 1:{ // mul_matrix
                //printf("out : %lu, op1: %lu, op2 : %lu\n", pc->data[0],pc->data[1],pc->data[2]);
                op1 = get_matrix(matrix_table, pc->data[1]);
                op2 = get_matrix(matrix_table, pc->data[2]);
                
                op1r = op1->row;
                op2r = op2->row;
                op1c = op1->col;
                op2c = op2->col;

                if(op1c != op2r){
                    fprintf(stderr, "unmatched dimension on mul_matrix (%lu != %lu)\n", op1c , op2r);
                    exit(1);
                }
                tmp_data = malloc(sizeof(__uint64_t *) * op1r);
                for(__uint64_t i = 0 ; i < op1r; i++){
                    tmp_data[i] = malloc(sizeof(__uint64_t) * op2c);
                    for(__uint64_t j = 0 ; j < op2c; j++){
                        val = 0;
                        for(__uint64_t k = 0; k < op1c; k++){
                            val += op1->data[i][k] * op2->data[k][j];
                        }
                        tmp_data[i][j] = val;
                    }
                }

                target = make_matrix(op1r, op2c, tmp_data);
                if(matrix_table == NULL) {
                    target->id = pc->data[0];
                    matrix_table = target;
                }
                else push_matrix(matrix_table, target, pc->data[0]);

                npc = pc->next;
            } break;
            case 2:{// index_read
                //printf("id : %lu, row: %lu, col : %lu\n", pc->data[0],pc->data[1],pc->data[2]);
                target = get_matrix(matrix_table, pc->data[0]);
                row = pc->data[1];
                col = pc->data[2];
                if(row < 0 || col < 0 || row >= target->row || col >= target->col){
                    fprintf(stderr, "out of bounds in index read\n");
                    exit(1);
                }
                target->data[row][col] = getchar();
                //printf("index read : data readed %lu\n",target->data[row][col]);
                npc = pc->next;
            } break;
            case 3:{// index_write
                //printf("id : %lu, row: %lu, col : %lu\n", pc->data[0],pc->data[1],pc->data[2]);
                target = get_matrix(matrix_table, pc->data[0]);
                row = pc->data[1];
                col = pc->data[2];
                if(row < 0 || col < 0 || row >= target->row || col >= target->col){
                    fprintf(stderr, "out of bounds in index write\n");
                    exit(1);
                }
                //printf("index_write : value %lu\n",target->data[row][col]);
                putchar((unsigned char)target->data[row][col]);
                npc = pc->next;
            }   break;
            case 4:{ // index_move
                //printf("dest id : %lu, dest row: %lu, dest col : %lu\n", pc->data[0],pc->data[1],pc->data[2]);
                //printf("src id : %lu, src row: %lu, src col : %lu\n", pc->data[3],pc->data[4],pc->data[5]);
                target = get_matrix(matrix_table, pc->data[3]);
                row = pc->data[4];
                col = pc->data[5];
                if(row < 0 || col < 0 || row >= target->row || col >= target->col){
                    fprintf(stderr, "out of bounds in index move\n");
                    exit(1);
                }
                val = target->data[row][col];
                

                target = get_matrix(matrix_table, pc->data[0]);
                row = pc->data[1];
                col = pc->data[2];
                if(row < 0 || col < 0 || row >= target->row || col >= target->col){
                    fprintf(stderr, "out of bounds in index move\n");
                    exit(1);
                }
                target->data[row][col] = val;
                //printf("index move : data moved %lu\n",val);
                npc = pc->next;
            }   break;
            case 5:{ // branch
                //printf("id : %lu, offset : %ld\n", pc->data[0], (int64_t)pc->data[1]);
                target = get_matrix(matrix_table, pc->data[0]);
                if(target->data[0][0] != 0) npc = jump_bytecode(pc, pc->data[1]);
                else npc = pc->next;
            }   break;
            default :{
                fprintf(stderr, "unknown opcode %u\n", pc->opcode);
                exit(1);
            }   break;
        }

        pc = npc;  
    }
}

int main(int argc, const char *argv[]){
    int fd;
    const char * content = NULL;
    off_t file_size;
    bytecode * bc = NULL;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("file open error");
        exit(1);
    }

    file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    if ((content = (const char *) mmap(0, PAGE_ROUND_UP(file_size), PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    {
        perror("mmap error");
        exit(1);
    }

    bc = parse(content, file_size);
    //printf("parse done (bc : %p)\n", bc);
    run(bc);
    return 0;
}