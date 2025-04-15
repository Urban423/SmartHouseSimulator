#pragma once
#include "../IOSystem/IOSystem.h"
#include "Vector2.h"

#define KeyCode_LeftMouseButton 	1
#define KeyCode_RightMouseButton 	2
#define KeyCode_MiddleMouseButton 	4

#define KeyCode_Enter 		13
#define KeyCode_Left_Shift 	16
#define KeyCode_Left_CTRL	17
#define KeyCode_Left_ALT 	18
#define KeyCode_Escape 		27
#define KeyCode_Space 		32

#define KeyCode_LeftArrow 	37
#define KeyCode_UpArrrow 	38
#define KeyCode_RightArrow 	39
#define KeyCode_DownArrow  	40
#define KeyCode_Delete  	46

#define KeyCode_Alpha0  48
#define KeyCode_Alpha1  49
#define KeyCode_Alpha2  50
#define KeyCode_Alpha3  51
#define KeyCode_Alpha4  52
#define KeyCode_Alpha5  53
#define KeyCode_Alpha6  54
#define KeyCode_Alpha7  55
#define KeyCode_Alpha8  56
#define KeyCode_Alpha9  57

#define KeyCode_A  65
#define KeyCode_B  66
#define KeyCode_C  67
#define KeyCode_D  68
#define KeyCode_E  69
#define KeyCode_F  70
#define KeyCode_G  71
#define KeyCode_H  72
#define KeyCode_I  73
#define KeyCode_J  74
#define KeyCode_K  75
#define KeyCode_L  76
#define KeyCode_M  77
#define KeyCode_N  78
#define KeyCode_O  79
#define KeyCode_P  80
#define KeyCode_Q  81
#define KeyCode_R  82
#define KeyCode_S  83
#define KeyCode_T  84
#define KeyCode_U  85
#define KeyCode_V  86
#define KeyCode_W  87
#define KeyCode_X  88
#define KeyCode_Y  89
#define KeyCode_Z  90


inline bool 	GetKey(unsigned char key) 		{ return IOSystem::getInputInstance().getInputState()[key] & 0x80; }
inline bool 	GetKeyDown(unsigned char key) 	{ return (IOSystem::getInputInstance().getInputState()[key] & 0x80) && (IOSystem::getInputInstance().getOldInputState()[key] & 0x80) != 0x80; }
inline bool 	GetKeyUp(unsigned char key) 	{ return (IOSystem::getInputInstance().getInputState()[key] & 0x80) != 0x80 && (IOSystem::getInputInstance().getOldInputState()[key] & 0x80); }
inline Vector2 	mousePositionDelta() 			{ return Vector2(IOSystem::getInputInstance().getPos()->x - IOSystem::getInputInstance().getOldPos()->x, IOSystem::getInputInstance().getPos()->y - IOSystem::getInputInstance().getOldPos()->y); }
inline void		setInputEventSystemI();

inline Vector2	getMouseScene() { 
	Rect winSize = IOSystem::getOutputInstance().getInnerSize();
	return Vector2(
		(2 * (IOSystem::getInputInstance().getPos()->x - winSize.left) / winSize.width())  - 1,
		(-2 *(IOSystem::getInputInstance().getPos()->y - winSize.top)  / winSize.height()) + 1);
};

inline void	setMousePosition() {};