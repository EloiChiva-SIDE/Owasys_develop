/* 
 * @file   demo.h
 * @author owasys
 * @brief  Module adaptation to the pollux-core. It's the pollux-core plug-in for the custom-demo project.
 * @date   2024-03-20, 13:25
 * 
 * 
 * @note: This is part of the libcustom-demo library, which is built as a dynamic library to be used on pollux-core and polluxc-mqtt
 * The functions on this file will be used by polluxd to process the data from the DBUS or polluxd and send it to the redis queue
 * 
 * Copyright (c) 2024 Owasys
 * All rights reserved. Permission to use, modify or copy this software in whole or in part is
 * forbidden without prior, writen consent of the copyright holder.* 
 */


#ifndef DEMO_H
#define DEMO_H

#include <owasys/pollux/type-defs.h>

/*
 * cmd: Tells if it is "reboot", "reconnect", "update","set", "get","info", "tunnel start","tunnel stop"
 * data: frame itself to be executed
 * ts: Timestamp
 * id: device identifier source of the information
 * type: Type of the value, bool, int, float, double....
 */
#define  QUEUE_OUT         "demo"
#define  LIBRARY_NAME      NAME

#define  PATTERN_BOF       "%s|{ \"values\":{"
#define  PATTERN_POSITION  "\"position\":{\"latitude\":%7.5lf,\"longitude\":%8.5lf,\
\"lastvalid\":%"PRIu64",\"speed\":%4.1lf,\"course\":%4.1lf,\"numsat\":%d,\"altitude\":%d, \"accuracy\": %.1f, \"moving\":%d}}"
#define  PATTERN_EOF       ", \"timestamp\": \"%s\", \"ts\": %"PRIu64"000}"

#define LOG_PATH        "/var/log/pollux/"
#define CONF_PATH       "/etc/pollux/"

#define LOG_FILE        LOG_PATH"demo.log"
#define CUSTOM_FILE     CONF_PATH"custom-data.json"

#ifdef _DEMO_C
   #define  BUFFER_SIZE       1024
   #define  ID_LEN            20
   #define  MAX_GEOFENCES     255
   //Can definitions
   #define  MAX_CANS          1
   #define  MAX_CAN_VARS      32
   #define  MAX_CAN_SENSORS   4
   #define  MAX_CAN_SENSOR_T  2  //seconds to consider sensor as valid
   #define  CAN_ENABLED_TIME  10 //seconds to consider can is disabled after not receiving data from can

//Subscriptions
   #define MAX_CAN_METRICS    20 //Maximum can metrics to send
   typedef struct { //Generic commands
      char *cmd;
      int  ( *fHandler) ( void*, void*);
   } in_t;
   typedef struct{
      bool     valid;
      sh_val_t value;
      time_t   ts;
      int      time;    //Time to send in seconds...not used
      float    delta;   //Delta value...not used
   }sensor_t;
   typedef struct{
      bool     running; //The can bus is running
      time_t   last;    //The last received data 
      sensor_t values[ MAX_CAN_VARS];
   }can_t;
   typedef struct {
      Status_t status;
      int      ( *cb_getVar)  ( char*, void *);       //Var initialization. To know from other modules
      int      ( *cb_command) ( int, char*);   //Command to the main module...
      int      ( *cb_newvalue)( char*, void*);     //Call to the parent service to notifiy a new value
      void     ( *cb_log)     ( int, const char*, ...);   //Log function
      int      shmId;
      char     owaSn[ ID_LEN];
      struct{
         int valid;
         double lat;
         double lon;
      }pos;
      can_t    can;
      struct{
         int   count;   //Count of the variables
         time_t ts;
         char msg[ BUFFER_SIZE];
      }msg;
      char filename[ MAX_STRING_SIZE];
      struct{
         int  update_time_ms; //Time interval to update the geoposition and the metrics
      }conf;
      struct{
         struct{
            char out[ MAX_STRING_SIZE];
         }queue;
      }redis;
      struct{
         time_t      last;                //Last time data was sent
         int         count;               //Number of metrics
         struct{
            char        tags[ MAX_CAN_METRICS][ MAX_STRING_SIZE];
            char        alias[ MAX_CAN_METRICS][ MAX_TAG_SIZE];
         }name;
         sh_val_t    vars[ MAX_CAN_METRICS];  //The var tags will be the alias
      }metrics;
      Pposition_t position;
   } Live_t;


   int   Init     ( void *args);
   int   Start    ( );
   int   End      ( );
   int   Run      ( );
   Status_t   Status    ( );
   int   Command        ( int cmd, char *args);
   int   EventHandler   ( int type, char *source, void *val);
   /* Propietary declarations */
   int   Load           ( char *filename);
   void __valueStringify( sh_val_t val, char *source, char *value, int size, char *alias);
   void __Log           ( int __pri, const char *__fmt, ...);
   int _Commands        ( void *data, void *aux);
   // Metrics management functions
   int   metrics_rx     ( char *source, void *var);
   int   compose_msg    ( time_t now);
   /* Variables declarations */
   static Live_t live;
#else
   extern int   Init     ( void *args);
   extern int   Start    ( );
   extern int   End      ( );
   extern int   Run      ( );
   extern Status_t   Status   ( );
   extern int   Command       ( int cmd, char *args);
   extern int   EventHandler  ( int type, char *source, void *val);   
#endif
#endif /* DEMO_H */

