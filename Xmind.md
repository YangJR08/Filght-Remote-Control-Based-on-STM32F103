## 无线通讯模块
安信可公司有手册驱动电路等提供，需要对使用的芯片SI24R1进行驱动移植
使用芯片的SPI1外设，涉及引脚PB3(SPI1_SCK) PB4(SPI1_MISO) PB5(SPI1_MOSI) PA15(SPI_NSS) PB7 (SI_EN)-高电平有效配置时默认低电平
`READ_REG`、`WRITE_REG`修改驱动里这两个宏定义，因为在stm32库中会重复定义，修改为`SI24R1_WRITE_REG`、`SI24R1_READ_REG`。
剩下的宏定义不用修改，需要重写剩下的函数