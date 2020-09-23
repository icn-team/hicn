/*
 * Copyright (c) 2017-2019 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HICNLIGHT_CS_LRU_H
#define HICNLIGHT_CS_LRU_H

typedef struct {
    off_t prev;
    off_t next;
} cs_entry_lru_state_t;

typedef struct {
    off_t head;
    off_t tail;
} cs_lru_state_t;

typedef struct {
    uint64_t countExpiryEvictions;
    uint64_t countRCTEvictions;
    uint64_t countLruEvictions;
    uint64_t countAdds;
    uint64_t countHits;
    uint64_t countMisses;
} cs_lru_stats_t;

#endif /* HICNLIGHT_CS_LRU_H */
