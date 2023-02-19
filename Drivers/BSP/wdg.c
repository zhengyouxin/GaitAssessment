/* CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : wdg.c
  * @brief          : wdg bsp
  * @author			: ZYX
  * @time			: 2020.11
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 SZCHWS.
  * All rights reserved.
  ******************************************************************************/

#include "stm32l4xx_hal.h"
#include "wdg.h"

extern IWDG_HandleTypeDef hiwdg;

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
//void MX_IWDG_Init(void)
//{

//  /* USER CODE BEGIN IWDG_Init 0 */

//  /* USER CODE END IWDG_Init 0 */

//  /* USER CODE BEGIN IWDG_Init 1 */

//  /* USER CODE END IWDG_Init 1 */
//  hiwdg.Instance = IWDG;
//  hiwdg.Init.Prescaler = IWDG_PRESCALER_4;
//  hiwdg.Init.Window = 4095;
//  hiwdg.Init.Reload = 4095;
//  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
//  {
//    //Error_Handler();
//  }
//  /* USER CODE BEGIN IWDG_Init 2 */

//  /* USER CODE END IWDG_Init 2 */

//}

void IWDG_Feed(void)
{   
    HAL_IWDG_Refresh(&hiwdg); 	
}


