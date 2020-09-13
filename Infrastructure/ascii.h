#ifndef ASCII_H
#define ASCII_H

///*************************************************************
///*************************************************************
///     ascii defintions 
///*************************************************************
///*************************************************************

#define ASCII_NUL 0x00   /*  null */
#define ASCII_SOH 0x01   /*  start of header */
#define ASCII_STX 0X02   /*  start of text */
#define ASCII_ETX 0x03   /*  end of text */
#define ASCII_EOT 0x04   /*  end of transmission */
#define ASCII_ENQ 0x05   /*  enquiry */
#define ASCII_ACK 0x06   /*  acknowledge */
#define ASCII_BEL 0x07   /*  bell or alarm */
#define ASCII_BS  0x08   /*  backspace */
#define ASCII_HT  0x09   /*  horizontal tab */
#define ASCII_LF  0x0A   /*  line feed */
#define ASCII_VT  0x0B   /*  vertical tab */
#define ASCII_FF  0x0C   /*  form feed */
#define ASCII_CR  0x0D   /*  carriage return */
#define ASCII_SO  0x0E   /*  shift out */
#define ASCII_SI  0x0F   /*  shift in */
#define ASCII_DLE 0x10   /*  data link escape */
#define ASCII_DC1 0x11   /*  device control 1   CTRL_Q */
#define ASCII_DC2 0x12   /*  device control 2 */
#define ASCII_DC3 0x13   /*  device control 3   CTRL_S */
#define ASCII_DC4 0x14   /*  device control 4 */
#define ASCII_NAK 0x15   /*  negative acknowledge */
#define ASCII_SYN 0x16   /*  synchronous idle */
#define ASCII_ETB 0x17   /*  end of transmission block */
#define ASCII_CAN 0x18   /*  cancel */
#define ASCII_EM  0x19   /*  end of medium */
#define ASCII_SUB 0x1A   /*  substitute */
#define ASCII_ESC 0x1b   /*  escape */
#define ASCII_FS  0x1c   /*  file seperator */
#define ASCII_GS  0x1d   /*  group seperator */
#define ASCII_RS  0x1e   /*  record seperator */
#define ASCII_US  0x1f   /*  unit seperator */
#define ASCII_SP  0x20   /*  space character */
#define ASCII_DEL 0x7f   /*  delete */

///*************************************************************
///*************************************************************
/* control sequences */
//#define ASCII_CTRL_@ 0x00   /*  null */
#define ASCII_CTRL_A 0x01   /*  start of header */
#define ASCII_CTRL_B 0X02   /*  start of text */
#define ASCII_CTRL_C 0x03   /*  end of text */
#define ASCII_CTRL_D 0x04   /*  end of transmission */
#define ASCII_CTRL_E 0x05   /*  enquiry */
#define ASCII_CTRL_F 0x06   /*  acknowledge */
#define ASCII_CTRL_G 0x07   /*  bell or alarm */
#define ASCII_CTRL_H 0x08   /*  backspace */
#define ASCII_CTRL_I 0x09   /*  horizontal tab */
#define ASCII_CTRL_J 0x0A   /*  line feed */
#define ASCII_CTRL_K 0x0B   /*  vertical tab */
#define ASCII_CTRL_L 0x0C   /*  form feed */
#define ASCII_CTRL_M 0x0D   /*  carriage return */
#define ASCII_CTRL_N 0x0E   /*  shift out */
#define ASCII_CTRL_O 0x0F   /*  shift in */
#define ASCII_CTRL_P 0x10   /*  data link escape */
#define ASCII_CTRL_Q 0x11   /*  device control 1   CTRL_Q */
#define ASCII_CTRL_R 0x12   /*  device control 2 */
#define ASCII_CTRL_S 0x13   /*  device control 3   CTRL_S */
#define ASCII_CTRL_T 0x14   /*  device control 4 */
#define ASCII_CTRL_U 0x15   /*  negative acknowledge */
#define ASCII_CTRL_V 0x16   /*  synchronous idle */
#define ASCII_CTRL_W 0x17   /*  end of transmission block */
#define ASCII_CTRL_X 0x18   /*  cancel */
#define ASCII_CTRL_Y 0x19   /*  end of medium */
#define ASCII_CTRL_Z 0x1A   /*  substitute */

///*************************************************************
///*************************************************************

#define ASCII_CTRL_LEFTBRACKET 0x1b   /*  escape */
#define ASCII_CTRL_FORWARDSLASH 0x1c   /*  file seperator */
#define ASCII_CTRL_RIGHTBRACKET 0x1d   /*  group seperator */
#define ASCII_CTRL_CARET 0x1e   /*  record seperator */
#define ASCII_CTRL_MINUS 0x1f   /*  unit seperator */

///*************************************************************
///*************************************************************

#endif
