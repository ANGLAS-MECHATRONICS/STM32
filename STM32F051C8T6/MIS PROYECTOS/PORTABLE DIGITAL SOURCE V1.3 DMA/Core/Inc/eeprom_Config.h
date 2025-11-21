#ifndef __EECONFIG_H
#define __EECONFIG_H
//al subir un programa al uC muestra lo siguiente
//Erasing internal memory sectors [0 61]
//es decir mi programa es grande y llena 62 paginas(0-61) de la flash
// y me deja libre 2 paginas 62 y 63;
//aveces falla en subir el programa, la solucion es colocar 62 o actualizar el firmware del stlink
#define   _EE_USE_FLASH_PAGE_OR_SECTOR              (63) //PAGINA 63
#define   _EE_USE_RAM_BYTE                          (1024)
#define   _EE_VOLTAGE                               FLASH_VOLTAGE_RANGE_3 //  use in some devices
#endif
