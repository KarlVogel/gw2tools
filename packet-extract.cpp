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
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <bfd.h>
#include <string.h>
#include <assert.h>
#include "netfield.hpp"


struct psect {
	bfd_byte *data;
        asection *sec;
};
struct psect secText, secRdata, secData;

#define inSection(s, a)  (a >= bfd_section_vma(tmp_bfd, (s)->sec) && a <= bfd_section_vma(tmp_bfd, (s)->sec)+bfd_section_size(tmp_bfd, (s)->sec))

bfd_vma f2v(struct psect *s, uint32_t addr)
{
        bfd_vma dest;

        dest= (bfd_vma)addr;
        dest -= bfd_section_vma(tmp_bfd, s->sec);
        dest += (bfd_vma)&s->data[0];
        return dest;
}

uint32_t v2f(struct psect *s, bfd_vma addr)
{
        uint32_t dest;

        dest= (uint32_t)(addr - (bfd_vma)&s->data[0]);
        dest += bfd_section_vma(tmp_bfd, s->sec);
        return dest;
}


class Message {
public:
        struct netField *message;
        bool isServer;
        bool hasSub;
        uint32_t msgid;

        Message(char *n, struct netField *nf, bool srv);
        void display();
	std::string getName() const;
	std::string getBasicName() const { return name; };
private:
        void decodeFields(std::string name, struct netField *nf);
        void innerDisplay(std::string name, struct netField *nf);

        std::string name;
};

Message::Message(char *n, struct netField *netf, bool srv)
        : message(netf), isServer(srv), hasSub(false)
{
        std::string msgName(n);

        struct netField *nf = message;
        while( nf->type != MP_END && hasSub == false) {
                switch(nf->type) {
                case MP_MSGID:
                        msgid= nf->param;
                        break;
                case MP_ARRAYFIXED:
                case MP_ARRAY_VAR_SMALL:
                case MP_ARRAY_VAR_LARGE:
                case MP_OPTIONAL:
                        hasSub= true;
                        break;

                default:
                        break;
                }
                nf++;
        }

        size_t pos= msgName.rfind(".cpp");
        if (pos != std::string::npos)
                msgName.erase(pos);

	name= msgName;
}
std::string
Message::getName() const
{
        std::ostringstream ss;

        ss << (isServer?"server":"client")
           << "Msg" << name << '_' << std::hex << msgid;

        return ss.str();
}

void
Message::decodeFields(std::string name, struct netField *nf)
{
        uint32_t field= 0;
        while( nf->type != MP_END) {

                field++;
#if 0
                printf("\t\t\t\t\t NETFIELD_PARAM(0x%x,0x%x,0x%x,0x%x,0x%x,0x%x),\n",
                       nf->type, nf->unk0, nf->param, nf->refTypeDef,
                       nf->defSize, nf->maxSize);
#endif

                std::cout << '\t';
                switch(nf->type) {
                case MP_MSGID:
                        std::cout << "NETFIELD_MSGID(0x" << std::hex << nf->param
                                  << ", \"" << name << "\"),";
			std::cout << "\t// 0x" << std::hex << v2f(&secData, (bfd_vma)nf ) << std::endl;
                        break;

                case MP_COMPRESSED_LONG:
                        std::cout << "\tNETFIELD_COMPRESSED_LONG()," << std::endl;
                        break;

                case MP_BYTE:
                        std::cout << "\tNETFIELD_BYTE()," << std::endl;
                        break;

                case MP_SHORT:
                        std::cout << "\tNETFIELD_SHORT()," << std::endl;
                        break;

                case MP_STRING:
                        std::cout << "\tNETFIELD_STRING(0x" << std::hex << nf->param << ")," << std::endl;
                        break;

                case MP_CSTRING:
                        std::cout << "\tNETFIELD_CSTRING(0x" << std::hex << nf->param << ")," << std::endl;
                        break;

                case MP_16BYTES:
                        std::cout << "\tNETFIELD_16BYTES()," << std::endl;
                        break;

                case MP_28BYTES:
                        std::cout << "\tNETFIELD_28BYTES()," << std::endl;
                        break;

                case MP_VEC2:
                        std::cout << "\tNETFIELD_VEC2()," << std::endl;
                        break;

                case MP_VEC3:
                        std::cout << "\tNETFIELD_VEC3()," << std::endl;
                        break;

                case MP_VEC4:
                        std::cout << "\tNETFIELD_VEC4()," << std::endl;
                        break;

                case MP_FLOAT:
                        std::cout << "\tNETFIELD_FLOAT()," << std::endl;
                        break;

                case MP_mALIGNED_LONGLONG:
                        std::cout << "\tNETFIELD_mALIGNED_LONGLONG()," << std::endl;
                        break;

                case MP_BUFFERFIXED:
                        std::cout << "\tNETFIELD_BUFFERFIXED(0x" << std::hex << nf->param << ")," << std::endl;
                        break;

                case MP_BUFFER_VAR_SMALL:
                        std::cout << "\tNETFIELD_BUFFER_VAR_SMALL(0x" << std::hex << nf->param << ")," << std::endl;
                        break;

                case MP_BUFFER_VAR_LARGE:
                        std::cout << "\tNETFIELD_BUFFER_VAR_LARGE(0x" << std::hex << nf->param << ")," << std::endl;
                        break;

                case MP_ARRAYFIXED:
                        std::cout << "\tNETFIELD_ARRAYFIXED(0x" << std::hex << nf->param
                                  << ", " << name << '_' << std::hex << (uint32_t)field << ")," << std::endl;
                        break;

                case MP_ARRAY_VAR_SMALL:
                        std::cout << "\tNETFIELD_ARRAY_VAR_SMALL(0x" << std::hex << nf->param
                                  << ", " << name << '_' << std::hex << (uint32_t)field << ")," << std::endl;
                        break;

                case MP_ARRAY_VAR_LARGE:
                        std::cout << "\tNETFIELD_ARRAY_VAR_LARGE(0x" << std::hex << nf->param
                                  << ", " << name << '_' << std::hex << (uint32_t)field << ")," << std::endl;
                        break;

                case MP_OPTIONAL:
                        std::cout << "\tNETFIELD_OPTIONAL("
                                  << name << '_' << std::hex << (uint32_t)field 
                                  << ")," << std::endl;
                        break;

		case MP_mALIGN_TO_ELEMSIZE:
                        std::cout << "\tNETFIELD_mALIGN_TO_ELEMSIZE(0x" << std::hex << nf->param << ")," << std::endl;
			break;

                default:
                        std::cerr << "Unknown type 0x" << std::hex << nf->type << std::endl;
                        std::cerr << "type    0x" << std::hex << nf->type << std::endl;
                        std::cerr << "unk0    0x" << std::hex << nf->unk0 << std::endl;
                        std::cerr << "parm    0x" << std::hex << nf->param << std::endl;
                        std::cerr << "ref     0x" << std::hex << nf->refTypeDef << std::endl;
                        std::cerr << "defSize 0x" << std::hex << nf->defSize << std::endl;
                        std::cerr << "maxSize 0x" << std::hex << nf->maxSize << std::endl;
                        //exit(1);
                        break;
                }
                nf++;
        }
        std::cout << "\tNETFIELD_END()" << std::endl;
        std::cout << "};" << std::endl;
}

void
Message::innerDisplay(std::string name, struct netField *message)
{
        uint8_t field= 0;
        struct netField *nf= message;

        if (hasSub) {
                while( nf->type != MP_END) {

                        field++;
                        switch(nf->type) {

                        case MP_ARRAYFIXED:
                        case MP_ARRAY_VAR_SMALL:
                        case MP_ARRAY_VAR_LARGE:
                        case MP_OPTIONAL:
                        {
                                uint32_t addr;
                                if (inSection(&secRdata, nf->refTypeDef))
                                        addr= f2v(&secRdata, nf->refTypeDef);
                                else
                                        addr= f2v(&secData, nf->refTypeDef);

                                std::stringstream ss;
                                ss << name << '_' << std::hex << (uint32_t)field;
                                innerDisplay(ss.str(), (struct netField *)addr);
                        }
                        break;


                        default:
                                break;
                        }
                        nf++;
                }
        }

        std::cout << "struct netFieldI " << name << "[] = {" << std::endl;
        decodeFields(name, message);
}

void
Message::display()
{
        innerDisplay(getName(), message);
}


typedef std::multimap<uint32_t*, Message> MessageMap;
typedef std::pair<uint32_t*, Message> MessageEntry;




bool isPush(bfd_byte **p, uint32_t& val)
{
        // push 0x12345678
        if (*p[0] == 0x68) {
                val= *(uint32_t *)(*p+1);
                *p += 5;
                return true;
        }
        // push 0x12
        if (*p[0] == 0x6a) {
                val= (uint32_t)*(uint8_t *)(*p+1);
                *p += 2;
                return true;
        }
        return false;
}

bool isXor(bfd_byte **p)
{
        if (*p[0] == 0x33 &&
            *(*p+2) == 0x33 &&
            *(*p+1) ^ *(*p+3) == 0xd2 ^ 0xc9 ) {
                *p += 4;
                return true;
        }
        return false;
}

bool isXorEDX(bfd_byte **p)
{
        if (*p[0] == 0x33 &&
            *(*p+1) == 0xd2  ) {
                *p += 2;
                return true;
        }
        return false;
}


bool isLeaECX(bfd_byte **p)
{
        if (*p[0] == 0x8D &&
            *(*p+1) == 0x4A 
            && *(*p+2) == 0x03 
) {
                *p += 3;
                return true;
        }
        return false;
}

bool isCall(bfd_byte **p, uint32_t& val)
{
        // call 0x12345678
        if (*p[0] == 0xe8) {
                val= *(uint32_t *)(*p+1);
                val += (uint32_t)(*p - secText.data + bfd_section_vma(tmp_bfd, secText.sec));
                val += 5;
                *p += 5;
                return true;
        }
        return false;
}

bool
isMsgRegister(bfd_byte *p, uint32_t **rcv, uint8_t *rcvCount, uint32_t **snd, uint8_t *sndCount,
        uint32_t **callPtr)
{
        bfd_byte *oldp= p;
        uint32_t p1, p2, p3, p4, dest;

        if (isPush(&p, p1) &&
            isPush(&p, p2) &&
            isPush(&p, p3) &&
            isPush(&p, p4) &&
            isXor(&p)      &&
            isCall(&p, dest))
        {
                *callPtr = (uint32_t *)dest;
                if ( (p1 < 0x1000 || p1 == 0 ) && 
                     (p2 > 0x1000 || p2 == 0 ) && 
                     (p3 < 0x1000 || p3 == 0 ) &&
                     (p4 > 0x1000 || p4 == 0) ) {

                        *rcvCount= (uint8_t)p1;
                        *rcv = (uint32_t *)p2;
                        *sndCount= (uint8_t)p3;
                        *snd = (uint32_t *)p4;

                } else if ( (p1 > 0x1000 || p1 == 0) && 
                            (p2 < 0x1000 || p2 == 0) && 
                            (p3 > 0x1000 || p3 == 0) &&
                            (p4 < 0x1000 || p4 == 0)) {

                        *rcv = (uint32_t *)p1;
                        *rcvCount= (uint8_t)p2;
                        *snd = (uint32_t *)p3;
                        *sndCount= (uint8_t)p4;
                } else
                        return false;

                return true;
        }
        p = oldp;
        if (isPush(&p, p1) &&
            isPush(&p, p2) &&
            isPush(&p, p3) &&
            isXorEDX(&p)      &&
            isPush(&p, p4) &&
            isLeaECX(&p)      &&
            isCall(&p, dest))
        {
                
                *callPtr = (uint32_t *)dest;
                if ( (p1 < 0x1000 || p1 == 0 ) && 
                     (p2 > 0x1000 || p2 == 0 ) && 
                     (p3 < 0x1000 || p3 == 0 ) &&
                     (p4 > 0x1000 || p4 == 0) ) {

                        *rcvCount= (uint8_t)p1;
                        *rcv = (uint32_t *)p2;
                        *sndCount= (uint8_t)p3;
                        *snd = (uint32_t *)p4;

                } else if ( (p1 > 0x1000 || p1 == 0) && 
                            (p2 < 0x1000 || p2 == 0) && 
                            (p3 > 0x1000 || p3 == 0) &&
                            (p4 < 0x1000 || p4 == 0)) {

                        *rcv = (uint32_t *)p1;
                        *rcvCount= (uint8_t)p2;
                        *snd = (uint32_t *)p3;
                        *sndCount= (uint8_t)p4;
                } else
                        return false;

                return true;
        }
        return false;
}


int main(int argc, char *argv[])
{
        bfd *tmp_bfd = NULL;
	bfd_byte *p;
        uint32_t *call;
        uint32_t *rcv;
        uint32_t *snd;
        uint8_t rcvCount;
        uint8_t sndCount;
        uint32_t *scan;
        MessageMap msgs;

	if (argc < 2) {
		printf("Specify filename\n");
		exit(1);
	}
        tmp_bfd = bfd_openr (argv[1], NULL);
        if (tmp_bfd == NULL) {
                printf ("Error openning file");
                exit(1);
        }
        //check if the file is in format
        if (!bfd_check_format (tmp_bfd, bfd_object)) {
                if (bfd_get_error () != bfd_error_file_ambiguously_recognized) {
                        printf("Incompatible format\n");
                        exit(1);
                }
        }

	secText.sec = bfd_get_section_by_name(tmp_bfd, ".text");
	bfd_malloc_and_get_section(tmp_bfd, secText.sec, &secText.data);

	secRdata.sec = bfd_get_section_by_name(tmp_bfd, ".rdata");
        if (!secRdata.sec) {
                printf("Failed to get section\n");
                exit(10);
        }
	if (!bfd_malloc_and_get_section(tmp_bfd, secRdata.sec, &secRdata.data)) {
                printf("Alloc error\n");
                exit(10);
        }


	secData.sec = bfd_get_section_by_name(tmp_bfd, ".data");
        if (!secData.sec) {
                printf("Failed to get section\n");
                exit(10);
        }
	if (!bfd_malloc_and_get_section(tmp_bfd, secData.sec, &secData.data)) {
                printf("Alloc error\n");
                exit(10);
        }
		

/*
  406060:       68 40 6f 0e 01          push   $0x10e6f40
  406065:       6a 06                   push   $0x6
  406067:       68 38 6f 0e 01          push   $0x10e6f38
  40606c:       6a 02                   push   $0x2
  40606e:       33 d2                   xor    %edx,%edx
  406070:       33 c9                   xor    %ecx,%ecx
  406072:       e8 e9 16 5e 00          call   0x9e7760
*/
	for(p= secText.data; p < secText.data+bfd_section_size(tmp_bfd, secText.sec); p++) {

		if ( p< secText.data+bfd_section_size(tmp_bfd, secText.sec)-22 &&
                     isMsgRegister(p, &rcv, &rcvCount, &snd, &sndCount, &call)
                        )
                {
                        uint8_t i;

                        bfd_vma dest;

                        scan= NULL;
                        if (rcvCount) {
                                if (inSection(&secRdata, (bfd_vma)rcv))
                                        rcv= (uint32_t *)f2v(&secRdata, (uint32_t)rcv);
                                else
                                        rcv= (uint32_t *)f2v(&secData, (uint32_t)rcv);
                                scan= rcv;
                        }
                        if (sndCount) {
                                if (inSection(&secRdata, (bfd_vma)snd))
                                        snd= (uint32_t *)f2v(&secRdata, (uint32_t)snd);
                                else
                                        snd= (uint32_t *)f2v(&secData, (uint32_t)snd);
                                scan= snd;
                        }

                        char *text= NULL;
                        char *clitext= NULL;
                        if (scan) {
                                for(uint32_t *cp = scan-0x200; cp < scan; cp++) {
                                        char *p;
                                        if ((p=strstr((char *)cp, ".cpp")) == NULL)
                                                continue;

                                        while(*p != '\\')
                                                p--;

                                        text= &p[1];
					if (strstr(text, "Cli"))
						clitext= text;
                                }
                                if (!text) {
                                        printf("Name missing\n");
                                        exit(1);
                                }

                        }
#if 0
			if (clitext)
				text= clitext;
#endif

                        if (rcvCount) {
                                rcvCount *= 2;

                                for(i=0; i< rcvCount; i++) {

                                        if ( !(i%2) &&
                                             !inSection(&secData, rcv[i]) &&
                                             !inSection(&secRdata, rcv[i])) {
                                                //printf("Not in (r)data section\n");
                                                goto next;
                                        }

                                        if ( (i%2) &&
                                             !inSection(&secText, rcv[i])) {
                                                //printf("Not in text section 0x%p\n", rcv[i]);
                                                goto next;
                                        }

                                }
                                for(i=0; i< rcvCount; i+=2) {

                                        struct netField *nf= (struct netField *)
                                                f2v(&secData, rcv[i]);

                                        msgs.insert( MessageEntry(call, Message(text,nf, true) ));
                                }
                        }

                        if (sndCount) {
                                for(i=0; i< sndCount; i++) {
                                        if ( !inSection(&secData, snd[i]))
                                                goto next;

                                }
                                for(i=0; i< sndCount; i++) {
                                        struct netField *nf= (struct netField *)
                                                f2v(&secData, snd[i]);

                                        msgs.insert( MessageEntry(call, Message(text,nf, false) ));
                                }
                        }


		}
        next:;
	}

        std::cout << "#ifndef PACKETS_H\n"
                  << "#define PACKETS_H\n"
                  << "#include \"netfield.hpp\"\n"
                  << std::endl;

        for(MessageMap::iterator m = msgs.begin();
            m != msgs.end();
            ++m) {
                Message msg = m->second;

                msg.display();
        }

        // Server Messages
        uint32_t *oldcall= NULL;
        for(MessageMap::iterator m = msgs.begin();
            m != msgs.end();
            ++m) {
                uint32_t *call = m->first;
                Message msg = m->second;

                if (!msg.isServer)
                        continue;

                if (oldcall != call) {
                        if (oldcall)
                                std::cout << "\tNULL"
                                        << "};\n\n";
                        std::cout << "struct netFieldI *serverMsgPack_"
                                  << msg.getBasicName() << "[] = {" << std::endl;
                        oldcall = call;
                }
                std::cout << "\t" << msg.getName() << "," << std::endl;
        }
        std::cout << "\tNULL\n"
                  <<"};\n\n" << std::endl;

        // Client Messages
        oldcall= NULL;
        for(MessageMap::iterator m = msgs.begin();
            m != msgs.end();
            ++m) {
                uint32_t *call = m->first;
                Message msg = m->second;

                if (msg.isServer)
                        continue;

                if (oldcall != call) {
                        if (oldcall)
                                std::cout << "\tNULL"
                                        << "};\n\n";
                        std::cout << "struct netFieldI *clientMsgPack_"
                                  << msg.getBasicName() << "[] = {" << std::endl;
                        oldcall = call;
                }
                std::cout << "\t" << msg.getName() << "," << std::endl;
        }
        std::cout << "\tNULL\n"
                  <<"};\n\n"
                  << "#endif" << std::endl;

        return 0;
}
