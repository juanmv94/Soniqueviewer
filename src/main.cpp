#include <windows.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "vis.h"

//#define MIN(a,b) ((a<b)?a:b)
//#define MAX(a,b) ((a>b)?a:b)

typedef VisInfo* (*f_funci)();

using namespace std;

void customzoomandblur(unsigned long *v,unsigned long *vt,int XS,int YS) {
	const float zoom=0.8;
	unsigned long *vtp=vt;
	//zoom
	float centerx=XS/2;
	float centery=YS/2;
	for (int y=0;y<YS;y++) {
		for (int x=0;x<XS;x++) {
			int origx=(int)((x-centerx)*zoom+centerx);
			int origy=(int)((y-centery)*zoom+centery);
			//*(vtp++)=v[MAX(MIN(origx,XS-1),0)+MAX(MIN(origy,YS-1),0)*XS];	//Safe
			*(vtp++)=v[origx+origy*XS];	//Unsafe (zoom<1)
		}
	}
	//blur and darkness
	const int s=3;
	const int darkness=24;
	for (int y=0;y<YS;y++) {
		for (int x=0;x<XS;x++) {
			if (y==0 || x==0 || x==(XS-1) || y==(YS-1)) {
				v[x+XS*y]=vt[x+XS*y];
				continue;
			}
			int t[4]={0};
			vtp=vt+XS*(y-s/2)+(x-s/2);
			for (int i=0;i<s;i++) {
				for (int j=0;j<s;j++) {
					for (int k=0;k<4;k++) {
						t[k]+=((unsigned char*)vtp)[k];
					}
					vtp++;
				}
				vtp+=XS-s;
			}
			for (int k=0;k<4;k++) {
				((unsigned char*)(&v[x+XS*y]))[k]=max(t[k]/(s*s)-darkness,0);
			}
		}
	}
	//memmove(v,vt,XS*YS*sizeof(unsigned long));
}


int main(int argc, char ** argv)
{
  //parse parameters
  bool correctparameters=argc>1;
  char defaultvis[]="vis.ini";
  char * customvis=nullptr;
  char defaultwaveform[]="defaultwaveform.raw";
  char * customwaveform=nullptr;
  char defaultspectrum[]="defaultspectrum.raw";
  char * customspectrum=nullptr;
  int XS=640;
  int YS=400;
  int delay=40;
  bool zoomandblur=true;
  
  for (int i=2;i<argc;i+=2) {
	  if (argv[i][0]!='-' || strlen(argv[i])!=2) {correctparameters=false; break;}
	  if ((i+1)==argc) {correctparameters=false; break;}
	  switch(argv[i][1]) {
		  case 'r': {
			  char * x=strchr(argv[i+1], 'x');
			  if (x==nullptr) {correctparameters=false; break;}
			  *x=0;
			  XS=atoi(argv[i+1]);
			  YS=atoi(x+1);
			  break;
		  }
		  case 'f': {
			  delay=1000/atoi(argv[i+1]);
			  break;
		  }
		  case 'w': {
			  customwaveform=argv[i+1];
			  break;
		  }
		  case 's': {
			  customspectrum=argv[i+1];
			  break;
		  }
		  case 'v': {
			  customvis=argv[i+1];
			  break;
		  }
		  case 'z': {
			  zoomandblur=(strlen(argv[i+1])==1 && argv[i+1][0]=='1');
			  break;
		  }
		  default: {
			correctparameters=false;
		  }
	  }
	  if (!correctparameters) break;
  }
  if (!correctparameters) {
	  cout<<"Wrong arguments. Usage example: " << std::endl << argv[0] << " Spec_n_Hopp.svp [-r 640x400][-w defaultwaveform.raw][-s defaultspectrum.raw][-v vis.ini][-f 25][-z 1]" << std::endl;
	  return EXIT_FAILURE;
  }
	
  HINSTANCE hGetProcIDDLL = LoadLibrary(argv[1]);

  if (!hGetProcIDDLL) {
    cout << "Could not load the svp file" << std::endl;
    return EXIT_FAILURE;
  }

  // resolve function address here
  f_funci funci = (f_funci)GetProcAddress(hGetProcIDDLL, "QueryModule");
  if (!funci) {
    cout << "Wrong svp file" << std::endl;
    return EXIT_FAILURE;
  }
  
  unsigned long *v=new unsigned long[XS*YS];	//video buffer
  unsigned long *vt;							//Temporal video buffer for customzoomandblur
  unsigned char mousebuttons=0xFF;
  
  VisInfo* vi=funci();
  cout << "----- " << vi->PluginName << " -----" << std::endl << std::endl;
  ifstream waveform,spectrum;
  char *memwaveform, *memspectrum;
  unsigned long waveformsize,spectrumsize;
  unsigned long waveformcur=0,spectrumcur=0;
  if (vi->lRequired & VI_WAVEFORM) {
	  cout << "This efect requires waveform"<< std::endl;
	  waveform.open(customwaveform ? customwaveform : defaultwaveform, ios::binary);
	  if (!waveform.is_open()) {
		  cout << "Could not locate waveform file" << std::endl;
		  return EXIT_FAILURE;
	  }
	  cout << "Loading waveform file..." << std::endl;
	  waveform.seekg(0, ios::end);
	  waveformsize=waveform.tellg();
	  memwaveform=new char[waveformsize];
	  waveform.seekg(0, ios::beg);
	  waveform.read(memwaveform,waveformsize);
  }
  if (vi->lRequired & VI_SPECTRUM) {
	  cout << "This efect requires spectrum"<< std::endl;
	  spectrum.open(customspectrum ? customspectrum : defaultspectrum, ios::binary);
	  if (!spectrum.is_open()) {
		  cout << "Could not locate spectrum file" << std::endl;
		  return EXIT_FAILURE;
	  }
	  cout << "Loading spectrum file..." << std::endl;
	  spectrum.seekg(0, ios::end);
	  spectrumsize=spectrum.tellg();
	  memspectrum=new char[spectrumsize];
	  spectrum.seekg(0, ios::beg);
	  spectrum.read(memspectrum,spectrumsize);
  }
  if (vi->lRequired & SONIQUEVISPROC) {
	  if (zoomandblur) {
		cout << "This efect enables native Sonique blur and zoom efect and will be simulated" << std::endl;
		vt=new unsigned long[XS*YS];
	  }
	  else {
		  cout << "This efect enables native Sonique blur and zoom efect and will NOT be simulated" << std::endl;
	  }
  }
  
  if (vi->Version > 0) cout << "This efect has mouse support" << std::endl;
  
  if (SDL_Init(SDL_INIT_VIDEO)) {
    cout << "could not init SDL" << std::endl;
    return EXIT_FAILURE;
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
  SDL_Window * SDLwindow = SDL_CreateWindow(vi->PluginName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, XS, YS, 0);
  SDL_Renderer * renderer = SDL_CreateRenderer(SDLwindow, -1, 0);
  SDL_Texture* bitmap_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, XS, YS);
  //SDL_SetTextureBlendMode( bitmap_tex, SDL_BLENDMODE_BLEND );
  int bitmap_pitch=XS*sizeof(unsigned long);
    
  //vi->ReceiveQueryInterface(NULL);
  vi->OpenSettings(customvis ? customvis : defaultvis);
  vi->Initialize();
  VisData *vd= new VisData();
  unsigned int lastticks=0;
  while(1) {
	  vd->MillSec=lastticks;
	  
	  if (vi->lRequired & VI_WAVEFORM) {
		  if ((waveformcur+2*512)>waveformsize) waveformcur=0;
		  for (int i=0;i<512;i++) {
			vd->Waveform[0][i]=memwaveform[waveformcur++];
			waveformcur++;
			vd->Waveform[1][i]=memwaveform[waveformcur++];
		  }
	  }
	  
	  if (vi->lRequired & VI_SPECTRUM) {
		  if ((spectrumcur+2*256)>spectrumsize) spectrumcur=0;
		  for (int i=0;i<256;i++) {
			vd->Spectrum[0][i]=memspectrum[spectrumcur++];
			vd->Spectrum[1][i]=memspectrum[spectrumcur++];
		  }
	  }

	  if (vi->lRequired & SONIQUEVISPROC) {
		  if (zoomandblur) customzoomandblur(v,vt,XS,YS);
		  else for (int j=0;j<(XS*YS);j++) v[j]=0;
	  }
	  vi->Render(v, XS, YS, XS, vd);
	  //for (int j=0;j<(XS*YS);j++) v[j]|=ALPHA_MASK;
	  
	  unsigned int newticks=SDL_GetTicks();
	  int del=delay-(int)(newticks-lastticks);
	  if (del>0) SDL_Delay(del);
	  lastticks=newticks;
	  
	  SDL_UpdateTexture(bitmap_tex,NULL,v,bitmap_pitch);
      SDL_RenderCopy(renderer, bitmap_tex, NULL, NULL);
	  SDL_RenderPresent(renderer);
	  
	  //Events
	  SDL_Event event;
	  SDL_PollEvent(&event);
	  if ((event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_CLOSE) ||
            (event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE)) {
        break;
      }
	  else if (event.type==SDL_KEYDOWN && event.key.keysym.sym==SDLK_RETURN) {
		  bool isFullScreen=SDL_GetWindowFlags(SDLwindow) & SDL_WINDOW_FULLSCREEN;
		  SDL_SetWindowFullscreen(SDLwindow, isFullScreen ? 0 : SDL_WINDOW_FULLSCREEN);
	  }
	  else if (event.type==SDL_MOUSEBUTTONDOWN && vi->Version>0) {
		  int x, y;
          SDL_GetMouseState( &x, &y );
		  switch (event.button.button&mousebuttons) {
			  case SDL_BUTTON_LEFT:
			  vi->Clicked(x,y,1);
			  break;
			  case SDL_BUTTON_RIGHT:
			  vi->Clicked(x,y,2);
			  break;
		  }
		  mousebuttons&=(0xFF-event.button.button);
	  }
	  else if (event.type==SDL_MOUSEBUTTONUP && vi->Version>0) {
		  mousebuttons|=event.button.button;
	  }
	  
  }
  if (vi->Version > 0) vi->Deinit();	//It seems that Deinit() first comes with version 1 efects
  delete[] v;
  if ((vi->lRequired & SONIQUEVISPROC) && zoomandblur) delete[] vt;
  
  SDL_DestroyTexture(bitmap_tex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(SDLwindow);
  SDL_Quit();
  return EXIT_SUCCESS;
}