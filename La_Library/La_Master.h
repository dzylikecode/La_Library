#pragma once
#include "La_Audio.h"
#include "La_Graphic.h"
#include "La_Input.h"
#include "La_NetWork.h"
class AUDIOMaster
{
public:
	~AUDIOMaster() { AUDIO::CloseAudio(); }
}audioMaster;


//全局变量代理者
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

class SOCKETMaster
{
public:
	SOCKETMaster() { LADZY::InitializeNetWork(); }
	~SOCKETMaster() { LADZY::CloseNetWork(); }
}socketMaster;