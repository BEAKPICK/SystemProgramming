/*
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다.
 *
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3
#define MAX_TYPE 4

/*
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit
{
	char instName[10];
	int type[MAX_TYPE];
	char opCode[3];
	int operandNum[MAX_OPERAND];
};

// instruction의 정보를 가진 구조체를 관리하는 테이블 생성
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;

//프로그램 크기를 저장할 변수
int* program_size[MAX_LINES];
static int program_size_indexer;

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;

/*
* 최종 기계어코드를 저장할 테이블이다.
* 라인 단위로 관리할 수 있다.
*/
struct machine_code_struct
{
	int *length;
	int *machine_code;
};

typedef struct machine_code_struct machine_code;
machine_code *machine_code_table[MAX_LINES];

/*
* Control Section에서 외부참조 심볼의 경우, Object Code에 미결정된 부분코드의 위치에 대한 정보를 기입하기 위해 저장할 구조체
* 중간에 NULL값을 두어 각 Section을 구분하도록 한다.
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
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit
{
    char *label;                //명령어 라인 중 label
    char *operator;             //명령어 라인 중 operator
    char *operand[MAX_OPERAND]; //명령어 라인 중 operand
    char *comment;              //명령어 라인 중 comment
    unsigned char nixbpe; // 추후 프로젝트에서 사용된다.
};

typedef struct token_unit token;
token *token_table[MAX_LINES];
static int token_line;

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 * 추후 프로젝트에서 사용된다.
 */
struct symbol_unit
{
    char symbol[10];
    int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];

/*
* 리터럴을 관리하는 구조체이다.
* 리터럴 테이블은 리터럴의 이름, 리터럴의 위치로 구성된다.
* 추후 프로젝트에서 사용된다.
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

/* 추후 프로젝트에서 사용하게 되는 함수*/
void make_symtab_output(char *file_name);
void make_literaltab_output(char *file_name);
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
