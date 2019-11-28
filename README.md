# Soniqueviewer

## what is this ~~sh\*t~~ amazing project?
Maybe some of you remember [Sonique](https://en.wikipedia.org/wiki/Sonique_(media_player)), the media player released by Lycos at the start of the century. That media player was mostly known for their amazing visual efects that react with music, and for being a nice alternative to Winamp.
![Sonique](https://upload.wikimedia.org/wikipedia/en/thumb/d/d4/Media_player_sonique-1.95.png/300px-Media_player_sonique-1.95.png)

These visual efects were made by third parties as *\*.svp* files and could be downloaded from [Sonique's website](https://web.archive.org/web/20011201225519/http://sonique.lycos.com/customize/archive.php?type=plugin) as *.exe files that extract this plugins into Sonique instalation folder.

![Sonique](https://www.downloadsource.net/upload/files/gallery/old/90/9/0/d7db9edcbb803a9f88aceaf5cc5c3b40.jpg)

Unfortunately (or not), Sonique years of glory ended time ago, and Sonique visuals were forgotten... Until now...

![Soniqueviewer](https://github.com/juanmv94/Soniqueviewer/raw/master/screenshots/specnhopp.png)

*Soniqueviewer* allows you to run these *\*.svp* files without needing to install Sonique using SDL library. It also allows you to set bigger resolutions than allowed by Sonique, and to set custom framerate.

## Compile
This project is compiled with MINGW and the SDL library using the *make.bat* script. Should work with MSVC too.

## Parameters
You must call soniqueviewer the following way:

**soniqueviewer.exe Spec_n_Hopp.svp [-r 640x400][-w defaultwaveform.raw][-s defaultspectrum.raw][-v vis.ini][-f 25][-z 1]**

The SVP file must be the first argument, the following are optional and default values are shown.

* **-r**: use this to change the resolution
* **-w**: use this to use custom waveform file
* **-s**: use this to use custom spectrum file
* **-v**: use this to use custom vis.ini file
* **-f**: limit the framerate
* **-z**: use (1) or not (0) the soniqueviewer zoom and blur function when needed.

## Controls
Use ESC key to exit simulation, and ENTER key to toggle fullscreen

## The SVP file
A sonique SVP file consists of a DLL library with a single function *QueryModule()* that return a struct with efect info and callback function pointers. This struct definition can be found in **src/vis.h**.
There are 2 diferent versions of SVP files, that differ in *VisInfo.Version* 0 or 1. Version 1 adds mouse click support and callbacks *Deinit*, *Clicked*, and *ReceiveQueryInterface*.

## Waveform and Spectrum dump
Most of these efects need the Waveforms and Spectrums of the playing song to work. Soniqueviewer uses Waveform and Spectrum files for this purpose. Default Waveform and Spectrum dump from song "Da Edge - Believe" are provided in the repository to make Soniqueviewer work with these efects. There's also a waveform and spectrum dump plugin and source in the folder *soniquedump*.

## Sonique zoom and blur
flag *SONIQUEVISPROC* tells when Sonique native zoom and blur efect should be enabled. This makes Sonique to alter the video buffer on each frame zooming it and bluring it, so then the SVP *Render* callback writes over this altered video buffer.
Since this is a native Sonique feature, I had 2 options: decompile Sonique or make a custom zoom and blur function. I ended making myself a *customzoomandblur()* function with very similar results to the Sonique's native feature.
It's enabled by default, but can be disabled it with parameter **-z 0**.
Examples:
* Sonique render: ![Sonique render](https://github.com/juanmv94/Soniqueviewer/raw/master/screenshots/miffosonique.png)
* Soniqueviewer with -z 1: ![Soniqueviewer with -z 1](https://github.com/juanmv94/Soniqueviewer/raw/master/screenshots/miffoz1.png)
* Soniqueviewer with -z 0: ![Soniqueviewer with -z 0](https://github.com/juanmv94/Soniqueviewer/raw/master/screenshots/miffoz0.png)

## Included efects
There's 3 of the most popular Sonique plugins included in this repository:

* **CoR's - Spec'n'Hopp 1.1**:
  * *Spec\_n\_Hopp.svp*: efect file
  * *Spec\_n\_Hopp.txt*: readme file
  * *Spec\_n\_Hopp.bmp*: char bitmaps

* **Psikorp'99 - Miffo v1.0**:
  * *Miffo.svp*: efect file
  * *Miffo.txt*: readme file

* **Mykel - Faited**:
  * *Faited.svp*: efect file
  * *Faited.txt*: readme file

*Spec'n'Hopp* is the only included efect with mouse support and without Sonique native zoom and blur.
