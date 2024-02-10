/**
 Credit to SomePineapple
 Modified code from: https://github.com/SomePineaple/Phantom/blob/main/src/utils/XUtils.cpp
 */

#include "xutils.hpp"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>


int XUtils::mouseDeviceIndex = 0;
int XUtils::keyboardDeviceIndex = 0;
unsigned long XUtils::mouseDeviceID = 0;
unsigned long XUtils::keyboardDeviceID = 0;
bool XUtils::isDeviceShit = false;

XUtils::DeviceState XUtils::getDeviceState(Display *display, unsigned long deviceID) {
    DeviceState state{};

    XDevice *device = XOpenDevice(display, deviceID);
    XDeviceState *xState = XQueryDeviceState(display, device);

    XValuatorState *valState;
    XButtonState *buttonState;
    XKeyState *keyState;

    if (xState) {
        XInputClass *cls = xState->data;

        for (int i = 0; i < xState->num_classes; i++) {
            int i2;
            switch(cls->c_class) {
            case ValuatorClass:
                valState = (XValuatorState *) cls;
                state.numValuators = valState->num_valuators;
                state.valuatorStates = (int *) malloc(state.numValuators * sizeof(int));
                for (i2 = 0; i2 < valState->num_valuators; i2++)
                    state.valuatorStates[i2] = valState->valuators[i2];
                break;
            case ButtonClass:
                buttonState = (XButtonState *) cls;
                state.numButtons = buttonState->num_buttons;
                state.buttonStates = (bool *) malloc(state.numButtons * sizeof(bool));
                for (i2 = 1; i2 < buttonState->num_buttons; i2++)
                    state.buttonStates[i2] = (buttonState->buttons[i2 / 8] & (1 << (i2 % 8))) != 0;
                break;
            case KeyClass:
                keyState = (XKeyState *) cls;
                state.numKeys = keyState->num_keys;
                state.keyStates = (bool *) malloc(state.numKeys * sizeof(bool));
                for (i2 = 0; i2 < keyState->num_keys; i2++)
                    state.keyStates[i2] = (keyState->keys[i2 / 8] & (1 << (i2 % 8))) != 0;
                break;
            }
            cls = (XInputClass *) ((char *) cls + cls->length);
        }
        XFreeDeviceState(xState);
    }

    XCloseDevice(display, device);

    return state;
}

void XUtils::clickMouseXEvent(int button, long delayMS) {
    Display *dpy = XOpenDisplay(nullptr);
    XButtonEvent event;
    memset(&event, 0, sizeof(event));
    event.button = button;
    event.same_screen = true;
    event.subwindow = DefaultRootWindow(dpy);

    while (event.subwindow) {
        event.window = event.subwindow;
        XQueryPointer(dpy, event.window,
                      &event.root, &event.subwindow,
                      &event.x_root, &event.y_root,
                      &event.x, &event.y,
                      &event.state);
    }

    event.type = ButtonPress;
    XSendEvent(dpy, PointerWindow, True, ButtonPressMask, (XEvent *)&event);
    XFlush(dpy);
    usleep((useconds_t) delayMS * 1000);
    event.type = ButtonRelease;
    XSendEvent(dpy, PointerWindow, True, ButtonReleaseMask, (XEvent*)&event);
    XFlush(dpy);
    XCloseDisplay(dpy);
}

/*
void XUtils::renderMouseSelector() {
    Display *dpy = XOpenDisplay(nullptr);
    XDeviceInfo *devices;
    int numDevices;
    devices = XListInputDevices(dpy, &numDevices);

    if (isDeviceShit)
        ImGui::Text("Please select a valid mouse device");
    else
        ImGui::Text("Valid Mouse Selected");

    std::string comboItems;
    for (int i = 0; i < numDevices; i++) {
        comboItems.append(devices[i].name);
        comboItems.push_back('\0');
    }

    if (ImGui::Combo("Select your mouse", &mouseDeviceIndex, comboItems.c_str()))
        mouseDeviceID = devices[mouseDeviceIndex].id;

    ImGui::SameLine();
    ImGuiUtils::drawHelper("You must select the mouse you use to click with in minecraft. If you don't, autoclicker and aimassist won't work");

    XFreeDeviceList(devices);
    XCloseDisplay(dpy);
}

void XUtils::renderKeyboardSelector() {
    Display *dpy = XOpenDisplay(nullptr);
    XDeviceInfo  *devices;
    int numDevices;
    devices = XListInputDevices(dpy, &numDevices);

    std::string comboItems;
    for (int i = 0; i < numDevices; i++) {
        comboItems.append(devices[i].name);
        comboItems.push_back('\0');
    }

    if (ImGui::Combo("Select your keyboard", &keyboardDeviceIndex, comboItems.c_str()))
        keyboardDeviceID = devices[keyboardDeviceIndex].id;

    ImGui::SameLine();
    ImGuiUtils::drawHelper("You must select you keyboard, or else keybindings won't work");

    XFreeDeviceList(devices);
    XCloseDisplay(dpy);
}
 */

XUtils::DeviceState::DeviceState() {
    numKeys = 0;
    numButtons = 0;
    numValuators = 0;
    keyStates = nullptr;
    buttonStates = nullptr;
    valuatorStates = nullptr;
}

XUtils::DeviceState::~DeviceState() {
    if (keyStates != nullptr)
        free(keyStates);
    if (buttonStates != nullptr)
        free(buttonStates);
    if (valuatorStates != nullptr)
        free(valuatorStates);
}
