#include<linux/delay.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/time.h>
#include<linux/timer.h>
#include<linux/semaphore.h>
#include<linux/unistd.h>


#define MAX_CUSTOMERS 10

int customer(void *number);
int barber(void *junk);

static struct semaphore wait_room;
static struct semaphore b_chair;
static struct semaphore b_sleep;
static struct semaphore seat_belt;



int Done = 0 ;

asmlinkage long sys_sleepingbarber(void){

struct task_struct *btid;
struct task_struct *tid[10];


int num_chairs=3;
int num_cust=7;
int Num[MAX_CUSTOMERS];
int i=0;

printk("Number of Chairs are %d and Number of customers are %d\n",num_chairs,num_cust);


if(num_cust > MAX_CUSTOMERS){

printk("Max allowed Customers are %d\n",MAX_CUSTOMERS);  return 0; }



printk("--------------------SOLVING THE SLEEPING BARBER PROBLEM--------------------------\n");



for (i = 0; i < MAX_CUSTOMERS; i++) {
Num[i] = i;
}



sema_init(&wait_room, num_chairs);
sema_init(&b_chair, 1);
sema_init(&b_sleep, 0);
sema_init(&seat_belt, 0);



btid = kthread_create(barber,NULL,"barberthread");
if(btid){
wake_up_process(btid);
}


for (i = 0; i < num_cust; i++) {

tid[i] = kthread_create(customer, (void *)&Num[i], "customerthread");

if(tid[i]){

wake_up_process(tid[i]);
}

}


for (i = 0; i < num_cust; i++) {

kthread_stop(tid[i]);
}

Done=1;
up(&b_sleep);
kthread_stop(btid);
return 0;
}



int customer(void *number)  {

int num = *(int *)number;
num++;

printk("Customer %d is leaving for the barber shop\n",num);

msleep(5);


printk("Customer %d has reached the barber shop\n",num);

down(&wait_room);
printk("Customer %d is in the waiting room\n",num);
down(&b_chair);

up(&wait_room);
printk("Customer %d is waking the barber up\n",num);
up(&b_sleep);
down(&seat_belt);
up(&b_chair);


printk("Customer %d is leaving the barber shop\n",num);
return 0;
}


int barber(void *junk)  {

while(!Done)  {

printk("The Barber is sleeping\n");

down(&b_sleep);

if(!Done) {

printk("The Barber is cutting hair\n");

msleep(4);

printk("The Barber has finished cutting hair\n");

up(&seat_belt);
}

else{

printk("The Barber has completed his work for the day\n");

}
}
return 0;
}