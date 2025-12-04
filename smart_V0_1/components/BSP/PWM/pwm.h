/**
 ****************************************************************************************************
 * @file        pwm.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       PWM驱动代码
 * @license     
 ****************************************************************************************************
 * @attention
 *
 * 
 ****************************************************************************************************
 */

#ifndef __PWM_H_
#define __PWM_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "driver/gpio.h"


/* 引脚以及重要参数定义 */
#define LEDC_PWM_TIMER          LEDC_TIMER_1        /* 使用定时器0 */
#define LEDC_PWM_CH0_GPIO       GPIO_NUM_1          /* LED控制器通道对应GPIO */
#define LEDC_PWM_CH0_CHANNEL    LEDC_CHANNEL_0     /* LED控制器通道号 */
#define LEDC_PWM_CH1_CHANNEL    LEDC_CHANNEL_1      /* LED控制器通道号 */
#define LEDC_PWM_CH2_CHANNEL    LEDC_CHANNEL_2      /* LED控制器通道号 */
#define LEDC_PWM_CH3_CHANNEL    LEDC_CHANNEL_3     /* LED控制器通道号 */

/* 函数声明 */
void pwm_init(uint8_t resolution, uint16_t freq);   /* 初始化PWM */
void pwm_set_duty(uint16_t duty);                   /* PWM占空比设置 */

#endif
