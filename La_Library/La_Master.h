#pragma once
#include "La_Audio.h"
#include "La_Graphic.h"
#include "La_Input.h"
class AUDIOMaster
{
public:
	~AUDIOMaster() { AUDIO::CloseAudio(); }
}audioMaster;


//ȫ�ֱ���������
class GRAPHICMaster
{
public:
	~GRAPHICMaster() { GRAPHIC::CloseGraphic(); }
}graphicMaster;

class INPUTMaster
{
public:
	~INPUTMaster() { INPUT_::CloseInput(); }
}inputMaster;