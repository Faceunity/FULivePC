#ifndef _MOUSE_CONTROL_H_
#define _MOUSE_CONTROL_H_
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
/* Points. */
struct
FUPoint {
    float x;
    float y;
};

/* Sizes. */

struct FUSize {
    float width;
    float height;
};
struct FURect {
    FUPoint origin;
    FUSize size;
};
typedef enum {
//	MouseControlStateUnknown,
//	MouseControlStateDown = 0,  
//	MouseControlStateClicked,  
//	MouseControlStateDoubleClicked,  
//	MouseControlStateReleased, 
    MouseControlStateOrignal,
	MouseControlStateSelected, 
	MouseControlStateDraging, 
	MouseControlStateComplete
}MouseControlState;
class MouseControl{
public:
	FURect viewRect;
	MouseControl(FURect rect);
	void ReSet(FURect rect);
	~MouseControl();
	bool draging(ImVec2* dealt);
	bool dragComplete(ImVec2* dealt);
	void callback(FURect rect);
	void changeRect(FURect rect);
	bool isSelected(ImVec2* pos);
private:
	ImGuiIO& io=ImGui::GetIO();
	
	ImVec2 beginPos;
	MouseControlState mouseControlState;
	bool isMousedown();
	bool isMouseClicked();
	bool isMouseDoubleClicked();
	bool isMouseReleased();
	
};
#endif
