#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define END_TIME 480

double inter_arrival_time, max_time, min_time;
double current_time = 0;
double generate_time = 0;
double total_time = 0;
double wait_time[4] = { 0,0,0,0 };
int count;

enum ev_type { ARRIVAL, DEP_Q0, DEP_Q1, DEP_Q2, DEP_Q3 };

struct node {
	double time;
	double duration;
	enum ev_type ev_type;
	struct node *next;
} *evlist;
typedef struct node *nptr;

struct queue {
	nptr front;
	nptr rear;
	int num;
	int total_num;
	int users;
} Q[4];

void generate_new_customer();
void Insert_evlist(struct node *);
nptr Delete_evlist();
void Insert_Queue(nptr, int);
nptr Delete_Queue(int);
int short_Queue();
void arrive();
void depart();

int main(void) {
	int i;

	printf("평균 도착 시간? ");
	scanf("%lf", &inter_arrival_time);
	printf("최소 소요 시간? ");
	scanf("%lf", &min_time);
	printf("최대 소요 시간? ");
	scanf("%lf", &max_time);

	printf("Input Setting:\n");
	printf("	- Arrival time		= %f\n", inter_arrival_time);
	printf("	- Minimal duration	= %f\n", min_time);
	printf("	- Maximal duration	= %f\n\n", max_time);

	srand((unsigned)time(NULL));
	evlist = NULL;
	for (i = 0; i < 4; i++) {
		Q[i].front = NULL;
		Q[i].total_num = 0;
		Q[i].num = 0;
	}

	generate_new_customer();
	while ((evlist != NULL)) {
		if (evlist->ev_type == ARRIVAL)
			arrive();
		else
			depart();
	}

	printf("Output Result:\n");
	printf("	- Average response time	= %f분\n", total_time / count);
	for (i = 0; i < 4; i++)
		printf("	- Average length of Q%d	= %f\n", i, Q[i].total_num == 0 ? 1 : (double)Q[i].total_num / Q[i].users);

	free(evlist);

	return 0;
}

void generate_new_customer() {
	if (generate_time + inter_arrival_time <= 480) {
		struct node *customer;

		generate_time += inter_arrival_time;

		customer = (struct node*)malloc(sizeof(struct node));
		customer->time = generate_time;
		customer->duration = ((max_time - min_time)*((double)rand() / RAND_MAX)) + min_time;
		customer->ev_type = ARRIVAL;
		customer->next = NULL;

		Insert_evlist(customer);
		current_time = generate_time;
	}
}

void Insert_evlist(struct node *node) {
	nptr temp = evlist;

	if (evlist == NULL) {
		node->next = NULL;
		evlist = node;
		return;
	}

	if ((node->time) < (evlist->time)) {
		node->next = evlist;
		evlist = node;
	}
	else {
		struct node *prev = NULL;
		while ((temp != NULL) && (node->time >= temp->time)) {
			prev = temp;
			temp = temp->next;
		}
		node->next = temp;
		prev->next = node;
	}
}

nptr Delete_evlist() {
	nptr ret_node;

	if (evlist == NULL) {
		printf("evlist가 비었습니다.\n");
		return NULL;
	}

	ret_node = evlist;
	if (evlist->next != NULL)
		evlist = evlist->next;
	else
		evlist = NULL;

	return ret_node;
}

void Insert_Queue(nptr temp, int pos) {
	if (Q[pos].num == 0)
		Q[pos].front = Q[pos].rear = temp;
	else {
		Q[pos].rear->next = temp;
		Q[pos].rear = temp;
	}
	Q[pos].num++;
	Q[pos].total_num += Q[pos].num;
}

nptr Delete_Queue(int pos) {
	nptr del_node;

	del_node = Q[pos].front;
	Q[pos].front = Q[pos].front->next;

	return del_node;
}

int short_Queue() {
	int i, min = 0;

	for (i = 1; i < 4; i++) {
		if (Q[min].num > Q[i].num)
			min = i;
	}
	return min;
}

void arrive() {
	int pos;
	nptr temp;
	struct node *DEP_node;

	pos = short_Queue();
	temp = Delete_evlist();
	current_time = temp->time;

	DEP_node = (struct node *)malloc(sizeof(struct node));
	switch (pos) {
	case 0:
		DEP_node->ev_type = DEP_Q0;
		break;
	case 1:
		DEP_node->ev_type = DEP_Q1;
		break;
	case 2:
		DEP_node->ev_type = DEP_Q2;
		break;
	case 3:
		DEP_node->ev_type = DEP_Q3;
		break;
	}

	if (Q[pos].num == 0)
		DEP_node->time = temp->time + temp->duration;
	else
		DEP_node->time = wait_time[pos] + temp->duration;
	wait_time[pos] = DEP_node->time;
	DEP_node->duration = temp->duration;
	DEP_node->next = NULL;
	Insert_evlist(DEP_node);
	Insert_Queue(temp, pos);

	generate_new_customer();
}

void depart() {
	int pos;
	double wait_time;

	switch (evlist->ev_type) {
	case DEP_Q0:
		pos = 0;
		break;
	case DEP_Q1:
		pos = 1;
		break;
	case DEP_Q2:
		pos = 2;
		break;
	case DEP_Q3:
		pos = 3;
	}

	count++;
	Q[pos].users++;
	Q[pos].num--;
	current_time = evlist->time;
	wait_time = current_time - Q[pos].front->time;
	total_time += wait_time;

	Delete_Queue(pos);
	Delete_evlist();
}