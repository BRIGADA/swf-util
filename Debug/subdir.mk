################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ABCFile.cpp \
../DataReader.cpp \
../SWFFile.cpp \
../SWFTag.cpp \
../SWFTagFileAttributes.cpp \
../SWFTagMetadata.cpp \
../main.cpp 

OBJS += \
./ABCFile.o \
./DataReader.o \
./SWFFile.o \
./SWFTag.o \
./SWFTagFileAttributes.o \
./SWFTagMetadata.o \
./main.o 

CPP_DEPS += \
./ABCFile.d \
./DataReader.d \
./SWFFile.d \
./SWFTag.d \
./SWFTagFileAttributes.d \
./SWFTagMetadata.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


