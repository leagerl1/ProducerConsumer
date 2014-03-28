#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>

typedef struct professor_arguments{
  int id;
  int num_assignings;
  int min_prof_wait;
  int max_prof_wait;
  int min_num_assignments;
  int max_num_assignments;
  int min_assignment_hours;
  int max_assignment_hours;
  int students_per_assignment;
}professor_arguments;
typedef struct assignment{
  int a_id;
  int p_id;
  int hours;
  int studentCount;
}assignment;

void* professor_func();
void* student_func();

pthread_mutex_t rw;
sem_t emptyCount;
sem_t fullCount;
assignment* queue;
int full = 0;
int professorsComplete = 0;
int queueSize;
int professors;
int studentsPer;

int main(int argc, char *argv[])
{
  int c;
  int aValue = 10;
  int wValue = 1;
  int WValue = 5;
  int nValue = 1;
  int NValue = 10;
  int hValue = 1;
  int HValue = 5;
  int pValue = 2;
  int sValue = 2;
  int dValue = sValue;
  int qValue = 8;
  char* usageString = "program2 [-u]\n[-a <num_assignings>]\n[-w <min_prof_wait>] [-W <max_prof_wait>]\n[-n <min_num_assignments>] [-N <max_num_assignments>]\n[-h <min_assignment_hours>] [-H <max_assignment_hours>]\n[-p <num_professors>] [-s <num_students>]\n[-d <studends_per_assignment>] [-q <queue_size>]\n";
  extern char* optarg;
  extern int optopt;

  while((c = getopt(argc, argv, ":ua:w:W:n:N:h:H:p:s:d:q:")) != -1){
      switch(c){
      case 'u':
	fprintf(stderr,"%s\n",usageString);
	exit(0);
	break;      
      case 'a':
	if(isdigit(optarg[0])){
	  aValue = atoi(optarg);
	  if(aValue > 100000 || aValue < 1){
	    fprintf(stderr,"Error: num-assignments value is out-of-range\n");
	    exit(1);
	  }
	}
	else{
	  fprintf(stderr,"Error: Option -a is missing valid argument\n%s\n",usageString);
	  exit(1);
	}
	break;
      case 'w':
	if(isdigit(optarg[0])){
          wValue = atoi(optarg);
	  if(wValue > 10 || wValue < 1){
	    fprintf(stderr, "Error: min_prof_wait value is out-of-range\n");
	    exit(1);
	  }
	}
        else{
          fprintf(stderr,"Error: Option -w is missing valid argument\n%s\n",usageString);
          exit(1);
        }
	break;
      case 'W':
	if(isdigit(optarg[0])){
          WValue = atoi(optarg);
	  if(WValue > 100 || WValue < 1){
	    fprintf(stderr,"Error: max_prof_wait value is out-of-range\n");
	    exit(1);
	  }
	}
        else{
          fprintf(stderr,"Error: Option -W is missing valid argument\n%s\n",usageString);
          exit(1);
        }
	break;
      case 'n':
	if(isdigit(optarg[0])){
          nValue = atoi(optarg);
	  if(nValue > 10 || nValue < 1){
	    fprintf(stderr,"Error: min_num_assignments value is out-of-range\n");
	    exit(1);
	  }
	}
        else{
          fprintf(stderr,"Error: Option -n is missing valid argument\n%s\n",usageString);
          exit(1);
        }
	break;
      case 'N':
	if(isdigit(optarg[0])){
          NValue = atoi(optarg);
	  if(NValue > 100 || NValue < 1){
	    fprintf(stderr,"Error: max_num_assignments value is out-of-range\n");
	    exit(1);
	  }
	}
        else{
          fprintf(stderr,"Error: Option -N is missing valid argument\n%s\n",usageString);
          exit(1);
        }
	break;
      case 'h':
        if(isdigit(optarg[0])){
          hValue = atoi(optarg);
          if(hValue > 5 || hValue < 1){
            fprintf(stderr,"Error: min_assignment_hours value is out-of-range\n");
            exit(1);
          }
        }
        else{
          fprintf(stderr,"Error: Option -h is missing valid argument\n%s\n",usageString);
          exit(1);
        }
        break;
      case 'H':
        if(isdigit(optarg[0])){
          HValue = atoi(optarg);
          if(HValue > 10 || HValue < 1){
            fprintf(stderr,"Error: max_assignment_hours value is out-of-range\n");
            exit(1);
          }
        }
        else{
          fprintf(stderr,"Error: Option -H is missing valid argument\n%s\n",usageString);
          exit(1);
        }
        break;
      case 'p':
        if(isdigit(optarg[0])){
          pValue = atoi(optarg);
          if(pValue > 10 || pValue < 1){
            fprintf(stderr,"Error: num_professors value is out-of-range\n");
            exit(1);
          }
        }
        else{
          fprintf(stderr,"Error: Option -p is missing valid argument\n%s\n",usageString);
          exit(1);
        }
        break;
      case 's':
        if(isdigit(optarg[0])){
          sValue = atoi(optarg);
          if(sValue > 10 || sValue < 1){
            fprintf(stderr,"Error: num_students value is out-of-range\n");
            exit(1);
          }
        }
        else{
          fprintf(stderr,"Error: Option -s is missing valid argument\n%s\n",usageString);
          exit(1);
        }
        break;
      case 'd':
        if(isdigit(optarg[0])){
          dValue = atoi(optarg);
          if(dValue > 10 || dValue < 1){
            fprintf(stderr,"Error: students_per_assignment value is out-of-range\n");
            exit(1);
          }
        }
        else{
          fprintf(stderr,"Error: Option -d is missing valid argument\n%s\n",usageString);
          exit(1);
        }
        break;
      case 'q':
        if(isdigit(optarg[0])){
          qValue = atoi(optarg);
          if(qValue > 256 || qValue < 1){
            fprintf(stderr,"Error: queue_size value is out-of-range\n");
            exit(1);
          }
        }
        else{
          fprintf(stderr,"Error: Option -q is missing valid argument\n%s\n",usageString);
          exit(1);
        }
        break;
      case '?':
	fprintf(stderr, "Invalid option: -%c\n%s", optopt,usageString);
	  exit(1);
	  break;
      case ':':
	fprintf(stderr,"Option -%c is missing argument\n%s",optopt,usageString);
	exit(1);
	break;
      default:
	exit(0);
      }
  }
  if(wValue > WValue || nValue > NValue || hValue > HValue){
    fprintf(stderr,"A min-value exceeds its respective max-value\n%s",usageString);
    exit(1);
  }
  professors = pValue;
  queueSize = qValue;
  studentsPer = dValue;  
  //initialize mutex & semaphore
  pthread_mutex_init(&rw,NULL);
  sem_init(&emptyCount,0,qValue);
  sem_init(&fullCount,0,0);

  //make queue
  queue = malloc(qValue*sizeof(struct assignment));  

  //create professor threads
  pthread_t professor_threads[pValue];
  int i;
  for(i = 0; i < pValue; i++){
    professor_arguments* professorFlags = malloc(sizeof(struct professor_arguments));
    professorFlags->id = i+1;
    professorFlags->num_assignings = aValue;
    professorFlags->min_prof_wait = wValue;
    professorFlags->max_prof_wait = WValue;
    professorFlags->min_num_assignments = nValue;
    professorFlags->max_num_assignments = NValue;
    professorFlags->min_assignment_hours = hValue;
    professorFlags->max_assignment_hours = HValue;
    professorFlags->students_per_assignment = dValue;
    if(pthread_create(&(professor_threads[i]), NULL, professor_func, professorFlags) != 0) {
      perror("pthread_create");
      exit(1);
    }
  }
  
  //create student threads
  pthread_t student_threads[sValue];
  for(i = 0; i < sValue; i++){
    int* id = malloc(sizeof(int));
    *id = i+1;
    if(pthread_create(&(student_threads[i]), NULL, student_func, id) != 0) {
      perror("pthread_create");
      exit(1);
    } 
  }
  for(i = 0; i < pValue; i++){
    if(pthread_join(professor_threads[i], NULL) != 0) {
      perror("pthread_join");
      exit(1);
    }
  }
  for(i = 0; i < sValue; i++){
    if(pthread_join(student_threads[i], NULL) != 0) {
      perror("pthread_join");
      exit(1);
    }
  }
  
  pthread_exit(0);
  return 0;
}

void* professor_func(professor_arguments* professorFlags){
  printf("STARTING Professor %d\n",professorFlags->id);
  int i;
  unsigned int seed = time(NULL);
  for(i = 0; i < professorFlags->num_assignings; i++){
    sleep(professorFlags->min_prof_wait+rand_r(&seed)/(RAND_MAX/(professorFlags->max_prof_wait-professorFlags->min_prof_wait+1)+1));
  }
  int numAssignments = professorFlags->min_num_assignments+rand_r(&seed)/(RAND_MAX/(professorFlags->max_num_assignments-professorFlags->min_num_assignments+1)+1);
  //add numAssingments assignment to queue
  for(i = 0; i < numAssignments; i++){  
    assignment newAssignment;
    newAssignment.p_id = professorFlags->id;
    newAssignment.a_id = i+1;
    newAssignment.hours = professorFlags->min_assignment_hours+rand_r(&seed)/(RAND_MAX/(professorFlags->max_assignment_hours-professorFlags->min_assignment_hours+1)+1);
    newAssignment.studentCount = 0;
    sem_wait(&emptyCount);
    pthread_mutex_lock(&rw);
    queue[full] = newAssignment;
    printf("ASSIGN Professor %d adding Assignment %d: %d Hours\n",professorFlags->id,newAssignment.a_id,newAssignment.hours);
    full++;
    pthread_mutex_unlock(&rw);
    int j;
    for(j = 0; j < studentsPer; j++){   
      sem_post(&fullCount);
    }
  }
  pthread_mutex_lock(&rw); 
  professorsComplete++;
  pthread_mutex_unlock(&rw); 
  printf("EXITING Professor %d\n",professorFlags->id);
  pthread_exit(0);
}

void* student_func(int *thread){
  printf("STARTING Student %d\n",*thread);
  int i;
  assignment previous;
  previous.a_id = 0;
  previous.p_id = 0;
  while(1){
    if(professorsComplete == professors && full == 0){
      break;
    }
    sem_wait(&fullCount);
    pthread_mutex_lock(&rw);
    assignment current = queue[full-1];
    if(previous.a_id == current.a_id && previous.p_id == current.p_id){
      current = queue[full-2];
      queue[full-2].studentCount = queue[full-2].studentCount + 1;
    }
    else{
      queue[full-1].studentCount = queue[full-1].studentCount + 1;
    }
    if(current.studentCount + 1 == studentsPer){
      full--;
      sem_post(&emptyCount); 
    }
    pthread_mutex_unlock(&rw);
    printf("BEGIN Student %d working on Assignment %d from Professor %d\n",*thread,current.a_id,current.p_id);
    for(i = 0; i < current.hours; i++){
      sleep(1);
      printf("WORK Student %d working on Assignment %d Hour %d from Professor %d\n",*thread,current.a_id,i+1,current.p_id);
    }
    printf("END Student %d working on Assignment %d from Professor %d\n",*thread,current.a_id,current.p_id);
    previous = current;
  }
  printf("EXITING Student %d\n",*thread);
  pthread_exit(0);
}

