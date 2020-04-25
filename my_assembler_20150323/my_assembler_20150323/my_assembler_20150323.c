/*
 * 화일명 : my_assembler_00000000.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20150323.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}
	/*opcode 출력은 삭제*/
	//make_opcode_output("output_20150323");

	/*새로이 구현되는 내용*/

	//symtab_20150323 literaltab_20150323

	make_symboltab_literaltab();
	make_symtab_output("symtab_20150323");
	make_literaltab_output("literaltab_20150323");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}
	//"output_20150323"
	make_objectcode_output("output_20150323") ;
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE *file;
	int errno;
	
	file = fopen(inst_file, "r");
	char *type = NULL;
	char *tempOperandNum = NULL;
	char *ptr;
	if (file != NULL)
	{
		int tableIndexer = 0;
		int typeIndexer = 0;
		int operandNumIndexer = 0;
		while (!feof(file))
		{
			inst_table[tableIndexer] = malloc(sizeof(inst_table));
			if (type == NULL) type = malloc(sizeof(inst_table[tableIndexer]->type));
			if (tempOperandNum == NULL) tempOperandNum = malloc(sizeof(inst_table[tableIndexer]->operandNum));
			fscanf(file,
				"%s\t%s\t%s\t%s\n",
				inst_table[tableIndexer]->instName, type, inst_table[tableIndexer]->opCode, tempOperandNum);
			
			/*
			*	dealing with format type of inst
			*/
			for (int t = 0 ; t < sizeof(inst_table[tableIndexer]->type)/sizeof(int); t++)
			{
				inst_table[tableIndexer]->type[t] = 0;
			}

			ptr = strtok(type, ",");
			inst_table[tableIndexer]->type[typeIndexer] = (int)(*ptr) - 48;
			while ((ptr = strtok(NULL, ",")) != NULL)
			{
				typeIndexer += 1;
				inst_table[tableIndexer]->type[typeIndexer] = (int)(*ptr) - 48;
			}

			/*
			*	dealing with operandNum of inst
			*/
			for (int t = 0; t < sizeof(inst_table[tableIndexer]->operandNum) / sizeof(int); t++)
			{
				inst_table[tableIndexer]->operandNum[t] = -1;
			}
			ptr = strtok(tempOperandNum, ",");
			inst_table[tableIndexer]->operandNum[operandNumIndexer] = (int)(*ptr) - 48;
			while ((ptr = strtok(NULL, ",")) != NULL)
			{
				operandNumIndexer += 1;
				inst_table[tableIndexer]->operandNum[operandNumIndexer] = (int)(*ptr) - 48;
			}
			
			tableIndexer += 1;
			operandNumIndexer = 0;
			typeIndexer = 0;
		}
		fclose(file);
		free(type);
		free(tempOperandNum);
		errno = 0;
	}
	else
	{
		errno = -1;
	}
	
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE *file;
	file = fopen(input_file, "r");
	if (file != NULL)
	{
		char *s = malloc(sizeof(char)*100);
		char *ptr;
		int tableIndexer = 0;
		
		while (fgets(s, 100, file))
		{
			input_data[tableIndexer] = (char*)malloc(sizeof(char)*100);
			ptr = strtok(s, "\n");
			strcpy(input_data[tableIndexer], ptr);
			tableIndexer += 1;
		}
		fclose(file);
		free(s);
		errno = 0;
	}
	else
	{
		errno = -1;
	}
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다. 
 * 매계 : 파싱을 원하는 문자열  
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	if (str != NULL)
	{
		char *tempOperand = malloc(sizeof(char) * 40);
		for (int i = 0; i < 40; i++) tempOperand[i] = 0;
		char *strRemain = malloc(sizeof(char) * 100);	//strtok를 쓰면 원본 char* 데이터도 손상되기 때문에 복사를 하기 위한 동적할당.
		strcpy(strRemain, str);
		char *ptr = strtok(strRemain, "\t");	//strtok가 맨앞의 \t를 무시한다...

		token_table[token_line] = malloc(sizeof(token_table));
		token_table[token_line]->label = NULL;
		token_table[token_line]->operator = NULL;
		for (int i = 0; i < MAX_OPERAND; i++) token_table[token_line]->operand[i] = NULL;
		token_table[token_line]->comment = NULL;

		/*handle full comment line*/
		if (str[0] == '.')
		{
			token_table[token_line]->comment = (char*)malloc(sizeof(char) * 60);
			strcpy(token_table[token_line]->comment, ptr);
			errno = 1;
			return errno;
		}

		if (str[0] != '\t')
		{
			/*앞에 레이블이 있는 경우 ptr이 label이므로 할당후 operator할당*/
			token_table[token_line]->label = (char*)malloc(sizeof(char) * 40);
			strcpy(token_table[token_line]->label, ptr);
			ptr = strtok(NULL, "\t");
			if (ptr != NULL)
			{
				token_table[token_line]->operator = (char*)malloc(sizeof(char) * 40);
				strcpy(token_table[token_line]->operator, ptr);
			}
		}
		else
		{
			/*앞에 레이블이 없는 경우 ptr이 operator이므로 바로 넣어준다*/
			token_table[token_line]->operator = (char*)malloc(sizeof(char) * 40);
			if (ptr != NULL)strcpy(token_table[token_line]->operator, ptr);
		}

		ptr = strtok(NULL, "\t");
		if(ptr!=NULL && ptr[0] != ' ')strcpy(tempOperand, ptr);

		/*comment할당*/
		ptr = strtok(NULL, "\t");
		if (ptr != NULL)
		{
			token_table[token_line]->comment = (char*)malloc(sizeof(char) * 40);
			strcpy(token_table[token_line]->comment, ptr);
		}


		/*parsing operand*/
		for (int i = 0; i < MAX_OPERAND; i++)
		{
			token_table[token_line]->operand[i] = NULL;
		}

		ptr = strtok(tempOperand, ",");
		int operandIndexer = 0;
		while (ptr != NULL)
		{
			token_table[token_line]->operand[operandIndexer] = (char*)malloc(sizeof(char) * 40);
			strcpy(token_table[token_line]->operand[operandIndexer], ptr);
			ptr = strtok(NULL, ",");
			operandIndexer += 1;
		}
		errno = 0;
		free(tempOperand);
		free(strRemain);
	}
	else 
	{
		errno = -3;
	}
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
	int searchErr = 0;
	char* tmpStr = malloc(sizeof(char) * 10);

	if (str == NULL) return -1;

	if (str[0] == '+' || str[0] == '@')
	{
		strcpy(tmpStr, str + 1);
	}
	else strcpy(tmpStr, str);

	for (int i = 0; i < MAX_INST; i++)
	{
		if (inst_table[i] == NULL) break;
		if (strcmp(inst_table[i]->instName, tmpStr)==0)
		{
			return i;
		}
	}
	free(tmpStr);
	searchErr = -5;
	return searchErr;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	errno = 0;
	token_line = 0;
	char *str = malloc(sizeof(char) * 60);
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (input_data[i] == NULL) break;
		if (token_parsing(input_data[i]) < 0)
		{
			errno = -2;
			free(str);
			return errno;
		}
		
		token_line += 1;
	}
	errno = parse_inspection();
	/* input_data의 문자열을 한줄씩 입력 받아서 
	 * token_parsing()을 호출하여 token_unit에 저장
	 * 저장 후 파싱 검사
	 */
	free(str);
	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : assem_pass1의 주의사항에 의거한 에러 검사 함수. 오퍼랜드의 개수대로 맞게
*		파싱이 되었는지 확인한다.
* 매개 : 없음
* 반환 : 정상종료 = 0, 에러 < 0
* 주의 : 기계어로 변환가능한 대상만 parsing 검사를 진행한다.
* -----------------------------------------------------------------------------------
*/
int parse_inspection(void)
{
	errno = 0;
	int tempIndex;
	int operandCount = 0;
	char missCount = 0;
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (token_table[i] == NULL) break;
		if (token_table[i]->operator == NULL) continue;
		tempIndex = search_opcode(token_table[i]->operator);
		if (tempIndex >= 0)
		{
			operandCount = 0;
			missCount = 0;
			for (int t = 0; t < MAX_OPERAND; t++)
			{
				/*strtok의 건너뛰는 문제를 해결하기 위해 값이 없는 중간의 공간에 스페이스바를 넣었는데 이 때문에 스페이스바도 오퍼랜드가 없다*/
				if (token_table[i]->operand[t] == NULL || strcmp(token_table[i]->operand[t], " ") == 0) break;
				else operandCount++;
			}
			for (int t = 0; t < MAX_OPERAND; t++)
			{
				if (inst_table[tempIndex]->operandNum[t] == operandCount) 
				{
					missCount = 0;
					errno = 0;
					break;
				}
				else missCount += 1;
			}
			if (missCount > 0)
			{
				errno = -6;
				return errno;
			}
		}
	}
	return errno;
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매개 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	FILE * file;
	file = fopen(file_name, "w");
	int opIndex = 0;
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (token_table[i] == NULL) break;
		if (token_table[i]->comment != NULL && token_table[i]->comment[0] == '.') continue;
		if (token_table[i]->label != NULL)
		{
			fprintf(file, token_table[i]->label);
		}
		fprintf(file, "\t");
		fprintf(file, token_table[i]->operator);
		fprintf(file, "\t");
		if (token_table[i]->operand != NULL)
		{
			for (int t = 0; t < MAX_OPERAND; t++)
			{
				/*input에 operand가 없는 경우 " "를 operand로 인식하게끔 input을 조정*/
				if (token_table[i]->operand[t] == NULL || token_table[i]->operand[t] == " ")
				{
					break;
				}
				else
				{
					if (t != 0) fprintf(file, ",");
					fprintf(file, token_table[i]->operand[t]);
				}
			}
		}
		fprintf(file, "\t");
		opIndex = search_opcode(token_table[i]->operator);
		if (opIndex >= 0) fprintf(file, inst_table[opIndex]->opCode);
		fprintf(file, "\n");
	}
	fclose(file);
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매개 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	if (file_name == "")
	{
		char* ptr = malloc(sizeof(char)*10);
		for (int i = 0; i < MAX_LINES; i++)
		{
			if (strcmp(sym_table[i].symbol, "")==0) continue;
			sprintf(ptr, "%X",sym_table[i].addr);
			printf("%s\t%s\n", sym_table[i].symbol, ptr);
		}
		printf("\n");
		free(ptr);
		return;
	}
	FILE * file;
	file = fopen(file_name, "w");
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (strcmp(sym_table[i].symbol, "") == 0) continue;
		fprintf(file, sym_table[i].symbol);
		fprintf(file, "\t");
		fprintf(file, "%X", sym_table[i].addr);
		fprintf(file, "\n");
	}
	fclose(file);
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매개 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *file_name)
{
	if (file_name == "")
	{
		char* ptr = malloc(sizeof(char)*10);
		for (int i = 0; i < MAX_LINES; i++)
		{
			if (strcmp(literal_table[i].literal, "") == 0) continue;
			sprintf(ptr, "%X",literal_table[i].addr);
			printf("%s\t%s\n", literal_table[i].literal, ptr);
		}
		printf("\n");
		free(ptr);
		return;
	}
	FILE * file;
	file = fopen(file_name, "w");
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (strcmp(literal_table[i].literal, "") == 0) continue;
		fprintf(file, literal_table[i].literal);
		fprintf(file, "\t");
		fprintf(file, "%X", literal_table[i].addr);
		fprintf(file, "\n");
	}
	fclose(file);
}

/* -----------------------------------------------------------------------------------
* 설명 : 심볼끼리의 연산이 들어갔을 때 연산기호를 인식해서 계산을 처리하는 함수.
* 매개 : 파싱할 오퍼랜드
* 반환 : 없음
* 주의 : 안 쓸 예정이다.
* ------------------------------------------------------------------------------------
*/
int handle_symbol_cal(char* operand)
{
	char *tempLiteral = malloc(sizeof(char) * 20);
	char *tempCalOperand = malloc(sizeof(char) * 20);
	char *ptr;
	int indexPoint = 0, result = 0, tempSearchIndex = 0;
	strcpy(tempLiteral, operand);

	for (int i = 0; i < strlen(operand); i++)
	{
		if (tempLiteral[i] == '-')
		{
			ptr = strtok(tempLiteral, "-");
			indexPoint = i;

			break;
		}
		else if (tempLiteral[i] == '+')
		{
			ptr = strtok(tempLiteral, "+");
			indexPoint = i;
			break;
		}
	}
	for (int i = indexPoint; i < strlen(operand); i++)
	{
		if (tempLiteral[i] == '-')
		{
			ptr = strtok(NULL, "-");
		}
		else if (tempLiteral[i] == '+')
		{
			ptr = strtok(NULL, "+");
		}
	}
	free(tempLiteral);
	free(tempCalOperand);
	return result;
}

/* -----------------------------------------------------------------------------------
* 설명 : 심볼테이블과 리터럴테이블을 한번에 만들기 위한 함수이다. 프로그램 크기도 여기서 구한다.
* 매개 : 없음
* 반환 : 없음
* 주의 : 없음
* ------------------------------------------------------------------------------------
*/
void make_symboltab_literaltab()
{
	locCtr = 0;
	program_size_indexer = 0;
	int tempLocCtr = locCtr;
	int symIndexer = 0, literalIndexer = 0;
	int locSize = 0, literalLocSize = 0;
	int tempLiteralIndex = 0, opIndex = 0, sectionIndex = 0;
	char* tempLiteral = malloc(sizeof(char) * 20);
	int equResult = 0, symSearchIndex = 0;
	char* ptr;

	for (int i = 0; i < MAX_LINES; i++)
	{
		locSize = 0;
		if (token_table[i] == NULL)
		{
			break;
		}
		if (token_table[i]->comment != NULL && token_table[i]->comment[0] == '.') continue;

		/*중간에 CSECT을 만나면 tempLocCtr 초기화*/
		if (strcmp(token_table[i]->operator, "CSECT") == 0) 
		{
			program_size[program_size_indexer] = malloc(sizeof(int));
			*program_size[program_size_indexer] = tempLocCtr;
			program_size_indexer += 1;
			sectionIndex += 1;
			tempLocCtr = 0;
			symIndexer += 1;
			literalIndexer += 1;
			tempLiteralIndex += 1;
		}

		/*locSize를 구하는 과정 하지만 중간에 =.''형태가 있다면 리터럴테이블에 넣어주기*/
		if (token_table[i]->operator[0] == '+') locSize = 4;
		else
		{
			/*오퍼랜드에 =가 있을 때 즉, 리터럴을 만났을 때 리터럴 테이블에 리터럴 저장해놓기*/
			if (token_table[i]->operand[0] != NULL && token_table[i]->operand[0][0] == '=')
			{
				strcpy(tempLiteral, token_table[i]->operand[0]);
				strtok(tempLiteral, "\'");
				ptr = strtok(NULL, "\'");
				if (search_literal_table(ptr,sectionIndex) < 0)
				{
					strcpy(literal_table[literalIndexer].literal, ptr);
					literalIndexer += 1;
				}
			}

			/*기계어일 때 1,2,3/4로 정해진 패턴으로 locSize 설정. inst_table의 type은 무조건 오름차순으로 입력되었다고 가정하자.*/
			opIndex = search_opcode(token_table[i]->operator);
			if (opIndex >= 0)
			{
				for (int i = 0; i < MAX_TYPE; i++)
				{
					if (inst_table[opIndex]->type[i] == 1) locSize = 1;
					else if (inst_table[opIndex]->type[i] == 2) locSize = 2;
					else if (inst_table[opIndex]->type[i] == 3 || inst_table[opIndex]->type[i] == 4) locSize = 3;
				}
			}
			/*기계어로 번역되는 operator가 아닐 때 locSize설정*/
			else
			{
				locSize = set_addr_size(i);
			}
		}

		/*심볼테이블 핸들링*/
		if (token_table[i]->label != NULL)
		{
			strcpy(sym_table[symIndexer].symbol, token_table[i]->label);
			if (strcmp(token_table[i]->operator, "EQU") == 0 && strcmp(token_table[i]->operand[0], "*") != 0)
			{
				/*현재 -만 계산되도록 해놨다...다른 연산기호들이 포함되려면 아마 함수로 따로 빼야될 것 같다.*/
				strcpy(tempLiteral, token_table[i]->operand[0]);
				ptr = strtok(tempLiteral, "-");
				symSearchIndex = search_sym_table(ptr, sectionIndex);
				if(symSearchIndex>=0) equResult = sym_table[symSearchIndex].addr;
				else { printf("line %d : no operand in symbol table\n", i); continue; }
				ptr = strtok(NULL, "-");
				while (ptr != NULL)
				{
					symSearchIndex = search_sym_table(ptr, sectionIndex);
					if (symSearchIndex >= 0) equResult -= sym_table[symSearchIndex].addr;
					ptr = strtok(NULL, "-");
				}
				sym_table[symIndexer].addr = equResult;
			}
			else
			{
				sym_table[symIndexer].addr = tempLocCtr;
			}
			symIndexer += 1;
		}
		/*LTORG가 나타날 경우 LTORG 전까지 나타났던 리터럴들의 주소를 할당, 리터럴테이블 핸들링*/
		else if (strcmp(token_table[i]->operator, "LTORG") == 0)
		{
			for (tempLiteralIndex; tempLiteralIndex < literalIndexer; tempLiteralIndex++)
			{
				literalLocSize = 0;
				literal_table[tempLiteralIndex].addr = tempLocCtr;
				for (int i = 0; i < sizeof(literal_table[tempLiteralIndex].literal); i++)
				{
					if (literal_table[tempLiteralIndex].literal[i] == 0) break;
					else literalLocSize++;
				}

				tempLocCtr += literalLocSize;
				locCtr += literalLocSize;
			}
			continue;
		}
		tempLocCtr += locSize;
		locCtr += locSize;
	}

	/*다 끝났다면 아직 tempLocCtr가 할당되지 않은 리터럴에 주소를 할당하는 작업*/
	for (int i = tempLiteralIndex; i < literalIndexer; i++)
	{
		locSize = 0;
		/*LTORG는 맨 처음에 나올 확률이 없기 때문에 리터럴의 주소는 0이 될 수 없다.*/
		if (literal_table[i].literal != "" && literal_table[i].addr == 0)
		{
			literal_table[i].addr = tempLocCtr;
			/*리터럴이 문자일 때*/
			if (atoi(literal_table[i].literal) == 0)
			{
				locSize = (int)strlen(literal_table[i].literal);
			}
			/*리터럴이 문자가 아니라 숫자일 때*/
			else
			{
				locSize = (int)strlen(literal_table[i].literal) / 2;
			}
		}
		tempLocCtr += locSize;
		locCtr += locSize;
	}
	program_size[program_size_indexer] = malloc(sizeof(int));
	*program_size[program_size_indexer] = tempLocCtr;
	program_size_indexer += 1;
	free(tempLiteral);
}

/* ----------------------------------------------------------------------------------
* 설명 : 검출한 리터럴이 중복되어 테이블에 들어가는 것을 방지하기 위해 해당 리터럴이
	테이블에 있는 지에 대한 여부를 확인하는 함수, 인덱스를 반환하는 방식으로 구현.
* 매개 : 리터럴
* 반환 : 인덱스 >= 0, 결과없음 < 0
* 주의 : 없음
* -----------------------------------------------------------------------------------
*/
int search_literal_table(char* literal, int sectionIndex)
{
	int tempSectionIndex = 0;
	if (literal == NULL) return -1;
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (strcmp(literal_table[i].literal,"")==0)
		{
			tempSectionIndex += 1;
			if (tempSectionIndex > sectionIndex) return -1;
			continue;
		}
		if (strcmp(literal_table[i].literal, literal) == 0 && tempSectionIndex==sectionIndex) return i;
	}
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 기계어로 번역되지 않는 코드에 대해서 카운트해야 되는 주소의 크기를 리턴하는 함수.
* 매개 : 토큰 테이블 인덱스
* 반환 : 주소의 사이즈
* 주의 : 없음
* -----------------------------------------------------------------------------------
*/
int set_addr_size(int tokenTableIndex)
{
	if (token_table[tokenTableIndex] == NULL) return 0;
	int size = 0;
	if (token_table[tokenTableIndex]->comment != NULL && token_table[tokenTableIndex]->comment[0] == '.') return size;
	if (strcmp(token_table[tokenTableIndex]->operator, "RESB") == 0) size = atoi(*token_table[tokenTableIndex]->operand);
	else if (strcmp(token_table[tokenTableIndex]->operator, "RESW") == 0) size = 3;
	else if (strcmp(token_table[tokenTableIndex]->operator, "BYTE") == 0) size = 1;
	else if (strcmp(token_table[tokenTableIndex]->operator, "WORD") == 0) size = 3;
	return size;
}

/* ----------------------------------------------------------------------------------
* 설명 : 심볼테이블을 검색해서 검색된 데이터의 인덱스를 반환하고 
	결과가 없다면 0보다 작은 값을 리턴
* 매개 : 심볼, 섹션인덱스
* 반환 : 인덱스 >= 0 / 결과없음(-1), 검색은 되지만 다른 컨트롤 섹션에 존재(-2) < 0
* 주의 : 해당 참조가 다른 섹션에 있는 경우와 아예 없는 경우를 나눠서 리턴한다.
	해당 섹션까지 모두 찾아서 인덱스를 리턴해야된다. 섹션 구분을 공백으로 하기 때문이다.
* -----------------------------------------------------------------------------------
*/
int search_sym_table(char* symbol, int sectionIndex)
{
	if (symbol == NULL) return -1;

	int tempSectionIndex = 0;
	int result = -1;
	char* tmpStr = malloc(sizeof(char) * 25);
	if (symbol[0] == '@') strcpy(tmpStr, symbol + 1);
	else strcpy(tmpStr, symbol);
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (strcmp(sym_table[i].symbol, "") == 0) 
		{
			tempSectionIndex+=1;
			if (tempSectionIndex > sectionIndex) return result;
			continue;
		}
		if (strcmp(sym_table[i].symbol, tmpStr) == 0)
		{
			if(tempSectionIndex == sectionIndex) result = i;
			else result = -2;
		}
	}
	free(tmpStr);
	return result;
}

/* ------------------------------------------------------------------------------------
* 설명 : nixbpe를 설정해주는 부분이다. Control Section 구현이므로 
*	무조건 PC카운터로 계산되어지는 것으로 가정하고
*	BASE카운터를 사용하는 경우는 고려사항에서 제외한다.
* 매개 : 없음
* 반환 : 없음
* 주의 : 원칙적으로 심볼테이블과 리터럴테이블 작업이 완료가 된 후 호출을 하는 것이 맞다.
* -------------------------------------------------------------------------------------
*/
void set_nixbpe()
{
	int searchResult = 0;
	for (int i = 0; i < MAX_LINES; i++)
	{
		if (token_table[i] == NULL) break;
		token_table[i]->nixbpe = 0;
		searchResult = search_opcode(token_table[i]->operator);

		/*nixbpe가 사용되는 경우*/
		if (searchResult >= 0 && inst_table[searchResult]->type[0] >= 3)
		{
			if (token_table[i]->operator[0] == '+')
			{
				token_table[i]->nixbpe += 32;
				token_table[i]->nixbpe += 16;
				token_table[i]->nixbpe += 1;
			}
			else if (token_table[i]->operand[0] != NULL && *token_table[i]->operand[0] == '@')
			{
				token_table[i]->nixbpe += 32;
				token_table[i]->nixbpe += 2;
			}
			else if (token_table[i]->operand[0] != NULL && *token_table[i]->operand[0] == '#')
			{
				token_table[i]->nixbpe += 16;
			}
			else if (token_table[i]->operand[0] == NULL || *token_table[i]->operand[0] == ' ')
			{
				token_table[i]->nixbpe += 32;
				token_table[i]->nixbpe += 16;
			}
			else
			{
				token_table[i]->nixbpe += 32;
				token_table[i]->nixbpe += 16;
				token_table[i]->nixbpe += 2;
			}

			for (int t = 0; t < MAX_OPERAND; t++)
			{
				if (token_table[i]->operand[t]!= NULL && strcmp(token_table[i]->operand[t], "X") == 0)
				{
					token_table[i]->nixbpe += 8;
					break;
				}
			}
		}
	}
}

/* ----------------------------------------------------------------------------------
* 설명 : 파라미터로 받은 값들을 가지고 외부참조 테이블에 넣어주는 역할을 하는 함수.
* 매개 : 토큰 테이블 인덱스, 값을 삽입할 외부참조 테이블 인덱스, 현재 섹션 인덱스 , 현재 주소, 바이트 사이즈
* 반환 : 없음
* 주의 : operand의 형식은 항상 맨앞이 +, 그 뒤로는 모두 -이다. operand에 여러 항이 존재할 때 사용되는 용도이다.
* -----------------------------------------------------------------------------------
*/
void save_ext_ref_table(int tokenIndex, int sectionIndex ,int addr, int tempLocSize)
{
	char* tempStr = malloc(sizeof(char) * 15);
	int tempSearchIndex = 0;
	strcpy(tempStr, token_table[tokenIndex]->operand[0]);
	char* ptr = strtok(tempStr, "-");

	/*연산이 들어가는 operand를 가진다면 우선 operand가 모두 내부참조인지 확인*/
	/*이 역시 연산은 -만 허용한다.*/
	if (strcmp(ptr, token_table[tokenIndex]->operand[0]) != 0)
	{
		char tempPtr[20];
		tempPtr[0] = '+';
		strcpy(tempPtr + 1, ptr);

		int tempIndex = 0;
		while (ptr != NULL)
		{
			tempIndex = search_sym_table(ptr, sectionIndex);
			if (tempIndex < 0)
			{
				ext_ref_table[ref_table_line] = malloc(sizeof(ext_ref_table));
				ext_ref_table[ref_table_line]->count = malloc(sizeof(int));
				ext_ref_table[ref_table_line]->addr = malloc(sizeof(int));
				ext_ref_table[ref_table_line]->label = malloc(sizeof(char) * 10);
				//sym_table[tempSearchIndex].addr
				*ext_ref_table[ref_table_line]->addr = addr;
				/*외부참조이므로 값이 없기 때문에 비트 수가 늘어난다.*/
				if (tempLocSize == 3) *ext_ref_table[ref_table_line]->count = 6;
				else if (tempLocSize == 4) *ext_ref_table[ref_table_line]->count = 8;

				strcpy(ext_ref_table[ref_table_line]->label, tempPtr);
				ref_table_line += 1;
			}
			ptr = strtok(NULL, "-");
			tempPtr[0] = '-';
			if (ptr != NULL) strcpy(tempPtr + 1, ptr);
		}
	}
	free(tempStr);
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매개 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{
	set_nixbpe();
	int machineCodeIndexer = 0;
	ref_table_line = 0;
	int sectionIndex = 0;
	int tempLocCtr = 0, tempLocSize = 0;
	int tempSearchIndex = 0;
	int mask = 255;	//값을 계산한 뒤 마스킹을 해서 정확한 결과가 나오게 하기 위함
	int mask2 = 15;	//값을 계산한 뒤 마스킹을 해서 정확한 결과가 나오게 하기 위함
	int direction = 0;
	/*direction으로 과정을 통제하였다.*/
	int mark = 0;
	/*머신코드에서 중간에 따로 떨어져 나오는 부분이 생기는데 이 부분의 차이를 머신코드 테이블에 기록하기 위해서 mark변수사용*/
	/*지금은 코드가 들어가지 않는 기준을 잘 모르기 때문에 지금으로서는 RESW만 설정해놓는다.*/
	unsigned char* threeByteUnit = malloc(sizeof(char) * 3);
	unsigned char* fourByteUnit = malloc(sizeof(char) * 4);
	unsigned char* twoByteUnit = malloc(sizeof(char) * 2);
	unsigned char* oneByteUnit2 = malloc(sizeof(char));
	unsigned char* oneByteUnit = malloc(sizeof(char));
	char* tempStr = malloc(sizeof(char) * 15);

	for (int i = 0; i < MAX_LINES; i++)
	{
		if (token_table[i] == NULL)
		{
			direction = -1;
			break;
		}
		if (token_table[i]->comment != NULL && token_table[i]->comment[0] == '.') continue;
		if (strcmp(token_table[i]->operator, "END") == 0) continue;

		direction = 0;
		if (strcmp(token_table[i]->operator, "RESW") != 0) mark = tempLocCtr;

		if (strcmp(token_table[i]->operator, "CSECT") == 0)
		{
			tempLocCtr = 0;
			machineCodeIndexer += 1;
			ref_table_line += 1;
			sectionIndex += 1;
		}

		*oneByteUnit = token_table[i]->nixbpe;
		tempSearchIndex = search_opcode(token_table[i]->operator);

		/*지시어의 타입(포맷)이 3 혹은 4일 때*/
		if (tempSearchIndex >= 0 && inst_table[tempSearchIndex]->type[0] >= 3)
		{
			/*opCode와 nixbpe를 threeByteUnit에 넣는 과정, oneByteUnit2에 opCode를 넣고 있는 모습이다.*/
			*oneByteUnit2 = (unsigned char)strtol(inst_table[tempSearchIndex]->opCode, NULL, 16);

			/*+기호에 따라 사이즈를 정한다.*/
			if (token_table[i]->operator[0] == '+')
			{
				tempLocCtr += 4;
				tempLocSize = 4;

				fourByteUnit[0] = 0;
				fourByteUnit[1] = 0;
				fourByteUnit[2] = 0;
				fourByteUnit[3] = 0;

				/*이 과정은 opCode와 oneByteUnit의 nixbpe를 합치는 과정이다*/
				*oneByteUnit2 += (*oneByteUnit >> 4);
				fourByteUnit[0] = *oneByteUnit2;
				fourByteUnit[1] = (*oneByteUnit << 4);

			}
			else
			{
				tempLocCtr += 3;
				tempLocSize = 3;

				threeByteUnit[0] = 0;
				threeByteUnit[1] = 0;
				threeByteUnit[2] = 0;

				/*이 과정은 opCode와 oneByteUnit의 nixbpe를 합치는 과정이다*/
				*oneByteUnit2 += (*oneByteUnit >> 4);
				threeByteUnit[0] = *oneByteUnit2;
				threeByteUnit[1] = (*oneByteUnit << 4);
			}

			/*타입 3이상일 때 displacement를 threeByteUnit에 넣는 과정*/
			/*#이 들어가있는 애들을 처리하는 과정*/
			if (token_table[i]->operand[0] != NULL && token_table[i]->operand[0][0] == '#')
			{
				strcpy(tempStr, token_table[i]->operand[0] + 1);
				int t = atoi(tempStr);
				if (tempLocSize == 3)
				{
					threeByteUnit[1] += (t >> 8);
					threeByteUnit[2] += (t & mask);
				}
				else if(tempLocSize == 4)
				{
					fourByteUnit[1] += (t >> 8);
					fourByteUnit[2] += (t & mask);
				}
			}
			/*operand에 #이 없는 애들을 처리하는 과정*/
			else
			{
				/*타입 3이상은 1개 이상의 operand를 가진다 하지만 이때는 RSUB의 존재를 간과하고 있었다...*/
				tempSearchIndex = search_sym_table(token_table[i]->operand[0], sectionIndex);
				/*심볼테이블에 검색되는 operand를 가졌다면*/
				if (tempSearchIndex >= 0)
				{
					save_ext_ref_table(i, sectionIndex, tempLocCtr, tempLocSize);

					if (tempSearchIndex >= 0 && tempLocSize == 3)
					{
						threeByteUnit[1] += ((sym_table[tempSearchIndex].addr - tempLocCtr)>>8 & mask2);
						threeByteUnit[2] += ((sym_table[tempSearchIndex].addr - tempLocCtr) & mask);
					}
					else if (tempSearchIndex >= 0 && tempLocSize == 4)
					{
						fourByteUnit[1] += (sym_table[tempSearchIndex].addr - tempLocCtr)>>8 & mask2;
						fourByteUnit[2] += (sym_table[tempSearchIndex].addr - tempLocCtr) & mask;
					}
				}
				/*심볼테이블에 검색되지 않는다면 리터럴은 아닐 지 검사해본다. 리터럴 처리를 여기서 해준다.*/
				else if (token_table[i]->operand[0] != NULL && token_table[i]->operand[0][0] == '=')
				{
					strcpy(tempStr, token_table[i]->operand[0]);
					char* ptr = strtok(tempStr, "\'");
					ptr = strtok(NULL, "\'");
					tempSearchIndex = search_literal_table(ptr, sectionIndex);
					if (tempSearchIndex >= 0)
					{
						/*검색이 되면 이제 threeByteUnit 혹은 fourByteUnit에 displacement를 넣을 차례다*/
						if (tempLocSize == 3)
						{
							threeByteUnit[1] += ((literal_table[tempSearchIndex].addr - tempLocCtr) >> 8 & mask2);
							threeByteUnit[2] += ((literal_table[tempSearchIndex].addr - tempLocCtr) & mask);
						}
						else if(tempLocSize == 4)
						{
							fourByteUnit[1] += (literal_table[tempSearchIndex].addr - tempLocCtr) >> 8 & mask2;
							fourByteUnit[2] += (literal_table[tempSearchIndex].addr - tempLocCtr) & mask;
						}
					}
				}
				/*리터럴, 심볼테이블에 모두 없는 경우 외부참조라 생각하고 tempLocCtr를 저장해두자. 이것은 나중에 출력할 때 쓰인다.*/
				else
				{
					/*하지만 항상 RSUB같은 녀석도 있다는 걸 잊지말자... 지시어가 맞는데 operand가 필요없는 3,4형식인 RSUB...(OMG)*/
					if (token_table[i]->operand[0] == NULL || token_table[i]->operand[0] == " "
						|| tempSearchIndex == -1)
					{
						/*operand가 아예 없거나 인위적으로 없게 한 것이나 미리 스캔하여 만들어놓은 심볼테이블에 아예 없어도...RSUB은 다 빠져나간다.
						어쩔 수 없다...RSUB은 따로 검사해주자...RSUB이면 외부참조테이블에 들어가서는 안된다. 덕분에 코드가 매우 더러워졌다.*/
						if (strcmp(token_table[i]->operator, "RSUB") == 0) direction = 1;
						else direction = 0;
					}
					if (direction == 0)
					{
						ext_ref_table[ref_table_line] = malloc(sizeof(ext_ref_table));
						ext_ref_table[ref_table_line]->count = malloc(sizeof(int));
						ext_ref_table[ref_table_line]->addr = malloc(sizeof(int));
						ext_ref_table[ref_table_line]->label = malloc(sizeof(char) * 10);

						*ext_ref_table[ref_table_line]->addr = tempLocCtr - tempLocSize + 1;
						if (tempLocSize == 3) *ext_ref_table[ref_table_line]->count = 3;
						else if (tempLocSize == 4) *ext_ref_table[ref_table_line]->count = 5;

						tempStr[0] = '+';
						strcpy(tempStr + 1, token_table[i]->operand[0]);
						strcpy(ext_ref_table[ref_table_line]->label, tempStr);
						ref_table_line += 1;
					}
				}
			}
			/*결국엔 작업했던 3byte 혹은 4byte 머신코드를 모두 저장을 해야되는데 direction변수를 이용해서 중간과정들을 통제하여 여기까지 인도하도록 한다.*/
			if (tempLocSize == 3)
			{
				machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
				machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
				machine_code_table[machineCodeIndexer]->machine_code = malloc(sizeof(int));
				*machine_code_table[machineCodeIndexer]->machine_code = 0;
				*machine_code_table[machineCodeIndexer]->length = tempLocSize;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(threeByteUnit[0]) << 16;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(threeByteUnit[1]) << 8;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(threeByteUnit[2]);
			}
			else if (tempLocSize == 4)
			{
				machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
				machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
				machine_code_table[machineCodeIndexer]->machine_code = malloc(sizeof(int));
				*machine_code_table[machineCodeIndexer]->machine_code = 0;
				*machine_code_table[machineCodeIndexer]->length = tempLocSize;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(fourByteUnit[0]) << 24;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(fourByteUnit[1]) << 16;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(fourByteUnit[2]) << 8;
				*machine_code_table[machineCodeIndexer]->machine_code += (int)(fourByteUnit[3]);
			}
			machineCodeIndexer += 1;
		}
		/*2형식 지시어일 때*/
		else if (tempSearchIndex >= 0 && inst_table[tempSearchIndex]->type[0] == 2)
		{
			tempLocCtr += 2;
			tempLocSize = 2;

			twoByteUnit[0] = 0;
			twoByteUnit[1] = 0;

			*oneByteUnit2 = (unsigned char)strtol(inst_table[tempSearchIndex]->opCode, NULL, 16);

			twoByteUnit[0] += *oneByteUnit2;

			tempSearchIndex = register_number(token_table[i]->operand[0]);	//여기선 인덱스가 아님 주의
			tempSearchIndex = tempSearchIndex << 4;
			twoByteUnit[1] += tempSearchIndex;
			if (token_table[i]->operand[1] != NULL)
			{
				tempSearchIndex = register_number(token_table[i]->operand[1]);	//여기선 인덱스가 아님 주의
				twoByteUnit[1] += tempSearchIndex;
			}
			machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
			machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
			machine_code_table[machineCodeIndexer]->machine_code = malloc(sizeof(int));
			*machine_code_table[machineCodeIndexer]->machine_code = 0;
			*machine_code_table[machineCodeIndexer]->length = tempLocSize;
			*machine_code_table[machineCodeIndexer]->machine_code += (int)(twoByteUnit[0]) << 8;
			*machine_code_table[machineCodeIndexer]->machine_code += (int)(twoByteUnit[1]);
			machineCodeIndexer += 1;
		}
		/*1형식은 지원하지 않겠습니다*/
		/*지시어가 아닐 때*/
		else if (tempSearchIndex < 0)
		{
			if (strcmp(token_table[i]->operator, "LTORG") == 0) continue;
			tempLocCtr += set_addr_size(i);
			if (token_table[i]->operator!= NULL && strcmp(token_table[i]->operator, "BYTE") == 0)
			{
				tempLocSize = 1;
				machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
				machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
				machine_code_table[machineCodeIndexer]->machine_code = malloc(sizeof(int));
				*machine_code_table[machineCodeIndexer]->machine_code = 0;
				strcpy(tempStr, token_table[i]->operand[0]);
				char* ptr = strtok(tempStr, "\'");
				ptr = strtok(NULL, "\'");
				*machine_code_table[machineCodeIndexer]->length = (int)strlen(ptr) / 2;	//tempLocSize라고 해도 됨.
				*machine_code_table[machineCodeIndexer]->machine_code = (int)strtol(ptr, NULL, 16);
				machineCodeIndexer += 1;
				continue;
			}
			else if (strcmp(token_table[i]->operator, "WORD") == 0)
			{
				machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
				machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
				machine_code_table[machineCodeIndexer]->machine_code = malloc(sizeof(int));
				*machine_code_table[machineCodeIndexer]->machine_code = 0;
				*machine_code_table[machineCodeIndexer]->length = 3;
				machineCodeIndexer += 1;

				/*원래는 다수 항에서 값이 있는 건 있는 것 대로 연산을 진행해야하지만 여기서는 모두 값이 외부참조라고 가정했다.*/
				strcpy(tempStr, token_table[i]->operand[0]);
				char *ptr = strtok(tempStr, "-");
				int tempIndex = search_sym_table(ptr, sectionIndex);
				if(tempIndex<0) save_ext_ref_table(i, sectionIndex, tempLocCtr-set_addr_size(i), 3);
				else *machine_code_table[machineCodeIndexer]->machine_code = sym_table[tempIndex].addr;
			}
		}
		/*주소를 확인해서 처리해줘야할 리터럴이 존재할 경우 머신코드에 넣어주기. 출력 순서를 지키기 위해 마련한 방법*/
		for (int t = 0; t < MAX_LINES; t++)
		{

			if (search_literal_table(literal_table[t].literal, sectionIndex) < 0) continue;
			if ((literal_table[t].addr == tempLocCtr && literal_table[t].check == 0) || direction == -1)
			{
				/*만약 주소가 이어져있지 않고 떨어져 나온 머신코드가 존재할 때는 머신코드의 코드는 NULL 그리고 length는 띄어져있는 거리만큼 계산해 넣는다.*/
				if (tempLocCtr - set_addr_size(i) - mark - tempLocSize > 0)
				{
					/*출력을 할 때 여기서 추가된 기계어 코드는 분리가 되어 출력이 되기 때문에 머신코드테이블 중간에 차이값을 두어 출력이 용이하게 만든다.*/
					machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
					machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
					machine_code_table[machineCodeIndexer]->machine_code = NULL;
					*machine_code_table[machineCodeIndexer]->length = tempLocCtr - set_addr_size(i) - mark;

					machineCodeIndexer += 1;
				}

				machine_code_table[machineCodeIndexer] = malloc(sizeof(machine_code_table));
				machine_code_table[machineCodeIndexer]->length = malloc(sizeof(int));
				machine_code_table[machineCodeIndexer]->machine_code = malloc(sizeof(int));
				*machine_code_table[machineCodeIndexer]->machine_code = 0;

				if (atoi(literal_table[t].literal) != 0)
				{
					sprintf(tempStr, "%X", (int)literal_table[t].literal[0]-48);
					sprintf(tempStr+1, "%X", (int)literal_table[t].literal[1]-48);
				}
				else
				{
					sprintf(tempStr, "%X", (int)literal_table[t].literal[0]);
					sprintf(tempStr + 2, "%X", (int)literal_table[t].literal[1]);
					sprintf(tempStr + 4, "%X", (int)literal_table[t].literal[2]);
				}
				*machine_code_table[machineCodeIndexer]->length = (int)strlen(tempStr) / 2;
				*machine_code_table[machineCodeIndexer]->machine_code = (int)strtol(tempStr, NULL, 16);
				machineCodeIndexer += 1;
				literal_table[t].check = 1;
				break;
			}
		}
	}
	free(threeByteUnit);
	free(fourByteUnit);
	free(twoByteUnit);
	free(oneByteUnit2);
	free(oneByteUnit);
	free(tempStr);
	return 0;
}

/* ----------------------------------------------------------------------------------
* 설명 : 레지스터가 어떤 번호에 해당되는 지 검색해주는 함수
* 매개 : 레지스터 문자
* 반환 : 레지스터 번호 >= 0, 에러 < 0
* 주의 : 모든 레지스터가 들어간 것이 아니므로 검색 안될 수 있다.
* -----------------------------------------------------------------------------------
*/
int register_number(char* r)
{
	switch (*r)
	{
	case'X':
		return 1;
	case'A':
		return 0;
	case'S':
		return 4;
	case'T':
		return 5;
	default:
		return -1;
	}
}

/* ----------------------------------------------------------------------------------
* 설명 : 출력을 도와주는 함수, 원하는 문자열을 찾아준다.
* 매개 : 섹션이 시작하는 인덱스, 찾고 싶은 문자열
* 반환 : 찾고자 하는 문자열이 존재하는 인덱스, 값없음 < 0
* 주의 : operator만 찾을 수 있다. 중복된 내용이 있어도 무조건 가장 앞에 있는 값을 찾아온다.
* ----------------------------------------------------------------------------------
*/
int search_operator_in_token_table(int index, char* str)
{
	while (token_table[index] != NULL)
	{
		if (token_table[index]->operator!= NULL && strcmp(str, token_table[index]->operator) == 0) return index;
		if (strcmp(str, "CSECT") != 0 && (token_table[index]->operator != NULL && strcmp(token_table[index]->operator, "CSECT") == 0)) break;
		index++;
	}
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 출력을 도와주는 함수, 머신코드를 크기에 맞게 출력해준다.
* 매개 : 출력하고자 하는 머신코드 인덱스
* 반환 : 없음
* 주의 : 없음
* ----------------------------------------------------------------------------------
*/
void print_machine_code(int index, FILE* file)
{
	if (file == NULL)
	{
		if (*machine_code_table[index]->length == 4)
		{
			printf("%08X", *machine_code_table[index]->machine_code);
		}
		else if (*machine_code_table[index]->length == 3)
		{
			printf("%06X", *machine_code_table[index]->machine_code);
		}
		else if (*machine_code_table[index]->length == 2)
		{
			printf("%04X", *machine_code_table[index]->machine_code);
		}
		else if (*machine_code_table[index]->length == 1)
		{
			printf("%02X", *machine_code_table[index]->machine_code);
		}
	}
	else
	{
		if (*machine_code_table[index]->length == 4)
		{
			fprintf(file, "%08X", *machine_code_table[index]->machine_code);
		}
		else if (*machine_code_table[index]->length == 3)
		{
			fprintf(file, "%06X", *machine_code_table[index]->machine_code);
		}
		else if (*machine_code_table[index]->length == 2)
		{
			fprintf(file, "%04X", *machine_code_table[index]->machine_code);
		}
		else if (*machine_code_table[index]->length == 1)
		{
			fprintf(file, "%02X", *machine_code_table[index]->machine_code);
		}
	}
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매개 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	if (file_name == NULL)
	{
		int myLine = 0, searchedLine = 0, mySection = 0;
		int machineCodeIndexer = 0;
		int startAddr = 0;
		while (program_size[mySection] != NULL)
		{
			int totalLength = 0;
			int tempAddr = 0;
			int tempSearchedLine = 0;
			printf("\n\nH%s", token_table[searchedLine]->label);
			printf("%06X%06X", tempAddr, *program_size[mySection]);
			tempSearchedLine = search_operator_in_token_table(searchedLine+1, "EXTDEF");
			if (tempSearchedLine >= 0) searchedLine = tempSearchedLine;
			if (tempSearchedLine >= 0)
			{
				printf("\nD");
				for (int i = 0; i < MAX_OPERAND; i++)
				{
					if (token_table[tempSearchedLine]->operand[i] != NULL)
					{
						printf("%s", token_table[tempSearchedLine]->operand[i]);
						printf("%06X", sym_table[search_sym_table(token_table[tempSearchedLine]->operand[i], mySection)].addr);
					}
				}
			}
			tempSearchedLine = search_operator_in_token_table(searchedLine+1, "EXTREF");
			if (tempSearchedLine >= 0) searchedLine = tempSearchedLine;
			if (tempSearchedLine >= 0)
			{
				printf("\nR");
				for (int i = 0; i < MAX_OPERAND; i++)
				{
					if (token_table[tempSearchedLine]->operand[i] != NULL)
					{
						printf("%s", token_table[tempSearchedLine]->operand[i]);
					}

				}
			}
			int tempIndexer = 0;
			int progress = 0;
			while (machine_code_table[machineCodeIndexer] != NULL)
			{
				printf("\nT");
				printf("%06X", tempAddr);
				tempIndexer = machineCodeIndexer;
				while (1)
				{
					if (machine_code_table[machineCodeIndexer] == NULL || machine_code_table[machineCodeIndexer]->machine_code == NULL
						||totalLength + *machine_code_table[machineCodeIndexer]->length >= 30)
					{
						printf("%02X", totalLength);
						for (int i = tempIndexer; i < machineCodeIndexer; i++)
						{
							print_machine_code(i, NULL);
						}
						if (machine_code_table[machineCodeIndexer] == NULL)break;
						if (machine_code_table[machineCodeIndexer]->machine_code == NULL)
						{
							totalLength += *machine_code_table[machineCodeIndexer]->length;
							machineCodeIndexer += 1;
						}
						progress = machineCodeIndexer;
						tempAddr += totalLength;
						totalLength = 0;
						break;
					}
					else
					{
						totalLength += *machine_code_table[machineCodeIndexer]->length;
						machineCodeIndexer += 1;
					}
				}
			}
			while (ext_ref_table[myLine] != NULL)
			{
				printf("\nM");
				printf("%06X%02X%s", *ext_ref_table[myLine]->addr, *ext_ref_table[myLine]->count, ext_ref_table[myLine]->label);
				myLine += 1;
			}
			printf("\nE");
			if (mySection == 0) printf("%06X", startAddr);

			myLine += 1;
			tempSearchedLine = search_operator_in_token_table(searchedLine, "CSECT");
			if (tempSearchedLine >= 0) searchedLine = tempSearchedLine;
			mySection += 1;
			machineCodeIndexer += 1;
		}
	}
	else
	{
		FILE *file;

		file = fopen(file_name, "w");

		int myLine = 0, searchedLine = 0, mySection = 0;
		int machineCodeIndexer = 0;
		int startAddr = 0;
		while (program_size[mySection] != NULL)
		{
			int totalLength = 0;
			int tempAddr = 0;
			int tempSearchedLine = 0;
			fprintf(file, "\n\nH%s", token_table[searchedLine]->label);
			fprintf(file, "%06X%06X", tempAddr, *program_size[mySection]);
			tempSearchedLine = search_operator_in_token_table(searchedLine + 1, "EXTDEF");
			if (tempSearchedLine >= 0) searchedLine = tempSearchedLine;
			if (tempSearchedLine >= 0)
			{
				fprintf(file, "\nD");
				for (int i = 0; i < MAX_OPERAND; i++)
				{
					if (token_table[tempSearchedLine]->operand[i] != NULL)
					{
						fprintf(file, "%s", token_table[tempSearchedLine]->operand[i]);
						fprintf(file, "%06X", sym_table[search_sym_table(token_table[tempSearchedLine]->operand[i], mySection)].addr);
					}
				}
			}
			tempSearchedLine = search_operator_in_token_table(searchedLine + 1, "EXTREF");
			if (tempSearchedLine >= 0) searchedLine = tempSearchedLine;
			if (tempSearchedLine >= 0)
			{
				fprintf(file, "\nR");
				for (int i = 0; i < MAX_OPERAND; i++)
				{
					if (token_table[tempSearchedLine]->operand[i] != NULL)
					{
						fprintf(file, "%s", token_table[tempSearchedLine]->operand[i]);
					}

				}
			}
			int tempIndexer = 0;
			int progress = 0;
			while (machine_code_table[machineCodeIndexer] != NULL)
			{
				fprintf(file, "\nT");
				fprintf(file, "%06X", tempAddr);
				tempIndexer = machineCodeIndexer;
				while (1)
				{
					if (machine_code_table[machineCodeIndexer] == NULL || machine_code_table[machineCodeIndexer]->machine_code == NULL
						|| totalLength + *machine_code_table[machineCodeIndexer]->length >= 30)
					{
						fprintf(file, "%02X", totalLength);
						for (int i = tempIndexer; i < machineCodeIndexer; i++)
						{
							print_machine_code(i, file);
						}
						if (machine_code_table[machineCodeIndexer] == NULL)break;
						if (machine_code_table[machineCodeIndexer]->machine_code == NULL)
						{
							totalLength += *machine_code_table[machineCodeIndexer]->length;
							machineCodeIndexer += 1;
						}
						progress = machineCodeIndexer;
						tempAddr += totalLength;
						totalLength = 0;
						break;
					}
					else
					{
						totalLength += *machine_code_table[machineCodeIndexer]->length;
						machineCodeIndexer += 1;
					}
				}
			}
			while (ext_ref_table[myLine] != NULL)
			{
				fprintf(file, "\nM");
				fprintf(file, "%06X%02X%s", *ext_ref_table[myLine]->addr, *ext_ref_table[myLine]->count, ext_ref_table[myLine]->label);
				myLine += 1;
			}
			fprintf(file, "\nE");
			if (mySection == 0) fprintf(file, "%06X", startAddr);

			myLine += 1;
			tempSearchedLine = search_operator_in_token_table(searchedLine, "CSECT");
			if (tempSearchedLine >= 0) searchedLine = tempSearchedLine;
			mySection += 1;
			machineCodeIndexer += 1;
		}
		fclose(file);
	}
}
