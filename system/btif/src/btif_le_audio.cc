/*
 * Copyright 2019 HIMSA II K/S - www.himsa.com. Represented by EHIMA -
 * www.ehima.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hardware/bluetooth.h>
#include <hardware/bt_le_audio.h>

#include <vector>

#include "btif_common.h"
#include "btif_storage.h"
#include "hardware/bt_le_audio.h"
#include "types/raw_address.h"

using base::Bind;
using base::Unretained;
using bluetooth::le_audio::ConnectionState;
using bluetooth::le_audio::GroupNodeStatus;
using bluetooth::le_audio::GroupStatus;
using bluetooth::le_audio::LeAudioClientCallbacks;
using bluetooth::le_audio::LeAudioClientInterface;

namespace {
class LeAudioClientInterfaceImpl;
std::unique_ptr<LeAudioClientInterface> leAudioInstance;

class LeAudioClientInterfaceImpl : public LeAudioClientInterface,
                                   public LeAudioClientCallbacks {
  ~LeAudioClientInterfaceImpl() = default;

  void OnConnectionState(ConnectionState state,
                         const RawAddress& address) override {
    do_in_jni_thread(FROM_HERE, Bind(&LeAudioClientCallbacks::OnConnectionState,
                                     Unretained(callbacks), state, address));
  }

  void OnGroupStatus(int group_id, GroupStatus group_status) override {
    do_in_jni_thread(FROM_HERE,
                     Bind(&LeAudioClientCallbacks::OnGroupStatus,
                          Unretained(callbacks), group_id, group_status));
  }

  void OnGroupNodeStatus(const RawAddress& addr, int group_id,
                         GroupNodeStatus node_status) override {
    do_in_jni_thread(FROM_HERE,
                     Bind(&LeAudioClientCallbacks::OnGroupNodeStatus,
                          Unretained(callbacks), addr, group_id, node_status));
  }

  void OnAudioConf(uint8_t direction, int group_id, uint32_t snk_audio_location,
                   uint32_t src_audio_location, uint16_t avail_cont) override {
    do_in_jni_thread(FROM_HERE,
                     Bind(&LeAudioClientCallbacks::OnAudioConf,
                          Unretained(callbacks), direction, group_id,
                          snk_audio_location, src_audio_location, avail_cont));
  }

  void Initialize(LeAudioClientCallbacks* callbacks) override {
    this->callbacks = callbacks;
  }

  void Cleanup(void) override {}

  void RemoveDevice(const RawAddress& address) override {}

  void Connect(const RawAddress& address) override {}

  void Disconnect(const RawAddress& address) override {}

  void GroupSetActive(const int group_id) override {}

 private:
  LeAudioClientCallbacks* callbacks;
};

} /* namespace */

LeAudioClientInterface* btif_le_audio_get_interface() {
  if (!leAudioInstance) {
    leAudioInstance.reset(new LeAudioClientInterfaceImpl());
  }

  return leAudioInstance.get();
}
