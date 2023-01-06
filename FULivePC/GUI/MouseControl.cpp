#include "MouseControl.h"

MouseControl::MouseControl(FURect rect)
{
	viewRect = rect;
}
void MouseControl::ReSet(FURect rect){
	viewRect = rect;
}
bool MouseControl::isMousedown(){
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
		if (io.MouseDownDuration[i] >= 0.0f)
			return true;
	}
	return false;
}

bool MouseControl::isMouseClicked(){
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
		if (ImGui::IsMouseClicked(i))
			return true;
	}
	return false;
}

bool MouseControl::isMouseDoubleClicked(){
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
		if (ImGui::IsMouseDoubleClicked(i))
			return true;
	}
	return false;
}

bool MouseControl::isMouseReleased(){
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
		if (ImGui::IsMouseReleased(i))
			return true;
	}
	return false;
}


/// 检测当前负责的区域有没有被选中，如果被选中，返回第一次被点击的位置
/// @param pos 返回被点击的位置
bool MouseControl::isSelected(ImVec2* pos){
	if (isMouseClicked()) {
		ImVec2 curPos = io.MousePos;
		float curMouseX = curPos.x;
		float curMouseY= curPos.y;
		
		float minX = viewRect.origin.x;
		float maxX = viewRect.origin.x + viewRect.size.width;
		float minY = viewRect.origin.y;
		float maxY = viewRect.origin.y + viewRect.size.height;
		if (minX<=curMouseX&&curMouseX<=maxX
			&&minY<=curMouseY&&curMouseY<=maxY) {
			*pos = curPos;
			return true;
		}
	}
	return false;
}
static FURect tmpRect;
/// 一直检测当前负责的区域有没有被拖动，返回如果为true，代表被拖动，并通过参数返回偏移量
/// @param dealt 返回的偏移量
/// @return 当前负责的区域有没有被拖动
bool MouseControl::draging(ImVec2* dealt){
	ImVec2 selectedBeginPos;
	if (isSelected(&selectedBeginPos)) {
		beginPos = selectedBeginPos;
		mouseControlState = MouseControlStateSelected;
		
	}else if (isMousedown())
	{
		if (mouseControlState == MouseControlStateSelected || mouseControlState == MouseControlStateDraging) {
			mouseControlState = MouseControlStateDraging;
		}
		if (mouseControlState == MouseControlStateDraging) {
			ImVec2 curPos = io.MousePos;
			ImVec2 dealtVec2 = {curPos.x-beginPos.x,curPos.y-beginPos.y};
			*dealt = dealtVec2;
			return true;
		}
	}else if (isMouseReleased())
	{
		if (mouseControlState == MouseControlStateDraging) {
			// 更新 beginPos
			ImVec2 curPos = io.MousePos;
			ImVec2 dealtVec2 = {curPos.x-beginPos.x,curPos.y-beginPos.y};
			viewRect = tmpRect;
			mouseControlState = MouseControlStateComplete;
		}
	}
	return false;
}
bool MouseControl::dragComplete(ImVec2* dealt){
	
	if (mouseControlState == MouseControlStateComplete) {
		mouseControlState = MouseControlStateOrignal;
		return true;
	}
	return false;
}

/// 设置完成新的位置后的回调
/// @param rect  新的位置
void MouseControl::callback(FURect rect){
	tmpRect = rect;
}
void MouseControl::changeRect(FURect rect){
	viewRect = rect;
}
MouseControl::~MouseControl()
{
	
}

float MouseControl::isMouseWheel()
{
	return io.MouseWheel;
}

void MouseControl::ReSetOrigin()
{
	mouseControlState = MouseControlStateOrignal;
}
