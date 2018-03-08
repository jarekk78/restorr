#include "stdafx.h"
#include "WindowDataEntry.h"
#include <vector>
#include <string>
#include "Types.h"

WindowDataEntry::WindowDataEntry() {
}

void WindowDataEntry::setHwnd(HWND hwnd) {
	this->hwnd = hwnd;
}

void WindowDataEntry::setIsZoomed(bool isZoomed) {
	this->isZoomed = isZoomed;
}

void WindowDataEntry::setIsIconic(bool isIconic) {
	this->isIconic = isIconic;
}

void WindowDataEntry::setRect(RECT rect) {
	this->left = rect.left;
	this->top = rect.top;
	this->right = rect.right;
	this->bottom = rect.bottom;
}

HWND WindowDataEntry::getHwnd() {
	return this->hwnd;
}

bool WindowDataEntry::getIsZoomed() {
	return this->isZoomed;
}

bool WindowDataEntry::getIsIconic() {
	return this->isIconic;
}

LONG WindowDataEntry::getLeft() {
	return this->left;
}

LONG WindowDataEntry::getTop() {
	return this->top;
}

LONG WindowDataEntry::getRight() {
	return this->right;
}

LONG WindowDataEntry::getBottom() {
	return this->bottom;
}

bool WindowDataEntry::hasHwnd(HWND h) {
	return h == this->hwnd;
}