/*
 * ȭ�ϸ� : my_assembler_00000000.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20150323.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[])
{
	if (init_my_assembler() < 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0)
	{
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}
	/*opcode ����� ����*/
	//make_opcode_output("output_20150323");

	/*������ �����Ǵ� ����*/

	//symtab_20150323 literaltab_20150323

	make_symboltab_literaltab();
	make_symtab_output("symtab_20150323");
	make_literaltab_output("literaltab_20150323");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}
	//"output_20150323"
	make_objectcode_output("output_20150323") ;
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
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
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
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
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�. 
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char *str)
{
	if (str != NULL)
	{
		char *tempOperand = malloc(sizeof(char) * 40);
		for (int i = 0; i < 40; i++) tempOperand[i] = 0;
		char *strRemain = malloc(sizeof(char) * 100);	//strtok�� ���� ���� char* �����͵� �ջ�Ǳ� ������ ���縦 �ϱ� ���� �����Ҵ�.
		strcpy(strRemain, str);
		char *ptr = strtok(strRemain, "\t");	//strtok�� �Ǿ��� \t�� �����Ѵ�...

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
			/*�տ� ���̺��� �ִ� ��� ptr�� label�̹Ƿ� �Ҵ��� operator�Ҵ�*/
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
			/*�տ� ���̺��� ���� ��� ptr�� operator�̹Ƿ� �ٷ� �־��ش�*/
			token_table[token_line]->operator = (char*)malloc(sizeof(char) * 40);
			if (ptr != NULL)strcpy(token_table[token_line]->operator, ptr);
		}

		ptr = strtok(NULL, "\t");
		if(ptr!=NULL && ptr[0] != ' ')strcpy(tempOperand, ptr);

		/*comment�Ҵ�*/
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
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : ��ū ������ ���е� ���ڿ� 
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
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
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
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
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ� 
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 * ���� �� �Ľ� �˻�
	 */
	free(str);
	return errno;
}

/* ----------------------------------------------------------------------------------
* ���� : assem_pass1�� ���ǻ��׿� �ǰ��� ���� �˻� �Լ�. ���۷����� ������� �°�
*		�Ľ��� �Ǿ����� Ȯ���Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, ���� < 0
* ���� : ����� ��ȯ������ ��� parsing �˻縦 �����Ѵ�.
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
				/*strtok�� �ǳʶٴ� ������ �ذ��ϱ� ���� ���� ���� �߰��� ������ �����̽��ٸ� �־��µ� �� ������ �����̽��ٵ� ���۷��尡 ����*/
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
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
				/*input�� operand�� ���� ��� " "�� operand�� �ν��ϰԲ� input�� ����*/
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ LITERAL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
* ���� : �ɺ������� ������ ���� �� �����ȣ�� �ν��ؼ� ����� ó���ϴ� �Լ�.
* �Ű� : �Ľ��� ���۷���
* ��ȯ : ����
* ���� : �� �� �����̴�.
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
* ���� : �ɺ����̺�� ���ͷ����̺��� �ѹ��� ����� ���� �Լ��̴�. ���α׷� ũ�⵵ ���⼭ ���Ѵ�.
* �Ű� : ����
* ��ȯ : ����
* ���� : ����
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

		/*�߰��� CSECT�� ������ tempLocCtr �ʱ�ȭ*/
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

		/*locSize�� ���ϴ� ���� ������ �߰��� =.''���°� �ִٸ� ���ͷ����̺� �־��ֱ�*/
		if (token_table[i]->operator[0] == '+') locSize = 4;
		else
		{
			/*���۷��忡 =�� ���� �� ��, ���ͷ��� ������ �� ���ͷ� ���̺� ���ͷ� �����س���*/
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

			/*������ �� 1,2,3/4�� ������ �������� locSize ����. inst_table�� type�� ������ ������������ �ԷµǾ��ٰ� ��������.*/
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
			/*����� �����Ǵ� operator�� �ƴ� �� locSize����*/
			else
			{
				locSize = set_addr_size(i);
			}
		}

		/*�ɺ����̺� �ڵ鸵*/
		if (token_table[i]->label != NULL)
		{
			strcpy(sym_table[symIndexer].symbol, token_table[i]->label);
			if (strcmp(token_table[i]->operator, "EQU") == 0 && strcmp(token_table[i]->operand[0], "*") != 0)
			{
				/*���� -�� ���ǵ��� �س���...�ٸ� �����ȣ���� ���ԵǷ��� �Ƹ� �Լ��� ���� ���ߵ� �� ����.*/
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
		/*LTORG�� ��Ÿ�� ��� LTORG ������ ��Ÿ���� ���ͷ����� �ּҸ� �Ҵ�, ���ͷ����̺� �ڵ鸵*/
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

	/*�� �����ٸ� ���� tempLocCtr�� �Ҵ���� ���� ���ͷ��� �ּҸ� �Ҵ��ϴ� �۾�*/
	for (int i = tempLiteralIndex; i < literalIndexer; i++)
	{
		locSize = 0;
		/*LTORG�� �� ó���� ���� Ȯ���� ���� ������ ���ͷ��� �ּҴ� 0�� �� �� ����.*/
		if (literal_table[i].literal != "" && literal_table[i].addr == 0)
		{
			literal_table[i].addr = tempLocCtr;
			/*���ͷ��� ������ ��*/
			if (atoi(literal_table[i].literal) == 0)
			{
				locSize = (int)strlen(literal_table[i].literal);
			}
			/*���ͷ��� ���ڰ� �ƴ϶� ������ ��*/
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
* ���� : ������ ���ͷ��� �ߺ��Ǿ� ���̺� ���� ���� �����ϱ� ���� �ش� ���ͷ���
	���̺� �ִ� ���� ���� ���θ� Ȯ���ϴ� �Լ�, �ε����� ��ȯ�ϴ� ������� ����.
* �Ű� : ���ͷ�
* ��ȯ : �ε��� >= 0, ������� < 0
* ���� : ����
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
* ���� : ����� �������� �ʴ� �ڵ忡 ���ؼ� ī��Ʈ�ؾ� �Ǵ� �ּ��� ũ�⸦ �����ϴ� �Լ�.
* �Ű� : ��ū ���̺� �ε���
* ��ȯ : �ּ��� ������
* ���� : ����
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
* ���� : �ɺ����̺��� �˻��ؼ� �˻��� �������� �ε����� ��ȯ�ϰ� 
	����� ���ٸ� 0���� ���� ���� ����
* �Ű� : �ɺ�, �����ε���
* ��ȯ : �ε��� >= 0 / �������(-1), �˻��� ������ �ٸ� ��Ʈ�� ���ǿ� ����(-2) < 0
* ���� : �ش� ������ �ٸ� ���ǿ� �ִ� ���� �ƿ� ���� ��츦 ������ �����Ѵ�.
	�ش� ���Ǳ��� ��� ã�Ƽ� �ε����� �����ؾߵȴ�. ���� ������ �������� �ϱ� �����̴�.
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
* ���� : nixbpe�� �������ִ� �κ��̴�. Control Section �����̹Ƿ� 
*	������ PCī���ͷ� ���Ǿ����� ������ �����ϰ�
*	BASEī���͸� ����ϴ� ���� ������׿��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : ����
* ���� : ��Ģ������ �ɺ����̺�� ���ͷ����̺� �۾��� �Ϸᰡ �� �� ȣ���� �ϴ� ���� �´�.
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

		/*nixbpe�� ���Ǵ� ���*/
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
* ���� : �Ķ���ͷ� ���� ������ ������ �ܺ����� ���̺� �־��ִ� ������ �ϴ� �Լ�.
* �Ű� : ��ū ���̺� �ε���, ���� ������ �ܺ����� ���̺� �ε���, ���� ���� �ε��� , ���� �ּ�, ����Ʈ ������
* ��ȯ : ����
* ���� : operand�� ������ �׻� �Ǿ��� +, �� �ڷδ� ��� -�̴�. operand�� ���� ���� ������ �� ���Ǵ� �뵵�̴�.
* -----------------------------------------------------------------------------------
*/
void save_ext_ref_table(int tokenIndex, int sectionIndex ,int addr, int tempLocSize)
{
	char* tempStr = malloc(sizeof(char) * 15);
	int tempSearchIndex = 0;
	strcpy(tempStr, token_table[tokenIndex]->operand[0]);
	char* ptr = strtok(tempStr, "-");

	/*������ ���� operand�� �����ٸ� �켱 operand�� ��� ������������ Ȯ��*/
	/*�� ���� ������ -�� ����Ѵ�.*/
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
				/*�ܺ������̹Ƿ� ���� ���� ������ ��Ʈ ���� �þ��.*/
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
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
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
	int mask = 255;	//���� ����� �� ����ŷ�� �ؼ� ��Ȯ�� ����� ������ �ϱ� ����
	int mask2 = 15;	//���� ����� �� ����ŷ�� �ؼ� ��Ȯ�� ����� ������ �ϱ� ����
	int direction = 0;
	/*direction���� ������ �����Ͽ���.*/
	int mark = 0;
	/*�ӽ��ڵ忡�� �߰��� ���� ������ ������ �κ��� ����µ� �� �κ��� ���̸� �ӽ��ڵ� ���̺� ����ϱ� ���ؼ� mark�������*/
	/*������ �ڵ尡 ���� �ʴ� ������ �� �𸣱� ������ �������μ��� RESW�� �����س��´�.*/
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

		/*���þ��� Ÿ��(����)�� 3 Ȥ�� 4�� ��*/
		if (tempSearchIndex >= 0 && inst_table[tempSearchIndex]->type[0] >= 3)
		{
			/*opCode�� nixbpe�� threeByteUnit�� �ִ� ����, oneByteUnit2�� opCode�� �ְ� �ִ� ����̴�.*/
			*oneByteUnit2 = (unsigned char)strtol(inst_table[tempSearchIndex]->opCode, NULL, 16);

			/*+��ȣ�� ���� ����� ���Ѵ�.*/
			if (token_table[i]->operator[0] == '+')
			{
				tempLocCtr += 4;
				tempLocSize = 4;

				fourByteUnit[0] = 0;
				fourByteUnit[1] = 0;
				fourByteUnit[2] = 0;
				fourByteUnit[3] = 0;

				/*�� ������ opCode�� oneByteUnit�� nixbpe�� ��ġ�� �����̴�*/
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

				/*�� ������ opCode�� oneByteUnit�� nixbpe�� ��ġ�� �����̴�*/
				*oneByteUnit2 += (*oneByteUnit >> 4);
				threeByteUnit[0] = *oneByteUnit2;
				threeByteUnit[1] = (*oneByteUnit << 4);
			}

			/*Ÿ�� 3�̻��� �� displacement�� threeByteUnit�� �ִ� ����*/
			/*#�� ���ִ� �ֵ��� ó���ϴ� ����*/
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
			/*operand�� #�� ���� �ֵ��� ó���ϴ� ����*/
			else
			{
				/*Ÿ�� 3�̻��� 1�� �̻��� operand�� ������ ������ �̶��� RSUB�� ���縦 �����ϰ� �־���...*/
				tempSearchIndex = search_sym_table(token_table[i]->operand[0], sectionIndex);
				/*�ɺ����̺� �˻��Ǵ� operand�� �����ٸ�*/
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
				/*�ɺ����̺� �˻����� �ʴ´ٸ� ���ͷ��� �ƴ� �� �˻��غ���. ���ͷ� ó���� ���⼭ ���ش�.*/
				else if (token_table[i]->operand[0] != NULL && token_table[i]->operand[0][0] == '=')
				{
					strcpy(tempStr, token_table[i]->operand[0]);
					char* ptr = strtok(tempStr, "\'");
					ptr = strtok(NULL, "\'");
					tempSearchIndex = search_literal_table(ptr, sectionIndex);
					if (tempSearchIndex >= 0)
					{
						/*�˻��� �Ǹ� ���� threeByteUnit Ȥ�� fourByteUnit�� displacement�� ���� ���ʴ�*/
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
				/*���ͷ�, �ɺ����̺� ��� ���� ��� �ܺ������� �����ϰ� tempLocCtr�� �����ص���. �̰��� ���߿� ����� �� ���δ�.*/
				else
				{
					/*������ �׻� RSUB���� �༮�� �ִٴ� �� ��������... ���þ �´µ� operand�� �ʿ���� 3,4������ RSUB...(OMG)*/
					if (token_table[i]->operand[0] == NULL || token_table[i]->operand[0] == " "
						|| tempSearchIndex == -1)
					{
						/*operand�� �ƿ� ���ų� ���������� ���� �� ���̳� �̸� ��ĵ�Ͽ� �������� �ɺ����̺� �ƿ� ���...RSUB�� �� ����������.
						��¿ �� ����...RSUB�� ���� �˻�������...RSUB�̸� �ܺ��������̺� ������ �ȵȴ�. ���п� �ڵ尡 �ſ� ����������.*/
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
			/*�ᱹ�� �۾��ߴ� 3byte Ȥ�� 4byte �ӽ��ڵ带 ��� ������ �ؾߵǴµ� direction������ �̿��ؼ� �߰��������� �����Ͽ� ������� �ε��ϵ��� �Ѵ�.*/
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
		/*2���� ���þ��� ��*/
		else if (tempSearchIndex >= 0 && inst_table[tempSearchIndex]->type[0] == 2)
		{
			tempLocCtr += 2;
			tempLocSize = 2;

			twoByteUnit[0] = 0;
			twoByteUnit[1] = 0;

			*oneByteUnit2 = (unsigned char)strtol(inst_table[tempSearchIndex]->opCode, NULL, 16);

			twoByteUnit[0] += *oneByteUnit2;

			tempSearchIndex = register_number(token_table[i]->operand[0]);	//���⼱ �ε����� �ƴ� ����
			tempSearchIndex = tempSearchIndex << 4;
			twoByteUnit[1] += tempSearchIndex;
			if (token_table[i]->operand[1] != NULL)
			{
				tempSearchIndex = register_number(token_table[i]->operand[1]);	//���⼱ �ε����� �ƴ� ����
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
		/*1������ �������� �ʰڽ��ϴ�*/
		/*���þ �ƴ� ��*/
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
				*machine_code_table[machineCodeIndexer]->length = (int)strlen(ptr) / 2;	//tempLocSize��� �ص� ��.
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

				/*������ �ټ� �׿��� ���� �ִ� �� �ִ� �� ��� ������ �����ؾ������� ���⼭�� ��� ���� �ܺ�������� �����ߴ�.*/
				strcpy(tempStr, token_table[i]->operand[0]);
				char *ptr = strtok(tempStr, "-");
				int tempIndex = search_sym_table(ptr, sectionIndex);
				if(tempIndex<0) save_ext_ref_table(i, sectionIndex, tempLocCtr-set_addr_size(i), 3);
				else *machine_code_table[machineCodeIndexer]->machine_code = sym_table[tempIndex].addr;
			}
		}
		/*�ּҸ� Ȯ���ؼ� ó��������� ���ͷ��� ������ ��� �ӽ��ڵ忡 �־��ֱ�. ��� ������ ��Ű�� ���� ������ ���*/
		for (int t = 0; t < MAX_LINES; t++)
		{

			if (search_literal_table(literal_table[t].literal, sectionIndex) < 0) continue;
			if ((literal_table[t].addr == tempLocCtr && literal_table[t].check == 0) || direction == -1)
			{
				/*���� �ּҰ� �̾������� �ʰ� ������ ���� �ӽ��ڵ尡 ������ ���� �ӽ��ڵ��� �ڵ�� NULL �׸��� length�� ������ִ� �Ÿ���ŭ ����� �ִ´�.*/
				if (tempLocCtr - set_addr_size(i) - mark - tempLocSize > 0)
				{
					/*����� �� �� ���⼭ �߰��� ���� �ڵ�� �и��� �Ǿ� ����� �Ǳ� ������ �ӽ��ڵ����̺� �߰��� ���̰��� �ξ� ����� �����ϰ� �����.*/
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
* ���� : �������Ͱ� � ��ȣ�� �ش�Ǵ� �� �˻����ִ� �Լ�
* �Ű� : �������� ����
* ��ȯ : �������� ��ȣ >= 0, ���� < 0
* ���� : ��� �������Ͱ� �� ���� �ƴϹǷ� �˻� �ȵ� �� �ִ�.
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
* ���� : ����� �����ִ� �Լ�, ���ϴ� ���ڿ��� ã���ش�.
* �Ű� : ������ �����ϴ� �ε���, ã�� ���� ���ڿ�
* ��ȯ : ã���� �ϴ� ���ڿ��� �����ϴ� �ε���, ������ < 0
* ���� : operator�� ã�� �� �ִ�. �ߺ��� ������ �־ ������ ���� �տ� �ִ� ���� ã�ƿ´�.
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
* ���� : ����� �����ִ� �Լ�, �ӽ��ڵ带 ũ�⿡ �°� ������ش�.
* �Ű� : ����ϰ��� �ϴ� �ӽ��ڵ� �ε���
* ��ȯ : ����
* ���� : ����
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
