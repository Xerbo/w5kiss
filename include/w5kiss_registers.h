#ifndef _W5KISS_REGISTERS_H
#define _W5KISS_REGISTERS_H

// Blocks
enum {
    BSB_COMMON     = 0b00000,
    BSB_SOCKET0    = 0b00001,
    BSB_SOCKET0_TX = 0b00010,
    BSB_SOCKET0_RX = 0b00011,
};

// Common registers
enum {
    COMMON_MR      = 0x0000, // Mode Register [R/W]
    COMMON_SHAR    = 0x0009, // Source Hardware Address Register [R/W]
    COMMON_PHYCFGR = 0x002E, // W5500 PHY Configuration Register [R/W]
};

// Socket registers
enum {
    SOCKET_MR         = 0x0000, // Socket Mode Register [R/W]
    SOCKET_CR         = 0x0001, // Socket Command Register [R/W]
    SOCKET_RXBUF_SIZE = 0x001E, // Socket RX Buffer Size Register [R/W]
    SOCKET_TXBUF_SIZE = 0x001F, // Socket TX Buffer Size Register [R/W]
    SOCKET_TX_FSR     = 0x0020, // Socket TX Free Size Register [R]
    SOCKET_TX_WR      = 0x0024, // Socket TX Write Pointer Register [R/W]
    SOCKET_RX_RSR     = 0x0026, // Socket Received Size Register [R]
    SOCKET_RX_RD      = 0x0028, // Socket RX Read Data Pointer Register [R/W]
};

// Mode register values
enum {
    MR_RST   = 1 << 7, // Reset
    MR_WOL   = 1 << 5, // Wake on LAN
    MR_PB    = 1 << 4, // Ping Block Mode
    MR_PPPOE = 1 << 3, // PPPoE Mode
    MR_FARP  = 1 << 1, // Force ARP
};

// PHY config register values
enum {
    PHYCFGR_DPX  = 1 << 2, // Duplex Status
    PHYCFGR_SPD  = 1 << 1, // Speed Status
    PHYCFGR_LNK  = 1 << 0, // Link Status
};

// Socket mode register values
enum {
    SMR_MULTI_MFEN   = 1 << 7, // Multicast In UDP Mode / MAC Filter In MACRAW Mode
    SMR_BCASTB       = 1 << 6, // Broadcast Blocking In MACRAW Mode
    SMR_ND_MC_MMB    = 1 << 5, // Use No Delay ACK / Multicast / Multicast Blocking In MACRAW Mode
    SMR_UCASTB_MIP6B = 1 << 4, // Unicast Blocking In UDP Mode / IPv6 Blocking In MACRAW Mode
    SMR_CLOSED       = 1 << 3,
    SMR_MACRAW       = 1 << 2,
    SMR_UDP          = 1 << 1,
    SMR_TCP          = 1 << 0,
};

// Socket command register values
enum {
    SCR_OPEN      = 0x01,
    SCR_LISTEN    = 0x02,
    SCR_CONNECT   = 0x04,
    SCR_DISCON    = 0x08,
    SCR_CLOSE     = 0x10,
    SCR_SEND      = 0x20,
    SCR_SEND_MAC  = 0x21,
    SCR_SEND_KEEP = 0x22,
    SCR_RECV      = 0x40,
};

#endif
