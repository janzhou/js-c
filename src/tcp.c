#ifdef _WIN32
#include <winsock2.h>
#endif

#ifdef __linux
#include <sys/socket.h>
#include <unistd.h>/*close*/
#endif

#include <malloc.h>
#include <string.h> 
#include <stdio.h>

#include "tcp.h"
#include "thread.h"

//======================================================================================
//======================receive and send=====================================================
//======================================================================================
int tcp_receive_wait(int sock_id,void *buf, int len) {
    return  recv(sock_id, (char *)buf, len, 0);
}

int tcp_send(int sock_id,void *buf ,int len) {
    return send(sock_id, (char *)buf, len, 0);
}


//======================================================================================
//======================client============================================================
//======================================================================================
typedef struct sock_list{
    int sock_id;
    void (* callback)(int,int,void *,int,void*);
    void * callbackdata;
    struct sock_list * next;

    thread_t tid;
} sock_list;

sock_list * sock_head = NULL;

thread_func_t receive_th(thread_arg_t arg) {
    sock_list * p_sock = (sock_list *)arg;
    char receiveBuf[SOCK_BUF_LEN];
    int len = 0;

    if(p_sock->callback == NULL) return 0;

    (* p_sock->callback)(p_sock->sock_id, TCP_CONNECT,receiveBuf, len, p_sock->callbackdata);

    while(p_sock->sock_id){
        if( (len = tcp_receive_wait(p_sock->sock_id, receiveBuf, SOCK_BUF_LEN)) <= 0 ){
            (* p_sock->callback) (p_sock->sock_id, TCP_DISCONNECT,receiveBuf, len, p_sock->callbackdata);
            break;
        }
        else if(len > 0) {
            receiveBuf[len]=0;
            (* p_sock->callback)(p_sock->sock_id, TCP_DATAREADY, receiveBuf, len, p_sock->callbackdata);
        }
    }

    return 0;
}

int tcp_connect(char * addr, int port, void (* callback)(int,int,void *,int,void*),void * callbackdata){
    struct sockaddr_in sin;

    sock_list * new_sock = (sock_list *)malloc(sizeof(sock_list));

#ifdef _WIN32
    struct  in_addr inAddr;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons((unsigned short)port);

    //创建socket初始化
    inAddr.S_un.S_addr = inet_addr(addr);
    memcpy(&(sin.sin_addr), &inAddr, sizeof(struct  in_addr));

    if((new_sock->sock_id= socket(AF_INET, SOCK_STREAM, 0)) ==-1){
        return -1;
    }

    //连接服务器
    if(!connect(new_sock->sock_id, (struct sockaddr *) &sin, sizeof(sin))){
        new_sock->callback = callback;
        new_sock->callbackdata = callbackdata;
        if(new_sock->sock_id > 2 && callback && create_thread(&new_sock->tid, receive_th, new_sock)){
            printf("tcp error create thread\n");
            closesocket(new_sock->sock_id);
            goto connect_error;
        }
        //		printf("tcp connect success %d\n", new_sock->sock_id);
        new_sock->next = sock_head;
        sock_head = new_sock;
        return new_sock->sock_id;
    }
    printf("tcp error connect\n");
#endif

#ifdef __linux
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    //创建socket初始化
    if(inet_pton(AF_INET, addr, &sin.sin_addr) != 1){
        return -1;
    }

    if((new_sock->sock_id= socket(AF_INET, SOCK_STREAM, 0)) ==-1){
        return -1;
    }

    //连接服务器
    if(!connect(new_sock->sock_id, (struct sockaddr *) &sin, sizeof(sin))){
        new_sock->callback = callback;
        new_sock->callbackdata = callbackdata;
        if(new_sock->sock_id > 2 && callback && create_thread(&new_sock->tid, receive_th, new_sock)){
            printf("tcp error create thread\n");
            close(new_sock->sock_id);
            goto connect_error;
        }
        //		printf("tcp connect success %d\n", new_sock->sock_id);
        new_sock->next = sock_head;
        sock_head = new_sock;
        return new_sock->sock_id;
    }
    printf("tcp error connect\n");
#endif



connect_error:
    free(new_sock);
    return -1;
}

int tcp_disconnect(int sock_id){	
    sock_list * temp;

#ifdef _WIN32
    if(closesocket(sock_id) == -1){
        return (-1);
    }
#endif

#ifdef __linux
    if(close(sock_id) == -1){
        return (-1);
    }
#endif

    if(sock_head->next==NULL){
        free(sock_head);
        sock_head=NULL;
    }
    else {
        for(temp=sock_head;temp!=NULL && temp->next!=NULL;temp=temp->next){
            if(temp->next->sock_id==sock_id){
                sock_list * tmp=temp->next;
                temp->next=temp->next->next;
                free(tmp);
            }
        }
    }
    return 0;
}

//======================================================================================
//======================server==========================================================
//======================================================================================
struct sv_callback{
    void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata);
    void *callbackdata;
};

struct accept_arg{
    int socketfd;
    struct sv_callback cb;

    thread_t thread;
};

struct server_arg{
    int clientfd;
    struct sv_callback cb;
    char ip[24];
    int port;

    thread_t thread;
};


static thread_func_t server_th(thread_arg_t arg) {
    struct server_arg * s_arg =(struct server_arg *) arg;
    if(s_arg->cb.callback){
        char receiveBuf[SOCK_BUF_LEN];
        int len;
        (*s_arg->cb.callback)(s_arg->ip, s_arg->port, s_arg->clientfd, TCP_CONNECT, NULL, 0,s_arg->cb.callbackdata);
        while( (len = tcp_receive_wait(s_arg->clientfd, receiveBuf, SOCK_BUF_LEN))){
            receiveBuf[len]=0;
            (*s_arg->cb.callback)(s_arg->ip, s_arg->port, s_arg->clientfd, TCP_DATAREADY, receiveBuf, len,s_arg->cb.callbackdata);
        }
        (*s_arg->cb.callback)(s_arg->ip, s_arg->port, s_arg->clientfd, TCP_DISCONNECT, NULL, 0,s_arg->cb.callbackdata);
    }

#ifdef _WIN32
    closesocket(s_arg->clientfd);
#endif

#ifdef __linux
    close(s_arg->clientfd);
#endif

    return 0;
}

thread_func_t server_accept_th(thread_arg_t arg){
    struct accept_arg * ac_arg = (struct accept_arg *)arg;
    struct sockaddr_in clientAdd;
    int  len = sizeof(struct sockaddr_in);
    int clientfd;
    while((clientfd = accept(ac_arg->socketfd, (struct sockaddr *)&clientAdd, (socklen_t *)&len)) > 0){
        struct server_arg * s_arg = (struct server_arg *)malloc(sizeof(struct server_arg));
        s_arg->clientfd = clientfd;
        s_arg->cb= ac_arg->cb;
        strcpy(s_arg->ip, inet_ntoa(clientAdd.sin_addr)); 
        s_arg->port = clientAdd.sin_port;

        if(create_thread(&s_arg->thread,server_th,s_arg)){
            free(s_arg);
        }
    }
    return 0;
}

int tcp_server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata)
{
    struct accept_arg * ac_arg = (struct accept_arg*)malloc(sizeof(struct accept_arg));

#ifdef _WIN32
    struct sockaddr_in saAddr;

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    ac_arg->cb.callback = callback;
    ac_arg->cb.callbackdata= callbackdata;

    ac_arg->socketfd = socket(AF_INET,SOCK_STREAM,0);
    if(ac_arg->socketfd == -1)
    {
        goto bind_error;
    }

#ifdef _WIN32
    memset(&saAddr,0,sizeof(saAddr));
    saAddr.sin_family = AF_INET;
    saAddr.sin_port = htons((unsigned short)port);
    saAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(saAddr.sin_zero),0,8);

    if(bind(ac_arg->socketfd,(struct sockaddr *)&saAddr,sizeof(saAddr))!=0){
        goto bind_error;
    }

    if(listen(ac_arg->socketfd,num )!=0){
        goto bind_error;
    }

    if(create_thread(&ac_arg->thread,server_accept_th,ac_arg)){
        goto bind_error;
    }

    return 0;
bind_error:
    free(ac_arg);
    return -1;
#endif

#ifdef __linux
    struct sockaddr_in sa ;
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(sa.sin_zero),8);

    if(bind(ac_arg->socketfd,(struct sockaddr *)&sa,sizeof(sa))!=0){
        goto bind_error;
    }

    if(listen(ac_arg->socketfd,num )!=0){
        goto bind_error;
    }

    if(create_thread(&ac_arg->thread,server_accept_th,ac_arg)){
        goto bind_error;
    }

    return 0;
bind_error:
    free(ac_arg);
    return -1;
#endif

}

