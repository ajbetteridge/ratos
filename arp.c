// this supports arp feature of networking, written for ratos 
#include "arp.h"

#define ArpMapSize 100
ArpMapTable_t HwIpMapTable[ArpMapSize];
uint32 CurrentHwIPMapSize = 0;

char HostHWAdd[6] = { 0x00,0xaa,0x00,0x60,0x21,0x01 };
char HostIP[6] = {10,0,2,2};
enum ArpType { ARP_REQUEST=1, ARP_REPLY=2 };
struct ArpPacket
{
    unsigned char HWType[2];
    unsigned char ProtocolType[2];
    unsigned char HWAddLength;
    unsigned char ProtocolAddLength;
    unsigned char Operation[2];
    unsigned char SenderHWAddress[6];
    unsigned char SenderProtocolAddress[4];
    unsigned char TargetHWAddress[6];
    unsigned char TargetProtocolAddress[4];
//    unsigned char Extra[20];
};

typedef struct ArpPacket ArpPacket_t;

void DumpArpPacket(ArpPacket_t *data)
{
    printf("Dumping Arp packet\n");
    uint16 hwType = data->HWType[0]<<8 |  data->HWType[1];
    switch (hwType) {
    case 1:
        printf("HW type is ethernet\n");
        break;
    default :
        printf("Unidentified HW type\n");
        break;
    }
    uint16 protocolType = data->ProtocolType[0] << 8 | data->ProtocolType[1];
    switch (protocolType) {
    case 0x0800:
        printf("Protocol type is IPv4\n");
        break;
    default :
        printf("Unknown protocol type\n");
        break;
    }
    uint16 operation = data->Operation[0]<<8 | data->Operation[1];
    printf("Operation  Type = ");
    switch(operation) {
    case 1:
        printf(" Arp Request\n");
        break;
    case 2:
        printf(" Arp Reply\n");
        break;
    case 3:
        printf(" RArp Request\n");
        break;
    case 4:
        printf(" RArp Reply\n");
        break;
    case 5:
        printf(" DRArp Request\n");
        break;
    case 6:
        printf(" DRArp Reply\n");
        break;
    default :
        printf("Unkonwn Operation Type\n");
        break;
    }

    int i;
    printf("Sender HW address = ");
    for(i=0; i<data->HWAddLength; i++) {
        printf("%x ",data->SenderHWAddress[i]);
    }
    printf("\n");
    printf("Sender Protocol address = ");
    for(i=0; i<data->ProtocolAddLength; i++) {
        printf("%d ",data->SenderProtocolAddress[i]);
    }
    printf("\n");
    printf("Target HW address = ");
    for(i=0; i<data->HWAddLength; i++) {
        printf("%x ",data->TargetHWAddress[i]);
    }
    printf("\n");
    printf("Target Protocol address = ");
    for(i=0; i<data->ProtocolAddLength; i++) {
        printf("%d ",data->TargetProtocolAddress[i]);
    }
//    printf("Extra \n");
//    for(i=0; i<20; i++) {
//        printf("%x ",data->Extra[i]);
//    }
    printf("\n");
}

void FillHwIpMapTable(unsigned char *ip, unsigned char *mac)
{
    copy(HwIpMapTable[CurrentHwIPMapSize].key, ip, 4);
    copy(HwIpMapTable[CurrentHwIPMapSize].value, mac, 6);
    if( CurrentHwIPMapSize <= 100 ) {
        CurrentHwIPMapSize++;
    }
}

int CheckHwIpMapTable(unsigned char *IP, unsigned char *mac)
{
    int i;
    for(i=0; i<CurrentHwIPMapSize; i++) {
        if(!strcmp(HwIpMapTable[i].key,IP)) {
            copy(mac, HwIpMapTable[i].value, 6);
            return 1;
        }
    }
    return 0;
}

void GetMacAddressForIP(unsigned char *IP, unsigned char *mac)
{
    if( CheckHwIpMapTable(IP, mac) ) {
        return ;
    }
    ArpPacket_t *packet = (ArpPacket_t *) imalloc(sizeof(ArpPacket_t));
    packet->HWType[0] = 0;
    packet->HWType[1] = 1;
    packet->ProtocolType[0] = 0x08;
    packet->ProtocolType[1] = 0x00;
    packet->HWAddLength = 6;
    packet->ProtocolAddLength = 4;
    packet->Operation[0] = 0; //ARP Request high byte;
    packet->Operation[1] = ARP_REQUEST; //ARP Request;
    int i;
    for(i=0; i<6; i++) {
        packet->SenderHWAddress[i] = HostHWAdd[i];
    }
    for(i=0; i<4; i++) {
        packet->SenderProtocolAddress[i] = HostIP[i];
    }
    for(i=0; i<6; i++) {
        packet->TargetHWAddress[i] = 0xff;  //broadcast
    }
    for(i=0; i<4; i++) {
        packet->TargetProtocolAddress[i] = IP[i];
    }
    SendPacketToCard(packet->TargetHWAddress, packet, 28, 0x0806);
    uint8 turn = 0;
    while(!CheckHwIpMapTable(IP, mac) && turn < 3) {
        sleep(1); //we don't know when the packet will be transmitted for arp, so just keep waiting till than.
        turn++;
    }
}

void SendArpReply(unsigned char TargetHWAdd[6], char TargetIP[4])
{
    ArpPacket_t *packet = (ArpPacket_t *) imalloc(sizeof(ArpPacket_t));
    packet->HWType[0] = 0;
    packet->HWType[1] = 1;
    packet->ProtocolType[0] = 0x08;
    packet->ProtocolType[1] = 0x00;
    packet->HWAddLength = 6;
    packet->ProtocolAddLength = 4;
    packet->Operation[0] = 0; //ARP Reply;
    packet->Operation[1] = 2; //ARP Reply;
    int i;
    for(i=0; i<6; i++) {
        packet->SenderHWAddress[i] = HostHWAdd[i];
    }
    for(i=0; i<4; i++) {
        packet->SenderProtocolAddress[i] = HostIP[i];
    }
    for(i=0; i<6; i++) {
        packet->TargetHWAddress[i] = TargetHWAdd[i];
    }
    for(i=0; i<4; i++) {
        packet->TargetProtocolAddress[i] = TargetIP[i];
    }
    SendPacketToCard(TargetHWAdd, packet, sizeof(ArpPacket_t), 0x0806);
//	DumpArpPacket(packet);
}

void HandleArpPacket(unsigned char *data)
{
    ArpPacket_t *packet = ( ArpPacket_t *)data;
//	DumpArpPacket(data);
    if( packet->Operation[0] == 0 && packet->Operation[1] == ARP_REQUEST ) {
        SendArpReply(packet->SenderHWAddress, packet->SenderProtocolAddress);
        FillHwIpMapTable(packet->SenderProtocolAddress, packet->SenderHWAddress);
    }
    if( packet->Operation[0] == 0 && packet->Operation[1] == ARP_REPLY ) {
        FillHwIpMapTable(packet->SenderProtocolAddress, packet->SenderHWAddress);
    }
}
