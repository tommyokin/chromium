// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/p2p/host_address_request.h"

#include "base/bind.h"
#include "base/message_loop_proxy.h"
#include "content/common/p2p_messages.h"
#include "content/renderer/p2p/socket_dispatcher.h"

P2PHostAddressRequest::P2PHostAddressRequest(P2PSocketDispatcher* dispatcher)
    : dispatcher_(dispatcher),
      ipc_message_loop_(dispatcher->message_loop()),
      delegate_message_loop_(base::MessageLoopProxy::current()),
      state_(STATE_CREATED),
      request_id_(0),
      registered_(false) {
}

P2PHostAddressRequest::~P2PHostAddressRequest() {
  DCHECK(state_ == STATE_CREATED || state_ == STATE_FINISHED);
}

void P2PHostAddressRequest::Request(const std::string& host_name,
                                    const DoneCallback& done_callback) {
  DCHECK(delegate_message_loop_->BelongsToCurrentThread());

  state_ = STATE_SENT;
  ipc_message_loop_->PostTask(FROM_HERE, base::Bind(
      &P2PHostAddressRequest::DoSendRequest, this, host_name, done_callback));
}

void P2PHostAddressRequest::Cancel() {
  DCHECK(delegate_message_loop_->BelongsToCurrentThread());

  if (state_ != STATE_FINISHED) {
    state_ = STATE_FINISHED;
    ipc_message_loop_->PostTask(FROM_HERE, base::Bind(
        &P2PHostAddressRequest::DoUnregister, this));
  }
}

void P2PHostAddressRequest::DoSendRequest(const std::string& host_name,
                                          const DoneCallback& done_callback) {
  DCHECK(ipc_message_loop_->BelongsToCurrentThread());

  done_callback_ = done_callback;
  request_id_ = dispatcher_->RegisterHostAddressRequest(this);
  registered_ = true;
  dispatcher_->SendP2PMessage(
      new P2PHostMsg_GetHostAddress(0, host_name, request_id_));
}

void P2PHostAddressRequest::DoUnregister() {
  DCHECK(ipc_message_loop_->BelongsToCurrentThread());
  if (registered_) {
    dispatcher_->UnregisterHostAddressRequest(request_id_);
    registered_ = false;
  }
}

void P2PHostAddressRequest::OnResponse(const net::IPAddressNumber& address) {
  DCHECK(ipc_message_loop_->BelongsToCurrentThread());
  delegate_message_loop_->PostTask(FROM_HERE, base::Bind(
      &P2PHostAddressRequest::DeliverResponse, this, address));
  dispatcher_->UnregisterHostAddressRequest(request_id_);
  registered_ = false;
}

void P2PHostAddressRequest::DeliverResponse(
    const net::IPAddressNumber& address) {
  DCHECK(delegate_message_loop_->BelongsToCurrentThread());
  if (state_ == STATE_SENT) {
    done_callback_.Run(address);
    state_ = STATE_FINISHED;
  }
}
