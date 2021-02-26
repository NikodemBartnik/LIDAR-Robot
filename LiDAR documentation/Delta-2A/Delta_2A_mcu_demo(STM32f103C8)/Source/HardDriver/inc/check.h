#ifndef  _CHECK_H
#define  _CHECK_H
#include <stdint.h>

uint16_t CRC16(uint8_t *Start_Byte,uint16_t Num_Bytes);
uint16_t Calc_Pack_CRC16(uint8_t *Phead,uint8_t *Pdat,uint16_t Len);

uint16_t Calc_CheckSum(uint8_t *Start_Byte,uint16_t Num_Bytes);
uint32_t Calc_CheckSum_32(uint8_t *Start_Byte,uint16_t Num_Bytes);
uint16_t Calc_Pack_Checksum(uint8_t *Phead,uint8_t *Pdat,uint16_t Len);

#endif

