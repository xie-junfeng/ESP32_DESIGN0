/**
 ****************************************************************************************************
 * @file        udp.c
 * @version     V1.0
 * @date        2023-08-26
 * @brief       
 ****************************************************************************************************
 * @attention
 *
 *
 ****************************************************************************************************
 */

#include "lwip_demo.h"


/* 需要自己设置远程IP地址 */
#define IP_ADDR   "192.168.3.10"

#define LWIP_DEMO_RX_BUFSIZE         100                        /* 最大接收数据长度 */
#define LWIP_DEMO_PORT               8080                       /* 连接的本地端口号 */
#define LWIP_SEND_THREAD_PRIO       ( tskIDLE_PRIORITY + 3 )    /* 发送数据线程优先级 */
/* 接收数据缓冲区 */
uint8_t g_lwip_demo_recvbuf[LWIP_DEMO_RX_BUFSIZE]; 

/* 发送数据内容 */
uint8_t g_lwip_demo_sendbuf[] = "ALIENTEK DATA \r\n";
/* 数据发送标志位 */
uint8_t g_lwip_send_flag;
int g_sock = -1;
int g_lwip_connect_state = 0;
static void lwip_send_thread(void *arg);

/**
 * @brief       发送数据线程
 * @param       无
 * @retval      无
 */
void lwip_data_send(void *buffer)
{
    xTaskCreate(lwip_send_thread, "lwip_send_thread", 4096, buffer, LWIP_SEND_THREAD_PRIO, NULL);
}

/**
 * @brief       lwip_demo实验入口
 * @param       无
 * @retval      无
 */
uint8_t lwip_demo(uint8_t *temp, uint8_t *humi)
{
    struct sockaddr_in atk_client_addr;
    err_t err;
    int recv_data_len;
    char *tbuf;
    

    // 安全格式化：数值转字符串并拼接
    // uint8_t *buffer = malloc(100);
    // char *buffer = (char *)calloc(50, sizeof(char)); // 分配并初始化为0
    // if (!buffer) exit(1);
    // // 使用buffer（内容全为0）
    // free(buffer)

    char buffer[50]="--------开始上传温湿度-------\r\n"; // 静态缓冲区，需确保足够大
    // snprintf(buffer, sizeof(buffer), "温度:%d℃, 湿度:%d%%", *temp, *humi);
    
    // char* data=strcat((char*)temp, (char*)humi); 

    lwip_data_send(&buffer);                                           /* 创建发送数据线程 */
    
    // free(buffer); // 使用后必须释放
    // buffer = NULL; // 防止野指针

    while (1)
    {
sock_start:
        g_lwip_connect_state = 0;
        atk_client_addr.sin_family = AF_INET;                   /* 表示IPv4网络协议 */
        atk_client_addr.sin_port = htons(LWIP_DEMO_PORT);       /* 端口号 */
        atk_client_addr.sin_addr.s_addr = inet_addr(IP_ADDR);   /* 远程IP地址 */
        g_sock = socket(AF_INET, SOCK_STREAM, 0);               /* 可靠数据流交付服务既是TCP协议 */
        memset(&(atk_client_addr.sin_zero), 0, sizeof(atk_client_addr.sin_zero));
        
        tbuf = malloc(200);                                     /* 申请内存 */
        sprintf((char *)tbuf, "Port:%d", LWIP_DEMO_PORT);       /* 客户端端口号 */
        lcd_show_string(5, 170, 200, 16, 16, tbuf, MAGENTA);
        
        /* 连接远程IP地址 */
        err = connect(g_sock, (struct sockaddr *)&atk_client_addr, sizeof(struct sockaddr));

        if (err == -1)
        {
            lcd_show_string(5, 190, 200, 16, 16, "State:Disconnect", MAGENTA);
            g_sock = -1;
            closesocket(g_sock);
            free(tbuf);
            vTaskDelay(10);
            goto sock_start;
        }

        lcd_show_string(5, 190, 200, 16, 16, "State:Connection Successful", MAGENTA);
        g_lwip_connect_state = 1;

        while (1)
        {
            recv_data_len = recv(g_sock,g_lwip_demo_recvbuf,
                                 LWIP_DEMO_RX_BUFSIZE,0);
            if (recv_data_len <= 0 )
            {
                closesocket(g_sock);
                g_sock = -1;
                lcd_fill(5, 190, lcd_self.width,320, WHITE);
                lcd_show_string(5, 190, 200, 16, 16, "State:Disconnect", MAGENTA);
                free(tbuf);
                goto sock_start;
            }
            // printf("%d\r\n",*temp); 
            // printf("%d\r\n",*humi);
            // snprintf(buffer, sizeof(buffer), "temperature:%dC,humidity:%d%%     \r\n", *temp, *humi);
            snprintf(buffer, sizeof(buffer), "PWM:%dC  \r\n", *g_lwip_demo_recvbuf);
            printf("%s\n", buffer); // 输出结果：温度:25℃, 湿度:60%       
            printf("%s\r\n",g_lwip_demo_recvbuf);
            vTaskDelay(10);
        }
    }

}

/**
 * @brief       发送数据线程函数(数据回传功能用于确认接收到的数据是否正确
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void lwip_send_thread(void *pvParameters)
{
    uint8_t *buffer = (uint8_t *)pvParameters;
    
    err_t err;
    
    while (1)
    {
        while (1)
        {
            if(((g_lwip_send_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) && (g_lwip_connect_state == 1)) /* 有数据要发送 */
            {   
                // strcat(str1, str2); 
                // printf("%s\n", buffer); // 输出结果：温度:25℃, 湿度:60%  
                // printf("%d\n",sizeof(buffer)); // 输出结果：温度:25℃, 湿度:60%  
                err = write(g_sock, buffer, 30);
                
                if (err < 0)
                {
                    break;
                }
                // free(data);  // 必须释放
                g_lwip_send_flag &= ~LWIP_SEND_DATA;
            }
            
            vTaskDelay(10);
        }
        
        closesocket(g_sock);
    }
}
