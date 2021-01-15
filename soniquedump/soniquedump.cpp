#include <windows.h>
#include <fstream>
#include "vis.h"

using namespace std;

ofstream waveform,spectrum;

void Initialize()
{
	waveform.open("waveform.raw",ios::binary);
	spectrum.open("spectrum.raw",ios::binary);
}

BOOL Render(unsigned long *Video, int width, int height, int pitch, VisData* pVD) {
	for (int i=0;i<512;i++) {
		waveform.write((const char*)&(pVD->Waveform[0][i]),2);
	}

	for (int i=0;i<256;i++) {
		spectrum.write((const char*)&(pVD->Spectrum[1][i]),2);
	}
	return true;
}

BOOL SaveSettings(char* FileName) {
	return true;
}

BOOL OpenSettings(char* FileName) {
	return true;
}

BOOL Deinit() {
	waveform.close();
	spectrum.close();
	return true;
}
BOOL Clicked(int x, int y, int buttons) {
	return true;
}

BOOL ReceiveQueryInterface(QueryInterface* Interface) {
	return true;
}

char name[]="soniquedump";
VisInfo vi={1,name,VI_SPECTRUM|VI_WAVEFORM,Initialize,Render,SaveSettings,OpenSettings,Deinit,Clicked,ReceiveQueryInterface};

DLLEXPORT VisInfo* QueryModule(void) {
	return &vi;
}

