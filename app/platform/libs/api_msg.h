#ifndef _API_MSG_H
#define _API_MSG_H


#define NO_MSG              0

void msg_queue_init(void);
void msg_queue_clear(void);
void msg_enqueue(u16 msg);
u16 msg_dequeue(void);
void msg_queue_detach(u16 msg, u8 flag);        //flag=0,消息池中剔除msg消息; flag=1,消息池只保留msg消息
#endif // _API_MSG_H
