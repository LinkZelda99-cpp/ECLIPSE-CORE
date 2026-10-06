#pragma once
#include "Config.h"
void updateEncoder(); void updateEncoderButton(); void updateBackButton(); bool consumeBackPress(); int consumeEncoderDelta(); bool consumeEncoderPress(); void clearEncoderEvents();
