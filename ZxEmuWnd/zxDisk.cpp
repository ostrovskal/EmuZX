
#include "stdafx.h"
#include "zxDisk.h"

zxDisk::zxDisk() : done(0), rlen(0), rpos(0), wpos(0), wlen(0) { 
}

void zxDisk::writePort(ssh_b A0A7, ssh_b val) {
	switch(A0A7) {
		// регистр команд КР1818ВГ93
		case 0x1f: if(curDisk().tp == DISK_TRD) writePortTRD(val); break;
		// регистр дорожки КР1818ВГ93
		case 0x3f: track = val; break;
		// регистр сектора КР1818ВГ93
		case 0x5f: sector = val; break;
		// регистр данных КР1818ВГ93
		case 0x7f: data = val; writeData(val); break;
		// системный порт BDI
		case 0xff: out = val; break;
	}
}

ssh_b zxDisk::readPort(ssh_b A0A7) {
   if(spin) {
	   rec.bit.b1 = 0;
	   if(!isNone()) {
		   ftime(&tmb);
		   ssh_w i = tmb.millitm /= 100;
		   ssh_b d = tmb.millitm %= 100;
		   rec.bit.b1 = ((!(i & 1)) && d <= 20);
	   }
   }
   switch(A0A7) {
		// комманда
		case 0x1f: return isNone() ? 0x80 : rec.byte;
		// дорожка
		case 0x3f: return track;
		// сектор
		case 0x5f: return sector;
		// данные
		case 0x7f: if(curDisk().tp == DISK_TRD) return readDataTRD();
		// управление
		case 0xff: return (in & (curTime() < done) ? ~0x40 : 0xff);
   }
   return 0xff;
}

ssh_b zxDisk::readDataTRD() {
	if(rpos >= rlen) return data;
	data = read[rpos];
	if(rpos > 255) sector++;
	if(sector > 16) sector = 1;
	rpos++;
	if(rpos == rlen) {
		in = 0x80;
		rec.byte = 0;
	} else {
		in = 0x40;
		rec.bit.b0 = 1;
		rec.bit.b1 = 1;
	}
	return data;
}

void zxDisk::writeData(ssh_b data) {
	if(wlen == 0) return;
	zxFile f;
	if(!f.open(curDisk().name, false)) return;
	if(f.setPos(wpos, SEEK_SET)) {
		f.write(&data, 1);
		wlen--; wpos++;
		if(wlen == 0) { in = 0x80; rec.byte = 0; } else in = 0x40;
	} else wlen = 0;
}

void zxDisk::seekDelay(ssh_b dst_track) {
	auto trk = &curDisk().trk;
	if((dst_track - *trk) > 0) dir = 1;
	if((dst_track - *trk) < 0) dir = 0;
	done = curTime() + 20 * abs(dst_track - *trk);
	in = (isNone() ? 0x80 : 0x40);// ?? (0x40 -> 0x80)
	*trk = dst_track;
	track = dst_track;
}

void zxDisk::setFlagsSeeks() {
	auto dsk = curDisk();
	rec.bit.b0 = 0;
	rec.bit.b2 = dsk.trk == 0;
	rec.bit.b3 = 0;
	rec.bit.b4 = 0;
	rec.bit.b6 = dsk.ro;
	rec.bit.b7 = isNone();
}

void zxDisk::writePortTRD(ssh_b val) {
	auto dsk = curDisk();

	int side = (out & 0x10) ? 0 : 1;

	if((val & 0xF0) == 0xD0) {
		 // interrupt
		in = 0x80;
		setFlagsSeeks();
		return;
	}
	if((val & 0xF0) == 0x00) {
		// seek track 0
		seekDelay(0);
		rec.bit.b5 = val & 8;
		setFlagsSeeks();
		if(val & 8) spin = 1;
		return;
	}
	if((val & 0xF0) == 0x10) {
		// seek track
		seekDelay(data);
		rec.bit.b5 = val & 8;
		setFlagsSeeks();
		if(val & 8) spin = 1;
		return;
	}
	if((val & 0xE0) == 0x40) {
		// fwd
		dir = 1;
		// step
		val = 0x20;
	}
	if((val & 0xE0) == 0x60) {
		// back
		dir = 0;
		// step
		val = 0x20; 
	}
	if((val & 0xE0) == 0x20) {
		// step
		seekDelay(dir ? ++dsk.trk : --dsk.trk);
		rec.bit.b5 = 1;
		setFlagsSeeks();
		spin = 1;
		return;
	}
	if((val & 0xE0) == 0x80) {
		// read sector
		rec.byte = 0x81;
		in = 0x40;
		spin = 0;
		if(isNone()) {
			rec.byte = 0x90;
			in = 0x80;
			//No disk in drive %c\n", 'A' + CurrentDiscNum);
			return;
		}
		if(sector == 0 || sector > 16) {
			// sector not found
			rec.byte |= 0x10; 
			in = 0x80;
			return;
		}
		zxFile f;
		if(!f.open(dsk.name, true)) { rec.byte = 0x90; in = 0x80; return; }
		int pointer = (dsk.trk * 2 + side) * 256 * 16 + (sector - 1) * 256;
		if(!f.setPos(pointer, SEEK_SET)) { /* sector not found */ rec.byte |= 0x10; in = 0x80; return; }
		if(val & 0x10) {
			f.read(buffer, 256 * (17 - sector));
			f.setPos(-256 * ((17 - sector) + (sector - 1)), SEEK_CUR);
			f.read(buffer, 256 * (sector - 1));
			read = buffer;
			rlen = 256 * 16;
			rpos = 0;
		} else {
			if(f.read(buffer, 256)) {
				read = buffer;
				rlen = 256;
				rpos = 0;
				done = curTime() + 30;
			} else {
				// sector not found
				rec.byte |= 0x10;
				in = 0x80;
			}
		}
		return;
	}
	if((val & 0xFB) == 0xC0) {
		// read address
		rec.byte = 0x81;
		in = 0x40;
		six[0] = dsk.trk;
		six[1] = 0;
		six[2] = sector;
		six[3] = 1;
		// crc
		six[4] = 0;
		// crc
		six[5] = 0;
		read = six;
		rlen = 6;
		rpos = 0;
		done = curTime() + 30;
		spin = 0;
		return;
	}
	if((val & 0xE0) == 0xA0) {
		// write sector
		rec.byte = 0x81;
		in = 0x40;
		spin = 0;
		if(dsk.ro) {
			rec.byte = 0x60;
			in = 0x80;
			return;
		}
		if(isNone()) {
			rec.byte = 0x90;
			in = 0x80;
			// no disk
			return;
		}
		if(sector == 0 || sector > 16) {
			// sector not found
			rec.byte |= 0x10; 
			in = 0x80;
			return;
		}
		wpos = (dsk.trk * 2 + side) * 256 * 16 + (sector - 1) * 256;
		wlen = 256;
		spin = 0;
	}
}

bool zxDisk::selectTRD(ssh_cws path, int slot) {
	/*
	int fil;
	unsigned char * temp;


	int messagebox = 0;

	if (!io_strncasecmp(arg, "none", 4))
	{
		//printf("%s \n", arg);

		strcpy(discs[num].filename, arg);
		return;
	}

	if (!FileExists(arg))
	{
		sprintf(tstring,"%s - No such file. Creating it. (%d)", arg, num);

		if (messagebox)
			MessageBoxInfo(tstring);
		else
			printf("%s\n",tstring);


		fil=io_creat(NULL, arg);

		if(fil==-1)
		{
			strcpy(discs[num].filename,"none");

			sprintf(tstring, "Can't create %s. You must be able io_read and io_write this directory.", arg);

			if (messagebox)
				MessageBoxErr(tstring);
			else
				printf("%s\n",tstring);

			return;
		}

		temp= (unsigned char *)malloc(655360);
		bzero(temp,655360);

		io_write(fil, temp, 655360);
		free(temp);

		io_close(fil);
	}


	fil=io_open(NULL, arg, O_RDWR);

	if(fil==-1)
	{
		io_close (fil);
		fil=io_open(NULL, arg, O_RDONLY);

		if (fil==-1)
		{
			strcpy(discs[num].filename, "none");
			sprintf(tstring,"Can't io_open %s.", arg);

			if (messagebox)
				MessageBoxErr(tstring);
			else
				printf("%s\n",tstring);

			return;
		}

		discs[num].ro=1;

		if (messagebox)
		{
			switch (num)
			{
			case 0:
				//XtVaSetValues(disk1ro,XmNset,1,NULL);
				break;
			case 1:
				//XtVaSetValues(disk2ro,XmNset,1,NULL);
				break;
			case 2:
				//XtVaSetValues(disk3ro,XmNset,1,NULL);
				break;
			case 3:
				//XtVaSetValues(disk4ro,XmNset,1,NULL);
				break;
			default: ;
				//ExitWithError(-1,"SelectTRDFile unknown value of num. This is bug. Report it.");
			}
		}

	}
	else
	{
		discs[num].ro=0;

		if (messagebox)
		{
			switch (num)
			{
			case 0:
				//XtVaSetValues(disk1ro,XmNset,0,NULL);
				break;
			case 1:
				//XtVaSetValues(disk2ro,XmNset,0,NULL);
				break;
			case 2:
				//XtVaSetValues(disk3ro,XmNset,0,NULL);
				break;
			case 3:
				//XtVaSetValues(disk4ro,XmNset,0,NULL);
				break;
			default: ;
				//ExitWithError(-1,"SelectTRDFile unknown value of num. This is bug. Report it.");
			}
		}
	}

	strcpy(discs[num].filename,arg);
	discs[num].type=0;
	io_close(fil);
	*/
	return false;
}
