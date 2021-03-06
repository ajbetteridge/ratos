#include "idt.h"
#include "isr.h"

IDTDescr_t idtentries[256];

void irq_remap();
void fillidttable(uint32 offset, int index, uint8 type )
{
    idtentries[index].offset_2 = (offset >> 16 ) & 0xffff ;
    idtentries[index].selector =  0x08;
    idtentries[index].zero = 0 ;
    idtentries[index].type_attr =  type;
    // present(1bit)Ring(2bit)interrupttype(1bit)gatetype(4bits)
    idtentries[index].offset_1 = (offset) & 0xffff;
}

idt_ptr_t idtpointer;

void initializeidt()
{
    idtpointer.limit = sizeof(IDTDescr_t) * 256 -1;
    idtpointer.base = (uint32 ) &idtentries[0];
    fillidttable((uint32)isr0, 0, 0x8e);
    fillidttable((uint32)isr1, 1, 0x8e);
    fillidttable((uint32)isr2, 2, 0x8e);
    fillidttable((uint32)isr3, 3, 0x8e);
    fillidttable((uint32)isr4, 4, 0x8e);
    fillidttable((uint32)isr5, 5, 0x8e);
    fillidttable((uint32)isr6, 6, 0x8e);
    fillidttable((uint32)isr7, 7, 0x8e);
    fillidttable((uint32)isr8, 8, 0x8e);
    fillidttable((uint32)isr9, 9, 0x8e);
    fillidttable((uint32)isr10, 10, 0x8e);
    fillidttable((uint32)isr11, 11, 0x8e);
    fillidttable((uint32)isr12, 12, 0x8e);
    fillidttable((uint32)isr13, 13, 0x8e);
    fillidttable((uint32)isr14, 14, 0x8e);
    fillidttable((uint32)isr15, 15, 0x8e);
    fillidttable((uint32)isr16, 16, 0x8e);
    fillidttable((uint32)isr17, 17, 0x8e);
    fillidttable((uint32)isr18, 18, 0x8e);
    fillidttable((uint32)isr19, 19, 0x8e);
    fillidttable((uint32)isr20, 20, 0x8e);
    fillidttable((uint32)isr21, 21, 0x8e);
    fillidttable((uint32)isr22, 22, 0x8e);
    fillidttable((uint32)isr23, 23, 0x8e);
    fillidttable((uint32)isr24, 24, 0x8e);
    fillidttable((uint32)isr25, 25, 0x8e);
    fillidttable((uint32)isr26, 26, 0x8e);
    fillidttable((uint32)isr27, 27, 0x8e);
    fillidttable((uint32)isr28, 28, 0x8e);
    fillidttable((uint32)isr29, 29, 0x8e);
    fillidttable((uint32)isr30, 30, 0x8e);
    fillidttable((uint32)isr31, 31, 0x8e);
    fillidttable((uint32)isr32, 32, 0x8e);
    fillidttable((uint32)isr33, 33, 0x8e);
    fillidttable((uint32)isr34, 34, 0x8e);
    fillidttable((uint32)isr35, 35, 0x8e);
    fillidttable((uint32)isr36, 36, 0x8e);
    fillidttable((uint32)isr37, 37, 0x8e);
    fillidttable((uint32)isr38, 38, 0x8e);
    fillidttable((uint32)isr39, 39, 0x8e);
    fillidttable((uint32)isr40, 40, 0x8e);
    fillidttable((uint32)isr41, 41, 0x8e);
    fillidttable((uint32)isr42, 42, 0x8e);
    fillidttable((uint32)isr43, 43, 0x8e);
    fillidttable((uint32)isr44, 44, 0x8e);
    fillidttable((uint32)isr45, 45, 0x8e);
    fillidttable((uint32)isr46, 46, 0x8e);
    fillidttable((uint32)isr47, 47, 0x8e);
    fillidttable((uint32)isr48, 48, 0x8e);
    fillidttable((uint32)isr49, 49, 0x8e);
    fillidttable((uint32)isr50, 50, 0x8e);
    fillidttable((uint32)isr51, 51, 0x8e);
    fillidttable((uint32)isr52, 52, 0x8e);
    fillidttable((uint32)isr53, 53, 0x8e);
    fillidttable((uint32)isr54, 54, 0x8e);
    fillidttable((uint32)isr55, 55, 0x8e);
    fillidttable((uint32)isr56, 56, 0x8e);
    fillidttable((uint32)isr57, 57, 0x8e);
    fillidttable((uint32)isr58, 58, 0x8e);
    fillidttable((uint32)isr59, 59, 0x8e);
    fillidttable((uint32)isr60, 60, 0x8e);
    idt_flush((uint32)&idtpointer);
    irq_remap();
    initializeisr();
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
// taken from the site http://www.osdever.net/bkerndev/Docs/irqs.htm
void irq_remap(void)
{
    // this is the remapping of the hardware interrupts nothing related with software interrupts generated by cpu.
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}
