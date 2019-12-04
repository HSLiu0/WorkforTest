/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"
#include <vector>
#include <string>
#include <deque>

int init(int queue_size);
void *producer (void* parameter);
void *consumer (void* parameter);
int rand_gen(int min, int max);
int id_gen(deque <pair<int, int>> buffer);

struct producer_data {
  int semid;
  int jobs_num;
  deque <pair<int, int>>* buffer;
  int* id;
};

struct consumer_data{
  int semid;
  deque <pair<int, int>>* buffer;
  int* id;
};

int main (int argc, char **argv)
{
  deque <pair<int, int>> buffer;
  int queue_size = stoi(argv[1]);
  int jobs_num = stoi(argv[2]);
  int num_producers = stoi(argv[3]);
  int num_consumers = stoi(argv[4]);
  int semid = init(queue_size);
  
  if (semid == -1){
    cout << "Error with initialisation." << endl;
    return semid;
  }

  vector <pthread_t> pthread_ids;
  
  int producer_id = 1;
  int consumer_id = 1;

  producer_data prod_params = {semid, jobs_num, &buffer, &producer_id};
  for (int i = 0; i < num_producers; i++){
    pthread_t id;
    pthread_create (&id, NULL, producer, (void *)&prod_params);
    pthread_ids.push_back(id);
  }
 
  consumer_data cons_params = {semid, &buffer, &consumer_id};
  for (int i = 0; i < num_consumers; i++){
    pthread_t id;
    pthread_create (&id, NULL, consumer, (void *)&cons_params);
    pthread_ids.push_back(id);
  }
 
  for (auto id: pthread_ids)
    pthread_join (id, NULL);

  sem_close(semid);
  return 0;
}

void *producer (void *parameter)
{

  producer_data params = *(producer_data *) parameter;
  int semid = params.semid;
  int jobs_num = params.jobs_num;
  deque <pair<int, int>> &buffer = *(params.buffer);
  int &id = *(params.id);
  int job_duration, job_id, thread_id;

  sem_wait (semid, IDS);
  thread_id = id++;
  sem_signal (semid, IDS);

  while(1){
    if (jobs_num){
      job_duration = rand_gen(1, 10);
      jobs_num--;
    } else {
      sem_wait (semid, STDOUT);
      cout << "Producer(" <<thread_id<<"): has to wait for an empty slot." << endl;
      sem_signal (semid, STDOUT);
      pthread_exit(0);
    }

    int ret = sem_wait_timeout (semid, SPACE);
    if (ret == -1) {
      sem_wait (semid, STDOUT);
      cout << "Producer("<< thread_id <<"): waited too long." << endl;
      sem_signal (semid, STDOUT);
      pthread_exit(0);
    }
    sem_wait (semid, MUTEX);
    job_id = id_gen(buffer);
    buffer.push_back({job_id, job_duration});
    sem_signal (semid, MUTEX);
    sem_signal (semid, EMPTY);

    sem_wait (semid, STDOUT);
    cout << "Producer("<<thread_id <<"): Job id "<< job_id<<" duration " <<job_duration<<endl;
    sem_signal (semid, STDOUT);
    sleep (rand_gen(1, 5));
  }
}

void *consumer (void *parameter)
{
  consumer_data params = *(consumer_data *) parameter;
  int semid = params.semid;
  deque <pair<int, int>> &buffer = *(params.buffer);
  int &id = *(params.id);
  int thread_id;

  sem_wait(semid, IDS);
  thread_id = id++;
  sem_signal(semid, IDS);

  while(1){
    int ret = sem_wait_timeout (semid, EMPTY);
    if (ret == -1) {
      sem_wait (semid, STDOUT);
      cout << "Consumer("<< thread_id<< "): has zero item to consume." << endl;
      sem_signal (semid, STDOUT);
      pthread_exit(0);
    }
    sem_wait(semid, MUTEX);
    pair<int, int> job = buffer.front();
    buffer.pop_front();
    sem_signal(semid, MUTEX);
    sem_signal(semid, SPACE);
    sem_wait (semid, STDOUT);
    cout << "Consumer(" << thread_id << "): Job id " << job.first<< " executing sleep duration " << job.second<<endl;
    sem_signal(semid, STDOUT);
    sleep (job.second);
  }
}


int init(int queue_size){
  int error, semid;
  semid = sem_create(SEM_KEY, 5);
  error = sem_init (semid, MUTEX, 1);
  if (error) return error;
  error = sem_init (semid, SPACE, queue_size);
  if (error) return error;
  error = sem_init (semid, EMPTY, 0);
  if (error) return error;
  error = sem_init (semid, IDS, 1);
  if (error) return error;
  error = sem_init (semid, STDOUT, 1);
  if (error) return error;
  return semid;
}


int rand_gen(int min, int max){
  return (rand() % (max - min + 1) + min);
}


int id_gen(deque <pair<int, int>> buffer){
  int id = 1;
  for (auto job: buffer){
    if (id == job.first){
      id++; continue;
    }
  }
  return id;
}
