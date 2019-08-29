#ifndef STEWART_CONTROL
#define STEWART_CONTROL

#include "lynxstructure.h"

using namespace LynxLib;

struct StewartControl
{
    StewartControl(LynxManager & lynx, char _structId) :
        lynxId(lynx.addStructure(_structId)),
        setpointX(lynx.addVariable(lynxId, eFloat)),
        setpointY(lynx.addVariable(lynxId, eFloat)),
        setpointZ(lynx.addVariable(lynxId, eFloat)),
        setpointRoll(lynx.addVariable(lynxId, eFloat)),
        setpointPitch(lynx.addVariable(lynxId, eFloat)),
        setpointYaw(lynx.addVariable(lynxId, eFloat))
	{}

    const LynxId lynxId;
    LynxVar_float setpointX;
    LynxVar_float setpointY;
    LynxVar_float setpointZ;
    LynxVar_float setpointRoll;
    LynxVar_float setpointPitch;
    LynxVar_float setpointYaw;

};
struct StewartFeedback
{
    StewartFeedback(LynxManager & lynx, char _structId) :
        lynxId(lynx.addStructure(_structId)),
        feedbackX(lynx.addVariable(lynxId, eFloat)),
        feedbackY(lynx.addVariable(lynxId, eFloat)),
        feedbackZ(lynx.addVariable(lynxId, eFloat)),
        feedbackRoll(lynx.addVariable(lynxId, eFloat)),
        feedbackPitch(lynx.addVariable(lynxId, eFloat)),
        feedbackYaw(lynx.addVariable(lynxId, eFloat))
    {}

    const LynxId lynxId;
    LynxVar_float feedbackX;
    LynxVar_float feedbackY;
    LynxVar_float feedbackZ;
    LynxVar_float feedbackRoll;
    LynxVar_float feedbackPitch;
    LynxVar_float feedbackYaw;

};

#endif // !STEWART_CONTROL
