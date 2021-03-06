// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "display-device.h"

namespace i915 {

class BootloaderDisplay : public DisplayDevice {
public:
    BootloaderDisplay(Controller* controller, int32_t id, registers::Ddi ddi, registers::Pipe pipe);

private:
    bool QueryDevice(edid::Edid* edid, zx_display_info_t* info) final;
    bool DefaultModeset() final;

    edid::BaseEdid fake_base_edid_;
};

} // namespace i915
