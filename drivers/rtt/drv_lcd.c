/*
 * Copyright (c) 2006-2026, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-11-07     RealThread   the first version
 */

#include <stddef.h>
#include <rthw.h>
#include <rtthread.h>

#include "drv_lcd.h"

#ifdef RT_USING_LCD

#define DBG_TAG "drv_lcd"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

struct drv_lcd_device
{
    struct rt_device parent;
    struct rt_device_graphic_info lcd_info;
};

static uint16_t *frame_buffer = RT_NULL;
struct drv_lcd_device _lcd = { 0 };
RGB color_write = { 0, 0xFF, 0xFF, 0xFF };
RGB color_black = { 0, 0x00, 0x00, 0x00 };
RGB color_red = { 0, 0xFF, 0x00, 0x00 };
RGB color_blue = { 0, 0x00, 0x00, 0xFF };
RGB color_green = { 0, 0x00, 0xFF, 0x00 };
RGB color_yellow = { 0, 0xFF, 0xFF, 0x00 };
RGB color_cyan = { 0, 0x00, 0xFF, 0xFF };
RGB color_magenta = { 0, 0xFF, 0x00, 0xFF };
RGB color_orange = { 0, 0xFF, 0xA5, 0x00 };
RGB color_purple = { 0, 0x80, 0x00, 0x80 };
RGB color_gray = { 0, 0x80, 0x80, 0x80 };
RGB color_lightgray = { 0, 0xD3, 0xD3, 0xD3 };
RGB color_darkgray = { 0, 0x40, 0x40, 0x40 };
RGB color_brown = { 0, 0xA5, 0x2A, 0x2A };
RGB color_pink = { 0, 0xFF, 0xC0, 0xCB };
RGB color_gold = { 0, 0xFF, 0xD7, 0x00 };
RGB color_silver = { 0, 0xC0, 0xC0, 0xC0 };

static void framebuffer_init(void);
static void lcd_layer_enable(uint32_t layer);
static void lcd_init(void);
void lcd_layer_init(uint32_t layer, uint32_t buf_addr, uint32_t width, uint32_t height);

rt_err_t drv_lcd_init(rt_device_t dev)
{
    return RT_EOK;
}

rt_err_t drv_lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

rt_err_t drv_lcd_close(rt_device_t dev)
{
    return RT_EOK;
}

rt_ssize_t drv_lcd_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    return RT_EOK;
}

rt_ssize_t drv_lcd_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    return RT_EOK;
}

rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args)
{
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *) args;

        RT_ASSERT(info != RT_NULL);
        info->pixel_format = _lcd.lcd_info.pixel_format;
        info->bits_per_pixel = _lcd.lcd_info.bits_per_pixel;
        info->width = _lcd.lcd_info.width;
        info->height = _lcd.lcd_info.height;
        info->framebuffer = _lcd.lcd_info.framebuffer;
    }
        break;
    case RTGRAPHIC_CTRL_RECT_UPDATE:
        tli_reload_config(TLI_REQUEST_RELOAD_EN);
        break;
    default:
        return RT_EOK;
    }

    return RT_EOK;
}

static int hw_drv_lcd_init(void)
{
    frame_buffer = rt_malloc(ACTIVE_WIDTH * ACTIVE_HEIGHT * PRE_PIXEL_BYTES);
    framebuffer_init();

    _lcd.parent.type = RT_Device_Class_Graphic;
    _lcd.parent.init = drv_lcd_init;
    _lcd.parent.open = drv_lcd_open;
    _lcd.parent.close = drv_lcd_close;
    _lcd.parent.read = drv_lcd_read;
    _lcd.parent.write = drv_lcd_write;
    _lcd.parent.control = drv_lcd_control;

    _lcd.lcd_info.framebuffer = (rt_uint8_t *) frame_buffer;
    _lcd.lcd_info.width = LCD_WIGHT;
    _lcd.lcd_info.height = LCD_HIGHT;
    _lcd.lcd_info.bits_per_pixel = PRE_PIXEL_BYTES * 8;
    _lcd.lcd_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565;

    rcu_periph_clock_enable(RCU_IPA);

    lcd_init();

    lcd_layer_init(0x0000, (uint32_t) frame_buffer, LCD_WIGHT, LCD_HIGHT);

    lcd_layer_enable(0x0000);
    tli_enable();

    rt_device_register(&_lcd.parent, "lcd", RT_DEVICE_FLAG_RDWR);

    return RT_EOK;
}
INIT_APP_EXPORT(hw_drv_lcd_init);

static void framebuffer_init(void)
{
    for (uint32_t y = 0; y < ACTIVE_HEIGHT; ++y)
    {
        for (uint32_t x = 0; x < ACTIVE_WIDTH; ++x)
        {
            frame_buffer[y * ACTIVE_WIDTH + x] = 0xFFFF;
        }
    }
}

static void lcd_init(void)
{
    tli_parameter_struct tli_init_struct;
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOH);
    rcu_periph_clock_enable(RCU_GPIOI);
    rcu_periph_clock_enable(RCU_GPIOG);
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure TLI pins AF */
    gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_5);
    gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOE, GPIO_AF_14, GPIO_PIN_4);

    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_2);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_3);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_8);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_9);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_10);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_11);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_12);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_13);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_14);
    gpio_af_set(GPIOH, GPIO_AF_14, GPIO_PIN_15);

    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_0);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_1);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_2);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_4);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_5);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_7);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_9);
    gpio_af_set(GPIOI, GPIO_AF_14, GPIO_PIN_10);

    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_6);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_7);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_10);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_11);
    gpio_af_set(GPIOG, GPIO_AF_14, GPIO_PIN_12);

    gpio_af_set(GPIOF, GPIO_AF_14, GPIO_PIN_10);

    /* configure HSYNC(PI10), VSYNC(PI9), PCLK(PG7) */
    /* configure LCD_R7(PG6), LCD_R6(PH12), LCD_R5(PH11), LCD_R4(PH10), LCD_R3(PH9),LCD_R2(PH8),
     LCD_R1(PH3), LCD_R0(PH2), LCD_G7(PI2), LCD_G6(PI1), LCD_G5(PI0), LCD_G4(PH15),
     LCD_G3(PH14), LCD_G2(PH13),LCD_G1(PE6), LCD_G0(PE5),LCD_B7(PI7), LCD_B6(PI6),
     LCD_B5(PI5), LCD_B4(PI4), LCD_B3(PG11),LCD_B2(PG10), LCD_B1(PG12), LCD_B0(PE4) */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);
    gpio_output_options_set(GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6);

    gpio_mode_set(GPIOH, GPIO_MODE_AF, GPIO_PUPD_NONE,
            GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13
                    | GPIO_PIN_14 | GPIO_PIN_15);
    gpio_output_options_set(GPIOH, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
            GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13
                    | GPIO_PIN_14 | GPIO_PIN_15);

    gpio_mode_set(GPIOI, GPIO_MODE_AF, GPIO_PUPD_NONE,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9
                    | GPIO_PIN_10);
    gpio_output_options_set(GPIOI, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
            GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9
                    | GPIO_PIN_10);

    gpio_mode_set(GPIOG, GPIO_MODE_AF, GPIO_PUPD_NONE,
            GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,
            GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* configure LCD_PWM_BackLight(PB15) */
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    gpio_bit_set(GPIOB, GPIO_PIN_15);

    /*TLI init */
    rcu_periph_clock_enable(RCU_TLI);

    /* configure PLLSAI prescalers for LCD */
    if (ERROR == rcu_pllsai_r_config(240, 2))
    {
        while (1)
            ;
    }
    rcu_tli_clock_div_config(RCU_PLLSAIR_DIV8);
    rcu_osci_on(RCU_PLLSAI_CK);
    if (ERROR == rcu_osci_stab_wait(RCU_PLLSAI_CK))
    {
        while (1)
            ;
    }

    /* TLI initialization */
    /* initialize the horizontal synchronization polarity as active low */
    tli_init_struct.signalpolarity_hs = TLI_HSYN_ACTLIVE_LOW;
    /* initialize the vertical synchronization polarity as active low */
    tli_init_struct.signalpolarity_vs = TLI_VSYN_ACTLIVE_LOW;
    /* initialize the data enable polarity as active low */
    tli_init_struct.signalpolarity_de = TLI_DE_ACTLIVE_LOW;
    /* initialize the pixel clock polarity as input pixel clock */
    tli_init_struct.signalpolarity_pixelck = TLI_PIXEL_CLOCK_TLI;

    /* LCD display timing configuration */
    tli_init_struct.synpsz_hpsz = HORIZONTAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.synpsz_vpsz = VERTICAL_SYNCHRONOUS_PULSE - 1;
    tli_init_struct.backpsz_hbpsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1;
    tli_init_struct.backpsz_vbpsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1;
    tli_init_struct.activesz_hasz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH - 1;
    tli_init_struct.activesz_vasz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT - 1;
    tli_init_struct.totalsz_htsz = HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH + ACTIVE_WIDTH
            + HORIZONTAL_FRONT_PORCH - 1;
    tli_init_struct.totalsz_vtsz = VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH + ACTIVE_HEIGHT
            + VERTICAL_FRONT_PORCH - 1;

    /* LCD background color configure*/
    tli_init_struct.backcolor_red = 0xFF;
    tli_init_struct.backcolor_green = 0xFF;
    tli_init_struct.backcolor_blue = 0xFF;
    tli_init(&tli_init_struct);
}

/*!
 \brief      initialize TLI layer0 or layer1
 \param[in]  layer: LCD layer
 \arg        LCD_LAYER_BACKGROUND
 \arg        LCD_LAYER_FOREGROUND
 \param[in]  width: width of the window
 \param[in]  height: height of the window
 \param[out] none
 \retval     none
 */
void lcd_layer_init(uint32_t layer, uint32_t buf_addr, uint32_t width, uint32_t height)
{
    tli_layer_parameter_struct tli_layer_init_struct;
    if (LCD_LAYER_BACKGROUND == layer)
    {
        /* TLI layer0 configuration */
        tli_layer_init_struct.layer_window_leftpos = (HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_rightpos =
                (width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_window_toppos = (VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_bottompos = (height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
        tli_layer_init_struct.layer_sa = 0xFF;
        tli_layer_init_struct.layer_default_blue = 0;
        tli_layer_init_struct.layer_default_green = 0;
        tli_layer_init_struct.layer_default_red = 0;
        tli_layer_init_struct.layer_default_alpha = 0;
        tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
        tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
        tli_layer_init_struct.layer_frame_bufaddr = buf_addr;
        tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3);
        tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);
        tli_layer_init_struct.layer_frame_total_line_number = height;
        tli_layer_init(LAYER0, &tli_layer_init_struct);
    }
    else if (LCD_LAYER_FOREGROUND == layer)
    {
        /* TLI layer1 configuration */
        tli_layer_init_struct.layer_window_leftpos = (HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_rightpos =
                (width + HORIZONTAL_SYNCHRONOUS_PULSE + HORIZONTAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_window_toppos = (VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH);
        tli_layer_init_struct.layer_window_bottompos = (height + VERTICAL_SYNCHRONOUS_PULSE + VERTICAL_BACK_PORCH - 1);
        tli_layer_init_struct.layer_ppf = LAYER_PPF_RGB565;
        tli_layer_init_struct.layer_sa = 0xFF;
        tli_layer_init_struct.layer_default_blue = 0;
        tli_layer_init_struct.layer_default_green = 0;
        tli_layer_init_struct.layer_default_red = 0;
        tli_layer_init_struct.layer_default_alpha = 0;
        tli_layer_init_struct.layer_acf1 = LAYER_ACF1_PASA;
        tli_layer_init_struct.layer_acf2 = LAYER_ACF2_PASA;
        tli_layer_init_struct.layer_frame_bufaddr = buf_addr;
        tli_layer_init_struct.layer_frame_line_length = ((width * 2) + 3);
        tli_layer_init_struct.layer_frame_buf_stride_offset = (width * 2);
        tli_layer_init_struct.layer_frame_total_line_number = height;
        tli_layer_init(LAYER1, &tli_layer_init_struct);
    }
}

/*!
 \brief      enable the LCD layer0 or layer1
 \param[in]  layer: LCD layer
 \arg        LCD_LAYER_BACKGROUND
 \arg        LCD_LAYER_FOREGROUND
 \param[out] none
 \retval     none
 */
static void lcd_layer_enable(uint32_t layer)
{
    if (LCD_LAYER_BACKGROUND == layer)
    {
        tli_layer_enable(LAYER0);
    }
    else if (LCD_LAYER_FOREGROUND == layer)
    {
        tli_layer_enable(LAYER1);
    }
    tli_reload_config(TLI_REQUEST_RELOAD_EN);
}

void lcd_draw_picture(uint32_t picture_addr, uint32_t picture_width, uint32_t picture_height, uint32_t picture_x,
        uint32_t picture_y)
{
    ipa_destination_parameter_struct ipa_destination_init_struct;
    ipa_foreground_parameter_struct ipa_fg_init_struct;

    uint16_t *_address = &frame_buffer[picture_y * ACTIVE_WIDTH + picture_x];
    uint32_t line_offset = ACTIVE_WIDTH - picture_width;

    rcu_periph_clock_enable(RCU_IPA);

    ipa_deinit();
    /* configure IPA pixel format convert mode */
    ipa_pixel_format_convert_mode_set(IPA_FGTODE);
    /* configure destination pixel format */
    ipa_destination_init_struct.destination_pf = IPA_DPF_RGB565;
    /* configure destination memory base address */
    ipa_destination_init_struct.destination_memaddr = (uint32_t) _address;
    /* configure destination pre-defined alpha value RGB */
    ipa_destination_init_struct.destination_pregreen = 0;
    ipa_destination_init_struct.destination_preblue = 0;
    ipa_destination_init_struct.destination_prered = 0;
    ipa_destination_init_struct.destination_prealpha = 0;
    /* configure destination line offset */
    ipa_destination_init_struct.destination_lineoff = line_offset;
    /* configure height of the image to be processed */
    ipa_destination_init_struct.image_height = picture_height;
    /* configure width of the image to be processed */
    ipa_destination_init_struct.image_width = picture_width;
    /* IPA destination initialization */
    ipa_destination_init(&ipa_destination_init_struct);

    /* configure IPA foreground */
    ipa_fg_init_struct.foreground_memaddr = picture_addr;
    ipa_fg_init_struct.foreground_pf = FOREGROUND_PPF_RGB565;
    ipa_fg_init_struct.foreground_alpha_algorithm = IPA_FG_ALPHA_MODE_0;
    ipa_fg_init_struct.foreground_prealpha = 0x0;
    ipa_fg_init_struct.foreground_lineoff = 0x0;
    ipa_fg_init_struct.foreground_preblue = 0x0;
    ipa_fg_init_struct.foreground_pregreen = 0x0;
    ipa_fg_init_struct.foreground_prered = 0x0;
    /* foreground initialization */
    ipa_foreground_init(&ipa_fg_init_struct);

    /* start transfer */
    ipa_transfer_enable();
    while (RESET == ipa_interrupt_flag_get(IPA_INT_FLAG_FTF))
        ;
}

void lcd_draw_point(uint32_t x, uint32_t y, RGB * color)
{
    if (x >= ACTIVE_WIDTH || y >= ACTIVE_HEIGHT)
        return;

    frame_buffer[y * ACTIVE_WIDTH + x] = color;
}
#endif /* RT_USING_LCD */

