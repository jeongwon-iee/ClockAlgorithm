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

	int pg = 0, op = 0; //페이지 번호, 연산 종류
	int new_disk_head_position = 0; //디스크 헤드 위치(페이지번호/1000)
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
		if (page[pg].valid_bit == 1) { //memory에 있는 경우
			//reference bit_1로
			memory[page[pg].memory_location].reference_bit = 1;
			//write요청인 경우 dirty_bit도 1로
			if (op == 1) memory[page[pg].memory_location].dirty_bit = 1;
		}
		else { //존재하지 않는 경우(페이지 폴트);이때만 바늘 움직임

			//페이지폴트 횟수 증가
			page_fault_count++;
			if (full == 1) { /* 빈공간 없는 경우 */
				//클락 시계바늘 이동시키며 refercencebit1은 0으로, 0인 페이지 탐색;
				while (clock_pointer <= memory_size) {
					if (clock_pointer == memory_size) clock_pointer = 0; //한바퀴 돌기
					if (memory[clock_pointer].reference_bit == 0) break;
					else memory[clock_pointer].reference_bit = 0;
					clock_pointer++;
				}
				//레퍼런스 0인게 쫓겨날 페이지
				//쫓겨나는 페이지의 밸리드빝 0으로 리셋
				page[memory[clock_pointer].page_num].valid_bit = 0;
				//쫓겨나는 페이지의 더리빗이 1이면(writeㅇ) 
				if (memory[clock_pointer].dirty_bit == 1) {
					//디스크 기록을 위해 이동거리 및 쓰기 횟수(disk write) 증가;
					new_disk_head_position = memory[clock_pointer].page_num / 1000;
					disk_head_moving_distance += abs(old_disk_head_position - new_disk_head_position);
					old_disk_head_position = new_disk_head_position;
					disk_write_count++;
					memory[clock_pointer].dirty_bit = 0;
				}//여기까지 페이지 쫓아내기
			}
			//요청된 페이지를 디스크에서 읽어 오면서 디스크 헤드 이동 거리 증가;
			new_disk_head_position = pg / 1000;
			disk_head_moving_distance += abs(old_disk_head_position - new_disk_head_position);
			old_disk_head_position = new_disk_head_position;
			//클락 바늘이 가리키는 메모리 위치에 새로 들어온 페이지 번호 입력;
			memory[clock_pointer].page_num = pg;
			//새로 들어온 페이지에 대한 요청이 쓰기 요청인 경우 dirty_bit 세팅;
			if (op == 1) memory[clock_pointer].dirty_bit = 1;
			//새로 들어온 페이지의 memory_location에 프레임 번호 지정;
			page[pg].memory_location = clock_pointer;
			//새로 들어온 페이지의 valid_bit 1로 세팅;
			page[pg].valid_bit = 1;
			clock_pointer++;
			if (clock_pointer == memory_size) full = 1;
		}
	}
	fclose(input_file);
	printf("%d\n%d\n%d\n", page_fault_count, disk_write_count, disk_head_moving_distance);
	return 0;
}