#include "VM.h"
#include <ctype.h>
#include <string.h>

INSTRUCTION instructions[MAX_CODE_LENGTH];
int instruction_count;

CPU cpu;

int registers[16];

int stack[MAX_STACK_HEIGHT];

int init();
int fetch();
int execute();
void print_instructions();

int main(int argc, char *argv[]) {
	FILE *fp;

	if (argc == 0) {
		printf("Error no file provided!");
		getchar();
		return -1;
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("Error file does not exist!");
		getchar();
		return -1;
	}

	if (get_instructions(fp) != 0) {
		getchar();
		return -1;
	}
	
	init();

	print_instructions();

	getchar();
}

int init() {
	cpu.bp = 1;
	cpu.pc = 0;
	cpu.sp = 0;
	cpu.ir = instructions[0];

	memset(registers, 0, sizeof(int) * 16);
	memset(stack, 0, sizeof(int) * MAX_STACK_HEIGHT);

}

int get_instructions(FILE *fp) {
	int op, r, l, m;

	instruction_count = 0;

	while (fscanf(fp,"%d %d %d %d", &op, &r, &l, &m) != EOF){
		instructions[instruction_count].op = op;
		instructions[instruction_count].r = r;
		instructions[instruction_count].l = l;
		instructions[instruction_count].m = m;
		instruction_count++;
	}

	return 0;
}

void print_instructions() {
	int i;

	for (i = 0; i < instruction_count; i++) {
		char op[4];
		int r, l, m;

		strcpy(op, opcodes[instructions[i].op]);
		r = instructions[i].r;
		l = instructions[i].l;
		m = instructions[i].m;
		printf("%d %s %d %d %d\n", i, op, r, l, m);
	}
}

int fetch() {
	if (cpu.pc < instruction_count) {
		cpu.ir = instructions[cpu.pc];
		cpu.pc += 1;
	} else {
		return -1;
	}
	return 0;
}

int execute() {
	
	switch (cpu.ir.op) {
		case LIT:
			registers[cpu.ir.r] = cpu.ir.m;
			break;
		case RTN:
			cpu.sp = cpu.bp - 1;
			cpu.bp = stack[cpu.sp + 3];
			cpu.pc = stack[cpu.sp + 4];
			break;
		case LOD:
			registers[cpu.ir.r] = stack[base(cpu.ir.l, cpu.bp) + cpu.ir.m];
			break;
		case STO:
			stack[base(cpu.ir.l, cpu.bp) + cpu.ir.m] = registers[cpu.ir.r];
			break;
		case CAL:
			stack[cpu.sp + 1] = 0;						//Space for rtn val
			stack[cpu.sp + 2] = base(cpu.ir.l, cpu.bp); //Static Link
			stack[cpu.sp + 3] = cpu.bp;					//Dynamic Link
			stack[cpu.sp + 4] = cpu.pc;					//Return Address
			cpu.bp = cpu.sp + 1;
			cpu.pc = cpu.ir.m;
			break;
		case INC:
			cpu.sp += cpu.ir.m;
			break;
		case JMP:
			cpu.pc = cpu.ir.m;
			break;
		case JPC:
			if (registers[cpu.ir.r] == 0)
				cpu.pc = cpu.ir.m;
			break;
		case SIO:
			if (cpu.ir.m == 1)
				printf("%d", registers[cpu.ir.r]);
			else if (cpu.ir.m == 2)
				scanf("%d", &registers[cpu.ir.r]);
			else if (cpu.ir.m == 3)
				return 1; //halt
			break;
		case NEG:

	}

}