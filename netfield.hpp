/*
    Copyright (C) 2013 Karl Vogel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _NETFIELD_H_
#define _NETFIELD_H_
#include <stdlib.h>
#include <stdint.h>

typedef enum {
        MP_END           = 0,
        MP_MSGID         = 1,
        MP_BYTE          = 2,
        MP_SHORT         = 3,
        MP_COMPRESSED_LONG  = 4,
        MP_mALIGNED_LONGLONG  = 5,
        MP_FLOAT         = 6,
        MP_VEC2          = 7,
        MP_VEC3          = 8,  // MP_m12BYTES
        MP_VEC4          = 9,
        MP_16BYTES       = 0xA, // GUID / 4 dword
        MP_28BYTES       = 0xB, // GUID / 7 dword
        MP_STRING        = 0xC,
        MP_CSTRING       = 0xD,
        MP_OPTIONAL      = 0xE,
        MP_ARRAYFIXED    = 0xF,
        MP_ARRAY_VAR_SMALL  = 0x10,
        MP_ARRAY_VAR_LARGE  = 0x11,
        MP_BUFFERFIXED  = 0x12,
        MP_BUFFER_VAR_SMALL  = 0x13,
        MP_BUFFER_VAR_LARGE  = 0x14,
        MP_mALIGN_TO_ELEMSIZE  = 0x15,
        MP_ALIGNED_LONG  = 0x16,
        MP_TERMINATE     = 0x17
} mpType;

struct netField {
        uint32_t type;
        uint32_t unk0;
        uint32_t param;
        uint32_t refTypeDef;
        uint32_t defSize;
        uint32_t maxSize;
};

struct netFieldI {
        mpType type;
        uint32_t param;
        uint32_t size;
        struct netFieldI* refTypeDef;
        const char *msgName;

        netFieldI(mpType t, uint32_t p, const char *n) 
                : type(t), param(p), msgName(n), size(0), refTypeDef(NULL) {};
        netFieldI(mpType t, uint32_t s=0, struct netFieldI* ref=NULL) 
                : type(t), size(s), refTypeDef(ref), param(0), msgName(NULL) {};
};

#define NETFIELD_MSGID(id,name)          { netFieldI(MP_MSGID,id,name) }
#define NETFIELD_END()                   { netFieldI(MP_END)           }
#define NETFIELD_BYTE()                  { netFieldI(MP_BYTE)            }
#define NETFIELD_SHORT()                 { netFieldI(MP_SHORT)           }
#define NETFIELD_COMPRESSED_LONG()       { netFieldI(MP_COMPRESSED_LONG) }
#define NETFIELD_mALIGNED_LONGLONG()     { netFieldI(MP_mALIGNED_LONGLONG)}
#define NETFIELD_FLOAT()                 { netFieldI(MP_FLOAT)           }
#define NETFIELD_VEC2()                  { netFieldI(MP_VEC2)            }
#define NETFIELD_VEC3()                  { netFieldI(MP_VEC3)            }
#define NETFIELD_VEC4()                  { netFieldI(MP_VEC4)            }
#define NETFIELD_16BYTES()               { netFieldI(MP_16BYTES)         }
#define NETFIELD_28BYTES()               { netFieldI(MP_28BYTES)         }
#define NETFIELD_STRING(s)               { netFieldI(MP_STRING,s) }
#define NETFIELD_CSTRING(s)              { netFieldI(MP_CSTRING,s) }
#define NETFIELD_OPTIONAL(ref)           { netFieldI(MP_OPTIONAL,0,ref) }
#define NETFIELD_ARRAYFIXED(s,ref)       { netFieldI(MP_ARRAYFIXED,s,ref) }
#define NETFIELD_ARRAY_VAR_SMALL(s,ref)  { netFieldI(MP_ARRAY_VAR_SMALL,s,ref) }
#define NETFIELD_ARRAY_VAR_LARGE(s, ref) { netFieldI(MP_ARRAY_VAR_LARGE,s,ref) }
#define NETFIELD_BUFFERFIXED(s)          { netFieldI(MP_BUFFERFIXED,s) }
#define NETFIELD_BUFFER_VAR_SMALL(s)     { netFieldI(MP_BUFFER_VAR_SMALL,s) }
#define NETFIELD_BUFFER_VAR_LARGE(s)     { netFieldI(MP_BUFFER_VAR_LARGE,s) }
#define NETFIELD_mALIGN_TO_ELEMSIZE(s)   { netFieldI(MP_mALIGN_TO_ELEMSIZE,s) }
#define NETFIELD_ALIGNED_LONG()          { netFieldI(MP_ALIGNED_LONG)    }
#define NETFIELD_TERMINATE()             { netFieldI(MP_TERMINATE)       }

#endif /* _NETFIELD_H_ */
