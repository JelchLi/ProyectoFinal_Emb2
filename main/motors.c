#include "motors.h"

#define in1 16
#define in2 17

#define in3 18
#define in4 19

static const char* TAG = "Main";

esp_err_t set_motors(void)
{
    //se configura el gpio16 como salida para el in1 del motor A
    gpio_reset_pin(in1);
    gpio_set_direction(in1, GPIO_MODE_OUTPUT);

    //se configura el gpio17 como salida para el in2 del motor A
    gpio_reset_pin(in2);
    gpio_set_direction(in2, GPIO_MODE_OUTPUT);

    //se configura el gpio18 como salida para el in3 del motor B
    gpio_reset_pin(in3);
    gpio_set_direction(in3, GPIO_MODE_OUTPUT);

    //se configura el gpio19 como salida para el in4 del motor B
    gpio_reset_pin(in4);
    gpio_set_direction(in4, GPIO_MODE_OUTPUT);

    return ESP_OK;
}

esp_err_t motorA_adelante(void)
{
    //voltaje alto en in1
    gpio_set_level(in1, 1);
    gpio_set_level(in2, 0);

    ESP_LOGW(TAG, "motor A adelante");

    return ESP_OK;
}

esp_err_t motorA_atras(void)
{
    //voltaje alto en in2
    gpio_set_level(in1, 0);
    gpio_set_level(in2, 1);

    ESP_LOGW(TAG, "motor A atras");

    return ESP_OK;
}

esp_err_t motorB_adelante(void)
{
    //voltaje alto en in3
    gpio_set_level(in3, 1);
    gpio_set_level(in4, 0);

    ESP_LOGW(TAG, "motor B adelante");

    return ESP_OK;
}

esp_err_t motorB_atras(void)
{
    //voltaje alto en in4
    gpio_set_level(in3, 0);
    gpio_set_level(in4, 1);

    ESP_LOGW(TAG, "motor B atras");

    return ESP_OK;
}

esp_err_t reset_motors(void)
{
    //reinicio de los motores
    gpio_set_level(in1, 0);
    gpio_set_level(in2, 0);
    gpio_set_level(in3, 0);
    gpio_set_level(in4, 0);

    ESP_LOGE(TAG, "motores reiniciados");

    return ESP_OK;
}