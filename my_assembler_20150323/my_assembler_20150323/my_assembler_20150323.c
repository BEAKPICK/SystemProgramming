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
	make_opcode_output("output_20150323");

	/*
	* ���� ������Ʈ���� ���Ǵ� �κ�
	
	make_symtab_output("symtab_00000000");
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output_00000000") ; 
	*/
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
		char *strRemain = malloc(sizeof(char) * 100);	//strtok�� ���� ���� char* �����͵� �ջ�Ǳ� ������ ���縦 �ϱ� ���� �����Ҵ�.
		strcpy(strRemain, str);
		char *ptr = strtok(strRemain, "\t");	//strtok�� �Ǿ��� \t�� �����Ѵ�...
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
		if(ptr!=NULL || ptr[0] != ' ')strcpy(tempOperand, ptr);

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
	/* add your code here */
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
	/* add your code here */
}

/* --------------------------------------------------------------------------------*
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
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
	/* add your code here */
}
