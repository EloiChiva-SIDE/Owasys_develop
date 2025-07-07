/* 
 * @file   common.h
 * @author ppi
 * @brief  common header file for the libcustom-demo library
 * @date   2024-03-22, 13:25
 * 
 * 
 * Copyright (c) 2024 Owasys
 * All rights reserved. Permission to use, modify or copy this software in whole or in part is
 * forbidden without prior, writen consent of the copyright holder.* 
 */

#define LIB_NAME     NAME
#define LIB_VERSION  VERSION

// Strings MACROS
#define streq(a, b) (strcmp((a), (b)) == 0)
#define exists(a) (access((a), F_OK) != -1)
#define streq_nocase(a,b) (strcasecmp((a),(b)) == 0)
#define streq_n(a,b,n) (strncmp((a),(b),(n)) == 0)
#define streq_nocase_n(a,b,n) (strncasecmp((a),(b),(n)) == 0)
#define strcontains(a,b) (strstr((a),(b)) != NULL)
#define strcontains_nocase(a,b) (strcasestr((a),(b)) != NULL)

