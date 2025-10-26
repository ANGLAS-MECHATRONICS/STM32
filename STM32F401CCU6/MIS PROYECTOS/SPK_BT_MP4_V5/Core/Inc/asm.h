/*
  * Author: VadRov
  * Copyright (C) 2022, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#ifndef INC_ASM_H_
#define INC_ASM_H_

#include "display.h"

void memset_word(void *, int, unsigned int);
void memset_halfword(void *, int, unsigned int);
void memset_byte(void *, int, unsigned int);

#endif /* INC_ASM_H_ */
