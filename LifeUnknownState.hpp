#pragma once

#include "LifeAPI.h"
#include "Bits.hpp"
#include "LifeStableState.hpp"

class LifeUnknownState {
public:
  LifeState state;
  LifeState unknown;
  LifeState unknownStable;
  LifeState glanceableUnknown;

  void UncertainStepInto(LifeState &__restrict__ next,
                         LifeState &__restrict__ nextUnknown) const;
  void UncertainStepSelf();
  LifeUnknownState UncertainStepMaintaining(const LifeStableState &stable) const;
  LifeState ActiveComparedTo(const LifeStableState &stable) const;
  bool CompatibleWith(const LifeStableState &stable) const;

  void UncertainStepColumn(int column, uint64_t &next, uint64_t &nextUnknown) const;
  std::pair<bool, bool> NextForCell(std::pair<int, int> cell) const;
  bool KnownNext(std::pair<int, int> cell) const;

  bool StillGlancingFor(std::pair<int, int> cell, const LifeStableState &stable) const;
};

void LifeUnknownState::UncertainStepInto(LifeState &__restrict__ next, LifeState &__restrict__ nextUnknown) const {
  LifeState oncol0(false), oncol1(false), unkcol0(false), unkcol1(false);
  CountRows(state, oncol0, oncol1);
  CountRows(unknown, unkcol0, unkcol1);

  #pragma clang loop unroll(full)
  for (int i = 0; i < N; i++) {
    int idxU;
    int idxB;
    if (i == 0)
      idxU = N - 1;
    else
      idxU = i - 1;

    if (i == N - 1)
      idxB = 0;
    else
      idxB = i + 1;

    uint64_t on3, on2, on1, on0;
    uint64_t unk3, unk2, unk1, unk0;

    {
      uint64_t u_on1 = oncol1.state[idxU];
      uint64_t u_on0 = oncol0.state[idxU];
      uint64_t c_on1 = oncol1.state[i];
      uint64_t c_on0 = oncol0.state[i];
      uint64_t l_on1 = oncol1.state[idxB];
      uint64_t l_on0 = oncol0.state[idxB];

      uint64_t uc0, uc1, uc2, uc_carry0;
      HalfAdd(uc0, uc_carry0, u_on0, c_on0);
      FullAdd(uc1, uc2, u_on1, c_on1, uc_carry0);

      uint64_t on_carry1, on_carry0;
      HalfAdd(on0, on_carry0, uc0, l_on0);
      FullAdd(on1, on_carry1, uc1, l_on1, on_carry0);
      HalfAdd(on2, on3, uc2, on_carry1);
      on2 |= on3;
      on1 |= on3;
      on0 |= on3;

      uint64_t u_unk1 = unkcol1.state[idxU];
      uint64_t u_unk0 = unkcol0.state[idxU];
      uint64_t c_unk1 = unkcol1.state[i];
      uint64_t c_unk0 = unkcol0.state[i];
      uint64_t l_unk1 = unkcol1.state[idxB];
      uint64_t l_unk0 = unkcol0.state[idxB];

      uint64_t ucunk0, ucunk1, ucunk2, ucunk_carry0;
      HalfAdd(ucunk0, ucunk_carry0, u_unk0, c_unk0);
      FullAdd(ucunk1, ucunk2, u_unk1, c_unk1, ucunk_carry0);

      uint64_t unk_carry1, unk_carry0;
      HalfAdd(unk0, unk_carry0, ucunk0, l_unk0);
      FullAdd(unk1, unk_carry1, ucunk1, l_unk1, unk_carry0);
      HalfAdd(unk2, unk3, ucunk2, unk_carry1);
      unk1 |= unk2 | unk3;
      unk0 |= unk2 | unk3;
    }

    uint64_t stateon = state.state[i];
    uint64_t stateunk = unknown.state[i];

    uint64_t next_on = 0;
    uint64_t unknown = 0;

    // ALWAYS CHECK THE PHASE that espresso outputs or you will get confused
// Begin Autogenerated
unknown |= (~on2) & unk1 & (on1 | on0 | unk0);
unknown |= stateon & (~on1) & (~on0) & (unk1 | unk0) ;
unknown |= (~stateunk) & (~stateon) & (~on2) & on1 & unk0 ;
unknown |= (~on2) & on1 & (~on0) & unk0 ;
next_on |= (stateunk | stateon | ~unk0) & (~on2) & on1 & on0 & (~unk1) ;
next_on |= stateon & (~on1) & (~on0) & (~unk1) & (~unk0) ;
// End Autogenerated

    next.state[i] = next_on;
    nextUnknown.state[i] = unknown;
  }
}

void LifeUnknownState::UncertainStepSelf() {
  LifeState next;
  LifeState nextUnknown;
  UncertainStepInto(next, nextUnknown);
  state = next;
  unknown = nextUnknown;
}

LifeUnknownState LifeUnknownState::UncertainStepMaintaining(const LifeStableState &stable) const {
  LifeUnknownState result;

  LifeState state3(false), state2(false), state1(false), state0(false);
  LifeState unknown3(false), unknown2(false), unknown1(false), unknown0(false);
  LifeState unknownStable3(false), unknownStable2(false), unknownStable1(false), unknownStable0(false);

  CountNeighbourhood(state, state3, state2, state1, state0);
  CountNeighbourhood(unknown, unknown3, unknown2, unknown1, unknown0);
  CountNeighbourhood(unknownStable, unknownStable3, unknownStable2, unknownStable1, unknownStable0);

  #pragma clang loop unroll(full)
  for (int i = 0; i < N; i++) {
    uint64_t on3 = state3.state[i];
    uint64_t on2 = state2.state[i];
    uint64_t on1 = state1.state[i];
    uint64_t on0 = state0.state[i];

    uint64_t unk3 = unknown3.state[i];
    uint64_t unk2 = unknown2.state[i];
    uint64_t unk1 = unknown1.state[i];
    uint64_t unk0 = unknown0.state[i];

    on2 |= on3;
    on1 |= on3;
    on0 |= on3;

    unk1 |= unk2 | unk3;
    unk0 |= unk2 | unk3;

    uint64_t stateon = state.state[i];
    uint64_t stateunk = unknown.state[i];

    uint64_t next_on = 0;
    uint64_t unknown = 0;

    // ALWAYS CHECK THE PHASE that espresso outputs or you will get confused
    // Begin Autogenerated
    unknown |= (~on2) & unk1 & (on1 | on0 | unk0);
    unknown |= stateon & (~on1) & (~on0) & (unk1 | unk0);
    unknown |= (~stateunk) & (~stateon) & (~on2) & on1 & unk0;
    unknown |= (~on2) & on1 & (~on0) & unk0;
    next_on |= (stateunk | stateon | ~unk0) & (~on2) & on1 & on0 & (~unk1);
    next_on |= stateon & (~on1) & (~on0) & (~unk1) & (~unk0);
    // End Autogenerated

    uint64_t any_unstable_unknown =
      (unknownStable3.state[i] ^ unknown3.state[i]) | (unknownStable2.state[i] ^ unknown2.state[i]) |
      (unknownStable1.state[i] ^ unknown1.state[i]) | (unknownStable0.state[i] ^ unknown0.state[i]);

    uint64_t unequal_stable =
        (state.state[i] ^ stable.state.state[i]) | (unknownStable.state[i] ^ stable.unknownStable.state[i]) |
         state3.state[i]                           | (state2.state[i] ^ stable.state2.state[i]) |
        (state1.state[i] ^ stable.state1.state[i]) | (state0.state[i] ^ stable.state0.state[i]) |
        (unknownStable3.state[i] ^ stable.unknown3.state[i]) | (unknownStable2.state[i] ^ stable.unknown2.state[i]) |
        (unknownStable1.state[i] ^ stable.unknown1.state[i]) | (unknownStable0.state[i] ^ stable.unknown0.state[i]) |
        // If any of the unknown are not stable
        // unknown, we can't use this trick.
        any_unstable_unknown;

    uint64_t equal_stable = ~unequal_stable;

    result.state.state[i] = next_on;
    result.unknown.state[i] = unknown;

    // Prevent unknown region from expanding
    result.state.state[i] = (result.state.state[i] & unequal_stable) | (stable.state.state[i] & equal_stable);
    result.unknown.state[i] = (result.unknown.state[i] & unequal_stable) | (stable.unknownStable.state[i] & equal_stable);
    result.unknownStable.state[i] = stable.unknownStable.state[i] & equal_stable;

    uint64_t glanceable = (~stateon) & (~stateunk)
      & unknown
      & (~on2) & (~on1) & on0
      & (unk2 | unk1 | unk0)
      & ~(stable.state2.state[i] | stable.state1.state[i] | stable.state0.state[i])
      & ~any_unstable_unknown;
    result.glanceableUnknown.state[i] = glanceable;

    // Remove unknown cells that we have decided were glancing
    uint64_t glanceSafe = (~stateon) & (~stateunk)
      & unknown
      & (~stable.state2.state[i] & ~stable.state1.state[i] & (~on2)) &
        (  (~stable.state0.state[i]  & (~on1) &   on0)
         | ( stable.state0.state[i]  & (~on1) &   on0)
         | ( stable.state0.state[i]  &   on1  & (~on0))
        )
      & ~any_unstable_unknown;
    result.unknown.state[i] &= ~(glanceSafe & stable.glanced.state[i]);
  }

  return result;
}

void LifeUnknownState::UncertainStepColumn(int column, uint64_t &next, uint64_t &nextUnknown) const {
  std::array<uint64_t, 3> nearbyState;
  std::array<uint64_t, 3> nearbyUnknown;

  for (int i = 0; i < 3; i++) {
    int c = (column + i - 1 + N) % N;
    nearbyState[i] = state.state[c];
    nearbyUnknown[i] = unknown.state[c];
  }

  std::array<uint64_t, 3> oncol0;
  std::array<uint64_t, 3> oncol1;
  std::array<uint64_t, 3> unkcol0;
  std::array<uint64_t, 3> unkcol1;

  for (int i = 0; i < 3; i++) {
    uint64_t a = nearbyState[i];
    uint64_t l = RotateLeft(a);
    uint64_t r = RotateRight(a);

    oncol0[i] = l ^ r ^ a;
    oncol1[i] = ((l ^ r) & a) | (l & r);
  }

  for (int i = 0; i < 3; i++) {
    uint64_t a = nearbyUnknown[i];
    uint64_t l = RotateLeft(a);
    uint64_t r = RotateRight(a);

    unkcol0[i] = l ^ r ^ a;
    unkcol1[i] = ((l ^ r) & a) | (l & r);
  }

  {
    int idxU = 0;
    int i = 1;
    int idxB = 2;

    uint64_t on3, on2, on1, on0;
    uint64_t unk3, unk2, unk1, unk0;

    {
      uint64_t u_on1 = oncol1[idxU];
      uint64_t u_on0 = oncol0[idxU];
      uint64_t c_on1 = oncol1[i];
      uint64_t c_on0 = oncol0[i];
      uint64_t l_on1 = oncol1[idxB];
      uint64_t l_on0 = oncol0[idxB];

      uint64_t uc0, uc1, uc2, uc_carry0;
      HalfAdd(uc0, uc_carry0, u_on0, c_on0);
      FullAdd(uc1, uc2, u_on1, c_on1, uc_carry0);

      uint64_t on_carry1, on_carry0;
      HalfAdd(on0, on_carry0, uc0, l_on0);
      FullAdd(on1, on_carry1, uc1, l_on1, on_carry0);
      HalfAdd(on2, on3, uc2, on_carry1);
      on2 |= on3;
      on1 |= on3;
      on0 |= on3;

      uint64_t u_unk1 = unkcol1[idxU];
      uint64_t u_unk0 = unkcol0[idxU];
      uint64_t c_unk1 = unkcol1[i];
      uint64_t c_unk0 = unkcol0[i];
      uint64_t l_unk1 = unkcol1[idxB];
      uint64_t l_unk0 = unkcol0[idxB];

      uint64_t ucunk0, ucunk1, ucunk2, ucunk_carry0;
      HalfAdd(ucunk0, ucunk_carry0, u_unk0, c_unk0);
      FullAdd(ucunk1, ucunk2, u_unk1, c_unk1, ucunk_carry0);

      uint64_t unk_carry1, unk_carry0;
      HalfAdd(unk0, unk_carry0, ucunk0, l_unk0);
      FullAdd(unk1, unk_carry1, ucunk1, l_unk1, unk_carry0);
      HalfAdd(unk2, unk3, ucunk2, unk_carry1);
      unk1 |= unk2 | unk3;
      unk0 |= unk2 | unk3;
    }

    uint64_t stateon = nearbyState[i];
    uint64_t stateunk = nearbyUnknown[i];

    uint64_t next_on = 0;
    uint64_t unknown = 0;

    // ALWAYS CHECK THE PHASE that espresso outputs or you will get confused
// Begin Autogenerated
unknown |= (~on2) & unk1 & (on1 | on0 | unk0);
unknown |= stateon & (~on1) & (~on0) & (unk1 | unk0) ;
unknown |= (~stateunk) & (~stateon) & (~on2) & on1 & unk0 ;
unknown |= (~on2) & on1 & (~on0) & unk0 ;
next_on |= (stateunk | stateon | ~unk0) & (~on2) & on1 & on0 & (~unk1) ;
next_on |= stateon & (~on1) & (~on0) & (~unk1) & (~unk0) ;
// End Autogenerated

    next = next_on;
    nextUnknown = unknown;
  }
}

std::pair<bool, bool> LifeUnknownState::NextForCell(std::pair<int, int> cell) const {
  uint64_t nextColumn;
  uint64_t nextUnknownColumn;

  UncertainStepColumn(cell.first, nextColumn, nextUnknownColumn);

  int y = cell.second;
  bool cellNext    = (nextColumn & (1ULL << y)) >> y;
  bool cellUnknown = (nextUnknownColumn & (1ULL << y)) >> y;
  return {cellNext, cellUnknown};
}

bool LifeUnknownState::KnownNext(std::pair<int, int> cell) const {
  return !NextForCell(cell).second;
}


LifeState LifeUnknownState::ActiveComparedTo(const LifeStableState &stable) const {
  return ~unknown & ~stable.unknownStable & stable.stateZOI & (stable.state ^ state);
}

bool LifeUnknownState::CompatibleWith(const LifeStableState &stable) const {
  return ActiveComparedTo(stable).IsEmpty();
}

bool LifeUnknownState::StillGlancingFor(std::pair<int, int> cell, const LifeStableState &stable) const {
  return !stable.state2.Get(cell) && !stable.state1.Get(cell) &&
    (stable.unknown3.Get(cell) || stable.unknown2.Get(cell) || stable.unknown1.Get(cell) || stable.unknown0.Get(cell));
}
