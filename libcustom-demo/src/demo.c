
/* 
 * @file:   demo.c
 * @author: owasys
 * @brief: Module adaptation to the pollux-core. It's the polluxd plug-in for libcustom demonstration
 * @date   2024-03-20, 13:25
 * 
 * Copyright (c) 2024 owasys
 * All rights reserved. Permission to use, modify or copy this software in whole or in part is
 * forbidden without prior, writen consent of the copyright holder.* 
 */


#define _DEMO_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

#include <owasys/pollux/pollux.h>

#include "common.h"
#include "demo.h"

int   Init     ( void *srv_conf){
   int ret;
   bzero( &live, sizeof( Live_t));

   live.cb_getVar = ( ( srv_param_t*) srv_conf)->cb_getVar;
   live.cb_command= ( ( srv_param_t*) srv_conf)->cb_command;
   live.cb_log    = ( ( srv_param_t*) srv_conf)->cb_log;
   live.status = ST_INIT;
   strncpy( live.filename, ( ( srv_param_t*) srv_conf)->filename, MAX_STRING_SIZE - 1);
   __Log( LOG_NOTICE, "%s (%s) Init", LIB_NAME, LIB_VERSION);
   ret = Load( ( ( srv_param_t*) srv_conf)->filename);
   if( ret){
      __Log( LOG_ERR, "onLoading: %d", ret);
   }
   return NO_ERROR;
}
int Load(char *Filename) {
   int ret = NO_ERROR, i;
   json_value *root, *metrics, *obj;
   const json_conf_t conf[] ={
      {"redis.queues.out", live.redis.queue.out, VAL_STR, 0, MAX_STRING_SIZE},
      {"metrics.send", &live.conf.update_time_ms, VAL_INT, 0, sizeof( int)},
      {"", NULL, 0, 0, 0}
   };
   load_json_conf_file( ( void*) conf, Filename);

   root = json_parse_file( Filename);
   if( root){
      obj = json_value_find( root, "metrics");
      if( obj){
         metrics = json_value_find( obj, "raw");
         if( metrics && metrics->type == json_array){
            live.metrics.count = metrics->u.array.length;
            for( i = 0; i < live.metrics.count && i < MAX_CAN_METRICS; i++){
               json_value *item = metrics->u.array.values[ i];
               if( item && item->type == json_object){
                  obj = json_value_find( item, "tag");
                  if( obj && obj->type == json_string){
                     snprintf( live.metrics.name.tags[ i], MAX_STRING_SIZE, "%s", obj->u.string.ptr);
                  }
                  obj = json_value_find( item, "alias");
                  if( obj && obj->type == json_string){
                     snprintf( live.metrics.name.alias[ i], MAX_STRING_SIZE, "%s", obj->u.string.ptr);
                  }
                  __Log( LOG_INFO, "Tag: %*s, Alias: %s", 64,live.metrics.name.tags[i], live.metrics.name.alias[ i]);
               }
            }
         }
      }
      json_value_free( root);
   }
   /*if( exists( CUSTOM_FILE)){
      load_json_conf_file( ( void*) live_conf, CUSTOM_FILE);
   }*/

   return ret;
}

int   Start    ( ){
   live.status = ST_STARTING;
   return NO_ERROR;   
}

int   End      ( ){
   live.status = ST_ENDING;
   live.status = ST_OFF;
   return NO_ERROR;
}
int   Run      ( ){
   time_t now = time( NULL);
   if( live.status < ST_RUNNING)
      live.status = ST_RUNNING;
   if( live.conf.update_time_ms > 0){ //If we have to send the metrics
      __Log( LOG_DEBUG, "Run: %ld", now - live.metrics.last);
      if( ( now - live.metrics.last) > live.conf.update_time_ms/1000){
         __Log( LOG_NOTICE, "> compose message by TIME: %d", now);
         compose_msg( now);
      }
   }
   return NO_ERROR;
}

int compose_msg( time_t now){
   char msg[ BUFFER_SIZE];
   bzero( msg, BUFFER_SIZE);
   int metrics = 0;
   snprintf( msg, BUFFER_SIZE, PATTERN_BOF, live.redis.queue.out);
   for( int i = 0; i < live.metrics.count; i++){
      if( ( live.metrics.vars[ i].stamp > live.metrics.last)){
         if( metrics > 0)//We have already a value
            snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), ", ");
         __valueStringify( live.metrics.vars[ i], "", &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), live.metrics.name.alias[i]);
         metrics++;
      }
   }
   if( metrics == 0){
      bzero( msg, BUFFER_SIZE);
   }
   __Log( LOG_DEBUG, "Run Position: %ld %ld", live.position.LastValid, live.metrics.last);
   if( live.position.LastValid > live.metrics.last){
      if( strlen( msg) > 0){
         snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), ", ");
         snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), PATTERN_POSITION, live.position.Latitude, live.position.Longitude,
            (uint64_t) live.position.LastValid, live.position.Speed.speed, live.position.Speed.course, live.position.satellites,
            live.position.altitude, live.position.accuracy, live.position.Speed.speed > 5?1:0);
      } else{
         snprintf( msg, BUFFER_SIZE, PATTERN_BOF, live.redis.queue.out);
         snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), PATTERN_POSITION, live.position.Latitude, live.position.Longitude,
            (uint64_t) live.position.LastValid, live.position.Speed.speed, live.position.Speed.course, live.position.satellites,
            live.position.altitude, live.position.accuracy, live.position.Speed.speed > 5?1:0);
      }
   } else {
      if( metrics > 0)
         snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), "}");
   }
   if( strlen(msg) > 0){
      char date[ MAX_STRING_SIZE];
      bzero( date, MAX_STRING_SIZE);
      date_ISO8601( now, date);
      snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), PATTERN_EOF, date, ( uint64_t) now);
   }
   live.metrics.last = now;
   if( strlen( msg) > 0){
      live.cb_command( APP_PUSHTOREDIS, msg);
   }
   return NO_ERROR;
}

Status_t   Status   ( ){
   return live.status;
}
int   Command       ( int cmd, char *args){
   return NO_ERROR;
}
int   EventHandler  ( int type, char *source, void *val){
   sh_val_t *var;
   Pposition_t *pos;
   char msg[ BUFFER_SIZE];
   char date[ MAX_STRING_SIZE];
   switch( type){
      case EV_RXVALUE: //Value coming from the system
         var = ( sh_val_t *)val;
         if( strstr( var->tag, "beat") || strstr( var->tag, "state"))
            return NO_ERROR;
         __Log( LOG_DEBUG, "RXVALUE: %s:%s", source, var->tag);
         metrics_rx( source, val);
         //** todo: add here any kind of calculation or send data on change */
         if( streq( source, "device") && streq( var->tag, "dio0")){
            __Log( LOG_NOTICE, "> compose message by DIO0: %d", var->val.iVal);
            compose_msg( time( NULL));
         } else if( streq( source, "app") && streq( var->tag, "move")){
            __Log( LOG_NOTICE, "> compose message by MOVE: %d", var->val.iVal);
            compose_msg( time( NULL));
         } else if( streq( source, "device") && ( streq( var->tag, "pow3") || streq( var->tag, "pow.fail"))){
            __Log( LOG_NOTICE, "> compose message by POWER FAIL: %d", var->val.iVal);
            compose_msg( time( NULL));
         }
         break;
      case EV_RXPOSITION:
         pos = (Pposition_t *) val;
         if( ( !live.position.DataValid) && ( !pos->DataValid)){
            return NO_ERROR;
         }
         memcpy( &live.position, pos, sizeof( Pposition_t));
         break;
      case EV_TXPOSITION:
         if( strncmp( source, "track", strlen( "track")) == 0){
            __Log( LOG_DEBUG, "Received track: %s", source);
            pos = (Pposition_t *) val;
            if( ( !live.position.DataValid) && ( !pos->DataValid)){
               __Log( LOG_INFO, "Repeated: non valid position");
               return NO_ERROR;
            }
            memcpy( &live.position, pos, sizeof( Pposition_t));
            bzero( msg, BUFFER_SIZE);
            snprintf( msg, BUFFER_SIZE, PATTERN_BOF, live.redis.queue.out);
            snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), PATTERN_POSITION, live.position.Latitude, live.position.Longitude,
               (uint64_t) live.position.LastValid, live.position.Speed.speed, live.position.Speed.course, live.position.satellites,
               live.position.altitude, live.position.accuracy, live.position.Speed.speed > 5?1:0);
            bzero( date, MAX_STRING_SIZE);
            date_ISO8601( live.position.LastValid, date);
            snprintf( &msg[strlen( msg)], BUFFER_SIZE - strlen( msg), PATTERN_EOF, date, (uint64_t) live.position.LastValid);
            live.cb_command( APP_PUSHTOREDIS, msg);
         }         
         break;
      case EV_TXVALUE: //Periodic or values sent by the rules....
         break;
      case EV_CMD:
         if( strcmp( source, "json") == 0){//cmd
            json_value *cmd = ( json_value *) val;
            if( cmd->type == json_string){
               _Commands( val,NULL);
            }
         }
         //return Commands( source, val);
         break;
      case EV_RXSMS:
         //return Sms( source, val);
         break;
      default:
         printf( "Not Treated %d, %s\n", type, source);
         break;
   }
   return NO_ERROR;
}
int metrics_rx( char *source, void *var){
   sh_val_t *val = (sh_val_t *) var;
   char strval[ MAX_STRING_SIZE];
   int i;

   bzero( strval, MAX_STRING_SIZE);
   snprintf( strval, MAX_STRING_SIZE, "%s.%s", source, val->tag);
   __Log( LOG_DEBUG, "(%d) > %s", val->type,  strval);
   for( i = 0; i < live.metrics.count; i++){
      if( strstr( live.metrics.name.tags[ i], source)){
         if( strstr( live.metrics.name.tags[ i], val->tag)){
            memcpy( &live.metrics.vars[ i], val, sizeof( sh_val_t));
            break;
         }
      }
   }
   return NO_ERROR;
}

void __valueStringify( sh_val_t val, char *source, char *value, int size, char *alias){
   char preffix[ MAX_TAG_SIZE] = "";
   char *name;
   if( strlen( source) > 0)
      snprintf( preffix, MAX_TAG_SIZE, "%s.", source);
   name = val.tag;
   if( alias)
      name = alias;
   switch( val.type){
      case SH_VAL_INT:
         snprintf( value, size, "\"%s%s\": %d", preffix, name, val.val.iVal);
         break;
      case SH_VAL_LONG:
         snprintf( value, size, "\"%s%s\": %ld", preffix, name, val.val.lVal);
         break;
      case SH_VAL_FLOAT:
         if( val.val.fVal != val.val.fVal){
            __Log( LOG_WARNING, "%s nan: %f", name, val.val.fVal);
            break;
         }
         snprintf( value, size, "\"%s%s\": %.1f", preffix, name, val.val.fVal);
         break;
      case SH_VAL_BOOL:
         snprintf( value, size, "\"%s%s\": %s", preffix, name, val.val.bVal?"true":"false");
         break;
      case SH_VAL_LONG_LONG:
         snprintf( value, size, "\"%s%s\": %lld", preffix, name, val.val.llVal);
         break;
      case SH_VAL_DOUBLE:
         if( val.val.dVal != val.val.dVal){
            __Log( LOG_WARNING, "%s nan: %f", name, val.val.dVal);
            break;
         }
         snprintf( value, size, "\"%s%s\": %.1lf", preffix, name, val.val.dVal);
         break;
      default:
         break;
   }
}
void __Log(int __pri, const char *__fmt, ...) {
   char log[ MAX_STRING_SIZE];
   va_list arg_ptr;
   if( live.cb_log){
      bzero( log, MAX_STRING_SIZE);
      snprintf( log, MAX_STRING_SIZE, "(%s) ", LIBRARY_NAME);
      va_start( arg_ptr, __fmt);
      vsnprintf( &log[strlen(log)], MAX_STRING_SIZE - strlen(log), __fmt, arg_ptr);
      va_end( arg_ptr);
      live.cb_log( __pri, log);
   }
}

int _Commands( void *data, void *aux){
   int i;
   json_value *cmd = ( json_value *) data;
   const in_t cmds[] = {
      { "", NULL}
   };
   i = 0;
   if( cmd->type == json_string){
      __Log( LOG_NOTICE, "_Commands: %s", cmd->u.string.ptr);
      while( cmds[i].fHandler != NULL){
         if( strncmp( cmds[i].cmd, cmd->u.string.ptr, strlen( cmds[i].cmd)) == 0){
            return cmds[i].fHandler( cmd, cmds[i].cmd);
         }
         i++;
      }
   }
   return -1;
}
