#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose
   
   Implemented by: Gemini (AI Assistant)
   Protocol: Go-Back-N (GBN)
**********************************************************************/

#define BIDIRECTIONAL 0    /* change to 1 if you're doing extra credit */
                           /* and write a routine called B_output */

/* ******************************************************************
 GBN 协议参数配置
*********************************************************************/
#define WINDOW_SIZE 8      /* 窗口大小 N */
#define MAX_BUFFER 50      /* 发送方缓冲区大小 */
#define RTT 15.0           /* 设定的超时时间 */

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
  char data[20];
  };

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
   int seqnum;
   int acknum;
   int checksum;
   char payload[20];
    };

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* * 全局变量定义 
 * A和B的变量被封装在结构体中，模拟它们无法共享内存
 */

// A 端 (发送方) 状态
struct sender_state {
    int base;              /* 发送窗口基序号 */
    int next_seq_num;      /* 下一个序列号 */
    struct pkt packet_buffer[MAX_BUFFER]; /* 数据包缓冲区 */
} A;

// B 端 (接收方) 状态
struct receiver_state {
    int expected_seq_num;  /* 期望收到的序列号 */
    struct pkt last_ack_pkt; /* 上一次发送的 ACK 包 (用于乱序/丢包时重传) */
} B;

/* * 计算校验和
 * seqnum + acknum + payload ASCII码累加
 */
int calc_checksum(struct pkt *p) {
    int i;
    int checksum = 0;
    checksum += p->seqnum;
    checksum += p->acknum;
    for (i = 0; i < 20; i++) {
        checksum += (unsigned char)p->payload[i];
    }
    return checksum;
}

/* * A 端初始化
 */
A_init()
{
    A.base = 1;
    A.next_seq_num = 1;
    // 清空缓冲区
    memset(A.packet_buffer, 0, sizeof(A.packet_buffer));
    printf("A_init: GBN Sender initialized. Window size = %d\n", WINDOW_SIZE);
}

/* * 上层有数据要发送时调用
 * 检查缓冲区 -> 检查窗口 -> 发送或仅缓存
 */
A_output(message)
  struct msg message;
{
    // 检查物理缓冲区是否已满
    if (A.next_seq_num >= A.base + MAX_BUFFER) {
        printf("A_output: Buffer full (Total %d). Dropping message.\n", MAX_BUFFER);
        return; 
    }

    // 组装数据包并存入缓冲区
    struct pkt *packet = &A.packet_buffer[A.next_seq_num % MAX_BUFFER];
    packet->seqnum = A.next_seq_num;
    packet->acknum = 0;
    strncpy(packet->payload, message.data, 20);
    packet->checksum = calc_checksum(packet);

    // 检查是否在发送窗口内 (next_seq_num < base + N)
    if (A.next_seq_num < A.base + WINDOW_SIZE) {
        printf("A_output: Sending packet seq=%d\n", A.next_seq_num);
        tolayer3(0, *packet); // 0 代表 A 端

        // 如果是窗口内的第一个包，启动定时器
        if (A.base == A.next_seq_num) {
            starttimer(0, RTT);
            printf("A_output: Timer started.\n");
        }
    } else {
        printf("A_output: Window full. Packet seq=%d buffered.\n", A.next_seq_num);
    }

    A.next_seq_num++;
}

/* * A 端收到 B 端发来的包
 * 校验 -> 累积确认 -> 重启定时器 -> 发送缓存中新进入窗口的包
 */
A_input(packet)
  struct pkt packet;
{
    // 校验和检查
    if (packet.checksum != calc_checksum(&packet)) {
        printf("A_input: Packet corrupted. Drop.\n");
        return;
    }

    // 处理 ACK ，如果收到 ACK n，说明 n 及其之前的都收到了，base 更新为 n+1
    if (packet.acknum >= A.base) {
        printf("A_input: Got ACK %d. Sliding window.\n", packet.acknum);
        
        int old_base = A.base;
        A.base = packet.acknum + 1;

        stoptimer(0);

        // 如果还有未确认的包，重启定时器为新的 base 计时
        if (A.base < A.next_seq_num) {
            starttimer(0, RTT);
            printf("A_input: Timer restarted for base %d.\n", A.base);
        }

        // 发送之前缓存的、现在进入窗口的包
        // 新窗口范围: [base, base + WINDOW_SIZE)
        // 只需要检查 [old_base + WINDOW_SIZE, base + WINDOW_SIZE) 这部分新露出来的槽位
        int i;
        int check_start = old_base + WINDOW_SIZE;
        int check_end = A.base + WINDOW_SIZE;

        for (i = check_start; i < check_end; i++) {
            if (i < A.next_seq_num) {
                printf("A_input: Sending buffered packet seq=%d\n", i);
                tolayer3(0, A.packet_buffer[i % MAX_BUFFER]);
            }
        }
    } else {
        printf("A_input: Duplicate/Old ACK %d. Ignore.\n", packet.acknum);
    }
}

/* * 定时器超时
 * 重传当前窗口内的所有已发送但未确认的包
 */
A_timerinterrupt()
{
    printf("A_timerinterrupt: Timeout! Resending window starting at %d.\n", A.base);

    // 重新启动定时器
    starttimer(0, RTT);

    // 重传 [base, next_seq_num) 范围内且在窗口内的包
    int i;
    for (i = A.base; i < A.next_seq_num && i < A.base + WINDOW_SIZE; i++) {
        printf("A_timerinterrupt: Resending packet seq=%d\n", i);
        tolayer3(0, A.packet_buffer[i % MAX_BUFFER]);
    }
}

B_output(message)  /* need be completed only for extra credit */
  struct msg message;
{

}

/* * B 端收到 A 端发来的包
 * 校验 -> 顺序检查 -> 交付数据 & 发送 ACK / 乱序重发旧 ACK
 */
B_input(packet)
  struct pkt packet;
{
    // 校验和检查
    if (packet.checksum != calc_checksum(&packet)) {
        printf("B_input: Packet corrupted. Resending ACK %d.\n", B.last_ack_pkt.acknum);
        // 出错时，重传上一个正确的 ACK
        tolayer3(1, B.last_ack_pkt); 
        return;
    }

    // 检查序列号是否是期望的
    if (packet.seqnum == B.expected_seq_num) {
        printf("B_input: Packet %d received correctly. Delivering to layer5.\n", packet.seqnum);

        // 交付数据给上层
        struct msg message;
        strncpy(message.data, packet.payload, 20);
        tolayer5(1, message); // 1 代表 B 端交付

        // 构造并发送 ACK
        struct pkt ack_pkt;
        ack_pkt.acknum = B.expected_seq_num;
        ack_pkt.seqnum = 0;
        memset(ack_pkt.payload, 0, 20); 
        ack_pkt.checksum = calc_checksum(&ack_pkt);
        
        // 发送 ACK
        tolayer3(1, ack_pkt);
        printf("B_input: Sent ACK %d.\n", ack_pkt.acknum);
        
        // 更新状态
        B.last_ack_pkt = ack_pkt;
        B.expected_seq_num++;     // 期望下一个
        
    } else {
        // 丢弃乱序包，并重发对上一个有序包的 ACK
        printf("B_input: Unexpected packet seq=%d (expected %d). Resending ACK %d.\n", 
               packet.seqnum, B.expected_seq_num, B.last_ack_pkt.acknum);
        tolayer3(1, B.last_ack_pkt);
    }
}

/* called when B's timer goes off */
B_timerinterrupt()
{

}

/* * B 端初始化
 */
B_init()
{
    B.expected_seq_num = 1;  // B 期望收到的第一个包，需与 A_init 中的 base 一致
    
    // 初始化一个默认的 ACK 包，假设序列号 0 是初始的“前一个”包
    B.last_ack_pkt.seqnum = 0; 
    B.last_ack_pkt.acknum = 0;
    memset(B.last_ack_pkt.payload, 0, 20);
    B.last_ack_pkt.checksum = calc_checksum(&B.last_ack_pkt);
    
    printf("B_init: Receiver initialized. Waiting for seq %d\n", B.expected_seq_num);
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
  The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

  (Original Emulator Code provided in assignment)
******************************************************************/

struct event {
   float evtime;           /* event time */
   int evtype;             /* event type code */
   int eventity;           /* entity where event occurs */
   struct pkt *pktptr;     /* ptr to packet (if any) assoc w/ this event */
   struct event *prev;
   struct event *next;
 };
struct event *evlist = NULL;   /* the event list */

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1

int TRACE = 1;             /* for my debugging */
int nsim = 0;              /* number of messages from 5 to 4 so far */ 
int nsimmax = 0;           /* number of msgs to generate, then stop */
float time = 0.000;
float lossprob;            /* probability that a packet is dropped  */
float corruptprob;         /* probability that one bit is packet is flipped */
float lambda;              /* arrival rate of messages from layer 5 */   
int   ntolayer3;           /* number sent into layer 3 */
int   nlost;               /* number lost in media */
int ncorrupt;              /* number corrupted by media*/

main()
{
   struct event *eventptr;
   struct msg  msg2give;
   struct pkt  pkt2give;
   
   int i,j;
   char c; 
  
   init();
   A_init();
   B_init();
   
   while (1) {
        eventptr = evlist;            /* get next event to simulate */
        if (eventptr==NULL)
           goto terminate;
        evlist = evlist->next;        /* remove this event from event list */
        if (evlist!=NULL)
           evlist->prev=NULL;
        if (TRACE>=2) {
           printf("\nEVENT time: %f,",eventptr->evtime);
           printf("  type: %d",eventptr->evtype);
           if (eventptr->evtype==0)
         printf(", timerinterrupt  ");
             else if (eventptr->evtype==1)
               printf(", fromlayer5 ");
             else
       printf(", fromlayer3 ");
           printf(" entity: %d\n",eventptr->eventity);
           }
        time = eventptr->evtime;        /* update time to next event time */
        if (nsim==nsimmax)
    break;                        /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5 ) {
            generate_next_arrival();   /* set up future arrival */
            /* fill in msg to give with string of same letter */    
            j = nsim % 26; 
            for (i=0; i<20; i++)  
               msg2give.data[i] = 97 + j;
            if (TRACE>2) {
               printf("          MAINLOOP: data given to student: ");
                 for (i=0; i<20; i++) 
                  printf("%c", msg2give.data[i]);
               printf("\n");
       }
            nsim++;
            if (eventptr->eventity == A) 
               A_output(msg2give);  
             else
               B_output(msg2give);  
            }
          else if (eventptr->evtype ==  FROM_LAYER3) {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i=0; i<20; i++)  
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
      if (eventptr->eventity ==A)      /* deliver packet by calling */
           A_input(pkt2give);            /* appropriate entity */
            else
           B_input(pkt2give);
      free(eventptr->pktptr);          /* free the memory for packet */
            }
          else if (eventptr->evtype ==  TIMER_INTERRUPT) {
            if (eventptr->eventity == A) 
         A_timerinterrupt();
             else
         B_timerinterrupt();
             }
          else  {
       printf("INTERNAL PANIC: unknown event type \n");
             }
        free(eventptr);
        }

terminate:
   printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n",time,nsim);
}

init()                         /* initialize the simulator */
{
  int i;
  float sum, avg;
  float jimsrand();
  
  
   printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
   printf("Enter the number of messages to simulate: ");
   scanf("%d",&nsimmax);
   printf("Enter  packet loss probability [enter 0.0 for no loss]:");
   scanf("%f",&lossprob);
   printf("Enter packet corruption probability [0.0 for no corruption]:");
   scanf("%f",&corruptprob);
   printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
   scanf("%f",&lambda);
   printf("Enter TRACE:");
   scanf("%d",&TRACE);

   srand(9999);              /* init random number generator */
   sum = 0.0;                /* test random number generator for students */
   for (i=0; i<1000; i++)
      sum=sum+jimsrand();    /* jimsrand() should be uniform in [0,1] */
   avg = sum/1000.0;
   if (avg < 0.25 || avg > 0.75) {
    printf("It is likely that random number generation on your machine\n" ); 
    printf("is different from what this emulator expects.  Please take\n");
    printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
    exit(0);
    }

   ntolayer3 = 0;
   nlost = 0;
   ncorrupt = 0;

   time=0.0;                    /* initialize time to 0.0 */
   generate_next_arrival();     /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand() 
{
  double mmm = 2147483647;   /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
  float x;                   /* individual students may need to change mmm */ 
  x = rand()/mmm;            /* x should be uniform in [0,1] */
  return(x);
}  

/********************* EVENT HANDLINE ROUTINES *******/
/* The next set of routines handle the event list   */
/*****************************************************/
 
generate_next_arrival()
{
   double x,log(),ceil();
   struct event *evptr;
   float ttime;
   int tempint;

   if (TRACE>2)
       printf("           GENERATE NEXT ARRIVAL: creating new arrival\n");
 
   x = lambda*jimsrand()*2;  /* x is uniform on [0,2*lambda] */
                             /* having mean of lambda        */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + x;
   evptr->evtype =  FROM_LAYER5;
   if (BIDIRECTIONAL && (jimsrand()>0.5) )
      evptr->eventity = B;
    else
      evptr->eventity = A;
   insertevent(evptr);
} 


insertevent(p)
   struct event *p;
{
   struct event *q,*qold;

   if (TRACE>2) {
      printf("            INSERTEVENT: time is %lf\n",time);
      printf("            INSERTEVENT: future time will be %lf\n",p->evtime); 
      }
   q = evlist;     /* q points to header of list in which p struct inserted */
   if (q==NULL) {   /* list is empty */
        evlist=p;
        p->next=NULL;
        p->prev=NULL;
        }
     else {
        for (qold = q; q !=NULL && p->evtime > q->evtime; q=q->next)
              qold=q; 
        if (q==NULL) {   /* end of list */
             qold->next = p;
             p->prev = qold;
             p->next = NULL;
             }
           else if (q==evlist) { /* front of list */
             p->next=evlist;
             p->prev=NULL;
             p->next->prev=p;
             evlist = p;
             }
           else {     /* middle of list */
             p->next=q;
             p->prev=q->prev;
             q->prev->next=p;
             q->prev=p;
             }
         }
}

printevlist()
{
  struct event *q;
  int i;
  printf("--------------\nEvent List Follows:\n");
  for(q = evlist; q!=NULL; q=q->next) {
    printf("Event time: %f, type: %d entity: %d\n",q->evtime,q->evtype,q->eventity);
    }
  printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
stoptimer(AorB)
int AorB;  /* A or B is trying to stop timer */
{
 struct event *q,*qold;

 if (TRACE>2)
    printf("          STOP TIMER: stopping timer at %f\n",time);
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
       /* remove this event */
       if (q->next==NULL && q->prev==NULL)
             evlist=NULL;         /* remove first and only event on list */
          else if (q->next==NULL) /* end of list - there is one in front */
             q->prev->next = NULL;
          else if (q==evlist) { /* front of list - there must be event after */
             q->next->prev=NULL;
             evlist = q->next;
             }
           else {     /* middle of list */
             q->next->prev = q->prev;
             q->prev->next =  q->next;
             }
       free(q);
       return;
     }
  printf("Warning: unable to cancel your timer. It wasn't running.\n");
}


starttimer(AorB,increment)
int AorB;  /* A or B is trying to stop timer */
float increment;
{

 struct event *q;
 struct event *evptr;

 if (TRACE>2)
    printf("          START TIMER: starting timer at %f\n",time);
 /* be nice: check to see if timer is already started, if so, then  warn */
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
   for (q=evlist; q!=NULL ; q = q->next)  
    if ( (q->evtype==TIMER_INTERRUPT  && q->eventity==AorB) ) { 
      printf("Warning: attempt to start a timer that is already started\n");
      return;
      }
 
/* create future event for when timer goes off */
   evptr = (struct event *)malloc(sizeof(struct event));
   evptr->evtime =  time + increment;
   evptr->evtype =  TIMER_INTERRUPT;
   evptr->eventity = AorB;
   insertevent(evptr);
} 


/************************** TOLAYER3 ***************/
tolayer3(AorB,packet)
int AorB;  /* A or B is trying to stop timer */
struct pkt packet;
{
 struct pkt *mypktptr;
 struct event *evptr,*q;
 float lastime, x, jimsrand();
 int i;


 ntolayer3++;

 /* simulate losses: */
 if (jimsrand() < lossprob)  {
      nlost++;
      if (TRACE>0)    
  printf("          TOLAYER3: packet being lost\n");
      return;
    }  

/* make a copy of the packet student just gave me since he/she may decide */
/* to do something with the packet after we return back to him/her */ 
 mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
 mypktptr->seqnum = packet.seqnum;
 mypktptr->acknum = packet.acknum;
 mypktptr->checksum = packet.checksum;
 for (i=0; i<20; i++)
    mypktptr->payload[i] = packet.payload[i];
 if (TRACE>2)  {
   printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
    mypktptr->acknum,  mypktptr->checksum);
    for (i=0; i<20; i++)
        printf("%c",mypktptr->payload[i]);
    printf("\n");
   }

/* create future event for arrival of packet at the other side */
  evptr = (struct event *)malloc(sizeof(struct event));
  evptr->evtype =  FROM_LAYER3;   /* packet will pop out from layer3 */
  evptr->eventity = (AorB+1) % 2; /* event occurs at other entity */
  evptr->pktptr = mypktptr;       /* save ptr to my copy of packet */
/* finally, compute the arrival time of packet at the other end.
   medium can not reorder, so make sure packet arrives between 1 and 10
   time units after the latest arrival time of packets
   currently in the medium on their way to the destination */
 lastime = time;
/* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
 for (q=evlist; q!=NULL ; q = q->next) 
    if ( (q->evtype==FROM_LAYER3  && q->eventity==evptr->eventity) ) 
      lastime = q->evtime;
 evptr->evtime =  lastime + 1 + 9*jimsrand();
 


 /* simulate corruption: */
 if (jimsrand() < corruptprob)  {
    ncorrupt++;
    if ( (x = jimsrand()) < .75)
       mypktptr->payload[0]='Z';   /* corrupt payload */
      else if (x < .875)
       mypktptr->seqnum = 999999;
      else
       mypktptr->acknum = 999999;
    if (TRACE>0)    
  printf("          TOLAYER3: packet being corrupted\n");
    }  

  if (TRACE>2)  
     printf("          TOLAYER3: scheduling arrival on other side\n");
  insertevent(evptr);
} 

tolayer5(AorB,datasent)
  int AorB;
  char datasent[20];
{
  int i;  
  if (TRACE>2) {
     printf("          TOLAYER5: data received: ");
     for (i=0; i<20; i++)  
        printf("%c",datasent[i]);
     printf("\n");
   }
  
}