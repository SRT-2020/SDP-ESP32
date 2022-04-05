idf_component_register(SRCS "motor_control.c"
                    INCLUDE_DIRS "include"
                    REQUIRES analogWrite pin_control constants delay)

