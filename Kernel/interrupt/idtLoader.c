#include <stdint.h>
#include "idtLoader.h"
#include "defs.h"
#include "interrupts.h"

#pragma pack(push) /* Push de la alineación actual */
#pragma pack(1)    /* Alinear las siguiente estructuras a 1 byte */

/* Descriptor de interrupcion */
typedef struct
{
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop) /* Reestablece la alinceación actual */

DESCR_INT *idt = (DESCR_INT *)0; // IDT de 255 entradas

static void setup_IDT_entry(int index, uint64_t offset);

void load_idt()
{
  // Clear Interruptions
  _cli();

  // Load IDT
  setup_IDT_entry(0x21, (uint64_t)&interrupt_keyboardHandler);
  setup_IDT_entry(0x20, (uint64_t)&interrupt_timerHandler);

  // Syscall
  setup_IDT_entry(0x80, (uint64_t)&interrupt_systemCall);

  // Exceptions
  setup_IDT_entry(0x00, (uint64_t)&exception_zeroDiv);
  setup_IDT_entry(0x06, (uint64_t)&exception_invalidOp);

  // Load IDTR
  picMasterMask(0xFC);
  picSlaveMask(0xFF);

  // Start Interruptions
  _sti();
}

static void setup_IDT_entry(int index, uint64_t offset)
{
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t)0;
}