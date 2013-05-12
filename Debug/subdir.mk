################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../ZEcc.o \
../ZMP.o \
../main.o 

C_SRCS += \
../ZEcc.c \
../ZMP.c \
../main.c \
../mainECC.c 

OBJS += \
./ZEcc.o \
./ZMP.o \
./main.o \
./mainECC.o 

C_DEPS += \
./ZEcc.d \
./ZMP.d \
./main.d \
./mainECC.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


