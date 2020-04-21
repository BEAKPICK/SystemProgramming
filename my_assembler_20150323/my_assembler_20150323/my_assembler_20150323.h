/*
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_TYPE 4

/*
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit
{
	char instName[10];
	int type[MAX_TYPE];
	char opCode[3];
	int operandNum[MAX_OPERAND];
};

// instruction�� ������ ���� ����ü�� �����ϴ� ���̺� ����
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

//���α׷� ũ�⸦ ������ ����
int* program_size[MAX_LINES];
static int program_size_indexer;

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
* ���� �����ڵ带 ������ ���̺��̴�.
* ���� ������ ������ �� �ִ�.
*/
struct machine_code_struct
{
	int *length;
	int *machine_code;
};

typedef struct machine_code_struct machine_code;
machine_code *machine_code_table[MAX_LINES];

/*
* Control Section���� �ܺ����� �ɺ��� ���, Object Code�� �̰����� �κ��ڵ��� ��ġ�� ���� ������ �����ϱ� ���� ������ ����ü
* �߰��� NULL���� �ξ� �� Section�� �����ϵ��� �Ѵ�.
*/
struct ext_ref_unit
{
	int *addr;
	int *count;
	char *label;
};

typedef struct ext_ref_unit ref;
ref *ext_ref_table[MAX_LINES];
static int ref_table_line;

/*
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit
{
    char *label;                //��ɾ� ���� �� label
    char *operator;             //��ɾ� ���� �� operator
    char *operand[MAX_OPERAND]; //��ɾ� ���� �� operand
    char *comment;              //��ɾ� ���� �� comment
    unsigned char nixbpe; // ���� ������Ʈ���� ���ȴ�.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 * ���� ������Ʈ���� ���ȴ�.
 */
struct symbol_unit
{
    char symbol[10];
    int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

/*
* ���ͷ��� �����ϴ� ����ü�̴�.
* ���ͷ� ���̺��� ���ͷ��� �̸�, ���ͷ��� ��ġ�� �����ȴ�.
* ���� ������Ʈ���� ���ȴ�.
*/
struct literal_unit
{
    char literal[10];
    int addr;
	int check;
};

typedef struct literal_unit literal;
literal literal_table[MAX_LINES];

static int locCtr;
//--------------

static char *input_file;
static char *output_file;

/*custom functions*/
int parse_inspection(void);
void make_symboltab_literaltab();
int search_sym_table(char* symbol, int sectionIndex);
int search_literal_table(char* literal, int sectionIndex);
int set_addr_size(int tokenTableIndex);
int register_number(char* r);
void print_machine_code(int index, FILE* file);
int search_operator_in_token_table(int index, char* str);

/*template functions*/
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
