// Copyright 2018 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <fbl/string.h>
#include <fbl/vector.h>
#include <lib/async/cpp/wait.h>
#include <lib/fidl/cpp/message_buffer.h>
#include <lib/zx/channel.h>
#include <stdint.h>

namespace process {

class LauncherImpl {
public:
    using ErrorCallback = fbl::Function<void(zx_status_t)>;

    explicit LauncherImpl(zx::channel channel);
    ~LauncherImpl();

    zx_status_t Begin(async_t* async);

    void set_error_handler(ErrorCallback error_handler) {
        error_handler_ = fbl::move(error_handler);
    }

private:
    void OnHandleReady(async_t* async, async::WaitBase* wait, zx_status_t status,
                       const zx_packet_signal_t* signal);
    zx_status_t ReadAndDispatchMessage(fidl::MessageBuffer* buffer);

    zx_status_t Launch(fidl::MessageBuffer* buffer, fidl::Message message);
    zx_status_t AddArgs(fidl::Message message);
    zx_status_t AddEnvirons(fidl::Message message);
    zx_status_t AddNames(fidl::Message message);
    zx_status_t AddHandles(fidl::Message message);

    void NotifyError(zx_status_t error);
    void Reset();

    zx::channel channel_;
    async::WaitMethod<LauncherImpl, &LauncherImpl::OnHandleReady> wait_;
    ErrorCallback error_handler_;

    // Per-launch state.
    fbl::Vector<fbl::String> args_;
    fbl::Vector<fbl::String> environs_;
    fbl::Vector<fbl::String> nametable_;
    fbl::Vector<uint32_t> ids_;
    fbl::Vector<zx::handle> handles_;
};

} // namespace process
