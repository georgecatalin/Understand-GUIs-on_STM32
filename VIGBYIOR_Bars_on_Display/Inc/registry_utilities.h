/*
 * macros.h
 *
 *  Created on: May 5, 2023
 *  Author: George Calin
 * 	Target Development Board: STM32 Nucleo F429ZI
 */

#ifndef REGISTRY_UTILITIES_H_
#define REGISTRY_UTILITIES_H_

#define RESET_BITS_REGISTRY(registry, start_bit, no_bits)     for(int i=(start_bit);i<(start_bit+no_bits);i++) \
																		{ \
																		  if(i>31) return; \
																		  (registry) &= ~(1UL<<i); \
																		}

#define SET_BITS_REGISTRY(registry, start_bit, no_bits)     for(int i=(start_bit);i<(start_bit+no_bits);i++) \
																		{ \
																		   if(i>31) return; \
																		  (registry) |= (1UL<<i); \
																		}

#endif /* REGISTRY_UTILITIES_H_ */
