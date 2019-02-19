
#include "stdafx.h"
#include "Z80.h"

void decodePage(ssh_b* ptr, ssh_b* dst, ssh_b* dstE, int sz, bool packed) {
	if(packed) {
		while(sz > 0 && dst <= dstE) {
			ssh_b b = *ptr++;
			ssh_b c = 1;
			sz--;
			if(b == 0xed) {
				c = *ptr++;
				if(c == 0xed) {
					c = *ptr++;
					b = *ptr++;
					sz -= 3;
				} else {
					ptr--;
					c = 1;
				}
			}
			memset(dst, b, c);
			dst += c;
		}
		if(*(ssh_d*)ptr == 0x00eded00) {
			sz = sz;
		}
	} else {
		memcpy(dst, ptr, sz);
	}
}

bool loadZ80(const zxString& path) {
	ssh_b* pptr = nullptr;
	bool result = false;
	int modify = 0;
	int model = -1;
	ssh_b state1 = 0;
	ssh_b* ptr;
	long sz;

	theApp->pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
		if(_hf != -1) {
			result = true;
			sz = _filelength(_hf);
			pptr = new ssh_b[sz];
			ptr = pptr;
			if(_read(_hf, ptr, sz) != sz) throw(0);
			int version = 1;
			HEAD1_Z80* z80 = (HEAD1_Z80*)ptr;
			ptr += sizeof(HEAD1_Z80);
			sz -= sizeof(HEAD1_Z80);
			HEAD2_Z80* head2 = (HEAD2_Z80*)ptr;
			state1 = z80->STATE1;
			if(state1 == 255) state1 = 1;

			if(z80->PC == 0) {
				// version 2
				ptr += sizeof(HEAD2_Z80);
				version = 2;
				int length = head2->length;
				sz -= length;
				modify = head2->emulateFlags & 128;
				if(length > 23) {
					// version 3
					version = 3;
					length += 2;
					HEAD3_Z80* head3 = (HEAD3_Z80*)ptr;
					ptr += (length - sizeof(HEAD2_Z80));

				}
			}
			if(!modify) {
				if(version == 1) model = MODEL_48K;
				else if(version == 2) {
					switch(head2->hardMode) {
						case 0: case 1: model = MODEL_48K; break;
						case 3: case 4: case 5: case 6: model = MODEL_128K; break;
						case 9: model = MODEL_PENTAGON_128K; break;
						case 10: model = MODEL_SCORPION_256K; break;
					}
				} else if(version == 3) {
					switch(head2->hardMode) {
						case 0: case 1: case 3: model = MODEL_48K; break;
						case 4: case 5: case 6: model = MODEL_128K; break;
						case 9: model = MODEL_PENTAGON_128K; break;
						case 10: model = MODEL_SCORPION_256K; break;
					}
				}
			}
			if(model == -1) throw(0);
			if(!theApp->bus.changeModel(model, *_MODEL)) throw(0);
			if(version > 1) {
				while(sz > 0) {
					ssh_w size = *(ssh_w*)ptr;
					ssh_b* pageS = nullptr;
					int szData = (size == 65535 ? 16384 : size);
					int page = *(ptr + 2);
					switch(model) {
						case MODEL_48K:
							switch(page) {
								case 4: pageS = PAGE_RAM[2]; break;
								case 5: pageS = PAGE_RAM[0]; break;
								case 8: pageS = PAGE_RAM[5]; break;
							}
							break;
						case MODEL_128K:
						case MODEL_PENTAGON_128K:
							if(page >= 3 && page <= 10) pageS = PAGE_RAM[page - 3];
							break;
						case MODEL_SCORPION_256K:
							if(page >= 3 && page <= 18) pageS = PAGE_RAM[page - 3];
							break;
					}
					ptr += 3;
					if(pageS != nullptr) decodePage(ptr, pageS, pageS + 16384, szData, size != 65535);
					ptr += szData;
					sz -= szData;
				}
			} else {
				ssh_b temp[49152];
				memset(temp, 0, sizeof(temp));
				decodePage(ptr, &temp[0], &temp[49151], sz, (state1 & 32) == 32);
				// раскидать по страницам
				memcpy(PAGE_RAM[5], &temp[0], 16384);
				memcpy(PAGE_RAM[2], &temp[16384], 16384);
				memcpy(PAGE_RAM[0], &temp[32768], 16384);
			}
			*_AF = z80->AF;
			*_IX = z80->IX; *_IY = z80->IY;
			*_BC = z80->BC; *_DE = z80->DE; *_HL = z80->HL; *_SP = z80->SP;
			*_I = z80->I; *_R = z80->R; *_IM = z80->STATE2 & 3; *_IFF1 = z80->IFF1; *_IFF2 = z80->IFF2;
			memcpy(&cpuZX[RC_], &z80->BC_, 8);
			(*_R) |= (state1 & 1) << 7;
			(*_PORT_FE) &= ~31;
			(*_PORT_FE) |= (state1 & 14) >> 1;
			if(version == 1) *_PC = z80->PC;
			else *_PC = head2->PC;
			if(*_TSTATE & ZX_128K) theApp->bus.writePort(0xfd, 0x7f, head2->hardState);
		}
	} catch(...) { result = false; }

	SAFE_CLOSE1(_hf);
	SAFE_DELETE(pptr);

	theApp->pauseCPU(false);

	return result;
}

bool saveZ80(const zxString& path) {
	ssh_b* pptr = nullptr;
	bool result = false;

	theApp->pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			result = true;
			HEAD1_Z80 z80;
			z80.AF = *_AF; z80.IX = *_IX; z80.IY = *_IY;
			z80.BC = *_BC; z80.DE = *_DE; z80.HL = *_HL; z80.SP = *_SP; z80.PC = *_PC;
			z80.I = *_I; z80.IFF1 = *_IFF1; z80.IFF2 = *_IFF2; z80.STATE2 = *_IM; z80.R = *_R;
			memcpy(&z80.BC_, &cpuZX[RC_], 8);
			z80.STATE1 = ((*_R) >> 7) | (((*_PORT_FE) & 7) << 1) | 32;
			if(_write(_hf, &z80, sizeof(HEAD1_Z80)) != sizeof(HEAD1_Z80)) throw(0);
			/*
			// RAM 48k
			ssh_b* src = &memZX[16384];
			ssh_b* srcE = src + 49152;

			pptr = new ssh_b[65535];

			ssh_b* dst = pptr;
			ssh_b b1;
			int c = 1;
			while(src < srcE) {
				b1 = *src++;
				if(b1 != *src) {
					if(c > 1) {
						if(c < 5 && b1 != 0xed) {
							memset(dst, b1, c);
							dst += c;
						} else {
							*dst++ = 0xed;
							*dst++ = 0xed;
							*dst++ = c;
							*dst++ = b1;
						}
					} else {
						*dst++ = b1;
					}
				} else {
					c++;
					if(c < 256) continue;
					*dst++ = 0xed;
					*dst++ = 0xed;
					*dst++ = c - 1;
					*dst++ = b1;
				}
				c = 1;
			}
			c--;
			if(c > 1) {
				*dst++ = 0xed;
				*dst++ = 0xed;
				*dst++ = c;
				*dst++ = b1;
			} else if(c == 1) *dst++ = b1;
			ssh_d sz = (ssh_d)(dst - pptr);
			if(_write(_hf, pptr, sz) != sz) throw(0);
			*/
		}
	} catch(...) { result = false; }
	
	SAFE_CLOSE1(_hf);
	SAFE_DELETE(pptr);

	theApp->pauseCPU(false);

	return result;
}

/*
void SaveZ80(char * path)
{
int fil;
char message[400];
int start;
unsigned short b_len=65535;

unsigned char buffer[200000]={0};

fil=io_creat(NULL, path);

if (fil==-1)
{
sprintf(message,"Error creating %s", path);
MessageBoxErr(message);
return;
}

buffer[0]=A;
buffer[1]=F;
buffer[2]=C;
buffer[3]=B;
buffer[4]=L;

buffer[5]=H;
memset(&buffer[6],0,2);
memcpy(&buffer[8],&SP,2);

#if defined BIGENDIAN
byteswp (&buffer[8]);
#endif

buffer[10]=I;

buffer[13]=E;
buffer[14]=D;
buffer[15]=C1;
buffer[16]=B1;
buffer[17]=E1;
buffer[18]=D1;
buffer[19]=L1;
buffer[20]=H1;
buffer[21]=A1;
buffer[22]=F1;

memcpy(&buffer[25],&IX,2);
memcpy(&buffer[23],&IY,2);


#if defined BIGENDIAN
byteswp (&buffer[23]);
byteswp (&buffer[25]);
#endif

buffer[27]=interrupts;
buffer[29]=IM;
buffer[12]=border*2;

buffer[30]=54;
memcpy(&buffer[32], &PC, 2);

#if defined BIGENDIAN
byteswp (&buffer[32]);
#endif

buffer[38]=register_latch;
memcpy(&buffer[39],Regs,15);

start=86;

if ((!io_strcasecmp(Mode,"128"))||(!io_strcasecmp(Mode,"Pentagon")))
{
buffer[34]=4;
buffer[35]=last7ffdout;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=3;
memcpy(&buffer[start+3],RAM0,16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=4;
memcpy(&buffer[start+3],RAM1,16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=5;
memcpy(&buffer[start+3],RAM2,16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);

buffer[start+2]=6;
memcpy(&buffer[start+3],RAM3,16384);
start+=16387;


memcpy(&buffer[start],&b_len,2);
buffer[start+2]=7;

memcpy(&buffer[start+3],RAM4,16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=8;
memcpy(&buffer[start+3],RAM5,16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=9;
memcpy(&buffer[start+3],RAM6,16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=10;
memcpy(&buffer[start+3],RAM7,16384);
start+=16387;
}

if (!io_strcasecmp(Mode,"48"))
{
memcpy(&buffer[start],&b_len,2);
buffer[start+2]=4;
memcpy(&buffer[start+3],&CPU12[32768],16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=5;
memcpy(&buffer[start+3],GiveAddrW1(49152),16384);
start+=16387;

memcpy(&buffer[start],&b_len,2);
buffer[start+2]=8;
memcpy(&buffer[start+3],&CPU12[16384],16384);
start+=16387;
}

io_write(fil, buffer, start);
io_close(fil);
}

int Loadz80(char * filename, char * startd = NULL, int pentagon = 0)
{
int fil;
unsigned char buffer[200000];
int len;

int start=0;
unsigned short b_len;
unsigned char n;
unsigned char v=0;

unsigned char page[16384];
unsigned char _48k[49152];
int j;


//printf ("%d \n", pentagon);

fil=io_open(startd, filename,O_RDONLY);

if (fil==-1)
{
sprintf(tstring,"Unable to open %s", filename);
MessageBoxErr(tstring);

return(errors);
}

len=io_read(fil,buffer,sizeof(buffer));

A=buffer[0];
F=buffer[1];
C=buffer[2];
B=buffer[3];
L=buffer[4];
H=buffer[5];

*(short*)&PC=*(short*)&buffer[6];
*(short*)&SP=*(short*)&buffer[8];

#if defined BIGENDIAN
byteswp((unsigned char *)&PC);
byteswp((unsigned char *)&SP);
#endif

I=buffer[10];
if (buffer[12]==255)buffer[12]=1;
border=(buffer[12]&0xe)>>1;
E=buffer[13];
D=buffer[14];
C1=buffer[15];
B1=buffer[16];
E1=buffer[17];
D1=buffer[18];
L1=buffer[19];
H1=buffer[20];
A1=buffer[21];
F1=buffer[22];

*(short*)&IX=*(short*)&buffer[25];
*(short*)&IY=*(short*)&buffer[23];

#if defined BIGENDIAN
byteswp((unsigned char *)&IX);
byteswp((unsigned char *)&IY);
#endif

if (buffer[27]==0)interrupts=off; else interrupts=on;
IM=buffer[29]&3;

if (PC!=0)
{
#if !defined log
printf(".z80 v1\n");
#endif

MapROM1();
Set48Screen();
Set48kMode();
Load48ROMs();
Zamolchi();


if ((buffer[12]&32)!=0)
Decompress48k(&buffer[30],_48k);
else

memcpy(_48k,&buffer[30],49152);

memcpy(&CPU12[16384],_48k,32768);
memcpy(GiveAddrW1(49152),&_48k[32768],16384);

goto LE;
}
else
{
if (buffer[30]==23)
{
v=2;
goto LPCL;
}

if (buffer[30]==54)
{
v=3;
goto LPCL;
}

if (buffer[30]==55)
{
v=4;
goto LPCL;
}


sprintf(tstring,"Loadz80 : Unknown file format : buffer[30] value unknown.");
MessageBoxErr(tstring);
goto LE;


LPCL:

*(short*)&PC=*(short*)&buffer[32];

#if defined BIGENDIAN
byteswp((unsigned char *)&PC);
#endif

register_latch=buffer[38];
for (j=0; j<15; j++)
AYio_writeRegNV(0,buffer[39+j]);


if (v==2)
{
if (buffer[34]==2)buffer[34]=3;
if (buffer[34]>=3)buffer[34]=4;
}

switch (buffer[34])
{
case 0:
case 1:
case 2:
{
if (v==2)
{
#if !defined log
printf(".z80 v2 48k\n");
#endif
start=55;
}

if (v==3)
{
#if !defined log
printf(".z80 v3 48k\n");
#endif
start=86;
}

if (v==4)
{
#if !defined log
printf(".z80 v3.X 48k\n");
#endif
start=87;
}

MapROM1();
Set48Screen();
Set48kMode();
Load48ROMs();
Zamolchi();


LD48:
*(short*)&b_len=*(short*)&buffer[start];

#if defined BIGENDIAN
byteswp((unsigned char *) &b_len);
#endif

n=buffer[start+2];
DecompressPage(&buffer[start+3], b_len, page);

switch(n)
{
case 4 : memcpy(GiveAddrW1(32768),page,16384); break;
case 5 : memcpy(GiveAddrW1(49152),page,16384);  break;

case 8 : memcpy(GiveAddrW1(16384),page,16384); break;

default:
{
sprintf(tstring,"Loadz80 : Unknown file format : page number %d unknown",n);
MessageBoxErr(tstring);
}
}

if (b_len==65535) b_len=16384;
if((start+b_len+3)<len)
{
start+=(b_len+3);
goto LD48;
}

break;
}

case 4:
case 5:
case 6:
{
Set128kMode();
Load128ROMs();

Port7ffdOut(buffer[35]);
PortbffdOut(buffer[38]);
memcpy(Regs,&buffer[39],16);

Zamolchi();


if (v==2)
{
#if !defined log
printf(".z80 v2 128k\n");
#endif
start=55;
}

if (v==3)
{
#if !defined log
printf(".z80 v3 128k\n");
#endif
start=86;
}

if (v==4)
{
#if !defined log
printf(".z80 v3.X 128k\n");
#endif
start=87;
}

LD:
*(short*)&b_len=*(short*)&buffer[start];

#if defined BIGENDIAN
byteswp((unsigned char *) &b_len);
#endif

n=buffer[start+2];
DecompressPage(&buffer[start+3],b_len,page);

switch(n)
{
case 3 : memcpy(RAM0,page,16384); break;
case 4 : memcpy(RAM1,page,16384); break;
case 5 : memcpy(RAM2,page,16384); break;
case 6 : memcpy(RAM3,page,16384); break;
case 7 : memcpy(RAM4,page,16384); break;
case 8 : memcpy(RAM5,page,16384); break;
case 9 : memcpy(RAM6,page,16384); break;
case 10: memcpy(RAM7,page,16384); break;

default:
{
sprintf(tstring,"Loadz80 : Unknown file format : page number %d unknown",n);
MessageBoxErr(tstring);
}
}

if (b_len==65535) b_len=16384;
if((start+b_len+3)<len)
{
start+=(b_len+3);

goto LD;
}

break;
}

case 3 :
{
sprintf(tstring,"SamRam not supported.");
MessageBoxErr(tstring);
goto LE;
}

default :
{
sprintf(tstring,"Loadz80 : Unknown file format : unknown machine type %d",buffer[34]);
MessageBoxErr(tstring);
goto LE;
}
}

goto LE;
}

LE:

io_close(fil);

if (mode==m_128k)
{
if (!pentagon)
strcpy(Mode,"128");
else
strcpy(Mode,"Pentagon");
}
else
{
if (!pentagon)
strcpy(Mode,"48");
else
strcpy(Mode,"Pentagon");
}
Set_z80times_per_line(35);

return(no_errors);
}
*/