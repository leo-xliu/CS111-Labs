#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process {
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 remaining_time;
  bool is_first_iteration;
  /* End of "Additional fields here" */
};

TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end) {
  u32 current = 0;
  bool started = false;
  while (*data != data_end) {
    char c = **data;

    if (c < 0x30 || c > 0x39) {
      if (started) {
	return current;
      }
    }
    else {
      if (!started) {
	current = (c - 0x30);
	started = true;
      }
      else {
	current *= 10;
	current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data) {
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++])) {
    if (c < 0x30 || c > 0x39) {
      exit(EINVAL);
    }
    if (!started) {
      current = (c - 0x30);
      started = true;
    }
    else {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1) {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED) {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;
  

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL) {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i) {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }
  
  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

  /* Your code here */

  //fill additional fields
  struct process* current_process; 
  for(u32 i = 0; i < size; i++){
    current_process = &data[i];
    current_process->remaining_time = current_process->burst_time;
    current_process->is_first_iteration = true;
  }
  
  //sort by arrival times: selection sort
  for(u32 i = 0; i < size-1; i++){
    current_process = &data[i];
    u32 bestInd = i;
    for(u32 j = i+1; j < size; j++){
      struct process* next_process = &data[j];
      if (current_process->arrival_time > next_process->arrival_time){
        current_process = next_process;
        bestInd = j;
      }
      else if (current_process->arrival_time == next_process->arrival_time){
        if (current_process->pid > next_process->pid){
          current_process = next_process;
          bestInd = j;
        }
      }
    }
    if (bestInd != i){
      struct process temp_process = data[i];
      data[i] = data[bestInd];
      data[bestInd] = temp_process;
    }
  }  

  //begin round robin scheduling
  u32 current_time = 0;
  u32 cur_quantum_len = quantum_length;
  u32 process_next = 0;
  if (process_next < size){
    current_process = &data[process_next];
    if (current_process->arrival_time == current_time){
      TAILQ_INSERT_TAIL(&list, current_process, pointers);
      process_next++;
    }
  }
  while (true){
    if (process_next == size && TAILQ_EMPTY(&list)){
      break;
    }
    //new process arrival
      if (process_next < size){
        current_process = &data[process_next];
        if (current_process->arrival_time == current_time+1){
          TAILQ_INSERT_TAIL(&list, current_process, pointers);
          process_next++;
        }
      }
    //execute top process by 1 unit time
    if (!TAILQ_EMPTY(&list)){
      current_process = TAILQ_FIRST(&list);
      if (current_process->is_first_iteration){
        current_process->is_first_iteration = false;
        total_response_time += current_time - current_process->arrival_time;
      }
      cur_quantum_len--;
      current_process->remaining_time--;
      if (current_process->remaining_time == 0){
        total_waiting_time += current_time + 1 - current_process->arrival_time - current_process->burst_time;
        TAILQ_REMOVE(&list, current_process, pointers);
        cur_quantum_len = quantum_length;
      }
      else if (cur_quantum_len == 0){
        cur_quantum_len = quantum_length;
        TAILQ_REMOVE(&list, current_process, pointers);
        TAILQ_INSERT_TAIL(&list, current_process, pointers);
      }
    }
    current_time++;
  }

  /* End of "Your code here" */

  printf("Average waiting time: %.2f\n", (float) total_waiting_time / (float) size);
  printf("Average response time: %.2f\n", (float) total_response_time / (float) size);

  free(data);
  return 0;
}
