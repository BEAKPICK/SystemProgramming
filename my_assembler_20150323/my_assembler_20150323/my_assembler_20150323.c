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
	make_opcode_output("output_20150323");

	/*
	* 추후 프로젝트에서 사용되는 부분
	
	make_symtab_output("symtab_00000000");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_00000000") ; 
	*/
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
		char *strRemain = malloc(sizeof(char) * 100);	//strtok를 쓰면 원본 char* 데이터도 손상되기 때문에 복사를 하기 위한 동적할당.
		strcpy(strRemain, str);
		char *ptr = strtok(strRemain, "\t");	//strtok가 맨앞의 \t를 무시한다...
		token_table[token_line] = malloc(sizeof(token_table));
		token_table[token_line]->label = NULL;
		token_table[token_line]->operator = NULL;
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
		if(ptr!=NULL || ptr[0] != ' ')strcpy(tempOperand, ptr);

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
* 매계 : 생성할 오브젝트 파일명
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
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char *file_name)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 LITERAL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_literaltab_output(char *file_name)
{
	/* add your code here */
}

/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */
}
