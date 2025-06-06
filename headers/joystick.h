#include "hardware/adc.h"
#include "hardware/pwm.h"

#ifndef JOYSTICK_H
#define JOYSTICK_H

//Inicializa o ADC nos eixos do joystick
void joystick_adc_init(uint joyX,uint joyY){
    adc_init();
    adc_gpio_init(joyX);
    adc_gpio_init(joyY);  
}

//Inicializa o PWM do LED que sera controlado pelo Joystick
uint gpio_pwm_config(uint gpio, float divisor,uint wrap,uint dutycycle){
    gpio_set_function(gpio,GPIO_FUNC_PWM);//Habilitando pino como PWM
    uint slice = pwm_gpio_to_slice_num(gpio);//Obter canal PWM da GPIO
    
    //Configurar clock
    pwm_set_clkdiv(slice,divisor);//Define o valor do divisor de clock 
    pwm_set_wrap(slice,wrap);//Define Valor de Wrap
    pwm_set_gpio_level(gpio,dutycycle);//Define valor Duty Cycle
    
    return slice;
}
#endif //JOYSTICK_H