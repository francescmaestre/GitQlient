#pragma once

namespace Graph
{

    enum class StateType;

    class State
    {
    public:
        State() = default;
        State(StateType type);

        bool operator==(const State& lane) const { return mType == lane.mType; }
        bool isHead() const;
        bool isTail() const;
        bool isJoin() const;
        bool isFreeLane() const;
        bool isMerge() const;
        bool isActive() const;
        StateType getType() const { return mType; }
        void setType(StateType type) { mType = type; }

    private:
        StateType mType;
    };
} // namespace Graph
