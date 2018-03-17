#ifndef _GDT_H
#define _GDT_H

struct tss_t {
  uint32_t reserved_0;
  void *rsp0;
  void *rsp1;
  void *rsp2;
  uint64_t reserved_1;
  void *ist1;
  void *ist2;
  void *ist3;
  void *ist4;
  void *ist5;
  void *ist6;
  void *ist7;
  uint64_t reserved_2;
  uint16_t reserved_3;
  uint16_t iomap_base;
}__attribute__((packed));

void init_gdt();
void set_tss_rsp(void *rsp);

#endif
