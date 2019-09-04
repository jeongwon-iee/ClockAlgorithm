#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct PAGE { //page table
	int memory_location;
	int valid_bit;
};
struct PAGE page[200000];

struct MEMORY { //physical memory
	int page_num;
	int reference_bit;
	int dirty_bit;
};

int main(int argc, char *argv[]) {

	int memory_size = atoi(argv[1]);
	FILE *input_file = fopen("a.txt", "r");
	if (input_file == NULL) {
		printf("Can't open input file\n");
		return -1;
	}
	struct MEMORY *memory = (struct MEMORY*)malloc(memory_size * sizeof(struct MEMORY));

	int pg = 0, op = 0; //������ ��ȣ, ���� ����
	int new_disk_head_position = 0; //��ũ ��� ��ġ(��������ȣ/1000)
	int old_disk_head_position = 0;
	int clock_pointer = 0;
	int full = 0;
	int page_fault_count = 0;
	int disk_write_count = 0;
	int disk_head_moving_distance = 0;

	for (int i = 0; i < memory_size; i++) {
		memory[i].page_num = 0;
		memory[i].reference_bit = 0;
		memory[i].dirty_bit = 0;
	}

	for (int i = 0; i < 200000; i++) page[i].valid_bit = 0;
	while (!feof(input_file)) {
		fscanf(input_file, "%d %d", &pg, &op);
		if (page[pg].valid_bit == 1) { //memory�� �ִ� ���
			//reference bit_1��
			memory[page[pg].memory_location].reference_bit = 1;
			//write��û�� ��� dirty_bit�� 1��
			if (op == 1) memory[page[pg].memory_location].dirty_bit = 1;
		}
		else { //�������� �ʴ� ���(������ ��Ʈ);�̶��� �ٴ� ������

			//��������Ʈ Ƚ�� ����
			page_fault_count++;
			if (full == 1) { /* ����� ���� ��� */
				//Ŭ�� �ð�ٴ� �̵���Ű�� refercencebit1�� 0����, 0�� ������ Ž��;
				while (clock_pointer <= memory_size) {
					if (clock_pointer == memory_size) clock_pointer = 0; //�ѹ��� ����
					if (memory[clock_pointer].reference_bit == 0) break;
					else memory[clock_pointer].reference_bit = 0;
					clock_pointer++;
				}
				//���۷��� 0�ΰ� �Ѱܳ� ������
				//�Ѱܳ��� �������� �븮�啻 0���� ����
				page[memory[clock_pointer].page_num].valid_bit = 0;
				//�Ѱܳ��� �������� �������� 1�̸�(write��) 
				if (memory[clock_pointer].dirty_bit == 1) {
					//��ũ ����� ���� �̵��Ÿ� �� ���� Ƚ��(disk write) ����;
					new_disk_head_position = memory[clock_pointer].page_num / 1000;
					disk_head_moving_distance += abs(old_disk_head_position - new_disk_head_position);
					old_disk_head_position = new_disk_head_position;
					disk_write_count++;
					memory[clock_pointer].dirty_bit = 0;
				}//������� ������ �ѾƳ���
			}
			//��û�� �������� ��ũ���� �о� ���鼭 ��ũ ��� �̵� �Ÿ� ����;
			new_disk_head_position = pg / 1000;
			disk_head_moving_distance += abs(old_disk_head_position - new_disk_head_position);
			old_disk_head_position = new_disk_head_position;
			//Ŭ�� �ٴ��� ����Ű�� �޸� ��ġ�� ���� ���� ������ ��ȣ �Է�;
			memory[clock_pointer].page_num = pg;
			//���� ���� �������� ���� ��û�� ���� ��û�� ��� dirty_bit ����;
			if (op == 1) memory[clock_pointer].dirty_bit = 1;
			//���� ���� �������� memory_location�� ������ ��ȣ ����;
			page[pg].memory_location = clock_pointer;
			//���� ���� �������� valid_bit 1�� ����;
			page[pg].valid_bit = 1;
			clock_pointer++;
			if (clock_pointer == memory_size) full = 1;
		}
	}
	fclose(input_file);
	printf("%d\n%d\n%d\n", page_fault_count, disk_write_count, disk_head_moving_distance);
	return 0;
}